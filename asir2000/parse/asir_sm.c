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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/asir_sm.c,v 1.6 2004/02/13 05:48:36 saito Exp $ 
*/
#include "ca.h"
#include "parse.h"
#if defined(THINK_C)
#include <console.h>
#endif

#if defined(PARI)
#include "genpari.h"
#  if !(PARI_VERSION_CODE > 131588)
extern jmp_buf environnement;
#  endif
#endif

extern jmp_buf env;
extern int ox_do_copy, ox_do_count, ox_count_length;
extern char *ox_copy_bptr;

Obj Asir_OperandStack[BUFSIZ];
char *Asir_DebugStack[BUFSIZ];
int Asir_OperandStackPtr;
int Asir_DebugStackPtr;

Asir_Start()
{
	int tmp;
	FILE *ifp;
	char ifname[BUFSIZ];
	extern int GC_dont_gc;
	extern int read_exec_file;
	extern int do_asirrc;
	char *getenv();
	static ox_asir_initialized = 0;

	asir_save_handler();
	if ( ox_asir_initialized )
		return;
	GC_init();
	ox_asir_initialized = 1;
	ox_do_copy = 1;
#if defined(THINK_C)
	param_init();
#endif
	rtime_init();
	env_init();
	endian_init();
#if !defined(TOWNS) && !defined(THINK_C)
/*	check_key(); */
#endif
#if defined(TOWNS) && !defined(GO32) && !defined(__WIN32__)
	disable_ctrl_c();
#endif
/*	process_args(argc,argv); */
#if 0
	copyright();
#endif
	output_init();
	arf_init();
	nglob_init();
	glob_init();
	sig_init();
	tty_init();
	debug_init();
	pf_init();
	sysf_init();
	parif_init();
#if defined(UINIT)
	reg_sysf();
#endif
#if defined(THINK_C) || defined(TOWNS)
	sprintf(ifname,"asirrc");
#else
	sprintf(ifname,"%s/.asirrc",getenv("HOME"));
#endif
	asir_set_handler();
	if ( do_asirrc && (ifp = fopen(ifname,"r")) ) {
		input_init(ifp,ifname);
		if ( !setjmp(env) ) {
			read_exec_file = 1;
			read_eval_loop();
		}
		fclose(ifp);
	}
	asir_reset_handler();
	input_init(0,"string");
	Asir_OperandStackPtr = Asir_DebugStackPtr = -1;
}

char *Asir_PopBinary();
char *Asir_PopString();
Obj asir_pop_obj();

extern char *parse_strp;
extern int ox_do_copy;

int Asir_ExecuteString(s)
char *s;
{
	SNODE snode;
	pointer val;
#if defined(PARI)
	static long tloc,listloc;
	extern long avloc;

	Asir_Start();
	asir_set_handler();
	avloc = avma; tloc = tglobal; listloc = marklist();
#  if !(PARI_VERSION_CODE > 131588)
	if ( setjmp(environnement) ) {
		avma = avloc; tglobal = tloc; recover(listloc);
		resetenv("");
	}
#  endif
#endif
	if ( setjmp(env) ) {
		asir_reset_handler();
		return -1;
	}
	parse_strp = s;
	if ( mainparse(&snode) ) {
		asir_reset_handler();
		return -1;
	}
	val = evalstat(snode);
	if ( NEXT(asir_infile) ) {
		while ( NEXT(asir_infile) ) {
			if ( mainparse(&snode) ) {
				asir_push_obj(val);
				asir_reset_handler();
				return -1;
			}
			nextbp = 0;
			val = evalstat(snode);
		}
	}
	asir_push_obj(val);
	asir_reset_handler();
	return 0;
}

char *Asir_PopString()
{
	Obj val;
	char *buf,*obuf;
	int l;

	Asir_Start();
	val = asir_pop_obj();
	if ( !val )
		return 0;
	else {
		l = estimate_length(CO,val);
		buf = (char *)ALLOCA(l+1);
		soutput_init(buf);
		sprintexpr(CO,val);
		l = strlen(buf);
		obuf = (char *)GC_malloc(l+1);
		strcpy(obuf,buf);
		return obuf;
	}
}

int Asir_Set(name)
char *name;
{
	int l,n;
	char *dummy = "=0;";
	SNODE snode;

	Asir_Start();
	l = strlen(name);
	n = l+strlen(dummy)+1;
	parse_strp = (char *)ALLOCA(n);
	sprintf(parse_strp,"%s%s",name,dummy);
	if ( mainparse(&snode) )
		return -1;
	FA1((FNODE)FA0(snode)) = (pointer)mkfnode(1,I_FORMULA,asir_pop_obj());
	evalstat(snode);	
	return 0;
}

int Asir_PushBinary(size,data)
int size;
char *data;
{
	Obj val;

	Asir_Start();
	ox_copy_bptr = data;
	loadvl(0);
	loadobj(0,&val);
	asir_push_obj(val);
	ox_copy_bptr = 0;
}

char *Asir_PopBinary(size)
int *size;
{
	Obj val;
	char *buf;
	VL vl,t;

	Asir_Start();
	val = asir_pop_obj();
	get_vars(val,&vl);
	for ( t = vl; t; t = NEXT(t) )
		if ( t->v->attr == (pointer)V_UC || t->v->attr == (pointer)V_PF )
			error("bsave : not implemented");
	ox_count_length = 0; ox_do_count = 1;
	savevl(0,vl);
	saveobj(0,val);
	*size = ox_count_length;
	ox_count_length = 0; ox_do_count = 0;
	ox_copy_bptr = buf = (char *)GC_malloc(*size);
	savevl(0,vl);
	saveobj(0,val);
	ox_copy_bptr = 0;
	return buf;
}

asir_push_obj(obj)
Obj obj;
{
	Asir_OperandStack[++Asir_OperandStackPtr] = obj;
}

Obj asir_pop_obj() {
	if ( Asir_OperandStackPtr < 0 )
		return 0;
	else
		return Asir_OperandStack[Asir_OperandStackPtr--];
}
