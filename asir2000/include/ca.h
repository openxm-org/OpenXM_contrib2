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
 * $OpenXM: OpenXM_contrib2/asir2000/include/ca.h,v 1.14 2001/06/07 04:54:41 noro Exp $ 
*/
#include <stdio.h>

#if defined(hpux)
#include <netinet/in.h>
# define setbuffer(FP,buf,siz) setvbuf(FP,buf,_IOFBF,siz)
#endif

#if defined(VISUAL)
#include <stdlib.h>
#endif

#if defined(linux) || (defined(sun) && !defined(SYSV)) || defined(news5000) || (defined(mips) && defined(ultrix))
#include <alloca.h>
#endif

#if (defined(sun) && defined(SYSV))
#define alloca(x) __builtin_alloca(x)
#endif

#if defined(VISUAL)
#include <malloc.h>
#endif

#if 0
#include <sys/types.h>
typedef caddr_t pointer;
#endif

typedef void * pointer;

#if defined(VISUAL)
#include <string.h>
#define index(s,c) strchr(s,c)
#define bzero(s,len) memset(s,0,len)
#define bcopy(x,y,len) memcpy(y,x,len)
#endif

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
#define O_VOID -1

#define N_Q 0
#define N_R 1
#define N_A 2
#define N_B 3
#define N_C 4
#define N_M 5
#define N_LM 6
#define N_GF2N 7
#define N_GFPN 8
#define N_GFS 9

#define ORD_REVGRADLEX 0
#define ORD_GRADLEX 1
#define ORD_LEX 2

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

typedef struct oN {
	int p;
	unsigned int b[1];
} *N;

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

typedef struct oQ {
	short id;
	char nid;
	char sgn;
	N nm;
	N dn;
} *Q;

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
	long body[1];
} *BF;

typedef struct oC {
	short id;
	char nid;
	char pad;
	struct oNum *r,*i;
} *C;

typedef struct oLM {
	short id;
	char nid;
	char pad;
	struct oN *body;
} *LM;

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

typedef struct oUSINT {
	short id;
	short pad;
	unsigned body;
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
} *QUOTE;

typedef struct oObj {
	short id;
	short pad;
} *Obj;

typedef struct oDCP {
	Q d;
	P c;
	struct oDCP *next;	
} *DCP;

typedef struct oMP {
	struct oDL *dl;
	P c;
	struct oMP *next;
} *MP;

typedef struct oDL {
	int td;
	int d[1];
} *DL;

typedef struct oVL {
	V v;
	struct oVL *next;
} *VL;

typedef struct oNODE {
	pointer body;
	struct oNODE *next;
} *NODE;

typedef struct oUM {
	int d;
	int c[1];
} *UM;

typedef struct oLUM {
	int d;
	int *c[1];
} *LUM;

typedef struct oML {
	int n;
	int mod;
	int bound;
	pointer c[1];
} *ML;

typedef struct oUB {
	int d;
	N c[1];
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

struct order_spec {
	int id;
	Obj obj;
	int nv;
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
	} ord;
};

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

/* include interval.h */
#include "interval.h"

#define INDEX 100

#if USE_FLOAT
typedef float ModNum;
#define NPrimes 536
#else
typedef unsigned int ModNum;
#define NPrimes 13681
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

/* memory allocators (W_... : uses alloca) */

#if 0
#define MALLOC(d) Risa_GC_malloc(d)
#define MALLOC_ATOMIC(d) Risa_GC_malloc_atomic(d)
#define REALLOC(p,d) Risa_GC_realloc(p,d)
#else
#define MALLOC(d) GC_malloc(d)
#define MALLOC_ATOMIC(d) GC_malloc_atomic(d)
#define REALLOC(p,d) GC_realloc(p,d)
#endif
#define CALLOC(d,e) MALLOC((d)*(e))

#if (defined(__GNUC__) || defined(vax) || defined(apollo) || defined(alloca) || defined(VISUAL))
#define ALLOCA(d) alloca(d)
#else
#define ALLOCA(d) MALLOC(d)
#endif

#define TRUESIZE(type,n,atype) (sizeof(struct type)+MAX((n),0)*sizeof(atype))
#define NALLOC(d) ((N)MALLOC_ATOMIC(TRUESIZE(oN,(d)-1,int)))
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

#define NEWUP2(q,w)\
((q)=(UP2)MALLOC_ATOMIC(TRUESIZE(oUP2,(w)-1,unsigned int)),\
bzero((char *)(q)->b,(w)*sizeof(unsigned int)))
#define W_NEWUP2(q,w)\
((q)=(UP2)ALLOCA(TRUESIZE(oUP2,(w)-1,unsigned int)),\
bzero((char *)(q)->b,(w)*sizeof(unsigned int)))
#define W_NEW_UP2(q,w)\
((q).b=(unsigned int *)ALLOCA((w)*sizeof(unsigned int)))

/* cell allocators */
#define NEWQ(q) ((q)=(Q)MALLOC(sizeof(struct oQ)),OID(q)=O_N,NID(q)=N_Q)
#define NEWMQ(q) ((q)=(MQ)MALLOC_ATOMIC(sizeof(struct oMQ)),OID(q)=O_N,NID(q)=N_M)
#define NEWGFS(q) ((q)=(GFS)MALLOC_ATOMIC(sizeof(struct oGFS)),OID(q)=O_N,NID(q)=N_GFS)
#define NEWP(p) ((p)=(P)MALLOC(sizeof(struct oP)),OID(p)=O_P)
#define NEWR(r) ((r)=(R)MALLOC(sizeof(struct oR)),OID(r)=O_R,(r)->reduced=0)
#define NEWLIST(l) ((l)=(LIST)MALLOC(sizeof(struct oLIST)),OID(l)=O_LIST)
#define NEWVECT(l) ((l)=(VECT)MALLOC(sizeof(struct oVECT)),OID(l)=O_VECT)
#define NEWSTR(l) ((l)=(STRING)MALLOC(sizeof(struct oSTRING)),OID(l)=O_STR)
#define NEWCOMP(c,n) ((c)=(COMP)MALLOC(sizeof(struct oCOMP)+((n)-1)*sizeof(Obj)),OID(c)=O_COMP)
#define NEWDP(d) ((d)=(DP)MALLOC(sizeof(struct oDP)),OID(d)=O_DP)
#define NEWUSINT(u) ((u)=(USINT)MALLOC_ATOMIC(sizeof(struct oUSINT)),OID(u)=O_USINT)
#define NEWERR(e) ((e)=(ERR)MALLOC(sizeof(struct oERR)),OID(e)=O_ERR)
#define NEWMATHCAP(e) ((e)=(MATHCAP)MALLOC(sizeof(struct oMATHCAP)),OID(e)=O_MATHCAP)
#define NEWBYTEARRAY(e) ((e)=(BYTEARRAY)MALLOC(sizeof(struct oBYTEARRAY)),OID(e)=O_BYTEARRAY)
#define NEWQUOTE(e) ((e)=(QUOTE)MALLOC(sizeof(struct oQUOTE)),OID(e)=O_QUOTE)

#define NEWNODE(a) ((a)=(NODE)MALLOC(sizeof(struct oNODE)))
#define NEWDC(dc) ((dc)=(DCP)MALLOC(sizeof(struct oDCP)))
#define NEWV(v) ((v)=(V)MALLOC(sizeof(struct oV)))
#define NEWVL(vl) ((vl)=(VL)MALLOC(sizeof(struct oVL)))
#define NEWMP(m) ((m)=(MP)MALLOC(sizeof(struct oMP)))

#define NEWMAT(l) ((l)=(MAT)MALLOC(sizeof(struct oMAT)),OID(l)=O_MAT)
#define NEWGF2MAT(l) ((l)=(GF2MAT)MALLOC(sizeof(struct oGF2MAT)),OID(l)=O_GF2MAT)
#define NEWGFMMAT(l) ((l)=(GFMMAT)MALLOC(sizeof(struct oGF2MAT)),OID(l)=O_GFMMAT)
#define NEWReal(q) ((q)=(Real)MALLOC_ATOMIC(sizeof(struct oReal)),OID(q)=O_N,NID(q)=N_R)
#define NEWAlg(r) ((r)=(Alg)MALLOC(sizeof(struct oAlg)),OID(r)=O_N,NID(r)=N_A)
#define NEWBF(q,l) ((q)=(BF)MALLOC_ATOMIC(TRUESIZE(oBF,(l)-1,long)),OID(q)=O_N,NID(q)=N_B)
#define NEWC(r) ((r)=(C)MALLOC(sizeof(struct oC)),OID(r)=O_N,NID(r)=N_C)
#define NEWLM(r) ((r)=(LM)MALLOC(sizeof(struct oLM)),OID(r)=O_N,NID(r)=N_LM)
#define NEWGF2N(r) ((r)=(GF2N)MALLOC(sizeof(struct oGF2N)),OID(r)=O_N,NID(r)=N_GF2N)
#define NEWGFPN(r) ((r)=(GFPN)MALLOC(sizeof(struct oGFPN)),OID(r)=O_N,NID(r)=N_GFPN)
#define NEWDL(d,n) \
((d)=(DL)MALLOC_ATOMIC(TRUESIZE(oDL,(n)-1,int)),bzero((char *)(d),TRUESIZE(oDL,(n)-1,int)))
#define NEWDL_NOINIT(d,n) \
((d)=(DL)MALLOC_ATOMIC(TRUESIZE(oDL,(n)-1,int)))

#define MKP(v,dc,p) \
(!DEG(dc)?((p)=COEF(dc)):(NEWP(p),VR(p)=(v),DC(p)=(dc),(p)))
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
#define MKLIST(a,b) (NEWLIST(a),(a)->body=(NODE)(b))
#define MKVECT(m,l) \
(NEWVECT(m),(m)->len=(l),(m)->body=(pointer *)CALLOC((l),sizeof(pointer)))
#define MKMAT(m,r,c) \
(NEWMAT(m),(m)->row=(r),(m)->col=(c),(m)->body=(pointer **)almat_pointer((r),(c)))
#define TOGF2MAT(r,c,b,m) (NEWGF2MAT(m),(m)->row=(r),(m)->col=(c),(m)->body=(b))
#define TOGFMMAT(r,c,b,m) (NEWGFMMAT(m),(m)->row=(r),(m)->col=(c),(m)->body=(b))
#define MKSTR(a,b) (NEWSTR(a),(a)->body=(char *)(b))
#define MKDP(n,m,d) (NEWDP(d),(d)->nv=(n),BDY(d)=(m))
#define MKLM(b,l) (!(b)?(l)=0:(NEWLM(l),(l)->body=(b),(l)))
#define MKGF2N(b,l) (!(b)?(l)=0:(NEWGF2N(l),(l)->body=(b),(l)))
#define MKGFPN(b,l) (!(b)?(l)=0:(NEWGFPN(l),(l)->body=(b),(l)))
#define MKUSINT(u,b) (NEWUSINT(u),(u)->body=(unsigned)(b))
#define MKERR(e,b) (NEWERR(e),(e)->body=(Obj)(b))
#define MKMATHCAP(e,b) (NEWMATHCAP(e),(e)->body=(LIST)(b))
#define MKBYTEARRAY(m,l) \
(NEWBYTEARRAY(m),(m)->len=(l),(m)->body=(char *)MALLOC_ATOMIC((l)),bzero((m)->body,(l)))
#define MKQUOTE(q,b) (NEWQUOTE(q),(q)->body=(pointer)(b))

#define NEXTDC(r,c) \
if(!(r)){NEWDC(r);(c)=(r);}else{NEWDC(NEXT(c));(c)=NEXT(c);}
#define NEXTNODE(r,c) \
if(!(r)){NEWNODE(r);(c)=(r);}else{NEWNODE(NEXT(c));(c)=NEXT(c);}
#define NEXTMP(r,c) \
if(!(r)){NEWMP(r);(c)=(r);}else{NEWMP(NEXT(c));(c)=NEXT(c);}
#define NEXTMP2(r,c,s) \
if(!(r)){(c)=(r)=(s);}else{NEXT(c)=(s);(c)=(s);}

/* convertors */
#define NTOQ(n,s,q) \
(!(n)?((q)=0):(NEWQ(q),SGN(q)=(s),NM(q)=(n),DN(q)=0,(q)))
#define NDTOQ(n,d,s,q) \
((!(d)||UNIN(d))?NTOQ(n,s,q):(NEWQ(q),SGN(q)=(s),NM(q)=(n),DN(q)=(d),(q)))
#define DUPQ(p,q) \
(NEWQ(q),SGN(q)=SGN(p),NM(q)=NM(p),DN(q)=DN(p))
#define STOQ(n,q) \
((!(n))?((q)=(Q)NULL):(NEWQ(q),\
SGN(q)=((n)>0?1:-1),NM(q)=NALLOC(1),\
PL(NM(q))=1,BD(NM(q))[0]=ABS(n),DN(q)=0,(q)))
#define UTOMQ(a,b) \
((a)?(NEWMQ(b),CONT(b)=(unsigned int)(a),(b)):((b)=0))
#define MKGFS(a,b) \
((NEWGFS(b),CONT(b)=(a),(b)))
#define STOMQ(a,b) \
((a)?(NEWMQ(b),CONT(b)=(a),(b)):((b)=0))
#define UTON(u,n) \
((!(u))?((n)=(N)NULL):((n)=NALLOC(1),PL(n)=1,BD(n)[0]=(unsigned int)(u),(n)))
#define UTOQ(n,q) \
((!(n))?((q)=(Q)NULL):(NEWQ(q),\
SGN(q)=1,NM(q)=NALLOC(1),\
PL(NM(q))=1,BD(NM(q))[0]=(unsigned int)(n),DN(q)=0,(q)))
#define QTOS(q) (!(q)?0:SGN(q)*((int)BD(NM(q))[0]))
#define STON(i,n)\
(i?((n)=NALLOC(1),PL(n)=1,BD(n)[0]=(i),(n)):((n)=(N)0))
#define PTOR(a,b) \
(!(a)?((b)=0):(NEWR(b),NM(b)=(a),DN(b)=(P)ONE,(b)->reduced=1,(b)))
#define RTOS(a) (!(a)?0:QTOS((Q)NM((R)a)))
#define MKReal(a,b) (!(a)?((b)=0):(NEWReal(b),BDY(b)=(a),(b)))
#define MKAlg(b,r) \
(!(b)?((r)=0):NUM(b)?((r)=(Alg)(b)):(NEWAlg(r),BDY(r)=(Obj)(b),(r)))

#if PARI
#define ToReal(a) (!(a)?(double)0.0:REAL(a)?BDY((Real)a):RATN(a)?RatnToReal((Q)a):BIGFLOAT(a)?rtodbl(BDY((BF)a)):0)
#else
#define ToReal(a) (!(a)?(double)0.0:REAL(a)?BDY((Real)a):RATN(a)?RatnToReal((Q)a):0.0)
#endif

/* predicates */
#define NUM(p) (OID(p)==O_N)
#define RAT(p) (OID(p)==O_R)
#define INT(q) (!DN((Q)q))
#define RATN(a) (NID(a)==N_Q)
#define REAL(a) (NID(a)==N_R)
#define BIGFLOAT(a) (NID(a)==N_B)
#define SFF(a) (NID(a)==N_M)
#define UNIQ(q) ((q)&&NUM(q)&&RATN(q)&&(SGN((Q)q)==1)&&UNIN(NM((Q)q))&&(!DN((Q)q)))
#define UNIMQ(q) ((q)&&NUM(q)&&SFF(q)&&(CONT((MQ)q)==1))
#define MUNIQ(q) ((q)&&NUM(q)&&RATN(q)&&(SGN((Q)q)==-1)&&UNIN(NM((Q)q))&&(!DN((Q)q)))
#define MUNIMQ(q) ((q)&&NUM(q)&&SFF(q)&&(CONT((MQ)q)==-1))
#define UNIN(n) ((n)&&(PL(n)==1)&&(BD(n)[0]==1))
#define EVENN(n) ((!(n))||(!(BD(n)[0]%2)))

/* special macros for private memory management */

#define NV(p) ((p)->nv)
#define C(p) ((p)->c)
#define ITOS(p) (((unsigned int)(p))&0x7fffffff)
#define STOI(i) ((P)((unsigned int)(i)|0x80000000))

/* immediate GFS representation */

#define IFTOF(p) ((int)(((unsigned int)(p))&0x7fffffff))
#define FTOIF(i) ((int)(((unsigned int)(i)|0x80000000)))

struct cdl {
	P c;
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

/* externals */
#if 0
double NatToReal();
double RatnToReal();
#endif

extern struct oR oUNIR;
extern struct oQ oUNIQ;
extern struct oMQ oUNIMQ;
extern struct oN oUNIN;
extern struct oUP2 oONEUP2;
extern struct oV oVAR[];
extern struct oV oPVAR[];
extern struct oVL oVLIST[];
extern struct oVL oPVLIST[];
extern VL CO,ALG;
extern VL PVL;
extern R ONER;
extern Q ONE;
extern MQ ONEM;
extern LM ONELM;
extern N ONEN;
extern UP2 ONEUP2;

extern FILE *asir_out;
#if defined(__GNUC__)
extern const int sprime[];
#else
extern int sprime[];
#endif

extern int lprime_size;
extern int *lprime;

extern void (*addnumt[])();
extern void (*subnumt[])();
extern void (*mulnumt[])();
extern void (*divnumt[])();
extern void (*pwrnumt[])();
extern int (*cmpnumt[])();
extern void (*chsgnnumt[])();

/* prototypes */
int compui(VL,USINT,USINT);
int compbytearray(VL,BYTEARRAY,BYTEARRAY);

void powermodup(UP,UP *);
void hybrid_powermodup(UP,UP *);

void powertabup(UP,UP,UP *);
void hybrid_powertabup(UP,UP,UP *);

void generic_powermodup(UP,UP,Q,UP *);
void hybrid_generic_powermodup(UP,UP,Q,UP *);

void powermod1up(UP,UP *);
void hybrid_rembymulup_special(int,UP,UP,UP,UP *);
void hybrid_tmulup(int,UP,UP,int,UP *);
void hybrid_squareup(int,UP,UP *);
void hybrid_mulup(int,UP,UP,UP *);

void getmod_lm(N *);

int maxblenup(UP);
void monicup(UP,UP *);
void simpup(UP,UP *);
void simpnum(Num,Num *);
void decompp(P,Q,P *,P *);
void truncp(P,Q,P *);
void uremp(P,P,P *);
void ugcdp(P,P,P *);
void reversep(P,Q,P *);
void invmodp(P,Q,P *);
void addup(UP,UP,UP *);
void subup(UP,UP,UP *);
void chsgnup(UP,UP *);
void mulup(UP,UP,UP *);
void tmulup(UP,UP,int,UP *);
void squareup(UP,UP *);
void remup(UP,UP,UP *);
void remup_destructive(UP,UP);
void qrup(UP,UP,UP *,UP *);
void qrup_destructive(UP,UP);
void gcdup(UP,UP,UP *);
void reverseup(UP,int,UP *);
void invmodup(UP,int,UP *);
void pwrup(UP,Q,UP *);
void squarep_gf2n(VL,P,P *);
void kmulp(VL,P,P,P *);
void ksquarep(VL,P,P *);
void kmulup(UP,UP,UP *);
void ksquareup(UP,UP *);
void extractup(UP,int,int,UP *);
void copyup(UP,UP);
void c_copyup(UP,int,pointer *);
void kmulupmain(UP,UP,UP *);
void ksquareupmain(UP,UP *);
void rembymulup(UP,UP,UP *);
void rembymulup_special(UP,UP,UP,UP *);
void tkmulup(UP,UP,int,UP *);
void shiftup(UP,int,UP *);
void set_degreeup(UP,int);
void decompup(UP,int,UP *,UP *);
void truncup(UP,int,UP *);
void uptofmarray(int,UP,ModNum *);
void fmarraytoup(ModNum *,int,UP *);
void uiarraytoup(unsigned int **,int,int,UP *);
void adj_coefup(UP,N,N,UP *);
void uptolmup(UP,UP *);
void remcup(UP,N,UP *);
void fft_mulup(UP,UP,UP *);
void fft_squareup(UP,UP *);
void trunc_fft_mulup(UP,UP,int,UP *);
void shoup_fft_mulup(UP,UP,UP *);
void shoup_fft_squareup(UP,UP *);
void shoup_trunc_fft_mulup(UP,UP,int,UP *);
void crup(ModNum **,int,int *,int,N,UP *);
void shoup_crup(ModNum **,int,int *,int,N,N,UP *);
void squareup_gf2n(UP,UP *);
void powermodup_gf2n(UP,UP *);
void generic_powermodup_gf2n(UP,UP,Q,UP *);
void tracemodup_gf2n(UP,UP,Q,UP *);
void tracemodup_gf2n_slow(UP,UP,Q,UP *);
void tracemodup_gf2n_tab(UP,UP,Q,UP *);
void square_rem_tab_up_gf2n(UP,UP *,UP *);
void powertabup_gf2n(UP,UP,UP *);
void find_root_gf2n(UP,GF2N *);

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
int compd(VL,DP,DP);
void adddl(int,DL,DL,DL *);
void divsdc(VL,DP,P,DP *);
void muldc(VL,DP,P,DP *);
void muldm(VL,DP,MP,DP *);
void muld(VL,DP,DP,DP *);
void chsgnd(DP,DP *);
void subd(VL,DP,DP,DP *);
void addd(VL,DP,DP,DP *);
int sugard(MP);
void nodetod(NODE,DP *);
void dtop(VL,VL,DP,P *);
void ptod(VL,VL,P,DP *);
void initd(struct order_spec *);

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
int n_bits(N);
void gcdBinary_27n(N,N,N *);
void rtime_init(void);
void setmod_gf2n(P);
void mt_sgenrand(unsigned long);
unsigned long mt_genrand(void);
#if defined(VISUAL)
void srandom(unsigned int);
unsigned int random(void);
#endif
void gcdbmodn(N,N,N *);
void gcdbinn(N,N,N *);
void gcdmodn(N,N,N *);
void gcdaccn(N,N,N *);
void gcdEuclidn(N,N,N *);
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
void binaryton(char *,N *);
void hexton(char *,N *);
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
void n32ton27(N,N *);
void n27ton32(N,N *);
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
void bshiftn(N,int,N *);
void remn(N,N,N*);
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
void kmuln(N,N,N *);
void extractn(N,int,int,N *);
void copyn(N,int,int *);
void kmulnmain(N,N,N *);
int qcoefp(Obj);
int qcoefr(Obj);
int get_allocwords(void);
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
void notdef(VL,Obj,Obj,Obj *);
void error(char *);
void ptoup2(P,UP2 *);
void ptoup2_sparse(P,UP2 *);
void up2top(UP2,P *);
void up2tovect(UP2,VECT *);
void up2ton(UP2,Q *);
void ntoup2(Q,UP2 *);
void gen_simpup2(UP2,GEN_UP2,UP2 *);
void gen_simpup2_destructive(UP2,GEN_UP2);
void gen_invup2(UP2,GEN_UP2,UP2 *);
void gen_pwrmodup2(UP2,Q,GEN_UP2,UP2 *);
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
void pwrmodup2(UP2,Q,UP2,UP2 *);
void pwrmodup2_sparse(UP2,Q,UP2,UP2 *);
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
Obj ToAlg(Num);
UM *berlemain(register int, UM, UM *);
void *Risa_GC_malloc(size_t);
void *Risa_GC_malloc_atomic(size_t);
void *Risa_GC_realloc(void *,size_t);
void *GC_malloc(size_t);
void *GC_malloc_atomic(size_t);
void *GC_realloc(void *,size_t);
double NatToReal(N,int *);
double RatnToReal(Q);
double pwrreal0(double,int);
double rtodbl(); /* XXX */
int **almat(int,int);
pointer **almat_pointer(int,int);
int berlecnt(register int,UM);
int berlecntmain(register int,int,int,register int **);
int cmpalg(Num,Num);
int cmpbf(Num,Num);
int cmpcplx(Num,Num);
int cmpn(N,N);
int cmpq(Q,Q);
int cmpreal(Real,Real);
int cmpmi(MQ,MQ);
int cmplm(LM,LM);
int compmat(VL,MAT,MAT);
int compnum(VL,Num,Num);
int compp(VL,P,P);
int compr(VL,Obj,Obj);
int compstr(VL,STRING,STRING);
int compvect(VL,VECT,VECT);
int ctest(P,ML,int,int *);
int cycchk(P);
int dbound(V,P);
int dcomp(P,P);
int deg(V,P);
int degtest(int,int *,ML,int);
int divcheck(VL,P *,int,P,P);
unsigned int divin(N,unsigned int,N *);
int divtdcpz(VL,P,P,P *);
int divtpz(VL,P,P,P *);
int divum(register int,UM,UM,UM);
int dm(int,int,int *);
int dmb(int,int,int,int *);
int dma(int,int,int,int *);
int dmab(int,int,int,int,int *);
int dmar(int,int,int,int);
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
unsigned int rem(N,unsigned int);
int sqfrchk(P);
int subn(N,N,N *);
int ucmpp(P,P);
int valideval(VL,DCP,VN);
int zerovpchk(VL,P,VN);

void addgf2n(GF2N,GF2N,GF2N *);
void subgf2n(GF2N,GF2N,GF2N *);
void mulgf2n(GF2N,GF2N,GF2N *);
void divgf2n(GF2N,GF2N,GF2N *);
void chsgngf2n(GF2N,GF2N *);
void pwrgf2n(GF2N,Q, GF2N *);
int cmpgf2n(GF2N,GF2N);

void addgfpn(GFPN,GFPN,GFPN *);
void subgfpn(GFPN,GFPN,GFPN *);
void mulgfpn(GFPN,GFPN,GFPN *);
void divgfpn(GFPN,GFPN,GFPN *);
void chsgngfpn(GFPN,GFPN *);
void pwrgfpn(GFPN,Q, GFPN *);
int cmpgfpn(GFPN,GFPN);

void addgfs(GFS,GFS,GFS *);
void subgfs(GFS,GFS,GFS *);
void mulgfs(GFS,GFS,GFS *);
void divgfs(GFS,GFS,GFS *);
void chsgngfs(GFS,GFS *);
void pwrgfs(GFS,Q, GFS *);
int cmpgfs(GFS,GFS);

void addalg(Num,Num,Num *);
void addbf(Num,Num,Num *);
void addcplx(Num,Num,Num *);
void addm2p(VL,Q,Q,P,P,P *);
void addm2q(Q,Q,Q,Q,Q *);
void addmat(VL,MAT,MAT,MAT *);
void addmp(VL,int,P,P,P *);
void addmpq(int,P,P,P *);
void addmptoc(VL,int,P,P,P *);
void addmq(int,MQ,MQ,MQ *);
void addn(N,N,N *);
void addnum(VL,Num,Num,Num *);
void addp(VL,P,P,P *);
void addpadic(int,int,unsigned int *,unsigned int *);
void addpq(P,P,P *);
void addptoc(VL,P,P,P *);
void addq(Q,Q,Q *);
void addr(VL,Obj,Obj,Obj *);
void addreal(Num,Num,Real *);
void addmi(MQ,MQ,MQ *);
void addlm(LM,LM,LM *);
void addstr(VL,STRING,STRING,STRING *);
void addum(int,UM,UM,UM);
void addvect(VL,VECT,VECT,VECT *);
void adjc(VL,P,P,P,Q,P *,P *);
void afctr(VL,P,P,DCP *);
void afctrmain(VL,P,P,int,DCP *);
void affine(VL,P,VN,P *);
void affinemain(VL,P,V,int,P *,P *);
void berle(int,int,P,ML *);
void bnton(register int,N,N *);
void cbound(VL,P,Q *);
void chnrem(int,V,P,Q,UM,P *,Q *);
void chnremp(VL,int,P,Q,P,P *);
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
void clctv(VL,P,VL *);
void clctvr(VL,Obj,VL *);
void cm2p(Q,Q,P,P *);
void cmax(P,Q *);
void cmp(Q,P,P *);
void coefp(P,int,P *);
void cpylum(int,LUM,LUM);
void cpyum(UM,UM);
void csump(VL,P,Q *);
void cycm(V,register int,DCP *);
void cycp(V,register int,DCP *);
void degp(V,P,Q *);
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
void divn(N,N,N *,N *);
void divnmain(int,int,unsigned int *,unsigned int *,unsigned int *);
void divnum(VL,Num,Num,Num *);
void divq(Q,Q,Q *);
void divr(VL,Obj,Obj,Obj *);
void divreal(Num,Num,Real *);
void divmi(MQ,MQ,MQ *);
void divlm(LM,LM,LM *);
void divsdcmp(VL,int,P,P,P *);
void divsdcp(VL,P,P,P *);
void divsmp(VL,int,P,P,P *);
void divsn(N,N,N *);
void divsp(VL,P,P,P *);
void divsrdcmp(VL,int,P,P,P *,P *);
void divsrdcp(VL,P,P,P *,P *);
void divsrmp(VL,int,P,P,P *,P *);
void divsrp(VL,P,P,P *,P *);
void divvect(VL,Obj,Obj,Obj *);
void dtest(P,ML,int,DCP *);
void dtestroot(int,int,P,LUM,struct oDUM *,DCP *);
void dtestroot1(int,int,P,LUM,P *);
void dtestsq(int,int,P,LUM,P *);
void dtestsql(P,ML,struct oDUM *,DCP *);
void estimatelc(VL,Q,DCP,VN,P *);
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
void factorial(int,Q *);
void fctrp(VL,P,DCP *);
void fctrwithmvp(VL,P,V,DCP *);
void gcda(VL,P,P,P,P *);
void gcdcp(VL,P,P *);
void gcdgen(P,ML,ML *);
void gcdmonomial(VL,DCP *,int,P *);
void gcdn(N,N,N *);
void gcdprsp(VL,P,P,P *);
void gcdum(register int,UM,UM,UM);
void getmindeg(V,P,Q *);
void henmain(LUM,ML,ML,ML *);
void henmv(VL,VN,P,P,P,P,P,P,P,P,P,Q,int,P *,P *);
void henmvmain(VL,VN,P,P,P,P,P,P,P,Q,Q,int,P *,P *);
void henprep(P,ML,ML,ML *,ML *);
void hensel(int,int,P,ML *);
void henzq(P,P,UM,P,UM,int,int,P *,P *,P *,P *,Q *);
void henzq1(P,P,Q,P *,P *,Q *);
void hsq(int,int,P,int *,DCP *);
void intersectv(VL,VL,VL *);
void invl(Q,Q,Q *);
void invmq(int,MQ,MQ *);
void invq(Q,Q *);
void lgp(P,N *,N *);
void lumtop(V,int,int,LUM,P *);
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
void mkbc(int,Q *);
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
void mulin(N,unsigned int,unsigned int *);
void mullum(int,int,LUM,LUM,LUM);
void mullumarray(P,ML,int,int *,P *);
void mulm2p(VL,Q,Q,P,P,P *);
void mulm2q(Q,Q,Q,Q,Q *);
void mulmat(VL,Obj,Obj,Obj *);
void mulmatmat(VL,MAT,MAT,MAT *);
void mulmatvect(VL,MAT,VECT,VECT *);
void mulmp(VL,int,P,P,P *);
void mulmpc(VL,int,P,P,P *);
void mulmpq(int,P,P,P *);
void mulmq(int,MQ,MQ,MQ *);
void muln(N,N,N *);
void mulnum(VL,Num,Num,Num *);
void mulp(VL,P,P,P *);
void mulpadic(int,int,unsigned int *,unsigned int *,unsigned int *);
void mulpc(VL,P,P,P *);
void mulpq(P,P,P *);
void mulq(Q,Q,Q *);
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
void nthrootchk(P,struct oDUM *,ML,DCP *);
void nthrootn(N,int,N *);
void ntobn(register int,N,N *);
void nuezgcdnpzmain(VL,P *,int,P *);
void padictoq(int,int,int *,Q *);
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
void pwrmp(VL,int,P,Q,P *);
void pwrmq(int,MQ,Q,MQ *);
void pwrn(N,int,N *);
void pwrnum(VL,Num,Num,Num *);
void pwrp(VL,P,Q,P *);
void pwrq(Q,Q,Q *);
void pwrr(VL,Obj,Obj,Obj *);
void pwrreal(Num,Num,Real *);
void pwrmi(MQ,Q,MQ *);
void pwrlm(LM,Q,LM *);
void pwrum(int,UM,int,UM);
void reallocarray(char **,int *,int *,int);
void reductr(VL,Obj,Obj *);
void reimtocplx(Num,Num,Num *);
void rem2q(Q,Q,Q,Q *);
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
void sqad(unsigned int,int);
void sqfrp(VL,P,DCP *);
void sqfrum(int,int,P,int *,struct oDUM **,ML *);
void sqfrummain(int,UM,UM,struct oDUM **);
void sqrtn(N,N *);
void srch2(VL,V,P,P,P *);
void srchmp(VL,int,V,P,P,P *);
void srchump(int,P,P,P *);
void srcr(VL,V,P,P,P *);
void strtobf(char *,BF *);
void subalg(Num,Num,Num *);
void subbf(Num,Num,Num *);
void subcplx(Num,Num,Num *);
void subm2p(VL,Q,Q,P,P,P *);
void subm2q(Q,Q,Q,Q,Q *);
void submat(VL,MAT,MAT,MAT *);
void submp(VL,int,P,P,P *);
void submq(int,MQ,MQ,MQ *);
void subnum(VL,Num,Num,Num *);
void subp(VL,P,P,P *);
void subq(Q,Q,Q *);
void subr(VL,Obj,Obj,Obj *);
void subreal(Num,Num,Real *);
void submi(MQ,MQ,MQ *);
void sublm(LM,LM,LM *);
void substmp(VL,int,P,V,P,P *);
void substp(VL,P,V,P,P *);
void substvp(VL,P,VN,P *);
void subum(int,UM,UM,UM);
void subvect(VL,VECT,VECT,VECT *);
void toreim(Num,Num *,Num *);
void ucsump(P,Q *);
void udivpwm(Q,P,P,P *,P *);
void udivpz(P,P,P *,P *);
void udivpzwm(Q,P,P,P *,P *);
void uexgcdnp(VL,P,P *,int,VN,Q,P *,P *,P *,P *,Q *);
void uezgcd1p(P,P,P *);
void uezgcdpp(DCP,P,P *);
void uezgcdpz(VL,P,P,P *);
void ufctr(P,int,DCP *);
void ufctrmain(P,int,DCP *);
void umtomp(V,UM,P *);
void umtop(V,UM,P *);
void usqp(P,DCP *);
void vntovl(VN,int,VL *);

#if 0 && !defined(VISUAL)
void bzero(const void *,int);
void bcopy(const void *,void *,int);
char *index(char *,int);
#endif
