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
 * $OpenXM: OpenXM_contrib2/asir2000/plot/plotf.c,v 1.25 2014/01/07 06:22:08 saito Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "ox.h"
#include "ifplot.h"

int validate_ox_plot_stream(int);
void ListCheck(char *,LIST);
void Pplot(NODE,Obj *);
void Ppolarplot(NODE,Obj *);
void Pobj_cp(NODE,Obj *),Parrayplot(NODE,Obj*),Pdrawcircle(NODE,Obj*);
void Pifplot(NODE,Obj *),Pconplot(NODE,Obj *),Pplotover(NODE,Obj *);
void Pmemory_ifplot(NODE,Obj *),Pmemory_conplot(NODE,Obj *);
void Pmemory_plot(NODE,Obj *);
void ifplot_main(NODE,int,char *,Obj *);
void plot_main(NODE,int,char *,Obj *);
void conplot_main(NODE,int,Obj *);

void Popen_canvas(NODE,Obj *),Pclear_canvas(NODE,Obj *),Pdraw_obj(NODE,Obj *);
void Pdraw_string(NODE,Obj *);
void Pox_rpc(NODE,Obj *), Pox_cmo_rpc(NODE,Obj *);

//NG
#if defined(INTERVAL)
void Pitvifplot(NODE, Obj *);
#endif
void PifplotD(NODE,Obj *),PifplotQ(NODE,Obj *),PifplotB(NODE,Obj *);
void PineqnD(NODE,Obj *), PineqnQ(NODE,Obj *), PineqnB(NODE,Obj *);
void PineqnandD(NODE,Obj *), PineqnandQ(NODE,Obj *), PineqnandB(NODE,Obj *);
void PineqnorD(NODE,Obj *), PineqnorQ(NODE,Obj *), PineqnorB(NODE,Obj *);
void PineqnxorD(NODE,Obj *), PineqnxorQ(NODE,Obj *), PineqnxorB(NODE,Obj *);
void PconplotD(NODE,Obj *),PconplotQ(NODE,Obj *),PconplotB(NODE,Obj *);
void PplotoverD(NODE,Obj *),PplotoverQ(NODE,Obj *),PplotoverB(NODE,Obj *);
void ifplot_mainNG(NODE,char *,Obj *);
void conplot_mainNG(NODE,char *,Obj *);
void plotover_mainNG(NODE,char *,Obj *);
	
struct ftab plot_tab[]={
	{PLOT,Pplot,-7},
#if defined(INTERVAL)
	{ITVIFPLOT,Pitvifplot,-8},
#endif
	{POLARPLOT,Ppolarplot,-6},
	{OBJ_CP,Pobj_cp,4},
	{IFPLOT,Pifplot,-7},
	{IFPLOTD,PifplotD,-8},
	{IFPLOTQ,PifplotQ,-8},
	{IFPLOTB,PifplotB,-8},
	{INEQN,PineqnD,-8},
	{INEQND,PineqnD,-8},
	{INEQNQ,PineqnQ,-8},
	{INEQNB,PineqnB,-8},
	{INEQNAND,PineqnandD,-4},
	{INEQNDAND,PineqnandD,-4},
	{INEQNQAND,PineqnandQ,-4},
	{INEQNBAND,PineqnandB,-4},
	{INEQNOR,PineqnorD,-4},
	{INEQNDOR,PineqnorD,-4},
	{INEQNQOR,PineqnorQ,-4},
	{INEQNBOR,PineqnorB,-4},
	{INEQNXOR,PineqnxorD,-4},
	{INEQNDXOR,PineqnxorD,-4},
	{INEQNQXOR,PineqnxorQ,-4},
	{INEQNBXOR,PineqnxorB,-4},
	{CONPLOT,Pconplot,-7},
	{CONPLOTD,PconplotD,-8},
	{CONPLOTB,PconplotB,-8},
	{CONPLOTQ,PconplotQ,-8},
	{PLOTOVER,Pplotover,-4},
	{PLOTOVERD,PplotoverD,-4},
	{PLOTOVERQ,PplotoverQ,-4},
	{PLOTOVERB,PplotoverB,-4},
	{MEMORY_IFPLOT,Pmemory_ifplot,-7},
	{MEMORY_CONPLOT,Pmemory_conplot,-7},
	{MEMORY_PLOT,Pmemory_plot,-7},
	{DRAWCIRCLE,Pdrawcircle,4},
	{OPEN_CANVAS,Popen_canvas,-3},
	{CLEAR_CANVAS,Pclear_canvas,2},
	{DRAW_OBJ,Pdraw_obj,-3},
	{DRAW_STRING,Pdraw_string,-4},
	{0,0,0},
};
char *pfn[]={
	IFPLOT,CONPLOT,PLOT,INTERACTIVE,POLARPLOT,PLOTOVER,
	IFPLOTD,IFPLOTQ,IFPLOTB,INEQND,INEQNQ,INEQNB,
	INEQNDAND,INEQNQAND,INEQNBAND,
	INEQNDOR,INEQNQOR,INEQNBOR,
	INEQNDXOR,INEQNQXOR,INEQNBXOR,
	CONPLOTD,CONPLOTQ,CONPLOTB,ITVIFPLOT,
	PLOTOVERD,PLOTOVERQ,PLOTOVERB,
	MEMORY_IFPLOT,MEMORY_CONPLOT,MEMORY_PLOT,ARRAYPLOT,OPEN_CANVAS,
	DRAWCIRCLE,DRAW_OBJ,DRAW_STRING,OBJ_CP,CLEAR_CANVAS};
/*
	IFPLOT:0,CONPLOT:1,PLOT:2,INTERACTIVE:3,POLARPLOT:4,PLOTOVER:5,
	IFPLOTD:6,IFPLOTQ:7,IFPLOTB:8,INEQND:9,INEQNQ:10,INEQNB:11,
	INEQNDAND:12,INEQNQAND:13,INEQNBAND:14,
	INEQNDOR:15,INEQNQOR:16,INEQNBOR:17,
	INEQNDXOR:18,INEQNQXOR:19,INEQNBXOR:20,
	CONPLOTD:21,CONPLOTQ:22,CONPLOTB:23,ITVIFPLOT:24,
	PLOTOVERD:25,PLOTOVERQ:26,PLOTOVERB:27,
	MEMORY_IFPLOT:28,MEMORY_CONPLOT:29,MEMORY_PLOT:30,ARRAYPLOT:31,
	OPEN_CANVAS:32,DRAWCIRCLE:33,DRAW_OBJ:34,DRAW_STRING:35,OBJ_CP:36,
	CLEAR_CANVAS:37
*/
int modeNO(char *fn){
	int i;
	char **z;
	for(i=0,z=pfn;*z!=NULL;z++,i++)if(!strcmp(fn,*z))return i;
	return -1;
}

void Popen_canvas(NODE arg,Obj *rp){
	Q w300,s_id;
	LIST geom;
	int stream;
	NODE n,n0;
	STRING fname,wname;

	geom=0;wname=0;stream=-1;
	for(;arg;arg=NEXT(arg))
		if(!BDY(arg)) stream=0;
		else switch (OID(BDY(arg))){
			case O_LIST:
				geom=(LIST)BDY(arg);
				break;
			case O_N:
				stream=QTOS((Q)BDY(arg));
				break;
			case O_STR:
				wname=(STRING)BDY(arg);
				break;
			default:
				error("open_canvas : invalid argument");
				break;
		}
	stream=validate_ox_plot_stream(stream);
	STOQ(stream,s_id);
	if(!geom){
		STOQ(300,w300);
		MKNODE(n0,w300,0);
		MKNODE(n,w300,n0);
		MKLIST(geom,n);
	}
	MKSTR(fname,OPEN_CANVAS);
	arg=mknode(4,s_id,fname,geom,wname);
	Pox_cmo_rpc(arg,rp);
	*rp=(Obj)s_id;
}

void Pifplot(NODE arg,Obj *rp){ifplot_main(arg,0,IFPLOT,rp);}
void Pmemory_ifplot(NODE arg,Obj *rp){ifplot_main(arg,1,IFPLOT,rp);}

void ifplot_main(NODE arg,int is_memory,char *fn,Obj *rp){
	Q m2,p2,w300,s_id;
	NODE defrange;
	LIST xrange,yrange,zrange,range[2],list,geom;
	VL vl,vl0;
	V v[2],av[2];
	int stream,ri,i,sign;
	P poly,var;
	NODE n,n0;
	STRING fname,wname;
	Obj t;

	STOQ(-2,m2);STOQ(2,p2);MKNODE(n,p2,0);MKNODE(defrange,m2,n);
	poly=0;vl=0;geom=0;wname=0;stream=-1;ri=0;xrange=0;yrange=0;zrange=0;
	v[0]=v[1]=0;
	for(;arg;arg=NEXT(arg))
		if(!BDY(arg))stream=0;
		else
		switch(OID(BDY(arg))){
		case O_P:
			poly=(P)BDY(arg);
			get_vars_recursive((Obj)poly,&vl);
			for(vl0=vl,i=0;vl0;vl0=NEXT(vl0)){
				if(vl0->v->attr==(pointer)V_IND){
					if(i>=2)error("ifplot : invalid argument");
					else v[i++]=vl0->v;
				}
			}
			break;
		case O_LIST:
			list=(LIST)BDY(arg);
			if(OID(BDY(BDY(list)))==O_P)
				if(ri>1) error("ifplot : invalid argument");
				else range[ri++]=list;
			else geom=list;
			break;
		case O_N:
			stream=QTOS((Q)BDY(arg));break;
		case O_STR:
			wname=(STRING)BDY(arg);break;
		default:
			error("ifplot : invalid argument");break;
		}
	if(!poly) error("ifplot : invalid argument");
	switch(ri){
	case 0:
		if(!v[1]) error("ifplot : please specify all variables");
		MKV(v[0],var);MKNODE(n,var,defrange);MKLIST(xrange,n);
		MKV(v[1],var);MKNODE(n,var,defrange);MKLIST(yrange,n);
		break;
	case 1:
		if(!v[1]) error("ifplot : please specify all variables");
		av[0]=VR((P)BDY(BDY(range[0])));
		if(v[0]==av[0]){
			xrange=range[0];
			MKV(v[1],var);MKNODE(n,var,defrange);MKLIST(yrange,n);
		} else if(v[1]==av[0]){
			MKV(v[0],var);MKNODE(n,var,defrange);MKLIST(xrange,n);
			yrange=range[0];
		} else error("ifplot : invalid argument");
		break;
	case 2:
		av[0]=VR((P)BDY(BDY(range[0])));
		av[1]=VR((P)BDY(BDY(range[1])));
		if(((v[0]==av[0])&&(!v[1]||v[1]==av[1]))||
			((v[0]==av[1])&&(!v[1]||v[1]==av[0]))){
			xrange=range[0];yrange=range[1];
		} else error("ifplot : invalid argument");
		break;
	default:
		error("ifplot : cannot happen");break;
	}
	/* ifplot in ox_plot requires 
		[s_id (Q),
		formula (Obj),
		xrange=[x,xmin,xmax] (LIST),
		yrange=[y,ymin,ymax] (LIST),
		zrange=0,
		geom=[xsize,ysize] (LIST),
		wname=name (STRING)]
	*/
	stream=validate_ox_plot_stream(stream);
	STOQ(stream,s_id);
	if(!geom){
		STOQ(300,w300);MKNODE(n0,w300,0);MKNODE(n,w300,n0);MKLIST(geom,n);
	}
	if(is_memory){
		MKSTR(fname,MEMORY_PLOT);
		arg=mknode(8,s_id,fname,poly,xrange,yrange,zrange,geom);
		Pox_rpc(arg,&t);
		arg=mknode(1,s_id);
		Pox_pop_cmo(arg,rp);
	} else {
		MKSTR(fname,fn);
/*
printf("%s\n",fn);
ListCheck("xrange",xrange);
ListCheck("yrange",yrange);
ListCheck("geom",geom);
*/
		arg=mknode(8,s_id,fname,poly,xrange,yrange,zrange,geom,wname);
		Pox_rpc(arg,&t);
		*rp=(Obj)s_id;
	}
}

void Pobj_cp(NODE arg,Obj *rp){
	//copy canvas
	//1:and,3:copy,6:xor,7:or
	Q sysid,index_A,index_B,op_code;
	STRING fname;
	Obj t;
	sysid=(Q)ARG0(arg);
	index_A=(Q)ARG1(arg);
	index_B=(Q)ARG2(arg);
	op_code=(Q)ARG3(arg);
	MKSTR(fname,OBJ_CP);
	arg=mknode(5,sysid,fname,index_A,index_B,op_code);
	Pox_rpc(arg,&t);
	*rp=(Obj)sysid;
}

void Pconplot(NODE arg,Obj *rp){conplot_main(arg, 0, rp);}
void Pmemory_conplot(NODE arg,Obj *rp){conplot_main(arg,1,rp);}

void conplot_main(NODE arg,int is_memory,Obj *rp){
	int stream,ri,i;
	Q m2,p2,w300,s_id,mxgc;
	NODE defrange,n,n0,n1,n2,n3;
	LIST xrange,yrange,zrange,range[3],list,geom;
	VL vl,vl0;
	V v[2],av[2];
	P poly,var;
	STRING fname,wname;
	Obj t;

	STOQ(-2,m2);STOQ(2,p2);MKNODE(n,p2,0);MKNODE(defrange,m2,n);
	poly=0;vl=0;geom=0;wname=0;stream=-1;ri=0;v[0]=v[1]=0;
	for(;arg;arg=NEXT(arg))
		if(!BDY(arg)) stream=0;
		else switch (OID(BDY(arg))){
			case O_P:
				poly=(P)BDY(arg);
				get_vars_recursive((Obj)poly,&vl);
				for(vl0=vl,i=0;vl0;vl0=NEXT(vl0)){
					if(vl0->v->attr==(pointer)V_IND){
						if(i>=2){
							error("conplot : invalid argument");
						} else v[i++]=vl0->v;
					}
				}
				break;
			case O_LIST:
				list=(LIST)BDY(arg);
				if(OID(BDY(BDY(list)))==O_P)
					if(ri>2) error("conplot : invalid argument");
					else range[ri++]=list;
				else geom=list;
				break;
			case O_N:
				stream=QTOS((Q)BDY(arg));
				break;
			case O_STR:
				wname=(STRING)BDY(arg);
				break;
			default:
				error("conplot : invalid argument");
				break;
		}
	if(!poly) error("conplot : invalid argument");
	// list format var,num,num[,num]
	switch (ri){
		case 0:
			if(!v[1]) error("conplot : please specify all variables");
			MKV(v[0],var);MKNODE(n,var,defrange);MKLIST(xrange,n);
			MKV(v[1],var);MKNODE(n,var,defrange);MKLIST(yrange,n);
			STOQ(MAXGC,mxgc);MKNODE(n1,mxgc,0);MKNODE(n2,p2,n1);MKNODE(n3,p2,n2);
			MKLIST(zrange,n3);
			break;
		case 1:
			if(!v[1]) error("conplot : please specify all variables");
			av[0]=VR((P)BDY(BDY(range[0])));
			if(v[0]==av[0]){
				xrange=range[0];
				MKV(v[1],var);MKNODE(n,var,defrange);MKLIST(yrange,n);
				STOQ(MAXGC,mxgc);MKNODE(n1,mxgc,0);MKNODE(n2,p2,n1);MKNODE(n3,p2,n2);
				MKLIST(zrange,n3);
			} else if(v[1]==av[0]){
				MKV(v[0],var);MKNODE(n,var,defrange);MKLIST(xrange,n);
				yrange=range[0];
				STOQ(MAXGC,mxgc);MKNODE(n1,mxgc,0);MKNODE(n2,p2,n1);MKNODE(n3,p2,n2);
				MKLIST(zrange,n3);
			} else {
				MKV(v[0],var);MKNODE(n,var,defrange);MKLIST(xrange,n);
				MKV(v[1],var);MKNODE(n,var,defrange);MKLIST(yrange,n);
				n=NEXT((NODE)BDY(range[0]));
				for(i=0,n1=n;n1;n1=NEXT(n1),i++);
				if(i==3)MKLIST(zrange,n);
				else if(i==2){
					n1=(NODE)BDY(n);
					n2=(NODE)(BDY(NEXT(n)));
					STOQ(MAXGC,mxgc);MKNODE(n,mxgc,0);
					MKNODE(n3,n1,n);
					MKNODE(n1,n2,n3);
					MKLIST(zrange,n1);
				} else error("conplot : zrange error");
			}
			break;
		case 2:
		case 3:
			av[0]=VR((P)BDY(BDY(range[0])));
			av[1]=VR((P)BDY(BDY(range[1])));
			if(((v[0]==av[0])&&(!v[1]||v[1]==av[1]))||
				((v[0]==av[1])&&(!v[1]||v[1]==av[0]))){
				xrange=range[0];
				yrange=range[1];
				if(ri==3) MKLIST(zrange,NEXT(BDY(range[2])));
				else {
					for(i=0,n=(NODE)BDY(range[0]);n;n=NEXT(n),i++);
					MKNODE(n,0,defrange);
					MKLIST(zrange,n);
				}
			} else error("conplot : invalid argument");
			break;
		default:
			error("conplot : cannot happen");
			break;
	}
	/* conplot in ox_plot requires 
	   [s_id (Q),
	   	formula (Obj),
	   	xrange=[x,xmin,xmax] (LIST),
	   	yrange=[y,ymin,ymax] (LIST),
	   	zrange=[z,zmin,zmax,nstep] (LIST),z:dummy var
	   	geom=[xsize,ysize] (LIST),
	   	wname=name (STRING)]
	*/
	stream=validate_ox_plot_stream(stream);
	STOQ(stream,s_id);
	if(!geom){
		STOQ(300,w300);
		MKNODE(n0,w300,0);
		MKNODE(n,w300,n0);
		MKLIST(geom,n);
	}
	if(is_memory){
		MKSTR(fname,MEMORY_PLOT);
		arg=mknode(7,s_id,fname,poly,xrange,yrange,zrange,geom);
		Pox_rpc(arg,&t);
		arg=mknode(1,s_id);
		Pox_pop_cmo(arg,rp);
	} else {
		//list check
		for(i=0,n=(NODE)BDY(xrange);n;i++,n=NEXT(n));
		if(i!=3)error("conplot : xrange error");
		for(i=0,n=(NODE)BDY(yrange);n;i++,n=NEXT(n));
		if(i!=3)error("conplot : yrange error");
		for(i=0,n=(NODE)BDY(zrange);n;i++,n=NEXT(n));
		if(i!=3)error("conplot : xrange error");
		MKSTR(fname,CONPLOT);
		arg=mknode(8,s_id,fname,poly,xrange,yrange,zrange,geom,wname);
		Pox_rpc(arg,&t);
		*rp=(Obj)s_id;
	}
}

void Pplot(NODE arg,Obj *rp){plot_main(arg,0,PLOT,rp);}
void Pmemory_plot(NODE arg,Obj *rp){plot_main(arg,1,PLOT,rp);}

void plot_main(NODE arg,int is_memory,char *fn,Obj *rp){
	Q m2,p2,w300,s_id;
	NODE defrange;
	LIST xrange,range[1],list,geom;
	VL vl,vl0;
	V v[1],av[1];
	int stream,ri,i;
	P poly;
	P var;
	NODE n,n0;
	STRING fname,wname;
	Obj t;

	STOQ(-2,m2);STOQ(2,p2);
	MKNODE(n,p2,0);MKNODE(defrange,m2,n);
	poly=0;vl=0;geom=0;wname=0;stream=-1;ri=0;
	v[0]=0;
	for(;arg;arg=NEXT(arg) )
		if(!BDY(arg) )
			stream=0;
		else
		switch ( OID(BDY(arg)) ){
			case O_P: case O_R:
				poly=(P)BDY(arg);
				get_vars_recursive((Obj)poly,&vl);
				for(vl0=vl, i=0;vl0;vl0=NEXT(vl0) ){
					if(vl0->v->attr==(pointer)V_IND ){
						if(i >= 1 ) error("ifplot : invalid argument");
						else v[i++]=vl0->v;
					}
				}
				if(i != 1 ) error("ifplot : invalid argument");
				break;
			case O_LIST:
				list=(LIST)BDY(arg);
				if(OID(BDY(BDY(list)))==O_P ){
					if(ri > 0 ) error("plot : invalid argument");
					else range[ri++]=list;
				} else geom=list;
				break;
			case O_N:
				stream=QTOS((Q)BDY(arg));break;
			case O_STR:
				wname=(STRING)BDY(arg);break;
			default:
				error("plot : invalid argument");break;
		}
	if(!poly )
		error("plot : invalid argument");
	switch ( ri ){
		case 0:
			MKV(v[0],var);MKNODE(n,var,defrange);MKLIST(xrange,n);
			break;
		case 1:
			av[0]=VR((P)BDY(BDY(range[0])));
			if(v[0]==av[0] )
				xrange=range[0];
			else
				error("plot : invalid argument");
			break;
		default:
			error("plot : cannot happen");break;
	}
	/* conplot in ox_plot requires 
	   [s_id (Q),
	   	formula (Obj),
	   	xrange=[x,xmin,xmax] (LIST),
	   	yrange=0,
	   	zrange=0,
	   	geom=[xsize,ysize] (LIST),
	   	wname=name (STRING)]
	*/
	stream=validate_ox_plot_stream(stream);
	STOQ(stream,s_id);
	if(!geom ){
		STOQ(300,w300);
		MKNODE(n0,w300,0);MKNODE(n,w300,n0);MKLIST(geom,n);
	}
	if(is_memory ){
		MKSTR(fname,MEMORY_PLOT);
		arg=mknode(7,s_id,fname,poly,xrange,NULLP,NULLP,geom);
		Pox_rpc(arg,&t);
		arg=mknode(1,s_id);
		Pox_pop_cmo(arg,rp);
	} else {
		MKSTR(fname,fn);
		arg=mknode(8,s_id,fname,poly,xrange,NULLP,NULLP,geom,wname);
		Pox_rpc(arg,&t);
		*rp=(Obj)s_id;
	}
}

#define Pi 3.14159265358979323846264

void Ppolarplot(NODE arg,Obj *rp){
	Q m2,p2,w300,defstep,s_id,color;
	NODE defrange,n,n0,n1,n2;
	LIST range,geom,list[2];
	VL vl,vl0;
	V v[1],av;
	int stream,ri,i,len,iNo,lNo,vNo,sNo,pfine,findG;
	P poly,var;
	STRING fname,wname;
	Real pi2;
	Obj t,frst,sec,thr;;
	char ebuf[BUFSIZ];

	iNo=lNo=sNo=findG=0;pfine=TRUE;
	poly=0;vl=0;geom=0;wname=0;color=0;stream=-1;ri=0;v[0]=0;
	for(;arg;arg=NEXT(arg)){
		if(!BDY(arg)) iNo++;
		else switch(OID(BDY(arg))){
		case O_P: case O_R://formular
			poly=(P)BDY(arg);
			get_vars_recursive((Obj)poly,&vl);
			for(vl0=vl,vNo=0;vl0;vl0=NEXT(vl0)){
				if(vl0->v->attr==(pointer)V_IND){
					if(vNo>=1){
						sprintf(ebuf,"%s : invalaid argument",POLARPLOT);
						error(ebuf);
					} else v[vNo++]=vl0->v;
				}
			}
			if(vNo!=1){
				sprintf(ebuf,"%s : only uni-variate formular",POLARPLOT);
				error(ebuf);
			}
			break;
		case O_N://color,id,idx,division
			switch (iNo){
			case 0://color arg
				color=(Q)BDY(arg);
				iNo++;
				break;
			case 1://stream arg
				stream=QTOS((Q)BDY(arg));
				iNo++;
				break;
			default://error
				sprintf(ebuf,"%s : invalid number arguments",POLARPLOT);
				error(ebuf);
				break;
			}
			break;
		case O_LIST://range,geomerty
			if(lNo<2)list[lNo++]=(LIST)BDY(arg);
			else {
				sprintf(ebuf,"%s : invalid list argument",POLARPLOT);
				error(ebuf);
			}
			break;
		case O_STR://wname
			wname=(STRING)BDY(arg);
			sNo++;
			break;
		default:
			break;
		}
	}
	//formular check
	if(!poly){
		sprintf(ebuf,"%s : invalid ploy argument",POLARPLOT);
		error(ebuf);
	}
	//vars check
	get_vars_recursive((Obj)poly,&vl);
	for(vl0=vl,vNo=0;vl0;vNo++,vl0=NEXT(vl0)){
		if(vl0->v->attr==(pointer)V_IND){
			if(vNo>=2){
				sprintf(ebuf,"%s : invalid ploy argument",POLARPLOT);
				error(ebuf);
			} else v[vNo]=vl0->v;
		}
	}
	//list check
	for(i=0;i<lNo;i++){
		if(OID(BDY(BDY(list[i])))!=O_P){
			// list first value is number (geometry)
			for(len=0,n=(NODE)BDY(list[i]);n;len++,n=NEXT(n)){
				if(len==0) frst=BDY(n);
				else if(len==1) sec=BDY(n);
				else {
					sprintf(ebuf,"%s : geometry list too long",POLARPLOT);
					error(ebuf);
				}
			}
			if(len!=2){
				sprintf(ebuf,"%s : geometry requierd 2 numbers", POLARPLOT);
				error(ebuf);
			} else geom=list[i];
		} else {
			//list first value is var (range)
			av=VR((P)BDY(BDY(list[i])));
			if(v[0]==av)range=list[i];
			else {
				sprintf(ebuf,"%s : invalid list length",POLARPLOT);
				error(ebuf);
			}
		}
	}
	// set default
	if(!range){
		STOQ(DEFAULTPOLARSTEP,defstep);MKReal(2*Pi,pi2);MKV(v[0],var);
		MKNODE(n,defstep,0);MKNODE(n1,pi2,n);MKNODE(n2,0,n1);
		MKNODE(defrange,var,n2);MKLIST(range,defrange);
	}
	if(!geom){
		STOQ(300,w300);MKNODE(n0,w300,0);MKNODE(n,w300,n0);MKLIST(geom,n);
	}
	stream=validate_ox_plot_stream(stream);
	STOQ(stream,s_id);
	MKSTR(fname,POLARPLOT);
	arg=mknode(7,s_id,fname,poly,color,range,geom,wname);
	Pox_rpc(arg,&t);
	*rp=(Obj)s_id;
}

void Pplotover(NODE arg,Obj *rp){
	Q s_id,w_id,color;
	P poly;
	STRING fname;
	Obj t;
	poly=(P)ARG0(arg);
	s_id=(Q)ARG1(arg);
	w_id=(Q)ARG2(arg);
	if(argc(arg)==4)color=(Q)ARG3(arg);
	else color=0;
	MKSTR(fname,PLOTOVER);
	arg=mknode(5,s_id,fname,w_id,poly,color);
	Pox_rpc(arg,&t);
	*rp=(Obj)s_id;
}

void Pdrawcircle(NODE arg,Obj *rp){
	Q s_id,index;
	Obj x,y,r,c,t;
	STRING fname;
	NODE n;
	LIST pos;

	x=(Obj)ARG0(arg);
	y=(Obj)ARG1(arg);
	r=(Obj)ARG2(arg);
	c=(Obj)ARG3(arg);
	s_id=(Q)ARG4(arg);
	index=(Q)ARG5(arg);
	MKSTR(fname,DRAWCIRCLE);
	n=mknode(3,x,y,r,c);
	MKLIST(pos,n);
	arg=mknode(5,s_id,fname,index,pos,c);
	Pox_rpc(arg,&t);
	*rp=(Obj)s_id;
}

void Pdraw_obj(NODE arg,Obj *rp){
	static STRING fname;
	Q s_id,index;
	LIST obj;
	Obj t;

	if(!fname)MKSTR(fname,DRAW_OBJ);
	s_id=(Q)ARG0(arg);
	index=(Q)ARG1(arg);
	obj=(LIST)ARG2(arg);
	// ARG3(arg)=color
	if(argc(arg)==4) arg=mknode(5,s_id,fname,index,obj,ARG3(arg));
	else arg=mknode(4,s_id,fname,index,obj);
	Pox_cmo_rpc(arg,&t);
	*rp=(Obj)s_id;
}

void Pdraw_string(NODE arg,Obj *rp){
	static STRING fname;
	STRING str;
	Q s_id,index;
	LIST pos;
	Obj t;

	if(!fname)MKSTR(fname,DRAW_STRING);
	s_id=(Q)ARG0(arg);
	index=(Q)ARG1(arg);
	pos=(LIST)ARG2(arg);
	str=(STRING)ARG3(arg);
	// ARG4(arg)=color
	if(argc(arg)==5) arg=mknode(6,s_id,fname,index,pos,str,ARG4(arg));
	else arg=mknode(5,s_id,fname,index,pos,str);
	Pox_cmo_rpc(arg,&t);
	*rp=(Obj)s_id;
}

void Pclear_canvas(NODE arg,Obj *rp){
	static STRING fname;
	Q s_id,index;
	Obj t;

	if(!fname) MKSTR(fname,CLEAR_CANVAS);
	s_id=(Q)ARG0(arg);
	index=(Q)ARG1(arg);
	arg=mknode(3,s_id,fname,index);
	Pox_cmo_rpc(arg,&t);
	*rp=(Obj)s_id;
}
//****************************ifplotNG
/*
 * name ifplot,ineqn,ineqnor,ineqnand,ineqnxor,conplot
 * type
 *  D:sign character using double
 *  Q:sign character use rational
 *  B:Boundary character use sturm theorem
 */
void ListCheck(char * head,LIST list){
	int i;
	NODE n;
	if(!list){
		printf("%s zero \n",head);
		return;
	}
	for(i=0,n=(NODE)BDY(list);n;i++,n=NEXT(n));
	printf("%s length %d\n",head,i);
	for(i=0,n=(NODE)BDY(list);n;i++,n=NEXT(n)){
		if(!BDY(n))printf("%d 0\n",i);
		else if(OID(BDY(n))==O_P) printf("%d poly\n",i);
		else if(OID(BDY(n))==O_R) printf("%d real\n",i);
		else if(OID(BDY(n))==O_N) printf("%d %d\n",i,QTOS((Q)BDY(n)));
	}
}

void PifplotD(NODE arg,Obj *rp){ifplot_mainNG(arg,IFPLOTD,rp);}
void PifplotQ(NODE arg,Obj *rp){ifplot_mainNG(arg,IFPLOTQ,rp);}
void PifplotB(NODE arg,Obj *rp){ifplot_mainNG(arg,IFPLOTB,rp);}

void PconplotD(NODE arg,Obj *rp){ifplot_mainNG(arg,CONPLOTD,rp);}
void PconplotQ(NODE arg,Obj *rp){ifplot_mainNG(arg,CONPLOTQ,rp);}
void PconplotB(NODE arg,Obj *rp){ifplot_mainNG(arg,CONPLOTB,rp);}

void PineqnD(NODE arg,Obj *rp){ifplot_mainNG(arg,INEQND,rp);}
void PineqnQ(NODE arg,Obj *rp){ifplot_mainNG(arg,INEQNQ,rp);}
void PineqnB(NODE arg,Obj *rp){ifplot_mainNG(arg,INEQNB,rp);}

#if defined(INTERVAL)
void Pitvifplot(NODE arg,Obj *rp){ifplot_mainNG(arg,ITVIFPLOT,rp);}
#endif

void PineqnorD(NODE arg,Obj *rp){plotover_mainNG(arg,INEQNDOR,rp);}
void PineqnorQ(NODE arg,Obj *rp){plotover_mainNG(arg,INEQNQOR,rp);}
void PineqnorB(NODE arg,Obj *rp){plotover_mainNG(arg,INEQNBOR,rp);}

void PineqnandD(NODE arg,Obj *rp){plotover_mainNG(arg,INEQNDAND,rp);}
void PineqnandQ(NODE arg,Obj *rp){plotover_mainNG(arg,INEQNQAND,rp);}
void PineqnandB(NODE arg,Obj *rp){plotover_mainNG(arg,INEQNBAND,rp);}

void PineqnxorD(NODE arg,Obj *rp){plotover_mainNG(arg,INEQNDXOR,rp);}
void PineqnxorQ(NODE arg,Obj *rp){plotover_mainNG(arg,INEQNQXOR,rp);}
void PineqnxorB(NODE arg,Obj *rp){plotover_mainNG(arg,INEQNBXOR,rp);}

void PplotoverD(NODE arg,Obj *rp){plotover_mainNG(arg,PLOTOVERD,rp);}
void PplotoverQ(NODE arg,Obj *rp){plotover_mainNG(arg,PLOTOVERQ,rp);}
void PplotoverB(NODE arg,Obj *rp){plotover_mainNG(arg,PLOTOVERB,rp);}

void ifplot_mainNG(NODE arg,char *fn,Obj *rp){
	Q m2,p2,w300,mxgc,s_id,color;
	NODE defrange,n,n0,n1,n2;
	P poly,var;
	VL vl,vl0;
	V v[2],av;
	LIST xrange,yrange,zrange,geom,range[2],list[4];
	int stream,ri,i,j,sign,llen,len,iNo,lNo,vNo,sNo,pfine,findG;
	STRING fname,wname;
	Obj t,frst,sec,thr;
	char ebuf[BUFSIZ];

	iNo=lNo=sNo=findG=0;pfine=TRUE;
	poly=0;stream=-1;wname=0;color=0;stream=0;
	STOQ(-2,m2);STOQ(2,p2);MKNODE(n,p2,0);MKNODE(defrange,m2,n);
	STOQ(MAXGC,mxgc);
	for(;arg;arg=NEXT(arg)){
		if(!BDY(arg)) iNo++;
		else switch(OID(BDY(arg))){
		case O_P://formular
			if(pfine){
				poly=(P)BDY(arg);
				pfine=FALSE;
			} else {
				sprintf(ebuf,"%s : to many ploy arguments",fn);
				error(ebuf);
			}
			break;
		case O_N://color,id,idx,division
			switch (iNo){
			case 0: //color arg
				color=(Q)BDY(arg);
				iNo++;
				break;
			case 1: //stream arg
				stream=QTOS((Q)BDY(arg));
				iNo++;
				break;
			default:
				sprintf(ebuf,"%s : invalid number arguments",fn);
				error(ebuf);
				break;
			}
			break;
		case O_LIST://xrange,yrange,zrange,geometry
			if(lNo<4) list[lNo++]=(LIST)BDY(arg);
			else {
				sprintf(ebuf,"%s : invalid list argument",fn);
				error(ebuf);
			}
			break;
		case O_STR://wname
			wname=(STRING)BDY(arg);
			sNo++;
			break;
		default:
			break;
		}
	}
	// formular check
	if(!poly){
		sprintf(ebuf,"%s : invalid ploy argument",fn);
		error(ebuf);
	}
	// vars check
	get_vars_recursive((Obj)poly,&vl);
	for(vl0=vl,vNo=0;vl0;vl0=NEXT(vl0)){
		if(vl0->v->attr==(pointer)V_IND){
			if(vNo>=2){
				sprintf(ebuf,"%s : invalid ploy argument",fn);
				error(ebuf);
			} else v[vNo++]=vl0->v;
		}
	}
	//list check
	xrange=yrange=zrange=geom=0;frst=sec=thr=0;
	for(i=0;i<lNo;i++){
		for(llen=0,n=(NODE)BDY(list[i]);n;llen++,n=NEXT(n));
		if(llen>4){
			sprintf(ebuf,"%s : invalid list length",fn);
			error(ebuf);
		}
		if(OID(BDY(BDY(list[i])))!=O_P){
			// First list value is number
			for(len=0,n=(NODE)BDY(list[i]);n;len++,n=NEXT(n)){
				if(len==0)frst=BDY(n);
				else if(len==1)sec=BDY(n);
				else thr=BDY(n);
			}
			switch(len){
			case 2:
				if(!strcmp(fn,CONPLOT)){
					if(thr==0)thr=(Obj)mxgc;
					MKNODE(n,thr,0);MKNODE(n1,sec,n);MKNODE(n2,frst,n1);MKLIST(zrange,n2);
				} else geom=list[i];
				break;
			case 3:
				zrange=list[i];
				break;
			case 0:
			case 1:
			default:
				sprintf(ebuf,"%s : invalid list length",fn);
				error(ebuf);
				break;
			}
		} else {
			//xrange,yrange
			av=VR((P)BDY(BDY(list[i])));
			if(v[0]==av)xrange=list[i];
			else if(v[1]==av)yrange=list[i];
			else {
				MKLIST(zrange,NEXT(BDY(list[i])));
			}
		}
	}
	//set default
	if(!xrange){
		MKV(v[0],var);MKNODE(n,var,defrange);MKLIST(xrange,n);
	}
	if(!yrange){
		MKV(v[1],var);MKNODE(n,var,defrange);MKLIST(yrange,n);
	}
	if(!geom){
		STOQ(300,w300);MKNODE(n0,w300,0);MKNODE(n,w300,n0);MKLIST(geom,n);
	}
	if(!(strcmp(fn,CONPLOTD)&strcmp(fn,CONPLOTQ)&strcmp(fn,CONPLOTB))&!zrange){
		MKNODE(n,mxgc,0);MKNODE(n1,m2,n);MKNODE(n2,m2,n1);MKLIST(zrange,n2);
	}
	/*new ifplot in ox_plot requires 
		[s_id (Q),
		formula (Obj),
		color (Q),
		geom=[xsize,ysize] (LIST optional),
		xrange=[x,xmin,xmax] (LIST optional),
		yrange=[y,ymin,ymax] (LIST optional),
		zrange=[zmin,zmax,zstep] (LIST optional),
		wname=name (STRING optional)],
		itvstep (Q) if ITVIFPLOT */
	stream=validate_ox_plot_stream(stream);
	STOQ(stream,s_id);
	MKSTR(fname,fn);
/*
printf("%s\n",fn);
ListCheck("xrange",xrange);
ListCheck("yrange",yrange);
ListCheck("zrange",zrange);
ListCheck("geom",geom);
printf("idv %d\n",idv);
*/
	arg=mknode(9,s_id,fname,poly,color,xrange,yrange,zrange,geom,wname);
	Pox_rpc(arg,&t);
	*rp=(Obj)s_id;
}

void plotover_mainNG(NODE arg,char *fn,Obj *rp){
	//[s_id (Q), w_id (Q), formula (Obj), color (Q)]
	Q s_id,w_id,color;
	P poly;
	STRING fname;
	Obj t;
	int iNo,pfine,sfine;
	char ebuf[BUFSIZ];

	pfine=sfine=TRUE;
	iNo=0;poly=0;color=s_id=w_id=0;
	for(;arg;arg=NEXT(arg)){
		if(!BDY(arg)) iNo++;
		else switch(OID(BDY(arg))){
		case O_P://formular
			if(pfine){
				poly=(P)BDY(arg);
				pfine=FALSE;
			} else {
				sprintf(ebuf,"%s : to many ploy arguments",fn);
				error(ebuf);
			}
			break;
		case O_N://color,s_id,w_id
			switch(iNo){
			case 0://color arg
				color=(Q)BDY(arg);
				iNo++;
				break;
			case 1://stream arg
				s_id=(Q)BDY(arg);
				iNo++;
				break;
			case 2://window arg
				w_id=(Q)BDY(arg);
				iNo++;
				break;
			default://error
				sprintf(ebuf,"%s : to many numbers",fn);
				error(ebuf);
				break;
			}
			break;
		default:
			sprintf(ebuf,"%s : arguments type miss match",fn);
			error(ebuf);
		}
	}
	MKSTR(fname,fn);
	arg=mknode(5,s_id,fname,w_id,poly,color);
//printf("fn:%s s_id:%d w_id:%d color:%d\n",BDY(fname),QTOS(s_id),QTOS(w_id),QTOS(color));
	Pox_rpc(arg,&t);
	*rp=(Obj)s_id;
}
