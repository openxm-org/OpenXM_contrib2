/*
 * $OpenXM: OpenXM_contrib2/asir2000/engine/f-itv.c,v 1.1 2000/12/22 10:03:28 saito Exp $
*/
#if defined(INTERVAL)
#include "ca.h"
#include "base.h"
#if PARI
#include "genpari.h"
#include "itv-pari.h"
extern long prec;
#endif

void ToBf(Num a, BF *rp)
{
	GEN	pa, pb, pc;
	BF	bn, bd;
	BF	c;
	long	ltop, lbot;

	if ( ! a ) {
		*rp = 0;
	}
	else switch ( NID(a) ) {
		case N_Q:
			ltop = avma;
			ritopa_i(NM((Q)a), SGN((Q)a), &pa);
			pb = cgetr(prec);
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
				pb = cgetr(prec);
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
	GEN	p;

	p = dbltor(d);
	patori(p, rp);
	cgiv(p);
}

double	bf2double(BF a)
{
	GEN	p;

	ritopa(a, &p);
	return rtodbl(p);
}

void getulp(BF a, BF *au)
{
	GEN	b, c;
	long	lb, i;

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
	Num	ai, as, aiu, asu, inf, sup;

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

void additvf(IntervalBigFloat a, IntervalBigFloat b, IntervalBigFloat *c)
{
	Num	ai,as,bi,bs,mas,mbs,tmp;
	Num	inf,sup;
	GEN	pa, pb, z;
	long	ltop, lbot;

	if ( !a )
		*c = b;
	else if ( !b )
		*c = a;
	else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
		addnum(0,(Num)a,(Num)b,(Num *)c);
	else {
		itvtois((Itv)a,&inf,&sup);
		ToBf(inf, (BF *)&ai); ToBf(sup, (BF *)&as);
		itvtois((Itv)b,&inf,&sup);
		ToBf(inf, (BF *)&bi); ToBf(sup, (BF *)&bs);
#if 0
printexpr(CO, ai);
printexpr(CO, as);
printexpr(CO, bi);
printexpr(CO, bs);
#endif

#if 1
		addnum(0,ai,bi,&inf);
		addnum(0,as,bs,&sup);
		istoitv(inf,sup,(Itv *)&tmp);
		addulp((IntervalBigFloat)tmp, c);
		return;
#else
		ltop = avma;
		ritopa(ai,&pa);
		ritopa(bi,&pb);
		lbot = avma;
		z = gerepile(ltop,lbot,PariAddDown(pa,pb));
		patori(z,&inf); cgiv(z);

	/* MUST check if ai, as, bi, bs are bigfloat. */

		/*  as + bs = ( - ( (-as) + (-bs) ) ) */
		chsgnbf(as,&mas);
		chsgnbf(bs,&mbs);
		ltop = avma;
		ritopa(mas,&pa);
		ritopa(mbs,&pb);
		lbot = avma;
		z = gerepile(ltop,lbot,PariAddDown(pa,pb));
		patori(z,&tmp); cgiv(z);

		chsgnbf(tmp,&sup);
		istoitv(inf,sup,c);
#endif
	}
}

void subitvf(IntervalBigFloat a, IntervalBigFloat b, IntervalBigFloat *c)
{
	Num	ai,as,bi,bs,mas, mbs;
	Num	inf,sup,tmp;
	GEN	pa, pb, z;
	long	ltop, lbot;

	if ( !a )
		chsgnnum((Num)b,(Num *)c);
	else if ( !b )
		*c = a;
	else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
		subnum(0,(Num)a,(Num)b,(Num *)c);
	else {
		itvtois((Itv)a,&inf,&sup);
		ToBf(inf, (BF *)&ai); ToBf(sup, (BF *)&as);
		itvtois((Itv)b,&inf,&sup);
		ToBf(inf, (BF *)&bi); ToBf(sup, (BF *)&bs);
#if 1
		subnum(0,ai,bs,&inf);
		subnum(0,as,bi,&sup);
		istoitv(inf,sup,(Itv *)&tmp);
		addulp((IntervalBigFloat)tmp, c);
#else

/* MUST check if ai, as, bi, bs are bigfloat. */
		/* ai - bs = ai + (-bs)  */
		chsgnbf(bs,&mbs);
		ltop = avma;
		ritopa(ai,&pa);
		ritopa(mbs,&pb);
		lbot = avma;
		z = gerepile(ltop,lbot,PariAddDown(pa,pb));
		patori(z,&inf); cgiv(z);

		/* as - bi = ( - ( bi + (-as) ) ) */
		chsgnbf(as,&mas);
		ltop = avma;
		ritopa(mas,&pa);
		ritopa(bi,&pb);
		lbot = avma;
		z = gerepile(ltop,lbot,PariAddDown(pa,pb));
		patori(z,&tmp); cgiv(z);

		chsgnbf(tmp,&sup);
		istoitv(inf,sup,c);
#endif
	}
}

void mulitvf(IntervalBigFloat a, IntervalBigFloat b, IntervalBigFloat *c)
{
	Num	ai,as,bi,bs,a1,a2,b1,b2,c1,c2,p,t,tmp;
	Num	inf, sup;
	int	ba,bb;

	if ( !a || !b )
		*c = 0;
	else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
		mulnum(0,(Num)a,(Num)b,(Num *)c);
	else {
		itvtois((Itv)a,&inf,&sup);
		ToBf(inf, (BF *)&ai); ToBf(sup, (BF *)&as);
		itvtois((Itv)b,&inf,&sup);
		ToBf(inf, (BF *)&bi); ToBf(sup, (BF *)&bs);

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
			istoitv(c1,t,(Itv *)&tmp);
		} else {
			subnum(0,0,c1,&t);
			istoitv(c2,t,(Itv *)&tmp);
		}
		addulp((IntervalBigFloat)tmp, c);
	}
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
	Num	ai,as,bi,bs,a1,a2,b1,b2,c1,c2,t,tmp;
	Num	inf, sup;
	int	ba,bb;

	if ( !b )
		error("divitv : division by 0");
	else if ( !a )
		*c = 0;
	else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
		divnum(0,(Num)a,(Num)b,(Num *)c);
	else {
		itvtois((Itv)a,&inf,&sup);
		ToBf(inf, (BF *)&ai); ToBf(sup, (BF *)&as);
		itvtois((Itv)b,&inf,&sup);
		ToBf(inf, (BF *)&bi); ToBf(sup, (BF *)&bs);
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
			istoitv(c1,t,(Itv *)&tmp);
		} else {
			subnum(0,0,c1,&t);
			istoitv(c2,t,(Itv *)&tmp);
		}
		addulp((IntervalBigFloat)tmp, c);
	}
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
#if PARI && 0
		GEN pa,pe,z;
		int ltop,lbot;

		ltop = avma; ritopa(a,&pa); ritopa(e,&pe); lbot = avma;
		z = gerepile(ltop,lbot,gpui(pa,pe,prec));
		patori(z,c); cgiv(z);
#else
		error("pwritv : can't calculate a fractional power");
#endif
	} else {
		ei = QTOS((Q)e);
		pwritv0f(a,ei,&t);
		if ( SGN((Q)e) < 0 )
			divbf((Num)ONE,(Num)t,(Num *)c);
		else
			*c = t;
	}
}

void pwritv0f(Itv a, int e, Itv *c)
{
	Num inf, sup;
	Num ai,Xmin,Xmax;
	IntervalBigFloat tmp;
	Q	ne;

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
		if ( ! Xmin )	inf = 0;
		else		pwrbf(Xmin,(Num)ne,&inf);
		if ( ! Xmax )   sup = 0;
		else            pwrbf(Xmax,(Num)ne,&sup);
		istoitv(inf,sup,(Itv *)&tmp);
		addulp(tmp, (IntervalBigFloat *)c);
	}
}

void chsgnitvf(Itv a, Itv *c)
{
	Num inf,sup;

	if ( !a )
		*c = 0;
	else if ( NID(a) <= N_B )
		chsgnnum((Num)a,(Num *)c);
	else {
		chsgnnum(INF((Itv)a),&inf);
		chsgnnum(SUP((Itv)a),&sup);
		istoitv(inf,sup,c);
	}
}

int cmpitvf(Itv a, Itv b)
{
	Num	ai,as,bi,bs;
	int	s,t;

	if ( !a ) {
		if ( !b || (NID(b)<=N_B) )
			return compnum(0,(Num)a,(Num)b);
		else
			return -1;
	} else if ( !b ) {
		if ( !a || (NID(a)<=N_B) )
			return compnum(0,(Num)a,(Num)b);
		else
			return initvp((Num)b,a);
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
	Num	ai,as;
	Num	t;
	Q	TWO;

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
	Num	ai,as,bi,bs;
	Num	inf,sup;

	itvtois(a,&ai,&as);
	itvtois(b,&bi,&bs);
	if ( compnum(0,ai,bi) > 0 )	inf = bi;
	else				inf = ai;
	if ( compnum(0,as,bs) > 0 )	sup = as;
	else				sup = bs;
	istoitv(inf,sup,c);
}

void capitvf(Itv a, Itv b, Itv *c)
{
	Num	ai,as,bi,bs;
	Num	inf,sup;

	itvtois(a,&ai,&as);
	itvtois(b,&bi,&bs);
	if ( compnum(0,ai,bi) > 0 )	inf = ai;
	else				inf = bi;
	if ( compnum(0,as,bs) > 0 )	sup = bs;
	else				sup = as;
	if ( compnum(0,inf,sup) > 0 )	*c = 0;
	else				istoitv(inf,sup,c);
}


void widthitvf(Itv a, Num *b)
{
	Num	ai,as;

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
	Num	ai,as,bi,bs;

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
		if ( compnum(0,bi,bs) > 0 )	*b = bi;
		else				*b = bs;
	}
}

void distanceitvf(Itv a, Itv b, Num *c)
{
	Num	ai,as,bi,bs;
	Num	di,ds;
	Itv	d;

	itvtois(a,&ai,&as);
	itvtois(b,&bi,&bs);
	subnum(0,ai,bi,&di);
	subnum(0,as,bs,&ds);
	istoitv(di,ds,&d);
	absitvf(d,c);
}

#endif
