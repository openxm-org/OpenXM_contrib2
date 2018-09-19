/*
 * $OpenXM$
 */
#include "ca.h"
#include "base.h"
#include <math.h>

extern int mpfr_roundmode;

Num tobf(Num,int);

#define BFPREC(a) (BDY((BF)(a))->_mpfr_prec)

void strtobf(char *s,BF *p)
{
  BF r;
  NEWBF(r);
  mpfr_init(BDY(r));
  mpfr_set_str(BDY(r),s,10,mpfr_roundmode);
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
      mpfr_init_set_d(r,BDY((Real)a),mpfr_roundmode);
      MPFRTOBF(r,d);
      return (Num)d;
      break;
    case N_Q:
      if ( INT(a) )
        mpfr_init_set_z(r,BDY((Z)a),mpfr_roundmode);
      else
        mpfr_init_set_q(r,BDY((Q)a),mpfr_roundmode);
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
      return 0;
      break;
    }
  }
}

void addbf(Num a,Num b,Num *c)
{
  mpfr_t r;
  BF d;

  if ( !a )
    *c = b;
  else if ( !b )
    *c = a;
  else if ( (NID(a) <= N_R) && (NID(b) <= N_R ) )
    (*addnumt[MAX(NID(a),NID(b))])(a,b,c);
  else if ( NID(a) == N_B ) {
    switch ( NID(b) ) {
    case N_Q:
      mpfr_init2(r,BFPREC(a));
      if ( INT(b) ) 
        mpfr_add_z(r,BDY((BF)a),BDY((Z)b),mpfr_roundmode);
      else
        mpfr_add_q(r,BDY((BF)a),BDY((Q)b),mpfr_roundmode);
      break;
    case N_R:
      /* double precision = 53 */
      mpfr_init2(r,MAX(BFPREC(a),53));
      mpfr_add_d(r,BDY((BF)a),BDY((Real)b),mpfr_roundmode);
      break;
    case N_B:
      mpfr_init2(r,MAX(BFPREC(a),BFPREC(b)));
      mpfr_add(r,BDY((BF)a),BDY((BF)b),mpfr_roundmode);
      break;
    default:
      goto err;
      break;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  } else if ( NID(b) == N_B ) {
    switch ( NID(a) ) {
    case N_Q:
      mpfr_init2(r,BFPREC(b));
      if ( INT(a) ) 
        mpfr_add_z(r,BDY((BF)b),BDY((Z)a),mpfr_roundmode);
      else
        mpfr_add_q(r,BDY((BF)b),BDY((Q)a),mpfr_roundmode);
      break;
    case N_R:
      /* double precision = 53 */
      mpfr_init2(r,MAX(BFPREC(b),53));
      mpfr_add_d(r,BDY((BF)b),BDY((Real)a),mpfr_roundmode);
      break;
    default:
      goto err;
      break;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  } else
    goto err;
  if ( !cmpbf(*c,0) ) *c = 0;
  return;

err: error("addbf : invalid argument");
}

void subbf(Num a,Num b,Num *c)
{
  mpfr_t r,s;
  BF d;

  if ( !a )
    (*chsgnnumt[NID(b)])(b,c);
  else if ( !b )
    *c = a;
  else if ( (NID(a) <= N_R) && (NID(b) <= N_R ) )
    (*subnumt[MAX(NID(a),NID(b))])(a,b,c);
  else if ( NID(a) == N_B ) {
    switch ( NID(b) ) {
    case N_Q:
      mpfr_init2(r,BFPREC(a));
      if ( INT(b) )
        mpfr_sub_z(r,BDY((BF)a),BDY((Z)b),mpfr_roundmode);
      else
        mpfr_sub_q(r,BDY((BF)a),BDY((Q)b),mpfr_roundmode);
      break;
    case N_R:
      /* double precision = 53 */
      mpfr_init2(r,MAX(BFPREC(a),53));
      mpfr_sub_d(r,BDY((BF)a),BDY((Real)b),mpfr_roundmode);
      break;
    case N_B:
      mpfr_init2(r,MAX(BFPREC(a),BFPREC(b)));
      mpfr_sub(r,BDY((BF)a),BDY((BF)b),mpfr_roundmode);
      break;
    default:
      goto err;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  } else if ( NID(b)==N_B ) {
    switch ( NID(a) ) {
    case N_Q:
      mpfr_init2(r,BFPREC(b));
      if ( INT(a) )
        mpfr_sub_z(r,BDY((BF)b),BDY((Z)a),mpfr_roundmode);
      else
        mpfr_sub_q(r,BDY((BF)b),BDY((Q)a),mpfr_roundmode);
      mpfr_neg(r,r,mpfr_roundmode);
      break;
    case N_R:
      /* double precision = 53 */
      mpfr_init2(r,MAX(BFPREC(b),53));
      mpfr_d_sub(r,BDY((Real)a),BDY((BF)b),mpfr_roundmode);
      break;
    default:
      goto err;
    }

    MPFRTOBF(r,d);
    *c = (Num)d;
  } else
    goto err;
  if ( !cmpbf(*c,0) ) *c = 0;
  return;

err: error("subbf : invalid argument");
}

void mulbf(Num a,Num b,Num *c)
{
  mpfr_t r;
  BF d;
  int prec;

  if ( !a || !b )
    *c = 0;
  else if ( (NID(a) <= N_R) && (NID(b) <= N_R ) )
    (*mulnumt[MAX(NID(a),NID(b))])(a,b,c);
  else if ( NID(a) == N_B ) {
    switch ( NID(b) ) {
    case N_Q:
      mpfr_init2(r,BFPREC(a));
      if ( INT(b) )
        mpfr_mul_z(r,BDY((BF)a),BDY((Z)b),mpfr_roundmode);
      else
        mpfr_mul_q(r,BDY((BF)a),BDY((Q)b),mpfr_roundmode);
      break;
    case N_R:
      /* double precision = 53 */
      mpfr_init2(r,MAX(BFPREC(a),53));
      mpfr_mul_d(r,BDY((BF)a),BDY((Real)b),mpfr_roundmode);
      break;
    case N_B:
      mpfr_init2(r,MAX(BFPREC(a),BFPREC(b)));
      mpfr_mul(r,BDY((BF)a),BDY((BF)b),mpfr_roundmode);
      break;
    default:
      goto err;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  } else if ( NID(b) == N_B ) {
    switch ( NID(a) ) {
    case N_Q:
      mpfr_init2(r,BFPREC(b));
      if ( INT(a) )
        mpfr_mul_z(r,BDY((BF)b),BDY((Z)a),mpfr_roundmode);
      else
        mpfr_mul_q(r,BDY((BF)b),BDY((Q)a),mpfr_roundmode);
      break;
    case N_R:
      /* double precision = 53 */
      mpfr_init2(r,MAX(BFPREC(b),53));
      mpfr_mul_d(r,BDY((BF)b),BDY((Real)a),mpfr_roundmode);
      break;
    default:
      goto err;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  } else
    goto err;

  if ( !cmpbf(*c,0) ) *c = 0;
  return;

err: error("mulbf : invalid argument");
}

void divbf(Num a,Num b,Num *c)
{
  mpfr_t s,r;
  BF d;

  if ( !b )
    error("divbf : division by 0");
  else if ( !a )
    *c = 0;
  else if ( (NID(a) <= N_R) && (NID(b) <= N_R ) )
    (*divnumt[MAX(NID(a),NID(b))])(a,b,c);
  else if ( NID(a) == N_B ) {
    switch ( NID(b) ) {
    case N_Q:
      mpfr_init2(r,BFPREC(a));
      if ( INT(b) )
        mpfr_div_z(r,BDY((BF)a),BDY((Z)b),mpfr_roundmode);
      else
        mpfr_div_q(r,BDY((BF)a),BDY((Q)b),mpfr_roundmode);
      break;
    case N_R:
      /* double precision = 53 */
      mpfr_init2(r,MAX(BFPREC(a),53));
      mpfr_div_d(r,BDY((BF)a),BDY((Real)b),mpfr_roundmode);
      break;
    case N_B:
      mpfr_init2(r,MAX(BFPREC(a),BFPREC(b)));
      mpfr_div(r,BDY((BF)a),BDY((BF)b),mpfr_roundmode);
      break;
    default:
      goto err;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  } else if ( NID(b)==N_B ) {
    switch ( NID(a) ) {
    case N_Q:
      /* XXX : mpfr_z_div and mpfr_q_div are not implemented */
      a = tobf(a,BFPREC(b));
      mpfr_init2(r,BFPREC(b));
      mpfr_div(r,BDY((BF)a),BDY((BF)b),mpfr_roundmode);
      break;
    case N_R:
      /* double precision = 53 */
      mpfr_init2(r,MAX(BFPREC(b),53));
      mpfr_d_div(r,BDY((Real)a),BDY((BF)b),mpfr_roundmode);
      break;
    default:
      goto err;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  } else
    goto err;

  if ( !cmpbf(*c,0) ) *c = 0;
  return;

err: error("mulbf : invalid argument");
}

void pwrbf(Num a,Num b,Num *c)
{
  int prec;
  mpfr_t r;
  BF d;

  if ( !b )
    *c = (Num)ONE;
  else if ( !a )
    *c = 0;
  else if ( (NID(a) <= N_R) && (NID(b) <= N_R ) )
    (*pwrnumt[MAX(NID(a),NID(b))])(a,b,c);
  else if ( NID(a) == N_B ) {
    switch ( NID(b) ) {
    case N_Q:
      mpfr_init2(r,BFPREC(a));
      if ( INT(b) ) {
        mpfr_pow_z(r,BDY((BF)a),BDY((Z)b),mpfr_roundmode);
      } else {
        b = tobf(b,BFPREC(a));
        mpfr_pow(r,BDY((BF)a),BDY((BF)b),mpfr_roundmode);
      }
      break;
    case N_R:
      /* double precision = 53 */
      prec = MAX(BFPREC(a),53);
      mpfr_init2(r,prec);
      b = tobf(b,prec);
      mpfr_pow(r,BDY((BF)a),BDY((BF)b),mpfr_roundmode);
      break;
    case N_B:
      mpfr_init2(r,MAX(BFPREC(a),BFPREC(b)));
      mpfr_pow(r,BDY((BF)a),BDY((BF)b),mpfr_roundmode);
      break;
    default:
      goto err;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  } else if ( NID(b)==N_B ) {
    switch ( NID(a) ) {
    case N_Q:
      mpfr_init2(r,BFPREC(b));
      a = tobf(a,BFPREC(b));
      mpfr_pow(r,BDY((BF)a),BDY((BF)b),mpfr_roundmode);
      break;
    case N_R:
      /* double precision = 53 */
      prec = MAX(BFPREC(a),53);
      mpfr_init2(r,prec);
      a = tobf(a,prec);
      mpfr_pow(r,BDY((BF)a),BDY((BF)b),mpfr_roundmode);
      break;
    default:
      goto err;
    }
    MPFRTOBF(r,d);
    *c = (Num)d;
  } else
    goto err;

  if ( !cmpbf(*c,0) ) *c = 0;
  return;

err: error("pwrbf : invalid argument");
}

void chsgnbf(Num a,Num *c)
{
  mpfr_t r;
  BF d;

  if ( !a )
    *c = 0;
  else if ( NID(a) <= N_R )
    (*chsgnnumt[NID(a)])(a,c);
  else if ( NID(a) == N_B ) {
    mpfr_init2(r,BFPREC(a));
    mpfr_neg(r,BDY((BF)a),mpfr_roundmode);
    MPFRTOBF(r,d);
    *c = (Num)d;
  } else
    error("chsgnbf : invalid argument");
}

int cmpbf(Num a,Num b)
{
  int ret;

  if ( !a ) {
    if ( !b ) return 0;
    else if ( NID(b)<=N_R )
      return (*cmpnumt[NID(b)])(a,b);
    else if ( NID(b)==N_B )
      return -mpfr_sgn(BDY((BF)b));
    else
      goto err;
  } else if ( !b ) {
    if ( NID(a)<=N_R )
      return (*cmpnumt[NID(a)])(a,b);
    else if ( NID(a)==N_B )
      return mpfr_sgn(BDY((BF)a));
    else
      goto err;
  } else if ( NID(a) <= N_R && NID(b) <= N_R )
    return (*cmpnumt[MAX(NID(a),NID(b))])(a,b);
  else if ( NID(a) == N_B ) {
    switch ( NID(b) ) {
    case N_Q:
      if ( INT(b) )
        ret = mpfr_cmp_z(BDY((BF)a),BDY((Z)b));
      else
        ret = mpfr_cmp_q(BDY((BF)a),BDY((Q)b));
      break;
    case N_R:
      /* double precision = 53 */
      ret = mpfr_cmp_d(BDY((BF)a),BDY((Real)b));
      break;
    case N_B:
      ret = mpfr_cmp(BDY((BF)a),BDY((BF)b));
      break;
    default:
      goto err;
    }
    return ret;
  } else if ( NID(b)==N_B ) {
    switch ( NID(a) ) {
    case N_Q:
      if ( INT(a) )
        ret = mpfr_cmp_z(BDY((BF)b),BDY((Z)a));
      else
        ret = mpfr_cmp_q(BDY((BF)b),BDY((Q)a));
      break;
    case N_R:
      /* double precision = 53 */
      ret = mpfr_cmp_d(BDY((BF)b),BDY((Real)a));
      break;
    default:
      goto err;
    }
    return -ret;
  }
err: error("cmpbf : cannot compare");
  return 0;
}
