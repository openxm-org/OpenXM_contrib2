/*
 * $OpenXM: OpenXM_contrib2/asir2000/engine/bf.c,v 1.7 2015/08/04 06:20:45 noro Exp $ 
 */
#include "ca.h"
#include "base.h"
#include <math.h>

Num tobf(Num,int);

#define BFPREC(a) (((BF)(a))->body->_mpfr_prec)

void strtobf(char *s,BF *p)
{
  BF r;
  NEWBF(r);
  mpfr_init(r->body);
  mpfr_set_str(r->body,s,10,MPFR_RNDN);
  *p = r;
}

double mpfrtodbl(mpfr_t a)
{
  return mpfr_get_d(a,MPFR_RNDN);
}

Num tobf(Num a,int prec)
{
  mpfr_t r;
  mpz_t z;
  mpq_t q;
  BF d;
  N nm,dn;
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
      mpfr_init_set_d(r,((Real)a)->body,MPFR_RNDN);
      MPFRTOBF(r,d);
      return (Num)d;
      break;
    case N_Q:
      nm = NM((Q)a); dn = DN((Q)a); sgn = SGN((Q)a);
      if ( INT((Q)a) ) {
        mpz_init(z);
        mpz_import(z,PL(nm),-1,sizeof(BD(nm)[0]),0,0,BD(nm));
        if ( sgn < 0 ) mpz_neg(z,z);
        mpfr_init_set_z(r,z,MPFR_RNDN);
      } else {
        mpq_init(q);
        mpz_import(mpq_numref(q),PL(nm),-1,sizeof(BD(nm)[0]),0,0,BD(nm));
        mpz_import(mpq_denref(q),PL(dn),-1,sizeof(BD(dn)[0]),0,0,BD(dn));
        if ( sgn < 0 ) mpq_neg(q,q);
        mpfr_init_set_q(r,q,MPFR_RNDN);
      }
      MPFRTOBF(r,d);
      return (Num)d;
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
  else {
    if ( NID(a) == N_B ) {
      switch ( NID(b) ) {
      case N_Q:
        mpfr_init2(r,BFPREC(a));
        if ( INT((Q)b) ) {
          z = ztogz((Q)b);
          mpfr_add_z(r,((BF)a)->body,z->body,MPFR_RNDN);
        } else {
          q = qtogq((Q)b);
          mpfr_add_q(r,((BF)a)->body,q->body,MPFR_RNDN);
        }
        break;
      case N_R:
        /* double precision = 53 */
        mpfr_init2(r,MIN(BFPREC(a),53));
        mpfr_add_d(r,((BF)a)->body,((Real)b)->body,MPFR_RNDN);
        break;
      case N_B:
        mpfr_init2(r,MIN(BFPREC(a),BFPREC(b)));
        mpfr_add(r,((BF)a)->body,((BF)b)->body,MPFR_RNDN);
        break;
      }
    } else { /* NID(b)==N_B */
      switch ( NID(a) ) {
      case N_Q:
        mpfr_init2(r,BFPREC(b));
        if ( INT((Q)a) ) {
          z = ztogz((Q)a);
          mpfr_add_z(r,((BF)b)->body,z->body,MPFR_RNDN);
        } else {
          q = qtogq((Q)a);
          mpfr_add_q(r,((BF)b)->body,q->body,MPFR_RNDN);
        }
        break;
      case N_R:
        /* double precision = 53 */
        mpfr_init2(r,MIN(BFPREC(b),53));
        mpfr_add_d(r,((BF)b)->body,((Real)a)->body,MPFR_RNDN);
        break;
      }
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  }
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
  else {
    if ( NID(a) == N_B ) {
      switch ( NID(b) ) {
      case N_Q:
        mpfr_init2(r,BFPREC(a));
        if ( INT((Q)b) ) {
          z = ztogz((Q)b);
          mpfr_sub_z(r,((BF)a)->body,z->body,MPFR_RNDN);
        } else {
          q = qtogq((Q)b);
          mpfr_sub_q(r,((BF)a)->body,q->body,MPFR_RNDN);
        }
        break;
      case N_R:
        /* double precision = 53 */
        mpfr_init2(r,MIN(BFPREC(a),53));
        mpfr_sub_d(r,((BF)a)->body,((Real)b)->body,MPFR_RNDN);
        break;
      case N_B:
        mpfr_init2(r,MIN(BFPREC(a),BFPREC(b)));
        mpfr_sub(r,((BF)a)->body,((BF)b)->body,MPFR_RNDN);
        break;
      }
    } else { /* NID(b)==N_B */
      switch ( NID(a) ) {
      case N_Q:
        mpfr_init2(s,BFPREC(b));
        mpfr_init2(r,BFPREC(b));
        if ( INT((Q)a) ) {
          z = ztogz((Q)a);
          mpfr_sub_z(s,((BF)b)->body,z->body,MPFR_RNDN);
        } else {
          q = qtogq((Q)a);
          mpfr_sub_q(s,((BF)b)->body,q->body,MPFR_RNDN);
        }
        mpfr_neg(r,s,MPFR_RNDN);
        break;
      case N_R:
        /* double precision = 53 */
        mpfr_init2(r,MIN(BFPREC(b),53));
        mpfr_d_sub(r,((Real)a)->body,((BF)b)->body,MPFR_RNDN);
        break;
      }
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  }
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
  else {
    if ( NID(a) == N_B ) {
      switch ( NID(b) ) {
      case N_Q:
        mpfr_init2(r,BFPREC(a));
        if ( INT((Q)b) ) {
          z = ztogz((Q)b);
          mpfr_mul_z(r,((BF)a)->body,z->body,MPFR_RNDN);
        } else {
          q = qtogq((Q)b);
          mpfr_mul_q(r,((BF)a)->body,q->body,MPFR_RNDN);
        }
        break;
      case N_R:
        /* double precision = 53 */
        mpfr_init2(r,MIN(BFPREC(a),53));
        mpfr_mul_d(r,((BF)a)->body,((Real)b)->body,MPFR_RNDN);
        break;
      case N_B:
        mpfr_init2(r,MIN(BFPREC(a),BFPREC(b)));
        mpfr_mul(r,((BF)a)->body,((BF)b)->body,MPFR_RNDN);
        break;
      }
    } else { /* NID(b)==N_B */
      switch ( NID(a) ) {
      case N_Q:
        mpfr_init2(r,BFPREC(b));
        if ( INT((Q)a) ) {
          z = ztogz((Q)a);
          mpfr_mul_z(r,((BF)b)->body,z->body,MPFR_RNDN);
        } else {
          q = qtogq((Q)a);
          mpfr_mul_q(r,((BF)b)->body,q->body,MPFR_RNDN);
        }
        break;
      case N_R:
        /* double precision = 53 */
        mpfr_init2(r,MIN(BFPREC(b),53));
        mpfr_mul_d(r,((BF)b)->body,((Real)a)->body,MPFR_RNDN);
        break;
      }
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  }
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
  else {
    if ( NID(a) == N_B ) {
      switch ( NID(b) ) {
      case N_Q:
        mpfr_init2(r,BFPREC(a));
        if ( INT((Q)b) ) {
          z = ztogz((Q)b);
          mpfr_div_z(r,((BF)a)->body,z->body,MPFR_RNDN);
        } else {
          q = qtogq((Q)b);
          mpfr_div_q(r,((BF)a)->body,q->body,MPFR_RNDN);
        }
        break;
      case N_R:
        /* double precision = 53 */
        mpfr_init2(r,MIN(BFPREC(a),53));
        mpfr_div_d(r,((BF)a)->body,((Real)b)->body,MPFR_RNDN);
        break;
      case N_B:
        mpfr_init2(r,MIN(BFPREC(a),BFPREC(b)));
        mpfr_div(r,((BF)a)->body,((BF)b)->body,MPFR_RNDN);
        break;
      }
    } else { /* NID(b)==N_B */
      switch ( NID(a) ) {
      case N_Q:
        /* XXX : mpfr_z_div and mpfr_q_div are not implemented */
        a = tobf(a,BFPREC(b));
        mpfr_init2(r,BFPREC(b));
        mpfr_div(r,((BF)a)->body,((BF)b)->body,MPFR_RNDN);
        break;
      case N_R:
        /* double precision = 53 */
        mpfr_init2(r,MIN(BFPREC(b),53));
        mpfr_d_div(r,((Real)a)->body,((BF)b)->body,MPFR_RNDN);
        break;
      }
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  }
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
  else {
    if ( NID(a) == N_B ) {
      switch ( NID(b) ) {
      case N_Q:
        mpfr_init2(r,BFPREC(a));
        if ( INT((Q)b) ) {
          z = ztogz((Q)b);
          mpfr_pow_z(r,((BF)a)->body,z->body,MPFR_RNDN);
        } else {
          b = tobf(b,BFPREC(a));
          mpfr_pow(r,((BF)a)->body,((BF)b)->body,MPFR_RNDN);
        }
        break;
      case N_R:
        /* double precision = 53 */
        prec = MIN(BFPREC(a),53);
        mpfr_init2(r,prec);
        b = tobf(b,prec);
        mpfr_pow(r,((BF)a)->body,((BF)b)->body,MPFR_RNDN);
        break;
      case N_B:
        mpfr_init2(r,MIN(BFPREC(a),BFPREC(b)));
        mpfr_pow(r,((BF)a)->body,((BF)b)->body,MPFR_RNDN);
        break;
      }
    } else { /* NID(b)==N_B */
      switch ( NID(a) ) {
      case N_Q:
        mpfr_init2(r,BFPREC(b));
        a = tobf(a,BFPREC(b));
        mpfr_pow(r,((BF)a)->body,((BF)b)->body,MPFR_RNDN);
        break;
      case N_R:
        /* double precision = 53 */
        prec = MIN(BFPREC(a),53);
        mpfr_init2(r,prec);
        a = tobf(a,prec);
        mpfr_pow(r,((BF)a)->body,((BF)b)->body,MPFR_RNDN);
        break;
      }
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  }
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
    mpfr_neg(r,((BF)a)->body,MPFR_RNDN);
    MPFRTOBF(r,d);
    *c = (Num)d;
  }
}

int cmpbf(Num a,Num b)
{
  if ( !a ) {
    if ( !b || (NID(b)<=N_A) )
      return (*cmpnumt[NID(b)])(a,b);
    else
      return -mpfr_sgn(((BF)a)->body);
  } else if ( !b ) {
    if ( !a || (NID(a)<=N_A) )
      return (*cmpnumt[NID(a)])(a,b);
    else
      return mpfr_sgn(((BF)a)->body);
  } else {
    if ( NID(a) != N_B ) a = tobf(a,0);
    if ( NID(b) != N_B ) b = tobf(b,0);
    return mpfr_cmp(((BF)a)->body,((BF)b)->body);
  }
}
