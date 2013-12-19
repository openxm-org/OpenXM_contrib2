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
 * $OpenXM: OpenXM_contrib2/asir2000/plot/plotf.c,v 1.23 2012/10/04 04:56:39 saito Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "ox.h"
#include "ifplot.h"

void ListCheck(char *,LIST);
void Pplot(NODE, Obj *);
void Ppolarplot(NODE, Obj *);
void Pobj_cp(NODE, Obj *),Parrayplot(NODE,Obj*),Pdrawcircle(NODE,Obj*);
void Pifplot(NODE,Obj *),Pconplot(NODE,Obj *),Pplotover(NODE,Obj *);
void Pmemory_ifplot(NODE, Obj *),Pmemory_conplot(NODE, Obj *);
void Pmemory_plot(NODE, Obj *);
void ifplot_main(NODE, int, int, Obj *);
void plot_main(NODE, int, int, Obj *);
void conplot_main(NODE, int, Obj *);

void Popen_canvas(NODE,Obj *),Pclear_canvas(NODE,Obj *),Pdraw_obj(NODE,Obj *);
void Pdraw_string(NODE,Obj *);
void Pox_rpc(), Pox_cmo_rpc();

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
void ifplot_mainNG(NODE,int,Obj *);
void ifplot_mainOP(NODE,int,Obj *);
void conplot_mainNG(NODE,int,Obj *);
void plotover_mainNG(NODE,int,Obj *);
//void PplotD(NODE, Obj *);

char *pfunc[]={
	"ifplot","conplot","plot","interactive","polarplot","plotover",
	"ifplotD","ifplotQ","ifplotB","ineqnD","ineqnQ","ineqnB",
	"ineqnandD","ineqnandQ","ineqnandB","ineqnorD","ineqnorQ","ineqnorB",
	"ineqnxorD","ineqnxorQ","ineqnxorB",
	"conplotD","conplotQ","conplotB","itvifplot",
	"plotoverD","plotoverQ","plotoverB"};
// end NG
	
struct ftab plot_tab[]={
	{"plot",Pplot,-6},
#if defined(INTERVAL)
	{"itvifplot",Pitvifplot,-8},
#endif
	{"polarplot",Ppolarplot,-7},
	{"obj_cp",Pobj_cp,4},
	{"ifplot",Pifplot,-7},
	{"ifplotD",PifplotD,-8},
	{"ifplotQ",PifplotQ,-8},
	{"ifplotB",PifplotB,-8},
	{"ineqn",PineqnD,-8},
	{"ineqnD",PineqnD,-8},
	{"ineqnQ",PineqnQ,-8},
	{"ineqnB",PineqnB,-8},
	{"ineqnand",PineqnandD,4},//ifplot_mainOP :argc is const
	{"ineqnandD",PineqnandD,4},
	{"ineqnandQ",PineqnandQ,4},
	{"ineqnandB",PineqnandB,4},
	{"ineqnor",PineqnorD,4},
	{"ineqnorD",PineqnorD,4},
	{"ineqnorQ",PineqnorQ,4},
	{"ineqnorB",PineqnorB,4},
	{"ineqnxor",PineqnxorD,4},
	{"ineqnxorD",PineqnxorD,4},
	{"ineqnxorQ",PineqnxorQ,4},
	{"ineqnxorB",PineqnxorB,4},
	{"conplot",Pconplot,-8},
	{"conplotD",PconplotD,-9},
	{"conplotB",PconplotB,-9},
	{"conplotQ",PconplotQ,-9},
	{"plotover",Pplotover,-5},
	{"plotoverD",PplotoverD,-5},
	{"plotoverQ",PplotoverQ,-5},
	{"plotoverB",PplotoverB,-5},
	{"memory_ifplot",Pmemory_ifplot,-6},
	{"memory_conplot",Pmemory_conplot,-7},
	{"memory_plot",Pmemory_plot,-5},
	{"drawcircle",Pdrawcircle,6},
	{"open_canvas",Popen_canvas,-3},
	{"clear_canvas",Pclear_canvas,2},
	{"draw_obj",Pdraw_obj,-4},
	{"draw_string",Pdraw_string,-5},
	{0,0,0},
};

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
	MKSTR(fname,"open_canvas");
	arg=mknode(4,s_id,fname,geom,wname);
	Pox_cmo_rpc(arg,rp);
	*rp=(Obj)s_id;
}

void Pifplot(NODE arg,Obj *rp){ifplot_main(arg,0,MODE_IFPLOT,rp);}
void Pmemory_ifplot(NODE arg,Obj *rp){ifplot_main(arg,1,MODE_IFPLOT,rp);}

void ifplot_main(NODE arg,int is_memory,int fn,Obj *rp){
	Q m2,p2,w300,s_id;
	NODE defrange;
	LIST xrange,yrange,range[2],list,geom;
	VL vl,vl0;
	V v[2],av[2];
	int stream,ri,i,sign;
	P poly,var;
	NODE n,n0;
	STRING fname,wname;
	Obj t;

	STOQ(-2,m2);STOQ(2,p2);MKNODE(n,p2,0);MKNODE(defrange,m2,n);
	poly=0;vl=0;geom=0;wname=0;stream=-1;ri=0;
	v[0]=v[1]=0;
	for(;arg;arg=NEXT(arg))
		if(!BDY(arg))stream=0;
		else
		switch(OID(BDY(arg))){
		case O_P:
			poly=(P)BDY(arg);
			get_vars_recursive((Obj)poly,&vl);
			for(vl0=vl,i=0;vl0;vl0=NEXT(vl0))
				if(vl0->v->attr==(pointer)V_IND)
					if(i>=2)error("ifplot : invalid argument");
					else v[i++]=vl0->v;
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
		MKSTR(fname,"memory_plot");
		arg=mknode(8,s_id,fname,poly,xrange,yrange,0,geom);
		Pox_rpc(arg,&t);
		arg=mknode(1,s_id);
		Pox_pop_cmo(arg,rp);
	} else {
		MKSTR(fname,pfunc[fn]);
		arg=mknode(8,s_id,fname,poly,xrange,yrange,0,geom,wname);
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
	MKSTR(fname,"objcp");
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
				for(vl0=vl,i=0;vl0;vl0=NEXT(vl0))
					if(vl0->v->attr==(pointer)V_IND)
						if(i>=2) error("conplot : invalid argument");
						else v[i++]=vl0->v;
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
		MKSTR(fname,"memory_plot");
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
		MKSTR(fname,pfunc[MODE_CONPLOT]);
		arg=mknode(8,s_id,fname,poly,xrange,yrange,zrange,geom,wname);
		Pox_rpc(arg,&t);
		*rp=(Obj)s_id;
	}
}

void Pplot(NODE arg,Obj *rp){plot_main(arg,0,MODE_PLOT,rp);}
void Pmemory_plot(NODE arg,Obj *rp){plot_main(arg,1,MODE_PLOT,rp);}

void plot_main(NODE arg,int is_memory,int fn,Obj *rp){
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
				for(vl0=vl, i=0;vl0;vl0=NEXT(vl0) )
					if(vl0->v->attr==(pointer)V_IND )
						if(i >= 1 )
							error("ifplot : invalid argument");
						else
							v[i++]=vl0->v;
				if(i != 1 )
					error("ifplot : invalid argument");
				break;
			case O_LIST:
				list=(LIST)BDY(arg);
				if(OID(BDY(BDY(list)))==O_P )
					if(ri > 0 )
						error("plot : invalid argument");
					else
						range[ri++]=list;
				else
					geom=list;
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
		MKSTR(fname,"memory_plot");
		arg=mknode(7,s_id,fname,poly,xrange,NULLP,NULLP,geom);
		Pox_rpc(arg,&t);
		arg=mknode(1,s_id);
		Pox_pop_cmo(arg,rp);
	} else {
		MKSTR(fname,pfunc[fn]);
		arg=mknode(8,s_id,fname,poly,xrange,NULLP,NULLP,geom,wname);
		Pox_rpc(arg,&t);
		*rp=(Obj)s_id;
	}
}

#define Pi 3.14159265358979323846264

void Ppolarplot(NODE arg,Obj *rp){
	Q m2,p2,w300,s_id;
	NODE defrange;
	LIST zrange,range[1],list,geom;
	VL vl,vl0;
	V v[1],av[1];
	int stream,ri,i;
	P poly;
	P var;
	NODE n,n0;
	STRING fname,wname;
	Real pi2;
	Obj t;

	MKReal(2*Pi,pi2);
	MKNODE(n,pi2,0);MKNODE(defrange,0,n);
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
				for(vl0=vl, i=0;vl0;vl0=NEXT(vl0) )
					if(vl0->v->attr==(pointer)V_IND )
						if(i >= 1 )
							error("polarplot : invalid argument");
						else
							v[i++]=vl0->v;
				if(i != 1 )
					error("polarplot : invalid argument");
				break;
			case O_LIST:
				list=(LIST)BDY(arg);
				if(OID(BDY(BDY(list)))==O_P )
					if(ri > 0 )
						error("polarplot : invalid argument");
					else
						range[ri++]=list;
				else
					geom=list;
				break;
			case O_N:
				stream=QTOS((Q)BDY(arg));break;
			case O_STR:
				wname=(STRING)BDY(arg);break;
			default:
				error("polarplot : invalid argument");break;
		}
	if(!poly )
		error("polarplot : invalid argument");
	switch ( ri ){
		case 0:
			MKV(v[0],var);MKNODE(n,var,defrange);MKLIST(zrange,n);
			break;
		case 1:
			av[0]=VR((P)BDY(BDY(range[0])));
			if(v[0]==av[0] )
				zrange=range[0];
			else
				error("polarplot : invalid argument");
			break;
		default:
			error("polarplot : cannot happen");break;
	}
	stream=validate_ox_plot_stream(stream);
	STOQ(stream,s_id);
	if(!geom){
		STOQ(300,w300);
		MKNODE(n0,w300,0);MKNODE(n,w300,n0);MKLIST(geom,n);
	}
	MKSTR(fname,pfunc[MODE_POLARPLOT]);
	arg=mknode(8,s_id,fname,poly,NULLP,NULLP,zrange,geom,wname);
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
	MKSTR(fname,"plotover");
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
	MKSTR(fname,"drawcircle");
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

	if(!fname)MKSTR(fname,"draw_obj");
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

	if(!fname)MKSTR(fname,"draw_string");
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

	if(!fname) MKSTR(fname,"clear_canvas");
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
	printf("%s\n",head);
	if(!list){
		printf("zero list\n");
		return;
	}
	for(i=0,n=(NODE)BDY(list);n;i++,n=NEXT(n)){
		if(!BDY(n))printf("%d 0\n",i);
		else if(OID(BDY(n))==O_P) printf("%d poly\n",i);
		else printf("%d %d\n",i,QTOS((Q)BDY(n)));
	}
}

void PifplotD(NODE arg,Obj *rp){ifplot_mainNG(arg,MODE_IFPLOTD,rp);}
void PifplotQ(NODE arg,Obj *rp){ifplot_mainNG(arg,MODE_IFPLOTQ,rp);}
void PifplotB(NODE arg,Obj *rp){ifplot_mainNG(arg,MODE_IFPLOTB,rp);}

void PconplotD(NODE arg,Obj *rp){ifplot_mainNG(arg,MODE_CONPLOTD,rp);}
void PconplotQ(NODE arg,Obj *rp){ifplot_mainNG(arg,MODE_CONPLOTQ,rp);}
void PconplotB(NODE arg,Obj *rp){ifplot_mainNG(arg,MODE_CONPLOTB,rp);}

void PineqnD(NODE arg,Obj *rp){ifplot_mainNG(arg,MODE_INEQND,rp);}
void PineqnQ(NODE arg,Obj *rp){ifplot_mainNG(arg,MODE_INEQNQ,rp);}
void PineqnB(NODE arg,Obj *rp){ifplot_mainNG(arg,MODE_INEQNB,rp);}

#if defined(INTERVAL)
void Pitvifplot(NODE arg,Obj *rp){ifplot_mainNG(arg,MODE_ITVIFPLOT,rp);}
#endif

void PineqnorD(NODE arg,Obj *rp){ifplot_mainOP(arg,MODE_INEQNORD,rp);}
void PineqnorQ(NODE arg,Obj *rp){ifplot_mainOP(arg,MODE_INEQNORQ,rp);}
void PineqnorB(NODE arg,Obj *rp){ifplot_mainOP(arg,MODE_INEQNORB,rp);}

void PineqnandD(NODE arg,Obj *rp){ifplot_mainOP(arg,MODE_INEQNANDD,rp);}
void PineqnandQ(NODE arg,Obj *rp){ifplot_mainOP(arg,MODE_INEQNANDQ,rp);}
void PineqnandB(NODE arg,Obj *rp){ifplot_mainOP(arg,MODE_INEQNANDB,rp);}

void PineqnxorD(NODE arg,Obj *rp){ifplot_mainOP(arg,MODE_INEQNXORD,rp);}
void PineqnxorQ(NODE arg,Obj *rp){ifplot_mainOP(arg,MODE_INEQNXORQ,rp);}
void PineqnxorB(NODE arg,Obj *rp){ifplot_mainOP(arg,MODE_INEQNXORB,rp);}

void PplotoverD(NODE arg,Obj *rp){plotover_mainNG(arg,MODE_PLOTOVERD,rp);}
void PplotoverQ(NODE arg,Obj *rp){plotover_mainNG(arg,MODE_PLOTOVERQ,rp);}
void PplotoverB(NODE arg,Obj *rp){plotover_mainNG(arg,MODE_PLOTOVERB,rp);}

void ifplot_mainNG(NODE arg,int fn,Obj *rp){
	Q m2,p2,w300,mxgc,s_id,color,idv;
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
	poly=0;stream=-1;wname=0;color=0;idv=0;stream=0;
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
				sprintf(ebuf,"%s : to many ploy arguments",pfunc[fn]);
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
			case 2: //division arg
				if(fn==MODE_ITVIFPLOT){//itvifplot
					idv=(Q)BDY(arg);
					iNo++;
				} else {//others
					sprintf(ebuf,"%s : invalid number arguments",pfunc[fn]);
					error(ebuf);
				}
				break;
			case 3:// error
				sprintf(ebuf,"%s : invalid number arguments",pfunc[fn]);
				error(ebuf);
				break;
			}
			break;
		case O_LIST://xrange,yrange,zrange,geometry
			if(lNo<4) list[lNo++]=(LIST)BDY(arg);
			else {
				sprintf(ebuf,"%s : invalid list argument",pfunc[fn]);
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
		sprintf(ebuf,"%s : invalid ploy argument",pfunc[fn]);
		error(ebuf);
	}
	// vars check
	get_vars_recursive((Obj)poly,&vl);
	for(vl0=vl,vNo=0;vl0;vl0=NEXT(vl0))
		if(vl0->v->attr==(pointer)V_IND)
			if(vNo>=2){
				sprintf(ebuf,"%s : invalid ploy argument",pfunc[fn]);
				error(ebuf);
			} else v[vNo++]=vl0->v;
	//list check
	xrange=yrange=zrange=geom=0;frst=sec=thr=0;
	for(i=0;i<lNo;i++){
		for(llen=0,n=(NODE)BDY(list[i]);n;llen++,n=NEXT(n));
		if(llen>4){
			sprintf(ebuf,"%s : invalid list length",pfunc[fn]);
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
				if(!zrange){
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
				sprintf(ebuf,"%s : invalid list length",pfunc[fn]);
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
	if(fn==MODE_CONPLOTD||fn==MODE_CONPLOTQ||fn==MODE_CONPLOTB) if(!zrange){
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
		itvstep (Q) if MODE_ITVIFPLOT */
	stream=validate_ox_plot_stream(stream);
	STOQ(stream,s_id);
	MKSTR(fname,pfunc[fn]);
/*
printf("%s\n",pfunc[fn]);
ListCheck("xrange",xrange);
ListCheck("yrange",yrange);
ListCheck("zrange",zrange);
ListCheck("geom",geom);
*/
	if(fn==MODE_ITVIFPLOT)
		arg=mknode(10,s_id,fname,poly,color,xrange,yrange,zrange,geom,wname,idv);
	else arg=mknode(9,s_id,fname,poly,color,xrange,yrange,zrange,geom,wname);
	Pox_rpc(arg,&t);
	*rp=(Obj)s_id;
}

void ifplot_mainOP(NODE arg,int fn,Obj *rp){
	/*
		new ineqnXX in ox_plot requires 
		[s_id (Q),
		w_id (Q),
		formula (Obj),
		color (Q)]
	*/
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
				sprintf(ebuf,"%s : to many ploy arguments",pfunc[fn]);
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
				sprintf(ebuf,"%s : to many numbers",pfunc[fn]);
				error(ebuf);
				break;
			}
			break;
		default:
			sprintf(ebuf,"%s : arguments type miss match",pfunc[fn]);
			error(ebuf);
		}
	}
	MKSTR(fname,pfunc[fn]);
/*
printf("fname %s\n",BDY(fname));
printf("s_id %d\n",QTOS((Q)s_id));
printf("w_id %d\n",QTOS((Q)w_id));
printf("color %d\n",QTOS((Q)color));
*/
	arg=mknode(5,s_id,fname,w_id,poly,color);
	Pox_rpc(arg,&t);
	*rp=(Obj)s_id;
}

void plotover_mainNG(NODE arg,int fn,Obj *rp){
	/*
		[s_id (Q),
		w_id (Q),
		formula (Obj),
		color (Q)]
	*/
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
				sprintf(ebuf,"%s : to many ploy arguments",pfunc[fn]);
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
				sprintf(ebuf,"%s : to many numbers",pfunc[fn]);
				error(ebuf);
				break;
			}
			break;
		default:
			sprintf(ebuf,"%s : arguments type miss match",pfunc[fn]);
			error(ebuf);
		}
	}
	MKSTR(fname,pfunc[fn]);
/*
printf("fname %s\n",BDY(fname));
printf("s_id %d\n",QTOS((Q)s_id));
printf("w_id %d\n",QTOS((Q)w_id));
printf("color %d\n",QTOS((Q)color));
*/
	arg=mknode(5,s_id,fname,w_id,poly,color);
	Pox_rpc(arg,&t);
	*rp=(Obj)s_id;
}
