/* $OpenXM: OpenXM/src/asir99/fft/dft.h,v 1.1.1.1 1999/11/10 08:12:27 noro Exp $ */
#if LONG_IS_32BIT
#if defined(VISUAL)
typedef unsigned _int64 UL;
#else
typedef unsigned long long UL;
#endif
#else
typedef unsigned long UL;
#endif

struct PrimesS {
	int	prime, primroot;
	int  bitwid, d;
};

#if USE_FLOAT
#define NPrimes 536
#else
#define NPrimes 13681
#endif

#if USE_FLOAT
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
