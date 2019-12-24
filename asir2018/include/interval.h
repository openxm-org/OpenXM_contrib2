/*
 * $OpenXM: OpenXM_contrib2/asir2018/include/interval.h,v 1.4 2019/11/12 10:53:23 kondoh Exp $
*/
#ifndef  _INTERVAL_H
#define  _INTERVAL_H

#define  PRINTF_G  0
#define  PRINTF_E  1

#define	INT_ASIR_VERSION	20191219

#if defined(INTERVAL)
#if INTERVAL == 0
#undef INTERVAL
#endif
#endif
#if defined(INTERVAL)

#include <math.h>

#ifdef  sun
#if OSMajorVersion < 4 || (OSMajorVersion == 4 && OSMinorVersion < 1)
#include <sys/ieeefp.h>
#endif
#if defined(__svr4__)
#include  <ieeefp.h>
#define  FPNEAREST  fpsetround(FP_RN);
#define  FPPLUSINF  fpsetround(FP_RP);
#define  FPMINUSINF  fpsetround(FP_RM);
#define  FPTOZERO  fpsetround(FP_RZ);
#else
static char  *Interval_dummy;
#define  FPNEAREST  ieee_flags("clear", "direction", Interval_dummy, &Interval_dummy);
#define  FPPLUSINF  ieee_flags("set", "direction", "positive", &Interval_dummy);
#define  FPMINUSINF  ieee_flags("set", "direction", "negative", &Interval_dummy);
#define  FPTOZERO  ieee_flags("set", "direction", "tozero", &Interval_dummy);
#endif
#endif

#if 0
#ifdef  linux
#include  <fpu_control.h>
#if 1

#if defined(__ARM_ARCH) || defined(ANDROID)
#ifndef _FPU_RC_NEAREST
#define _FPU_RC_NEAREST 0x0
#endif
#ifndef _FPU_RC_DOWN
#define _FPU_RC_DOWN    0x400
#endif
#ifndef _FPU_RC_UP
#define _FPU_RC_UP      0x800
#endif
#ifndef _FPU_RC_ZERO
#define _FPU_RC_ZERO    0xC00
#endif
#endif /* __ARM_ARCH */

#define  LINUX_FPU_RC_MASK        0xf3ff
#define  LINUX_FPU_SETCW(c)      \
{fpu_control_t __tmp__; _FPU_GETCW(__fpu_control);\
 __tmp__ = (__fpu_control & LINUX_FPU_RC_MASK | c);\
 _FPU_SETCW(__tmp__);}
#define  FPNEAREST  LINUX_FPU_SETCW(_FPU_RC_NEAREST);
#define  FPPLUSINF  LINUX_FPU_SETCW(_FPU_RC_UP);
#define  FPMINUSINF  LINUX_FPU_SETCW(_FPU_RC_DOWN);
#define  FPTOZERO    LINUX_FPU_SETCW(_FPU_RC_ZERO);
#else
#define  _FPU_DEFAULT_p_FPU_RC_UP  0x1b72
#define  _FPU_DEFAULT_p_FPU_RC_DOWN  0x1772
#define  _FPU_DEFAULT_p_FPU_RC_ZERO  0x1f72
#define  FPNEAREST  __setfpucw(_FPU_DEFAULT);
#define  FPPLUSINF  __setfpucw(_FPU_DEFAULT_p_FPU_RC_UP);
#define  FPMINUSINF  __setfpucw(_FPU_DEFAULT_p_FPU_RC_DOWN);
#define  FPTOZERO  __setfpucw(_FPU_DEFAULT_p_FPU_RC_ZERO);
#endif
#endif

#if defined(__osf__)
#if 0
#include  <float.h>
#define  FPNEAREST  write_rnd(FP_RND_RN);
#define  FPPLUSINF  write_rnd(FP_RND_RP);
#define  FPMINUSINF  write_rnd(FP_RND_RM);
#define  FPTOZERO  write_rnd(FP_RND_RZ);
#else
#define  FPNEAREST  
#define  FPPLUSINF  
#define  FPMINUSINF  
#define  FPTOZERO  
#endif
#endif

#if defined(__FreeBSD__) && defined(__GNUC__)
#include  <floatingpoint.h>
#define  FPNEAREST  fpsetround(FP_RN);
#define  FPPLUSINF  fpsetround(FP_RP);
#define  FPMINUSINF  fpsetround(FP_RM);
#define  FPTOZERO  fpsetround(FP_RZ);
#endif

#if defined(VISUAL) || defined(__MINGW32__)
#include <float.h>
#define FPNEAREST _controlfp(_RC_NEAR,_MCW_RC);
#define FPPLUSINF _controlfp(_RC_UP,_MCW_RC);
#define FPMINUSINF _controlfp(_RC_DOWN,_MCW_RC);
#define FPTOZERO _controlfp(_RC_CHOP,_MCW_RC);
#endif
#endif
#include <fenv.h>

#define	FPNEAREST	fesetround(FE_TONEAREST);
#define	FPPLUSINF	fesetround(FE_UPWARD);
#define	FPMINUSINF	fesetround(FE_DOWNWARD);
#define	FPTOZERO	fesetround(FE_TOWARDZERO);


/* no control function of floating point rounding */
#ifndef FPNEAREST
#define  FPNEAREST  fprintf(stderr, "Fpu control FPNEAREST is not supported in this machine yet.\n");
#endif
#ifndef FPMINUSINF
#define  FPMINUSINF  fprintf(stderr, "Fpu control FPMINUSINF is not supported in this machine yet.\n");
#endif
#ifndef FPPLUSINF
#define  FPPLUSINF  fprintf(stderr, "Fpu control FPPLUSINF is not supported in this machine yet.\n");
#endif
#ifndef FPTOZERO
#define  FPTOZERO  fprintf(stderr, "Fpu control FPZERO is not supported in this machine yet.\n");
#endif
#define FPCLEAR    FPNEAREST

#define  MID_PRINTF_G  2
#define  MID_PRINTF_E  3

#define DEFAULTPREC	0	/* for mpfr */

/* data structures */
struct oItv {
        short  id;
        char  nid;
        char  pad;
        Num  inf;
        Num  sup;
};

typedef struct oItv *Itv;

struct oIntervalDouble {
        short  id;
        char  nid;
        char  pad;
        double  inf;
        double  sup;
};

typedef struct oIntervalDouble *IntervalDouble;

struct oforth {
  short  sign;
  short  exp;
  double  u;
  double  l;
};

typedef struct oforth *forth;

struct oIntervalQuad {
        short  id;
        char  nid;
        char  pad;
        forth  inf;
        forth  sup;
};

typedef struct oIntervalQuad *IntervalQuad;

struct oIntervalBigFloat {
        short  id;
        char  nid;
        char  pad;
        BF  inf;
        BF  sup;
};

typedef struct oIntervalBigFloat *IntervalBigFloat;

extern int zerorewrite;
extern int zerorewriteCount;

#define	ZEROREWRITE	  if (zerorewrite && initvp(0,c) ) { *rp = 0; zerorewriteCount++; }

/* general macros */
#define INF(p)  ((p)->inf)
#define SUP(p)  ((p)->sup)

#define NEWItvP(q)  ((q)=(Itv)MALLOC(sizeof(struct oItv)),\
          OID(q)=O_N,NID(q)=N_IP)
#define NEWIntervalDouble(q)  ((q)=(IntervalDouble)MALLOC(sizeof(struct oIntervalDouble)),\
          OID(q)=O_N,NID(q)=N_IntervalDouble)
#define NEWIntervalQuad(q)  ((q)=(IntervalDouble)MALLOC(sizeof(struct oIntervalQuad)),\
          OID(q)=O_N,NID(q)=N_IntervalQuad)
#define NEWIntervalBigFloat(q)  ((q)=(IntervalBigFloat)MALLOC(sizeof(struct oIntervalBigFloat)),\
          OID(q)=O_N,NID(q)=N_IntervalBigFloat)
#define MKItvP(a,b,c)  (NEWItvP(c),(INF(c)=(a),SUP(c)=(b)))
#define MKIntervalDouble(a,b,c)  if((zerorewrite) && ((a)<=0.0) && ((b)>=0.0)) {(c)=0;zerorewriteCount++;}\
	else (NEWIntervalDouble(c),(INF(c)=(a),SUP(c)=(b)))
#define MKIntervalQuad(a,b,c)  (NEWIntervalQuad(c),(INF(c)=(a),SUP(c)=(b)))
#define MKIntervalBigFloat(a,b,c)  (NEWIntervalBigFloat(c),(INF(c)=(a),SUP(c)=(b)))

#define ToItvP(a,c)  (NEWItvP(c),INF(c)=(a),SUP(c)=(a))
#define ToIntervalDouble(a,c)  (NEWIntervalDouble(c),INF(c)=(toRealDown(a)),SUP(c)=(toRealUp(a)))
#define ToIntervalBigFloat(a,c)  (NEWIntervalBigFloat(c),INF(c)=(a),SUP(c)=(a))

#define ITVP(a) (NID(a)==N_IP)
#define ITVD(a) (NID(a)==N_IntervalDouble)
#define ITVQ(a) (NID(a)==N_IntervalQuad)
#define ITVF(a) (NID(a)==N_IntervalBigFloat)

#define	EvalIntervalDouble		0
#define	EvalIntervalQuad		1
#define	EvalIntervalBigFloat	2

/***    engine/p-itv.c    ***/
void  itvtois(Itv, Num *, Num *);
void  istoitv(Num, Num, Itv *);

void    additvp(Itv, Itv, Itv *);
void    subitvp(Itv, Itv, Itv *);
void    mulitvp(Itv, Itv, Itv *);
void    divitvp(Itv, Itv, Itv *);
void    pwritvp(Itv, Num, Itv *);
void    pwritv0p(Itv, long, Itv *);
void    chsgnitvp(Itv, Itv *);
int     initvp(Num , Itv );
int     itvinitvp(Itv, Itv);
int     cmpitvp(Itv, Itv);
void miditvp(Itv, Num *);
void    cupitvp(Itv, Itv, Itv *);
void    capitvp(Itv, Itv, Itv *);
void    widthitvp(Itv, Num *);
void	absitvp(Itv, Num *);
void	absintvalp(Itv, Itv *);
void    distanceitvp(Itv, Itv, Num *);


/***    engine/f-itv.c    ***/

//void  ToBf(Num, BF *);
//void  double2bf(double, BF *);
//double  bf2double(BF);
//void  addulp(IntervalBigFloat, IntervalBigFloat *);
//void  getulp(BF, BF *);
double mpfr2dblDown(mpfr_t);
double mpfr2dblUp(mpfr_t);
void toInterval(Num, int, int, Num *);

void    additvf(IntervalBigFloat, IntervalBigFloat, IntervalBigFloat *);
void    subitvf(IntervalBigFloat, IntervalBigFloat, IntervalBigFloat *);
void    mulitvf(IntervalBigFloat, IntervalBigFloat, IntervalBigFloat *);
void    divitvf(IntervalBigFloat, IntervalBigFloat, IntervalBigFloat *);
void    chsgnitvf(IntervalBigFloat, IntervalBigFloat *);
int     initvf(Num, Itv);
int     itvinitvf(Itv, Itv);
int     cmpitvf(Itv, Itv);
void    pwritvf(Itv, Num, Itv *);
void    pwritv0f(Itv, long, Itv *);
void miditvf(Itv, Num *);
void cupitvf(Itv, Itv, Itv *);
void capitvf(Itv, Itv, Itv *);
void widthitvf(Itv, Num *);
void absitvf(Itv, Num *);
void distanceitvf(Itv, Itv, Num *);

/***    engine/d-itv.c    ***/
double  toRealDown(Num);
double  toRealUp(Num);
void  Num2double(Num, double *, double *);

void    additvd(Num, Num, IntervalDouble *);
void    subitvd(Num, Num, IntervalDouble *);
void    mulitvd(Num, Num, IntervalDouble *);
void    divitvd(Num, Num, IntervalDouble *);
void    chsgnitvd(IntervalDouble, IntervalDouble *);
int     initvd(Num, IntervalDouble);
int     cmpitvd(IntervalDouble, IntervalDouble);
void    pwritvd(Num, Num, IntervalDouble *);
void    pwritv0d(IntervalDouble, long, IntervalDouble *);
void    miditvd(IntervalDouble, Num *);
void    cupitvd(IntervalDouble, IntervalDouble, IntervalDouble *);
void    capitvd(IntervalDouble, IntervalDouble, IntervalDouble *);
void    widthitvd(IntervalDouble, Num *);
void    absitvd(IntervalDouble, Num *);
void    absintvald(IntervalDouble, IntervalDouble *);
void    distanceitvd(IntervalDouble, IntervalDouble, Num *);

/***    builtin/itvnum.c    ***/

void evalitvr(VL ,Obj ,int , int , Obj *);
void evalitvp(VL ,P ,int , int , P *);
void evalitvv(VL ,V ,int , int , Obj *);

extern void (*pi_itv_ft[])();
extern void (*e_itv_ft[])();
extern void (*sin_itv_ft[])();
extern void (*cos_itv_ft[])();
extern void (*tan_itv_ft[])();
extern void (*asin_itv_ft[])();
extern void (*acos_itv_ft[])();
extern void (*atan_itv_ft[])();
extern void (*sinh_itv_ft[])();
extern void (*cosh_itv_ft[])();
extern void (*tanh_itv_ft[])();
extern void (*asinh_itv_ft[])();
extern void (*acosh_itv_ft[])();
extern void (*atanh_itv_ft[])();
extern void (*exp_itv_ft[])();
extern void (*log_itv_ft[])();
extern void (*abs_itv_ft[])();
extern void (*pow_itv_ft[])();

#endif /* end of INTERVAL */
#endif /* end of _INTERVAL_H */
