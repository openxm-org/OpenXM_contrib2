/*
 * Copyright (c) 1994-2000 FUJITSU LABORATORIES LIMITED 
 * All rights reserved.
 * 
 * FUJITSU LABORATORIES LIMITED ("FLL") hereby grants you a limited,
 * non-exclusive and royalty-free license to use, copy, modify and
 * redistribute, solely for non-commercial and non-profit purposes, the
 * computer program, "Risa/Asir" ("SOFTWARE"), subject to the terms and
 * conditions of this Agreement. For the avoidance of doubt, you acquire
 * only a limited right to use the SOFTWARE hereunder, and FLL or any
 * third party developer retains all rights, including but not limited to
 * copyrights, in and to the SOFTWARE.
 * 
 * (1) FLL does not grant you a license in any way for commercial
 * purposes. You may use the SOFTWARE only for non-commercial and
 * non-profit purposes only, such as academic, research and internal
 * business use.
 * (2) The SOFTWARE is protected by the Copyright Law of Japan and
 * international copyright treaties. If you make copies of the SOFTWARE,
 * with or without modification, as permitted hereunder, you shall affix
 * to all such copies of the SOFTWARE the above copyright notice.
 * (3) An explicit reference to this SOFTWARE and its copyright owner
 * shall be made on your publication or presentation in any form of the
 * results obtained by use of the SOFTWARE.
 * (4) In the event that you modify the SOFTWARE, you shall notify FLL by
 * e-mail at risa-admin@sec.flab.fujitsu.co.jp of the detailed specification
 * for such modification or the source code of the modified part of the
 * SOFTWARE.
 * 
 * THE SOFTWARE IS PROVIDED AS IS WITHOUT ANY WARRANTY OF ANY KIND. FLL
 * MAKES ABSOLUTELY NO WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY, AND
 * EXPRESSLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT OF THIRD PARTIES'
 * RIGHTS. NO FLL DEALER, AGENT, EMPLOYEES IS AUTHORIZED TO MAKE ANY
 * MODIFICATIONS, EXTENSIONS, OR ADDITIONS TO THIS WARRANTY.
 * UNDER NO CIRCUMSTANCES AND UNDER NO LEGAL THEORY, TORT, CONTRACT,
 * OR OTHERWISE, SHALL FLL BE LIABLE TO YOU OR ANY OTHER PERSON FOR ANY
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, PUNITIVE OR CONSEQUENTIAL
 * DAMAGES OF ANY CHARACTER, INCLUDING, WITHOUT LIMITATION, DAMAGES
 * ARISING OUT OF OR RELATING TO THE SOFTWARE OR THIS AGREEMENT, DAMAGES
 * FOR LOSS OF GOODWILL, WORK STOPPAGE, OR LOSS OF DATA, OR FOR ANY
 * DAMAGES, EVEN IF FLL SHALL HAVE BEEN INFORMED OF THE POSSIBILITY OF
 * SUCH DAMAGES, OR FOR ANY CLAIM BY ANY OTHER PARTY. EVEN IF A PART
 * OF THE SOFTWARE HAS BEEN DEVELOPED BY A THIRD PARTY, THE THIRD PARTY
 * DEVELOPER SHALL HAVE NO LIABILITY IN CONNECTION WITH THE USE,
 * PERFORMANCE OR NON-PERFORMANCE OF THE SOFTWARE.
 *
 * $OpenXM: OpenXM_contrib2/asir2018/include/ca.h,v 1.20 2021/01/25 00:39:52 noro Exp $
*/
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <mpc.h>
#include <limits.h>
#include "wsio.h"
#include "gc.h"

/* from mpfr-impl.h */
#define MPFR_PREC(x)      ((x)->_mpfr_prec)
#define MPFR_EXP(x)       ((x)->_mpfr_exp)
#define MPFR_MANT(x)      ((x)->_mpfr_d)
#define MPFR_LAST_LIMB(x) ((MPFR_PREC (x) - 1) / GMP_NUMB_BITS)
#define MPFR_LIMB_SIZE(x) (MPFR_LAST_LIMB (x) + 1)

#define RAT_CEIL(nm,dn) (((nm)+(dn)-1)/((dn)))
#define MPFR_LIMB_SIZE_REAL(x) (RAT_CEIL(MPFR_PREC((x)),sizeof(mp_limb_t)*CHAR_BIT) * (sizeof(mp_limb_t)/sizeof(int)) )
#define MPFR_LIMB_SIZE_BODY(x) (RAT_CEIL(MPFR_PREC((x)),sizeof(unsigned int)*CHAR_BIT))

#if defined(hpux)
#include <netinet/in.h>
# define setbuffer(FP,buf,siz) setvbuf(FP,buf,_IOFBF,siz)
#endif

#if !defined(VISUAL) && !defined(__MINGW32__)
#include <unistd.h>
#include <sys/param.h>
#endif

#if defined(linux) || (defined(sun) && !defined(SYSV)) || defined(news5000) || (defined(mips) && defined(ultrix))
#include <alloca.h>
#endif

#if (defined(sun) && defined(SYSV))
#define alloca(x) __builtin_alloca(x)
#endif

#if defined(VISUAL) || defined(__MINGW32__)
#include <limits.h>
#include <malloc.h>
#endif

#if 0
#include <sys/types.h>
typedef caddr_t pointer;
#endif


typedef void * pointer;

#if defined(ANDROID)
#include <strings.h>
#include <string.h>
#define index(s,c) strchr(s,c)
#define rindex(s,c) strrchr(s,c)
#elif defined(sun)
#include <strings.h>
#else
#include <string.h>
#if defined(VISUAL) || defined(__MINGW32__)
#define index(s,c) strchr(s,c)
#define bzero(s,len) memset(s,0,len)
#define bcopy(x,y,len) memcpy(y,x,len)
#endif
#endif

#define NULLP ((void *)0)

#define TODO    printf("%s: not implemented!\n", __func__)

#define COPY(a,b) ((b)=(a))
#define FREEN(p) 
#define FREEQ(p) 
#define FREE(p)
#define INITRC(p)

/* data structures */

#define O_N 1
#define O_P 2
#define O_R 3
#define O_LIST 4
#define O_VECT 5
#define O_MAT 6
#define O_STR 7
#define O_COMP 8
#define O_DP 9
#define O_USINT 10
#define O_ERR 11
#define O_GF2MAT 12
#define O_MATHCAP 13
#define O_F 14
#define O_GFMMAT 15
#define O_BYTEARRAY 16
#define O_QUOTE 17
#define O_OPTLIST 18
#define O_SYMBOL 19
#define O_RANGE 20
#define O_TB 21
#define O_DPV 22
#define O_QUOTEARG 23
#define O_VOID -1
/* IMAT */
#define O_IMAT 24
/* IMAT */
#define O_NBP 25
#define O_DPM 26

#define N_Q 0
#define N_R 1
#define N_A 2
#define N_B 3
#define N_NEXT_B    (N_B)
#if defined(INTERVAL)
#define N_Quad  (N_NEXT_B+1)
#define  N_IP  (N_NEXT_B+2)
#define N_IntervalDouble  (N_NEXT_B+3)
#define N_IntervalQuad  (N_NEXT_B+4)
#define N_IntervalBigFloat  (N_NEXT_B+5)
#define N_PRE_C  N_IntervalBigFloat
#define N_BASE (N_NEXT_B+5)
#else
#define N_BASE N_NEXT_B
#endif
#define N_C (N_BASE+1)
#define N_M (N_BASE+2)
#define N_LM (N_BASE+3)
#define N_GF2N (N_BASE+4)
#define N_GFPN (N_BASE+5)
#define N_GFS (N_BASE+6)
#define N_GFSN (N_BASE+7)
#define N_DA (N_BASE+8)
#define N_PARIB (N_BASE+11)

#define G_Z 0
#define G_Q 1

#define ORD_REVGRADLEX 0
#define ORD_GRADLEX 1
#define ORD_LEX 2

typedef enum {
  A_end=0,A_fnode,A_arf,A_int,A_str,A_internal,A_node,A_notimpl,A_func
} farg_type;

#if SIZEOF_LONG == 8
typedef long L;
typedef unsigned long UL;
#elif defined(HAVE_UNSIGNED_LONG_LONG)
typedef long long L;
typedef unsigned long long UL;
#elif defined(_MSC_VER)
typedef _int64 L;
typedef unsigned _int64 UL;
#endif

#if defined(__GNUC__) && SIZEOF_LONG == 8
typedef __uint128_t U128;
typedef __int64_t L64;
typedef __int128_t L128;

#endif

typedef struct oZ {
  short id;
  char nid;
  char z;
  mpz_t body;
} *Z;

typedef struct oQ {
  short id;
  char nid;
  char z;
  mpq_t body;
} *Q;

typedef struct oLM {
  short id;
  char nid;
  char pad;
  mpz_t body;
} *LM;

typedef struct oUP2 {
  int w;
  unsigned int b[1];
} *UP2;

typedef struct _oUP2 {
  int w;
  unsigned int *b;
} *_UP2;

#define UP2_DENSE 0
#define UP2_SPARSE 1

typedef struct oGEN_UP2 {
  int id;
  UP2 dense;
  UP2 sparse;
} *GEN_UP2;

typedef struct oV {
  char *name;
  pointer attr;
  pointer priv;
} *V;

typedef struct oReal {
  short id;
  char nid;
  char pad;
  double body;
} *Real;

typedef struct oAlg {
  short id;
  char nid;
  char pad;
  struct oObj *body;
} *Alg;

typedef struct oBF {
  short id;
  char nid;
  char pad;
  mpfr_t body;
} *BF;

typedef struct oC {
  short id;
  char nid;
  char pad;
  struct oNum *r,*i;
} *C;

typedef struct oGF2N {
  short id;
  char nid;
  char pad;
  struct oUP2 *body;
} *GF2N;

typedef struct oGFPN {
  short id;
  char nid;
  char pad;
  struct oUP *body;
} *GFPN;

typedef struct oDAlg {
  short id;
  char nid;
  char pad;
  struct oDP *nm;
  struct oZ *dn;
} *DAlg;

typedef struct oNum {
  short id;
  char nid;
  char pad;
} *Num;

typedef struct oMQ {
  short id;
  char nid;
  char pad;
  int cont;
} *MQ;

typedef struct oGFS {
  short id;
  char nid;
  char pad;
  int cont;
} *GFS;

typedef struct oGFSN {
  short id;
  char nid;
  char pad;
  struct oUM *body;
} *GFSN;

typedef struct oP {
  short id;
  short pad;
  V v;
  struct oDCP *dc;
} *P;

typedef struct oR {
  short id;
  short reduced;
  P nm;
  P dn;
} *R;

typedef struct oVECT {
  short id;
  short pad;
  int len;
  pointer *body;
} *VECT;

typedef struct oMAT {
  short id;
  short pad;
  int row,col;
  pointer **body;
} *MAT;

typedef struct oGF2MAT {
  short id;
  short pad;
  int row,col;
  unsigned int **body;
} *GF2MAT, *GFMMAT;

/* IMAT */
#define IMATCH 64

typedef struct oIENT {
  int cr;
  int row, col;
  pointer *body;
} IENT;

typedef struct oIMATC {
  pointer *fore;
  pointer *next;
  IENT ient[IMATCH];
} *IMATC;

typedef struct oIMAT {
  short id;
  int row, col, clen;
  pointer *root;
  pointer *toor;
} *IMAT;
/* IMAT */
typedef struct oLIST {
  short id;
  short pad;
  struct oNODE *body;
} *LIST;

typedef struct oSTRING {
  short id;
  short pad;
  char *body;
} *STRING;

typedef struct oCOMP {
  short id;
  short type;
  struct oObj *member[1];
} *COMP;

typedef struct oDP {
  short id;
  short nv;
  int sugar;
  struct oMP *body;
} *DP;

typedef struct oDPV {
  short id;
  int len;
  int sugar;
  struct oDP **body;
} *DPV;


typedef struct oUSINT {
  short id;
  short pad;
  unsigned int body;
} *USINT;

typedef struct oERR {
  short id;
  short pad;
  struct oObj *body;
} *ERR;

typedef struct oMATHCAP {
  short id;
  short pad;
  struct oLIST *body;
} *MATHCAP;

typedef struct oBYTEARRAY {
  short id;
  short pad;
  int len;
  unsigned char *body;
} *BYTEARRAY;

typedef struct oQUOTE {
  short id;
  short pad;
  pointer body;
  struct oVS *pvs;
} *QUOTE;

typedef struct oQUOTEARG {
  short id;
  short pad;
  farg_type type;
  pointer body;
} *QUOTEARG;

typedef struct oOPTLIST {
  short id;
  short pad;
  struct oNODE *body;
} *OPTLIST;

typedef struct oSYMBOL {
  short id;
  short pad;
  char *name;
  int value;
} *SYMBOL;

typedef struct oRANGE {
  short id;
  short pad;
  struct oObj *start,*end;
} *RANGE;

typedef struct oTB {
  short id;
  short pad;
  int size,next;
  char **body;
} *TB;

typedef struct oNBP {
  short id;
  short pad;
  struct oNODE *body;
} *NBP;

/* non-commutative bivariate monomial */

typedef struct oNBM {
  int d;
  P c;
  unsigned int *b;
} *NBM;

#define NEWNBM(p) ((p)=(NBM)MALLOC(sizeof(struct oNBM)))
#define NEWNBMBDY(p,d) \
((p)->b=(unsigned int *)MALLOC((((d)+31)/32)*sizeof(unsigned int)))
#define NEWNBP(p) ((p)=(NBP)MALLOC(sizeof(struct oNBP)),OID(p)=O_NBP)
#define MKNBP(p,b) (NEWNBP(p),BDY(p)=(b))

#define NBM_GET(a,j) (((a)[(j)>>5]&(1<<((j)&31)))?1:0)
#define NBM_SET(a,j) ((a)[(j)>>5]|=(1<<((j)&31)))
#define NBM_CLR(a,j) ((a)[(j)>>5]&=(~(1<<((j)&31))))

typedef struct oObj {
  short id;
  short pad;
} *Obj;

typedef struct oDCP {
  Z d;
  P c;
  struct oDCP *next;  
} *DCP;

typedef struct oMP {
  struct oDL *dl;
  Obj c;
  struct oMP *next;
} *MP;

typedef struct oDPM {
  short id;
  short nv;
  int sugar;
  struct oDMM *body;
} *DPM;

typedef struct oDMM {
  int pos;
  struct oDL *dl;
  Obj c;
  struct oDMM *next;
} *DMM;

typedef struct oDMMstack {
  int rank,ordtype;
  DMM *in;
  DMM *sum;
  LIST obj;
  struct oDMMstack *next;
} *DMMstack;

typedef struct oDMMstack_array {
  int len;
  DMMstack *body;
} *DMMstack_array;

typedef struct oDL {
  int td;
  int d[1];
} *DL;

struct dp_pairs {
  int dp1, dp2;
  DL lcm;
  int sugar;
  struct dp_pairs *next;
};

typedef struct dp_pairs *DP_pairs;

struct p_pair {
  struct oUM *p0;
  struct oUM *p1;
  struct p_pair *next;
};

struct oMF {
  int m;
  P f;
};

/* 
 * compressed DP
 */

typedef struct oCDP {
  int len;
  long psindex;
  unsigned int *body;
} *CDP;

typedef struct oCM {
  int index;
  int c;
} *CM;

/* bucket list for DL */

typedef struct oDLBUCKET {
  int td;
  struct oNODE *body;
  struct oDLBUCKET *next;
} *DLBUCKET;

typedef struct oGeoBucket {
  int m;
  struct oNODE *body[32];
} *GeoBucket;

typedef struct oVL {
  V v;
  struct oVL *next;
} *VL;

typedef struct oNODE {
  pointer body;
  struct oNODE *next;
} *NODE;

typedef struct oNODE2 {
  pointer body1,body2;
  struct oNODE2 *next;
} *NODE2;

/* univariate poly over small finite field; dense */
typedef struct oUM {
  int d;
  int c[1];
} *UM;

/* univariate poly with nadic coeff */
typedef struct oLUM {
  int d;
  int *c[1];
} *LUM;

/* bivariate poly over small finite field; dense */

typedef struct oBM {
  int d;
  UM c[1];
} *BM;

typedef struct oML {
  int n;
  int mod;
  int bound;
  pointer c[1];
} *ML;

typedef struct oUB {
  int d;
  Z c[1];
} *UB;

typedef struct oVN {
  V v;
  int n;
} *VN;

typedef struct oUP {
  int d;
  Num c[1];
} *UP;

typedef struct oDUM {
  int n;
  UM f;
} *DUM;

struct order_pair {
  int order, length;
};

struct sparse_weight {
  int pos, value;
};

#define IS_DENSE_WEIGHT 0
#define IS_SPARSE_WEIGHT 1
#define IS_BLOCK 2

struct weight_or_block {
  int type;
  int length;
  union {
    int *dense_weight;
    struct sparse_weight *sparse_weight;
    struct {
      int order, start;
    } block;
  } body;
};

struct order_spec {
  int id;
  Obj obj;
  int nv;
  int module_ordtype; /* 0=TOP, 1=POT, 2=wPOT, 3=Schreyer */
  int pot_nelim; /* size of positions for pot-elimination order */
  int *top_weight;
  int module_rank;
  int *module_top_weight;
  int *module_base_ord;
  struct order_spec *base; /* for schreyer order */
  DMMstack dmmstack;
  int (*cmpdl)(int n,DL d1,DL d2);
  union {
    int simple;
    struct {
      int length;
      struct order_pair *order_pair;
    } block;
    struct {
      int row;
      int **matrix;
    } matrix;
    struct {
      int length;
      struct weight_or_block *w_or_b;
    } composite;
  } ord;
};

struct modorder_spec {
  /* id : ORD_REVGRADLEX, ORD_GRADLEX, ORD_LEX */
  int id;
  Obj obj;
  int len;
  int *degree_shift;
};

typedef struct oNumberField {
  int n;
  int psn;
  int dim;
  VL vl;
  P *defpoly;
  DP *mb;
  DP *ps;
  struct oDAlg *one;
  NODE ind;
  struct order_spec *spec;
} *NumberField;

/* structure for cputime */

struct oEGT {
  double exectime,gctime;
};

/* constant */

/* ground finite field specification */
#define FF_NOT_SET 0
#define FF_GFP 1
#define FF_GF2N 2
#define FF_GFPN 3
#define FF_GFS 4
#define FF_GFSN 5

/* include interval.h */
#include "interval.h"

#define INDEX 100

#if defined(USE_FLOAT)
typedef float ModNum;
#define NPrimes 536
#else
typedef unsigned int ModNum;
#define NPrimes 13681
#endif

/* general macros */
#if defined(MAX)
#undef MAX
#endif
#if defined(MIN)
#undef MIN
#endif
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
#define CONT(a) ((a)->cont)
#define UDEG(f) ZTOS(DEG(DC(f)))
#define UCOEF(f) (COEF(DC(f)))
#define LC(f) (NUM(f)?(f):COEF(DC(f)))

/* memory allocators (W_... : uses alloca) */

#define MALLOC(d) Risa_GC_malloc(d)
#define MALLOC_ATOMIC(d) Risa_GC_malloc_atomic(d)
#define MALLOC_ATOMIC_IGNORE_OFF_PAGE(d) Risa_GC_malloc_atomic_ignore_off_page(d)
#define REALLOC(p,d) Risa_GC_realloc(p,d)
#define GCFREE(p) Risa_GC_free(p)
#define CALLOC(d,e) MALLOC((d)*(e))

#if 0
#if !defined(__CYGWIN__) && (defined(__GNUC__) || defined(vax) || defined(apollo) || defined(alloca) || defined(VISUAL) || defined(__MINGW32__))
#define ALLOCA(d) alloca(d)
#else
#define ALLOCA(d) MALLOC(d)
#endif
#endif

#define ALLOCA(d) MALLOC(d)

/* for handling signals */
#if defined(HAVE_SIGACTION)  /* POSIX */
void (*set_signal(int sig, void (*handler)(int)))(int);
#define set_signal_for_restart(x,y) (0)
#else
#define set_signal(x,y)             (signal(x,y))
#define set_signal_for_restart(x,y) (signal(x,y))
#endif

/* for setjmp/longjmp compatibility */
#if defined(ANDROID)
#define JMP_BUF jmp_buf
#define SETJMP(x) _setjmp(x)
#define LONGJMP(x,y) _longjmp(x,y)
#elif !defined(_WIN64) && (defined(__CYGWIN__) || defined(HAVE_SIGACTION) || (defined(__x86_64) && !defined(__MINGW32__)))
#define JMP_BUF sigjmp_buf
#define SETJMP(x) sigsetjmp(x,~0)
#define LONGJMP(x,y) siglongjmp(x,y)
#else
#define JMP_BUF jmp_buf
#define SETJMP(x) setjmp(x)
#define LONGJMP(x,y) longjmp(x,y)
#endif

#define TRUESIZE(type,n,atype) (sizeof(struct type)+MAX((n),0)*sizeof(atype))
#define UMALLOC(d) ((UM)MALLOC(TRUESIZE(oUM,d,int)))
#define UPALLOC(d) ((UP)MALLOC(TRUESIZE(oUP,(d),Num)))
#define C_UMALLOC(d) ((UM)MALLOC(TRUESIZE(oUM,d,int)))
#define MLALLOC(d) ((ML)MALLOC(TRUESIZE(oML,d,pointer)))

#define W_ALLOC(d) ((int *)ALLOCA(((d)+1)*sizeof(int)))
#define W_CALLOC(n,type,p) \
((p)=(type *)ALLOCA(((n)+1)*sizeof(type)),\
bzero((char *)(p),(int)(((n)+1)*sizeof(type))))
#define W_UMALLOC(d) ((UM)ALLOCA(TRUESIZE(oUM,d,int)))
#define W_UPALLOC(d) ((UP)ALLOCA(TRUESIZE(oUP,(d),Num)))
#define W_MLALLOC(d) ((ML)ALLOCA(TRUESIZE(oML,d,pointer)))
#define W_LUMALLOC(n,bound,p)\
{\
  LUM ___q___;\
  int ___i___,**___c___;\
  (___q___) = (LUM)ALLOCA(TRUESIZE(oLUM,(n),int *));\
  DEG(___q___) = n;\
  for ( ___i___ = 0, ___c___ = (int **)COEF(___q___); ___i___ <= n; ___i___++ ) {\
    ___c___[___i___] = (int *)ALLOCA(((bound)+1)*sizeof(int));\
    bzero((char *)___c___[___i___],((bound)+1)*sizeof(int));\
  }\
  (p) = ___q___;\
}

#define W_BMALLOC(dx,dy,p)\
{\
  BM ___q___;\
  int ___i___;\
  UM *___c___;\
  (___q___) = (BM)ALLOCA(TRUESIZE(oBM,(dy),UM));\
  DEG(___q___) = dy;\
  ___c___ = (UM *)COEF(___q___);\
  for ( ___i___ = 0; ___i___ <= dy; ___i___++ ) {\
    ___c___[___i___] = W_UMALLOC(dx);\
    clearum(___c___[___i___],dx);\
  }\
  (p) = ___q___;\
}

#define NEWUP2(q,w)\
((q)=(UP2)MALLOC_ATOMIC(TRUESIZE(oUP2,(w)-1,unsigned int)),\
bzero((char *)(q)->b,(w)*sizeof(unsigned int)))
#define W_NEWUP2(q,w)\
((q)=(UP2)ALLOCA(TRUESIZE(oUP2,(w)-1,unsigned int)),\
bzero((char *)(q)->b,(w)*sizeof(unsigned int)))
#define W_NEW_UP2(q,w)\
((q).b=(unsigned int *)ALLOCA((w)*sizeof(unsigned int)))

/* cell allocators */
#define NEWZ(q) ((q)=(Z)MALLOC(sizeof(struct oZ)),OID(q)=O_N,NID(q)=N_Q,(q)->z=1,mpz_init((q)->body))
#define NEWQ(q) ((q)=(Q)MALLOC(sizeof(struct oQ)),OID(q)=O_N,NID(q)=N_Q,(q)->z=0,mpq_init((q)->body))
#define NEWMQ(q) ((q)=(MQ)MALLOC_ATOMIC(sizeof(struct oMQ)),OID(q)=O_N,NID(q)=N_M)
#define NEWGFS(q) ((q)=(GFS)MALLOC_ATOMIC(sizeof(struct oGFS)),OID(q)=O_N,NID(q)=N_GFS)
#define NEWGFSN(q) ((q)=(GFSN)MALLOC(sizeof(struct oGFSN)),OID(q)=O_N,NID(q)=N_GFSN)
#define NEWP(p) ((p)=(P)MALLOC(sizeof(struct oP)),OID(p)=O_P)
#define NEWR(r) ((r)=(R)MALLOC(sizeof(struct oR)),OID(r)=O_R,(r)->reduced=0)
#define NEWLIST(l) ((l)=(LIST)MALLOC(sizeof(struct oLIST)),OID(l)=O_LIST)
#define NEWVECT(l) ((l)=(VECT)MALLOC(sizeof(struct oVECT)),OID(l)=O_VECT)
#define NEWSTR(l) ((l)=(STRING)MALLOC(sizeof(struct oSTRING)),OID(l)=O_STR)
#define NEWCOMP(c,n) ((c)=(COMP)MALLOC(sizeof(struct oCOMP)+((n)-1)*sizeof(Obj)),OID(c)=O_COMP)
#define NEWDP(d) ((d)=(DP)MALLOC(sizeof(struct oDP)),OID(d)=O_DP)
#define NEWDPV(d) ((d)=(DPV)MALLOC(sizeof(struct oDPV)),OID(d)=O_DPV)
#define NEWDPM(d) ((d)=(DPM)MALLOC(sizeof(struct oDPM)),OID(d)=O_DPM)
#define NEWUSINT(u) ((u)=(USINT)MALLOC_ATOMIC(sizeof(struct oUSINT)),OID(u)=O_USINT)
#define NEWERR(e) ((e)=(ERR)MALLOC(sizeof(struct oERR)),OID(e)=O_ERR)
#define NEWMATHCAP(e) ((e)=(MATHCAP)MALLOC(sizeof(struct oMATHCAP)),OID(e)=O_MATHCAP)
#define NEWBYTEARRAY(e) ((e)=(BYTEARRAY)MALLOC(sizeof(struct oBYTEARRAY)),OID(e)=O_BYTEARRAY)
#define NEWQUOTE(e) ((e)=(QUOTE)MALLOC(sizeof(struct oQUOTE)),OID(e)=O_QUOTE)
#define NEWQUOTEARG(e) ((e)=(QUOTEARG)MALLOC(sizeof(struct oQUOTEARG)),OID(e)=O_QUOTEARG)
#define NEWOPTLIST(l) ((l)=(OPTLIST)MALLOC(sizeof(struct oOPTLIST)),OID(l)=O_OPTLIST)
#define NEWSYMBOL(l) ((l)=(SYMBOL)MALLOC(sizeof(struct oSYMBOL)),OID(l)=O_SYMBOL)
#define NEWRANGE(l) ((l)=(RANGE)MALLOC(sizeof(struct oRANGE)),OID(l)=O_RANGE)
#define NEWTB(l) ((l)=(TB)MALLOC(sizeof(struct oTB)),OID(l)=O_TB,(l)->size=256,(l)->next=0,(l)->body=(char **)MALLOC((l)->size*sizeof(char *)))

#define NEWNODE(a) ((a)=(NODE)MALLOC(sizeof(struct oNODE)))
#define NEWDC(dc) ((dc)=(DCP)MALLOC(sizeof(struct oDCP)))
#define NEWV(v) ((v)=(V)MALLOC(sizeof(struct oV)))
#define NEWVL(vl) ((vl)=(VL)MALLOC(sizeof(struct oVL)))
#define NEWMP(m) ((m)=(MP)MALLOC(sizeof(struct oMP)))
#define NEWDMM(m) ((m)=(DMM)MALLOC(sizeof(struct oDMM)))
#define NEWDMMstack(m) ((m)=(DMMstack)MALLOC(sizeof(struct oDMMstack)))
#define NEWDMMstack_array(m) ((m)=(DMMstack_array)MALLOC(sizeof(struct oDMMstack_array)))
#define NEWDLBUCKET(a) ((a)=(DLBUCKET)MALLOC(sizeof(struct oDLBUCKET)))
#define NEWDPP(a) ((a)=(DP_pairs)MALLOC(sizeof(struct dp_pairs)))

#define NEWMAT(l) ((l)=(MAT)MALLOC(sizeof(struct oMAT)),OID(l)=O_MAT)
#define NEWGF2MAT(l) ((l)=(GF2MAT)MALLOC(sizeof(struct oGF2MAT)),OID(l)=O_GF2MAT)
#define NEWGFMMAT(l) ((l)=(GFMMAT)MALLOC(sizeof(struct oGF2MAT)),OID(l)=O_GFMMAT)
/* IMAT */
#define NEWIMAT(l) ((l)=(IMAT)MALLOC(sizeof(struct oIMAT)),OID(l)=O_IMAT,l->clen=0,l->root=0,l->toor=0)
#define NEWIENT(l) ((l)=(IMATC)MALLOC(sizeof(struct oIMATC)),l->fore=0,l->next=0)
/* IMAT */
#define NEWReal(q) ((q)=(Real)MALLOC_ATOMIC(sizeof(struct oReal)),OID(q)=O_N,NID(q)=N_R)
#define NEWAlg(r) ((r)=(Alg)MALLOC(sizeof(struct oAlg)),OID(r)=O_N,NID(r)=N_A)
#define NEWDAlg(r) ((r)=(DAlg)MALLOC(sizeof(struct oDAlg)),OID(r)=O_N,NID(r)=N_DA)
#define NEWBF(q) ((q)=(BF)MALLOC(sizeof(struct oBF)),OID(q)=O_N,NID(q)=N_B)
#define NEWPARIBF(q,l) ((q)=(BF)MALLOC_ATOMIC(TRUESIZE(oBF,(l)-1,long)),OID(q)=O_N,NID(q)=N_PARIB)
#define NEWC(r) ((r)=(C)MALLOC(sizeof(struct oC)),OID(r)=O_N,NID(r)=N_C)
#define NEWLM(r) ((r)=(LM)MALLOC(sizeof(struct oLM)),OID(r)=O_N,NID(r)=N_LM)
#define NEWGF2N(r) ((r)=(GF2N)MALLOC(sizeof(struct oGF2N)),OID(r)=O_N,NID(r)=N_GF2N)
#define NEWGFPN(r) ((r)=(GFPN)MALLOC(sizeof(struct oGFPN)),OID(r)=O_N,NID(r)=N_GFPN)
#define NEWDL(d,n) \
((d)=(DL)MALLOC_ATOMIC(TRUESIZE(oDL,(n)-1,int)),bzero((char *)(d),TRUESIZE(oDL,(n)-1,int)))
#define NEWDL_NOINIT(d,n) \
((d)=(DL)MALLOC_ATOMIC(TRUESIZE(oDL,(n)-1,int)))

#define MKP(v,dc,p) \
((!DEG(dc)&&!NEXT(dc))?((p)=COEF(dc)):(NEWP(p),VR(p)=(v),DC(p)=(dc),(p)))
#define MKV(v,p) \
(NEWP(p),VR(p)=(v),NEWDC(DC(p)),\
DEG(DC(p))=ONE,COEF(DC(p))=(P)ONE,NEXT(DC(p))=0)
#define MKRAT(n,d,r,p) \
(NEWR(p),NM(p)=(n),DN(p)=(d),(p)->reduced=(r))
#define MKMV(v,p) \
(NEWP(p),VR(p)=(v),NEWDC(DC(p)),\
DEG(DC(p))=ONE,COEF(DC(p))=(P)ONEM,NEXT(DC(p))=0)
#define MKNODE(a,b,c) \
(NEWNODE(a),(a)->body=(pointer)b,NEXT(a)=(NODE)(c))
#define NEWNODE2(a) ((a)=(NODE2)MALLOC(sizeof(struct oNODE2)))
#define MKNODE2(a,b,c,d) \
(NEWNODE2(a),(a)->body1=(pointer)b,(a)->body2=(pointer)c,NEXT(a)=(NODE2)(d))
#define BDY1(a) ((a)->body1)
#define BDY2(a) ((a)->body2)
#define MKLIST(a,b) (NEWLIST(a),(a)->body=(NODE)(b))
#define MKVECT(m,l) \
(NEWVECT(m),(m)->len=(l),(m)->body=(pointer *)CALLOC((l),sizeof(pointer)))
#define MKMAT(m,r,c) \
(NEWMAT(m),(m)->row=(r),(m)->col=(c),(m)->body=(pointer **)almat_pointer((r),(c)))
#define TOGF2MAT(r,c,b,m) (NEWGF2MAT(m),(m)->row=(r),(m)->col=(c),(m)->body=(b))
#define TOGFMMAT(r,c,b,m) (NEWGFMMAT(m),(m)->row=(r),(m)->col=(c),(m)->body=(b))
#define MKSTR(a,b) (NEWSTR(a),(a)->body=(char *)(b))
#define MKDP(n,m,d) (NEWDP(d),(d)->nv=(n),BDY(d)=(m))
#define MKDPV(len,m,d) (NEWDPV(d),(d)->len=(len),BDY(d)=(m))
#define MKDPM(n,m,d) (NEWDPM(d),(d)->nv=(n),BDY(d)=(m))
#define MKLM(b,l) (!(b)?(l)=0:(NEWLM(l),(l)->body[0]=(b)[0],(l)))
#define MKGF2N(b,l) (!(b)?(l)=0:(NEWGF2N(l),(l)->body=(b),(l)))
#define MKGFPN(b,l) (!(b)?(l)=0:(NEWGFPN(l),(l)->body=(b),(l)))
#define MKUSINT(u,b) (NEWUSINT(u),(u)->body=(unsigned)(b))
#define MKERR(e,b) (NEWERR(e),(e)->body=(Obj)(b))
#define MKMATHCAP(e,b) (NEWMATHCAP(e),(e)->body=(LIST)(b))
#define MKBYTEARRAY(m,l) \
(NEWBYTEARRAY(m),(m)->len=(l),(m)->body=(unsigned char *)MALLOC_ATOMIC((l)),bzero((m)->body,(l)))
#define MKQUOTE(q,b) (NEWQUOTE(q),(q)->body=(pointer)(b),(q)->pvs=CPVS)
#define MKQUOTEARG(q,t,b) (NEWQUOTEARG(q),(q)->type=(t),(q)->body=(pointer)(b))

#define NEXTDC(r,c) \
if(!(r)){NEWDC(r);(c)=(r);}else{NEWDC(NEXT(c));(c)=NEXT(c);}
#define NEXTNODE(r,c) \
if(!(r)){NEWNODE(r);(c)=(r);}else{NEWNODE(NEXT(c));(c)=NEXT(c);}
#define NEXTMP(r,c) \
if(!(r)){NEWMP(r);(c)=(r);}else{NEWMP(NEXT(c));(c)=NEXT(c);}
#define NEXTMP2(r,c,s) \
if(!(r)){(c)=(r)=(s);}else{NEXT(c)=(s);(c)=(s);}
#define NEXTDLBUCKET(r,c) \
if(!(r)){NEWDLBUCKET(r);(c)=(r);}else{NEWDLBUCKET(NEXT(c));(c)=NEXT(c);}
#define NEXTVL(r,c) \
if(!(r)){NEWVL(r);(c)=(r);}else{NEWVL(NEXT(c));(c)=NEXT(c);}
#define NEXTDPP(r,c) \
if(!(r)){NEWDPP(r);(c)=(r);}else{NEWDPP(NEXT(c));(c)=NEXT(c);}
#define NEXTDMM(r,c) \
if(!(r)){NEWDMM(r);(c)=(r);}else{NEWDMM(NEXT(c));(c)=NEXT(c);}

/* convertors */
#define NTOQ(n,s,z) (!(n)?((z)=0):((s)<0?mpz_neg(BDY(n),BDY(n)):0,MPZTOZ(n,z)))
#define NDTOQ(n,s,q) (!(n)?((q)=0):((s)<0?mpz_neg(BDY(n),BDY(n)):0,MPDNTOQ(n,d,q)))

#define DUPZ(p,q) (NEWZ(q),BDY(q)=BDY(p))
#define DUPQ(p,q) (NEWQ(q),BDY(q)=BDY(p))

#define STOZ(n,q) ((!(n))?(void)((q)=0):(NEWZ(q),mpz_set_si(BDY(q),n)))
#define UTOZ(n,q) ((!(n))?(void)((q)=0):(NEWZ(q),mpz_set_ui(BDY(q),n)))

/* for initializing static object */
#define STOZ0(n,q) (OID(q)=O_N,NID(q)=N_Q,(q)->z=1,mpz_init(BDY(q)),mpz_set_si(BDY(q),n))
#define STOLM0(n,q) (OID(q)=O_N,NID(q)=N_LM,mpz_init(BDY(q)),mpz_set_si(BDY(q),n))

#define ZTOS(q) (!(q)?0:((q)->z==1?mpz_get_si(BDY((Z)q)):(error("ZTOS : invalid argument"),0)))
#define LMTOS(q) (!(q)?0:mpz_get_si(BDY(q)))

#define UTOMQ(a,b) ((a)?(NEWMQ(b),CONT(b)=(unsigned int)(a),(b)):((b)=0))
#define MKGFS(a,b) ((NEWGFS(b),CONT(b)=(a),(b)))
#define MKGFSN(a,b) ((DEG(a)>=0)?(NEWGFSN(b),BDY(b)=(a),(b)):((b)=0))
#define STOMQ(a,b) ((a)?(NEWMQ(b),CONT(b)=(a),(b)):((b)=0))


#define PTOR(a,b) (!(a)?((b)=0):(NEWR(b),NM(b)=(a),DN(b)=(P)ONE,(b)->reduced=1,(b)))

#define MKReal(a,b) (!(a)?((b)=0):(NEWReal(b),BDY(b)=(a),(b)))
#define MKAlg(b,r) (!(b)?((r)=0):NUM(b)?((r)=(Alg)(b)):(NEWAlg(r),BDY(r)=(Obj)(b),(r)))
#define MKDAlg(dp,den,r) (!(dp)?(void *)((r)=0):(void *)(NEWDAlg(r),(r)->nm = (dp),(r)->dn=(den)))

#define IMM_MAX 1073741823
#define IMM_MIN -1073741823

#define SL(n) ((n)->p)

#define ToReal(a) (!(a)?(double)0.0:REAL(a)?BDY((Real)a):RATN(a)?RatnToReal((Q)a):BIGFLOAT(a)?mpfrtodbl(BDY((BF)a)):0)

/* predicates */
#define NUM(p) (OID(p)==O_N)
#define RAT(p) (OID(p)==O_R)
#define RATN(a) (NID((Num)a)==N_Q)
#define POLY(a) (!(a) ||(OID(a)<=O_P))
#define INT(q) (!(q)||(NUM(q)&&NID((Num)q)==N_Q&&((Q)q)->z))
#define REAL(a) (NID((Num)a)==N_R)
#define BIGFLOAT(a) (NID((Num)a)==N_B)
#define SFF(a) (NID((Num)a)==N_M)
#define UNIZ(q) ((q)&&INT(q)&&mpz_cmp_si((BDY((Z)(q))),1)==0)
#define MUNIZ(q) ((q)&&INT(q)&&mpz_cmp_si((BDY((Z)(q))),-1)==0)
#define UNIMQ(q) ((q)&&NUM(q)&&SFF(q)&&(CONT((MQ)q)==1))
#define MUNIMQ(q) ((q)&&NUM(q)&&SFF(q)&&(CONT((MQ)q)==-1))
#define EVENZ(n) ((!(n))||(INT(n)&&mpz_tstbit(((Z)(n))->body,0)==0))
#define UNIMPZ(q) (mpz_cmp_si(q,1)==0)
#define MUNIMPZ(q) (mpz_cmp_si(q,-1)==0)

/* special macros for private memory management */

#define NV(p) ((p)->nv)
#define C(p) ((p)->c)
#if 0
#define ITOS(p) (((unsigned long)(p))&0x7fffffff)
#define STOI(i) ((P)((unsigned long)(i)|0x80000000))
#else
#define ITOS(p) (((unsigned long)(p)))
#define STOI(i) ((P)((unsigned long)(i)))
#endif

/* immediate GFS representation */

#define IFTOF(p) ((int)(((unsigned long)(p))&0x7fffffff))
#define FTOIF(i) ((int)(((unsigned long)(i)|0x80000000)))

struct cdl {
  Obj c;
  DL d;
};

struct cdlm {
  int c;
  DL d;
};

extern MP _mp_free_list;
extern DP _dp_free_list;       
extern DL _dl_free_list;       
extern int current_dl_length;      

#define _NEWDL_NOINIT(d,n) if ((n)!= current_dl_length){_dl_free_list=0; current_dl_length=(n);} if(!_dl_free_list)_DL_alloc(); (d)=_dl_free_list; _dl_free_list = *((DL *)_dl_free_list)
#define _NEWDL(d,n) if ((n)!= current_dl_length){_dl_free_list=0; current_dl_length=(n);} if(!_dl_free_list)_DL_alloc(); (d)=_dl_free_list; _dl_free_list = *((DL *)_dl_free_list); bzero((d),(((n)+1)*sizeof(int)))
#define _NEWMP(m) if(!_mp_free_list)_MP_alloc(); (m)=_mp_free_list; _mp_free_list = NEXT(_mp_free_list)
#define _MKDP(n,m,d) if(!_dp_free_list)_DP_alloc(); (d)=_dp_free_list; _dp_free_list = (DP)BDY(_dp_free_list); (d)->id = O_DP; (d)->nv=(n); BDY(d)=(m)

#define _NEXTMP(r,c) \
if(!(r)){_NEWMP(r);(c)=(r);}else{_NEWMP(NEXT(c));(c)=NEXT(c);}

#define _NEXTMP2(r,c,s) \
if(!(r)){(c)=(r)=(s);}else{NEXT(c)=(s);(c)=(s);}

#define _FREEDL(m) *((DL *)m)=_dl_free_list; _dl_free_list=(m)
#define _FREEMP(m) NEXT(m)=_mp_free_list; _mp_free_list=(m)
#define _FREEDP(m) BDY(m)=(MP)_dp_free_list; _dp_free_list=(m)

#define MUL_WEIGHT(a,i) (current_dl_weight_vector?(a)*current_dl_weight_vector[i]:(a))

/* externals */
#if 0
double NatToReal();
double RatnToReal();
#endif

extern struct oR oUNIR;
extern struct oZ oUNIZ;
extern struct oMQ oUNIMQ;
extern struct oN oUNIN;
extern struct oUP2 oONEUP2;
extern struct oV oVAR[];
extern struct oV oPVAR[];
extern struct oVL oVLIST[];
extern struct oVL oPVLIST[];
extern VL CO,ALG,LASTCO;
extern VL PVL;
extern R ONER;
extern Z ONE;
extern Z TWO;
extern MQ ONEM;
extern LM ONELM;
extern UP2 ONEUP2;
extern mpz_t ONEMPZ;

extern FILE *asir_out;
#if defined(__GNUC__)
extern const int sprime[];
#else
extern int sprime[];
#endif

typedef void (*NFUNC)(Num,Num,Num *);
typedef void (*NFUNC2)(Num,Num *);
typedef int (*NFUNCI)(Num,Num);

extern NFUNC addnumt[], subnumt[], mulnumt[], divnumt[], pwrnumt[];
extern NFUNCI cmpnumt[];
extern NFUNC2 chsgnnumt[];

extern int current_mod;
extern GEN_UP2 current_mod_gf2n;
extern int lm_lazy;
extern int current_ff;
extern V up_var;
extern V up2_var;
extern P current_gfs_ext;
extern int current_gfs_p;
extern int current_gfs_q;
extern int current_gfs_q1;
extern int *current_gfs_plus1;
extern int *current_gfs_ntoi;
extern int *current_gfs_iton;

extern int *current_dl_weight_vector;

/* prototypes */
int compui(VL,USINT,USINT);
int compbytearray(VL,BYTEARRAY,BYTEARRAY);

void powermodup(UP,UP *);
void hybrid_powermodup(UP,UP *);

void powertabup(UP,UP,UP *);
void hybrid_powertabup(UP,UP,UP *);

void powermod1up(UP,UP *);
void hybrid_rembymulup_special(int,UP,UP,UP,UP *);
void hybrid_tmulup(int,UP,UP,int,UP *);
void hybrid_squareup(int,UP,UP *);
void hybrid_mulup(int,UP,UP,UP *);

int cmpdl_composite(int,DL,DL);
int cmpdl_matrix(int,DL,DL);
int cmpdl_order_pair(int,DL,DL);
int cmpdl_elim(int,DL,DL);
int cmpdl_blexrev(int,DL,DL);
int cmpdl_bgradrev(int,DL,DL);
int cmpdl_brevrev(int,DL,DL);
int cmpdl_brevgradlex(int,DL,DL);
int cmpdl_bgradlex(int,DL,DL);
int cmpdl_blex(int,DL,DL);
int cmpdl_revgradlex(int,DL,DL);
int cmpdl_gradlex(int,DL,DL);
int cmpdl_revlex(int,DL,DL);
int cmpdl_lex(int,DL,DL);

void adddv(VL,DPV,DPV,DPV *);
void subdv(VL,DPV,DPV,DPV *);
void muldv(VL,DP,DPV,DPV *);
void chsgndv(DPV,DPV *);
int compdv(VL,DPV,DPV);

void adddpm(VL,DPM,DPM,DPM *);
void subdpm(VL,DPM,DPM,DPM *);
void mulobjdpm(VL,Obj,DPM,DPM *);
void chsgndpm(DPM,DPM *);
int compdpm(VL,DPM,DPM);

void _printdp(DP);
void _dp_sp_mod(DP,DP,int,DP *);
void _dp_mod(DP,int,NODE,DP *);
void _dp_red_mod(DP,DP,int,DP *);
void _dtop_mod(VL,VL,DP,P *);
void _mulmdm(VL,int,DP,MP,DP *);
void _mulmd(VL,int,DP,DP,DP *);
void _chsgnmd(int,DP,DP *);
void _submd(VL,int,DP,DP,DP *);
void _addmd(VL,int,DP,DP,DP *);
void _mdtop(VL,int,VL,DP,P *);
void divsmdc(VL,int,DP,P,DP *);
void mulmdc(VL,int,DP,P,DP *);
void mulmdm(VL,int,DP,MP,DP *);
void mulmd(VL,int,DP,DP,DP *);
void chsgnmd(int,DP,DP *);
void submd(VL,int,DP,DP,DP *);
void addmd(VL,int,DP,DP,DP *);
void mdtop(VL,int,VL,DP,P *);
void mptomd(VL,int,VL,P,DP *);
void ptomd(VL,int,VL,P,DP *);
int p_mag(P);
int z_bits(Q);
void rtime_init(void);
void setmod_gf2n(P);
void mt_sgenrand(unsigned long);
unsigned long mt_genrand(void);
#if defined(VISUAL) || defined(__MINGW32__)
void srandom(unsigned int);
unsigned int random(void);
#endif
void GC_free(void *);
void FFT_primes(int,int *,int *,int *);
int FFT_pol_product(unsigned int,unsigned int *, unsigned int,unsigned int *,
  unsigned int *,int,unsigned int *);
int FFT_pol_square(unsigned int,unsigned int *,
  unsigned int *,int,unsigned int *);
void dcptolist(DCP,LIST *);
void gcdprsmp(VL,int,P,P,P *);
void mult_mod_tab(UM,int,UM *,UM,int);
int nfctr_mod(UM,int);
int irred_check(UM,int);
void modfctrp(P,int,int,DCP *);
void pf_init(void);
void ptolmp(P,P *);
void lmptop(P,P *);
void ulmptoum(int,UP,UM);
void objtobobj(int,Obj,Obj *);
void bobjtoobj(int,Obj,Obj *);
void numtobnum(int,Num,Num *);
void bnumtonum(int,Num,Num *);
void ptobp(int,P,P *);
void bptop(int,P,P *);
void listtoblist(int,LIST,LIST *);
void blisttolist(int,LIST,LIST *);
void vecttobvect(int,VECT,VECT *);
void bvecttovect(int,VECT,VECT *);
void mattobmat(int,MAT,MAT *);
void bmattomat(int,MAT,MAT *);
void kmulum(int,UM,UM,UM);
void saveobj(FILE *,Obj);
void endian_init(void);
void write_char(FILE *,unsigned char *);
void write_short(FILE *,unsigned short *);
void write_int(FILE *,unsigned int *);
void write_double(FILE *,double *);
void write_intarray(FILE *,unsigned int *,int);
void write_string(FILE *,unsigned char *,int);
void savestr(FILE *,char *);
void loadstr(FILE *,char **);
void savevl(FILE *,VL);
void loadvl(FILE *);
void skipvl(FILE *);
void savev(FILE *,V);
void loadv(FILE *,V *);
int save_convv(V);
V load_convv(int);
void swap_bytes(char *,int,int);
void read_char(FILE *,unsigned char *);
void read_short(FILE *,unsigned short *);
void read_int(FILE *,unsigned int *);
void read_double(FILE *,double *);
void read_intarray(FILE *,unsigned int *,int);
void read_string(FILE *,unsigned char *,int);
void loadobj(FILE *,Obj *);
void invum(int,UM,UM,UM);
void addarray_to(unsigned int *,int,unsigned int *,int);
void muln_1(unsigned int *,int,unsigned int,unsigned int *);
unsigned int divn_1(unsigned int *,int,unsigned int,unsigned int *);
void ptoup(P,UP *);
void uptop(UP,P *);
void printnum(Num);
void printv(VL,V);
void kmulq(Q,Q,Q *);
void simplm(LM,LM *);
void qtolm(Q,LM *);
int qpcheck(Obj);
int headsgn(P);
void adjsgn(P,DCP);
void setmod_g2n(P);
void simpgf2n(GF2N,GF2N *);
void ptogf2n(Obj,GF2N *);
void gf2ntop(GF2N,P *);
void gf2ntovect(GF2N,VECT *);
void squaregf2n(GF2N,GF2N *);
void randomgf2n(GF2N *);
void invgf2n(GF2N,GF2N *);
int qcoefp(Obj);
int qcoefr(Obj);
size_t get_allocwords(void);
double get_clock(void);
void get_eg(struct oEGT *);
void printtime(struct oEGT *,struct oEGT *,double);
void init_eg(struct oEGT *);
void add_eg(struct oEGT *,struct oEGT *,struct oEGT *);
void print_eg(char *,struct oEGT *);
void print_split_eg(struct oEGT *,struct oEGT *);
void print_split_e(struct oEGT *,struct oEGT *);
void suspend_timer(void);
void resume_timer(void);
void reset_engine(void);
void error(char *);
void ptoup2(P,UP2 *);
void ptoup2_sparse(P,UP2 *);
void up2top(UP2,P *);
void up2tovect(UP2,VECT *);
void up2toz(UP2,Z *);
void ztoup2(Z,UP2 *);
void gen_simpup2(UP2,GEN_UP2,UP2 *);
void gen_simpup2_destructive(UP2,GEN_UP2);
void gen_invup2(UP2,GEN_UP2,UP2 *);
void simpup2(UP2,UP2,UP2 *);
int degup2(UP2);
int degup2_sparse(UP2);
int degup2_1(unsigned int);
void addup2(UP2,UP2,UP2 *);
void subup2(UP2,UP2,UP2 *);
void mulup2_n1(unsigned int *,int,unsigned int,unsigned int *);
void mulup2_nh(unsigned int *,int,unsigned int,unsigned int *);
void _mulup2_1(UP2,unsigned int,UP2);
void _mulup2_h(UP2,unsigned int,UP2);
void mulup2(UP2,UP2,UP2 *);
void _kmulup2_(unsigned int *,unsigned int *,int,unsigned int *);
void _mulup2_nn(unsigned int *,unsigned int *,int,unsigned int *);
void _mulup2(UP2,UP2,UP2);
void _mulup2_(_UP2,_UP2,_UP2);
void squareup2(UP2,UP2 *);
void _adjup2(UP2);
void _adjup2_(_UP2);
void _addup2(UP2,UP2,UP2);
void _addup2_destructive(UP2,UP2);
void _addup2_(_UP2,_UP2,_UP2);
void _addtoup2_(_UP2,_UP2);
unsigned int mulup2_bb(unsigned int,unsigned int);
void init_up2_tab(void);
unsigned int quoup2_11(unsigned int,unsigned int);
void divup2_1(unsigned int,unsigned int,int,int,unsigned int *,unsigned int *);
void qrup2(UP2,UP2,UP2 *,UP2 *);
void _qrup2(UP2,UP2,UP2,UP2);
void remup2(UP2,UP2,UP2 *);
void _remup2(UP2,UP2,UP2);
void remup2_sparse(UP2,UP2,UP2 *);
void remup2_sparse_destructive(UP2,UP2);
void remup2_type1_destructive(UP2,int);
void remup2_3_destructive(UP2,UP2);
void remup2_5_destructive(UP2,UP2);
void _invup2_1(unsigned int,unsigned int,unsigned int *,unsigned int *);
void _gcdup2_1(unsigned int,unsigned int,unsigned int *);
void up2_init_eg(void);
void up2_show_eg(void);
void invup2(UP2,UP2,UP2 *);
void gcdup2(UP2,UP2,UP2 *);
void chsgnup2(UP2,UP2 *);
void pwrmodup2(UP2,Z,UP2,UP2 *);
void pwrmodup2_sparse(UP2,Z,UP2,UP2 *);
int compup2(UP2,UP2);
void printup2(UP2);
void _copyup2(UP2,UP2);
void _bshiftup2(UP2,int,UP2);
void _bshiftup2_destructive(UP2,int);
void diffup2(UP2,UP2 *);
int sqfrcheckup2(UP2);
int irredcheckup2(UP2);
int irredcheck_dddup2(UP2);
void _copy_up2bits(UP2,unsigned int **,int);
void _print_frobmat(unsigned int **,int,int);
int compute_multiplication_matrix(P,GF2MAT *);
void compute_change_of_basis_matrix(P,P,int,GF2MAT *,GF2MAT *);
int compute_representation_conversion_matrix(P,GF2MAT *,GF2MAT *);
void mul_nb(GF2MAT,unsigned int *,unsigned int *,unsigned int *);
void leftshift(unsigned int *,int);
void mat_to_gf2mat(MAT,unsigned int ***);
void gf2mat_to_mat(unsigned int **,int,MAT *);
void mulgf2mat(int,unsigned int **,unsigned int **,unsigned int **);
void mulgf2vectmat(int,unsigned int *,unsigned int **,unsigned int *);
int mulgf2vectvect(int,unsigned int *,unsigned int *);
int invgf2mat(int,unsigned int **,unsigned int **);
void _mulup2_11(unsigned int,unsigned int,unsigned int *);
void _mulup2_22(unsigned int *,unsigned int *,unsigned int *);
void _mulup2_33(unsigned int *,unsigned int *,unsigned int *);
void _mulup2_44(unsigned int *,unsigned int *,unsigned int *);
void _mulup2_55(unsigned int *,unsigned int *,unsigned int *);
void _mulup2_66(unsigned int *,unsigned int *,unsigned int *);
void printup2_(unsigned int *,int);
void type1_bin_invup2(UP2,int,UP2 *);
int int_bits(int);


LUM LUMALLOC(int, int);
BM BMALLOC(int, int);
Obj ToAlg(Num);
UM *berlemain(register int, UM, UM *);
void Risa_GC_set_adj(int,int);
void Risa_GC_get_adj(int *,int *);
void *Risa_GC_malloc(size_t);
void *Risa_GC_malloc_atomic(size_t);
void *Risa_GC_realloc(void *,size_t);
void Risa_GC_free(void *);
void *Risa_GC_malloc_atomic_ignore_off_page(size_t);
void *GC_malloc(size_t);
void *GC_malloc_atomic(size_t);
void *GC_realloc(void *,size_t);
double RatnToReal(Q);
double pwrreal0(double,int);
double rtodbl(); /* XXX */
double mpfrtodbl(mpfr_t a);
int **almat(int,int);
pointer **almat_pointer(int,int);
int berlecnt(register int,UM);
int berlecntmain(register int,int,int,register int **);
int cmpalg(Num,Num);
int cmpbf(Num,Num);
int cmpcplx(Num,Num);
int cmpq(Q,Q);
int cmpreal(Real,Real);
int cmpmi(MQ,MQ);
int cmplm(LM,LM);
int compmat(VL,MAT,MAT);
int compnum(VL,Num,Num);
int compp(VL,P,P);
int compr(VL,Obj,Obj);
int equalp(VL,P,P);
int equalr(VL,Obj,Obj);
int compstr(VL,STRING,STRING);
int compvect(VL,VECT,VECT);
int ctest(P,ML,int,int *);
int cycchk(P);
int dbound(V,P);
int dcomp(P,P);
int deg(V,P);
int degtest(int,int *,ML,int);
int divcheck(VL,P *,int,P,P);
int divtdcpz(VL,P,P,P *);
int divtpz(VL,P,P,P *);
int divum(register int,UM,UM,UM);
unsigned int dm(unsigned int,unsigned int,unsigned int *);
unsigned int dmb(unsigned int,unsigned int,unsigned int,unsigned int *);
unsigned int dma(unsigned int,unsigned int,unsigned int,unsigned int *);
unsigned int dmab(unsigned int,unsigned int,unsigned int,unsigned int,unsigned int *);
unsigned int dmar(unsigned int,unsigned int,unsigned int,unsigned int);
int dtestmain(P,Q,ML,int,int *,P *,P *);
int geldb(VL,P);
int getchomdeg(V,P);
int getdeg(V,P);
int getlchomdeg(V,P,int *);
int homdeg(P);
unsigned int invm(unsigned int,int);
int iscycm(P);
int iscycp(P);
int lengthp(P);
int mig(int,int,P);
int mignotte(int,P);
int minimain(register int,int,int,register int **);
int ncombi(int,int,int,int *);
int nextbin(VN,int);
int nmonop(P);
int pcoef(VL,VL,P,P *);
int pcoef0(VL,VL,P,P *);
unsigned int pwrm(register int,register int,int);
int sqfrchk(P);
int ucmpp(P,P);
int valideval(VL,DCP,VN);
int zerovpchk(VL,P,VN);

void addgf2n(GF2N,GF2N,GF2N *);
void subgf2n(GF2N,GF2N,GF2N *);
void mulgf2n(GF2N,GF2N,GF2N *);
void divgf2n(GF2N,GF2N,GF2N *);
void chsgngf2n(GF2N,GF2N *);
void pwrgf2n(GF2N,Z, GF2N *);
int cmpgf2n(GF2N,GF2N);

void addgfpn(GFPN,GFPN,GFPN *);
void subgfpn(GFPN,GFPN,GFPN *);
void mulgfpn(GFPN,GFPN,GFPN *);
void divgfpn(GFPN,GFPN,GFPN *);
void chsgngfpn(GFPN,GFPN *);
void pwrgfpn(GFPN,Z, GFPN *);
int cmpgfpn(GFPN,GFPN);

void addgfs(GFS,GFS,GFS *);
void subgfs(GFS,GFS,GFS *);
void mulgfs(GFS,GFS,GFS *);
void divgfs(GFS,GFS,GFS *);
void chsgngfs(GFS,GFS *);
void pwrgfs(GFS,Z, GFS *);
int cmpgfs(GFS,GFS);

void addgfsn(GFSN,GFSN,GFSN *);
void subgfsn(GFSN,GFSN,GFSN *);
void mulgfsn(GFSN,GFSN,GFSN *);
void divgfsn(GFSN,GFSN,GFSN *);
void chsgngfsn(GFSN,GFSN *);
int cmpgfsn(GFSN,GFSN);

void adddalg(DAlg,DAlg,DAlg *);
void subdalg(DAlg,DAlg,DAlg *);
void muldalg(DAlg,DAlg,DAlg *);
void divdalg(DAlg,DAlg,DAlg *);
void chsgndalg(DAlg,DAlg *);
void pwrdalg(DAlg,Z, DAlg *);
int cmpdalg(DAlg,DAlg);

void addalg(Num,Num,Num *);
void addbf(Num,Num,Num *);
void addcplx(Num,Num,Num *);
void addmat(VL,MAT,MAT,MAT *);
void addmp(VL,int,P,P,P *);
void addmpq(int,P,P,P *);
void addmptoc(VL,int,P,P,P *);
void addmq(int,MQ,MQ,MQ *);
void addnum(VL,Num,Num,Num *);
void addp(VL,P,P,P *);
void addpadic(int,int,unsigned int *,unsigned int *);
void addpq(P,P,P *);
void addptoc(VL,P,P,P *);
void addr(VL,Obj,Obj,Obj *);
void addreal(Num,Num,Real *);
void addmi(MQ,MQ,MQ *);
void addlm(LM,LM,LM *);
void addstr(VL,STRING,STRING,STRING *);
void addum(int,UM,UM,UM);
void addvect(VL,VECT,VECT,VECT *);
void addquote(VL,QUOTE,QUOTE,QUOTE *);
void afctr(VL,P,P,DCP *);
void afctrmain(VL,P,P,int,DCP *);
void affine(VL,P,VN,P *);
void affinemain(VL,P,V,int,P *,P *);
void berle(int,int,P,ML *);
void cbound(VL,P,Q *);
void chnrem(int,V,P,Z,UM,P *,Z *);
void chnremp(VL,int,P,Z,P,P *);
void chsgnalg(Num,Num *);
void chsgnbf(Num a,Num *);
void chsgncplx(Num,Num *);
void chsgnmat(MAT,MAT *);
void chsgnmp(int,P,P *);
void chsgnnum(Num,Num *);
void chsgnp(P,P *);
void chsgnq(Q,Q *);
void chsgnr(Obj,Obj *);
void chsgnreal(Num,Num *);
void chsgnmi(MQ,MQ *);
void chsgnlm(LM,LM *);
void chsgnvect(VECT,VECT *);
void chsgnquote(QUOTE,QUOTE *);
void clctv(VL,P,VL *);
void clctvr(VL,Obj,VL *);
void cm2p(Z,Z,P,P *);
void cmax(P,Q *);
void cmp(Z,P,P *);
void coefp(P,int,P *);
void cpylum(int,LUM,LUM);
void cpyum(UM,UM);
void csump(VL,P,Q *);
void cycm(V,register int,DCP *);
void cycp(V,register int,DCP *);
void degp(V,P,Z *);
void degum(UM,int);
void detmp(VL,int,P **,int,P *);
void detp(VL,P **,int,P *);
void diffp(VL,P,V,P *);
void diffum(register int,UM,UM);
void divalg(Num,Num,Num *);
void divbf(Num,Num,Num *);
void divcp(P,Q,P *);
void divcplx(Num,Num,Num *);
void divmat(VL,Obj,Obj,Obj *);
void divmq(int,MQ,MQ,MQ *);
void divnum(VL,Num,Num,Num *);
void divr(VL,Obj,Obj,Obj *);
void divreal(Num,Num,Real *);
void divmi(MQ,MQ,MQ *);
void divlm(LM,LM,LM *);
void divsdcmp(VL,int,P,P,P *);
void divsdcp(VL,P,P,P *);
void divsmp(VL,int,P,P,P *);
void divsp(VL,P,P,P *);
void divsrdcmp(VL,int,P,P,P *,P *);
void divsrdcp(VL,P,P,P *,P *);
void divsrmp(VL,int,P,P,P *,P *);
void divsrp(VL,P,P,P *,P *);
void divvect(VL,Obj,Obj,Obj *);
void divquote(VL,QUOTE,QUOTE,QUOTE *);
void dtest(P,ML,int,DCP *);
void dtestroot(int,int,P,LUM,struct oDUM *,DCP *);
void dtestroot1(int,int,P,LUM,P *);
void dtestsq(int,int,P,LUM,P *);
void dtestsql(P,ML,struct oDUM *,DCP *);
void ediffp(VL,P,V,P *);
void eucum(register int,UM,UM,UM,UM);
void exthp(VL,P,int,P *);
void exthpc(VL,V,P,int,P *);
void ezgcd1p(VL,P,P,P *);
void ezgcdhensel(P,int,UM,UM,ML *);
void ezgcdnp(VL,P,P *,int,P *);
void ezgcdnpp(VL,DCP,P *,int,P *);
void ezgcdnpz(VL,P *,int,P *);
void ezgcdp(VL,P,P,P *);
void ezgcdpp(VL,DCP,P,P *);
void ezgcdpz(VL,P,P,P *);
void fctrp(VL,P,DCP *);
void fctrwithmvp(VL,P,V,DCP *);
void gcda(VL,P,P,P,P *);
void gcdcp(VL,P,P *);
void gcdgen(P,ML,ML *);
void gcdmonomial(VL,DCP *,int,P *);
void gcdprsp(VL,P,P,P *);
void gcdum(register int,UM,UM,UM);
void getmindeg(V,P,Z *);
void henmain(LUM,ML,ML,ML *);
void henprep(P,ML,ML,ML *,ML *);
void hensel(int,int,P,ML *);
void hsq(int,int,P,int *,DCP *);
void intersectv(VL,VL,VL *);
void invl(Q,Q,Q *);
void invmq(int,MQ,MQ *);
void invq(Q,Q *);
void lgp(P,Z *,Z *);
void lumtop(V,int,int,LUM,P *);
void lumtop_unsigned(V v,int mod,int bound,LUM f,P *g);
void markv(VN,int,P);
void maxdegp(VL,P,VL *,P *);
void mergev(VL,VL,VL,VL *);
void mfctr(VL,P,DCP *);
void mfctrhen2(VL,VN,P,P,P,P,P,P,P *);
void mfctrmain(VL,P,DCP *);
void mfctrwithmv(VL,P,V,DCP *);
void min_common_vars_in_coefp(VL,P,VL *,P *);
void minchdegp(VL,P,VL *,P *);
void mindegp(VL,P,VL *,P *);
void mini(register int,UM,UM);
void minlcdegp(VL,P,VL *,P *);
void mkbcm(int,int,MQ *);
void mkssum(V,int,int,int,P *);
void monomialfctr(VL,P,P *,DCP *);
void mptop(P,P *);
void mptoum(P,UM);
void msqfr(VL,P,DCP *);
void msqfrmain(VL,P,DCP *);
void msqfrmainmain(VL,P,VN,P,DCP,DCP *,P *);
void mulalg(Num,Num,Num *);
void mulbf(Num,Num,Num *);
void mulcplx(Num,Num,Num *);
void mullum(int,int,LUM,LUM,LUM);
void mullumarray(P,ML,int,int *,P *);
void mulmat(VL,Obj,Obj,Obj *);
void mulmatmat(VL,MAT,MAT,MAT *);
void mulmatvect(VL,MAT,VECT,VECT *);
void mulmp(VL,int,P,P,P *);
void mulmpc(VL,int,P,P,P *);
void mulmpq(int,P,P,P *);
void mulmq(int,MQ,MQ,MQ *);
void mulnum(VL,Num,Num,Num *);
void mulp(VL,P,P,P *);
void mulpadic(int,int,unsigned int *,unsigned int *,unsigned int *);
void mulpc(VL,P,P,P *);
void mulpq(P,P,P *);
void mulr(VL,Obj,Obj,Obj *);
void mulreal(Num,Num,Real *);
void mulmi(MQ,MQ,MQ *);
void mullm(LM,LM,LM *);
void mulrmat(VL,Obj,MAT,MAT *);
void mulrvect(VL,Obj,VECT,VECT *);
void mulsgn(VN,VN,int,VN);
void mulsum(register int,UM,register int,UM);
void mulum(register int,UM,UM,UM);
void mulvect(VL,Obj,Obj,Obj *);
void mulquote(VL,QUOTE,QUOTE,QUOTE *);
void mulvectmat(VL,VECT,MAT,VECT *);
void next(VN);
void nezgcdnp_sqfr_primitive(VL,P,P *,int,P *);
void nezgcdnpp(VL,DCP,P *,int,P *);
void nezgcdnpz(VL,P *,int,P *);
void nezgcdnpzmain(VL,P *,int,P *);
void nglob_init(void);
void norm(P,Q *);
void norm1(P,P *);
void norm1c(P,Q *);
void normalizemp(int,P);
void nthrootz(Z,int,Z *);
int ztonadic(int,Z,unsigned int **);
void nuezgcdnpzmain(VL,P *,int,P *);
void padictoq(int,int,int *,Z *);
void padictoq_unsigned(int,int,int *,Z *);
void risa_pari_init(void);
void pcp(VL,P,P *,P *);
void pderivr(VL,Obj,V,Obj *);
void pdiva(VL,P,P,P,P *);
void pinva(P,P,P *);
void plisttop(P *,V,int,P *);
void pmonic(VL,P,P,P *);
void pqra(VL,P,P,P,P *,P *);
void premmp(VL,int,P,P,P *);
void premp(VL,P,P,P *);
void ptolum(int,int,P,LUM);
void ptomp(int,P,P *);
void ptoum(int,P,UM);
void ptozp(P,int,Q *,P *);
void ptozp0(P,P *);
void pwralg(Num,Num,Num *);
void pwrbf(Num,Num,Num *);
void pwrcplx(Num,Num,Num *);
void pwrcplx0(Num,int,Num *);
void pwrlum(int,int,LUM,int,LUM);
void pwrmat(VL,MAT,Obj,MAT *);
void pwrmatmain(VL,MAT,int,MAT *);
void pwrmp(VL,int,P,Z,P *);
void pwrmq(int,MQ,Q,MQ *);
void pwrnum(VL,Num,Num,Num *);
void pwrp(VL,P,Z,P *);
void pwrq(Q,Q,Q *);
void pwrr(VL,Obj,Obj,Obj *);
void pwrquote(VL,QUOTE,QUOTE,QUOTE *);
void pwrreal(Num,Num,Real *);
void pwrmi(MQ,Q,MQ *);
void pwrum(int,UM,int,UM);
void asir_reallocarray(char **,int *,int *,int);
void reductr(VL,Obj,Obj *);
void reimtocplx(Num,Num,Num *);
void rema(VL,P,P,P,P *);
void remq(Q,Q,Q *);
void remsdcp(VL,P,P,P *);
void reordermp(VL,int,VL,P,P *);
void reorderp(VL,VL,P,P *);
void reordvar(VL,V,VL *);
void res_ch_det(VL,V,P,P,P *);
void res_detmp(VL,int,V,P,P,P *);
void restore(VL,P,VN,P *);
void resultmp(VL,int,V,P,P,P *);
void resultp(VL,V,P,P,P *);
void setlum(int,int,LUM);
void sort_by_deg(int,P *,P *);
void sort_by_deg_rev(int,P *,P *);
void sortfs(DCP *);
void sortfsrev(DCP *);
void sortplist(P *,int);
void sortplistbyhomdeg(P *,int);
void sprs(VL,V,P,P,P *);
void sqa(VL,P,P,DCP *);
void sqfrp(VL,P,DCP *);
void sqfrum(int,int,P,int *,struct oDUM **,ML *);
void sqfrummain(int,UM,UM,struct oDUM **);
void sqrtz(Z,Z *);
void srch2(VL,V,P,P,P *);
void srchmp(VL,int,V,P,P,P *);
void srchump(int,P,P,P *);
void srcr(VL,V,P,P,P *);
void strtobf(char *,BF *);
void subalg(Num,Num,Num *);
void subbf(Num,Num,Num *);
void subcplx(Num,Num,Num *);
void submat(VL,MAT,MAT,MAT *);
void submp(VL,int,P,P,P *);
void submq(int,MQ,MQ,MQ *);
void subnum(VL,Num,Num,Num *);
void subp(VL,P,P,P *);
void subr(VL,Obj,Obj,Obj *);
void subreal(Num,Num,Real *);
void submi(MQ,MQ,MQ *);
void sublm(LM,LM,LM *);
void substmp(VL,int,P,V,P,P *);
void substp(VL,P,V,P,P *);
void substvp(VL,P,VN,P *);
void subum(int,UM,UM,UM);
void subvect(VL,VECT,VECT,VECT *);
void subquote(VL,QUOTE,QUOTE,QUOTE *);
void toreim(Num,Num *,Num *);
void ucsump(P,Q *);
void uexgcdnp(VL,P,P *,int,VN,Q,P *,P *,P *,P *,Z *);
void uezgcd1p(P,P,P *);
void uezgcdpp(DCP,P,P *);
void uezgcdpz(VL,P,P,P *);
void ufctr(P,int,DCP *);
void ufctrmain(P,int,DCP *);
void umtomp(V,UM,P *);
void umtop(V,UM,P *);
void usqp(P,DCP *);
void vntovl(VN,int,VL *);

void saveerror(FILE *,ERR);
void saveui(FILE *,USINT);
void savedp(FILE *,DP);
void savedpm(FILE *,DPM);
void savestring(FILE *,STRING);
void savemat(FILE *,MAT);
void savevect(FILE *,VECT);
void savelist(FILE *,LIST);
void saver(FILE *,R);
void savep(FILE *,P);
void savegf2n(FILE *,GF2N);
void savegfpn(FILE *,GFPN);
void savegfs(FILE *,GFS);
void savedalg(FILE *,DAlg);
void savelm(FILE *,LM);
void savemi(FILE *,MQ);
void savecplx(FILE *,C);
void savebf(FILE *,BF);
void savereal(FILE *,Real);
void saveq(FILE *,Q);
void savenum(FILE *,Num);
void savepfins(FILE *,V);
void savegfmmat(FILE *,GFMMAT);
void savebytearray(FILE *,BYTEARRAY);
void savegfsn(FILE *,GFSN);

void loaderror(FILE *,ERR *);
void loadui(FILE *,USINT *);
void loaddp(FILE *,DP *);
void loaddpm(FILE *,DPM *);
void loadstring(FILE *,STRING *);
void loadmat(FILE *,MAT *);
void loadvect(FILE *,VECT *);
void loadlist(FILE *,LIST *);
void loadr(FILE *,R *);
void loadp(FILE *,P *);
void loadgf2n(FILE *,GF2N *);
void loadgfpn(FILE *,GFPN *);
void loadgfs(FILE *,GFS *);
void loadgfsn(FILE *,GFSN *);
void loaddalg(FILE *,DAlg *);
void loadlm(FILE *,LM *);
void loadmi(FILE *,MQ *);
void loadcplx(FILE *,C *);
void loadbf(FILE *,BF *);
void loadreal(FILE *,Real *);
void loadq(FILE *,Q *);
void loadnum(FILE *,Num *);
void loadgfmmat(FILE *,GFMMAT *);
void loadbytearray(FILE *,BYTEARRAY *);
V loadpfins(FILE *);
void eucum2(int mod,UM f1,UM f2,UM a,UM b);
void clearum(UM p,int n);
BM BMALLOC(int dx,int dy);
int isequalum(UM f1,UM f2);
void dec_um(int p,int a,UM u);
void setmod_sf(int p,int n);
int search_defpoly_and_primitive_root(int p,int n,UM dp);
void generate_defpoly_um(int p,int n,UM dp);
int generate_primitive_root_enc(int p,int n,UM dp);
int mulremum_enc(int p,int n,UM dp,int a,int b);
void gfs_galois_action(GFS a,Q e,GFS *c);
void gfs_embed(GFS z,int k,int pm,GFS *c);
void qtogfs(Q a,GFS *c);
void mqtogfs(MQ a,GFS *c);
void gfstomq(GFS a,MQ *c);
void ntogfs(Obj a,GFS *b);
void addgfs(GFS a,GFS b,GFS *c);
void subgfs(GFS a,GFS b,GFS *c);
void mulgfs(GFS a,GFS b,GFS *c);
void divgfs(GFS a,GFS b,GFS *c);
void chsgngfs(GFS a,GFS *c);
int cmpgfs(GFS a,GFS b);
void randomgfs(GFS *r);
int _addsf(int a,int b);
int _chsgnsf(int a);
int _subsf(int a,int b);
int _mulsf(int a,int b);
int _invsf(int a);
int _divsf(int a,int b);
int _pwrsf(int a,int b);
int _onesf();
int _itosf(int n);
int _isonesf(int a);
int _randomsf();
int field_order_sf();
int characteristic_sf();
int extdeg_sf();
void addsfum(UM p1,UM p2,UM pr);
void subsfum(UM p1,UM p2,UM pr);
void gcdsfum(UM p1,UM p2,UM pr);
void mulsfum(UM p1,UM p2,UM pr);
void mulssfum(UM p,int n,UM pr);
void kmulsfum(UM n1,UM n2,UM nr);
void kmulsfummain(UM n1,UM n2,UM nr);
int divsfum(UM p1,UM p2,UM pq);
void diffsfum(UM f,UM fd);
void monicsfum(UM f);
int compsfum(UM a,UM b);
void mulsfbm(BM f1,BM f2,BM fr);
int degbm(BM f);
void addtosfbm(BM f,BM g);
void eucsfum(UM f1,UM f2,UM a,UM b);
void shiftsfum(UM f,int a,UM g);
void shiftsflum(int n,LUM f,int ev);
void shiftsfbm(BM f,int a);
void clearbm(int n,BM f);

void ksquareum(int mod,UM n1,UM nr);
void extractum(UM n,int index,int len,UM nr);
void copyum(UM n1,UM n2);
void c_copyum(UM n,int len,int *p);
void kmulummain(int mod,UM n1,UM n2,UM nr);
void ksquareummain(int mod,UM n1,UM nr);

void simp_ff(Obj p,Obj *rp);
void field_order_ff(Z *order);

void random_lm(LM *r);
void setmod_lm(Z p);
void getmod_lm(Z *p);

void qtolm(Q q,LM *l);
void addlm(LM a,LM b,LM *c);
void sublm(LM a,LM b,LM *c);
void mullm(LM a,LM b,LM *c);
void divlm(LM a,LM b,LM *c);
void chsgnlm(LM a,LM *c);
void pwrlm(LM a,Z b,LM *c);
int cmplm(LM a,LM b);
void simplm(LM a,LM *b);
void simplm_force(LM a,LM *b);

void addlf(Z a,Z b,Z *c);
void sublf(Z a,Z b,Z *c);
void mullf(Z a,Z b,Z *c);
void divlf(Z a,Z b,Z *c);
void chsgnlf(Z a,Z *c);
int cmplf(Z a,Z b);
void simplf(Z a,Z *b);
void simplf_force(Z a,Z *b);
void setmod_lf(Z p);

void setmod_gfpn(P p);
void getmod_gfpn(UP *up);
void ptogfpn(Obj q,GFPN *l);
void gfpntop(GFPN q,P *l);
void simpgfpn(GFPN n,GFPN *r);
void ntogfpn(Obj a,GFPN *b);
void addgfpn(GFPN a,GFPN b,GFPN *c);
void subgfpn(GFPN a,GFPN b,GFPN *c);
void mulgfpn(GFPN a,GFPN b,GFPN *c);
void squaregfpn(GFPN a,GFPN *c);
void divgfpn(GFPN a,GFPN b,GFPN *c);
void invgfpn(GFPN b,GFPN *c);
void chsgngfpn(GFPN a,GFPN *c);
int cmpgfpn(GFPN a,GFPN b);
void randomgfpn(GFPN *r);
void setmod_gfsn(UM p);
void getmod_gfsn(UM *up);
void simpgfsn(GFSN n,GFSN *r);
void ntogfsn(Obj a,GFSN *b);
void addgfsn(GFSN a,GFSN b,GFSN *c);
void subgfsn(GFSN a,GFSN b,GFSN *c);
void mulgfsn(GFSN a,GFSN b,GFSN *c);
void divgfsn(GFSN a,GFSN b,GFSN *c);
void invgfsn(GFSN b,GFSN *c);
void chsgngfsn(GFSN a,GFSN *c);
void pwrgfsn(GFSN a,Z b,GFSN *c);
int cmpgfsn(GFSN a,GFSN b);
void randomgfsn(GFSN *r);
void setmod_gf2n(P p);
void getmod_gf2n(UP2 *p);
void simpgf2n(GF2N n,GF2N *r);
void ptogf2n(Obj q,GF2N *l);
void gf2ntop(GF2N q,P *l);
void gf2ntovect(GF2N q,VECT *l);
void addgf2n(GF2N a,GF2N b,GF2N *c);
void subgf2n(GF2N a,GF2N b,GF2N *c);
void mulgf2n(GF2N a,GF2N b,GF2N *c);
void squaregf2n(GF2N a,GF2N *c);
void divgf2n(GF2N a,GF2N b,GF2N *c);
void invgf2n(GF2N b,GF2N *c);
void chsgngf2n(GF2N a,GF2N *c);
int cmpgf2n(GF2N a,GF2N b);
void randomgf2n(GF2N *r);
void nadictoz(int base,int d,unsigned int *w,Z *nrp);
void intarraytoz(int len,unsigned int *w,Z *nrp);
void ptomp(int m,P p,P *pr);
void mptop(P f,P *gp);
void ptosfp(P p,P *pr);
void sfptop(P f,P *gp);
void sf_galois_action(P p,Q e,P *pr);
void sf_embed(P p,int k,int pm,P *pr);
void ptolmp(P p,P *pr);
void lmptop(P f,P *gp);
void ptoum(int m,P f,UM wf);
void umtop(V v,UM w,P *f);
void ptosfum(P f,UM wf);
void sfumtop(V v,UM w,P *f);
void ptoup(P n,UP *nr);
void uptop(UP n,P *nr);
void ulmptoum(int m,UP f,UM wf);
void objtobobj(int base,Obj p,Obj *rp);
void bobjtoobj(int base,Obj p,Obj *rp);
void numtobnum(int base,Num p,Num *rp);
void bnumtonum(int base,Num p,Num *rp);
void ptobp(int base,P p,P *rp);
void bptop(int base,P p,P *rp);
void listtoblist(int base,LIST p,LIST *rp);
void blisttolist(int base,LIST p,LIST *rp);
void vecttobvect(int base,VECT p,VECT *rp);
void bvecttovect(int base,VECT p,VECT *rp);
void mattobmat(int base,MAT p,MAT *rp);
void bmattomat(int base,MAT p,MAT *rp);
void mptoum(P p,UM pr);
void umtomp(V v,UM p,P *pr);
void enc_to_p(int p,int a,V v,P *pr);
int comp_dum(DUM a,DUM b);
void fctrsf(P p,DCP *dcp);
void gensqfrsfum(UM p,struct oDUM *dc);
void randsfum(int d,UM p);
void pwrmodsfum(UM p,int e,UM f,UM pr);
void spwrsfum(UM m,UM f,Z e,UM r);
void tracemodsfum(UM m,UM f,int e,UM r);
void make_qmatsf(UM p,UM *tab,int ***mp);
void nullsf(int **mat,int n,int *ind);
void null_to_solsf(int **mat,int *ind,int n,UM *r);
void czsfum(UM f,UM *r);
int berlekampsf(UM p,int df,UM *tab,UM *r);
void minipolysf(UM f,UM p,UM mp);
int find_rootsf(UM p,int *root);
void sqfrsf(VL vl,P f, DCP *dcp);
void canzassf(UM f,int d,UM *r);
void sfhensel(int count,P f,V x,V y,int degbound,GFS *evp,P *sfp,ML *listp);
int sfberle(V x,V y,P f,int count,GFS *ev,DCP *dcp);
void sfgcdgen(P f,ML blist,ML *clistp);
void sfhenmain2(BM f,UM g0,UM h0,int dy,BM *gp);
void ptosfbm(int dy,P f,BM fl);
void sfbmtop(BM f,V x,V y,P *fp);
void sfsqfr(P f,DCP *dcp);
void sfusqfr(P f,DCP *dcp);
void sfbsqfr(P f,V x,V y,DCP *dcp);
void sfbfctr(P f,V x,V y,int degbound,DCP *dcp);
void sfdtest(P f,ML list,V x,V y,DCP *dcp);
int sfdtestmain(VL vl,P lcg,UM lcg0,BM lcy,P csum,ML list,
  int k,int *in,P *fp,P *cofp);
void const_term(P f,UM c);
void const_term_sfbm(BM f,UM c);
int sfctest(UM lcg0,BM lcy,ML list,int k,int *in);
void mulsfbmarray(int dx,BM lcy,ML list,int k,int *in,V x,V y,P *g);
void sfcsump(VL vl,P f,P *s);
void cont_pp_sfp(VL vl,P f,P *cp,P *fp);
int divtp_by_sfbm(VL vl,P f,P g,P *qp);
void generate_defpoly_sfum(int n,UM *dp);
NODE symb_merge(NODE,NODE,int);
void _free_private_storage();
void _DL_alloc();
void _MP_alloc();
void _DP_alloc();
void _addmd_destructive(int mod,DP p1,DP p2,DP *pr);
void _mulmd_dup(int mod,DP p1,DP p2,DP *pr);
void _comm_mulmd_dup(int mod,DP p1,DP p2,DP *pr);
void _weyl_mulmd_dup(int mod,DP p1,DP p2,DP *pr);
void _mulmdm_dup(int mod,DP p,MP m0,DP *pr);
void _weyl_mulmdm_dup(int mod,MP m0,DP p,DP *pr);
void _weyl_mulmmm_dup(int mod,MP m0,MP m1,int n,struct cdlm *rtab,int rtablen);
void _comm_mulmd_tab(int mod,int nv,struct cdlm *t,int n,struct cdlm *t1,int n1,struct cdlm *rt);
void _comm_mulmd_tab_destructive(int mod,int nv,struct cdlm *t,int n,struct cdlm *t1,int n1);
void dlto_dl(DL d,DL *dr);
void _dltodl(DL d,DL *dr);
void _adddl_dup(int n,DL d1,DL d2,DL *dr);
void _free_dlarray(DL *a,int n);
void _free_dp(DP f);
void dpto_dp(DP p,DP *r);
void _dptodp(DP p,DP *r);
NODE _symb_merge(NODE m1,NODE m2,int n);
void _addd_destructive(VL vl,DP p1,DP p2,DP *pr);
void _muld_dup(VL vl,DP p1,DP p2,DP *pr);
void _comm_muld_dup(VL vl,DP p1,DP p2,DP *pr);
void _weyl_muld_dup(VL vl,DP p1,DP p2,DP *pr);
void _muldm_dup(VL vl,DP p,MP m0,DP *pr);
void _weyl_muldm_dup(VL vl,MP m0,DP p,DP *pr);
void _weyl_mulmm_dup(VL vl,MP m0,MP m1,int n,struct cdl *rtab,int rtablen);
void _comm_muld_tab(VL vl,int nv,struct cdl *t,int n,struct cdl *t1,int n1,struct cdl *rt);
void _comm_muld_tab_destructive(VL vl,int nv,struct cdl *t,int n,struct cdl *t1,int n1);

int has_fcoef(DP f);
int has_fcoef_p(P f);
void initd(struct order_spec *spec);
void ptod(VL vl,VL dvl,P p,DP *pr);
void dtop(VL vl,VL dvl,DP p,Obj *pr);
void nodetod(NODE node,DP *dp);
void nodetodpm(NODE node,Obj pos,DPM *dp);
int sugard(MP m);
void addd(VL vl,DP p1,DP p2,DP *pr);
void symb_addd(DP p1,DP p2,DP *pr);
NODE symb_merge(NODE m1,NODE m2,int n);
DLBUCKET symb_merge_bucket(DLBUCKET m1,DLBUCKET m2,int n);
void subd(VL vl,DP p1,DP p2,DP *pr);
void chsgnd(DP p,DP *pr);
void muld(VL vl,DP p1,DP p2,DP *pr);
void comm_muld(VL vl,DP p1,DP p2,DP *pr);
void muldm(VL vl,DP p,MP m0,DP *pr);
void weyl_muld(VL vl,DP p1,DP p2,DP *pr);
void weyl_muldm(VL vl,MP m0,DP p,DP *pr);
void weyl_mulmm(VL vl,MP m0,MP m1,int n,struct cdl *rtab,int rtablen);
void comm_muld_tab(VL vl,int nv,struct cdl *t,int n,struct cdl *t1,int n1,struct cdl *rt);
void muldc(VL vl,DP p,Obj c,DP *pr);
void divsdc(VL vl,DP p,P c,DP *pr);
void divdc(VL vl,DP p,Obj c,DP *pr);
void adddl(int n,DL d1,DL d2,DL *dr);
void adddl_destructive(int n,DL d1,DL d2);
int compd(VL vl,DP p1,DP p2);
int cmpdl_lex(int n,DL d1,DL d2);
int cmpdl_revlex(int n,DL d1,DL d2);
int cmpdl_gradlex(int n,DL d1,DL d2);
int cmpdl_revgradlex(int n,DL d1,DL d2);
int cmpdl_blex(int n,DL d1,DL d2);
int cmpdl_bgradlex(int n,DL d1,DL d2);
int cmpdl_brevgradlex(int n,DL d1,DL d2);
int cmpdl_brevrev(int n,DL d1,DL d2);
int cmpdl_bgradrev(int n,DL d1,DL d2);
int cmpdl_blexrev(int n,DL d1,DL d2);
int cmpdl_elim(int n,DL d1,DL d2);
int cmpdl_weyl_elim(int n,DL d1,DL d2);
int cmpdl_homo_ww_drl(int n,DL d1,DL d2);
int cmpdl_order_pair(int n,DL d1,DL d2);
int cmpdl_matrix(int n,DL d1,DL d2);
void ptomd(VL vl,int mod,VL dvl,P p,DP *pr);
void mptomd(VL vl,int mod,VL dvl,P p,DP *pr);
void mdtop(VL vl,int mod,VL dvl,DP p,P *pr);
void addmd(VL vl,int mod,DP p1,DP p2,DP *pr);
void submd(VL vl,int mod,DP p1,DP p2,DP *pr);
void chsgnmd(int mod,DP p,DP *pr);
void mulmd(VL vl,int mod,DP p1,DP p2,DP *pr);
void comm_mulmd(VL vl,int mod,DP p1,DP p2,DP *pr);
void weyl_mulmd(VL vl,int mod,DP p1,DP p2,DP *pr);
void mulmdm(VL vl,int mod,DP p,MP m0,DP *pr);
void weyl_mulmdm(VL vl,int mod,DP p,MP m0,DP *pr);
void weyl_mulmmm(VL vl,int mod,MP m0,MP m1,int n,DP *pr);
void mulmdc(VL vl,int mod,DP p,P c,DP *pr);
void divsmdc(VL vl,int mod,DP p,P c,DP *pr);
void _dtop_mod(VL vl,VL dvl,DP p,P *pr);
void _dp_mod(DP p,int mod,NODE subst,DP *rp);
void _dp_monic(DP p,int mod,DP *rp);
void _printdp(DP d);
void addmd_destructive(int mod,DP p1,DP p2,DP *pr);
void mulmd_dup(int mod,DP p1,DP p2,DP *pr);
void comm_mulmd_dup(int mod,DP p1,DP p2,DP *pr);
void weyl_mulmd_dup(int mod,DP p1,DP p2,DP *pr);
void mulmdm_dup(int mod,DP p,MP m0,DP *pr);
void weyl_mulmdm_dup(int mod,MP m0,DP p,DP *pr);
void weyl_mulmmm_dup(int mod,MP m0,MP m1,int n,struct cdlm *rtab,int rtablen);
void comm_mulmd_tab_destructive(int mod,int nv,struct cdlm *t,int n,struct cdlm *t1,int n1);
void adddl_dup(int n,DL d1,DL d2,DL *dr);
void monicup(UP a,UP *b);
void simpup(UP a,UP *b);
void simpnum(Num a,Num *b);
void uremp(P p1,P p2,P *rp);
void ugcdp(P p1,P p2,P *rp);
void reversep(P p1,Q d,P *rp);
void invmodp(P p1,Q d,P *rp);
void addup(UP n1,UP n2,UP *nr);
void subup(UP n1,UP n2,UP *nr);
void chsgnup(UP n1,UP *nr);
void hybrid_mulup(int ff,UP n1,UP n2,UP *nr);
void hybrid_squareup(int ff,UP n1,UP *nr);
void hybrid_tmulup(int ff,UP n1,UP n2,int d,UP *nr);
void mulup(UP n1,UP n2,UP *nr);
void mulcup(Num c,UP n1,UP *nr);
void tmulup(UP n1,UP n2,int d,UP *nr);
void squareup(UP n1,UP *nr);
void remup(UP n1,UP n2,UP *nr);
void remup_destructive(UP n1,UP n2);
void qrup(UP n1,UP n2,UP *nq,UP *nr);
void qrup_destructive(UP n1,UP n2);
void gcdup(UP n1,UP n2,UP *nr);
void extended_gcdup(UP a,UP m,UP *r);
void reverseup(UP n1,int d,UP *nr);
void invmodup(UP n1,int d,UP *nr);
void pwrup(UP n,Z e,UP *nr);
int compup(UP n1,UP n2);
void kmulp(VL vl,P n1,P n2,P *nr);
void ksquarep(VL vl,P n1,P *nr);
void kmulup(UP n1,UP n2,UP *nr);
void ksquareup(UP n1,UP *nr);
void copyup(UP n1,UP n2);
void c_copyup(UP n,int len,pointer *p);
void kmulupmain(UP n1,UP n2,UP *nr);
void ksquareupmain(UP n1,UP *nr);
void rembymulup(UP n1,UP n2,UP *nr);
void hybrid_rembymulup_special(int ff,UP n1,UP n2,UP inv2,UP *nr);
void rembymulup_special(UP n1,UP n2,UP inv2,UP *nr);
void tkmulup(UP n1,UP n2,int d,UP *nr);
void shiftup(UP n,int d,UP *nr);
void fft_rembymulup_special(UP n1,UP n2,UP inv2,UP *nr);
void set_degreeup(UP n,int d);
void decompup(UP n,int d,UP *n0,UP *n1);
void truncup(UP n1,int d,UP *nr);
int int_bits(int t);
int maxblenup(UP n);
void uptofmarray(int mod,UP n,ModNum *f);
void fmarraytoup(ModNum *f,int d,UP *nr);
void uiarraytoup(unsigned int **f,int n,int d,UP *nr);
void adj_coefup(UP n,Z m,Z m2,UP *nr);
void remcup(UP n,Z mod,UP *nr);
void fft_mulup(UP n1,UP n2,UP *nr);
void trunc_fft_mulup(UP n1,UP n2,int dbd,UP *nr);
void fft_squareup(UP n1,UP *nr);
void fft_mulup_main(UP n1,UP n2,int dbd,UP *nr);
void crup(ModNum **f,int d,int *mod,int index,Z m,UP *r);
void crup_lm(ModNum **f,int d,int *mod,int index,Z m,Z lm_mod,UP *r);
void fft_mulup_specialmod_main(UP n1,UP n2,int dbd,int *modind,int nmod,UP *nr);
void fft_mulup_lm(UP n1,UP n2,UP *nr);
void fft_squareup_lm(UP n1,UP *nr);
void trunc_fft_mulup_lm(UP n1,UP n2,int dbd,UP *nr);
void fft_rembymulup_special_lm(UP n1,UP n2,UP inv2,UP *nr);
void uptolmup(UP n,UP *nr);
void save_up(UP obj,char *name);
void hybrid_powermodup(UP f,UP *xp);
void powermodup(UP f,UP *xp);
void hybrid_generic_powermodup(UP g,UP f,Z d,UP *xp);
void generic_powermodup(UP g,UP f,Z d,UP *xp);
void hybrid_powertabup(UP f,UP xp,UP *tab);
void powertabup(UP f,UP xp,UP *tab);
void squarep_gf2n(VL vl,P n1,P *nr);
void squareup_gf2n(UP n1,UP *nr);
void powermodup_gf2n(UP f,UP *xp);
void generic_powermodup_gf2n(UP g,UP f,Z d,UP *xp);
void tracemodup_gf2n(UP g,UP f,Z d,UP *xp);
void tracemodup_gf2n_slow(UP g,UP f,Z d,UP *xp);
void tracemodup_gf2n_tab(UP g,UP f,Z d,UP *xp);
void square_rem_tab_up_gf2n(UP f,UP *tab,UP *rp);
void powertabup_gf2n(UP f,UP xp,UP *tab);
void find_root_gf2n(UP f,GF2N *r);
void ptoup2(P n,UP2 *nr);
void ptoup2_sparse(P n,UP2 *nr);
void up2top(UP2 n,P *nr);
void up2tovect(UP2 n,VECT *nr);
void up2ton(UP2 p,Q *n);
void ntoup2(Q n,UP2 *p);
void gen_simpup2(UP2 p,GEN_UP2 m,UP2 *r);
void gen_simpup2_destructive(UP2 p,GEN_UP2 m);
void gen_invup2(UP2 p,GEN_UP2 m,UP2 *r);
void gen_pwrmodup2(UP2 a,Z b,GEN_UP2 m,UP2 *c);
void simpup2(UP2 p,UP2 m,UP2 *r);
int degup2(UP2 a);
int degup2_sparse(UP2 a);
int degup2_1(unsigned int a);
void addup2(UP2 a,UP2 b,UP2 *c);
void subup2(UP2 a,UP2 b,UP2 *c);
void _mulup2_1(UP2 a,unsigned int b,UP2 c);
void _mulup2_h(UP2 a,unsigned int b,UP2 c);
void mulup2(UP2 a,UP2 b,UP2 *c);
void _kmulup2_(unsigned int *a,unsigned int *b,int w,unsigned int *c);
void _mulup2_nn(unsigned int *a,unsigned int *b,int w,unsigned int *c);
void _mulup2(UP2 a,UP2 b,UP2 c);
void _mulup2_(_UP2 a,_UP2 b,_UP2 c);
void squareup2(UP2 n,UP2 *nr);
void _squareup2(UP2 n,UP2 nr);
void _adjup2(UP2 n);
void _adjup2_(_UP2 n);
void _addup2(UP2 a,UP2 b,UP2 c);
void _addup2_destructive(UP2 a,UP2 b);
void _addup2_(_UP2 a,_UP2 b,_UP2 c);
void _addtoup2_(_UP2 a,_UP2 b);
unsigned int mulup2_bb(unsigned int a,unsigned int b);
void init_up2_tab();
void divup2_1(unsigned int a1,unsigned int a2,int e1,int e2,unsigned int *qp,unsigned int *rp);
void qrup2(UP2 a,UP2 b,UP2 *q,UP2 *r);
void _qrup2(UP2 a,UP2 b,UP2 q,UP2 r);
void remup2(UP2 a,UP2 b,UP2 *c);
void _remup2(UP2 a,UP2 b,UP2 c);
void remup2_sparse(UP2 a,UP2 b,UP2 *c);
void remup2_sparse_destructive(UP2 a,UP2 b);
void remup2_type1_destructive(UP2 a,int d);
void remup2_3_destructive(UP2 a,UP2 b);
void remup2_5_destructive(UP2 a,UP2 b);
void _invup2_1(unsigned int f1,unsigned int f2,unsigned int *a1,unsigned int *b1);
void _gcdup2_1(unsigned int f1,unsigned int f2,unsigned int *gcd);
void up2_init_eg();
void up2_show_eg();
void invup2(UP2 a,UP2 m,UP2 *inv);
void gcdup2(UP2 a,UP2 m,UP2 *gcd);
void chsgnup2(UP2 a,UP2 *c);
int compup2(UP2 n1,UP2 n2);
void _copyup2(UP2 n,UP2 r);
void _bshiftup2(UP2 n,int b,UP2 r);
void _bshiftup2_destructive(UP2 n,int b);
void diffup2(UP2 f,UP2 *r);
int sqfrcheckup2(UP2 f);
int irredcheckup2(UP2 f);
int irredcheck_dddup2(UP2 f);
void _copy_up2bits(UP2 p,unsigned int **mat,int pos);
int compute_multiplication_matrix(P p0,GF2MAT *mp);
void compute_change_of_basis_matrix(P p0,P p1,int to,GF2MAT *m01,GF2MAT *m10);
void compute_change_of_basis_matrix_with_root(P p0,P p1,int to,GF2N root,GF2MAT *m01,GF2MAT *m10);
int compute_representation_conversion_matrix(P p0,GF2MAT *np,GF2MAT *pn);
void mul_nb(GF2MAT mat,unsigned int *a,unsigned int *b,unsigned int *c);
void leftshift(unsigned int *a,int n);
void mat_to_gf2mat(MAT a,unsigned int ***b);
void gf2mat_to_mat(unsigned int **a,int n,MAT *b);
void mulgf2mat(int n,unsigned int **a,unsigned int **b,unsigned int **c);
void mulgf2vectmat(int n,unsigned int *a,unsigned int **b,unsigned int *c);
int mulgf2vectvect(int n,unsigned int *a,unsigned int *b);
int invgf2mat(int n,unsigned int **a,unsigned int **b);
void _mulup2_22(unsigned int *a1,unsigned int *a2,unsigned int *ar);
void _mulup2_33(unsigned int *a1,unsigned int *a2,unsigned int *ar);
void _mulup2_44(unsigned int *a1,unsigned int *a2,unsigned int *ar);
void _mulup2_55(unsigned int *a1,unsigned int *a2,unsigned int *ar);
void _mulup2_66(unsigned int *a1,unsigned int *a2,unsigned int *ar);
void type1_bin_invup2(UP2 a,int n,UP2 *inv);
UP2 *compute_tab_gf2n(UP2 f);
UP compute_trace_gf2n(UP2 *tab,GF2N c,int n);
void up2toup(UP2 f,UP *r);
void find_root_up2(UP2 f,GF2N *r);
void addq(Q n1,Q n2,Q *nr);
void subq(Q n1,Q n2,Q *nr);
void mulq(Q n1,Q n2,Q *nr);
void divq(Q n1,Q n2,Q *nq);
void divsq(Q n1,Q n2,Q *nq);
void invq(Q n,Q *nr);
void chsgnq(Q n,Q *nr);
void pwrq(Q n1,Q n,Q *nr);
int cmpq(Q q1,Q q2);
void remq(Q n,Q m,Q *nr);
void mkbc(int n,Z *t);
void mkwcm(int k,int l,int m,int *t);
void invl(Q a,Q mod,Q *ar);
void plisttop(P *f,V v,int n,P *gp);
int divtp(VL vl,P p1,P p2,P *q);
int divtdcp(VL vl,P p1,P p2,P *q);
int divtpz(VL vl,P p1,P p2,P *q);
int divtdcpz(VL vl,P p1,P p2,P *q);
void udivpz(P f1,P f2,P *fqp,P *frp);
void udivpwm(Z mod,P p1,P p2,P *q,P *r);
void udivpzwm(Z mod,P f1,P f2,P *fqp,P *frp);
void henmv(VL vl,VN vn,P f,P g0,P h0,P a0,P b0,P lg,P lh,P lg0,P lh0,Z q,int k,P *gp,P *hp);
void henmvmain(VL vl,VN vn,P f,P fi0,P fi1,P gi0,P gi1,P l0,P l1,Z mod,Z mod2,int k,P *fr0,P *fr1);
void henzq(P f,P i0,UM fi0,P i1,UM fi1,int p,int k,P *fr0p,P *fr1p,P *gr0p,P *gr1p,Z *qrp);
void henzq1(P g,P h,Q bound,P *gcp,P *hcp,Z *qp);
void addm2p(VL vl,Z mod,Z mod2,P n1,P n2,P *nr);
void subm2p(VL vl,Z mod,Z mod2,P n1,P n2,P *nr);
void mulm2p(VL vl,Z mod,Z mod2,P n1,P n2,P *nr);
void addm2q(Z mod,Z mod2,Z n1,Z n2,Z *nr);
void subm2q(Z mod,Z mod2,Z n1,Z n2,Z *nr);
void mulm2q(Z mod,Z mod2,Z n1,Z n2,Z *nr);
void rem2q(Z n,Z m,Z m2,Z *nr);
void exthpc_generic(VL vl,P p,int d,V v,P *pr);
void exthp(VL vl,P p,int d,P *pr);
void exthpc(VL vl,V v,P p,int d,P *pr);
void cbound(VL vl,P p,Q *b);
int geldb(VL vl,P p);
int getdeg(V v,P p);
void cmax(P p,Q *b);
int nextbin(VN vn,int n);
void mulsgn(VN vn,VN vnt,int n,VN vn1);
void next(VN vn);
void clctv(VL vl,P p,VL *nvlp);
void markv(VN vn,int n,P p);
void vntovl(VN vn,int n,VL *vlp);
int dbound(V v,P f);
int homdeg(P f);
int minhomdeg(P f);
void adjc(VL vl,P f,P a,P lc0,Z q,P *fr,P *ar);
void affinemain(VL vl,P p,V v0,int n,P *pl,P *pr);
void restore(VL vl,P f,VN vn,P *fr);
void mergev(VL vl,VL vl1,VL vl2,VL *nvlp);
void substvp(VL vl,P f,VN vn,P *g);
void affine(VL vl,P f,VN vn,P *fr);
void addnum(VL vl,Num a,Num b,Num *c);
void subnum(VL vl,Num a,Num b,Num *c);
void mulnum(VL vl,Num a,Num b,Num *c);
void divnum(VL vl,Num a,Num b,Num *c);
void pwrnum(VL vl,Num a,Num b,Num *c);
void chsgnnum(Num a,Num *c);
int compnum(VL vl,Num a,Num b);
void one_ff(Num *c);
int negative_number(Num c);
void simp_ff(Obj p,Obj *rp);
int fft_available(int d1,int n1,int d2,int n2);
void get_fft_prime(int index,int *p,int *d);
void nglob_init();
void get_eg(struct oEGT *p);
void init_eg(struct oEGT *eg);
void add_eg(struct oEGT *base,struct oEGT *start,struct oEGT *end);
void print_eg(char *item,struct oEGT *eg);
void print_split_eg(struct oEGT *start,struct oEGT *end);
void print_split_e(struct oEGT *start,struct oEGT *end);
void suspend_timer() ;
void resume_timer() ;
void reset_engine() ;
unsigned int get_asir_version() ;
char *get_asir_distribution();
void create_error(ERR *err,unsigned int serial,char *msg,LIST trace);
void init_lprime();
int get_lprime(int index);
void create_new_lprimes(int index);
void reorderp(VL nvl,VL ovl,P p,P *pr);
void substp(VL vl,P p,V v0,P p0,P *pr);
void detp(VL vl,P **rmat,int n,P *dp);
void invmatp(VL vl,P **rmat,int n,P ***imatp,P *dnp);
void reordvar(VL vl,V v,VL *nvlp);
void gcdprsp(VL vl,P p1,P p2,P *pr);
void gcdcp(VL vl,P p,P *pr);
void sprs(VL vl,V v,P p1,P p2,P *pr);
void resultp(VL vl,V v,P p1,P p2,P *pr);
void srch2(VL vl,V v,P p1,P p2,P *pr);
void srcr(VL vl,V v,P p1,P p2,P *pr);
void res_ch_det(VL vl,V v,P p1,P p2,P *pr);
void res_detmp(VL vl,int mod,V v,P p1,P p2,P *dp);
void premp(VL vl,P p1,P p2,P *pr);
void ptozp0(P p,P *pr);
void mindegp(VL vl,P p,VL *mvlp,P *pr);
void maxdegp(VL vl,P p,VL *mvlp,P *pr);
void min_common_vars_in_coefp(VL vl,P p,VL *mvlp,P *pr);
void minlcdegp(VL vl,P p,VL *mvlp,P *pr);
void sort_by_deg(int n,P *p,P *pr);
void sort_by_deg_rev(int n,P *p,P *pr);
void minchdegp(VL vl,P p,VL *mvlp,P *pr);
int getchomdeg(V v,P p);
int getlchomdeg(V v,P p,int *d);
int nmonop(P p);
int qpcheck(Obj p);
int uzpcheck(Obj p);
int p_mag(P p);
int maxblenp(P p);
void berle(int index,int count,P f,ML *listp);
int berlecnt(int mod,UM f);
int berlecntmain(int mod,int n,int m,int **c);
UM *berlemain(int mod,UM f,UM *fp);
void hensel(int index,int count,P f,ML *listp);
void hensel2(int index,int count,P f,ML *listp);
void henmain2(LUM f,UM g0,UM h0,UM a0,UM b0,int m,int bound,LUM *gp);
void clearlum(int n,int bound,LUM f);
void addtolum(int m,int bound,LUM f,LUM g);
void hsq(int index,int count,P f,int *nindex,DCP *dcp);
void gcdgen(P f,ML blist,ML *clistp);
void henprep2(int mod,int q,int k,UM f,UM g,UM h,UM qg,UM qh,UM qa,UM qb);
void henprep(P f,ML blist,ML clist,ML *bqlistp,ML *cqlistp);
void henmain(LUM f,ML bqlist,ML cqlist,ML *listp);
int mignotte(int q,P f);
int mig(int q,int d,P f);
void sqad(mp_limb_t man,int exp);
void ptolum(int q,int bound,P f,LUM fl);
void modfctrp(P p,int mod,int flag,DCP *dcp);
void gensqfrum(int mod,UM p,struct oDUM *dc);
void ddd(int mod,UM f,UM *r);
void canzas(int mod,UM f,int d,UM *base,UM *r);
void randum(int mod,int d,UM p);
void pwrmodum(int mod,UM p,int e,UM f,UM pr);
void spwrum0(int mod,UM m,UM f,Z e,UM r);
void mult_mod_tab(UM p,int mod,UM *tab,UM r,int d);
void make_qmat(UM p,int mod,UM *tab,int ***mp);
void null_mod(int **mat,int mod,int n,int *ind);
void null_to_sol(int **mat,int *ind,int mod,int n,UM *r);
void newddd(int mod,UM f,UM *r);
int nfctr_mod(UM f,int mod);
int irred_check(UM f,int mod);
int berlekamp(UM p,int mod,int df,UM *tab,UM *r);
void minipoly_mod(int mod,UM f,UM p,UM mp);
void lnf_mod(int mod,int n,UM p0,UM p1,struct p_pair *list,UM np0,UM np1);
void showum(UM p);
void showumat(int **mat,int n);
int find_root(int mod,UM p,int *root);
void fctrp(VL vl,P f,DCP *dcp);
void fctr_wrt_v_p(VL vl,P f,V v,DCP *dcp);
void homfctr(VL vl,P g,DCP *dcp);
void mfctr(VL vl,P f,DCP *dcp);
void mfctr_wrt_v(VL vl,P f,V v,DCP *dcp);
void adjsgn(P p,DCP dc);
int headsgn(P p);
void fctrwithmvp(VL vl,P f,V v,DCP *dcp);
void mfctrwithmv(VL vl,P f,V v,DCP *dcp);
void ufctr(P f,int hint,DCP *dcp);
void mfctrmain(VL vl,P p,DCP *dcp);
void ufctrmain(P p,int hint,DCP *dcp);
void cycm(V v,int n,DCP *dcp);
void cycp(V v,int n,DCP *dcp);
void calcphi(V v,int n,struct oMF *mfp);
void mkssum(V v,int e,int s,int sgn,P *r);
int iscycp(P f);
int iscycm(P f);
void sortfs(DCP *dcp);
void sortfsrev(DCP *dcp);
void nthrootchk(P f,struct oDUM *dc,ML fp,DCP *dcp);
void sqfrp(VL vl,P f,DCP *dcp);
void msqfr(VL vl,P f,DCP *dcp);
void usqp(P f,DCP *dcp);
void msqfrmain(VL vl,P p,DCP *dcp);
void msqfrmainmain(VL vl,P p,VN vn,P p0,DCP dc0,DCP *dcp,P *pp);
void mfctrhen2(VL vl,VN vn,P f,P f0,P g0,P h0,P lcg,P lch,P *gp);
int sqfrchk(P p);
int cycchk(P p);
int zerovpchk(VL vl,P p,VN vn);
int valideval(VL vl,DCP dc,VN vn);
void estimatelc(VL vl,Z c,DCP dc,VN vn,P *lcp);
void monomialfctr(VL vl,P p,P *pr,DCP *dcp);
void afctr(VL vl,P p0,P p,DCP *dcp);
void afctrmain(VL vl,P p0,P p,int init,DCP *dcp);
int divtmp(VL vl,int mod,P p1,P p2,P *q);
int divtdcmp(VL vl,int mod,P p1,P p2,P *q);
void GC_gcollect();

/* IMAT */
void Pnewimat(NODE, IMAT *);
void PChsgnI(NODE, IMAT *);
void Pm2Im(NODE, IMAT *);
void PIm2m(NODE, MAT *);

void AddMatI(VL, IMAT, IMAT, IMAT *);
void MulMatI(VL, IMAT, IMAT, IMAT *);
void MulMatG(VL, Obj, Obj, Obj *);
void MulrMatI(VL, Obj, Obj, Obj *);
void MulMatS(VL, IMAT, IMAT, IMAT *);
void PutIent(IMAT, int, int, Obj);
void GetIent(IMAT, int, int, Obj);
void GetIbody(IMAT, int, int, Obj *);
void ChsgnI(IMAT, IMAT *c);
void AppendIent(IMAT, int, int, Obj);
void MEnt(int, int, int, Obj, IENT *);
void GetForeIent(IMATC *, IENT *, int *);
void GetNextIent(IMATC *, IENT *, int *);
void SubMatI(VL, IMAT, IMAT, IMAT *);
/* IMAT */


#if 0 && !defined(VISUAL) && !defined(__MINGW32__)
void bzero(const void *,int);
void bcopy(const void *,void *,int);
char *index(char *,int);
#endif

void chsgnnbp(NBP p,NBP *rp);
void subnbp(VL vl,NBP p1,NBP p2, NBP *rp);
void addnbp(VL vl,NBP p1,NBP p2, NBP *rp);
void mulnbp(VL vl,NBP p1,NBP p2, NBP *rp);
void pwrnbp(VL vl,NBP p1,Z n, NBP *rp);
int compnbp(VL vl,NBP p1,NBP p2);

#define WORDSIZE_IN_N(a) (ABS((a)->_mp_size)*GMP_LIMB_BITS/32)

#define MPZTOZ(g,q) (!mpz_sgn(g)?((q)=0):(NEWZ(q),BDY(q)[0]=(g)[0],(q)))
#define MPDNTOQ(n,d,q) (NEWQ(q),mpq_numref(BDY(q))[0]=(n)[0],mpq_denref(BDY(q))[0]=(d)[0],(q))
#define MPQTOQ(g,q) (!mpq_sgn(g)?((q)=0):(NEWQ(q),BDY(q)[0]=(g)[0],(q)))

#define INTMPQ(a)  (!mpz_cmp_ui(mpq_denref(a),1))
#define INTQ(a) ((a)&&NUM(a)&&RATN(a)&&((Q)a)->z==1)
#define UNIQ(a) (INTQ(a)&&!mpz_cmp_si(BDY((Z)a),1))
#define MUNIQ(a) (INTQ(a)&&!mpz_cmp_si(BDY((Z)a),-1))

#define MPZTOMPQ(z,q) (mpq_init(q),mpq_numref(q)[0] = (z)[0],mpz_set_ui(mpq_denref(q),1))

#define MPFRTOBF(g,q) (NEWBF(q),BDY(q)[0]=(g)[0],(q))

void *gc_realloc(void *p,size_t osize,size_t nsize);
void gc_free(void *p,size_t size);
void init_gmpq();
Q mpqtozq(mpq_t a);

unsigned long mul64(unsigned long a,unsigned long b,unsigned long *c);
unsigned long mod64(unsigned long u,unsigned long l,unsigned long b);
unsigned long mulmod64(unsigned long a,unsigned long b,unsigned long m);
unsigned long muladdmod64(unsigned long a,unsigned long b,unsigned long c,unsigned long m);
unsigned long invmod64(unsigned long a,unsigned long m);

void addz(Z n1,Z n2,Z *nr);
void subz(Z n1,Z n2,Z *nr);
void mulz(Z n1,Z n2,Z *nr);
void mul1z(Z n1,long n2,Z *nr);
void muladdtoz(Z n1,Z n2,Z *nr);
void mul1addtoz(Z n1,long n2,Z *nr);
void divz(Z n1,Z n2,Z *nq);
void divqrz(Z n1,Z n2,Z *nq,Z *nr);
void remz(Z n1,Z n2,Z *nr);
void invz(Z n1,Z n2,Z *nr);
void chsgnz(Z n,Z *nr);
void absz(Z n,Z *nr);
int evenz(Z n);
int smallz(Z n);
void bshiftz(Z n,int s,Z *nr);
void pwrz(Z n1,Z n,Z *nr);
int cmpz(Z q1,Z q2);
void gcdz(Z n1,Z n2,Z *nq);
void lcmz(Z n1,Z n2,Z *nq);
void gcdvz(VECT v,Z *q);
void gcdvz_estimate(VECT v,Z *q);
void factorialz(unsigned int n,Z *nr);
void randomz(int blen,Z *nr);
int tstbitz(Z n,int k);
void addq(Q n1,Q n2,Q *nr);
void subq(Q n1,Q n2,Q *nr);
void mulq(Q n1,Q n2,Q *nr);
void divq(Q n1,Q n2,Q *nq);
void chsgnq(Q n,Q *nr);
void absq(Q n,Q *nr);
void pwrq(Q n1,Q n,Q *nr);
int cmpq(Q n1,Q n2);
void mkwc(int k,int l,Z *t);
unsigned int remqi(Q n,unsigned int m);
void nmq(Q q,Z *z);
void dnq(Q q,Z *z);
void _mdtodp(DP p,DP *pr);
void add_denomlist(P f);
void algobjtorat(Obj f,Obj *r);
void algtodalg(Alg a,DAlg *r);
void appenduflist(NODE n);
void arf_add(VL,Obj,Obj,Obj *);
void arf_chsgn(Obj,Obj *);
void arf_div(VL,Obj,Obj,Obj *);
void arf_mul(VL,Obj,Obj,Obj *);
void arf_sub(VL,Obj,Obj,Obj *);
void asir_terminate(int);
void check_intr();
void clctalg(P,VL *);
void comm_muld_trunc(VL vl,DP p1,DP p2,DL dl,DP *pr);
void comm_quod(VL vl,DP p1,DP p2,DP *pr);
void create_modorder_spec(int id,LIST shift,struct modorder_spec **s);
void dalgtoalg(DAlg da,Alg *r);
void divsz(Z n1,Z n2,Z *nq);
void dp_ht(DP p,DP *rp);
void dp_interreduce(LIST f,LIST v,int field,struct order_spec *ord,LIST *rp);
void dp_mbase(NODE,NODE *);
void dp_nf_tab_f(DP p,LIST *tab,DP *rp);
void dp_ptozp(DP,DP *);
void dp_sort(DP p,DP *rp);
void dp_subd(DP,DP,DP *);
void dp_true_nf(NODE,DP,DP *,int,DP *,P *);
void dp_true_nf_marked(NODE b,DP g,DP *ps,DP *hps,DP *rp,P *nmp,P *dnp);
void dp_true_nf_marked_mod(NODE b,DP g,DP *ps,DP *hps,int mod,DP *rp,P *dnp);
void gbcheck_list(NODE f,int n,VECT *gp,LIST *pp);
void gcdsf(VL vl,P *pa,int k,P *r);
void get_algtree(Obj f,VL *r);
void get_vars(Obj,VL *);
void gfstopgfs(GFS a,V v,P *c);
void henmain_incremental(LUM f,LUM *bqlist,ML cqlist, int np, int mod, int start, int bound);
void iftogfs(int n,GFS *c);
void indextogfs(int index,GFS *c);
void init_denomlist();
void inva_chrem(P p0,P p,NODE *pr);
void itogfs(int n,GFS *c);
void makevar(char *,P *);
void mdtodp(DP p,DP *pr);
void mfctrsf(VL vl,P f,DCP *dcp);
void mulp_trunc(VL vl,P p1,P p2,VN vn,P *pr);
void nd_det(int mod,MAT f,P *rp);
void nd_gr(LIST f,LIST v,int m,int homo,int retdp,int f4,struct order_spec *ord,LIST *rp);
void nd_gr_postproc(LIST f,LIST v,int m,struct order_spec *ord,int do_check,LIST *rp);
void nd_gr_recompute_trace(LIST f,LIST v,int m,struct order_spec *ord,LIST tlist,LIST *rp);
void nd_gr_trace(LIST f,LIST v,int trace,int homo,int retdp,int f4,struct order_spec *ord,LIST *rp);
void nd_nf_p(Obj f,LIST g,LIST v,int m,struct order_spec *ord,Obj *rp);
void obj_algtodalg(Obj obj,Obj *r);
void obj_dalgtoalg(Obj obj,Obj *r);
void ox_bcast_102(int root);
void ox_reduce_102(int root,void (*func)());
void print_to_wfep(Obj obj);
void printz(Z n);
void pthrootgfs(GFS a,GFS *b);
void quop_trunc(VL vl,P p1,P p2,VN vn,P *pr);
void red_by_vect_sf(int m,unsigned int *p,unsigned int *r,unsigned int hc,int len);
void red_by_vect_lf(mpz_t *p,mpz_t *r,mpz_t hc,int len);
void removecont_array(P *c,int n,int full);
void reset_top_weight();
void setfield_dalg(NODE alist);
void setfield_gb(NODE gb,VL vl,struct order_spec *spec);
void setsecuremode(int value);
void sfbfctr_shift(P f,V x,V y,int degbound,GFS *evp,P *sfp,DCP *dcp);
void sfexgcd_by_hensel(BM g,BM h,int dy,BM *ap,BM *bp);
void sfptopsfp(P f,V v,P *gp);
void simpdalg(DAlg da,DAlg *r);
void simple_derivr(VL vl,Obj a,V v,Obj *b);
void substpp(VL vl,P p,V *vvect,P *svect,int nv,P *pr);
void ufctrsf(P p,DCP *dcp);
void vltopl(VL vl,LIST *l);

int arf_comp(VL, Obj, Obj);
int available_mcindex(int ind);
int compare_zero(int n,int *u,int row,int **w);
int create_composite_order_spec(VL vl,LIST order,struct order_spec **specp);
int create_order_spec(VL,Obj,struct order_spec **);
int dalgtoup(DAlg da,P *up,Z *dn);
int dl_equal(int nv,DL dl1,DL dl2);
int dp_redble(DP,DP);
int dpv_hp(DPV p);
int generic_gauss_elim(MAT mat,MAT *nm,Z *dn,int **rindp,int **cindp);
int generic_gauss_elim2(MAT mat,MAT *nm,Z *dn,int **rindp,int **cindp);
int generic_gauss_elim_full(MAT mat,MAT *nm,Z *dn,int **rindp,int **cindp);
int generic_gauss_elim_direct(MAT mat,MAT *nm,Z *dn,int **rindp,int **cindp);
int generic_gauss_elim_hensel(MAT mat,MAT *nmmat,Z *dn,int **rindp,int **cindp);
int generic_gauss_elim_hensel_dalg(MAT mat,DP *mb,MAT *nmmat,Z *dn,int **rindp,int **cindp);
int generic_gauss_elim_mod(int **mat0,int row,int col,int md,int *colstat);
int generic_gauss_elim_mod2(int **mat0,int row,int col,int md,int *colstat,int *rowstat);
int get_field_type(P p);
int get_ox_server_id(int index);
int getsecuremode();
int find_lhs_and_lu_mod(unsigned int **a,int row,int col,unsigned int md,int **rinfo,int **cinfo);
void solve_by_lu_mod(int **a,int n,int md,int **b,int l,int normalize);
int inttorat(Z c,Z m,Z b,Z *nmp,Z *dnp);
int intvtoratv(Z *v,int n,Z md,Z b,Z *nm,Z *dn);
int intmtoratm(MAT mat,Z md,MAT nm,Z *dn);
int intmtoratm2(MAT mat,Z md,MAT nm,Z *dn,int *stat);
int gensolve_check(MAT mat,MAT nm,Z dn,int *rind,int *cind);
int gensolve_check2(MAT mat,MAT nm,Z *dn,int *rind,int *cind);
int invdalg(DAlg a,DAlg *c);
int is_eq(Obj a0,Obj a1);
int length(NODE);
int lu_mod(unsigned int **a,int n,unsigned int md,int **rinfo);
int n_bits_z(Z a);
int nd_gauss_elim_z(Z **mat0,int *sugar,int row,int col,int *colstat);
int poly_is_dependent(P p,V v);
int setsecureflag(char *name,int value);
int sfdegtest(int dy,int bound,UM *d1c,int k,int *in);
int sgnz(Z n);
int sgnq(Q n);
void binarytoz(char *binary,Z *np);
void hextoz(char *hex,Z *np);
void todouble(Obj,Obj *);
void notdef(VL vl,Obj a,Obj b,Obj *c);
void notdef2(Obj b,Obj *c);
int notdefi(VL vl,Obj b,Obj c);
void lmtolf(LM f,Z *b);

#if SIZEOF_LONG == 8
mp_limb_t get_lprime64(int index);
mp_limb_t **almat64(int,int);
mp_limb_t remqi64(Q n,mp_limb_t m);
int generic_gauss_elim64(MAT mat,MAT *nm,Z *dn,int **rindp,int **cindp);
int generic_gauss_elim_mod64(mp_limb_t **mat,int row,int col,mp_limb_t md,int *colstat);
#endif

#if defined(VISUAL) || defined(__MINGW32__)
void check_intr();
void enter_signal_cs();
void leave_signal_cs();
void leave_signal_cs_all();
#define LEAVE_SIGNAL_CS_ALL  leave_signal_cs_all()
#else
#define LEAVE_SIGNAL_CS_ALL
#endif

extern void *thread_args[];
typedef void *(*WORKER_FUNC)(void *);
void create_and_execute_worker(int nworker,WORKER_FUNC func);
void execute_worker(int nworker,WORKER_FUNC func);
