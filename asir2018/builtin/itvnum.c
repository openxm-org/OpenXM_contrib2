/*
 * $OpenXM$
 */

#include "ca.h"
#include "parse.h"
#include "version.h"
#if !defined(ANDROID)
#include "../plot/ifplot.h"
#endif

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
void miditvp(Itv,Num *);
void absitvp(Itv,Num *);
int initvd(Num,IntervalDouble);
int initvp(Num,Itv);
int itvinitvp(Itv,Itv);
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
  {"disintval",Pdisjitv,2},
  {"inintval",Pinitv,2},
  {"cup",Pcup,2},
  {"cap",Pcap,2},
  {"mid",Pmid,1},
  {"width",Pwidth,1},
  {"diam",Pwidth,1},
  {"distance",Pdistance,2},
  {"iversion",Pitvversion,0},
/* plot time check */
  {"ifcheck",Pifcheck,-7},
#endif
  {0,0,0},
};

#if defined(INTERVAL)

/* plot time check */
static void
Pifcheck(NODE arg, Obj *rp)
{
  Q m2,p2,s_id;
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
  Q one;
  int width, height, ix, iy;
  int id;

  STOQ(-2,m2); STOQ(2,p2);
  STOQ(1,one);
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
    can->width = QTOS((Q)BDY(BDY(geom)));
    can->height = QTOS((Q)BDY(NEXT(BDY(geom))));
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
Pitvversion(Q *rp)
{
  STOQ(ASIR_VERSION, *rp);
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
  Itv  c;
  BF  ii,ss;
  double  inf, sup;

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
  inf = ToRealDown(a0);
  sup = ToRealUp(a1);
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
Pinitv(arg,rp)
NODE arg;
Obj *rp;
{
  int  s;
  Q  q;

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
  Itv  s;

  asir_assert(ARG0(arg),O_N,"disjitv");
  asir_assert(ARG1(arg),O_N,"disjitv");
  error("disjitv: not implemented yet");
  if ( ! s ) *rp = 0;
  else *rp = (Obj)ONE;
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
    l=QTOS(a);
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
  

