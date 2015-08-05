/* $OpenXM: OpenXM_contrib2/asir2000/builtin/bfaux.c,v 1.1 2015/08/04 06:55:02 noro Exp $ */
#include "ca.h"
#include "parse.h"

void Peval(), Psetprec(), Ptodouble();

struct ftab bf_tab[] = {
	{"eval",Peval,-2},
	{"setprec",Psetprec,-1},
	{"todouble",Ptodouble,1},
	{0,0,0},
};

void Ptodouble(NODE arg,Num *rp)
{
	double r,i;
	Real real,imag;
	Num num;

	asir_assert(ARG0(arg),O_N,"todouble");
	num = (Num)ARG0(arg);
	if ( !num ) {
		*rp = 0;
		return;
	}
	switch ( NID(num) ) {
		case N_R: case N_Q: case N_B:
			r = ToReal(num);
			MKReal(r,real);
			*rp = (Num)real;
			break;
		case N_C:
			r = ToReal(((C)num)->r);
			i = ToReal(((C)num)->i);
			MKReal(r,real);
			MKReal(i,imag);
			reimtocplx((Num)real,(Num)imag,rp);
			break;
		default:
			*rp = num;
			break;
	}
}

void Peval(arg,rp)
NODE arg;
Obj *rp;
{
  int prec;

	asir_assert(ARG0(arg),O_R,"eval");
  if ( argc(arg) == 2 ) {
	  prec = QTOS((Q)ARG1(arg));
    if ( prec < MPFR_PREC_MIN ) prec = MPFR_PREC_MIN;
    else if ( prec > MPFR_PREC_MAX ) prec = MPFR_PREC_MAX;
  } else
    prec = 0;
	evalr(CO,(Obj)ARG0(arg),prec,rp);
}

/* bit precision */

void Psetprec(NODE arg,Obj *rp)
{
	int p;
	Q q;

	long prec = mpfr_get_default_prec();

    STOQ(prec,q); *rp = (Obj)q;
	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"setprec");
		p = QTOS((Q)ARG0(arg));
		if ( p > 0 )
			prec = p;
	}
  if ( prec < MPFR_PREC_MIN ) prec = MPFR_PREC_MIN;
  else if ( prec > MPFR_PREC_MAX ) prec = MPFR_PREC_MAX;
	mpfr_set_default_prec(prec);
}

Num tobf(Num a,int prec);

void mp_pi(NODE arg,BF *rp)
{
    int prec;
	BF r;

	prec = arg ? QTOS((Q)ARG0(arg)) : 0;
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_const_pi(r->body,MPFR_RNDN);
    *rp = r; 
}

void mp_e(NODE arg,BF *rp)
{
    int prec;
	mpfr_t one;
	BF r;

	prec = arg ? QTOS((Q)ARG0(arg)) : 0;
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_init(one);
	mpfr_set_ui(one,1,MPFR_RNDN);
	mpfr_exp(r->body,one,MPFR_RNDN);
    *rp = r; 
}

void mp_sin(NODE arg,BF *rp)
{
	Num a;
    int prec;
	BF r;

	prec = NEXT(arg) ? QTOS((Q)ARG1(arg)) : 0;
	a = tobf(ARG0(arg),prec);
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_sin(r->body,((BF)a)->body,MPFR_RNDN);
    *rp = r; 
}

void mp_cos(NODE arg,BF *rp)
{
	Num a;
    int prec;
	BF r;

	prec = NEXT(arg) ? QTOS((Q)ARG1(arg)) : 0;
	a = tobf(ARG0(arg),prec);
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_cos(r->body,((BF)a)->body,MPFR_RNDN);
    *rp = r; 
}

void mp_tan(NODE arg,BF *rp)
{
	Num a;
    int prec;
	BF r;

	prec = NEXT(arg) ? QTOS((Q)ARG1(arg)) : 0;
	a = tobf(ARG0(arg),prec);
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_tan(r->body,((BF)a)->body,MPFR_RNDN);
    *rp = r; 
}

void mp_asin(NODE arg,BF *rp)
{
	Num a;
    int prec;
	BF r;

	prec = NEXT(arg) ? QTOS((Q)ARG1(arg)) : 0;
	a = tobf(ARG0(arg),prec);
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_asin(r->body,((BF)a)->body,MPFR_RNDN);
    *rp = r; 
}
void mp_acos(NODE arg,BF *rp)
{
	Num a;
    int prec;
	BF r;

	prec = NEXT(arg) ? QTOS((Q)ARG1(arg)) : 0;
	a = tobf(ARG0(arg),prec);
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_acos(r->body,((BF)a)->body,MPFR_RNDN);
    *rp = r; 
}
void mp_atan(NODE arg,BF *rp)
{
	Num a;
    int prec;
	BF r;

	prec = NEXT(arg) ? QTOS((Q)ARG1(arg)) : 0;
	a = tobf(ARG0(arg),prec);
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_atan(r->body,((BF)a)->body,MPFR_RNDN);
    *rp = r; 
}

void mp_sinh(NODE arg,BF *rp)
{
	Num a;
    int prec;
	BF r;

	prec = NEXT(arg) ? QTOS((Q)ARG1(arg)) : 0;
	a = tobf(ARG0(arg),prec);
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_sinh(r->body,((BF)a)->body,MPFR_RNDN);
    *rp = r; 
}

void mp_cosh(NODE arg,BF *rp)
{
	Num a;
    int prec;
	BF r;

	prec = NEXT(arg) ? QTOS((Q)ARG1(arg)) : 0;
	a = tobf(ARG0(arg),prec);
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_cosh(r->body,((BF)a)->body,MPFR_RNDN);
    *rp = r; 
}

void mp_tanh(NODE arg,BF *rp)
{
	Num a;
    int prec;
	BF r;

	prec = NEXT(arg) ? QTOS((Q)ARG1(arg)) : 0;
	a = tobf(ARG0(arg),prec);
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_tanh(r->body,((BF)a)->body,MPFR_RNDN);
    *rp = r; 
}

void mp_asinh(NODE arg,BF *rp)
{
	Num a;
    int prec;
	BF r;

	prec = NEXT(arg) ? QTOS((Q)ARG1(arg)) : 0;
	a = tobf(ARG0(arg),prec);
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_asinh(r->body,((BF)a)->body,MPFR_RNDN);
    *rp = r; 
}
void mp_acosh(NODE arg,BF *rp)
{
	Num a;
    int prec;
	BF r;

	prec = NEXT(arg) ? QTOS((Q)ARG1(arg)) : 0;
	a = tobf(ARG0(arg),prec);
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_acosh(r->body,((BF)a)->body,MPFR_RNDN);
    *rp = r; 
}
void mp_atanh(NODE arg,BF *rp)
{
	Num a;
    int prec;
	BF r;

	prec = NEXT(arg) ? QTOS((Q)ARG1(arg)) : 0;
	a = tobf(ARG0(arg),prec);
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_atanh(r->body,((BF)a)->body,MPFR_RNDN);
    *rp = r; 
}

void mp_exp(NODE arg,BF *rp)
{
	Num a;
    int prec;
	BF r;

	prec = NEXT(arg) ? QTOS((Q)ARG1(arg)) : 0;
	a = tobf(ARG0(arg),prec);
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_exp(r->body,((BF)a)->body,MPFR_RNDN);
    *rp = r; 
}

void mp_log(NODE arg,BF *rp)
{
	Num a;
    int prec;
	BF r;

	prec = NEXT(arg) ? QTOS((Q)ARG1(arg)) : 0;
	a = tobf(ARG0(arg),prec);
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_log(r->body,((BF)a)->body,MPFR_RNDN);
    *rp = r; 
}

void mp_pow(NODE arg,BF *rp)
{
	Num a,e;
    int prec;
	BF r;

	prec = NEXT(NEXT(arg)) ? QTOS((Q)ARG2(arg)) : 0;
	a = tobf(ARG0(arg),prec);
	e = tobf(ARG1(arg),prec);
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_pow(r->body,((BF)a)->body,((BF)e)->body,MPFR_RNDN);
    *rp = r; 
}
