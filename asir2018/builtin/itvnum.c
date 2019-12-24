/*
 * $OpenXM: OpenXM_contrib2/asir2018/builtin/itvnum.c,v 1.5 2019/11/12 10:53:22 kondoh Exp $
 */

#include "ca.h"
#include "parse.h"
#include "version.h"
#if !defined(ANDROID)
#include "../plot/ifplot.h"
#endif

#include <mpfr.h>
#include <mpfi.h>
// in engine/bf.c
Num tobf(Num,int);

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
static void Pitvversion(NODE, Q *);
static void PzeroRewriteMode(NODE, Obj *);
static void PzeroRewriteCountClear(NODE, Obj *);
static void PzeroRewriteCount(NODE, Obj *);
//void miditvp(Itv,Num *);
//void absitvp(Itv,Num *);
//int initvd(Num,IntervalDouble);
//int initvp(Num,Itv);
//int itvinitvp(Itv,Itv);
static void Pevalitv(NODE, Obj *);
static void Pevalitvbf(NODE, Obj *);
static void Pevalitvd(NODE, Obj *);

static void Pitvbf_pi(NODE ,Obj *);
static void Pitvbf_e(NODE ,Obj *);
static void Pitvbf_sin(NODE ,Obj *);
static void Pitvbf_cos(NODE ,Obj *);
static void Pitvbf_tan(NODE ,Obj *);
static void Pitvbf_asin(NODE ,Obj *);
static void Pitvbf_acos(NODE ,Obj *);
static void Pitvbf_atan(NODE ,Obj *);
static void Pitvbf_sinh(NODE ,Obj *);
static void Pitvbf_cosh(NODE ,Obj *);
static void Pitvbf_tanh(NODE ,Obj *);
static void Pitvbf_asinh(NODE ,Obj *);
static void Pitvbf_acosh(NODE ,Obj *);
static void Pitvbf_atanh(NODE ,Obj *);
static void Pitvbf_exp(NODE ,Obj *);
static void Pitvbf_log(NODE ,Obj *);
static void Pitvbf_abs(NODE ,Obj *);
static void Pitvbf_pow(NODE ,Num *);

static void Pitvd_pi(NODE ,Obj *);
static void Pitvd_e(NODE ,Obj *);
static void Pitvd_sin(NODE ,Obj *);
static void Pitvd_cos(NODE ,Obj *);
static void Pitvd_tan(NODE ,Obj *);
static void Pitvd_asin(NODE ,Obj *);
static void Pitvd_acos(NODE ,Obj *);
static void Pitvd_atan(NODE ,Obj *);
static void Pitvd_sinh(NODE ,Obj *);
static void Pitvd_cosh(NODE ,Obj *);
static void Pitvd_tanh(NODE ,Obj *);
static void Pitvd_asinh(NODE ,Obj *);
static void Pitvd_acosh(NODE ,Obj *);
static void Pitvd_atanh(NODE ,Obj *);
static void Pitvd_exp(NODE ,Obj *);
static void Pitvd_log(NODE ,Obj *);
static void Pitvd_abs(NODE ,Obj *);
static void Pitvd_pow(NODE ,Num *);

static void Pitv_pi(NODE ,Obj *);
static void Pitv_e(NODE ,Obj *);
static void Pitv_sin(NODE ,Obj *);
static void Pitv_cos(NODE ,Obj *);
static void Pitv_tan(NODE ,Obj *);
static void Pitv_asin(NODE ,Obj *);
static void Pitv_acos(NODE ,Obj *);
static void Pitv_atan(NODE ,Obj *);
static void Pitv_sinh(NODE ,Obj *);
static void Pitv_cosh(NODE ,Obj *);
static void Pitv_tanh(NODE ,Obj *);
static void Pitv_asinh(NODE ,Obj *);
static void Pitv_acosh(NODE ,Obj *);
static void Pitv_atanh(NODE ,Obj *);
static void Pitv_exp(NODE ,Obj *);
static void Pitv_log(NODE ,Obj *);
static void Pitv_abs(NODE ,Obj *);
static void Pitv_pow(NODE ,Num *);
#endif
static void Pprintmode(NODE, Obj *);

/* plot time check func */
static void ccalc(double **, struct canvas *, int);
static void Pifcheck(NODE, Obj *);

#if  defined(__osf__) && 0
int  end;
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
  {"absitv",Pabsitv,1},
  {"disintval",Pdisjitv,2},
  {"inintval",Pinitv,2},
  {"cup",Pcup,2},
  {"cap",Pcap,2},
  {"mid",Pmid,1},
  {"width",Pwidth,1},
  {"diam",Pwidth,1},
  {"distance",Pdistance,2},
  {"iversion",Pitvversion,-1},
  {"intvalversion",Pitvversion,-1},
  {"zerorewritemode",PzeroRewriteMode,-1},
  {"zeroRewriteMode",PzeroRewriteMode,-1},
  {"zeroRewriteCountClear",PzeroRewriteCountClear,-1},
  {"zeroRewriteCount",PzeroRewriteCount,-1},
/* eval */
  {"evalitv", 	Pevalitv,	-2},
  {"evalitvbf",	Pevalitvbf,	-2},
  {"evalitvd",	Pevalitvd,	1},
/* math */

  {"piitv",		Pitv_pi,	-1},
  {"piitvbf",	Pitvbf_pi,	-1},
  {"piitvd",	Pitvd_pi,	-1},
  {"eitv",		Pitv_e,	-1},
  {"eitvbf",	Pitvbf_e,	-1},
  {"eitvd",		Pitvd_e,	-1},
#if 0
  {"factorialitv",Pfactorialitv,1},
  {"factorialitvd",Pfactorialitvd,1},

  {"absitv",	Pitv_abs,	-2},
  {"absitvbf",	Pitvbf_abs,	-2},
  {"absitvd",	Pitvd_abs,	-2},
#endif

  {"logitv",	Pitv_log,	-2},
  {"logitvbf",	Pitvbf_log,	-2},
  {"logitvd",	Pitvd_log,	-2},
  {"expitv",	Pitv_exp,	-2},
  {"expitvbf",	Pitvbf_exp,	-2},
  {"expitvd",	Pitvd_exp,	-2},
  {"powitv",	Pitv_pow,	-3},
  {"powitvbf",	Pitvbf_pow,	-3},
  {"powitvd",	Pitvd_pow,	-3},

  {"sinitv",	Pitv_sin,	-2},
  {"sinitvbf",	Pitvbf_sin,	-2},
  {"sinitvd",	Pitvd_sin,	-2},
  {"cositv",	Pitv_cos,	-2},
  {"cositvbf",	Pitvbf_cos,	-2},
  {"cositvd",	Pitvd_cos,	-2},
  {"tanitv",	Pitv_tan,	-2},
  {"tanitvbf",	Pitvbf_tan,	-2},
  {"tanitvd",	Pitvd_tan,	-2},
  {"asinitv",	Pitv_asin,	-2},
  {"asinitvbf",	Pitvbf_asin,	-2},
  {"asinitvd",	Pitvd_asin,		-2},
  {"acositv",	Pitv_acos,		-2},
  {"acositvbf",	Pitvbf_acos,	-2},
  {"acositvd",	Pitvd_acos,		-2},
  {"atanitv",	Pitv_atan,		-2},
  {"atanitvbf",	Pitvbf_atan,	-2},
  {"atanitvd",	Pitvd_atan,		-2},
  {"sinhitv",	Pitv_sinh,		-2},
  {"sinhitvbf",	Pitvbf_sinh,	-2},
  {"sinhitvd",	Pitvd_sinh,		-2},
  {"coshitv",	Pitv_cosh,		-2},
  {"coshitvbf",	Pitvbf_cosh,	-2},
  {"coshitvd",	Pitvd_cosh,		-2},
  {"tanhitv",	Pitv_tanh,		-2},
  {"tanhitvbf",	Pitvbf_tanh,	-2},
  {"tanhitvd",	Pitvd_tanh,		-2},
  {"asinhitv",	Pitv_asinh,		-2},
  {"asinhitvbf",	Pitvbf_asinh,	-2},
  {"asinhitvd",		Pitvd_asinh,	-2},
  {"acoshitv",		Pitv_acosh,		-2},
  {"acoshitvbf",	Pitvbf_acosh,	-2},
  {"acoshitvd",		Pitvd_acosh,	-2},
  {"atanhitv",		Pitv_atanh,		-2},
  {"atanhitvbf",	Pitvbf_atanh,	-2},
  {"atanhitvd",		Pitvd_atanh,	-2},

/* plot time check */
  {"ifcheck",Pifcheck,-7},
#endif
  {0,0,0},
};

extern int mpfr_roundmode;

#if defined(INTERVAL)

/* plot time check */
static void
Pifcheck(NODE arg, Obj *rp)
{
  Z m2,p2,s_id;
  NODE defrange;
  LIST xrange,yrange,range[2],list,geom;
  VL vl,vl0;
  V v[2],av[2];
  int ri,i,j,sign;
  P poly;
  P var;
  NODE n,n0;
  Obj t;

  struct canvas *can;
  MAT m;
  pointer **mb;
  double **tabe, *px, *px1, *px2;
  Z one;
  int width, height, ix, iy;
  int id;

  STOZ(-2,m2); STOZ(2,p2);
  STOZ(1,one);
  MKNODE(n,p2,0); MKNODE(defrange,m2,n);
  poly = 0; vl = 0; geom = 0; ri = 0;
  v[0] = v[1] = 0;
  for ( ; arg; arg = NEXT(arg) ){
    switch ( OID(BDY(arg)) ) {
    case O_P:
      poly = (P)BDY(arg);
      get_vars_recursive((Obj)poly,&vl);
      for(vl0=vl,i=0;vl0;vl0=NEXT(vl0)){
        if(vl0->v->attr==(pointer)V_IND){
          if(i>=2){
            error("ifplot : invalid argument");
          } else {
            v[i++]=vl0->v;
          }
        }
      }
      break;
    case O_LIST:
      list = (LIST)BDY(arg);
      if ( OID(BDY(BDY(list))) == O_P )
        if ( ri > 1 )
          error("ifplot : invalid argument");
        else
          range[ri++] = list;
      else
        geom = list;
      break;
    default:
      error("ifplot : invalid argument"); break;
    }
  }
  if ( !poly ) error("ifplot : invalid argument");
  switch ( ri ) {
    case 0:
      if ( !v[1] ) error("ifplot : please specify all variables");
      MKV(v[0],var); MKNODE(n,var,defrange); MKLIST(xrange,n);
      MKV(v[1],var); MKNODE(n,var,defrange); MKLIST(yrange,n);
      break;
    case 1:
      if ( !v[1] ) error("ifplot : please specify all variables");
      av[0] = VR((P)BDY(BDY(range[0])));
      if ( v[0] == av[0] ) {
        xrange = range[0];
        MKV(v[1],var); MKNODE(n,var,defrange); MKLIST(yrange,n);
      } else if ( v[1] == av[0] ) {
        MKV(v[0],var); MKNODE(n,var,defrange); MKLIST(xrange,n);
        yrange = range[0];
      } else
        error("ifplot : invalid argument");
      break;
    case 2:
      av[0] = VR((P)BDY(BDY(range[0])));
      av[1] = VR((P)BDY(BDY(range[1])));
      if ( ((v[0] == av[0]) && (!v[1] || v[1] == av[1])) ||
         ((v[0] == av[1]) && (!v[1] || v[1] == av[0])) ) {
          xrange = range[0]; yrange = range[1];
      } else error("ifplot : invalid argument");
      break;
    default:
      error("ifplot : cannot happen"); break;
  }
  can = canvas[id = search_canvas()];
  if ( !geom ) {
    width = 300;
    height = 300;
    can->width = 300;
    can->height = 300;
  } else {
    can->width = ZTOS((Z)BDY(BDY(geom)));
    can->height = ZTOS((Z)BDY(NEXT(BDY(geom))));
    width = can->width;
    height = can->height;
  }
  if ( xrange ) {
    n = BDY(xrange); can->vx = VR((P)BDY(n)); n = NEXT(n);
    can->qxmin = (Q)BDY(n); n = NEXT(n); can->qxmax = (Q)BDY(n);
    can->xmin = ToReal(can->qxmin); can->xmax = ToReal(can->qxmax); 
  }
  if ( yrange ) {
    n = BDY(yrange); can->vy = VR((P)BDY(n)); n = NEXT(n);
    can->qymin = (Q)BDY(n); n = NEXT(n); can->qymax = (Q)BDY(n);
    can->ymin = ToReal(can->qymin); can->ymax = ToReal(can->qymax); 
  }
  can->wname = "ifcheck";
  can->formula = poly;
  tabe = (double **)ALLOCA((width+1)*sizeof(double *));
  for ( i = 0; i <= width; i++ )
    tabe[i] = (double *)ALLOCA((height+1)*sizeof(double));
  for(i=0;i<=width;i++)for(j=0;j<=height;j++)tabe[i][j]=0;
  ccalc(tabe,can,0);
  MKMAT(m,width,height);
  mb = BDY(m);
  for( ix=0; ix<width; ix++ ){
    for( iy=0; iy<height; iy++){
      if ( tabe[ix][iy] >= 0 ){
        if ( (tabe[ix+1][iy] <= 0) ||
          (tabe[ix][iy+1] <= 0 ) ||
          (tabe[ix+1][iy+1] <= 0 ) ) mb[ix][iy] = (Obj)one;
      } else {
        if ( (tabe[ix+1][iy] >= 0 ) ||
          ( tabe[ix][iy+1] >= 0 ) ||
          ( tabe[ix+1][iy+1] >= 0 )) mb[ix][iy] = (Obj)one;
      }
    }
  }
  *rp = (Obj)m;
}

void ccalc(double **tab,struct canvas *can,int nox)
{
   double x,y,xmin,ymin,xstep,ystep;
   int ix,iy;
   Real r,rx,ry;
   Obj fr,g;
   int w,h;
   V vx,vy;
   Obj t,s;

   MKReal(1.0,r); mulr(CO,(Obj)can->formula,(Obj)r,&fr);
   vx = can->vx;
   vy = can->vy;
   w = can->width; h = can->height; 
   xmin = can->xmin; xstep = (can->xmax-can->xmin)/w;
   ymin = can->ymin; ystep = (can->ymax-can->ymin)/h;
   MKReal(1.0,rx); MKReal(1.0,ry);
   for( ix = 0, x = xmin; ix < w+1 ; ix++, x += xstep ) {
      BDY(rx) = x; substr(CO,0,fr,vx,x?(Obj)rx:0,&t);
      devalr(CO,t,&g);
      for( iy = 0, y = ymin; iy < h+1 ; iy++, y += ystep ) {
         BDY(ry) = y;
         substr(CO,0,g,vy,y?(Obj)ry:0,&t);
         devalr(CO,t,&s);
         tab[ix][iy] = ToReal(s);
      }
   }
}
/* end plot time check */

static void
Pitvversion(NODE arg, Q *rp)
{
	Z r;
  STOZ(INT_ASIR_VERSION, r);
	*rp = (Q)r;
}

extern int  bigfloat;

static void
Pitv(NODE arg, Obj *rp)
{
  Num  a, i, s;
  Itv  c;
  double  inf, sup;

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
  Num  a, i, s;
  IntervalBigFloat  c;
  Num  ii,ss;
  Real di, ds;
  double  inf, sup;
  int current_roundmode;

  asir_assert(ARG0(arg),O_N,"intvalbf");
  a = (Num)ARG0(arg);
  if ( argc(arg) > 1 ) {
    asir_assert(ARG1(arg),O_N,"intvalbf");

    i = (Num)ARG0(arg);
    s = (Num)ARG1(arg);
    current_roundmode = mpfr_roundmode;
    mpfr_roundmode = MPFR_RNDD;
    ii = tobf(i, DEFAULTPREC);
    mpfr_roundmode = MPFR_RNDU;
    ss = tobf(s, DEFAULTPREC);
    istoitv(ii,ss,(Itv *)&c);
//    MKIntervalBigFloat((BF)ii,(BF)ss,c);
//    ToBf(s, &ss);
    mpfr_roundmode = current_roundmode;
  } else {
    if ( ! a ) {
      *rp = 0;
      return;
    }
    else if ( NID(a) == N_IP ) {
      itvtois((Itv)a, &i, &s);
      current_roundmode = mpfr_roundmode;
      mpfr_roundmode = MPFR_RNDD;
      ii = tobf(i, DEFAULTPREC);
      mpfr_roundmode = MPFR_RNDU;
      ss = tobf(s, DEFAULTPREC);
      istoitv(ii,ss,(Itv *)&c);
//      MKIntervalBigFloat((BF)ii,(BF)ss,c);
      mpfr_roundmode = current_roundmode;
    }
    else if ( NID(a) == N_IntervalBigFloat) {
      *rp = (Obj)a;
      return;
    }
    else if ( NID(a) == N_IntervalDouble ) {
      inf = INF((IntervalDouble)a);
      sup = SUP((IntervalDouble)a);
      current_roundmode = mpfr_roundmode;
      //double2bf(inf, (BF *)&i);
      //double2bf(sup, (BF *)&s);
      mpfr_roundmode = MPFR_RNDD;
      MKReal(inf,di);
      ii = tobf((Num)di, DEFAULTPREC);
      mpfr_roundmode = MPFR_RNDU;
      MKReal(sup,ds);
      ss = tobf((Num)ds, DEFAULTPREC);
      istoitv(ii,ss,(Itv *)&c);
//      MKIntervalBigFloat((BF)ii,(BF)ss,c);
      mpfr_roundmode = current_roundmode;
    }
    else {
      current_roundmode = mpfr_roundmode;
      mpfr_roundmode = MPFR_RNDD;
      ii = tobf(a, DEFAULTPREC);
      mpfr_roundmode = MPFR_RNDU;
      ss = tobf(a, DEFAULTPREC);
      //ToBf(a, (BF *)&i);
      istoitv(ii,ss,(Itv *)&c);
//      MKIntervalBigFloat((BF)ii,(BF)ss,c);
      mpfr_roundmode = current_roundmode;
    }
  }
//  if ( c && OID( c ) == O_N && NID( c ) == N_IntervalBigFloat )
//    addulp((IntervalBigFloat)c, (IntervalBigFloat *)rp);
//  else *rp = (Obj)c;
  *rp = (Obj)c;
}

static void
Pitvd(NODE arg, Obj *rp)
{
  double  inf, sup;
  Num  a, a0, a1, t;
  Itv  ia;
  IntervalDouble  d;

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
  inf = toRealDown(a0);
  sup = toRealUp(a1);
  MKIntervalDouble(inf,sup,d);
  *rp = (Obj)d;
}

static void
Pinf(NODE arg, Obj *rp)
{
  Num  a, i, s;
  Real  r;
  double  d;

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
  Num  a, i, s;
  Real  r;
  double  d;

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
  Num  a, s;
  Real  r;
  double  d;

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
  Itv  s;
  Num  a, b;

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
  Itv  s;
  Num  a, b;

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
  Num  s;
  Num  a;

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
  Num  s;
  Num  a, b;

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
  Num  s;
  Num  a, b;

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
Pinitv(NODE arg, Obj *rp)
{
  int  s;
  Z  q;

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
  STOZ(s,q);
  *rp = (Obj)q;
}

static void
Pdisjitv(arg,rp)
NODE arg;
Obj *rp;
{
  Itv  s;

  asir_assert(ARG0(arg),O_N,"disjitv");
  asir_assert(ARG1(arg),O_N,"disjitv");
  error("disjitv: not implemented yet");
  if ( ! s ) *rp = 0;
  else *rp = (Obj)ONE;
}

static void
PzeroRewriteMode(NODE arg, Obj *rp)
{
  Q  a;
  Z r;

  STOZ(zerorewrite,r);
  *rp = (Obj)r;
 
  if (arg) {
    a = (Q)ARG0(arg);
    if(!a) {
      zerorewrite = 0;
    } else if ( (NUM(a)&&INT(a)) ){
      zerorewrite = 1;
    }
  }
}
  
static void
PzeroRewriteCountClear(NODE arg, Obj *rp)
{
  Q  a;
  Z  r;

  STOZ(zerorewriteCount,r);
  *rp = (Obj)r;

  if (arg) {
    a = (Q)ARG0(arg);
    if(a &&(NUM(a)&&INT(a))){
      zerorewriteCount = 0;
    }
  }
}
  
static void
PzeroRewriteCount(NODE arg, Obj *rp)
{
  Z  r;

  STOZ(zerorewriteCount,r);
  *rp = (Obj)r;
}
  

#endif
extern int  printmode;

static void  pprintmode( void )
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
  int  l;
  Z  a, r;

  a = (Z)ARG0(arg);
  if(!a||(NUM(a)&&INT(a))){
    l=ZTOS(a);
    if ( l < 0 ) l = 0;
#if defined(INTERVAL)
    else if ( l > MID_PRINTF_E ) l = 0;
#else
    else if ( l > PRINTF_E ) l = 0;
#endif
    STOZ(printmode,r);
    *rp = (Obj)r;
    printmode = l;
    pprintmode();
  } else {
    *rp = 0;
  }
}

#if defined(INTERVAL)
void 
Ppi_itvd(NODE arg, Obj *rp)
{
	double inf, sup;
	IntervalDouble c;
    FPMINUSINF
	sscanf("3.1415926535897932384626433832795028841971693993751", "%lf", &inf);
    FPPLUSINF
    sscanf("3.1415926535897932384626433832795028841971693993752", "%lf", &sup);
    FPNEAREST
    MKIntervalDouble(inf,sup,c);
	*rp = (Obj)c;
}
void 
Pe_itvd(NODE arg, Obj *rp)
{
	double inf, sup;
	IntervalDouble c;
    FPMINUSINF
	sscanf( "2.7182818284590452353602874713526624977572470936999", "%lf", &inf);
    FPPLUSINF
    sscanf( "2.7182818284590452353602874713526624977572470937000", "%lf", &sup);
    FPNEAREST
    MKIntervalDouble(inf,sup,c);
	*rp = (Obj)c;
}
void 
Pln2_itvd(NODE arg, Obj *rp)
{
	double inf, sup;
	IntervalDouble c;
    FPMINUSINF
	sscanf( "0.69314718055994530941723212145817656807550013436025", "%lf", &inf);
    FPPLUSINF
    sscanf( "0.69314718055994530941723212145817656807550013436026", "%lf", &sup);
    FPNEAREST
    MKIntervalDouble(inf,sup,c);
	*rp = (Obj)c;
}

void mpfi_func(NODE arg, int (*mpfi_f)(), int prec, Obj *rp)
{
  Num a, ii, ss;
  Itv c;
  BF inf, sup;
  int arg1prec;
  mpfi_t mpitv, rv;


/*  
  if ( argc(arg) == 2 ) {
    prec = QTOS((Q)ARG1(arg))*3.32193;
    if ( prec < MPFR_PREC_MIN ) prec = MPFR_PREC_MIN;
    else if ( prec > MPFR_PREC_MAX ) prec = MPFR_PREC_MAX;
  } else {
    prec = 0;
    prec = mpfr_get_default_prec();
  }
*/
  if ( prec > 0 ) arg1prec = prec;
  else arg1prec = NEXT(arg) ? ZTOS((Q)ARG1(arg)) : mpfr_get_default_prec();
  a = ARG0(arg);
  itvtois((Itv)a, &ii, &ss);

  inf = (BF)tobf(ii, arg1prec);
  sup = (BF)tobf(ss, arg1prec);
  
  mpfi_init2(rv,arg1prec);
  mpfi_init2(mpitv,arg1prec);
  mpfr_set(&(mpitv->left),  BDY(inf), MPFR_RNDD);
  mpfr_set(&(mpitv->right), BDY(sup), MPFR_RNDU);

  (*mpfi_f)(rv, mpitv);
  //mpfi_sin(rv, mpitv);

  MPFRTOBF(&(rv->left), inf);
  MPFRTOBF(&(rv->right), sup);

  if ( !cmpbf((Num)inf,0) ) inf = 0;
  if ( !cmpbf((Num)sup,0) ) sup = 0;          

  if ( inf || sup ) {
  	istoitv((Num)inf, (Num)sup, &c);
  } else {
    c = 0;
  }
  *rp = (Obj)c;
  //mpfi_clear(rv);
  mpfi_clear(mpitv);
}

void mpfi_func_d(NODE arg, int (*mpfi_f)(), Obj *rp)
{
  Obj bfv;
  Num ii, ss;
  IntervalDouble c;
  double inf, sup;

  mpfi_func(arg, mpfi_f, 53, &bfv);
  itvtois((Itv)bfv, &ii, &ss);
  inf = toRealDown(ii);
  sup = toRealUp(ss);
  MKIntervalDouble(inf,sup,c);
  *rp = (Obj)c;
}

  
void
Psinitvd(NODE arg, Obj *rp)
{
  Obj bfv;
  Num ii,ss;
  IntervalDouble c;
  double  ai,as,mas, bi,bs;
  double  inf,sup;

  mpfi_func(arg, mpfi_sin, 53, &bfv);
  itvtois((Itv)bfv, &ii, &ss);
  inf = toRealDown(ii);
  sup = toRealUp(ss);
  MKIntervalDouble(inf,sup,c);
  *rp = (Obj)c;
}

static
void
Psinitv(NODE arg, Obj *rp)
{
  mpfi_func(arg, mpfi_sin, 0, rp);
}




//void evalitvr(VL, Obj, int, int, Obj *);

static void
Pevalitv(NODE arg, Obj *rp)
{
  if ( bigfloat )
	Pevalitvbf(arg, rp);
  else
	Pevalitvd(arg, rp);
}

static void
Pevalitvbf(NODE arg, Obj *rp)
{
  int prec;

  asir_assert(ARG0(arg),O_R,"evalitv");
  if ( argc(arg) == 2 ) {
    long int mpfr_prec_max = MPFR_PREC_MAX;
    prec = ZTOS((Q)ARG1(arg))*3.32193;
    if ( prec < MPFR_PREC_MIN ) prec = MPFR_PREC_MIN;
    //else if ( prec > MPFR_PREC_MAX ) prec = MPFR_PREC_MAX;
    else if ( prec > mpfr_prec_max ) prec = mpfr_prec_max;
  } else
    prec = 0;
  evalitvr(CO,(Obj)ARG0(arg),prec,EvalIntervalBigFloat,rp);
}

static void
Pevalitvd(NODE arg, Obj *rp)
{
  asir_assert(ARG0(arg),O_R,"evalitvd");
  evalitvr(CO,(Obj)ARG0(arg),53,EvalIntervalDouble,rp);
}

// in parse/puref.c
void instoobj(PFINS ins,Obj *rp);

// in this
void evalitvr(VL, Obj, int, int, Obj *);
void evalitvp(VL,   P, int, int, P *);
void evalitvv(VL,   V, int, int, Obj *);
void evalitvins(PFINS, int, int, Obj *);



void evalitvr(VL vl,Obj a,int prec, int type, Obj *c)
{
  Obj nm,dn;

  if ( !a )
    *c = 0;
  else {
    switch ( OID(a) ) {
      case O_N:
        toInterval((Num)a, prec, type, (Num *)c);
		break;
      case O_P:
        evalitvp(vl,(P)a,prec,type,(P *)c);
		break;
      case O_R:  
        evalitvp(vl,NM((R)a),prec,type,(P *)&nm);
		evalitvp(vl,DN((R)a),prec,type,(P *)&dn);
        divr(vl,nm,dn,c);
        break;
      default:
        error("evalr : not implemented"); break;
    }
  }
}

void evalitvp(VL vl,P p,int prec, int type, P *pr)
{
  P t;
  DCP dc,dcr0,dcr;
  Obj u;

  if ( !p || NUM(p) ) {
        toInterval((Num)p, prec, type, (Num *)pr);
    //*pr = p;
  } else {
    for ( dcr0 = 0, dc = DC((P)p); dc; dc = NEXT(dc) ) {
      evalitvp(vl,COEF(dc),prec,type, &t);
      if ( t ) {
        NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
      }
    }
    if ( !dcr0 ) {
      *pr = 0; return;
    } else {
      NEXT(dcr) = 0; MKP(VR(p),dcr0,t);
    }
    if ( NUM(t) ) {
      //*pr = t;
        toInterval((Num)t, prec, type, (Num *)pr);
		return;
	} else if ( (VR(t) != VR(p)) || ((vid)VR(p)->attr != V_PF) ) {
      *pr = t; return;
    } else {
    	evalitvv(vl,VR(p),prec,type,&u);
		substr(vl,1,(Obj)t,VR(p),u, (Obj *)&t);
		if ( t && NUM(t) ) {
        	toInterval((Num)t, prec, type, (Num *)pr);
		} else {
			*pr = t;
		}
    }
  }
}

void evalitvv(VL vl,V v,int prec, int type, Obj *rp)
{
  PFINS ins,tins;
  PFAD ad,tad;
  PF pf;
  P t;
  int i;

  if ( (vid)v->attr != V_PF ) {
    MKV(v,t); *rp = (Obj)t;
  } else {
    ins = (PFINS)v->priv; ad = ins->ad; pf = ins->pf;
    tins = (PFINS)CALLOC(1,sizeof(PF)+pf->argc*sizeof(struct oPFAD));
    tins->pf = pf;
    for ( i = 0, tad = tins->ad; i < pf->argc; i++ ) {
      tad[i].d = ad[i].d;
		evalitvr(vl,ad[i].arg,prec,type,&tad[i].arg);
    }
    evalitvins(tins,prec,type,rp);
  }
}

void evalitvins(PFINS ins,int prec, int type, Obj *rp)
{
  PF pf;
  PFINS tins;
  PFAD ad,tad;
  int i;
  Z q;
  V v;
  P x;
  NODE n0,n;


  pf = ins->pf; ad = ins->ad;
  tins = (PFINS)CALLOC(1,sizeof(PF)+pf->argc*sizeof(struct oPFAD));
  tins->pf = pf; tad = tins->ad;
  for ( i = 0; i < pf->argc; i++ ) {
    //tad[i].d = ad[i].d; evalr(CO,ad[i].arg,prec,&tad[i].arg);
    tad[i].d = ad[i].d; evalitvr(CO,ad[i].arg,prec,type,&tad[i].arg);
   }
  for ( i = 0; i < pf->argc; i++ )
    if ( tad[i].d || (tad[i].arg && !NUM(tad[i].arg)) ) break;
  if ( (i != pf->argc) || !pf->intervalfunc[type] ) { ///////////////////////////
    instoobj(tins,rp);
  } else {
	int IsCPLX = 0;
    for ( n0 = 0, i = 0; i < pf->argc; i++ ) {
      NEXTNODE(n0,n); BDY(n) = (pointer)tad[i].arg;
		if (tad[i].arg && NID(tad[i].arg) == N_C) IsCPLX = 1;
    }
    if ( prec ) {
      NEXTNODE(n0,n); STOZ(prec,q); BDY(n) = (pointer)q;
    }
    if ( n0 ) NEXT(n) = 0;


	if ( IsCPLX ) {
    	instoobj(tins,rp);
	} else {
    	(*pf->intervalfunc[type])(n0,rp);
	}
  }
}


static
void Pitvbf_pi(NODE arg, Obj *rp)
{
  BF inf, sup;
  IntervalBigFloat c;
  mpfi_t rv;
  int prec;

  prec = (arg) ? ZTOS((Q)ARG0(arg)) : 0; //mpfr_get_default_prec();
  prec ? mpfi_init2(rv,prec) : mpfi_init(rv);

  mpfi_const_pi(rv);

  MPFRTOBF(&(rv->left), inf);
  MPFRTOBF(&(rv->right), sup);

  MKIntervalBigFloat(inf,sup,c);

  *rp = (Obj)c;
}

static
void Pitvd_pi(NODE arg, Obj *rp)
{
  BF bfinf, bfsup;
  IntervalDouble c;
  mpfi_t rv;
  double inf, sup;

  mpfi_init2(rv,53);

  mpfi_const_pi(rv);

  MPFRTOBF(&(rv->left),  bfinf);
  MPFRTOBF(&(rv->right), bfsup);

  inf = toRealDown((Num)bfinf);
  sup = toRealUp((Num)bfsup);

  MKIntervalDouble(inf,sup,c);

  *rp = (Obj)c;
}

static
void Pitvbf_e(NODE arg,Obj *rp)
{
  BF inf, sup;
  IntervalBigFloat c;
  mpfi_t rv;
  mpfi_t one;
  int prec;

  prec = (arg) ? ZTOS((Q)ARG0(arg)) : 0; //mpfr_get_default_prec();
  prec ? mpfi_init2(rv,prec) : mpfi_init(rv);

  mpfi_init(one);
  mpfi_set_ui(one,1);
  mpfi_exp(rv,one);

  MPFRTOBF(&(rv->left), inf);
  MPFRTOBF(&(rv->right), sup);

  MKIntervalBigFloat(inf,sup,c);
  //istoitv((Num)inf, (Num)sup, &c);
  *rp = (Obj)c;
  mpfi_clear(one);
}

static
void Pitvd_e(NODE arg, Obj *rp)
{
  BF bfinf, bfsup;
  IntervalDouble c;
  mpfi_t rv;
  mpfi_t one;
  double inf, sup;

  mpfi_init2(rv,53);

  mpfi_init2(one, 53);
  mpfi_set_ui(one,1);
  mpfi_exp(rv,one);


  MPFRTOBF(&(rv->left),  bfinf);
  MPFRTOBF(&(rv->right), bfsup);

  inf = toRealDown((Num)bfinf);
  sup = toRealUp((Num)bfsup);

  MKIntervalDouble(inf,sup,c);

  *rp = (Obj)c;
}

void (*pi_itv_ft[])() =		{Pitvd_pi,		0,	Pitvbf_pi};
void (*e_itv_ft[])() =		{Pitvd_e,		0,	Pitvbf_e};
//void (*sin_itv_ft[])() =	{Psinitvd,	0,	Psinitv};
void (*sin_itv_ft[])() =	{Pitvd_sin,		0,	Pitvbf_sin};
void (*cos_itv_ft[])() =	{Pitvbf_cos,	0,	Pitvbf_cos};
void (*tan_itv_ft[])() =	{Pitvd_tan,		0,	Pitvbf_tan};
void (*asin_itv_ft[])() =	{Pitvd_asin,	0,	Pitvbf_asin};
void (*acos_itv_ft[])() =	{Pitvd_acos,	0,	Pitvbf_acos};
void (*atan_itv_ft[])() =	{Pitvd_atan,	0,	Pitvbf_atan};
void (*sinh_itv_ft[])() =	{Pitvd_sinh,	0,	Pitvbf_sinh};
void (*cosh_itv_ft[])() =	{Pitvd_cosh,	0,	Pitvbf_cosh};
void (*tanh_itv_ft[])() =	{Pitvd_tanh,	0,	Pitvbf_tanh};
void (*asinh_itv_ft[])() =	{Pitvd_asinh,	0,	Pitvbf_asinh};
void (*acosh_itv_ft[])() =	{Pitvd_acosh,	0,	Pitvbf_acosh};
void (*atanh_itv_ft[])() =	{Pitvd_atanh,	0,	Pitvbf_atanh};
void (*exp_itv_ft[])() =	{Pitvd_exp,		0,	Pitvbf_exp};
void (*log_itv_ft[])() =	{Pitvd_log,		0,	Pitvbf_log};
void (*abs_itv_ft[])() =	{0};
void (*pow_itv_ft[])() =	{Pitvbf_pow,	0,	Pitvbf_pow};
//void (*pow_itv_ft[])() =	{0,	0,	0};       


static
void Pitvbf_sin(NODE arg,Obj *rp)
{
  mpfi_func(arg, mpfi_sin, 0, rp);
}

static
void Pitvbf_cos(NODE arg,Obj *rp)
{
  mpfi_func(arg, mpfi_cos, 0, rp);
}

static
void Pitvbf_tan(NODE arg,Obj *rp)
{
  mpfi_func(arg, mpfi_tan, 0, rp);
}

static
void Pitvbf_asin(NODE arg,Obj *rp)
{
  mpfi_func(arg, mpfi_asin, 0, rp);
}

static
void Pitvbf_acos(NODE arg,Obj *rp)
{
  mpfi_func(arg, mpfi_acos, 0, rp);
}

static
void Pitvbf_atan(NODE arg,Obj *rp)
{
  mpfi_func(arg, mpfi_atan, 0, rp);
}

static
void Pitvbf_sinh(NODE arg,Obj *rp)
{
  mpfi_func(arg, mpfi_sinh, 0, rp);
}

static
void Pitvbf_cosh(NODE arg,Obj *rp)
{
  mpfi_func(arg, mpfi_cosh, 0, rp);
}

static
void Pitvbf_tanh(NODE arg,Obj *rp)
{
  mpfi_func(arg, mpfi_tanh, 0, rp);
}

static
void Pitvbf_asinh(NODE arg,Obj *rp)
{
  mpfi_func(arg, mpfi_asinh, 0, rp);
}

static
void Pitvbf_acosh(NODE arg,Obj *rp)
{
  mpfi_func(arg, mpfi_acosh, 0, rp);
}

static
void Pitvbf_atanh(NODE arg,Obj *rp)
{
  mpfi_func(arg, mpfi_atanh, 0, rp);
}

static
void Pitvbf_exp(NODE arg,Obj *rp)
{
  mpfi_func(arg, mpfi_exp, 0, rp);
}

static
void Pitvbf_log(NODE arg,Obj *rp)
{
  mpfi_func(arg, mpfi_log, 0, rp);
}

static
void Pitvbf_abs(NODE arg,Obj *rp)
{
  mpfi_func(arg, mpfi_abs, 0, rp);
}

static
void Pitvd_sin(NODE arg,Obj *rp)
{
  mpfi_func_d(arg, mpfi_sin, rp);
}

static
void Pitvd_cos(NODE arg,Obj *rp)
{
  mpfi_func_d(arg, mpfi_cos, rp);
}

static
void Pitvd_tan(NODE arg,Obj *rp)
{
  mpfi_func_d(arg, mpfi_tan, rp);
}

static
void Pitvd_asin(NODE arg,Obj *rp)
{
  mpfi_func_d(arg, mpfi_asin, rp);
}

static
void Pitvd_acos(NODE arg,Obj *rp)
{
  mpfi_func_d(arg, mpfi_acos, rp);
}

static
void Pitvd_atan(NODE arg,Obj *rp)
{
  mpfi_func_d(arg, mpfi_atan, rp);
}

static
void Pitvd_sinh(NODE arg,Obj *rp)
{
  mpfi_func_d(arg, mpfi_sinh, rp);
}

static
void Pitvd_cosh(NODE arg,Obj *rp)
{
  mpfi_func_d(arg, mpfi_cosh, rp);
}

static
void Pitvd_tanh(NODE arg,Obj *rp)
{
  mpfi_func_d(arg, mpfi_tanh, rp);
}

static
void Pitvd_asinh(NODE arg,Obj *rp)
{
  mpfi_func_d(arg, mpfi_asinh, rp);
}

static
void Pitvd_acosh(NODE arg,Obj *rp)
{
  mpfi_func_d(arg, mpfi_acosh, rp);
}

static
void Pitvd_atanh(NODE arg,Obj *rp)
{
  mpfi_func_d(arg, mpfi_atanh, rp);
}

static
void Pitvd_exp(NODE arg,Obj *rp)
{
  mpfi_func_d(arg, mpfi_exp, rp);
}

static
void Pitvd_log(NODE arg,Obj *rp)
{
  mpfi_func_d(arg, mpfi_log, rp);
}

static
void Pitvd_abs(NODE arg,Obj *rp)
{
  mpfi_func_d(arg, mpfi_abs, rp);
}

/*
void mp_factorial(NODE arg,Num *rp)
{
  struct oNODE arg0;
  Num a,a1;

  a = (Num)ARG0(arg);
  if ( !a ) *rp = (Num)ONE;
  else if ( INT(a) ) Pfac(arg,rp);
  else {
    addnum(0,a,(Num)ONE,&a1);
    arg0.body = (pointer)a1;
    arg0.next = arg->next;
    Pmpfr_gamma(&arg0,rp);
  }
}
*/
static
void Pitvd_pow(NODE arg,Num *rp)
{
  Num ii, ss;
  IntervalDouble c;
  Num rpbf;
  double inf, sup;

  Pitvbf_pow(arg, &rpbf);
  itvtois((Itv)rpbf, &ii, &ss);
  inf = toRealDown(ii);
  sup = toRealUp(ss);
  MKIntervalDouble(inf,sup,c);
  *rp = (Num)c;
}

static
void Pitvbf_pow(NODE arg,Num *rp)
{
  Num a,e;
  BF r,re,im;
  C c;
  mpc_t mpc,a1,e1;
  int prec;

  prec = NEXT(NEXT(arg)) ? ZTOS((Q)ARG2(arg)) : mpfr_get_default_prec();
  a = ARG0(arg);
  e = ARG1(arg);
  if ( !e ) {
    *rp = (Num)ONE;
  } else if ( !a ) {
    *rp = 0;
  } else if ( NID(a) == N_C || NID(e) == N_C ) {
    error("itv_pow() : not support args");
    *rp = 0;
  } else {
    Num ii, ss;
    Itv c;
    BF inf, sup;
    mpfi_t a_val, e_val, rv;

    mpfi_init2(rv,prec);

    itvtois((Itv)a, &ii, &ss);
    inf = (BF)tobf(ii, prec);
    sup = (BF)tobf(ss, prec);
    mpfi_init2(a_val,prec);
    mpfr_set(&(a_val->left),  BDY(inf), MPFR_RNDD);
    mpfr_set(&(a_val->right), BDY(sup), MPFR_RNDU);

    itvtois((Itv)e, &ii, &ss);
    inf = (BF)tobf(ii, prec);
    sup = (BF)tobf(ss, prec);
    mpfi_init2(e_val,prec);
    mpfr_set(&(e_val->left),  BDY(inf), MPFR_RNDD);
    mpfr_set(&(e_val->right), BDY(sup), MPFR_RNDU);


    mpfi_log(rv, a_val);
    mpfi_mul(a_val, rv, e_val);
    mpfi_exp(rv, a_val);

    MPFRTOBF(&(rv->left), inf);
    MPFRTOBF(&(rv->right), sup);
 
    if ( !cmpbf((Num)inf,0) ) inf = 0;
    if ( !cmpbf((Num)sup,0) ) sup = 0;

    if ( inf || sup ) {
      istoitv((Num)inf, (Num)sup, &c);
    } else {
      c = 0;
    }
    *rp = (Num)c;
  //mpfi_clear(rv);
    mpfi_clear(a_val);
    mpfi_clear(e_val);
  }
}

static void Pitv_pi(NODE arg, Obj *rp)
{
  if ( bigfloat )
	Pitvbf_pi(arg, rp);
  else
	Pitvd_pi(arg, rp);
}

static void Pitv_e(NODE arg, Obj *rp)
{
  if ( bigfloat )
	Pitvbf_e(arg, rp);
  else
	Pitvd_e(arg, rp);
}

static void Pitv_sin(NODE arg, Obj *rp)
{
  if ( bigfloat )
	Pitvbf_sin(arg, rp);
  else
	Pitvd_sin(arg, rp);
}

static void Pitv_cos(NODE arg, Obj *rp)
{
  if ( bigfloat )
	Pitvbf_cos(arg, rp);
  else
	Pitvd_cos(arg, rp);
}

static void Pitv_tan(NODE arg, Obj *rp)
{
  if ( bigfloat )
	Pitvbf_tan(arg, rp);
  else
	Pitvd_tan(arg, rp);
}

static void Pitv_asin(NODE arg, Obj *rp)
{
  if ( bigfloat )
	Pitvbf_asin(arg, rp);
  else
	Pitvd_asin(arg, rp);
}

static void Pitv_acos(NODE arg, Obj *rp)
{
  if ( bigfloat )
	Pitvbf_acos(arg, rp);
  else
	Pitvd_acos(arg, rp);
}

static void Pitv_atan(NODE arg, Obj *rp)
{
  if ( bigfloat )
	Pitvbf_atan(arg, rp);
  else
	Pitvd_atan(arg, rp);
}

static void Pitv_sinh(NODE arg, Obj *rp)
{
  if ( bigfloat )
	Pitvbf_sinh(arg, rp);
  else
	Pitvd_sinh(arg, rp);
}

static void Pitv_cosh(NODE arg, Obj *rp)
{
  if ( bigfloat )
	Pitvbf_cosh(arg, rp);
  else
	Pitvd_cosh(arg, rp);
}

static void Pitv_tanh(NODE arg, Obj *rp)
{
  if ( bigfloat )
	Pitvbf_tanh(arg, rp);
  else
	Pitvd_tanh(arg, rp);
}

static void Pitv_asinh(NODE arg, Obj *rp)
{
  if ( bigfloat )
	Pitvbf_asinh(arg, rp);
  else
	Pitvd_asinh(arg, rp);
}

static void Pitv_acosh(NODE arg, Obj *rp)
{
  if ( bigfloat )
	Pitvbf_acosh(arg, rp);
  else
	Pitvd_acosh(arg, rp);
}

static void Pitv_atanh(NODE arg, Obj *rp)
{
  if ( bigfloat )
	Pitvbf_atanh(arg, rp);
  else
	Pitvd_atanh(arg, rp);
}

static void Pitv_exp(NODE arg, Obj *rp)
{
  if ( bigfloat )
	Pitvbf_exp(arg, rp);
  else
	Pitvd_exp(arg, rp);
}

static void Pitv_log(NODE arg, Obj *rp)
{
  if ( bigfloat )
	Pitvbf_log(arg, rp);
  else
	Pitvd_log(arg, rp);
}

static void Pitv_abs(NODE arg, Obj *rp)
{
  if ( bigfloat )
	Pitvbf_abs(arg, rp);
  else
	Pitvd_abs(arg, rp);
}

static void Pitv_pow(NODE arg, Num *rp)
{
  if ( bigfloat )
	Pitvbf_pow(arg, rp);
  else
	Pitvd_pow(arg, rp);
}

#endif
