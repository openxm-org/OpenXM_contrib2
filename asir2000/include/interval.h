/*
 * $OpenXM: OpenXM_contrib2/asir2000/include/interval.h,v 1.4 2002/01/04 17:01:40 saito Exp $
*/
#ifndef	_INTERVAL_H
#define	_INTERVAL_H

#define	PRINTF_G	0
#define	PRINTF_E	1

#if defined(INTERVAL)

#include <math.h>

#ifdef	sun
#if OSMajorVersion < 4 || (OSMajorVersion == 4 && OSMinorVersion < 1)
#include <sys/ieeefp.h>
#endif
#if defined(__svr4__)
#include	<ieeefp.h>
#define	FPNEAREST	fpsetround(FP_RN);
#define	FPPLUSINF	fpsetround(FP_RP);
#define	FPMINUSINF	fpsetround(FP_RM);
#define	FPTOZERO	fpsetround(FP_RZ);
#else
static char	*Interval_dummy;
#define	FPNEAREST	ieee_flags("clear", "direction", Interval_dummy, &Interval_dummy);
#define	FPPLUSINF	ieee_flags("set", "direction", "positive", &Interval_dummy);
#define	FPMINUSINF	ieee_flags("set", "direction", "negative", &Interval_dummy);
#define	FPTOZERO	ieee_flags("set", "direction", "tozero", &Interval_dummy);
#endif
#endif

#ifdef	linux
#include	<fpu_control.h>
#if 1
#define	LINUX_FPU_RC_MASK				0xf3ff
#define	LINUX_FPU_SETCW(c)			{_FPU_GETCW(__fpu_control);\
				_FPU_SETCW(__fpu_control & LINUX_FPU_RC_MASK | c);}
#define	FPNEAREST	LINUX_FPU_SETCW(_FPU_RC_NEAREST);
#define	FPPLUSINF	LINUX_FPU_SETCW(_FPU_RC_UP);
#define	FPMINUSINF	LINUX_FPU_SETCW(_FPU_RC_DOWN);
#define	FPTOZERO		LINUX_FPU_SETCW(_FPU_RC_ZERO);
#else
#define	_FPU_DEFAULT_p_FPU_RC_UP	0x1b72
#define	_FPU_DEFAULT_p_FPU_RC_DOWN	0x1772
#define	_FPU_DEFAULT_p_FPU_RC_ZERO	0x1f72
#define	FPNEAREST	__setfpucw(_FPU_DEFAULT);
#define	FPPLUSINF	__setfpucw(_FPU_DEFAULT_p_FPU_RC_UP);
#define	FPMINUSINF	__setfpucw(_FPU_DEFAULT_p_FPU_RC_DOWN);
#define	FPTOZERO	__setfpucw(_FPU_DEFAULT_p_FPU_RC_ZERO);
#endif
#endif

#if defined(__osf__)
#if 0
#include	<float.h>
#define	FPNEAREST	write_rnd(FP_RND_RN);
#define	FPPLUSINF	write_rnd(FP_RND_RP);
#define	FPMINUSINF	write_rnd(FP_RND_RM);
#define	FPTOZERO	write_rnd(FP_RND_RZ);
#else
#define	FPNEAREST	
#define	FPPLUSINF	
#define	FPMINUSINF	
#define	FPTOZERO	
#endif
#endif

#if defined(__FreeBSD__)
#include	<floatingpoint.h>
#define	FPNEAREST	fpsetround(FP_RN);
#define	FPPLUSINF	fpsetround(FP_RP);
#define	FPMINUSINF	fpsetround(FP_RM);
#define	FPTOZERO	fpsetround(FP_RZ);
#endif

/* no control function of floating point rounding */
#ifndef FPNEAREST
#define	FPNEAREST	fprintf(stderr, "Fpu control FPNEAREST is not supported in this machine yet.\n");
#endif
#ifndef FPMINUSINF
#define	FPMINUSINF	fprintf(stderr, "Fpu control FPMINUSINF is not supported in this machine yet.\n");
#endif
#ifndef FPPLUSINF
#define	FPPLUSINF	fprintf(stderr, "Fpu control FPPLUSINF is not supported in this machine yet.\n");
#endif
#ifndef FPTOZERO
#define	FPTOZERO	fprintf(stderr, "Fpu control FPZERO is not supported in this machine yet.\n");
#endif
#define FPCLEAR		FPNEAREST

#define	MID_PRINTF_G	2
#define	MID_PRINTF_E	3

#undef  N_C
#undef  N_M
#undef  N_LM
#undef  N_GF2N
#undef  N_GFPN
#undef  N_GFS
#undef  N_GFSN
#define	N_NEXT_B		(N_B+1)
#define N_Quad			(N_NEXT_B)
#define N_IP			(N_NEXT_B+1)
#define N_IntervalDouble	(N_NEXT_B+2)
#define N_IntervalQuad		(N_NEXT_B+3)
#define N_IntervalBigFloat	(N_NEXT_B+4)
#define N_PRE_C	N_IntervalBigFloat
#define N_C	(N_NEXT_B+5)
#define N_M	(N_NEXT_B+6)
#define N_LM	(N_NEXT_B+7)
#define N_GF2N	(N_NEXT_B+8)
#define N_GFPN	(N_NEXT_B+9)
#define N_GFS	(N_NEXT_B+10)
#define N_GFSN	(N_NEXT_B+11)

/* data structures */
struct oItv {
        short	id;
        char	nid;
        char	pad;
        Num	inf;
        Num	sup;
};

typedef struct oItv *Itv;

struct oIntervalDouble {
        short	id;
        char	nid;
        char	pad;
        double	inf;
        double	sup;
};

typedef struct oIntervalDouble *IntervalDouble;

struct oforth {
	short	sign;
	short	exp;
	double	u;
	double	l;
};

typedef struct oforth *forth;

struct oIntervalQuad {
        short	id;
        char	nid;
        char	pad;
        forth	inf;
        forth	sup;
};

typedef struct oIntervalQuad *IntervalQuad;

struct oIntervalBigFloat {
        short	id;
        char	nid;
        char	pad;
        BF	inf;
        BF	sup;
};

typedef struct oIntervalBigFloat *IntervalBigFloat;

extern int zerorewrite;

/* general macros */
#define INF(p)	((p)->inf)
#define SUP(p)	((p)->sup)

#define NEWItvP(q)	((q)=(Itv)MALLOC(sizeof(struct oItv)),\
					OID(q)=O_N,NID(q)=N_IP)
#define NEWIntervalDouble(q)	((q)=(IntervalDouble)MALLOC(sizeof(struct oIntervalDouble)),\
					OID(q)=O_N,NID(q)=N_IntervalDouble)
#define NEWIntervalQuad(q)	((q)=(IntervalDouble)MALLOC(sizeof(struct oIntervalQuad)),\
					OID(q)=O_N,NID(q)=N_IntervalQuad)
#define NEWIntervalBigFloat(q)	((q)=(IntervalBigFloat)MALLOC(sizeof(struct oIntervalBigFloat)),\
					OID(q)=O_N,NID(q)=N_IntervalBigFloat)
#define MKItvP(a,b,c)	(NEWItvP(c),(INF(c)=(a),SUP(c)=(b)))
#define MKIntervalDouble(a,b,c)	if((zerorewrite) && ((a)<=0.0) && ((b)>=0.0)) (c)=0;\
			else (NEWIntervalDouble(c),(INF(c)=(a),SUP(c)=(b)))
#define MKIntervalQuad(a,b,c)	(NEWIntervalQuad(c),(INF(c)=(a),SUP(c)=(b)))
#define MKIntervalBigFloat(a,b,c)	(NEWIntervalBigFloat(c),(INF(c)=(a),SUP(c)=(b)))

#define ToItvP(a,c)	(NEWItvP(c),INF(c)=(a),SUP(c)=(a))
#define ToIntervalDouble(a,c)	(NEWIntervalDouble(c),INF(c)=(ToReal(a)),SUP(c)=(ToReal(a)))
#define ToIntervalBigFloat(a,c)	(NEWIntervalBigFloat(c),INF(c)=(a),SUP(c)=(a))

#define ITVP(a) (NID(a)==N_IP)
#define ITVD(a) (NID(a)==N_IntervalDouble)
#define ITVQ(a) (NID(a)==N_IntervalQuad)
#define ITVF(a) (NID(a)==N_IntervalBigFloat)

/***    engine/itv.c    ***/
double  ToRealSup(Num);
double  ToRealInf(Num);
double  RatnToRealUp(Q);
double  NatToRealUp(N, int *);

void	double2bf(double, BF *);
double	bf2double(BF);

void	itvtois(Itv, Num *, Num *);
void	istoitv(Num, Num, Itv *);

#if 0
void    additv(Num, Num, Num *);
void    subitv(Num, Num, Num *);
void    mulitv(Num, Num, Num *);
int     initv(Num, double);
int     itvinitv(Itv, Itv);
void    divitv(Num, Num, Num *);
void    sqrtitv(Num, Num *);
void    pwritv(Num, Num, Num *);
void    pwritv0(Itv, int, Itv *);
void    chsgnitv(Num, Num *);
void    miditv(Itv, Num *);
int     cmpitv(Itv, Itv);
void    cupitv(Itv, Itv, Itv *);
void    capitv(Itv, Itv, Itv *);
void    widthitv(Itv, Num *);
void    distanceitv(Itv, Itv, Num *);
#endif

void    additvp(Itv, Itv, Itv *);
void    subitvp(Itv, Itv, Itv *);
void    mulitvp(Itv, Itv, Itv *);
void    divitvp(Itv, Itv, Itv *);
void    chsgnitvp(Itv, Itv *);
int     cmpitvp(Itv, Itv);
void    pwritvp(Itv, Num, Itv *);
void    pwritv0p(Itv, int, Itv *);
void    cupitvp(Itv, Itv, Itv *);
void    capitvp(Itv, Itv, Itv *);
void    widthitvp(Itv, Num *);
void    distanceitvp(Itv, Itv, Num *);

void	ToBf(Num, BF *);
void	double2bf(double, BF *);
double	bf2double(BF);
void	addulp(IntervalBigFloat, IntervalBigFloat *);
void	getulp(BF, BF *);

void    additvf(IntervalBigFloat, IntervalBigFloat, IntervalBigFloat *);
void    subitvf(IntervalBigFloat, IntervalBigFloat, IntervalBigFloat *);
void    mulitvf(IntervalBigFloat, IntervalBigFloat, IntervalBigFloat *);
void    divitvf(IntervalBigFloat, IntervalBigFloat, IntervalBigFloat *);
void    chsgnitvf(Itv, Itv *);
int     cmpitvf(Itv, Itv);
void    pwritvf(Itv, Num, Itv *);
void    pwritv0f(Itv, int, Itv *);

/***    engine/d-itv.c    ***/
double	ToRealDown(Num);
double	ToRealUp(Num);
void	Num2double(Num, double *, double *);

void    additvd(Num, Num, IntervalDouble *);
void    subitvd(Num, Num, IntervalDouble *);
void    mulitvd(Num, Num, IntervalDouble *);
void    divitvd(Num, Num, IntervalDouble *);
void    chsgnitvd(IntervalDouble, IntervalDouble *);
int     cmpitvd(IntervalDouble, IntervalDouble);
void    pwritvd(Num, Num, IntervalDouble *);
void    pwritv0d(IntervalDouble, int, IntervalDouble *);
void    miditvd(IntervalDouble, Num *);
void    cupitvd(IntervalDouble, IntervalDouble, IntervalDouble *);
void    capitvd(IntervalDouble, IntervalDouble, IntervalDouble *);
void    widthitvd(IntervalDouble, Num *);
void    absitvd(IntervalDouble, Num *);
void    distanceitvd(IntervalDouble, IntervalDouble, Num *);

#endif /* end of INTERVAL */
#endif /* end of _INTERVAL_H */
