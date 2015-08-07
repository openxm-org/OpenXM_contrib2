/* $OpenXM: OpenXM_contrib2/asir2000/builtin/bfaux.c,v 1.6 2015/08/07 06:15:00 takayama Exp $ */
#include "ca.h"
#include "parse.h"

void Peval(), Psetprec(), Psetbprec(), Ptodouble(), Psetround();
void Pmpfr_gamma(), Pmpfr_floor(), Pmpfr_round();

struct ftab bf_tab[] = {
	{"eval",Peval,-2},
	{"setprec",Psetprec,-1},
	{"setbprec",Psetbprec,-1},
	{"setround",Psetround,-1},
	{"todouble",Ptodouble,1},
	{"mpfr_gamma",Pmpfr_gamma,-2},
	{"mpfr_floor",Pmpfr_floor,-1},
	{"mpfr_round",Pmpfr_round,-1},
	{0,0,0},
};

int mpfr_roundmode = MPFR_RNDN;

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

void Peval(NODE arg,Obj *rp)
{
  int prec;

	asir_assert(ARG0(arg),O_R,"eval");
  if ( argc(arg) == 2 ) {
	  prec = QTOS((Q)ARG1(arg))*3.32193;
    if ( prec < MPFR_PREC_MIN ) prec = MPFR_PREC_MIN;
    else if ( prec > MPFR_PREC_MAX ) prec = MPFR_PREC_MAX;
  } else
    prec = 0;
	evalr(CO,(Obj)ARG0(arg),prec,rp);
}

/* set/get decimal precision */

void Psetprec(NODE arg,Obj *rp)
{
	int p;
	Q q;
	int prec,dprec;
  
  prec = mpfr_get_default_prec();
  /* decimal precision */
  dprec = prec*0.30103;
  STOQ(dprec,q); *rp = (Obj)q;
	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"setprec");
		prec = QTOS((Q)ARG0(arg))*3.32193;
		if ( p > 0 )
			prec = p;
	}
  if ( prec < MPFR_PREC_MIN ) prec = MPFR_PREC_MIN;
  else if ( prec > MPFR_PREC_MAX ) prec = MPFR_PREC_MAX;
	mpfr_set_default_prec(prec);
}

/* set/get bit precision */

void Psetbprec(NODE arg,Obj *rp)
{
	int p;
	Q q;
	int prec;
  
  prec = mpfr_get_default_prec();
  STOQ(prec,q); *rp = (Obj)q;
	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"setbprec");
		prec = QTOS((Q)ARG0(arg));
		if ( p > 0 )
			prec = p;
	}
  if ( prec < MPFR_PREC_MIN ) prec = MPFR_PREC_MIN;
  else if ( prec > MPFR_PREC_MAX ) prec = MPFR_PREC_MAX;
	mpfr_set_default_prec(prec);
}

void Psetround(NODE arg,Q *rp)
{
  int round;

  STOQ(mpfr_roundmode,*rp);
  if ( arg ) {
		asir_assert(ARG0(arg),O_N,"setround");
    round = QTOS((Q)ARG0(arg)); 
    switch ( round ) {
    case 0:
      mpfr_roundmode = MPFR_RNDN;
      break;
    case 1:
      mpfr_roundmode = MPFR_RNDZ;
      break;
    case 2:
      mpfr_roundmode = MPFR_RNDU;
      break;
    case 3:
      mpfr_roundmode = MPFR_RNDD;
      break;
    case 4:
      mpfr_roundmode = MPFR_RNDA;
      break;
    case 5:
      mpfr_roundmode = MPFR_RNDF;
      break;
    case 6:
      mpfr_roundmode = MPFR_RNDNA;
      break;
    default:
      error("setround : invalid rounding mode");
      break;
    }
  }
}

Num tobf(Num a,int prec);

void mp_pi(NODE arg,BF *rp)
{
    int prec;
	BF r;

	prec = arg ? QTOS((Q)ARG0(arg)) : 0;
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_const_pi(r->body,mpfr_roundmode);
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
	mpfr_set_ui(one,1,mpfr_roundmode);
	mpfr_exp(r->body,one,mpfr_roundmode);
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
	mpfr_sin(r->body,((BF)a)->body,mpfr_roundmode);
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
	mpfr_cos(r->body,((BF)a)->body,mpfr_roundmode);
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
	mpfr_tan(r->body,((BF)a)->body,mpfr_roundmode);
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
	mpfr_asin(r->body,((BF)a)->body,mpfr_roundmode);
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
	mpfr_acos(r->body,((BF)a)->body,mpfr_roundmode);
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
	mpfr_atan(r->body,((BF)a)->body,mpfr_roundmode);
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
	mpfr_sinh(r->body,((BF)a)->body,mpfr_roundmode);
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
	mpfr_cosh(r->body,((BF)a)->body,mpfr_roundmode);
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
	mpfr_tanh(r->body,((BF)a)->body,mpfr_roundmode);
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
	mpfr_asinh(r->body,((BF)a)->body,mpfr_roundmode);
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
	mpfr_acosh(r->body,((BF)a)->body,mpfr_roundmode);
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
	mpfr_atanh(r->body,((BF)a)->body,mpfr_roundmode);
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
	mpfr_exp(r->body,((BF)a)->body,mpfr_roundmode);
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
	mpfr_log(r->body,((BF)a)->body,mpfr_roundmode);
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
	mpfr_pow(r->body,((BF)a)->body,((BF)e)->body,mpfr_roundmode);
    *rp = r; 
}

void Pmpfr_gamma(NODE arg,BF *rp)
{
	Num a;
  int prec;
	BF r;

	prec = NEXT(arg) ? QTOS((Q)ARG1(arg)) : 0;
  prec *= 3.32193;
	a = tobf(ARG0(arg),prec);
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_gamma(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_floor(NODE arg,Q *rp)
{
	Num a;
  int prec;
	BF r;
	mpz_t t;
	GZ rz;

	prec = NEXT(arg) ? QTOS((Q)ARG1(arg)) : 0;
  prec *= 3.32193;
	a = tobf(ARG0(arg),prec);
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_floor(r->body,((BF)a)->body);
	mpz_init(t);
	mpfr_get_z(t,r->body,mpfr_roundmode);
	MPZTOGZ(t,rz);
	*rp = gztoz(rz); 
}

void Pmpfr_round(NODE arg,Q *rp)
{
	Num a;
  int prec;
	BF r;
	mpz_t t;
	GZ rz;

	prec = NEXT(arg) ? QTOS((Q)ARG1(arg)) : 0;
  prec *= 3.32193;
	a = tobf(ARG0(arg),prec);
	NEWBF(r);
	prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
	mpfr_round(r->body,((BF)a)->body);
	mpz_init(t);
	mpfr_get_z(t,r->body,mpfr_roundmode);
	MPZTOGZ(t,rz);
	*rp = gztoz(rz); 
}
