/*
 * $OpenXM: OpenXM_contrib2/asir2000/engine/t-itv.c,v 1.3 2003/02/14 22:29:09 ohara Exp $
*/
#if defined(INTERVAL)
#include "ca.h"
#include "base.h"
#if defined(PARI)
#include "genpari.h"
#endif

void itvtois(Itv a, Num *inf, Num *sup)
{
	if ( !a )
		*inf = *sup = (Num)0;
	else if ( NID(a) <= N_B ) {
		*inf = (Num)a; *sup = (Num)a;
	} else {
		*inf = INF(a);
		*sup = SUP(a);
	}
}

void istoitv(Num inf, Num sup, Itv *rp)
{
	Itv c;

	if ( !inf && !sup )
		*rp = 0;
	else {
		NEWItvP(c);
		if ( compnum(0,inf,sup) >= 0 ) {
			INF(c) = sup; SUP(c) = inf;
		} else {
			INF(c) = inf; SUP(c) = sup;
		}
		*rp = c;
	}
}

void additvp(Itv a, Itv b, Itv *c)
{
	Num	ai,as,bi,bs;
	Num	inf,sup;

	if ( !a )
		*c = b;
	else if ( !b )
		*c = a;
	else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
		addnum(0,a,b,c);
	else if ( (NID(a) == N_IP) && (NID(b) == N_R ) 
		||(NID(a) == N_R ) && (NID(b) == N_IP) )
		additvd((Num)a,(Num)b,(IntervalDouble *)c);
	else {
		itvtois(a,&ai,&as);
		itvtois(b,&bi,&bs);
		addnum(0,ai,bi,&inf);
		addnum(0,as,bs,&sup);
		istoitv(inf,sup,c);
	}
}

void subitvp(Itv a, Itv b, Itv *c)
{
	Num	ai,as,bi,bs;
	Num	inf,sup;

	if ( !a )
		chsgnnum(b,c);
	else if ( !b )
		*c = a;
	else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
		subnum(0,a,b,c);
	else if ( (NID(a) == N_IP) && (NID(b) == N_R ) 
		||(NID(a) == N_R ) && (NID(b) == N_IP) )
		subitvd((Num)a,(Num)b,(IntervalDouble *)c);
	else {
		itvtois(a,&ai,&as);
		itvtois(b,&bi,&bs);
		subnum(0,ai,bs,&inf);
		subnum(0,as,bi,&sup);
		istoitv(inf,sup,c);
	}
}

void mulitvp(Itv a, Itv b, Itv *c)
{
	Num	ai,as,bi,bs,a1,a2,b1,b2,c1,c2,p,t;
	int	ba,bb;

	if ( !a || !b )
		*c = 0;
	else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
		mulnum(0,a,b,c);
	else if ( (NID(a) == N_IP) && (NID(b) == N_R ) 
		||(NID(a) == N_R ) && (NID(b) == N_IP) )
		mulitvd((Num)a,(Num)b,(IntervalDouble *)c);
	else {
		itvtois(a,&ai,&as);
		itvtois(b,&bi,&bs);
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
			istoitv(c1,t,c);
		} else {
			subnum(0,0,c1,&t);
			istoitv(c2,t,c);
		}
	}
}

int     initvp(Num a, Itv b)
{
	Num inf, sup;

	itvtois(b, &inf, &sup);
	if ( compnum(0,inf,a) > 0 ) return 0;
	else if ( compnum(0,a,sup) > 0 ) return 0;
	else return 1;
}

int     itvinitvp(Itv a, Itv b)
{
	Num ai,as,bi,bs;

	itvtois(a, &ai, &as);
	itvtois(b, &bi, &bs);
	if ( compnum(0,ai,bi) >= 0  && compnum(0,bs,as) >= 0 ) return 1;
	else return 0;
}

void divitvp(Itv a, Itv b, Itv *c)
{
	Num	ai,as,bi,bs,a1,a2,b1,b2,c1,c2,t;
	int	ba,bb;

	if ( !b )
		error("divitv : division by 0");
	else if ( !a )
		*c = 0;
	else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
		divnum(0,a,b,c);
	else if ( (NID(a) == N_IP) && (NID(b) == N_R ) 
		||(NID(a) == N_R ) && (NID(b) == N_IP) )
		divitvd((Num)a,(Num)b,(IntervalDouble *)c);
	else {
		itvtois(a,&ai,&as);
		itvtois(b,&bi,&bs);
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
			istoitv(c1,t,c);
		} else {
			subnum(0,0,c1,&t);
			istoitv(c2,t,c);
		}
	}
}

void pwritvp(Itv a, Num e, Itv *c)
{
	int ei;
	Itv t;

	if ( !e )
		*c = (Itv)ONE;
	else if ( !a )
		*c = 0;
	else if ( NID(a) <= N_B )
		pwrnum(0,a,e,c);
	else if ( !INT(e) ) {
#if defined(PARI) && 0
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
		pwritv0p(a,ei,&t);
		if ( SGN((Q)e) < 0 )
			divnum(0,(Num)ONE,(Num)t,c);
		else
			*c = t;
	}
}

void pwritv0p(Itv a, int e, Itv *c)
{
	Num inf, sup;
	Num ai,Xmin,Xmax;
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
		else		pwrnum(0,Xmin,ne,&inf);
		pwrnum(0,Xmax,ne,&sup);
		istoitv(inf,sup,c);
	}
}

void chsgnitvp(Itv a, Itv *c)
{
	Num inf,sup;

	if ( !a )
		*c = 0;
	else if ( NID(a) <= N_B )
		chsgnnum(a,c);
	else {
		chsgnnum(INF((Itv)a),&inf);
		chsgnnum(SUP((Itv)a),&sup);
		istoitv(inf,sup,c);
	}
}

int cmpitvp(Itv a, Itv b)
{
	Num	ai,as,bi,bs;
	int	s,t;

	if ( !a ) {
		if ( !b || (NID(b)<=N_B) )
			return compnum(0,a,b);
		else
			return -1;
	} else if ( !b ) {
		if ( !a || (NID(a)<=N_B) )
			return compnum(0,a,b);
		else
			return initvp((Num)b,a);
	} else {
		itvtois(a,&ai,&as);
		itvtois(b,&bi,&bs);
		s = compnum(0,ai,bi) ;
		t = compnum(0,as,bs) ;
		if ( !s && !t ) return 0;
		else  return -1;
	}
}

void miditvp(Itv a, Num *b)
{
	Num	ai,as;
	Num	t;

	if ( ! a ) *b = 0;
	else if ( (NID(a) <= N_B) )
		*b = (Num)a;
	else {
		STOQ(2,TWO);
		itvtois(a,&ai,&as);
		addnum(0,ai,as,&t);
		divnum(0,t,TWO,b);
	}
}

void cupitvp(Itv a, Itv b, Itv *c)
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

void capitvp(Itv a, Itv b, Itv *c)
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


void widthitvp(Itv a, Num *b)
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

void absitvp(Itv a, Num *b)
{
	Num	ai,as,bi,bs;

	if ( ! a ) *b = 0;
	else if ( (NID(a) <= N_B) ) {
		if ( compnum(0,a,0) < 0 ) chsgnnum(a,b);
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

void distanceitvp(Itv a, Itv b, Num *c)
{
	Num	ai,as,bi,bs;
	Num	di,ds;
	Itv	d;

	itvtois(a,&ai,&as);
	itvtois(b,&bi,&bs);
	subnum(0,ai,bi,&di);
	subnum(0,as,bs,&ds);
	istoitv(di,ds,&d);
	absitvp(d,c);
}

#endif
