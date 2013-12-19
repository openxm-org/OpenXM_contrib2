/*
 * Copyright (c) 1994-2000 FUJITSU LABORATORIES LIMITED 
 * All rights reserved.
 * 
 * FUJITSU LABORATORIES LIMITED ("FLL") hereby grants you a limited,
 * non-exclusive and royalty-free license to use, copy, modify and
 * redistribute, solely for non-commercial and non-profit purposes, the
 * computer program, "Risa/Asir" ("SOFTWARE"), subject to the terms and
 * conditions of this Agreement. For the avoidance of doubt, you acquire
 * only a limited right to use the SOFTWARE hereunder, and FLL or any
 * third party developer retains all rights, including but not limited to
 * copyrights, in and to the SOFTWARE.
 * 
 * (1) FLL does not grant you a license in any way for commercial
 * purposes. You may use the SOFTWARE only for non-commercial and
 * non-profit purposes only, such as academic, research and internal
 * business use.
 * (2) The SOFTWARE is protected by the Copyright Law of Japan and
 * international copyright treaties. If you make copies of the SOFTWARE,
 * with or without modification, as permitted hereunder, you shall affix
 * to all such copies of the SOFTWARE the above copyright notice.
 * (3) An explicit reference to this SOFTWARE and its copyright owner
 * shall be made on your publication or presentation in any form of the
 * results obtained by use of the SOFTWARE.
 * (4) In the event that you modify the SOFTWARE, you shall notify FLL by
 * e-mail at risa-admin@sec.flab.fujitsu.co.jp of the detailed specification
 * for such modification or the source code of the modified part of the
 * SOFTWARE.
 * 
 * THE SOFTWARE IS PROVIDED AS IS WITHOUT ANY WARRANTY OF ANY KIND. FLL
 * MAKES ABSOLUTELY NO WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY, AND
 * EXPRESSLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT OF THIRD PARTIES'
 * RIGHTS. NO FLL DEALER, AGENT, EMPLOYEES IS AUTHORIZED TO MAKE ANY
 * MODIFICATIONS, EXTENSIONS, OR ADDITIONS TO THIS WARRANTY.
 * UNDER NO CIRCUMSTANCES AND UNDER NO LEGAL THEORY, TORT, CONTRACT,
 * OR OTHERWISE, SHALL FLL BE LIABLE TO YOU OR ANY OTHER PERSON FOR ANY
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, PUNITIVE OR CONSEQUENTIAL
 * DAMAGES OF ANY CHARACTER, INCLUDING, WITHOUT LIMITATION, DAMAGES
 * ARISING OUT OF OR RELATING TO THE SOFTWARE OR THIS AGREEMENT, DAMAGES
 * FOR LOSS OF GOODWILL, WORK STOPPAGE, OR LOSS OF DATA, OR FOR ANY
 * DAMAGES, EVEN IF FLL SHALL HAVE BEEN INFORMED OF THE POSSIBILITY OF
 * SUCH DAMAGES, OR FOR ANY CLAIM BY ANY OTHER PARTY. EVEN IF A PART
 * OF THE SOFTWARE HAS BEEN DEVELOPED BY A THIRD PARTY, THE THIRD PARTY
 * DEVELOPER SHALL HAVE NO LIABILITY IN CONNECTION WITH THE USE,
 * PERFORMANCE OR NON-PERFORMANCE OF THE SOFTWARE.
 *
 * $OpenXM: OpenXM_contrib2/asir2000/plot/ox_plot.c,v 1.22 2011/08/10 04:51:58 saito Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "ox.h"
#include "ifplot.h"
#include "version.h"
#include <signal.h>
#if defined(PARI)
#include "genpari.h"
#endif

void ox_usr1_handler();

extern int asir_OperandStackSize;
extern Obj *asir_OperandStack;
extern int asir_OperandStackPtr;

// environement is defined in libpari.a
#if !(PARI_VERSION_CODE > 131588)
extern jmp_buf environnement;
#endif

extern int do_message;
extern int ox_flushing;
extern JMP_BUF ox_env;
extern MATHCAP my_mathcap;
extern char LastError[];

void create_error(ERR *,unsigned int ,char *,LIST );

void ox_io_init();
void ox_asir_init(int,char **,char *);
Obj asir_pop_one();
void asir_push_one(Obj);
void asir_end_flush();
int asir_executeString();
void asir_evalName(unsigned int);
void asir_setName(unsigned int);
void asir_pops();
void asir_popString();
void asir_popCMO(unsigned int);
void asir_popSerializedLocalObject();
char *name_of_cmd(unsigned int);
char *name_of_id(int);
LIST asir_GetErrorList();

static void asir_do_cmd(unsigned int,unsigned int);
static void process_ox();
static void asir_executeFunction();

#if defined(VISUAL)
void ox_plot_main()
#else
void ox_plot_main(int argc,char **argv)
#endif
{
	int ds;
	fd_set r;
	int n;
	int use_x;

#if !defined(VISUAL)
	ox_asir_init(argc,argv,"ox_plot");
	use_x=init_plot_display(argc,argv);
	if(use_x) ds=ConnectionNumber(display);
	else fprintf(stderr,"Entering no X mode\n");
#endif
	if(do_message)fprintf(stderr,"I'm an ox_plot, Version %d.\n",ASIR_VERSION);

	if(SETJMP(ox_env)){
		while(NEXT(asir_infile))closecurrentinput();
		reset_current_computation();
		ox_send_sync(0);
	}
	while (1){
#if defined(VISUAL)
		process_ox();
#else
		if(ox_data_is_available(0)) process_ox();
		else if(use_x){
			FD_ZERO(&r);
			FD_SET(3,&r); FD_SET(ds,&r);
			select(FD_SETSIZE,&r,NULL,NULL,NULL);
			if(FD_ISSET(3,&r)) process_ox();
			else if(FD_ISSET(ds,&r)) process_xevent();
		}
	}
#endif
}

static void process_ox(){
	int id;
	unsigned int cmd;
	Obj obj;
	ERR err;
	unsigned int serial;
	int ret;

	serial=ox_recv(0,&id,&obj);
	if(do_message) fprintf(stderr,"#%d Got %s",serial,name_of_id(id));
	switch (id){
		case OX_COMMAND:
			cmd=((USINT)obj)->body;
			if(ox_flushing) break;
			if(do_message) fprintf(stderr," %s\n",name_of_cmd(cmd));
			if(ret=SETJMP(main_env)){
				if(ret==1){
					create_error(&err,serial,LastError,0);
					asir_push_one((Obj)err);
				}
				break;
			}
			asir_do_cmd(cmd,serial);
			break;
		case OX_DATA:
		case OX_LOCAL_OBJECT_ASIR:
			if(ox_flushing)break;
			if(do_message)fprintf(stderr," -> data pushed");
			asir_push_one(obj);
			break;
		case OX_SYNC_BALL:
			asir_end_flush();
			break;
		default:
			break;
	}
	if(do_message)fprintf(stderr,"\n");
}

static void asir_do_cmd(unsigned int cmd,unsigned int serial){
	MATHCAP client_mathcap;
	LIST list;
	int i;
	Q q;

	switch (cmd){
		case SM_dupErrors:
			list=asir_GetErrorList();
			asir_push_one((Obj)list);
			break;
		case SM_getsp:
			i=asir_OperandStackPtr+1;
			STOQ(i,q);
			asir_push_one((Obj)q);
			break;
		case SM_popSerializedLocalObject:
			asir_popSerializedLocalObject();
			break;
		case SM_popCMO:
			asir_popCMO(serial);
			break;
		case SM_popString:
			asir_popString();
			break;
		case SM_setName:
			asir_setName(serial);
			break;
		case SM_evalName:
			asir_evalName(serial);
			break;
		case SM_executeStringByLocalParser:
			asir_executeString();
			break;
		case SM_executeFunction:
			asir_executeFunction(serial);
			break;
		case SM_shutdown:
			asir_terminate(2);
			break;
		case SM_pops:
			asir_pops();
			break;
		case SM_mathcap:
			asir_push_one((Obj)my_mathcap);
			break;
		case SM_setMathcap:
			client_mathcap=(MATHCAP)asir_pop_one();
			store_remote_mathcap(0,client_mathcap);
			break;
		case SM_nop:
		default:
			break;
	}
}

static void asir_executeFunction(int serial){ 
	char *fn;
	int argc,id;
	FUNC f;
	Q ret;
	VL vl;
	ERR err;
	NODE n,n1; 
	LIST bytes;

	fn=((STRING)asir_pop_one())->body;
	argc=(int)(((USINT)asir_pop_one())->body);
	for(n=0;argc;argc--){
		NEXTNODE(n,n1);
		BDY(n1)=(pointer)asir_pop_one();
	}
	if(n)NEXT(n1)=0;
	id=-1;
	if(!strcmp(fn,"plot")){
		id=plot(n,MODE_PLOT);
		STOQ(id,ret);
		asir_push_one((Obj)ret);
	} else if(!strcmp(fn,"ifplot")){
		id=plot(n,MODE_IFPLOT);
		STOQ(id,ret);
		asir_push_one((Obj)ret);
	} else if(!strcmp(fn,"conplot")){
		id=plot(n,MODE_CONPLOT);
		STOQ(id,ret);
		asir_push_one((Obj)ret);
	} else if(!strcmp(fn,"polarplot")){
		id=plot(n,MODE_POLARPLOT);
		STOQ(id,ret);
		asir_push_one((Obj)ret);
	} else if(!strcmp(fn,"memory_plot")){
		memory_plot(n,&bytes);
		asir_push_one((Obj)bytes);
	} else if(!strcmp(fn,"arrayplot")){
		id=arrayplot(n);
		STOQ(id,ret);
		asir_push_one((Obj)ret);
	} else if(!strcmp(fn,"open_canvas")){
		id=open_canvas(n);
		STOQ(id,ret);
		asir_push_one((Obj)ret);
	} else if(!strcmp(fn,"plotover")){
		plotover(n);
	} else if(!strcmp(fn,"drawcircle")){
		drawcircle(n);
	} else if(!strcmp(fn,"draw_obj")){
		if(draw_obj(n) < 0 ){
			create_error(&err,serial,LastError,0);
			asir_push_one((Obj)err);
		}	
	} else if(!strcmp(fn,"draw_string")){
		if(draw_string(n) < 0 ){
			create_error(&err,serial,LastError,0);
			asir_push_one((Obj)err);
		}
	} else if(!strcmp(fn,"clear_canvas")){
		clear_canvas(n);
// ifplotNG
	} else if(!strcmp(fn,"objcp")){
		id=objcp(n);
		STOQ(id,ret);
		asir_push_one((Obj)ret);
	} else if(!(
		strcmp(fn,"ifplotD")&strcmp(fn,"ifplotQ")&strcmp(fn,"ifplotB")&
		strcmp(fn,"ineqnD")&strcmp(fn,"ineqnQ")&strcmp(fn,"ineqnB")&
		strcmp(fn,"conplotD")&strcmp(fn,"conplotQ")&strcmp(fn,"conplotB"))){
		id=ifplotNG(n,fn);
		STOQ(id,ret);
		asir_push_one((Obj)ret);
	} else if(!(
		strcmp(fn,"ineqnandD")&strcmp(fn,"ineqnandQ")&strcmp(fn,"ineqnandB")&
		strcmp(fn,"ineqnorD")&strcmp(fn,"ineqnorQ")&strcmp(fn,"ineqnorB")&
		strcmp(fn,"ineqnxorD")&strcmp(fn,"ineqnxorQ")&strcmp(fn,"ineqnxorB")&
		strcmp(fn,"plotoverD")&strcmp(fn,"plotoverQ")&strcmp(fn,"plotoverB"))){
		id=ifplotOP(n,fn);
		STOQ(id,ret);
		asir_push_one((Obj)ret);
#if defined(INTERVAL)
	} else if(!strcmp(fn,"itvifplot")){
		id=ifplotNG(n,fn);
		STOQ(id,ret);
		asir_push_one((Obj)ret);
#endif
	}
}
