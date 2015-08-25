/* $OpenXM: OpenXM_contrib2/asir2000/builtin/bfaux.c,v 1.10 2015/08/20 08:42:07 noro Exp $ */
#include "ca.h"
#include "parse.h"

void Peval(), Psetprec(), Psetbprec(), Ptodouble(), Psetround();
void Pmpfr_ai();
void Pmpfr_eint(), Pmpfr_erf(), Pmpfr_erfc(), Pmpfr_li2();
void Pmpfr_zeta();
void Pmpfr_j0(), Pmpfr_j1();
void Pmpfr_y0(), Pmpfr_y1();
void Pmpfr_gamma(), Pmpfr_lngamma(), Pmpfr_digamma();
void Pmpfr_floor(), Pmpfr_round(), Pmpfr_ceil();

struct ftab bf_tab[] = {
	{"eval",Peval,-2},
	{"setprec",Psetprec,-1},
	{"setbprec",Psetbprec,-1},
	{"setround",Psetround,-1},
	{"todouble",Ptodouble,1},
	{"mpfr_ai",Pmpfr_ai,-2},
	{"mpfr_zeta",Pmpfr_zeta,-2},
	{"mpfr_j0",Pmpfr_j0,-2},
	{"mpfr_j1",Pmpfr_j1,-2},
	{"mpfr_y0",Pmpfr_y0,-2},
	{"mpfr_y1",Pmpfr_y1,-2},
	{"mpfr_eint",Pmpfr_eint,-2},
	{"mpfr_erf",Pmpfr_erf,-2},
	{"mpfr_erfc",Pmpfr_erfc,-2},
	{"mpfr_li2",Pmpfr_li2,-2},
	{"mpfr_gamma",Pmpfr_gamma,-2},
	{"mpfr_lngamma",Pmpfr_gamma,-2},
	{"mpfr_digamma",Pmpfr_gamma,-2},
	{"mpfr_floor",Pmpfr_floor,-2},
	{"mpfr_ceil",Pmpfr_ceil,-2},
	{"mpfr_round",Pmpfr_round,-2},
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
		p = QTOS((Q)ARG0(arg))*3.32193;
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
		p = QTOS((Q)ARG0(arg));
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
  if ( !cmpbf((Num)r,0) ) r = 0;
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
  if ( !cmpbf((Num)r,0) ) r = 0;
  *rp = r; 
}

void mpfr_or_mpc(NODE arg,int (*mpfr_f)(),int (*mpc_f)(),Num *rp)
{
	Num a;
  int prec;
	BF r,re,im;
  C c;
  mpc_t mpc,a1;

	prec = NEXT(arg) ? QTOS((Q)ARG1(arg)) : mpfr_get_default_prec();
	a = tobf(ARG0(arg),prec);
  if ( a && NID(a)==N_C ) {
    mpc_init2(mpc,prec); mpc_init2(a1,prec);
    re = (BF)((C)a)->r; im = (BF)((C)a)->i;
    mpc_set_fr_fr(a1,re->body,im->body,mpfr_roundmode);
    (*mpc_f)(mpc,a1,mpfr_roundmode);
    MPFRTOBF(mpc_realref(mpc),re);
    MPFRTOBF(mpc_imagref(mpc),im);
    if ( !cmpbf((Num)re,0) ) re = 0;
    if ( !cmpbf((Num)im,0) ) im = 0;
    if ( !im )
      *rp = (Num)re;
    else {
      NEWC(c); c->r = (Num)re; c->i = (Num)im;
      *rp = (Num)c; 
    }
  } else {
	  NEWBF(r);
	  mpfr_init2(r->body,prec);
	  (*mpfr_f)(r->body,((BF)a)->body,mpfr_roundmode);
    if ( !cmpbf((Num)r,0) ) r = 0;
    *rp = (Num)r; 
  }
}

void mp_sin(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_sin,mpc_sin,rp);
}

void mp_cos(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_cos,mpc_cos,rp);
}

void mp_tan(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_tan,mpc_tan,rp);
}

void mp_asin(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_asin,mpc_asin,rp);
}

void mp_acos(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_acos,mpc_acos,rp);
}

void mp_atan(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_atan,mpc_atan,rp);
}

void mp_sinh(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_sinh,mpc_sinh,rp);
}

void mp_cosh(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_cosh,mpc_cosh,rp);
}

void mp_tanh(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_tanh,mpc_tanh,rp);
}

void mp_asinh(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_asinh,mpc_asinh,rp);
}

void mp_acosh(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_acosh,mpc_acosh,rp);
}

void mp_atanh(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_atanh,mpc_atanh,rp);
}

void mp_exp(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_exp,mpc_exp,rp);
}

void mp_log(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_log,mpc_log,rp);
}

void mp_pow(NODE arg,Num *rp)
{
	Num a,e;
  int prec;
	BF r,re,im;
  C c;
  mpc_t mpc,a1,e1;

	prec = NEXT(NEXT(arg)) ? QTOS((Q)ARG2(arg)) : mpfr_get_default_prec();
	a = tobf(ARG0(arg),prec);
	e = tobf(ARG1(arg),prec);
  if ( NID(a) == N_C || NID(e) == N_C || MPFR_SIGN(((BF)a)->body) < 0 ) {
    mpc_init2(mpc,prec); mpc_init2(a1,prec); mpc_init2(e1,prec);
    if ( NID(a) == N_C ) {
      re = (BF)((C)a)->r; im = (BF)((C)a)->i;
      mpc_set_fr_fr(a1,re->body,im->body,mpfr_roundmode);
    } else {
      re = (BF)a;
      mpc_set_fr(a1,re->body,mpfr_roundmode);
    }
    if ( NID(e) == N_C ) {
      re = (BF)((C)e)->r; im = (BF)((C)e)->i;
      mpc_set_fr_fr(e1,re->body,im->body,mpfr_roundmode);
    } else {
      re = (BF)e;
      mpc_set_fr(e1,re->body,mpfr_roundmode);
    }
    mpc_pow(mpc,a1,e1,mpfr_roundmode);
    MPFRTOBF(mpc_realref(mpc),re);
    MPFRTOBF(mpc_imagref(mpc),im);
    if ( !cmpbf((Num)re,0) ) re = 0;
    if ( !cmpbf((Num)im,0) ) im = 0;
    if ( !im )
      *rp = (Num)re;
    else {
      NEWC(c); c->r = (Num)re; c->i = (Num)im;
      *rp = (Num)c; 
    }
  } else {
	  NEWBF(r);
	  mpfr_init2(r->body,prec);
	  mpfr_pow(r->body,((BF)a)->body,((BF)e)->body,mpfr_roundmode);
    *rp = (Num)r; 
  }
}

#define SETPREC \
 (prec)=NEXT(arg)?QTOS((Q)ARG1(arg)):0;\
 (prec)*=3.32193;\
 (a)=tobf(ARG0(arg),prec);\
 NEWBF(r);\
 prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);


void Pmpfr_gamma(NODE arg,BF *rp)
{
	Num a;
  int prec;
	BF r;

  SETPREC
	mpfr_gamma(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_lngamma(NODE arg,BF *rp)
{
	Num a;
  int prec;
	BF r;

  SETPREC
	mpfr_lngamma(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_digamma(NODE arg,BF *rp)
{
	Num a;
  int prec;
	BF r;

  SETPREC
	mpfr_digamma(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_zeta(NODE arg,BF *rp)
{
	Num a;
  int prec;
	BF r;

  SETPREC
	mpfr_zeta(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_eint(NODE arg,BF *rp)
{
	Num a;
  int prec;
	BF r;

  SETPREC
	mpfr_eint(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_erf(NODE arg,BF *rp)
{
	Num a;
  int prec;
	BF r;

  SETPREC
	mpfr_erf(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_erfc(NODE arg,BF *rp)
{
	Num a;
  int prec;
	BF r;

  SETPREC
	mpfr_erfc(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_j0(NODE arg,BF *rp)
{
	Num a;
  int prec;
	BF r;

  SETPREC
	mpfr_j0(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_j1(NODE arg,BF *rp)
{
	Num a;
  int prec;
	BF r;

  SETPREC
	mpfr_j1(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_y0(NODE arg,BF *rp)
{
	Num a;
  int prec;
	BF r;

  SETPREC
	mpfr_y0(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_y1(NODE arg,BF *rp)
{
	Num a;
  int prec;
	BF r;

  SETPREC
	mpfr_y1(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_li2(NODE arg,BF *rp)
{
	Num a;
  int prec;
	BF r;

  SETPREC
	mpfr_li2(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_ai(NODE arg,BF *rp)
{
	Num a;
  int prec;
	BF r;

  SETPREC
	mpfr_ai(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_floor(NODE arg,Q *rp)
{
	Num a;
  int prec;
	BF r;
	mpz_t t;
	GZ rz;

  SETPREC
	mpfr_floor(r->body,((BF)a)->body);
	mpz_init(t);
	mpfr_get_z(t,r->body,mpfr_roundmode);
	MPZTOGZ(t,rz);
	*rp = gztoz(rz); 
}

void Pmpfr_ceil(NODE arg,Q *rp)
{
	Num a;
  int prec;
	BF r;
	mpz_t t;
	GZ rz;

  SETPREC
	mpfr_ceil(r->body,((BF)a)->body);
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

  SETPREC
	mpfr_round(r->body,((BF)a)->body);
	mpz_init(t);
	mpfr_get_z(t,r->body,mpfr_roundmode);
	MPZTOGZ(t,rz);
	*rp = gztoz(rz); 
}
