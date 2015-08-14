/*
 * $OpenXM: OpenXM_contrib2/asir2000/engine/d-itv.c,v 1.5 2015/08/08 14:19:41 fujimoto Exp $
*/
#if defined(INTERVAL)
#include <float.h>
#include "ca.h"
#include "base.h"
#if defined(PARI)
#include "genpari.h"
#endif

#if defined(ITVDEBUG)
void printbinint(int d)
{
	int	i, j, mask;
	union {
		int	x;
		char	c[sizeof(int)];
	} a;

	a.x = d;
	for(i=sizeof(int)-1;i>=0;i--) {
		mask = 0x80;
		for(j=0;j<8;j++) {
			if (a.c[i] & mask) fprintf(stderr,"1");
			else fprintf(stderr,"0");
			mask >>= 1;
		}
	}
	fprintf(stderr,"\n");
}
#endif

double NatToRealUp(N a, int *expo)
{
	int *p;
	int l,all,i,j,s,tb,top,tail;
	unsigned int t,m[2];	
	N	b,c;
	int	kk, carry, rem;
	
	p = BD(a); l = PL(a) - 1;
	for ( top = 0, t = p[l]; t; t >>= 1, top++ );
	all = top + BSH*l; tail = (53-top)%BSH; j = l-(53-top)/BSH-1; 


	if ( j >= 0 ) {

#if defined(ITVDEBUG)
	fprintf(stderr," %d : tail = %d\n", j, tail);
	printbinint(p[j]);
#endif
		kk = (1<< (BSH - tail)) - 1;
		rem = p[j] & kk;
		if ( !rem )
			for (i=0;i<j;i++)
				if ( p[j] ) {
					carry = 1;
					break;
				}
		else carry = 1;
		if ( carry ) {
			b = NALLOC(j+1+1);
			PL(b) = j+1;
			p = BD(b);
			for(i=0;i<j;i++) p[i] = 0;
			p[j]=(1<< (BSH - tail));

			addn(a,b,&c);

			p = BD(c); l = PL(c) - 1;
			for ( top = 0, t = p[l]; t; t >>= 1, top++ );
			all = top + BSH*l; tail = (53-top)%BSH; i = l-(53-top)/BSH-1; 
		} else i = j;
	} else i = j;


	m[1] = i < 0 ? 0 : p[i]>>(BSH-tail);
	for ( j = 1, i++, tb = tail; i <= l; i++ ) {
		s = 32-tb; t = i < 0 ? 0 : p[i];
		if ( BSH > s ) {
			m[j] |= ((t&((1<<s)-1))<<tb);
			if ( !j ) 
				break;
			else {
				j--; m[j] = t>>s; tb = BSH-s;
			}
		} else {
			m[j] |= (t<<tb); tb += BSH;
		}
	}
	s = (all-1)+1023;
	m[0] = (m[0]&((1<<20)-1))|(MIN(2046,s)<<20); *expo = MAX(s-2046,0);
#ifdef vax
	t = m[0]; m[0] = m[1]; m[1] = t; itod(m);
#endif
#if defined(MIPSEL) || defined(TOWNS) || defined(linux) || defined(__alpha) || defined(__FreeBSD__) || defined(__NetBSD__)
	t = m[0]; m[0] = m[1]; m[1] = t;
#endif
	return *((double *)m);
}

static double	Q2doubleDown(Q a)
{
	double nm,dn,t,snm,rd;
	int enm,edn,e;
	unsigned int *p,s;

	nm = (SGN(a)>0)? NatToReal(NM(a),&enm): NatToRealUp(NM(a),&enm);

	if ( INT(a) )
		if ( enm >= 1 )
			error("Q2doubleDown : Overflow");
		else
			return SGN(a)>0 ? nm : -nm;
	else {
		dn = (SGN(a)>0)? NatToReal(DN(a),&edn): NatToRealUp(DN(a),&edn);

		if ( ((e = enm - edn) >= 1024) || (e <= -1023) )
			error("Q2doubleDown : Overflow"); /* XXX */
		else {
			t = 0.0; p = (unsigned int *)&t; 	/* Machine */
			*p = ((e+1023)<<20);
#ifdef vax
			s = p[0]; p[0] = p[1]; p[1] = s; itod(p);
#endif
#if defined(MIPSEL) || defined(TOWNS) || defined(linux) || defined(__alpha) || defined(__FreeBSD__) || defined(__NetBSD__)
			s = p[0]; p[0] = p[1]; p[1] = s;
#endif
			FPMINUSINF
			snm = (SGN(a)>0) ? nm : -nm;
			rd  = snm / dn * t;
			FPNEAREST
			return rd;
		}
	}
}


static double	Q2doubleUp(Q a)
{
	double nm,dn,t,snm,rd;
	int enm,edn,e;
	unsigned int *p,s;

	nm = (SGN(a)>0)? NatToRealUp(NM(a),&enm): NatToReal(NM(a),&enm);

	if ( INT(a) )
		if ( enm >= 1 )
			error("Q2doubleUp : Overflow");
		else
			return SGN(a)>0 ? nm : -nm;
	else {
		dn = (SGN(a)>0)? NatToRealUp(DN(a),&edn): NatToReal(DN(a),&edn);

		if ( ((e = enm - edn) >= 1024) || (e <= -1023) )
			error("Q2doubleUp : Overflow"); /* XXX */
		else {
			t = 0.0; p = (unsigned int *)&t; 	/* Machine */
			*p = ((e+1023)<<20);
#ifdef vax
			s = p[0]; p[0] = p[1]; p[1] = s; itod(p);
#endif
#if defined(MIPSEL) || defined(TOWNS) || defined(linux) || defined(__alpha) || defined(__FreeBSD__) || defined(__NetBSD__)
			s = p[0]; p[0] = p[1]; p[1] = s;
#endif
#if 0
			FPPLUSINF
			snm = (SGN(a)>0) ? nm : -nm;
			rd  = snm / dn * t;
#endif

			FPMINUSINF
			snm = (SGN(a)>0) ? -nm : nm;
			rd  = snm / dn * t;
			FPNEAREST
			return (-rd);
		}
	}
}

static double	PARI2doubleDown(BF a)
{
        GEN     p;
	double  d;

        ritopa(a, &p);
        d = rtodbl(p);
	cgiv(p);
	return d;
}

static double	PARI2doubleUp(BF a)
{
	return PARI2doubleDown(a);
}

double	subulpd(double d)
{
	double inf;

	FPMINUSINF
	inf = d - DBL_MIN;
	FPNEAREST
	return inf;
}

double	addulpd(double d)
{
	double md, sup;

	md = -d;
	FPMINUSINF
	sup = md - DBL_MIN;
	FPNEAREST
	return (-sup);
}

double	ToRealDown(Num a)
{
	double inf;

	if ( ! a ) return 0.0;
	switch ( NID(a) ) {
		case N_Q:
			inf = Q2doubleDown((Q)a); break;
		case N_R:
			inf = subulpd(BDY((Real)a)); break;
		case N_B:
			inf = PARI2doubleDown((BF)a); break;
		case N_IP:
			inf = ToRealDown(INF((Itv)a));
			break;
		case N_IntervalDouble:
			inf = INF((IntervalDouble)a); break;
		case N_A:
		default:
			inf = 0.0;
			error("ToRealDown: not supported operands.");
			break;
	}
	return inf;
}

double	ToRealUp(Num a)
{
	double sup;

	if ( ! a ) return 0.0;
	switch ( NID(a) ) {
		case N_Q:
			sup = Q2doubleUp((Q)a); break;
		case N_R:
			sup = addulpd(BDY((Real)a)); break;
		case N_B:
			sup = PARI2doubleUp((BF)a); break;
		case N_IP:
			sup = ToRealUp(SUP((Itv)a)); break;
		case N_IntervalDouble:
			sup = SUP((IntervalDouble)a); break;
		case N_A:
		default:
			sup = 0.0;
			error("ToRealUp: not supported operands.");
			break;
	}
	return sup;
}


void	Num2double(Num a, double *inf, double *sup)
{
	switch ( NID(a) ) {
		case N_Q:
			*inf = Q2doubleDown((Q)a);
			*sup = Q2doubleUp((Q)a);
			break;
		case N_R:
			*inf = BDY((Real)a);
			*sup = BDY((Real)a);
			break;
		case N_B:
			*inf = PARI2doubleDown((BF)a);
			*sup = PARI2doubleUp((BF)a);
			break;
		case N_IP:
			*inf = ToRealDown(INF((Itv)a));
			*sup = ToRealUp(SUP((Itv)a));
			break;
		case N_IntervalDouble:
			*inf = INF((IntervalDouble)a);
			*sup = SUP((IntervalDouble)a);
			break;
		case N_A:
		default:
			*inf = 0.0;
			*sup = 0.0;
			error("Num2double: not supported operands.");
			break;
	}
}


void additvd(Num a, Num b, IntervalDouble *c)
{
	double	ai,as,mas, bi,bs;
	double	inf,sup;

	if ( !a ) {
		*c = (IntervalDouble)b;
	} else if ( !b ) {
		*c = (IntervalDouble)a;
#if	0
	} else if ( (NID(a) <= N_IP) && (NID(b) <= N_IP ) ) {
		addnum(0,a,b,c);
#endif
	} else {

		Num2double((Num)a,&ai,&as);
		Num2double((Num)b,&bi,&bs);
		mas = -as;
		FPMINUSINF
		inf = ai + bi;
		sup = mas - bs;		/*  as + bs = ( - ( (-as) - bs ) ) */
		FPNEAREST
		MKIntervalDouble(inf,(-sup),*c);
	}
}

void subitvd(Num a, Num b, IntervalDouble *c)
{
	double	ai,as,mas, bi,bs;
	double	inf,sup;

	if ( !a ) {
		*c = (IntervalDouble)b;
	} else if ( !b ) {
		*c = (IntervalDouble)a;
#if	0
	} else if ( (NID(a) <= N_IP) && (NID(b) <= N_IP ) ) {
		addnum(0,a,b,c);
#endif
	} else {
		Num2double(a,&ai,&as);
		Num2double(b,&bi,&bs);
		FPMINUSINF
		inf = ai - bs;
		sup = ( bi - as );	/* as - bi = ( - ( bi - as ) ) */
		FPNEAREST
		MKIntervalDouble(inf,(-sup),*c);
	}
}

void	mulitvd(Num a, Num b, IntervalDouble *c)
{
	double	ai,as,bi,bs,a1,a2,b1,b2,c1,c2,p;
	double	inf, sup;
	int	ba,bb;

	if ( !a || !b ) {
		*c = 0;
#if	0
	} else if ( (NID(a) <= N_IP) && (NID(b) <= N_IP ) ) {
		mulnum(0,a,b,c);
#endif
	} else {
		Num2double(a,&ai,&as);
		Num2double(b,&bi,&bs);

		FPMINUSINF

		a2 = -as;
		b2 = -bs;

		if ( ba = ( a2 < 0.0 ) ) {
			a1 = ai;
		} else {
			a1 = a2;
			a2 = ai;
		}
		if ( bb = ( b2 < 0.0 ) ) {
			b1 = bi;
		} else {
			b1 = b2;
			b2 = bi;
		}

		c2 = - a2 * b2;
		if ( b1 < 0.0 ) {
			c1 = - a2 * b1;
			if ( a1 < 0.0 ) {
				p = - a1 * b2;
				if ( p < c1 ) c1 = p;
				p = - a1 * b1;
				if ( p < c2 ) c2 = p;
			}
		} else {
			c1 = (a1<0.0)?( - a1 * b2 ):( a1 * b1 );
		}
		if ( ba == bb ) {
			inf = c1;
			sup = - c2;
		} else {
			inf = c2;
			sup = - c1;
		}
		FPNEAREST
		MKIntervalDouble(inf,sup,*c);
	}
}

int     initvd(Num a, IntervalDouble b)
{
	Real inf, sup;

	if ( NID(b) == N_IntervalDouble ) {
		MKReal(INF(b), inf);
		MKReal(SUP(b), sup);
	} else return 0;
	if ( compnum(0,(Num)inf,a) > 0 ) return 0;
	else if ( compnum(0,a,(Num)sup) > 0 ) return 0;
	else return 1;
}

void	divitvd(Num a, Num b, IntervalDouble *c)
{
	double	ai,as,bi,bs,a1,a2,b1,b2,c1,c2;
	double	inf, sup;
	int	ba,bb;

	if ( !b ) {
		*c = 0;
		error("divitvd : division by 0");
	} else if ( !a ) {
		*c = 0;
#if	0
	} else if ( (NID(a) <= N_IP) && (NID(b) <= N_IP ) ) {
		divnum(0,a,b,c);
#endif
	} else {
		Num2double(a,&ai,&as);
		Num2double(b,&bi,&bs);

		FPMINUSINF

		a2 = -as;
		b2 = -bs;

		if ( ba = ( a2 < 0.0 ) ) {
			a1 = ai;
		} else {
			a1 = a2;
			a2 = ai;
		}
		if ( bb = ( b2 < 0.0 ) ) {
			b1 = bi;
		} else {
			b1 = b2;
			b2 = bi;
		}

		if ( b1 <= 0.0 ) {
			*c = 0;
			error("divitvd : division by 0 interval");
		} else {
			c2 = a2 / b1;
			c1 = (a1<0.0)?( a1 / b1 ):( - a1 / b2 );
		}
		if ( ba == bb ) {
			inf = c1;
			sup = - c2;
		} else {
			inf = c2;
			sup = - c1;
		}
		FPNEAREST
		MKIntervalDouble(inf,sup,*c);
	}
}

void	pwritvd(Num a, Num e, IntervalDouble *c)
{
	int	ei;
	IntervalDouble	t;

	if ( !e ) {
		*c = (IntervalDouble)ONE;
	}  else if ( !a ) {
		*c = 0;
#if	0
	} else if ( NID(a) <= N_IP ) {
		pwrnum(0,a,e,c);
#endif
	} else if ( !INT(e) ) {
#if defined(PARI) && 0
		gpui_ri((Obj)a,(Obj)c,(Obj *)c);
#else
		error("pwritvd : can't calculate a fractional power");
#endif
	} else {
		ei = QTOS((Q)e);
		pwritv0d((IntervalDouble)a,ei,&t);
		if ( SGN((Q)e) < 0 )
			divnum(0,(Num)ONE,(Num)t,(Num *)c);
		else
			*c = t;
	}
}

void	pwritv0d(IntervalDouble a, int e, IntervalDouble *c)
{
	double inf, sup;
	double t, Xmin, Xmax;
	int i;

	if ( e == 1 )
		*c = a;
	else {
		if ( !(e%2) ) {
			if ( initvd(0,a) ) {
				Xmin = 0.0;
				t = - INF(a);
				Xmax = SUP(a);
				if ( t > Xmax ) Xmax = t;
			} else {
				if ( INF(a) > 0.0 ) {
					Xmin = INF(a);
					Xmax = SUP(a);
				} else {
					Xmin = - SUP(a);
					Xmax = - INF(a);
				}
			}
		} else {
			Xmin = INF(a);
			Xmax = SUP(a);
		}
		FPPLUSINF
		sup = (Xmax!=0.0)?pwrreal0(Xmax,e):0.0;
		FPMINUSINF
		inf = (Xmin!=0.0)?pwrreal0(Xmin,e):0.0;
		FPNEAREST
		MKIntervalDouble(inf,sup,*c);
	}
}

void	chsgnitvd(IntervalDouble a, IntervalDouble *c)
{
	double inf,sup;

	if ( !a )
		*c = 0;
#if	0
	else if ( NID(a) <= N_IP )
		chsgnnum(a,c);
#endif
	else {
		inf = - SUP(a);
		sup = - INF(a);
		MKIntervalDouble(inf,sup,*c);
	}
}

int	cmpitvd(IntervalDouble a, IntervalDouble b)
/*    a > b:  1       */
/*    a = b:  0       */
/*    a < b: -1       */
{
	double	ai,as,bi,bs;

	if ( !a ) {
		if ( !b || (NID(b)<=N_IP) ) {
			return compnum(0,(Num)a,(Num)b);
		} else {
			bi = INF(b);
			bs = SUP(b);
			if ( bi > 0.0 ) return -1;
			else if ( bs < 0.0 ) return 1;
			else return 0;
		}
	} else if ( !b ) {
		if ( !a || (NID(a)<=N_IP) ) {
			return compnum(0,(Num)a,(Num)b);
		} else {
			ai = INF(a);
			as = SUP(a);
			if ( ai > 0.0 ) return 1;
			else if ( as < 0.0 ) return -1;
			else return 0;
		}
	} else {
		bi = INF(b);
		bs = SUP(b);
		ai = INF(a);
		as = SUP(a);
		if ( ai > bs ) return 1;
		else if ( bi > as ) return -1;
		else return 0;
	}
}

void	miditvd(IntervalDouble a, Num *b)
{
	double	t;
	Real	rp;

	if ( ! a ) *b = 0;
	else if ( (NID(a) <= N_IP) )
		*b = (Num)a;
	else {
		t = (INF(a) + SUP(a))/2.0;
		MKReal(t,rp);
		*b = (Num)rp;
	}
}

void	cupitvd(IntervalDouble a, IntervalDouble b, IntervalDouble *c)
{
	double	ai,as,bi,bs;
	double	inf,sup;

	ai = INF(a);
	as = SUP(a);
	bi = INF(b);
	bs = SUP(b);
	inf = MIN(ai,bi);
	sup = MAX(as,bs);
	MKIntervalDouble(inf,sup,*c);
}

void	capitvd(IntervalDouble a, IntervalDouble b, IntervalDouble *c)
{
	double	ai,as,bi,bs;
	double	inf,sup;

	ai = INF(a);
	as = SUP(a);
	bi = INF(b);
	bs = SUP(b);
	inf = MAX(ai,bi);
	sup = MIN(as,bs);
	if ( inf > sup ) *c = 0;
	else MKIntervalDouble(inf,sup,*c);
}


void	widthitvd(IntervalDouble a, Num *b)
{
	double	t;
	Real	rp;

	if ( ! a ) *b = 0;
	else {
		t = SUP(a) - INF(a);
		MKReal(t,rp);
		*b = (Num)rp;
	}
}

void	absitvd(IntervalDouble a, Num *b)
{
	double	ai,as,bi,bs;
	double	t;
	Real	rp;

	if ( ! a ) *b = 0;
	else {
		ai = INF(a);
		as = SUP(a);
		if (ai < 0) bi = -ai;	else	bi = ai;
		if (as < 0) bs = -as;	else	bs = as;
		if ( bi > bs )	t = bi; else	t = bs;
		MKReal(t,rp);
		*b = (Num)rp;
	}
}

void	distanceitvd(IntervalDouble a, IntervalDouble b, Num *c)
{
	double	ai,as,bi,bs;
	double	di,ds;
	double	t;
	Real	rp;

	ai = INF(a);
	as = SUP(a);
	bi = INF(b);
	bs = SUP(b);
	di = bi - ai;
	ds = bs - as;

	if (di < 0) di = -di;
	if (ds < 0) ds = -ds;
	if (di > ds)	t = di; else	t = ds;
	MKReal(t,rp);
	*c = (Num)rp;
}

#endif
