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
 * $OpenXM: OpenXM_contrib2/asir2000/include/ca-27.h,v 1.7 2015/08/06 10:01:52 fujimoto Exp $ 
*/
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
/* data structures */

#define O_N 1

#define N_Q 0

struct oN {
	int p;
	int b[1];
};

typedef struct oN *N;

#if defined(VISUAL) || defined(__MINGW32__)
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
