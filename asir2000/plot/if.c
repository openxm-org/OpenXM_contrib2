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
 * e-mail at risa-admin@flab.fujitsu.co.jp of the detailed specification
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
 * $OpenXM: OpenXM_contrib2/asir2000/plot/if.c,v 1.1.1.1 1999/12/03 07:39:13 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "ox.h"
#include "ifplot.h"

extern jmp_buf ox_env;

int plot(NODE arg)
{
	int id;
	NODE n;
	struct canvas *can;
	P formula;
	LIST xrange,yrange,zrange,wsize;
	STRING wname;

	formula = (P)ARG0(arg);
	xrange = (LIST)ARG1(arg);
	yrange = (LIST)ARG2(arg);
	zrange = (LIST)ARG3(arg);
	wsize = (LIST)ARG4(arg);
	wname = (STRING)ARG5(arg);

	can = canvas[id = search_canvas()];
	n = BDY(xrange); can->vx = VR((P)BDY(n)); n = NEXT(n);
	can->qxmin = (Q)BDY(n); n = NEXT(n); can->qxmax = (Q)BDY(n);
	can->xmin = ToReal(can->qxmin); can->xmax = ToReal(can->qxmax); 
	if ( yrange ) {
		n = BDY(yrange); can->vy = VR((P)BDY(n)); n = NEXT(n);
		can->qymin = (Q)BDY(n); n = NEXT(n); can->qymax = (Q)BDY(n);
		can->ymin = ToReal(can->qymin); can->ymax = ToReal(can->qymax); 
		if ( zrange ) {
			n = NEXT(BDY(zrange));
			can->zmin = ToReal(BDY(n)); n = NEXT(n); can->zmax = ToReal(BDY(n));
			if ( n = NEXT(n) )
				can->nzstep = QTOS((Q)BDY(n));
			else
				can->nzstep = MAXGC;
			can->mode = MODE_CONPLOT;
		} else
			can->mode = MODE_IFPLOT;
	} else
		can->mode = MODE_PLOT;
	if ( !wsize ) {
		can->width = DEFAULTWIDTH; can->height = DEFAULTHEIGHT;
	} else {
		can->width = QTOS((Q)BDY(BDY(wsize)));
		can->height = QTOS((Q)BDY(NEXT(BDY(wsize))));
	}
	if ( wname )
		can->wname = BDY(wname);
	else
		can->wname = "";
	can->formula = formula; 
	create_canvas(can);
	if ( can->mode == MODE_PLOT ) {
		plotcalc(can);
		plot_print(display,can);
	} else
		ifplotmain(can);
	copy_to_canvas(can);
	return id;
}

int plotover(NODE arg)
{
	int index;
	P formula;
	struct canvas *can;
	struct canvas fakecan;
	VL vl,vl0;

	index = QTOS((Q)ARG0(arg));
	formula = (P)ARG1(arg);
	can = canvas[index];
	if ( !can->window )
		return -1;
	get_vars_recursive(formula,&vl);
	for ( vl0 = vl; vl0; vl0 = NEXT(vl0) )
		if ( vl0->v->attr == V_IND )
			if ( vl->v != can->vx && vl->v != can->vy )
				return -1;
	current_can = can;
	fakecan = *can; fakecan.formula = formula;
	if ( can->mode == MODE_PLOT ) {
		plotcalc(&fakecan);
		plot_print(display,&fakecan);
	} else
		ifplotmain(&fakecan);
	copy_to_canvas(&fakecan);
	return index;
}

int drawcircle(NODE arg)
{
	int id;
	int index;
	pointer ptr;
	Q ret;
	LIST xyr;
	Obj x,y,r;
	int wx,wy,wr;
	struct canvas *can;
	struct canvas fakecan;

	index = QTOS((Q)ARG0(arg));
	xyr = (LIST)ARG1(arg); 
	x = (Obj)ARG0(BDY(xyr)); y = (Obj)ARG1(BDY(xyr)); r = (Obj)ARG2(BDY(xyr));
	can = canvas[index];
	if ( !can->window )
		return -1;
	else {
		current_can = can;
		wx = (ToReal(x)-can->xmin)*can->width/(can->xmax-can->xmin);
		wy = (can->ymax-ToReal(y))*can->height/(can->ymax-can->ymin);
		wr = ToReal(r);
		XFillArc(display,can->pix,colorGC,wx-wr/2,wy-wr/2,wr,wr,0,360*64);
		copy_to_canvas(can);
		return index;
	}
}

#define RealtoDbl(r) ((r)?BDY(r):0.0)

int arrayplot(NODE arg)
{
	int id,ix,w,h;
	VECT array;
	LIST xrange,wsize;
	char *wname;
	NODE n;
	Q ret;
	double ymax,ymin,dy,xstep;
	Real *tab;
	struct canvas *can;
	POINT *pa;

	array = (VECT)ARG0(arg);
	xrange = (LIST)ARG1(arg);
	can = canvas[id = search_canvas()];
	n = BDY(xrange); can->vx = VR((P)BDY(n)); n = NEXT(n);
	can->qxmin = (Q)BDY(n); n = NEXT(n); can->qxmax = (Q)BDY(n);
	can->xmin = ToReal(can->qxmin); can->xmax = ToReal(can->qxmax); 
	if ( !wsize ) {
		can->width = DEFAULTWIDTH; can->height = DEFAULTHEIGHT;
	} else {
		can->width = QTOS((Q)BDY(BDY(wsize)));
		can->height = QTOS((Q)BDY(NEXT(BDY(wsize))));
	}
	can->wname = wname; can->formula = 0; can->mode = MODE_PLOT;
	create_canvas(can);
	w = array->len;
	h = can->height;
	tab = (Real *)BDY(array);
	if ( can->ymax == can->ymin ) {
		for ( ymax = ymin = RealtoDbl(tab[0]), ix = 1; ix < w; ix++ ) {
			if ( RealtoDbl(tab[ix]) > ymax )
				ymax = RealtoDbl(tab[ix]);
			if ( RealtoDbl(tab[ix]) < ymin )
				ymin = RealtoDbl(tab[ix]);
		}
		can->ymax = ymax; can->ymin = ymin;
	} else {
		ymax = can->ymax; ymin = can->ymin;
	}
	dy = ymax-ymin;
	can->pa = (struct pa *)MALLOC(sizeof(struct pa));
	can->pa[0].length = w;
	can->pa[0].pos = pa = (POINT *)MALLOC(w*sizeof(POINT));
	xstep = (double)can->width/(double)(w-1);
	for ( ix = 0; ix < w; ix++ ) {
#ifndef MAXSHORT
#define MAXSHORT ((short)0x7fff)
#endif
		double t;

		pa[ix].x = (int)(ix*xstep); 
		t = (h - 1)*(ymax - RealtoDbl(tab[ix]))/dy;
		if ( t > MAXSHORT )
			pa[ix].y = MAXSHORT;
		else if ( t < -MAXSHORT )
			pa[ix].y = -MAXSHORT;
		else
			pa[ix].y = t;
	}
	plot_print(display,can);
	copy_to_canvas(can);
	return id;
}

ifplot_resize(can,spos,epos)
struct canvas *can;
POINT spos,epos;
{
	struct canvas *ncan;
	struct canvas fakecan;
	Q dx,dy,dx2,dy2,xmin,xmax,ymin,ymax,xmid,ymid;
	Q sx,sy,ex,ey,cw,ch,ten,two;
	Q s,t;
	int new;
	int w,h,m;
	Q ret;

	if ( XC(spos) < XC(epos) && YC(spos) < YC(epos) ) {
		if ( can->precise && !can->wide ) {
			fakecan = *can; ncan = &fakecan;
		} else {
			new = search_canvas(); ncan = canvas[new];
		}
		ncan->mode = can->mode;
		ncan->zmin = can->zmin; ncan->zmax = can->zmax;
		ncan->nzstep = can->nzstep;
		ncan->wname = can->wname;
		ncan->vx = can->vx; ncan->vy = can->vy;
		ncan->formula = can->formula;
		w = XC(epos)-XC(spos);
		h = YC(epos)-YC(spos);
		m = MAX(can->width,can->height);
		if ( can->precise ) {
			ncan->width = w; ncan->height = h;
		} else if ( w > h ) {
			ncan->width = m; ncan->height = m * h/w;
		} else {
			ncan->width = m * w/h; ncan->height = m;
		}
		if ( can->wide ) {
			STOQ(10,ten); STOQ(2,two);
			subq(can->qxmax,can->qxmin,&t); mulq(t,ten,&dx);
			subq(can->qymax,can->qymin,&t); mulq(t,ten,&dy);
			addq(can->qxmax,can->qxmin,&t); divq(t,two,&xmid);
			addq(can->qymax,can->qymin,&t); divq(t,two,&ymid);
			divq(dx,two,&dx2); divq(dy,two,&dy2);
			subq(xmid,dx2,&xmin); addq(xmid,dx2,&xmax);
			subq(ymid,dy2,&ymin); addq(ymid,dy2,&ymax);
		} else {
			subq(can->qxmax,can->qxmin,&dx); subq(can->qymax,can->qymin,&dy);
			xmin = can->qxmin; xmax = can->qxmax; 
			ymin = can->qymin; ymax = can->qymax;
		}
		STOQ(XC(spos),sx); STOQ(YC(spos),sy); STOQ(XC(epos),ex); STOQ(YC(epos),ey);
		STOQ(can->width,cw); STOQ(can->height,ch);
		mulq(sx,dx,&t); divq(t,cw,&s); addq(xmin,s,&ncan->qxmin);
		mulq(ex,dx,&t); divq(t,cw,&s); addq(xmin,s,&ncan->qxmax);
		mulq(ey,dy,&t); divq(t,ch,&s); subq(ymax,s,&ncan->qymin);
		mulq(sy,dy,&t); divq(t,ch,&s); subq(ymax,s,&ncan->qymax);
		ncan->xmin = ToReal(ncan->qxmin); ncan->xmax = ToReal(ncan->qxmax);
		ncan->ymin = ToReal(ncan->qymin); ncan->ymax = ToReal(ncan->qymax);
		if ( can->precise && !can->wide ) {
			current_can = can;
			alloc_pixmap(ncan);
			qifplotmain(ncan);
			copy_subimage(ncan,can,spos);
			copy_to_canvas(can);
		} else {
			create_canvas(ncan);
			if ( can->precise )
				qifplotmain(ncan);
			else
				ifplotmain(ncan);
			copy_to_canvas(ncan);
		}
	}
}

plot_resize(can,spos,epos)
struct canvas *can;
POINT spos,epos;
{
	struct canvas *ncan;
	Q dx,dx2,xmin,xmax,xmid;
	double dy,dy2,ymin,ymax,ymid;
	Q sx,ex,cw,ten,two;
	double sy,ey;
	Q s,t;
	int new;
	int w,h,m;

	if ( XC(spos) < XC(epos) && YC(spos) < YC(epos) ) {
		new = search_canvas(); ncan = canvas[new];
		ncan->mode = can->mode;
		ncan->zmin = can->zmin; ncan->zmax = can->zmax;
		ncan->nzstep = can->nzstep;
		ncan->wname = can->wname;
		ncan->vx = can->vx; ncan->vy = can->vy;
		ncan->formula = can->formula;
		w = XC(epos)-XC(spos);
		h = YC(epos)-YC(spos);
		m = MAX(can->width,can->height);
		if ( w > h ) {
			ncan->width = m; ncan->height = m * h/w;
		} else {
			ncan->width = m * w/h; ncan->height = m;
		}
		if ( can->wide ) {
			STOQ(10,ten); STOQ(2,two);
			subq(can->qxmax,can->qxmin,&t); mulq(t,ten,&dx);
			addq(can->qxmax,can->qxmin,&t); divq(t,two,&xmid);
			divq(dx,two,&dx2); subq(xmid,dx2,&xmin); addq(xmid,dx2,&xmax);

			dy = (can->ymax-can->ymin)*10;
			ymid = (can->ymax+can->ymin)/2;
			ymin = ymid-dy/2; ymax = ymid+dy/2;
		} else {
			subq(can->qxmax,can->qxmin,&dx); 
			xmin = can->qxmin; xmax = can->qxmax; 

			dy = can->ymax-can->ymin;
			ymin = can->ymin; ymax = can->ymax;
		}
		STOQ(XC(spos),sx); STOQ(XC(epos),ex); STOQ(can->width,cw); 
		mulq(sx,dx,&t); divq(t,cw,&s); addq(xmin,s,&ncan->qxmin);
		mulq(ex,dx,&t); divq(t,cw,&s); addq(xmin,s,&ncan->qxmax);
		ncan->xmin = ToReal(ncan->qxmin); ncan->xmax = ToReal(ncan->qxmax);

		ncan->ymin = ymax-YC(epos)*dy/can->height;
		ncan->ymax = ymax-YC(spos)*dy/can->height;

		create_canvas(ncan); 
		plotcalc(ncan);
		plot_print(display,ncan);
		copy_to_canvas(ncan);
	}
}

ifplotmain(can)
struct canvas *can;
{
	int width,height;
	double **tabe,*tabeb;
	int i;

	width = can->width; height = can->height;
	tabe = (double **)ALLOCA(width*sizeof(double *));
	for ( i = 0; i < width; i++ )
		tabe[i] = (double *)ALLOCA(height*sizeof(double));
	define_cursor(can->window,runningcur);
	set_busy(can); set_selection();
	calc(tabe,can); if_print(display,tabe,can);
	reset_selection(); reset_busy(can);
	define_cursor(can->window,normalcur);
}

qifplotmain(can)
struct canvas *can;
{
	int width,height;
	char **tabe,*tabeb;
	int i;

	width = can->width; height = can->height;
	tabe = (char **)ALLOCA(width*sizeof(char *)+width*height*sizeof(char));
	bzero(tabe,width*sizeof(char *)+width*height*sizeof(char));
	for ( i = 0, tabeb = (char *)(tabe+width); i < width; i++ )
		tabe[i] = tabeb + height*i;
	define_cursor(can->window,runningcur);
	set_busy(can); set_selection();
	qcalc(tabe,can); qif_print(display,tabe,can);
	reset_selection(); reset_busy(can);
	define_cursor(can->window,normalcur);
}
