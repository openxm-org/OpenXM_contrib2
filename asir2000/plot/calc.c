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
 * $OpenXM: OpenXM_contrib2/asir2000/plot/calc.c,v 1.7 2003/02/14 22:29:19 ohara Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "ifplot.h"
#include <math.h>
#if defined(PARI)
#include "genpari.h"
#endif

#ifndef MAXSHORT
#define MAXSHORT ((short)0x7fff)
#endif

void calc(double **tab,struct canvas *can,int nox)
{
	double x,y,xmin,ymin,xstep,ystep;
	int ix,iy;
	Real r,rx,ry;
	Obj fr,g;
	int w,h;
	V vx,vy;
	Obj t,s;

	if ( !nox ) initmarker(can,"Evaluating...");
	MKReal(1.0,r); mulr(CO,(Obj)can->formula,(Obj)r,&fr);
	vx = can->vx;
	vy = can->vy;
	w = can->width; h = can->height; 
	xmin = can->xmin; xstep = (can->xmax-can->xmin)/w;
	ymin = can->ymin; ystep = (can->ymax-can->ymin)/h;
	MKReal(1.0,rx); MKReal(1.0,ry); /* dummy real */
	for( ix = 0, x = xmin; ix < w ; ix++, x += xstep ) {
		BDY(rx) = x; substr(CO,0,fr,vx,x?(Obj)rx:0,&t);
		devalr(CO,t,&g);
		if ( !nox ) marker(can,DIR_X,ix);
		for( iy = 0, y = ymin; iy < h ; iy++, y += ystep ) {
			BDY(ry) = y;
			substr(CO,0,g,vy,y?(Obj)ry:0,&t);
			devalr(CO,t,&s);
			tab[ix][iy] = ToReal(s);
		}
	}
}

double usubstrp(P p,double r)
{
	double t;
	DCP dc;
	int d;
	double pwrreal0();

	if ( !p ) 
		t = 0.0;
	else if ( NUM(p) ) 
		t = BDY((Real)p);
	else {
		dc = DC(p); t = BDY((Real)COEF(dc));
		for ( d = QTOS(DEG(dc)), dc = NEXT(dc); dc; 
			d = QTOS(DEG(dc)), dc = NEXT(dc) ) {
			t = t*pwrreal0(r,(d-QTOS(DEG(dc))))+BDY((Real)COEF(dc));
		}
		if ( d )
			t *= pwrreal0(r,d);
	}
	return t;
}

void qcalc(char **tab,struct canvas *can)
{
	Q dx,dy,w,h,xstep,ystep,c,q1;
	P g,g1,f1,f2,x,y,t,s;
	int ix,iy;
	int *a,*pa;
	VECT ss;

	subq(can->qxmax,can->qxmin,&dx); STOQ(can->width,w); divq(dx,w,&xstep);
	subq(can->qymax,can->qymin,&dy); STOQ(can->height,h); divq(dy,h,&ystep);
	MKV(can->vx,x); mulp(CO,(P)xstep,x,&t); 
	addp(CO,(P)can->qxmin,t,&s); substp(CO,can->formula,can->vx,s,&f1);
	MKV(can->vy,y); mulp(CO,(P)ystep,y,&t);
	addp(CO,(P)can->qymin,t,&s); substp(CO,f1,can->vy,s,&f2);
	ptozp(f2,1,&c,&g);
	a = (int *)ALLOCA((MAX(can->width,can->height)+1)*sizeof(int));
	initmarker(can,"Horizontal scan...");
	for( ix = 0; ix < can->width; ix++ ) {
		marker(can,DIR_X,ix);
		STOQ(ix,q1); substp(CO,g,can->vx,(P)q1,&t); ptozp(t,1,&c,&g1);
		if ( !g1 )
			for ( iy = 0; iy < can->height; iy++ )
				tab[ix][iy] = 1;
		else if ( !NUM(g1) ) {
			sturmseq(CO,g1,&ss); seproot(ss,0,can->height,a);
			for ( iy = 0, pa = a; iy < can->height; iy++, pa++ )
				if ( *pa < 0 || (*(pa+1) >= 0 && (*pa > *(pa+1))) )
					tab[ix][iy] = 1;
		}
	}
	initmarker(can,"Vertical scan...");
	for( iy = 0; iy < can->height; iy++ ) {
		marker(can,DIR_Y,iy);
		STOQ(iy,q1); substp(CO,g,can->vy,(P)q1,&t); ptozp(t,1,&c,&g1);
		if ( !g1 )
			for ( ix = 0; ix < can->width; ix++ )
				tab[ix][iy] = 1;
		else if ( !NUM(g1) ) {
			sturmseq(CO,g1,&ss); seproot(ss,0,can->width,a);
			for ( ix = 0, pa = a; ix < can->width; ix++, pa++ )
				if ( *pa < 0 || (*(pa+1) >= 0 && (*pa > *(pa+1))) )
					tab[ix][iy] = 1;
		}
	}
}

void sturmseq(VL vl,P p,VECT *rp)
{
	P g1,g2,q,r,s;
	P *t;
	V v;
	VECT ret;
	int i,j;
	Q a,b,c,d,h,l,m,x;

	v = VR(p); t = (P *)ALLOCA((deg(v,p)+1)*sizeof(P));
	g1 = t[0] = p; diffp(vl,p,v,(P *)&a); ptozp((P)a,1,&c,&g2); t[1] = g2;
	for ( i = 1, h = ONE, x = ONE; ; ) {
		if ( NUM(g2) )
			break;
		subq(DEG(DC(g1)),DEG(DC(g2)),&d);
		l = (Q)LC(g2);
		if ( SGN(l) < 0 ) {
			chsgnq(l,&a); l = a;
		}
		addq(d,ONE,&a); pwrq(l,a,&b); mulp(vl,(P)b,g1,(P *)&a);
		divsrp(vl,(P)a,g2,&q,&r);
		if ( !r )
			break;
		chsgnp(r,&s); r = s; i++;
		if ( NUM(r) ) {
			t[i] = r; break;
		}
		pwrq(h,d,&m); g1 = g2;
		mulq(m,x,&a); divsp(vl,r,(P)a,&g2); t[i] = g2;
		x = (Q)LC(g1);
		if ( SGN(x) < 0 ) {
			chsgnq(x,&a); x = a;
		}
		pwrq(x,d,&a); mulq(a,h,&b); divq(b,m,&h);
	}
	MKVECT(ret,i+1);
	for ( j = 0; j <= i; j++ )
		ret->body[j] = (pointer)t[j];
	*rp = ret;	
}

void seproot(VECT s,int min,int max,int *ar)
{
	P f;
	P *ss;
	Q q,t;
	int i,j,k;

	ss = (P *)s->body; f = ss[0];
	for ( i = min; i <= max; i++ ) {
		STOQ(i,q); usubstqp(f,q,&t);
		if ( !t )
			ar[i] = -1;
		else {
			ar[i] = numch(s,q,t); break;
		}
	}
	if ( i > max )
		return;
	for ( j = max; j >= min; j-- ) {
		STOQ(j,q); usubstqp(f,q,&t);
		if ( !t )
			ar[j] = -1;
		else {
			if ( i != j )
				ar[j] = numch(s,q,t);
			break;
		}
	}
	if ( j <= i+1 )
		return;
	if ( ar[i] == ar[j] ) {
		for ( k = i+1; k < j; k++ )
			ar[k] = ar[i];
		return;
	}
	k = (i+j)/2;
	seproot(s,i,k,ar);
	seproot(s,k,j,ar);
}

int numch(VECT s,Q n,Q a0)
{
	int len,i,c;
	Q a;
	P *ss;

	len = s->len; ss = (P *)s->body;
	for ( i = 1, c = 0; i < len; i++ ) {
		usubstqp(ss[i],n,&a);
		if ( a ) {
			if ( (SGN(a)>0 && SGN(a0)<0) || (SGN(a)<0 && SGN(a0)>0) )
				c++;
			a0 = a;
		}
	}
	return c;
}

void usubstqp(P p,Q r,Q *v)
{
	Q d,d1,a,b,t;
	DCP dc;

	if ( !p )
		*v = 0;
	else if ( NUM(p) ) 
		*v = (Q)p;
	else {
		dc = DC(p); t = (Q)COEF(dc);
		for ( d = DEG(dc), dc = NEXT(dc); dc; 
			d = DEG(dc), dc = NEXT(dc) ) {
			subq(d,DEG(dc),&d1); pwrq(r,d1,&a);
			mulq(t,a,&b); addq(b,(Q)COEF(dc),&t);
		}
		if ( d ) {
			pwrq(r,d,&a); mulq(t,a,&b); t = b;
		}
		*v = t;
	}
}

void plotcalc(struct canvas *can)
{
	double x,xmin,xstep,ymax,ymin,dy;
	int ix;
	Real r;
	Obj fr;
	double usubstrp();
	int w,h;
	double *tab;
	POINT *pa;
	Real rx;
	Obj t,s;

	MKReal(1.0,r); mulr(CO,(Obj)can->formula,(Obj)r,&fr); 
	w = can->width; h = can->height;
	xmin = can->xmin; xstep = (can->xmax-can->xmin)/w;
	tab = (double *)ALLOCA(w*sizeof(double));
	MKReal(1,rx); /* dummy real number */
	for( ix = 0, x = xmin; ix < w ; ix++, x += xstep ) {
		/* full substitution */
		BDY(rx) = x;
		substr(CO,0,fr,can->vx,x?(Obj)rx:0,&s);
		devalr(CO,(Obj)s,&t);
		if ( t && (OID(t)!=O_N || NID((Num)t)!=N_R) )
			error("plotcalc : invalid evaluation");
		tab[ix] = ToReal((Num)t);	
	}
	if ( can->ymax == can->ymin ) {
		for ( ymax = ymin = tab[0], ix = 1; ix < w; ix++ ) {
			if ( tab[ix] > ymax )
				ymax = tab[ix];
			if ( tab[ix] < ymin )
				ymin = tab[ix];
		}
		can->ymax = ymax; can->ymin = ymin;
	} else {
		ymax = can->ymax; ymin = can->ymin;
	}
	dy = ymax-ymin;
	can->pa = (struct pa *)MALLOC(sizeof(struct pa));
	can->pa[0].length = w;
	can->pa[0].pos = pa = (POINT *)MALLOC(w*sizeof(POINT));
	for ( ix = 0; ix < w; ix++ ) {
		double t;

		XC(pa[ix]) = ix; 
		t = (h - 1)*(ymax - tab[ix])/dy;
		if ( t > MAXSHORT )
			YC(pa[ix]) = MAXSHORT;
		else if ( t < -MAXSHORT )
			YC(pa[ix]) = -MAXSHORT;
		else
			YC(pa[ix]) = (long)t;
	}
}

void polarplotcalc(struct canvas *can)
{
	double xmax,xmin,ymax,ymin,dx,dy,pmin,pstep;
	int i,nstep;
	double usubstrp();
	int w,h;
	double tr,p;
	double *tabx,*taby;
	POINT *pa;
	Real r;
	Obj fr,t,s;

	MKReal(1.0,r); mulr(CO,(Obj)can->formula,(Obj)r,&fr); 
	w = can->width; h = can->height; nstep = can->nzstep;
	pmin = can->zmin; pstep = (can->zmax-can->zmin)/nstep;
	tabx = (double *)ALLOCA(nstep*sizeof(double));
	taby = (double *)ALLOCA(nstep*sizeof(double));
	MKReal(1,r); /* dummy real number */
	for( i = 0, p = pmin; i < nstep ; i++, p += pstep ) {
		/* full substitution */
		BDY(r) = p;
		substr(CO,0,fr,can->vx,p?(Obj)r:0,&s);
		devalr(CO,(Obj)s,&t);
		if ( t && (OID(t)!=O_N || NID((Num)t)!=N_R) )
			error("polarplotcalc : invalid evaluation");
		tr = ToReal((Num)t);	
		tabx[i] = tr*cos(p);
		taby[i] = tr*sin(p);
	}
	xmax = xmin = tabx[0];
	ymax = ymin = taby[0];
	for ( i = 1; i < nstep; i++ ) {
		if ( tabx[i] > xmax ) xmax = tabx[i];
		if ( tabx[i] < xmin ) xmin = tabx[i];
		if ( taby[i] > ymax ) ymax = taby[i];
		if ( taby[i] < ymin ) ymin = taby[i];
	}
	can->xmax = xmax; can->xmin = xmin;
	can->ymax = ymax; can->ymin = ymin;
	dx = xmax-xmin;
	dy = ymax-ymin;
	can->pa = (struct pa *)MALLOC(sizeof(struct pa));
	can->pa[0].length = nstep;
	can->pa[0].pos = pa = (POINT *)MALLOC(w*sizeof(POINT));
	for ( i = 0; i < nstep; i++ ) {
		XC(pa[i]) = (w-1)*(tabx[i]-xmin)/dx; 
		YC(pa[i]) = (h-1)*(ymax-taby[i])/dy; 
	}
}

#if defined(INTERVAL)
#define NORMAL 1
#define TRANSFER 1
#define RECURSION 1
#define RECTRANS 1

void ineqncalc(double **tab,struct canvas *can,int nox)
{
	double x, y, xmin, ymax, xstep, ystep;
	int ix, iy;
	Real r, rx, ry;
	Obj fr, g, t, s;
	int w, h;
	V vx, vy;

	if ( !nox ) initmarker(can,"Evaluating...");
	MKReal(1.0,r); mulr(CO,(Obj)can->formula,(Obj)r,&fr);
	vx = can->vx; vy = can->vy;
	w = can->width; h = can->height; 
	xmin = can->xmin; xstep = (can->xmax-can->xmin)/w;
	ymax = can->ymax; ystep = (can->ymax-can->ymin)/h;
	MKReal(1.0,rx); MKReal(1.0,ry); /* dummy real */

	for( iy = 0, y = ymax; iy <= h ; iy++, y -= ystep ) {
		BDY(ry) = y; substr(CO,0,fr,vy,y?(Obj)ry:0,&t);
		devalr(CO,t,&g);
		if ( !nox ) marker(can,DIR_Y,iy);
		for( ix = 0, x = xmin; ix <= w ; ix++, x += xstep ) {
			BDY(rx) = x;
			substr(CO,0,g,vx,x?(Obj)rx:0,&t);
			devalr(CO,t,&s);
			tab[iy][ix] = ToReal(s);
		}
	}
}

void reccalc(P fr, V vx, V vy, int ixlw, int ixhg, int iylw, int iyhg,
	double *xval, double *yval, int w, int **mask, int itvsize)
{
	int i, j;
	int ixmd, iymd;
	double inf, sup, dt;
	Real rxwid, rywid, rxlw, rylw, rzero;
	Itv itx, ity;
	Obj fr1;
	P py, px, tmp;

	double **tbl , xs, ys;
	int ywidth, xwidth;
	Real rx, ry;
	Obj g, t, s;

	if ( (ixlw > ixhg) || (iylw > iyhg) ) return;
	NEWItvP(itx); NEWItvP(ity);
	MKReal(xval[ixhg + 1] - xval[ixlw], rxwid);
	MKReal(yval[iyhg + 1] - yval[iylw], rywid);
	MKReal(0.0,rzero);
	istoitv((Num)rzero, (Num)rxwid, &itx);
	istoitv((Num)rzero, (Num)rywid, &ity);

	MKReal(xval[ixlw],rxlw);MKReal(yval[iylw],rylw);
	MKV(vx,tmp); addp(CO,(P)tmp,(P)rxlw,&px);
	MKV(vy,tmp); addp(CO,(P)tmp,(P)rylw,&py);
	substp(CO,(P)fr,(V)vx,(P)px,(P*)&fr1);
	substp(CO,(P)fr1,(V)vy,(P)py,(P*)&fr1);
	substr(CO,0,(Obj)fr1,vx,(Obj)itx,&fr1);
	substr(CO,0,(Obj)fr1,vy,(Obj)ity,&fr1);
	Num2double((Num)fr1,&inf,&sup);

	if(inf > sup){dt=inf;inf=sup;sup=dt;}
	if ( inf <= 0.0 && sup >= 0.0 ) {
		if ( (ixhg - ixlw <= itvsize) && (iyhg - iylw <= itvsize) ){
			ywidth = iyhg - iylw + 2; xwidth = ixhg - ixlw + 2;
			MKReal(1.0, rx); MKReal(1.0, ry);
			tbl = (double **)ALLOCA((ywidth)*sizeof(double *));
			for (i=0;i<ywidth;i++)
				tbl[i] = (double *)ALLOCA((xwidth)*sizeof(double));
			for (i = 0; i < ywidth; i++){
				BDY(ry) = yval[iylw+i];
				substr(CO,0,(Obj)fr,vy,(Obj)ry,&t);
				devalr(CO, t, &g);
				for (j = 0; j < xwidth; j++){
					BDY(rx) = xval[ixlw+j];
					substr(CO,0,g,vx,(Obj)rx,&t);
					devalr(CO, t, &s);
					tbl[i][j] = ToReal(s);
				}
			}
			for ( i = 0; i < ywidth - 1; i++){
				for ( j = 0; j < xwidth - 1; j++){
					if ( tbl[i][j] >= 0.0 ){
						if ( (tbl[i+1][j] <= 0.0) ||
							(tbl[i+1][j+1] <= 0.0) ||
							(tbl[i][j+1]   <= 0.0) ) mask[w-(i+iylw)][j+ixlw] = 0;
						else mask[w-(i+iylw)][j+ixlw] = 1;
					} else {
						if ( (tbl[i+1][j] >= 0.0) ||
							(tbl[i+1][j+1] >= 0.0) ||
							(tbl[i][j+1]   >= 0.0) ) mask[w-(i+iylw)][j+ixlw] = 0;
						else mask[w-(i+iylw)][j+ixlw] = 1;
					}
				}
			}
		} else {
			ixmd = (ixhg + ixlw)/2; iymd = (iyhg + iylw)/2;
			reccalc((P)fr,vx,vy,ixlw,ixmd,iylw,iymd,xval,yval,w,mask,itvsize);
			reccalc((P)fr,vx,vy,ixmd+1,ixhg,iylw,iymd,xval,yval,w,mask,itvsize);
			reccalc((P)fr,vx,vy,ixlw,ixmd,iymd+1,iyhg,xval,yval,w,mask,itvsize);
			reccalc((P)fr,vx,vy,ixmd+1,ixhg,iymd+1,iyhg,xval,yval,w,mask,itvsize);
		}
	} else {
		for (i=w-iyhg; i> w-iylw; i--)
			for (j=ixlw; j <= ixhg; j++) mask[i][j] = -1;
	}
}

void itvcalc(int **mask, struct canvas *can, int nox, int itvsize)
{
	double xstep, ystep, xv, yv, *xval, *yval;
	int imx, imy, i, w, h;
	Real r;
	Obj fr;
	V vx, vy;

	if ( !nox ) initmarker(can,"Evaluating...");
	MKReal(1.0,r); mulr(CO,(Obj)can->formula,(Obj)r,&fr);
	vx = can->vx; vy = can->vy;
	w = can->width; h = can->height; 
	xstep = (can->xmax - can->xmin)/w;
	ystep = (can->ymax - can->ymin)/h;

	xval = (double *)ALLOCA((w+1)*sizeof(double));
	yval = (double *)ALLOCA((h+1)*sizeof(double));
	for (i=0, xv=can->xmin; i<= w; i++, xv += xstep) xval[i] = xv;
	for (i=0, yv=can->ymin; i<= h; i++, yv += ystep) yval[i] = yv;
	imx = w/2; imy = h/2;

	reccalc((P)fr,vx,vy,0,imx,0,imy,xval,yval,w-1,mask,itvsize);
	reccalc((P)fr,vx,vy,imx+1,w-1,0,imy,xval,yval,w-1,mask,itvsize);
	reccalc((P)fr,vx,vy,0,imx,imy+1,h-1,xval,yval,w-1,mask,itvsize);
	reccalc((P)fr,vx,vy,imx+1,w-1,imy+1,h-1,xval,yval,w-1,mask,itvsize);
}

#if NORMAL
void itvcalc1(int **mask, struct canvas *can, int nox)
{
	double x, y, xstep, ystep;
	int ix, iy, w, h;
	Itv ity, itx;
	Real r, rx, ry, rx1, ry1;
	Obj fr, g, t;
	V vx, vy;

	if ( !nox ) initmarker(can,"Evaluating...");
	MKReal(1.0,r); mulr(CO,(Obj)can->formula,(Obj)r,&fr);
	vx = can->vx; vy = can->vy;
	w = can->width; h = can->height; 
	xstep = (can->xmax - can->xmin)/w;
	ystep = (can->ymax - can->ymin)/h;

	for( iy = 0, y = can->ymax; iy < h ; iy++, y -= ystep ) {
		MKReal(y, ry);
		MKReal(y-ystep,ry1);
		istoitv((Num)(ry1),(Num)ry,&ity);
		substr(CO,0,(Obj)fr,vy,(Obj)ity,&t);
		for( ix = 0, x = can->xmin; ix < w ; ix++, x += xstep ) {
			MKReal(x,rx);
			MKReal(x+xstep,rx1);
			istoitv((Num)rx,(Num)rx1,&itx);
			substr(CO,0,(Obj)t,vx,(Obj)itx,&g);
			if (compnum(0,0,(Num)g)) mask[iy][ix] = -1;
			else mask[iy][ix] = 0;
		}
	}
}
#endif

#if TRANSFER
void itvcalc2(int **mask, struct canvas *can, int nox)
{
	double x, y, xstep, ystep;
	int ix, iy, w, h;
	Itv ity, itx;
	Real r, rx, ry, rzero;
	Obj fr, g, t, s;
	V vx, vy;
	P mp, fr2;
	Real qx,qy;

	if ( !nox ) initmarker(can,"Evaluating...");
	MKReal(1.0,r); mulr(CO,(Obj)can->formula,(Obj)r,&fr);
	vx = can->vx; vy = can->vy;
	w = can->width; h = can->height; 
	xstep = (can->xmax - can->xmin)/w;
	ystep = (can->ymax - can->ymin)/h;

	MKReal(0.0, rzero);
	MKReal(ystep,ry);
	istoitv((Num)rzero,(Num)ry,&ity);
	MKReal(xstep,rx);
	istoitv((Num)rzero,(Num)rx,&itx);
	for( iy = 0, y = can->ymin; iy < h ; iy++, y += ystep ) {
		MKReal(y,qy);
		MKV(vy,mp);subp(CO,mp,(P)qy,&mp);
		substp(CO,(P)fr,(V)vy,(P)mp,&fr2);
		substr(CO,0,(Obj)fr2,vy,(Obj)ity,&t);
		for( ix = 0, x = can->xmin; ix < w ; ix++, x += xstep ) {
			MKReal(x,qx);
			MKV(vx,mp);addp(CO,mp,(P)qx,&mp);
			substp(CO,(P)t,(V)vx,(P)mp,(P*)&s);
			substr(CO,0,(Obj)s,vx,(Obj)itx,&g);
			if (compnum(0,0,(Num)g)) mask[iy][ix] = -1;
			else mask[iy][ix] = 0;
		}
	}
}
#endif

#if RECURSION
void reccalc3(P fr, V vx, V vy, int ixlw, int ixhg, int iylw, int iyhg,
	double *xval, double *yval,int w, int **mask, int itvsize)
{
	int i, j, ixmd, iymd;
	double inf, sup, dt;
	Real rxlw, rxhg, rylw, ryhg;
	Itv itx, ity;
	Obj fr1;

	NEWItvP(itx); NEWItvP(ity);

	if ( (ixlw > ixhg) || (iylw > iyhg) ) return;

	MKReal(xval[ixlw], rxlw); MKReal(xval[ixhg + 1], rxhg);
	MKReal(yval[iylw], rylw); MKReal(yval[iyhg + 1], ryhg);
	istoitv((Num)rxlw, (Num)rxhg, &itx);
	istoitv((Num)rylw, (Num)ryhg, &ity);
	substr(CO, 0, (Obj)fr, vy, (Obj)ity, &fr1);
	substr(CO, 0, (Obj)fr1, vx, (Obj)itx, &fr1);
	Num2double((Num)fr1, &inf, &sup);

	if( itvsize <= 0 ) itvsize = 1;
	if(inf > sup){dt = inf; inf = sup; sup = dt;}
	if ( inf <= 0.0 && sup >= 0.0 ) {
		if ( (ixhg - ixlw < itvsize) || (iyhg - iylw < itvsize) ){
			for(i = iylw; i <=iyhg; i++)
				for(j = ixlw; j <=ixhg; j++) mask[w-i][j] = 0;
		} else {
			ixmd = (ixhg + ixlw)/2; iymd = (iyhg + iylw)/2;
			reccalc3((P)fr,vx,vy,ixlw,ixmd,iylw,iymd,xval,yval,w,mask,itvsize);
			reccalc3((P)fr,vx,vy,ixmd+1,ixhg,iylw,iymd,xval,yval,w,mask,itvsize);
			reccalc3((P)fr,vx,vy,ixlw,ixmd,iymd+1,iyhg,xval,yval,w,mask,itvsize);
			reccalc3((P)fr,vx,vy,ixmd+1,ixhg,iymd+1,iyhg,xval,yval,w,mask,itvsize);
		}
	} else {
		for (i = iylw; i<= iyhg; i++)
			for (j = ixlw; j <= ixhg; j++) mask[w-i][j] = -1;
	}
}

void itvcalc3(int **mask, struct canvas *can, int nox, int itvsize)
{
	double xstep, ystep, xv, yv, *xval, *yval;
	int i, imx, imy, w, h;
	Real r;
	Obj fr;
	V vx, vy;

	if ( !nox ) initmarker(can,"Evaluating...");
	MKReal(1.0,r); mulr(CO,(Obj)can->formula,(Obj)r,&fr);
	vx = can->vx; vy = can->vy;
	w = can->width; h = can->height; 
	xstep = (can->xmax - can->xmin)/w;
	ystep = (can->ymax - can->ymin)/h;

	xval = (double *)ALLOCA((w+1)*sizeof(double));
	yval = (double *)ALLOCA((h+1)*sizeof(double));
	for (i = 0, xv = can->xmin; i <= w; i++, xv += xstep) xval[i] = xv;
	for (i = 0, yv = can->ymin; i <= h; i++, yv += ystep) yval[i] = yv;
	imx = w/2; imy = h/2;

	reccalc3((P)fr,vx,vy,0,imx,0,imy,xval,yval,w-1,mask,itvsize);
	reccalc3((P)fr,vx,vy,imx+1,w-1,0,imy,xval,yval,w-1,mask,itvsize);
	reccalc3((P)fr,vx,vy,0,imx,imy+1,h-1,xval,yval,w-1,mask,itvsize);
	reccalc3((P)fr,vx,vy,imx+1,w-1,imy+1,h-1,xval,yval,w-1,mask,itvsize);
}
#endif

#if RECTRANS
void reccalc4(P fr, V vx, V vy, int ixlw, int ixhg, int iylw, int iyhg,
	double *xval, double *yval, int w, int **mask, int itvsize)
{
	int i, j;
	int ixmd, iymd;
	double inf, sup, dt;
	Real rxlw, rylw, rzero, rxwid, rywid;
	Itv itx, ity;
	Obj fr1;
	P py, px, tmp;

	if ( (ixlw > ixhg) || (iylw > iyhg)) return;
	NEWItvP(itx); NEWItvP(ity);

	MKReal(0.0,rzero);
	MKReal(xval[ixhg + 1] - xval[ixlw], rxwid);
	MKReal(yval[iyhg + 1] - yval[iylw], rywid);
	istoitv((Num)rzero, (Num)rxwid, &itx);
	istoitv((Num)rzero, (Num)rywid, &ity);

	MKReal(xval[ixlw], rxlw); MKReal(yval[iylw], rylw);
	MKV(vx,tmp);addp(CO, (P)tmp, (P)rxlw, &px);
	MKV(vy,tmp);addp(CO, (P)tmp, (P)rylw, &py);
	substp(CO, (P)fr, (V)vx, (P)px, (P*)&fr1);
	substp(CO, (P)fr1, (V)vy, (P)py, (P*)&fr1);
	substr(CO, 0, (Obj)fr1, vx, (Obj)itx, &fr1);
	substr(CO, 0, (Obj)fr1, vy, (Obj)ity, &fr1);
	Num2double((Num)fr1, &inf, &sup);

	if(inf > sup){dt=inf;inf=sup;sup=dt;}
	if ( inf <= 0.0 && sup >= 0.0 ) {
		if ( (ixhg - ixlw <= itvsize) && (iyhg - iylw <= itvsize) ){
			for(i = iylw; i <= iyhg; i++)
				for(j = ixlw; j <= ixhg; j++) mask[w-i][j]=0;
		} else {
			ixmd = (ixhg + ixlw)/2; iymd = (iyhg + iylw)/2;

			reccalc4((P)fr,vx,vy,ixlw,ixmd,iylw,iymd,xval,yval,w,mask,itvsize);
			reccalc4((P)fr,vx,vy,ixmd+1,ixhg,iylw,iymd,xval,yval,w,mask,itvsize);
			reccalc4((P)fr,vx,vy,ixlw,ixmd,iymd+1,iyhg,xval,yval,w,mask,itvsize);
			reccalc4((P)fr,vx,vy,ixmd+1,ixhg,iymd+1,iyhg,xval,yval,w,mask,itvsize);
		}
	} else {
		for (i = iylw; i<= iyhg; i++)
			for (j = ixlw; j <= ixhg; j++) mask[w-i][j] = -1;
	}
}

void itvcalc4(int **mask, struct canvas *can, int nox, int itvsize)
{
	double xstep, ystep, xv, yv, *xval, *yval;
	int i, imx, imy, w, h;
	Real r;
	Obj fr;
	V vx, vy;

	if ( !nox ) initmarker(can,"Evaluating...");
	MKReal(1.0,r); mulr(CO,(Obj)can->formula,(Obj)r,&fr);
	vx = can->vx; vy = can->vy;
	w = can->width; h = can->height; 
	xstep = (can->xmax - can->xmin)/w;
	ystep = (can->ymax - can->ymin)/h;

	xval = (double *)ALLOCA((w+1)*sizeof(double));
	yval = (double *)ALLOCA((h+1)*sizeof(double));
	for (i=0, xv=can->xmin; i <= w; i++, xv += xstep) xval[i] = xv;
	for (i=0, yv=can->ymin; i <= h; i++, yv += ystep) yval[i] = yv;
	imx = w/2; imy = h/2;

	reccalc4((P)fr,vx,vy,0,imx,0,imy,xval,yval,w-1,mask,itvsize);
	reccalc4((P)fr,vx,vy,imx+1,w-1,0,imy,xval,yval,w-1,mask,itvsize);
	reccalc4((P)fr,vx,vy,0,imx,imy+1,h-1,xval,yval,w-1,mask,itvsize);
	reccalc4((P)fr,vx,vy,imx+1,w-1,imy+1,h-1,xval,yval,w-1,mask,itvsize);
}
#endif
#endif
