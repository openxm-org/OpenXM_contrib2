/* $OpenXM: OpenXM/src/asir99/engine/R.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"

void addr(vl,a,b,c)
VL vl;
Obj a,b,*c;
{
	P t,s,u;
	R r;

	if ( !a ) 
		*c = b;
	else if ( !b )
		*c = a;
	else if ( !RAT(a) )
		if ( !RAT(b) )
			addp(vl,(P)a,(P)b,(P *)c);
		else {
			mulp(vl,(P)a,DN((R)b),&t); addp(vl,t,NM((R)b),&s);
			if ( s )
				MKRAT(s,DN((R)b),((R)b)->reduced,r);
			else
				r = 0;
			*c = (Obj)r;
		}
	else if ( !RAT(b) ) {
		mulp(vl,DN((R)a),(P)b,&t); addp(vl,NM((R)a),t,&s);
		if ( s )
			MKRAT(s,DN((R)a),((R)a)->reduced,r);
		else
			r = 0;
		*c = (Obj)r;
	} else {
		mulp(vl,NM((R)a),DN((R)b),&t); mulp(vl,NM((R)b),DN((R)a),&s); 
		addp(vl,t,s,&u);
		if ( u ) {
			mulp(vl,DN((R)a),DN((R)b),&t); MKRAT(u,t,0,r); *c = (Obj)r;
		} else
			*c = 0;
	}
}

void subr(vl,a,b,c)
VL vl;
Obj a,b,*c;
{
	P t,s,u;
	R r;

	if ( !a ) 
		chsgnr(b,c);
	else if ( !b )
		*c = a;
	else if ( !RAT(a) )
		if ( !RAT(b) )
			subp(vl,(P)a,(P)b,(P *)c);
		else {
			mulp(vl,(P)a,DN((R)b),&t); subp(vl,t,NM((R)b),&s);
			if ( s )
				MKRAT(s,DN((R)b),((R)b)->reduced,r);
			else
				r = 0;
			*c = (Obj)r;
		}
	else if ( !RAT(b) ) {
		mulp(vl,DN((R)a),(P)b,&t); subp(vl,NM((R)a),t,&s);
		if ( s )
			MKRAT(s,DN((R)a),((R)a)->reduced,r);
		else
			r = 0;
		*c = (Obj)r;
	} else {
		mulp(vl,NM((R)a),DN((R)b),&t); mulp(vl,NM((R)b),DN((R)a),&s); 
		subp(vl,t,s,&u);
		if ( u ) {
			mulp(vl,DN((R)a),DN((R)b),&t); MKRAT(u,t,0,r); *c = (Obj)r;
		} else
			*c = 0;
	}
}

void mulr(vl,a,b,c)
VL vl;
Obj a,b,*c;
{
	P t,s;
	R r;

	if ( !a || !b ) 
		*c = 0;
	else if ( !RAT(a) )
		if ( !RAT(b) )
			mulp(vl,(P)a,(P)b,(P *)c);
		else {
			mulp(vl,(P)a,NM((R)b),&t); MKRAT(t,DN((R)b),0,r); *c = (Obj)r;
		}
	else if ( !RAT(b) ) {
		mulp(vl,NM((R)a),(P)b,&t); MKRAT(t,DN((R)a),0,r); *c = (Obj)r;
	} else {
		mulp(vl,NM((R)a),NM((R)b),&t); mulp(vl,DN((R)a),DN((R)b),&s); 
		MKRAT(t,s,0,r); *c = (Obj)r;
	}
}

void divr(vl,a,b,c)
VL vl;
Obj a,b,*c;
{
	P t,s;
	R r;

	if ( !b ) 
		error("divr : division by 0");
	else if ( !a )
		*c = 0;
	else if ( !RAT(a) )
		if ( !RAT(b) )
			if ( NUM(b) )
				divsp(vl,(P)a,(P)b,(P *)c);
			else {
				MKRAT((P)a,(P)b,0,r); *c = (Obj)r;
			}
		else {
			mulp(vl,(P)a,DN((R)b),&t); MKRAT(t,NM((R)b),0,r); *c = (Obj)r;
		}
	else if ( !RAT(b) ) {
		mulp(vl,DN((R)a),(P)b,&t); MKRAT(NM((R)a),t,0,r); *c = (Obj)r;
	} else {
		mulp(vl,NM((R)a),DN((R)b),&t); mulp(vl,DN((R)a),NM((R)b),&s); 
		MKRAT(t,s,0,r); *c = (Obj)r;
	}
}

void pwrr(vl,a,q,c)
VL vl;
Obj a,q,*c;
{
	P t,s;
	R r;
	Q q1;

	if ( !q )
		*c = (Obj)ONE;
	else if ( !a )
		*c = 0;
	else if ( !RAT(a) )
		pwrp(vl,(P)a,(Q)q,(P *)c);
	else if ( !NUM(q) || !RATN(q) || !INT(q) )
		notdef(vl,a,q,c);
	else {
		if ( SGN((Q)q) < 0 ) {
			chsgnq((Q)q,&q1); pwrp(vl,DN((R)a),q1,&t); pwrp(vl,NM((R)a),q1,&s);
		} else {
			pwrp(vl,NM((R)a),(Q)q,&t); pwrp(vl,DN((R)a),(Q)q,&s);
		}
		MKRAT(t,s,((R)a)->reduced,r); *c = (Obj)r;
	}
}

void chsgnr(a,b)
Obj a,*b;
{
	P t;
	R r;

	if ( !a )
		*b = 0;
	else if ( !RAT(a) )
		chsgnp((P)a,(P *)b);
	else {
		chsgnp(NM((R)a),&t); MKRAT(t,DN((R)a),((R)a)->reduced,r); *b = (Obj)r;
	}
}

int compr(vl,a,b)
VL vl;
Obj a,b;
{
	int t;

	if ( !a )
		return b ? -1 : 0;
	else if ( !b )
		return 1;
	else if ( !RAT(a) )
		return !RAT(b) ? compp(vl,(P)a,(P)b) : -1;
	else if ( !RAT(b) )
		return 1;
	else {
		t = compp(vl,NM((R)a),NM((R)b));
		if ( !t )
			t = compp(vl,DN((R)a),DN((R)b));
		return t;
	}
}
