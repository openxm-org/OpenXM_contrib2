/* $OpenXM: OpenXM/src/asir99/engine/bf.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"
#if PARI
#include "base.h"
#include <math.h>
#include "genpari.h"

extern long prec;

void ritopa(Obj,GEN *);
void patori(GEN,Obj *);

void addbf(a,b,c)
Num a,b;
Num *c;
{
	GEN pa,pb,z;
	long ltop,lbot;

	if ( !a )
		*c = b;
	else if ( !b )
		*c = a;
	else if ( (NID(a) <= N_A) && (NID(b) <= N_A ) )
		(*addnumt[MIN(NID(a),NID(b))])(a,b,c);
	else {
		ltop = avma; ritopa((Obj)a,&pa);
		ritopa((Obj)b,&pb); lbot = avma;
		z = gerepile(ltop,lbot,gadd(pa,pb));
		patori(z,(Obj *)c); cgiv(z);
	}
}

void subbf(a,b,c)
Num a,b;
Num *c;
{
	GEN pa,pb,z;
	long ltop,lbot;

	if ( !a )
		(*chsgnnumt[NID(b)])(b,c);
	else if ( !b )
		*c = a;
	else if ( (NID(a) <= N_A) && (NID(b) <= N_A ) )
		(*subnumt[MIN(NID(a),NID(b))])(a,b,c);
	else {
		ltop = avma; ritopa((Obj)a,&pa); ritopa((Obj)b,&pb); lbot = avma;
		z = gerepile(ltop,lbot,gsub(pa,pb));
		patori(z,(Obj *)c); cgiv(z);
	}
}

void mulbf(a,b,c)
Num a,b;
Num *c;
{
	GEN pa,pb,z;
	long ltop,lbot;

	if ( !a || !b )
		*c = 0;
	else if ( (NID(a) <= N_A) && (NID(b) <= N_A ) )
		(*mulnumt[MIN(NID(a),NID(b))])(a,b,c);
	else {
		ltop = avma; ritopa((Obj)a,&pa); ritopa((Obj)b,&pb); lbot = avma;
		z = gerepile(ltop,lbot,gmul(pa,pb));
		patori(z,(Obj *)c); cgiv(z);
	}
}

void divbf(a,b,c)
Num a,b;
Num *c;
{
	GEN pa,pb,z;
	long ltop,lbot;

	if ( !b )
		error("divbf : division by 0");
	else if ( !a )
		*c = 0;
	else if ( (NID(a) <= N_A) && (NID(b) <= N_A ) )
		(*divnumt[MIN(NID(a),NID(b))])(a,b,c);
	else {
		ltop = avma; ritopa((Obj)a,&pa); ritopa((Obj)b,&pb); lbot = avma;
		z = gerepile(ltop,lbot,gdiv(pa,pb));
		patori(z,(Obj *)c); cgiv(z);
	}
}

void pwrbf(a,e,c)
Num a,e;
Num *c;
{
	GEN pa,pe,z;
	long ltop,lbot;

	if ( !e )
		*c = (Num)ONE;
	else if ( !a )
		*c = 0;
	else {
		ltop = avma; ritopa((Obj)a,&pa); ritopa((Obj)e,&pe); lbot = avma;
		z = gerepile(ltop,lbot,gpui(pa,pe,prec));
		patori(z,(Obj *)c); cgiv(z);
	}
}

void chsgnbf(a,c)
Num a,*c;
{
	BF t;
	GEN z;
	int s;

	if ( !a )
		*c = 0;
	else if ( NID(a) <= N_A )
		(*chsgnnumt[NID(a)])(a,c);
	else {
		z = (GEN)((BF)a)->body; s = lg(z); NEWBF(t,s);
		bcopy((char *)a,(char *)t,sizeof(struct oBF)+((s-1)*sizeof(long)));
		z = (GEN)((BF)t)->body; setsigne(z,-signe(z));
		*c = (Num)t;
	}
}

int cmpbf(a,b)
Num a,b;
{
	GEN pa,pb;
	int s;

	if ( !a ) {
		if ( !b || (NID(b)<=N_A) )
			return (*cmpnumt[NID(b)])(a,b);
		else
			return -signe(((BF)b)->body);
	} else if ( !b ) {
		if ( !a || (NID(a)<=N_A) )
			return (*cmpnumt[NID(a)])(a,b);
		else
			return signe(((BF)a)->body);
	} else {
		ritopa((Obj)a,&pa); ritopa((Obj)b,&pb);
		s = gcmp(pa,pb); cgiv(pb); cgiv(pa);
		return s;
	}
}
#endif /* PARI */
