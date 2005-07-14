/*
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/itvnum.c,v 1.4 2005/02/08 16:53:00 saito Exp $
 */

#include "ca.h"
#include "parse.h"
#include "version.h"

#if defined(INTERVAL)

static void Pitv(NODE, Obj *);
static void Pitvd(NODE, Obj *);
static void Pitvbf(NODE, Obj *);
static void Pinf(NODE, Obj *);
static void Psup(NODE, Obj *);
static void Pmid(NODE, Obj *);
static void Pabsitv(NODE, Obj *);
static void Pdisjitv(NODE, Obj *);
static void Pinitv(NODE, Obj *);
static void Pcup(NODE, Obj *);
static void Pcap(NODE, Obj *);
static void Pwidth(NODE, Obj *);
static void Pdistance(NODE, Obj *);
static void Pitvversion(Q *);
#endif
static void Pprintmode(NODE, Obj *);

#if	defined(__osf__) && 0
int	end;
#endif

struct ftab interval_tab[] = {
	{"printmode",Pprintmode,1},
#if defined(INTERVAL)
	{"itvd",Pitvd,-2},
	{"intvald",Pitvd,-2},
	{"itv",Pitv,-2},
	{"intval",Pitv,-2},
	{"itvbf",Pitvbf,-2},
	{"intvalbf",Pitvbf,-2},
	{"inf",Pinf,1},
	{"sup",Psup,1},
	{"absintval",Pabsitv,1},
	{"disintval",Pdisjitv,2},
	{"inintval",Pinitv,2},
	{"cup",Pcup,2},
	{"cap",Pcap,2},
	{"mid",Pmid,1},
	{"width",Pwidth,1},
	{"diam",Pwidth,1},
	{"distance",Pdistance,2},
	{"iversion",Pitvversion,0},
#endif
	{0,0,0},
};

#if defined(INTERVAL)
static void
Pitvversion(Q *rp)
{
	STOQ(ASIR_VERSION, *rp);
}

extern int	bigfloat;

static void
Pitv(NODE arg, Obj *rp)
{
	Num	a, i, s;
	Itv	c;
	double	inf, sup;

#if 1
	if ( bigfloat )
		Pitvbf(arg, rp);
	else
		Pitvd(arg,rp);
#else
	asir_assert(ARG0(arg),O_N,"itv");
	if ( argc(arg) > 1 ) {
		asir_assert(ARG1(arg),O_N,"itv");
		istoitv((Num)ARG0(arg),(Num)ARG1(arg),&c);
	} else {
		a = (Num)ARG0(arg);
		if ( ! a ) {
			*rp = 0;
			return;
		}
		else if ( NID(a) == N_IP || NID(a) == N_IntervalBigFloat) {
			*rp = (Obj)a;
			return;
		}
		else if ( NID(a) == N_IntervalDouble ) {
			inf = INF((IntervalDouble)a);
			sup = SUP((IntervalDouble)a);
			double2bf(inf, (BF *)&i);
			double2bf(sup, (BF *)&s);
			istoitv(i,s,&c);
		}
		else istoitv(a,a,&c);
	}
	if ( NID( c ) == N_IntervalBigFloat )
		addulp((IntervalBigFloat)c, (IntervalBigFloat *)rp);
	else *rp = (Obj)c;
#endif
}

static void
Pitvbf(NODE arg, Obj *rp)
{
	Num	a, i, s;
	Itv	c;
	BF	ii,ss;
	double	inf, sup;

	asir_assert(ARG0(arg),O_N,"intvalbf");
	a = (Num)ARG0(arg);
	if ( argc(arg) > 1 ) {
		asir_assert(ARG1(arg),O_N,"intvalbf");
		i = (Num)ARG0(arg);
		s = (Num)ARG1(arg);
		ToBf(i, &ii);
		ToBf(s, &ss);
		istoitv((Num)ii,(Num)ss,&c);
	} else {
		if ( ! a ) {
			*rp = 0;
			return;
		}
		else if ( NID(a) == N_IP ) {
			itvtois((Itv)a, &i, &s);
			ToBf(i, &ii);
			ToBf(s, &ss);
			istoitv((Num)ii,(Num)ss,&c);
		}
		else if ( NID(a) == N_IntervalBigFloat) {
			*rp = (Obj)a;
			return;
		}
		else if ( NID(a) == N_IntervalDouble ) {
			inf = INF((IntervalDouble)a);
			sup = SUP((IntervalDouble)a);
			double2bf(inf, (BF *)&i);
			double2bf(sup, (BF *)&s);
			istoitv(i,s,&c);
		}
		else {
			ToBf(a, (BF *)&i);
			istoitv(i,i,&c);
		}
	}
	if ( c && OID( c ) == O_N && NID( c ) == N_IntervalBigFloat )
		addulp((IntervalBigFloat)c, (IntervalBigFloat *)rp);
	else *rp = (Obj)c;
}

static void
Pitvd(NODE arg, Obj *rp)
{
	double	inf, sup;
	Num	a, a0, a1, t;
	Itv	ia;
	IntervalDouble	d;

	asir_assert(ARG0(arg),O_N,"intvald");
	a0 = (Num)ARG0(arg);
	if ( argc(arg) > 1 ) {
		asir_assert(ARG1(arg),O_N,"intvald");
		a1 = (Num)ARG1(arg);
	} else {
		if ( a0 && OID(a0)==O_N && NID(a0)==N_IntervalDouble ) {
			inf = INF((IntervalDouble)a0);
			sup = SUP((IntervalDouble)a0);
			MKIntervalDouble(inf,sup,d);
			*rp = (Obj)d;
			return;
		}
		a1 = (Num)ARG0(arg);
	}
	if ( compnum(0,a0,a1) > 0 ) {
		t = a0; a0 = a1; a1 = t;
	}
	inf = ToRealDown(a0);
	sup = ToRealUp(a1);
	MKIntervalDouble(inf,sup,d);
	*rp = (Obj)d;
}

static void
Pinf(NODE arg, Obj *rp)
{
	Num	a, i, s;
	Real	r;
	double	d;

	a = (Num)ARG0(arg);
	if ( ! a ) {
		*rp = 0;
	} else if  ( OID(a) == O_N ) {
		switch ( NID(a) ) {
			case N_IntervalDouble:
				d = INF((IntervalDouble)a);
				MKReal(d, r);
				*rp = (Obj)r;
				break;
			case N_IP:
			case N_IntervalBigFloat:
			case N_IntervalQuad:
				itvtois((Itv)ARG0(arg),&i,&s);
				*rp = (Obj)i;
				break;
			default:
				*rp = (Obj)a;
				break;
		}
	} else {
		*rp = (Obj)a;
	}
}

static void
Psup(NODE arg, Obj *rp)
{
	Num	a, i, s;
	Real	r;
	double	d;

	a = (Num)ARG0(arg);
	if ( ! a ) {
		*rp = 0;
	} else if  ( OID(a) == O_N ) {
		switch ( NID(a) ) {
			case N_IntervalDouble:
				d = SUP((IntervalDouble)a);
				MKReal(d, r);
				*rp = (Obj)r;
				break;
			case N_IP:
			case N_IntervalBigFloat:
			case N_IntervalQuad:
				itvtois((Itv)ARG0(arg),&i,&s);
				*rp = (Obj)s;
				break;
			default:
				*rp = (Obj)a;
				break;
		}
	} else {
			*rp = (Obj)a;
	}
}

static void
Pmid(NODE arg, Obj *rp)
{
	Num	a, s;
	Real	r;
	double	d;

	a = (Num)ARG0(arg);
	if ( ! a ) {
		*rp = 0;
	} else switch (OID(a)) {
		case O_N:
			if ( NID(a) == N_IntervalDouble ) {
				d = ( INF((IntervalDouble)a)+SUP((IntervalDouble)a) ) / 2.0;
				MKReal(d, r);
				*rp = (Obj)r;
			} else if ( NID(a) == N_IntervalQuad ) {
				error("mid: not supported operation");
				*rp = 0;
			} else if ( NID(a) == N_IP || NID(a) == N_IntervalBigFloat ) {
				miditvp((Itv)ARG0(arg),&s);
				*rp = (Obj)s;
			} else {
				*rp = (Obj)a;
			}
			break;
#if 0
		case O_P:
		case O_R:
		case O_LIST:
		case O_VECT:
		case O_MAT:
#endif
		default:
			*rp = (Obj)a;
			break;
	}
}

static void
Pcup(NODE arg, Obj *rp)
{
	Itv	s;
	Num	a, b;

	asir_assert(ARG0(arg),O_N,"cup");
	asir_assert(ARG1(arg),O_N,"cup");
	a = (Num)ARG0(arg);
	b = (Num)ARG1(arg);
	if ( a && NID(a) == N_IntervalDouble && b && NID(b) == N_IntervalDouble ) {
		cupitvd((IntervalDouble)a, (IntervalDouble)b, (IntervalDouble *)rp);
	} else {
		cupitvp((Itv)ARG0(arg),(Itv)ARG1(arg),&s);
		*rp = (Obj)s;
	}
}

static void
Pcap(NODE arg, Obj *rp)
{
	Itv	s;
	Num	a, b;

	asir_assert(ARG0(arg),O_N,"cap");
	asir_assert(ARG1(arg),O_N,"cap");
	a = (Num)ARG0(arg);
	b = (Num)ARG1(arg);
	if ( a && NID(a) == N_IntervalDouble && b && NID(b) == N_IntervalDouble ) {
		capitvd((IntervalDouble)a, (IntervalDouble)b, (IntervalDouble *)rp);
	} else {
		capitvp((Itv)ARG0(arg),(Itv)ARG1(arg),&s);
		*rp = (Obj)s;
	}
}

static void
Pwidth(arg,rp)
NODE arg;
Obj *rp;
{
	Num	s;
	Num	a;

	asir_assert(ARG0(arg),O_N,"width");
	a = (Num)ARG0(arg);
	if ( ! a ) {
		*rp = 0;
	} else if ( NID(a) == N_IntervalDouble ) {
		widthitvd((IntervalDouble)a, (Num *)rp);
	} else {
		widthitvp((Itv)ARG0(arg),&s);
		*rp = (Obj)s;
	}
}

static void
Pabsitv(arg,rp)
NODE arg;
Obj *rp;
{
	Num	s;
	Num	a, b;

	asir_assert(ARG0(arg),O_N,"absitv");
	a = (Num)ARG0(arg);
	if ( ! a ) {
		*rp = 0;
	} else if ( NID(a) == N_IntervalDouble ) {
		absitvd((IntervalDouble)a, (Num *)rp);
	} else {
		absitvp((Itv)ARG0(arg),&s);
		*rp = (Obj)s;
	}
}

static void
Pdistance(arg,rp)
NODE arg;
Obj *rp;
{
	Num	s;
	Num	a, b;

	asir_assert(ARG0(arg),O_N,"distance");
	asir_assert(ARG1(arg),O_N,"distance");
	a = (Num)ARG0(arg);
	b = (Num)ARG1(arg);
	if ( a && NID(a) == N_IntervalDouble && b && NID(b) == N_IntervalDouble ) {
		distanceitvd((IntervalDouble)a, (IntervalDouble)b, (Num *)rp);
	} else {
		distanceitvp((Itv)ARG0(arg),(Itv)ARG1(arg),&s);
		*rp = (Obj)s;
	}
}

static void
Pinitv(arg,rp)
NODE arg;
Obj *rp;
{
	int	s;
	Q	q;

	asir_assert(ARG0(arg),O_N,"intval");
	asir_assert(ARG1(arg),O_N,"intval");
	if ( ! ARG1(arg) ) {
		if ( ! ARG0(arg) ) s = 1;
		else s = 0;
	}
	else if ( NID(ARG1(arg)) == N_IntervalDouble ) {
		s = initvd((Num)ARG0(arg),(IntervalDouble)ARG1(arg));

	} else if ( NID(ARG1(arg)) == N_IP || NID(ARG1(arg)) == N_IntervalBigFloat ) {
		if ( ! ARG0(arg) ) s = initvp((Num)ARG0(arg),(Itv)ARG1(arg));
		else if ( NID(ARG0(arg)) == N_IP ) {
			s = itvinitvp((Itv)ARG0(arg),(Itv)ARG1(arg));
		} else {
			s = initvp((Num)ARG0(arg),(Itv)ARG1(arg));
		}
	} else {
		s = ! compnum(0,(Num)ARG0(arg),(Num)ARG1(arg));
	}
	STOQ(s,q);
	*rp = (Obj)q;
}

static void
Pdisjitv(arg,rp)
NODE arg;
Obj *rp;
{
	Itv	s;

	asir_assert(ARG0(arg),O_N,"disjitv");
	asir_assert(ARG1(arg),O_N,"disjitv");
	error("disjitv: not implemented yet");
	if ( ! s ) *rp = 0;
	else *rp = (Obj)ONE;
}

#endif
extern int	printmode;

static void	pprintmode( void )
{
	switch (printmode) {
#if defined(INTERVAL)
		case MID_PRINTF_E:
			fprintf(stderr,"Interval printing mode is a mitpoint type.\n");
#endif
		case PRINTF_E:
			fprintf(stderr,"Printf's double printing mode is \"%%.16e\".\n");
			break;
#if defined(INTERVAL)
		case MID_PRINTF_G:
			fprintf(stderr,"Interval printing mode is a mitpoint type.\n");
#endif
		default:
		case PRINTF_G:
			fprintf(stderr,"Printf's double printing mode is \"%%g\".\n");
			break;
	}
}

static void
Pprintmode(NODE arg, Obj *rp)
{
	int	l;
	Q	a, r;

	a = (Q)ARG0(arg);
	if ( !a || NUM(a) && INT(a) ) {
		l = QTOS(a);
		if ( l < 0 ) l = 0;
#if defined(INTERVAL)
		else if ( l > MID_PRINTF_E ) l = 0;
#else
		else if ( l > PRINTF_E ) l = 0;
#endif
		STOQ(printmode,r);
		*rp = (Obj)r;
		printmode = l;
		pprintmode();
	} else {
		*rp = 0;
	}
}
	

