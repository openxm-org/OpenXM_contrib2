/* $OpenXM: OpenXM/src/asir99/engine/alg.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"
#include "base.h"

extern VL ALG;

Obj ToAlg();

Obj ToAlg(a)
Num a;
{
	Obj t;

	if ( !a || (NID(a) != N_A) )
		t = (Obj)a;
	else
		t = BDY((Alg)a);
	return t;
}

void addalg(a,b,c)
Num a,b,*c;
{
	Obj t;
	Alg r;

	addr(ALG,ToAlg(a),ToAlg(b),&t); MKAlg(t,r); *c = (Num)r;
}

void subalg(a,b,c)
Num a,b,*c;
{
	Obj t;
	Alg r;

	subr(ALG,ToAlg(a),ToAlg(b),&t); MKAlg(t,r); *c = (Num)r;
}

void mulalg(a,b,c)
Num a,b,*c;
{
	Obj t;
	Alg r;

	mulr(ALG,ToAlg(a),ToAlg(b),&t); MKAlg(t,r); *c = (Num)r;
}

void divalg(a,b,c)
Num a,b,*c;
{
	Obj t;
	Alg r;

	divr(ALG,ToAlg(a),ToAlg(b),&t); MKAlg(t,r); *c = (Num)r;
}

void chsgnalg(a,c)
Num a,*c;
{
	Obj t;
	Alg r;

	chsgnr(ToAlg(a),&t); MKAlg(t,r); *c = (Num)r;
}

void pwralg(a,b,c)
Num a,b,*c;
{
	Obj t;
	Alg r;

	pwrr(ALG,ToAlg(a),(Obj)b,&t); MKAlg(t,r); *c = (Num)r;
}

int cmpalg(a,b)
Num a,b;
{
	return compr(ALG,ToAlg(a),ToAlg(b));
}
