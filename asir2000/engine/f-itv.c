/*
 * $OpenXM: OpenXM_contrib2/asir2000/engine/f-itv.c,v 1.9 2019/06/04 07:11:22 kondoh Exp $
*/
#if defined(INTERVAL)
#include "ca.h"
#include "base.h"
#if 0
#if defined(PARI)
#include "genpari.h"
#include "itv-pari.h"
long get_pariprec();
#endif

void ToBf(Num a, BF *rp)
{
  GEN  pa, pb, pc;
  BF  bn, bd;
  BF  c;
  long  ltop, lbot;

  if ( ! a ) {
    *rp = 0;
  }
  else switch ( NID(a) ) {
    case N_Q:
      ltop = avma;
      ritopa_i(NM((Q)a), SGN((Q)a), &pa);
      pb = cgetr(get_pariprec());
      mpaff(pa, pb);
      if ( INT((Q)a) ) {
        lbot = avma;
        pb = gerepile(ltop, lbot, pb);
        patori(pb, &c);
        cgiv(pb);
        *rp = c;
      } else {
        patori(pb, &bn);
        ritopa_i(DN((Q)a), 1, &pa);
        pb = cgetr(get_pariprec());
        mpaff(pa, pb);
        lbot = avma;
        pb = gerepile(ltop, lbot, pb);
        patori(pb, &bd);
        cgiv(pb);
        divbf((Num)bn,(Num)bd, (Num *)&c);
        *rp = c;
      }
      break;
    case N_R:
      pb = dbltor(BDY((Real)a));
      patori(pb, &c);
      cgiv(pb);
      *rp = c;
      break;
    case N_B:
      *rp = (BF)a;
      break;
    default:
      error("ToBf : not implemented");
      break;
  }
}

void double2bf(double d, BF *rp)
{
  GEN  p;

  p = dbltor(d);
  patori(p, rp);
  cgiv(p);
}

double  bf2double(BF a)
{
  GEN  p;

  ritopa(a, &p);
  return rtodbl(p);
}

void getulp(BF a, BF *au)
{
  GEN  b, c;
  long  lb, i;

  ritopa(a, &b);
  lb = lg(b);
  c = cgetr(lb);
  c[1] = b[1];
  for (i=2;i<lb-1;i++) c[i] = 0;
  c[i] = 1;
  setsigne(c, 1);
  patori(c,au);
  cgiv(c);
  cgiv(b);
}

void addulp(IntervalBigFloat a, IntervalBigFloat *c)
{
  Num  ai, as, aiu, asu, inf, sup;

  itvtois((Itv)a,&ai,&as);
  if ( ai ) {
    getulp((BF)ai, (BF *)&aiu);
    subbf(ai,aiu,&inf);
  } else  inf = ai;
  if ( as ) {
    getulp((BF)as, (BF *)&asu);
    addbf(as,asu,&sup);
  } else  sup = as;
  istoitv(inf,sup, (Itv *)c);
}
#endif

// in builtin/bfaux.c
extern int mpfr_roundmode;

// in engine/bf.c
Num tobf(Num,int);

#define BFPREC(a) (((BF)(a))->body->_mpfr_prec)

double mpfr2dblDown(mpfr_t a)
{
	return mpfr_get_d(a,MPFR_RNDD);
}

double mpfr2dblUp(mpfr_t a)
{
	return mpfr_get_d(a,MPFR_RNDU);
}


void toInterval(Num a, int prec, int type, Num *rp)
{
	if ( ! a ) {
		*rp = 0;
	} else if (type == EvalIntervalDouble) {
		if (NID(a)==N_C) {
			double inf,sup;
			C z;
			IntervalDouble re, im;

			if ( ! ((C)a)->r ) {
				re = 0;
			} else {
				inf = toRealDown(((C)a)->r);
				sup = toRealUp(((C)a)->r);
				MKIntervalDouble(inf,sup,re);
            }
			if ( ! ((C)a)->i ) {
				im = 0;
			} else {
				inf = toRealDown(((C)a)->i);
				sup = toRealUp(((C)a)->i);
				MKIntervalDouble(inf,sup,im);
			}
			if ( !re && !im )
				z = 0;
			else
				reimtocplx((Num)re,(Num)im,(Num *)&z);
			*rp = (Num)z;
		} else {
			double inf,sup;
			IntervalDouble c;
	
			inf = toRealDown(a);
			sup = toRealUp(a);
	
			MKIntervalDouble(inf,sup,c);
			*rp = (Num) c;
		}
	} else if (type == EvalIntervalBigFloat) {
		if (NID(a)==N_C) {
			Num ai,as;
			Num inf,sup;
			C z;
			IntervalBigFloat re, im;
			int current_roundmode;

			current_roundmode = mpfr_roundmode;

			if ( ! ((C)a)->r ) 
				re = 0;
			else {
				itvtois((Itv)((C)a)->r,&ai,&as);
    			mpfr_roundmode = MPFR_RNDD;
				inf = tobf(ai, prec);
				mpfr_roundmode = MPFR_RNDU;
				sup = tobf(as, prec);
				istoitv(inf,sup,(Itv *)&re);
			}

			if ( ! ((C)a)->i ) 
				im = 0;
			else {
				itvtois((Itv)((C)a)->i,&ai,&as);
    			mpfr_roundmode = MPFR_RNDD;
				inf = tobf(ai, prec);
				mpfr_roundmode = MPFR_RNDU;
				sup = tobf(as, prec);
				istoitv(inf,sup,(Itv *)&im);
			}

    		mpfr_roundmode = current_roundmode;	
			reimtocplx((Num)re,(Num)im,(Num *)&z);
			*rp = (Num)z;
		} else {
			Num ai,as;
			Num inf,sup;
			IntervalBigFloat c;
			int current_roundmode;

			itvtois((Itv)a,&ai,&as);

			current_roundmode = mpfr_roundmode;
    		mpfr_roundmode = MPFR_RNDD;
			inf = tobf(ai, prec);
			mpfr_roundmode = MPFR_RNDU;
			sup = tobf(as, prec);
			istoitv(inf,sup,(Itv *)&c);
    		mpfr_roundmode = current_roundmode;	
			*rp = (Num) c;
		}
	} else {
		error("toInterval: not supported types.");
		*rp = 0;
	}
}


void additvf(IntervalBigFloat a, IntervalBigFloat b, IntervalBigFloat *rp)
{
  Num  ai,as,bi,bs;
  IntervalBigFloat c;
//,mas,mbs;
//,tmp;
  Num  inf,sup;
  //GEN  pa, pb, z;
  //long  ltop, lbot;
  int current_roundmode;

  if ( !a )
    *rp = b;
  else if ( !b )
    *rp = a;
  else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
    addnum(0,(Num)a,(Num)b,(Num *)rp);
  else {
    itvtois((Itv)a,&ai,&as);
    itvtois((Itv)b,&bi,&bs);

    current_roundmode = mpfr_roundmode;

    mpfr_roundmode = MPFR_RNDD;
    ai = tobf(ai, DEFAULTPREC);
    bi = tobf(bi, DEFAULTPREC);
    //addnum(0,ai,bi,&inf);
    addbf(ai,bi,&inf);

    mpfr_roundmode = MPFR_RNDU;
    as = tobf(as, DEFAULTPREC);
    bs = tobf(bs, DEFAULTPREC);
    //addnum(0,as,bs,&sup);
    addbf(as,bs,&sup);

    istoitv(inf,sup,(Itv *)&c);
    mpfr_roundmode = current_roundmode;
    //MKIntervalBigFloat((BF)inf, (BF)sup, c);
    *rp = c;
#if 0
printexpr(CO, ai);
printexpr(CO, as);
printexpr(CO, bi);
printexpr(CO, bs);
#endif
    return;
  }
}

void subitvf(IntervalBigFloat a, IntervalBigFloat b, IntervalBigFloat *rp)
{
  Num  ai,as,bi,bs;
  IntervalBigFloat c;
//,mas, mbs;
  Num  inf,sup;
//,tmp;
  //GEN  pa, pb, z;
  //long  ltop, lbot;
  int current_roundmode;

  if ( !a )
    chsgnnum((Num)b,(Num *)rp);
  else if ( !b )
    *rp = a;
  else if ( (NID(a) < N_IntervalBigFloat) && (NID(b) < N_IntervalBigFloat ) )
    subnum(0,(Num)a,(Num)b,(Num *)rp);
  else {
    itvtois((Itv)a,&ai,&as);
    itvtois((Itv)b,&bi,&bs);

    current_roundmode = mpfr_roundmode;

    mpfr_roundmode = MPFR_RNDD;
    ai = tobf(ai, DEFAULTPREC);
    bi = tobf(bi, DEFAULTPREC);

    mpfr_roundmode = MPFR_RNDU;
    as = tobf(as, DEFAULTPREC);
    bs = tobf(bs, DEFAULTPREC);
    //subnum(0,as,bi,&sup);
    subbf(as,bi,&sup);

    mpfr_roundmode = MPFR_RNDD;
    //subnum(0,ai,bs,&inf);
    subbf(ai,bs,&inf);

    istoitv(inf,sup,(Itv *)&c);
    mpfr_roundmode = current_roundmode;
    //MKIntervalBigFloat((BF)inf, (BF)sup, c);
    *rp = c;

    //addulp((IntervalBigFloat)tmp, c);
  }
}

void mulitvf(IntervalBigFloat a, IntervalBigFloat b, IntervalBigFloat *c)
{
  Num  ai,as,bi,bs,a1,a2,b1,b2,c1,c2,p,t,tmp;
  Num  inf, sup;
  int  ba,bb;
  int current_roundmode;

  if ( !a || !b )
    *c = 0;
  else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
    mulnum(0,(Num)a,(Num)b,(Num *)c);
  else {
    itvtois((Itv)a,&ai,&as);
    itvtois((Itv)b,&bi,&bs);

    current_roundmode = mpfr_roundmode;

    mpfr_roundmode = MPFR_RNDU;
    as = tobf(as, DEFAULTPREC);
    bs = tobf(bs, DEFAULTPREC);

    mpfr_roundmode = MPFR_RNDD;
    ai = tobf(ai, DEFAULTPREC);
    bi = tobf(bi, DEFAULTPREC);

//    itvtois((Itv)a,&inf,&sup);
//    ToBf(inf, (BF *)&ai); ToBf(sup, (BF *)&as);
//    itvtois((Itv)b,&inf,&sup);
//    ToBf(inf, (BF *)&bi); ToBf(sup, (BF *)&bs);

  /* MUST check if ai, as, bi, bs are bigfloat. */
    chsgnnum(as,&a2);
    chsgnnum(bs,&b2);


    if ( ba = (compnum(0,0,a2) > 0) ) {
      a1 = ai;
    } else {
      a1 = a2;
      a2 = ai;
    }
    if ( bb = (compnum(0,0,b2) > 0) ) {
      b1 = bi;
    } else {
      b1 = b2;
      b2 = bi;
    }
    mulnum(0,a2,b2,&t);
    subnum(0,0,t,&c2);
    if ( compnum(0,0,b1) > 0 ) {
      mulnum(0,a2,b1,&t);
      subnum(0,0,t,&c1);
      if ( compnum(0,0,a1) > 0 ) {
        mulnum(0,a1,b2,&t);
        subnum(0,0,t,&p);
        if ( compnum(0,c1,p) > 0 ) c1 = p;
        mulnum(0,a1,b1,&t);
        subnum(0,0,t,&p);
        if ( compnum(0,c2,p) > 0 ) c2 = p;
      }
    } else {
      if ( compnum(0,0,a1) > 0 ) {
        mulnum(0,a1,b2,&t);
        subnum(0,0,t,&c1);
      } else {
        mulnum(0,a1,b1,&c1);
      }
    }
    if ( ba == bb ) {
      subnum(0,0,c2,&t);
      istoitv(c1,t,(Itv *)c);
    } else {
      subnum(0,0,c1,&t);
      istoitv(c2,t,(Itv *)c);
    }
    //addulp((IntervalBigFloat)tmp, c);
  }
    mpfr_roundmode = current_roundmode;
}

int     initvf(Num a, Itv b)
{
  Num inf, sup;

  itvtois(b, &inf, &sup);
  if ( compnum(0,inf,a) > 0 ) return 0;
  else if ( compnum(0,a,sup) > 0 ) return 0;
  else return 1;
}

int     itvinitvf(Itv a, Itv b)
{
  Num ai,as,bi,bs;

  itvtois(a, &ai, &as);
  itvtois(b, &bi, &bs);
  if ( compnum(0,ai,bi) >= 0  && compnum(0,bs,as) >= 0 ) return 1;
  else return 0;
}

void divitvf(IntervalBigFloat a, IntervalBigFloat b, IntervalBigFloat *c)
{
  Num  ai,as,bi,bs,a1,a2,b1,b2,c1,c2,t,tmp;
  Num  inf, sup;
  int  ba,bb;
  int current_roundmode;

  if ( !b )
    error("divitv : division by 0");
  else if ( !a )
    *c = 0;
  else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
    divnum(0,(Num)a,(Num)b,(Num *)c);
  else {
    itvtois((Itv)a,&ai,&as);
    itvtois((Itv)b,&bi,&bs);

    current_roundmode = mpfr_roundmode;

    mpfr_roundmode = MPFR_RNDU;
    as = tobf(as, DEFAULTPREC);
    bs = tobf(bs, DEFAULTPREC);

    mpfr_roundmode = MPFR_RNDD;
    ai = tobf(ai, DEFAULTPREC);
    bi = tobf(bi, DEFAULTPREC);

//    itvtois((Itv)a,&inf,&sup);
//    ToBf(inf, (BF *)&ai); ToBf(sup, (BF *)&as);
//    itvtois((Itv)b,&inf,&sup);
//    ToBf(inf, (BF *)&bi); ToBf(sup, (BF *)&bs);
/* MUST check if ai, as, bi, bs are bigfloat. */
    chsgnnum(as,&a2);
    chsgnnum(bs,&b2);
    if ( ba = (compnum(0,0,a2) > 0) ) {
      a1 = ai;
    } else {
      a1 = a2;
      a2 = ai;
    }
    if ( bb = (compnum(0,0,b2) > 0) ) {
      b1 = bi;
    } else {
      b1 = b2;
      b2 = bi;
    }
    if ( compnum(0,0,b1) >= 0 ) {
      error("divitv : division by interval including 0.");
    } else {
      divnum(0,a2,b1,&c2);
      if ( compnum(0,0,a1) > 0 ) {
        divnum(0,a1,b1,&c1);
      } else {
        divnum(0,a1,b2,&t);
        subnum(0,0,t,&c1);
      }
    }
    if ( ba == bb ) {
      subnum(0,0,c2,&t);
      istoitv(c1,t,(Itv *)c);
    } else {
      subnum(0,0,c1,&t);
      istoitv(c2,t,(Itv *)c);
    }
    //addulp((IntervalBigFloat)tmp, c);
  }
    mpfr_roundmode = current_roundmode;
}

void pwritvf(Itv a, Num e, Itv *c)
{
  int ei;
  Itv t;

  if ( !e )
    *c = (Itv)ONE;
  else if ( !a )
    *c = 0;
  else if ( NID(a) <= N_B )
    pwrnum(0,(Num)a,e,(Num *)c);
  else if ( !INT(e) ) {
#if defined(PARI) && 0
    gpui_ri((Obj)a,(Obj)c,(Obj *)c);
#else
    error("pwritvf() : can't calculate a fractional power");
#endif
  } else {
    ei = QTOS((Q)e);
    if (ei<0) ei = -ei;
    pwritv0f(a,ei,&t);
    if ( SGN((Q)e) < 0 )
        divitvf((IntervalBigFloat)ONE,(IntervalBigFloat)t,(IntervalBigFloat *)c); /* bug ?? */
    else
      *c = t;
  }
}

void pwritv0f(Itv a, int e, Itv *c)
{
  Num inf, sup;
  Num ai,Xmin,Xmax;
  IntervalBigFloat tmp;
  Q  ne;
  int current_roundmode;

  if ( e == 1 )
    *c = a;
  else {
    STOQ(e,ne);
    if ( !(e%2) ) {
      if ( initvp(0,a) ) {
        Xmin = 0;
        chsgnnum(INF(a),&ai);
        if ( compnum(0,ai,SUP(a)) > 0 ) {
          Xmax = ai;
        } else {
          Xmax = SUP(a);
        }
      } else {
        if ( compnum(0,INF(a),0) > 0 ) {
          Xmin = INF(a);
          Xmax = SUP(a);
        } else {
          Xmin = SUP(a);
          Xmax = INF(a);
        }
      }
    } else {
      Xmin = INF(a);
      Xmax = SUP(a);
    }

    current_roundmode = mpfr_roundmode;
    if ( ! Xmin )  inf = 0;
    else {
      mpfr_roundmode = MPFR_RNDD;
      pwrbf(Xmin,(Num)ne,&inf);
    }
    if ( ! Xmax )   sup = 0;
    else {
      mpfr_roundmode = MPFR_RNDU;
      pwrbf(Xmax,(Num)ne,&sup);
    }
    istoitv(inf,sup,(Itv *)&tmp);
    mpfr_roundmode = current_roundmode;
    *c = (Itv)tmp;
 //   addulp(tmp, (IntervalBigFloat *)c);
  }
}

void chsgnitvf(IntervalBigFloat a, IntervalBigFloat *rp)
{
  Num inf,sup;
  IntervalBigFloat c;
  int current_roundmode;

  if ( !a )
    *rp = 0;
  else if ( NID(a) < N_IntervalBigFloat )
    chsgnnum((Num)a,(Num *)rp);
  else {
    current_roundmode = mpfr_roundmode;

    mpfr_roundmode = MPFR_RNDU;
    chsgnnum((Num)INF(a),&sup);
    mpfr_roundmode = MPFR_RNDD;
    chsgnnum((Num)SUP(a),&inf);
    //MKIntervalBigFloat((BF)inf,(BF)sup,c);
    istoitv(inf,sup,(Itv *)&c);
    *rp = c;
    mpfr_roundmode = current_roundmode;
  }
}

int cmpitvf(Itv a, Itv b)
{
  Num  ai,as,bi,bs;
  int  s,t;

  if ( !a ) {
    if ( !b || (NID(b)<=N_B) ) {
      return compnum(0,(Num)a,(Num)b);
    } else {
      itvtois(b,&bi,&bs);
      if ( compnum(0,(Num)a,bs) > 0 ) return 1;
      else if ( compnum(0,bi,(Num)a) > 0 ) return -1;
      else  return 0;
    }
  } else if ( !b ) {
    if ( !a || (NID(a)<=N_B) ) {
      return compnum(0,(Num)a,(Num)b);
    } else {
      itvtois(a,&ai,&as);
      if ( compnum(0,ai,(Num)b) > 0 ) return 1;
      else if ( compnum(0,(Num)b,as) > 0 ) return -1;
      else  return 0;
    }
  } else {
    itvtois(a,&ai,&as);
    itvtois(b,&bi,&bs);
    s = compnum(0,ai,bs) ;
    t = compnum(0,bi,as) ;
    if ( s > 0 ) return 1;
    else if ( t > 0 ) return -1;
    else  return 0;
  }
}

void miditvf(Itv a, Num *b)
{
  Num  ai,as;
  Num  t;

  if ( ! a ) *b = 0;
  else if ( (NID(a) <= N_B) )
    *b = (Num)a;
  else {
    STOQ(2,TWO);
    itvtois(a,&ai,&as);
    addbf(ai,as,&t);
    divbf(t,(Num)TWO,b);
  }
}

void cupitvf(Itv a, Itv b, Itv *c)
{
  Num  ai,as,bi,bs;
  Num  inf,sup;

  itvtois(a,&ai,&as);
  itvtois(b,&bi,&bs);
  if ( compnum(0,ai,bi) > 0 )  inf = bi;
  else        inf = ai;
  if ( compnum(0,as,bs) > 0 )  sup = as;
  else        sup = bs;
  istoitv(inf,sup,c);
}

void capitvf(Itv a, Itv b, Itv *c)
{
  Num  ai,as,bi,bs;
  Num  inf,sup;

  itvtois(a,&ai,&as);
  itvtois(b,&bi,&bs);
  if ( compnum(0,ai,bi) > 0 )  inf = ai;
  else        inf = bi;
  if ( compnum(0,as,bs) > 0 )  sup = bs;
  else        sup = as;
  if ( compnum(0,inf,sup) > 0 )  *c = 0;
  else        istoitv(inf,sup,c);
}


void widthitvf(Itv a, Num *b)
{
  Num  ai,as;

  if ( ! a ) *b = 0;
  else if ( (NID(a) <= N_B) )
    *b = (Num)a;
  else {
    itvtois(a,&ai,&as);
    subnum(0,as,ai,b);
  }
}

void absitvf(Itv a, Num *b)
{
  Num  ai,as,bi,bs;

  if ( ! a ) *b = 0;
  else if ( (NID(a) <= N_B) ) {
    if ( compnum(0,(Num)a,0) < 0 ) chsgnnum((Num)a,b);
    else *b = (Num)a;
  } else {
    itvtois(a,&ai,&as);
    if ( compnum(0,ai,0) < 0 ) chsgnnum(ai,&bi);
    else bi = ai;
    if ( compnum(0,as,0) < 0 ) chsgnnum(as,&bs);
    else bs = as;
    if ( compnum(0,bi,bs) > 0 )  *b = bi;
    else        *b = bs;
  }
}

void distanceitvf(Itv a, Itv b, Num *c)
{
  Num  ai,as,bi,bs;
  Num  di,ds;
  Itv  d;

  itvtois(a,&ai,&as);
  itvtois(b,&bi,&bs);
  subnum(0,ai,bi,&di);
  subnum(0,as,bs,&ds);
  istoitv(di,ds,&d);
  absitvf(d,c);
}

#endif
