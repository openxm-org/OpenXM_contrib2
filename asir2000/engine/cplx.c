/* $OpenXM: OpenXM/src/asir99/engine/cplx.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"
#include "base.h"
#if PARI
#include "genpari.h"
void patori(GEN,Obj *);
void patori_i(GEN,N *);
void ritopa(Obj,GEN *);
void ritopa_i(N,int,GEN *);
#endif

void toreim(a,rp,ip)
Num a;
Num *rp,*ip;
{
	if ( !a )
		*rp = *ip = 0;
	else if ( NID(a) <= N_B ) {
		*rp = a; *ip = 0;
	} else {
		*rp = ((C)a)->r; *ip = ((C)a)->i;
	}
}

void reimtocplx(r,i,cp)
Num r,i;
Num *cp;
{
	C c;

	if ( !i )
		*cp = r;
	else {
		NEWC(c); c->r = r; c->i = i; *cp = (Num)c;
	}
}

void addcplx(a,b,c)
Num a,b;
Num *c;
{
	Num ar,ai,br,bi,cr,ci;

	if ( !a )
		*c = b;
	else if ( !b )
		*c = a;
	else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
		addnum(0,a,b,c);
	else {
		toreim(a,&ar,&ai); toreim(b,&br,&bi);
		addnum(0,ar,br,&cr); addnum(0,ai,bi,&ci);
		reimtocplx(cr,ci,c);
	}
}

void subcplx(a,b,c)
Num a,b;
Num *c;
{
	Num ar,ai,br,bi,cr,ci;

	if ( !a )
		chsgnnum(b,c);
	else if ( !b )
		*c = a;
	else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
		subnum(0,a,b,c);
	else {
		toreim(a,&ar,&ai); toreim(b,&br,&bi);
		subnum(0,ar,br,&cr); subnum(0,ai,bi,&ci);
		reimtocplx(cr,ci,c);
	}
}

void mulcplx(a,b,c)
Num a,b;
Num *c;
{
	Num ar,ai,br,bi,cr,ci,t,s;

	if ( !a || !b )
		*c = 0;
	else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
		mulnum(0,a,b,c);
	else {
		toreim(a,&ar,&ai); toreim(b,&br,&bi);
		mulnum(0,ar,br,&t); mulnum(0,ai,bi,&s); subnum(0,t,s,&cr);
		mulnum(0,ar,bi,&t); mulnum(0,ai,br,&s); addnum(0,t,s,&ci);
		reimtocplx(cr,ci,c);
	}
}

void divcplx(a,b,c)
Num a,b;
Num *c;
{
	Num ar,ai,br,bi,cr,ci,t,s,u,w;

	if ( !b )
		error("divcplx : division by 0");
	else if ( !a )
		*c = 0;
	else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
		divnum(0,a,b,c);
	else {
		toreim(a,&ar,&ai); toreim(b,&br,&bi);
		mulnum(0,br,br,&t); mulnum(0,bi,bi,&s); addnum(0,t,s,&u);
		mulnum(0,ar,br,&t); mulnum(0,ai,bi,&s);
		addnum(0,t,s,&w); divnum(0,w,u,&cr);
		mulnum(0,ar,bi,&t); mulnum(0,ai,br,&s);
		subnum(0,s,t,&w); divnum(0,w,u,&ci);
		reimtocplx(cr,ci,c);
	}
}

void pwrcplx(a,e,c)
Num a,e;
Num *c;
{
	int ei;
	Num t;
	extern long prec;

	if ( !e )
		*c = (Num)ONE;
	else if ( !a )
		*c = 0;
	else if ( !INT(e) ) {
#if PARI
		GEN pa,pe,z;
		int ltop,lbot;

		ltop = avma; ritopa((Obj)a,&pa); ritopa((Obj)e,&pe); lbot = avma;
		z = gerepile(ltop,lbot,gpui(pa,pe,prec));
		patori(z,(Obj *)c); cgiv(z);
#else
		error("pwrcplx : can't calculate a fractional power");
#endif
	} else {
		ei = SGN((Q)e)*QTOS((Q)e);
		pwrcplx0(a,ei,&t);
		if ( SGN((Q)e) < 0 )
			divnum(0,(Num)ONE,t,c);
		else
			*c = t;
	}
}

void pwrcplx0(a,e,c)
Num a;
int e;
Num *c;
{
	Num t,s;

	if ( e == 1 )
		*c = a;
	else {
		pwrcplx0(a,e/2,&t);
		if ( !(e%2) )
			mulnum(0,t,t,c);
		else {
			mulnum(0,t,t,&s); mulnum(0,s,a,c);
		}
	}
}

void chsgncplx(a,c)
Num a,*c;
{
	Num r,i;

	if ( !a )
		*c = 0;
	else if ( NID(a) <= N_B )
		chsgnnum(a,c);
	else {
		chsgnnum(((C)a)->r,&r); chsgnnum(((C)a)->i,&i);
		reimtocplx(r,i,c);
	}
}

int cmpcplx(a,b)
Num a,b;
{
	Num ar,ai,br,bi;
	int s;

	if ( !a ) {
		if ( !b || (NID(b)<=N_B) )
			return compnum(0,a,b);
		else
			return -1;
	} else if ( !b ) {
		if ( !a || (NID(a)<=N_B) )
			return compnum(0,a,b);
		else
			return 1;
	} else {
		toreim(a,&ar,&ai); toreim(b,&br,&bi);
		s = compnum(0,ar,br);
		if ( s )
			return s;
		else
			return compnum(0,ai,bi);
	}
}
