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
 * $OpenXM: OpenXM_contrib2/asir2000/plot/plotf.c,v 1.6 2000/11/07 06:06:40 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "ox.h"
#include "ifplot.h"

void Pifplot(), Pconplot(), Pplotover(), Pplot(), Parrayplot(), Pdrawcircle();
void Popen_canvas(), Pclear_canvas(), Pdraw_obj();

struct ftab plot_tab[] = {
	{"ifplot",Pifplot,-7},
	{"conplot",Pconplot,-8},
	{"plot",Pplot,-6},
	{"plotover",Pplotover,3},
	{"drawcircle",Pdrawcircle,5},
	{"open_canvas",Popen_canvas,-3},
	{"clear_canvas",Pclear_canvas,2},
	{"draw_obj",Pdraw_obj,-4},
/*
	{"arrayplot",Parrayplot,2},
*/
	{0,0,0},
};

int current_s;

void Popen_canvas(arg,rp)
NODE arg;
Obj *rp;
{
	Q w300,sid;
	LIST geom;
	int stream,id,i;
	NODE n,n0;
	STRING fname,wname;

	geom = 0; wname = 0; stream = -1;
	for ( ; arg; arg = NEXT(arg) )
		if ( !BDY(arg) )
			stream = 0;
		else
		switch ( OID(BDY(arg)) ) {
			case O_LIST:
				geom = (LIST)BDY(arg);
				break;
			case O_N:
				stream = QTOS((Q)BDY(arg)); break;
			case O_STR:
				wname = (STRING)BDY(arg); break;
			default:
				error("open_canvas : invalid argument"); break;
		}
	/* open_canvas in ox_plot requires 
	   [sid (Q),
	   	geom=[xsize,ysize] (LIST),
	   	wname=name (STRING)]
	*/

	if ( stream < 0 )
		stream = current_s;
	else
		current_s = stream;
	STOQ(stream,sid);
	if ( !geom ) {
		STOQ(300,w300);
		MKNODE(n0,w300,0); MKNODE(n,w300,n0); MKLIST(geom,n);
	}
	MKSTR(fname,"open_canvas");
	arg = mknode(4,sid,fname,geom,wname);
	Pox_cmo_rpc(arg,rp);
}

void Pifplot(arg,rp)
NODE arg;
Obj *rp;
{
	Q m2,p2,w300,sid;
	NODE defrange;
	LIST xrange,yrange,range[2],list,geom;
	VL vl,vl0;
	V v[2],av[2];
	int stream,ri,id,i;
	P poly;
	P var;
	NODE n,n0;
	STRING fname,wname;

	STOQ(-2,m2); STOQ(2,p2);
	MKNODE(n,p2,0); MKNODE(defrange,m2,n); 
	poly = 0; vl = 0; geom = 0; wname = 0; stream = -1; ri = 0;
	for ( ; arg; arg = NEXT(arg) )
		if ( !BDY(arg) )
			stream = 0;
		else
		switch ( OID(BDY(arg)) ) {
			case O_P:
				poly = (P)BDY(arg); 
				get_vars_recursive(poly,&vl);
				for ( vl0 = vl, i = 0; vl0; vl0 = NEXT(vl0) )
					if ( vl0->v->attr == V_IND )
						if ( i >= 2 )
							error("ifplot : invalid argument");
						else
							v[i++] = vl0->v;
				break;
			case O_LIST:
				list = (LIST)BDY(arg);
				if ( OID(BDY(BDY(list))) == O_P )
					if ( ri > 1 )
						error("ifplot : invalid argument");
					else
						range[ri++] = list;
				else
					geom = list;
				break;
			case O_N:
				stream = QTOS((Q)BDY(arg)); break;
			case O_STR:
				wname = (STRING)BDY(arg); break;
			default:
				error("ifplot : invalid argument"); break;
		}
	if ( !poly )
		error("ifplot : invalid argument");
	switch ( ri ) {
		case 0:
			if ( !v[1] )
				error("ifplot : please specify all variables");
			MKV(v[0],var); MKNODE(n,var,defrange); MKLIST(xrange,n);
			MKV(v[1],var); MKNODE(n,var,defrange); MKLIST(yrange,n);
			break;
		case 1:
			if ( !v[1] )
				error("ifplot : please specify all variables");
			av[0] = VR((P)BDY(BDY(range[0])));
			if ( v[0] == av[0] ) {
				xrange = range[0];
				MKV(v[1],var); MKNODE(n,var,defrange); MKLIST(yrange,n);
			} else if ( v[1] == av[0] ) {
				MKV(v[0],var); MKNODE(n,var,defrange); MKLIST(xrange,n);
				yrange = range[0];
			} else
				error("ifplot : invalid argument");
			break;
		case 2:
			av[0] = VR((P)BDY(BDY(range[0])));
			av[1] = VR((P)BDY(BDY(range[1])));
			if ( ((v[0] == av[0]) && (!v[1] || v[1] == av[1])) ||
				 ((v[0] == av[1]) && (!v[1] || v[1] == av[0])) ) {
					xrange = range[0]; yrange = range[1];
			} else
					error("ifplot : invalid argument");
			break;
		default:
			error("ifplot : cannot happen"); break;
	}
	/* ifplot in ox_plot requires 
	   [sid (Q),
	   	formula (Obj),
	   	xrange=[x,xmin,xmax] (LIST),
	   	yrange=[y,ymin,ymax] (LIST),
	   	zrange=0,
	   	geom=[xsize,ysize] (LIST),
	   	wname=name (STRING)]
	*/

	if ( stream < 0 )
		stream = current_s;
	else
		current_s = stream;
	STOQ(stream,sid);
	if ( !geom ) {
		STOQ(300,w300);
		MKNODE(n0,w300,0); MKNODE(n,w300,n0); MKLIST(geom,n);
	}
	MKSTR(fname,"plot");
	arg = mknode(8,sid,fname,poly,xrange,yrange,0,geom,wname);
	Pox_rpc(arg,rp);
}

void Pconplot(arg,rp)
NODE arg;
Obj *rp;
{
	Q m2,p2,w300,sid;
	NODE defrange;
	LIST xrange,yrange,zrange,range[3],list,geom;
	VL vl,vl0;
	V v[2],av[2];
	int stream,ri,id,i;
	P poly;
	P var;
	NODE n,n0;
	STRING fname,wname;

	STOQ(-2,m2); STOQ(2,p2);
	MKNODE(n,p2,0); MKNODE(defrange,m2,n); 
	poly = 0; vl = 0; geom = 0; wname = 0; stream = -1; ri = 0;
	for ( ; arg; arg = NEXT(arg) )
		if ( !BDY(arg) )
			stream = 0;
		else
		switch ( OID(BDY(arg)) ) {
			case O_P:
				poly = (P)BDY(arg);
				get_vars_recursive(poly,&vl);
				for ( vl0 = vl, i = 0; vl0; vl0 = NEXT(vl0) )
					if ( vl0->v->attr == V_IND )
						if ( i >= 2 )
							error("ifplot : invalid argument");
						else
							v[i++] = vl0->v;
				break;
			case O_LIST:
				list = (LIST)BDY(arg);
				if ( OID(BDY(BDY(list))) == O_P )
					if ( ri > 2 )
						error("ifplot : invalid argument");
					else
						range[ri++] = list;
				else
					geom = list;
				break;
			case O_N:
				stream = QTOS((Q)BDY(arg)); break;
			case O_STR:
				wname = (STRING)BDY(arg); break;
			default:
				error("ifplot : invalid argument"); break;
		}
	if ( !poly )
		error("ifplot : invalid argument");
	switch ( ri ) {
		case 0:
			if ( !v[1] )
				error("ifplot : please specify all variables");
			MKV(v[0],var); MKNODE(n,var,defrange); MKLIST(xrange,n);
			MKV(v[1],var); MKNODE(n,var,defrange); MKLIST(yrange,n);
			MKNODE(n,0,defrange); MKLIST(zrange,n);
			break;
		case 1:
			if ( !v[1] )
				error("ifplot : please specify all variables");
			av[0] = VR((P)BDY(BDY(range[0])));
			if ( v[0] == av[0] ) {
				xrange = range[0];
				MKV(v[1],var); MKNODE(n,var,defrange); MKLIST(yrange,n);
				MKNODE(n,0,defrange); MKLIST(zrange,n);
			} else if ( v[1] == av[0] ) {
				MKV(v[0],var); MKNODE(n,var,defrange); MKLIST(xrange,n);
				yrange = range[0];
				MKNODE(n,0,defrange); MKLIST(zrange,n);
			} else {
				MKV(v[0],var); MKNODE(n,var,defrange); MKLIST(xrange,n);
				MKV(v[1],var); MKNODE(n,var,defrange); MKLIST(yrange,n);
				zrange = range[0];
			}
			break;
		case 2: case 3:
			av[0] = VR((P)BDY(BDY(range[0])));
			av[1] = VR((P)BDY(BDY(range[1])));
			if ( ((v[0] == av[0]) && (!v[1] || v[1] == av[1])) ||
				 ((v[0] == av[1]) && (!v[1] || v[1] == av[0])) ) {
				xrange = range[0]; yrange = range[1];
				if ( ri == 3 )
					zrange = range[2];
				else {
					MKNODE(n,0,defrange); MKLIST(zrange,n);
				}
			} else
				error("ifplot : invalid argument");
			break;
		default:
			error("ifplot : cannot happen"); break;
	}
	if ( stream < 0 )
		stream = current_s;
	else
		current_s = stream;

	/* conplot in ox_plot requires 
	   [sid (Q),
	   	formula (Obj),
	   	xrange=[x,xmin,xmax] (LIST),
	   	yrange=[y,ymin,ymax] (LIST),
	   	zrange=[z,zmin,zmax] (LIST),
	   	geom=[xsize,ysize] (LIST),
	   	wname=name (STRING)]
	*/

	if ( stream < 0 )
		stream = current_s;
	else
		current_s = stream;
	STOQ(stream,sid);
	if ( !geom ) {
		STOQ(300,w300);
		MKNODE(n0,w300,0); MKNODE(n,w300,n0); MKLIST(geom,n);
	}
	MKSTR(fname,"plot");
	arg = mknode(8,sid,fname,poly,xrange,yrange,zrange,geom,wname);
	Pox_rpc(arg,rp);
}

void Pplot(arg,rp)
NODE arg;
Obj *rp;
{
	Q m2,p2,w300,sid;
	NODE defrange;
	LIST xrange,range[1],list,geom;
	VL vl,vl0;
	V v[1],av[1];
	int stream,ri,id,i;
	P poly;
	P var;
	NODE n,n0;
	STRING fname,wname;

	STOQ(-2,m2); STOQ(2,p2);
	MKNODE(n,p2,0); MKNODE(defrange,m2,n); 
	poly = 0; vl = 0; geom = 0; wname = 0; stream = -1; ri = 0;
	for ( ; arg; arg = NEXT(arg) )
		if ( !BDY(arg) )
			stream = 0;
		else
		switch ( OID(BDY(arg)) ) {
			case O_P: case O_R:
				poly = (P)BDY(arg);
				get_vars_recursive(poly,&vl);
				for ( vl0 = vl, i = 0; vl0; vl0 = NEXT(vl0) )
					if ( vl0->v->attr == V_IND )
						if ( i >= 1 )
							error("ifplot : invalid argument");
						else
							v[i++] = vl0->v;
				break;
			case O_LIST:
				list = (LIST)BDY(arg);
				if ( OID(BDY(BDY(list))) == O_P )
					if ( ri > 0 )
						error("plot : invalid argument");
					else
						range[ri++] = list;
				else
					geom = list;
				break;
			case O_N:
				stream = QTOS((Q)BDY(arg)); break;
			case O_STR:
				wname = (STRING)BDY(arg); break;
			default:
				error("plot : invalid argument"); break;
		}
	if ( !poly )
		error("plot : invalid argument");
	switch ( ri ) {
		case 0:
			MKV(v[0],var); MKNODE(n,var,defrange); MKLIST(xrange,n);
			break;
		case 1:
			av[0] = VR((P)BDY(BDY(range[0])));
			if ( v[0] == av[0] )
				xrange = range[0];
			else
				error("plot : invalid argument");
			break;
		default:
			error("plot : cannot happen"); break;
	}
	/* conplot in ox_plot requires 
	   [sid (Q),
	   	formula (Obj),
	   	xrange=[x,xmin,xmax] (LIST),
	   	yrange=0,
	   	zrange=0,
	   	geom=[xsize,ysize] (LIST),
	   	wname=name (STRING)]
	*/
	if ( stream < 0 )
		stream = current_s;
	else
		current_s = stream;
	STOQ(stream,sid);
	if ( !geom ) {
		STOQ(300,w300);
		MKNODE(n0,w300,0); MKNODE(n,w300,n0); MKLIST(geom,n);
	}
	MKSTR(fname,"plot");
	arg = mknode(8,sid,fname,poly,xrange,0,0,geom,wname);
	Pox_rpc(arg,rp);
}

void Pplotover(arg,rp)
NODE arg;
Obj *rp;
{
	Q sid,index;
	P poly;
	STRING fname;

	poly = (P)ARG0(arg);
	sid = (Q)ARG1(arg);
	index = (Q)ARG2(arg);
	MKSTR(fname,"plotover");
	arg = mknode(4,sid,fname,index,poly);
	Pox_rpc(arg,rp);
}

/* arg = [x,y,r,sid,index] */

void Pdrawcircle(arg,rp)
NODE arg;
Obj *rp;
{
	Q sid,index;
	Obj x,y,r;
	STRING fname;
	NODE n;
	LIST pos;

	x = (Obj)ARG0(arg);
	y = (Obj)ARG1(arg);
	r = (Obj)ARG2(arg);
	sid = (Q)ARG3(arg);
	index = (Q)ARG4(arg);
	MKSTR(fname,"drawcircle");
	n = mknode(3,x,y,r); MKLIST(pos,n);
	arg = mknode(4,sid,fname,index,pos);
	Pox_rpc(arg,rp);
}

/* draw_obj(sid,cindex,point|line); point = [x,y], line = [xa,ya,xb,yb] */
void Pdraw_obj(arg,rp)
NODE arg;
Obj *rp;
{
	static STRING fname;
	Q sid,index;
	LIST obj;

	if ( !fname ) {
		MKSTR(fname,"draw_obj");
	}
	sid = (Q)ARG0(arg);
	index = (Q)ARG1(arg);
	obj = (LIST)ARG2(arg);
	/* ARG3(arg) = color */
	if ( argc(arg) == 4 )
		arg = mknode(5,sid,fname,index,obj,ARG3(arg));
	else
		arg = mknode(4,sid,fname,index,obj);
	Pox_cmo_rpc(arg,rp);
}

void Pclear_canvas(arg,rp)
NODE arg;
Obj *rp;
{
	static STRING fname;
	Q sid,index;
	LIST obj;

	if ( !fname ) {
		MKSTR(fname,"clear_canvas");
	}
	sid = (Q)ARG0(arg);
	index = (Q)ARG1(arg);
	arg = mknode(3,sid,fname,index);
	Pox_cmo_rpc(arg,rp);
}

#if 0
void Parrayplot(arg,rp)
NODE arg;
Obj *rp;
{
	int s;
	int id;

	if ( ID((Obj)ARG0(arg)) == O_VECT && ID((Obj)ARG1(arg)) == O_LIST ) {
		s = current_s;
		gensend(s,C_APLOT,0);
		gensend(s,C_OBJ,ARG0(arg)); gensend(s,C_OBJ,ARG1(arg)); genflush(s);
	}
	genrecv(s,&id,rp);
}
#endif
