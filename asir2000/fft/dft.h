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
 * $OpenXM: OpenXM_contrib2/asir2000/fft/dft.h,v 1.8 2018/03/29 01:32:53 noro Exp $ 
*/
#if SIZEOF_LONG == 8
typedef unsigned long UL;
#elif defined(HAVE_UNSIGNED_LONG_LONG)
typedef unsigned long long UL;
#elif defined(_MSC_VER)
typedef unsigned _int64 UL;
#endif

int dmar(int,int,int,int);

struct PrimesS {
  int  prime, primroot;
  int  bitwid, d;
};

#if defined(USE_FLOAT)
#define NPrimes 536
#else
#define NPrimes 13681
#endif

#if defined(USE_FLOAT)
typedef float ModNum;

#define AxBplusCmodP(ans,anstype,a,b,c,p,pinv) \
  { double _w_ = ((double)(a))*((double)(b))+((double)(c)), \
      _dp_ = (double)(p); \
      if ( _w_ >= _dp_ ) _w_ -= (_dp_*(double)((int)(_w_*(pinv)))); \
      ans = (anstype) (_w_>=_dp_?_w_-_dp_: _w_); }

#define AxBmodP(ans,anstype,a,b,p,pinv) \
  { double _w_ = ((double)(a))*((double)(b)), _dp_ = (double)(p); \
      _dp_ = (double)(p); \
      if ( _w_ >= _dp_ ) _w_ -= (_dp_*(double)((int)(_w_*(pinv)))); \
      ans = (anstype) (_w_>=_dp_?_w_-_dp_: _w_); }

#define AxBmodPnostrchk(ans,anstype,a,b,p,pinv) \
  { double _w_ = ((double)(a))*((double)(b)), _dp_ = (double)(p); \
      if ( _w_ >= _dp_ ) _w_ -= (_dp_*(double)((int)(_w_*(pinv)))); \
      ans = (anstype) _w_; }

#define AxBplusCmodPnostrchk(ans,anstype,a,b,c,p,pinv) \
  { double _w_ = ((double)(a))*((double)(b))+((double)(c)), \
      _dp_ = (double)(p); \
      if ( _w_ >= _dp_ ) _w_ -= (_dp_*(double)((int)(_w_*(pinv)))); \
      ans = (anstype) _w_; }
#else

#include "inline.h"

typedef unsigned int ModNum;

/* XXX */
#if defined(__DARWIN__)
#undef DMAR
#define DMAR(a1,a2,a3,d,r) (r)=dmar(a1,a2,a3,d);
#endif

#define AxBplusCmodP(ans,anstype,a,b,c,p,pinv) \
  { DMAR(a,b,c,p,ans) }

#define AxBmodP(ans,anstype,a,b,p,pinv) \
  { DMAR(a,b,0,p,ans) }

#define AxBmodPnostrchk(ans,anstype,a,b,p,pinv) \
  { DMAR(a,b,0,p,ans) }

#define AxBplusCmodPnostrchk(ans,anstype,a,b,c,p,pinv) \
  { DMAR(a,b,c,p,ans) }
#endif

#define AstrictmodP(a,p) ((a)>=(p)?((a)-(p)):(a))

#define AplusBmodP(a,b,p) (((p)-(b))>(a)?((a)+(b)): ((a)-((p)-(b))))
#define A_BmodP(a,b,p) ((a)>=(b)?((a)-(b)): (((p)-(b))+(a)))

void C_DFT_FORE(ModNum *,int,int,int,ModNum *,
#ifdef POWA_STRIDE
  int
#else
  ModNum *,int,ModNum,double,ModNum *
#endif
);

void C_DFT_BACK(ModNum *,int,int,int,ModNum *,
#ifdef POWA_STRIDE
     int
#else
     ModNum *,int,int,int,ModNum,ModNum,double,ModNum *
#endif
);

void C_PREP_ALPHA(ModNum,int,int,int,ModNum *,ModNum,double);
