/* $OpenXM: OpenXM/src/asir99/engine/RU.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"

int qcoefr(r)
Obj r;
{
	if ( !r )
		return 1;
	else
		switch ( OID(r) ) {
			case O_N:
				return RATN(r)?1:0; break;
			case O_P:
				return qcoefp(r); break;
			default:
				return qcoefp((Obj)NM((R)r))&&qcoefp((Obj)DN((R)r)); break;
		}
}

int qcoefp(p)
Obj p;
{
	DCP dc;

	if ( !p )
		return 1;
	else
		switch ( OID(p) ) {
			case O_N:
				return RATN(p)?1:0; break;
			default:
				for ( dc = DC((P)p); dc; dc = NEXT(dc) )
					if ( !qcoefp((Obj)COEF(dc)) )
						return 0;
				return 1; break;
		}
}

void reductr(vl,p,r)
VL vl;
Obj p,*r;
{
	P t,s,u,cnm,cdn,pnm,pdn;
	R a;

	if ( !p )
		*r = 0;
	else if ( OID(p) <= O_P )
		*r = p;
	else if ( ((R)p)->reduced )
		*r = p;
	else if ( NUM(DN((R)p)) )
		divsp(vl,NM((R)p),DN((R)p),(P *)r);
	else if ( qcoefp((Obj)NM((R)p)) && qcoefp((Obj)DN((R)p)) ) {
		ptozp(NM((R)p),1,(Q *)&cnm,&pnm); ptozp(DN((R)p),1,(Q *)&cdn,&pdn);
		ezgcdpz(vl,pnm,pdn,&t); 
		divsp(vl,NM((R)p),t,&u); divsp(vl,DN((R)p),t,&s);
		if ( NUM(s) )
			divsp(vl,u,s,(P *)r);
		else {
			divsp(vl,u,cdn,&pnm); divsp(vl,s,cdn,&pdn); 
			MKRAT(pnm,pdn,1,a); *r = (Obj)a;
		}
	} else {
		MKRAT(NM((R)p),DN((R)p),1,a); *r = (Obj)a;
	}
}

void pderivr(vl,a,v,b)
VL vl;
V v;
Obj a,*b;
{
	P t,s,u;
	R r;

	if ( !a )
		*b = 0;
	else if ( OID(a) <= O_P )
		diffp(vl,(P)a,v,(P *)b); 
	else {
		diffp(vl,NM((R)a),v,&t); mulp(vl,t,DN((R)a),&s);
		diffp(vl,DN((R)a),v,&t); mulp(vl,t,NM((R)a),&u);
		subp(vl,s,u,&t);
		if ( t ) {
			mulp(vl,DN((R)a),DN((R)a),&u); MKRAT(t,u,0,r); *b = (Obj)r;
		} else 
			*b = 0;
	}
}

void clctvr(vl,p,nvl)
VL vl,*nvl;
Obj p;
{
	VL vl1,vl2;

	if ( !p )
		*nvl = 0;
	else if ( OID(p) <= O_P )
		clctv(vl,(P)p,nvl);
	else {
		clctv(vl,NM((R)p),&vl1); clctv(vl,DN((R)p),&vl2); mergev(vl,vl1,vl2,nvl);
	}
}
