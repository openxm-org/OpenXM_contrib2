/*
 * $OpenXM: OpenXM_contrib2/asir2000/engine/bf.c,v 1.11 2015/08/06 23:41:52 noro Exp $ 
 */
#include "ca.h"
#include "base.h"
#include <math.h>

extern int mpfr_roundmode;

Num tobf(Num,int);

#define BFPREC(a) (((BF)(a))->body->_mpfr_prec)

void strtobf(char *s,BF *p)
{
  BF r;
  NEWBF(r);
  mpfr_init(r->body);
  mpfr_set_str(r->body,s,10,mpfr_roundmode);
  *p = r;
}

double mpfrtodbl(mpfr_t a)
{
  return mpfr_get_d(a,mpfr_roundmode);
}

Num tobf(Num a,int prec)
{
  mpfr_t r;
  mpz_t z;
  mpq_t q;
  BF d;
  N nm,dn;
  C c;
  Num re,im;
  int sgn;

  if ( !a ) {
    prec ? mpfr_init2(r,prec) : mpfr_init(r);
    mpfr_set_zero(r,1);
    MPFRTOBF(r,d);
    return (Num)d;
  } else {
    switch ( NID(a) ) {
    case N_B:
      return a;
      break;
    case N_R:
      prec ? mpfr_init2(r,prec) : mpfr_init(r);
      mpfr_init_set_d(r,((Real)a)->body,mpfr_roundmode);
      MPFRTOBF(r,d);
      return (Num)d;
      break;
    case N_Q:
      nm = NM((Q)a); dn = DN((Q)a); sgn = SGN((Q)a);
      if ( INT((Q)a) ) {
        mpz_init(z);
        mpz_import(z,PL(nm),-1,sizeof(BD(nm)[0]),0,0,BD(nm));
        if ( sgn < 0 ) mpz_neg(z,z);
        mpfr_init_set_z(r,z,mpfr_roundmode);
      } else {
        mpq_init(q);
        mpz_import(mpq_numref(q),PL(nm),-1,sizeof(BD(nm)[0]),0,0,BD(nm));
        mpz_import(mpq_denref(q),PL(dn),-1,sizeof(BD(dn)[0]),0,0,BD(dn));
        if ( sgn < 0 ) mpq_neg(q,q);
        mpfr_init_set_q(r,q,mpfr_roundmode);
      }
      MPFRTOBF(r,d);
      return (Num)d;
      break;
    case N_C:
      re = tobf(((C)a)->r,prec); im = tobf(((C)a)->i,prec);
      NEWC(c); c->r = re; c->i = im;
      return (Num)c;
      break;
    default:
      error("tobf : invalid argument");
      break;
    }
  }
}

void addbf(Num a,Num b,Num *c)
{
  mpfr_t r;
  BF d;
  GZ z;
  GQ q;

  if ( !a )
    *c = b;
  else if ( !b )
    *c = a;
  else if ( (NID(a) <= N_A) && (NID(b) <= N_A ) )
    (*addnumt[MIN(NID(a),NID(b))])(a,b,c);
  else if ( NID(a) == N_B ) {
    switch ( NID(b) ) {
    case N_Q:
      mpfr_init2(r,BFPREC(a));
      if ( INT((Q)b) ) {
        z = ztogz((Q)b);
        mpfr_add_z(r,((BF)a)->body,z->body,mpfr_roundmode);
      } else {
        q = qtogq((Q)b);
        mpfr_add_q(r,((BF)a)->body,q->body,mpfr_roundmode);
      }
      break;
    case N_R:
      /* double precision = 53 */
      mpfr_init2(r,MAX(BFPREC(a),53));
      mpfr_add_d(r,((BF)a)->body,((Real)b)->body,mpfr_roundmode);
      break;
    case N_B:
      mpfr_init2(r,MAX(BFPREC(a),BFPREC(b)));
      mpfr_add(r,((BF)a)->body,((BF)b)->body,mpfr_roundmode);
      break;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  } else { /* NID(b)==N_B */
    switch ( NID(a) ) {
    case N_Q:
      mpfr_init2(r,BFPREC(b));
      if ( INT((Q)a) ) {
        z = ztogz((Q)a);
        mpfr_add_z(r,((BF)b)->body,z->body,mpfr_roundmode);
      } else {
        q = qtogq((Q)a);
        mpfr_add_q(r,((BF)b)->body,q->body,mpfr_roundmode);
      }
      break;
    case N_R:
      /* double precision = 53 */
      mpfr_init2(r,MAX(BFPREC(b),53));
      mpfr_add_d(r,((BF)b)->body,((Real)a)->body,mpfr_roundmode);
      break;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  }
  if ( !cmpbf(*c,0) ) *c = 0;
}

void subbf(Num a,Num b,Num *c)
{
  mpfr_t r,s;
  GZ z;
  GQ q;
  BF d;

  if ( !a )
    (*chsgnnumt[NID(b)])(b,c);
  else if ( !b )
    *c = a;
  else if ( (NID(a) <= N_A) && (NID(b) <= N_A ) )
    (*subnumt[MIN(NID(a),NID(b))])(a,b,c);
  else if ( NID(a) == N_B ) {
    switch ( NID(b) ) {
    case N_Q:
      mpfr_init2(r,BFPREC(a));
      if ( INT((Q)b) ) {
        z = ztogz((Q)b);
        mpfr_sub_z(r,((BF)a)->body,z->body,mpfr_roundmode);
      } else {
        q = qtogq((Q)b);
        mpfr_sub_q(r,((BF)a)->body,q->body,mpfr_roundmode);
      }
      break;
    case N_R:
      /* double precision = 53 */
      mpfr_init2(r,MAX(BFPREC(a),53));
      mpfr_sub_d(r,((BF)a)->body,((Real)b)->body,mpfr_roundmode);
      break;
    case N_B:
      mpfr_init2(r,MAX(BFPREC(a),BFPREC(b)));
      mpfr_sub(r,((BF)a)->body,((BF)b)->body,mpfr_roundmode);
      break;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  } else { /* NID(b)==N_B */
    switch ( NID(a) ) {
    case N_Q:
      mpfr_init2(r,BFPREC(b));
      if ( INT((Q)a) ) {
        z = ztogz((Q)a);
        mpfr_sub_z(r,((BF)b)->body,z->body,mpfr_roundmode);
      } else {
        q = qtogq((Q)a);
        mpfr_sub_q(r,((BF)b)->body,q->body,mpfr_roundmode);
      }
      mpfr_neg(r,r,mpfr_roundmode);
      break;
    case N_R:
      /* double precision = 53 */
      mpfr_init2(r,MAX(BFPREC(b),53));
      mpfr_d_sub(r,((Real)a)->body,((BF)b)->body,mpfr_roundmode);
      break;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  }
  if ( !cmpbf(*c,0) ) *c = 0;
}

void mulbf(Num a,Num b,Num *c)
{
  mpfr_t r;
  GZ z;
  GQ q;
  BF d;
  int prec;

  if ( !a || !b )
    *c = 0;
  else if ( (NID(a) <= N_A) && (NID(b) <= N_A ) )
    (*mulnumt[MIN(NID(a),NID(b))])(a,b,c);
  else if ( NID(a) == N_B ) {
    switch ( NID(b) ) {
    case N_Q:
      mpfr_init2(r,BFPREC(a));
      if ( INT((Q)b) ) {
        z = ztogz((Q)b);
        mpfr_mul_z(r,((BF)a)->body,z->body,mpfr_roundmode);
      } else {
        q = qtogq((Q)b);
        mpfr_mul_q(r,((BF)a)->body,q->body,mpfr_roundmode);
      }
      break;
    case N_R:
      /* double precision = 53 */
      mpfr_init2(r,MAX(BFPREC(a),53));
      mpfr_mul_d(r,((BF)a)->body,((Real)b)->body,mpfr_roundmode);
      break;
    case N_B:
      mpfr_init2(r,MAX(BFPREC(a),BFPREC(b)));
      mpfr_mul(r,((BF)a)->body,((BF)b)->body,mpfr_roundmode);
      break;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  } else { /* NID(b)==N_B */
    switch ( NID(a) ) {
    case N_Q:
      mpfr_init2(r,BFPREC(b));
      if ( INT((Q)a) ) {
        z = ztogz((Q)a);
        mpfr_mul_z(r,((BF)b)->body,z->body,mpfr_roundmode);
      } else {
        q = qtogq((Q)a);
        mpfr_mul_q(r,((BF)b)->body,q->body,mpfr_roundmode);
      }
      break;
    case N_R:
      /* double precision = 53 */
      mpfr_init2(r,MAX(BFPREC(b),53));
      mpfr_mul_d(r,((BF)b)->body,((Real)a)->body,mpfr_roundmode);
      break;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  }
  if ( !cmpbf(*c,0) ) *c = 0;
}

void divbf(Num a,Num b,Num *c)
{
  mpfr_t s,r;
  GZ z;
  GQ q;
  BF d;

  if ( !b )
    error("divbf : division by 0");
  else if ( !a )
    *c = 0;
  else if ( (NID(a) <= N_A) && (NID(b) <= N_A ) )
    (*divnumt[MIN(NID(a),NID(b))])(a,b,c);
  else if ( NID(a) == N_B ) {
    switch ( NID(b) ) {
    case N_Q:
      mpfr_init2(r,BFPREC(a));
      if ( INT((Q)b) ) {
        z = ztogz((Q)b);
        mpfr_div_z(r,((BF)a)->body,z->body,mpfr_roundmode);
      } else {
        q = qtogq((Q)b);
        mpfr_div_q(r,((BF)a)->body,q->body,mpfr_roundmode);
      }
      break;
    case N_R:
      /* double precision = 53 */
      mpfr_init2(r,MAX(BFPREC(a),53));
      mpfr_div_d(r,((BF)a)->body,((Real)b)->body,mpfr_roundmode);
      break;
    case N_B:
      mpfr_init2(r,MAX(BFPREC(a),BFPREC(b)));
      mpfr_div(r,((BF)a)->body,((BF)b)->body,mpfr_roundmode);
      break;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  } else { /* NID(b)==N_B */
    switch ( NID(a) ) {
    case N_Q:
      /* XXX : mpfr_z_div and mpfr_q_div are not implemented */
      a = tobf(a,BFPREC(b));
      mpfr_init2(r,BFPREC(b));
      mpfr_div(r,((BF)a)->body,((BF)b)->body,mpfr_roundmode);
      break;
    case N_R:
      /* double precision = 53 */
      mpfr_init2(r,MAX(BFPREC(b),53));
      mpfr_d_div(r,((Real)a)->body,((BF)b)->body,mpfr_roundmode);
      break;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  }
  if ( !cmpbf(*c,0) ) *c = 0;
}

void pwrbf(Num a,Num b,Num *c)
{
  int prec;
  mpfr_t r;
  GZ z;
  BF d;

  if ( !b )
    *c = (Num)ONE;
  else if ( !a )
    *c = 0;
  else if ( (NID(a) <= N_A) && (NID(b) <= N_A ) )
    (*pwrnumt[MIN(NID(a),NID(b))])(a,b,c);
  else if ( NID(a) == N_B ) {
    switch ( NID(b) ) {
    case N_Q:
      mpfr_init2(r,BFPREC(a));
      if ( INT((Q)b) ) {
        z = ztogz((Q)b);
        mpfr_pow_z(r,((BF)a)->body,z->body,mpfr_roundmode);
      } else {
        b = tobf(b,BFPREC(a));
        mpfr_pow(r,((BF)a)->body,((BF)b)->body,mpfr_roundmode);
      }
      break;
    case N_R:
      /* double precision = 53 */
      prec = MAX(BFPREC(a),53);
      mpfr_init2(r,prec);
      b = tobf(b,prec);
      mpfr_pow(r,((BF)a)->body,((BF)b)->body,mpfr_roundmode);
      break;
    case N_B:
      mpfr_init2(r,MAX(BFPREC(a),BFPREC(b)));
      mpfr_pow(r,((BF)a)->body,((BF)b)->body,mpfr_roundmode);
      break;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  } else { /* NID(b)==N_B */
    switch ( NID(a) ) {
    case N_Q:
      mpfr_init2(r,BFPREC(b));
      a = tobf(a,BFPREC(b));
      mpfr_pow(r,((BF)a)->body,((BF)b)->body,mpfr_roundmode);
      break;
    case N_R:
      /* double precision = 53 */
      prec = MAX(BFPREC(a),53);
      mpfr_init2(r,prec);
      a = tobf(a,prec);
      mpfr_pow(r,((BF)a)->body,((BF)b)->body,mpfr_roundmode);
      break;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  }
  if ( !cmpbf(*c,0) ) *c = 0;
}

void chsgnbf(Num a,Num *c)
{
  mpfr_t r;
  BF d;

  if ( !a )
    *c = 0;
  else if ( NID(a) <= N_A )
    (*chsgnnumt[NID(a)])(a,c);
  else {
    mpfr_init2(r,BFPREC(a));
    mpfr_neg(r,((BF)a)->body,mpfr_roundmode);
    MPFRTOBF(r,d);
    *c = (Num)d;
  }
}

int cmpbf(Num a,Num b)
{
  int ret;
  GZ z;
  GQ q;

  if ( !a ) {
    if ( !b ) return 0;
    else if ((NID(b)<=N_A) )
      return (*cmpnumt[NID(b)])(a,b);
    else
      return -mpfr_sgn(((BF)a)->body);
  } else if ( !b ) {
    if ( !a || (NID(a)<=N_A) )
      return (*cmpnumt[NID(a)])(a,b);
    else
      return mpfr_sgn(((BF)a)->body);
  } else if ( NID(a) == N_B ) {
    switch ( NID(b) ) {
    case N_Q:
      if ( INT((Q)b) ) {
        z = ztogz((Q)b);
        ret = mpfr_cmp_z(((BF)a)->body,z->body);
      } else {
        q = qtogq((Q)b);
        ret = mpfr_cmp_q(((BF)a)->body,q->body);
      }
      break;
    case N_R:
      /* double precision = 53 */
      ret = mpfr_cmp_d(((BF)a)->body,((Real)b)->body);
      break;
    case N_B:
      ret = mpfr_cmp(((BF)a)->body,((BF)b)->body);
      break;
    }
    return ret;
  } else { /* NID(b)==N_B */
    switch ( NID(a) ) {
    case N_Q:
      if ( INT((Q)a) ) {
        z = ztogz((Q)a);
        ret = mpfr_cmp_z(((BF)b)->body,z->body);
      } else {
        q = qtogq((Q)a);
        ret = mpfr_cmp_q(((BF)b)->body,q->body);
      }
      break;
    case N_R:
      /* double precision = 53 */
      ret = mpfr_cmp_d(((BF)b)->body,((Real)a)->body);
      break;
    }
    return -ret;
  }
}
