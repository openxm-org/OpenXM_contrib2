/* $OpenXM: OpenXM/src/asir99/plot/calc.c,v 1.1.1.1 1999/11/10 08:12:34 noro Exp $ */
#include "ca.h"
#include "ifplot.h"
#include <math.h>
#if PARI
#include "genpari.h"
#endif

double usubstrp(P,double);

void calc(tab,can)
double **tab;
struct canvas *can;
{
	double x,y,xmin,ymin,xstep,ystep;
	int ix,iy;
	Real r,rx,ry;
	Obj fr,g;
	int w,h;
	V vx,vy;
	Obj t,s;

	initmarker(can,"Evaluating...");
	MKReal(1.0,r); mulr(CO,(Obj)can->formula,(Obj)r,&fr);
	vx = can->vx;
	vy = can->vy;
	w = can->width; h = can->height; 
	xmin = can->xmin; xstep = (can->xmax-can->xmin)/w;
	ymin = can->ymin; ystep = (can->ymax-can->ymin)/h;
	MKReal(1.0,rx); MKReal(1.0,ry); /* dummy real */
	for( ix = 0, x = xmin; ix < w ; ix++, x += xstep ) {
#if 0
		MKReal(x,r); substp(CO,fr,vx,(P)r,&g);
		marker(can,DIR_X,ix);
		for( iy = 0, y = ymin; iy < h ; iy++, y += ystep )
			tab[ix][iy] = usubstrp(g,y);
#endif
		BDY(rx) = x; substr(CO,0,fr,vx,x?(P)rx:0,&t); devalr(CO,t,&g);
		marker(can,DIR_X,ix);
		for( iy = 0, y = ymin; iy < h ; iy++, y += ystep ) {
			BDY(ry) = y;
			substr(CO,0,g,vy,y?(P)ry:0,&t); devalr(CO,t,&s);
			tab[ix][iy] = ToReal(s);
		}
	}
}

double usubstrp(p,r)
P p;
double r;
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

void qcalc(tab,can)
char **tab;
struct canvas *can;
{
	Q dx,dy,w,h,xstep,ystep,c,q1,q2;
	P g,g1,f1,f2,x,y,t,s;
	DCP dc;
	int ix,iy;
	int *a,*pa;
	char *px;
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
			strum(CO,g1,&ss); seproot(ss,0,can->height,a);
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
			strum(CO,g1,&ss); seproot(ss,0,can->width,a);
			for ( ix = 0, pa = a; ix < can->width; ix++, pa++ )
				if ( *pa < 0 || (*(pa+1) >= 0 && (*pa > *(pa+1))) )
					tab[ix][iy] = 1;
		}
	}
}

strum(vl,p,rp)
VL vl;
P p;
VECT *rp;
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

seproot(s,min,max,ar)
VECT s;
int min,max;
int *ar;
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

numch(s,n,a0)
VECT s;
Q n,a0;
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

usubstqp(p,r,v)
P p;
Q r;
Q *v;
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

void plotcalc(can)
struct canvas *can;
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
		substr(CO,0,fr,can->vx,x?(P)rx:0,&s); devalr(CO,(Obj)s,&t);
		if ( t && (OID(t)!=O_N || NID((Num)t)!=N_R) )
			error("plotcalc : invalid evaluation");
		tab[ix] = ToReal((Num)t);	
#if 0
		tab[ix] = usubstrp(fr,x);
#endif
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
#ifndef MAXSHORT
#define MAXSHORT ((short)0x7fff)
#endif
		double t;

		XC(pa[ix]) = ix; 
		t = (h - 1)*(ymax - tab[ix])/dy;
		if ( t > MAXSHORT )
			YC(pa[ix]) = MAXSHORT;
		else if ( t < -MAXSHORT )
			YC(pa[ix]) = -MAXSHORT;
		else
			YC(pa[ix]) = t;
	}
}
