/* $OpenXM: OpenXM/src/asir99/include/ca-27.h,v 1.1.1.1 1999/11/10 08:12:30 noro Exp $ */
#if LONG_IS_32BIT
#if defined(VISUAL)
typedef _int64 L;
typedef unsigned _int64 UL;
#else
typedef long long L;
typedef unsigned long long UL;
#endif
#else
typedef long L;
typedef unsigned long UL;
#endif
/* data structures */

#define O_N 1

#define N_Q 0

struct oN {
	int p;
	int b[1];
};

typedef struct oN *N;

#if defined(VISUAL)
#include <string.h>
#define index(s,c) strchr(s,c)
#define bzero(s,len) memset(s,0,len)
#define bcopy(x,y,len) memcpy(y,x,len)
#endif

/* general macros */
#define MAX(a,b) ((a) > (b) ? (a) : (b) )
#define MIN(a,b) ((a) > (b) ? (b) : (a) )
#ifdef ABS
#undef ABS
#endif
#define ABS(a) ((a)>0?(a):-(a))
#define ID(p) ((p)->id)
#define OID(p) (((Obj)(p))->id)
#define NID(p) (((Num)(p))->nid)
#define BDY(p) ((p)->body)
#define VR(p) ((p)->v)
#define NAME(p) ((p)->name)
#define NEXT(p) ((p)->next)
#define NM(q) ((q)->nm)
#define DN(q) ((q)->dn)
#define SGN(q) ((q)->sgn)
#define DC(p) ((p)->dc)
#define COEF(p) ((p)->c)
#define DEG(p) ((p)->d)
#define PL(n) ((n)->p)
#define BD(n) ((n)->b)
#define CONT(a) ((a)->cont)
#define UDEG(f) BD(NM(DEG(DC(f))))[0]
#define UCOEF(f) (COEF(DC(f)))
#define LC(f) (NUM(f)?(f):COEF(DC(f)))

#if defined(__GNUC__) || defined(vax) || defined(apollo) || defined(alloca)
#define ALLOCA(d) alloca(d)
#else
#define ALLOCA(d) GC_malloc(d)
#endif

#define MALLOC(d) GC_malloc(d)
#define MALLOC_ATOMIC(d) GC_malloc_atomic(d)
#define CALLOC(d,e) GC_malloc((d)*(e))

#define TRUESIZE(type,n,atype) (sizeof(struct type)+MAX((n),0)*sizeof(atype))
#define NALLOC(d) ((N)MALLOC_ATOMIC(TRUESIZE(oN,(d)-1,int)))

#define W_ALLOC(d) ((int *)ALLOCA(((d)+1)*sizeof(int)))
#define W_CALLOC(n,type,p) \
((p)=(type *)ALLOCA(((n)+1)*sizeof(type)),\
bzero((char *)(p),(int)(((n)+1)*sizeof(type))))

/* cell allocators */

#define STON(i,n)\
(i?((n)=NALLOC(1),PL(n)=1,BD(n)[0]=ABS(i),(n)):((n)=(N)0))

/* predicates */
#define NUM(p) (OID(p)==O_N)
#define UNIN(n) ((n)&&(PL(n)==1)&&(BD(n)[0]==1))
#define EVENN(n) ((!(n))||(!(BD(n)[0]%2)))

/* externals */
extern struct oN oUNIN;
extern N ONEN;

void *GC_malloc(int);
void *GC_malloc_atomic(int);
int cmpn(N,N);

void divn_27(N,N,N *,N *);
void divnmain_27(int,int,int *,int *,int *);
void mulin_27(N,int,int *);
int divin_27(N,int,N *);

void error(char *);
