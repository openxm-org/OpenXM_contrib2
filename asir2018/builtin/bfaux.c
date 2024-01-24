/* $OpenXM: OpenXM_contrib2/asir2018/builtin/bfaux.c,v 1.2 2018/09/28 08:20:27 noro Exp $ */
#include "ca.h"
#include "parse.h"

void Ptodouble(NODE arg,Obj *rp);
void Peval(NODE arg,Obj *rp);
void Psetprec(NODE arg,Obj *rp);
void Psetbprec(NODE arg,Obj *rp);
void Psetround(NODE arg,Z *rp);
void Pfac(NODE arg,Num *rp);;
void Pmpfr_gamma(NODE arg,BF *rp);
void Pmpfr_lngamma(NODE arg,BF *rp);
void Pmpfr_digamma(NODE arg,BF *rp);
void Pmpfr_zeta(NODE arg,BF *rp);
void Pmpfr_eint(NODE arg,BF *rp);
void Pmpfr_erf(NODE arg,BF *rp);
void Pmpfr_erfc(NODE arg,BF *rp);
void Pmpfr_j0(NODE arg,BF *rp);
void Pmpfr_j1(NODE arg,BF *rp);
void Pmpfr_y0(NODE arg,BF *rp);
void Pmpfr_y1(NODE arg,BF *rp);
void Pmpfr_li2(NODE arg,BF *rp);
void Pmpfr_ai(NODE arg,BF *rp);
void Pmpfr_floor(NODE arg,Z *rp);
void Pmpfr_ceil(NODE arg,Z *rp);
void Pmpfr_round(NODE arg,Z *rp);
void Prk_ratmat(NODE arg,LIST *rp);
void mp_pi(NODE arg,BF *rp);
void mp_e(NODE arg,BF *rp);
void mpfr_or_mpc(NODE arg,int (*mpfr_f)(),int (*mpc_f)(),Num *rp);
void mp_sin(NODE arg,Num *rp);
void mp_cos(NODE arg,Num *rp);
void mp_tan(NODE arg,Num *rp);
void mp_asin(NODE arg,Num *rp);
void mp_acos(NODE arg,Num *rp);
void mp_atan(NODE arg,Num *rp);
void mp_sinh(NODE arg,Num *rp);
void mp_cosh(NODE arg,Num *rp);
void mp_tanh(NODE arg,Num *rp);
void mp_asinh(NODE arg,Num *rp);
void mp_acosh(NODE arg,Num *rp);
void mp_atanh(NODE arg,Num *rp);
void mp_exp(NODE arg,Num *rp);
void mp_log(NODE arg,Num *rp);
void mp_abs(NODE arg,Num *rp);
void mp_factorial(NODE arg,Num *rp);
void mp_pow(NODE arg,Num *rp);

struct ftab bf_tab[] = {
  {"eval",Peval,-2},
  {"setprec",Psetprec,-1},
  {"setbprec",Psetbprec,-1},
  {"setround",Psetround,-1},
  {"todouble",Ptodouble,1},
  {"mpfr_sin",mp_sin,-2},
  {"mpfr_cos",mp_cos,-2},
  {"mpfr_tan",mp_tan,-2},
  {"mpfr_asin",mp_asin,-2},
  {"mpfr_acos",mp_acos,-2},
  {"mpfr_atan",mp_atan,-2},
  {"mpfr_sinh",mp_sinh,-2},
  {"mpfr_cosh",mp_cosh,-2},
  {"mpfr_tanh",mp_tanh,-2},
  {"mpfr_asinh",mp_asinh,-2},
  {"mpfr_acosh",mp_acosh,-2},
  {"mpfr_atanh",mp_atanh,-2},
  {"mpfr_exp",mp_exp,-2},
  {"mpfr_log",mp_log,-2},
  {"mpfr_pow",mp_pow,-3},
  {"mpfr_ai",Pmpfr_ai,-2},
  {"mpfr_zeta",Pmpfr_zeta,-2},
  {"mpfr_j0",Pmpfr_j0,-2},
  {"mpfr_j1",Pmpfr_j1,-2},
  {"mpfr_y0",Pmpfr_y0,-2},
  {"mpfr_y1",Pmpfr_y1,-2},
  {"mpfr_eint",Pmpfr_eint,-2},
  {"mpfr_erf",Pmpfr_erf,-2},
  {"mpfr_erfc",Pmpfr_erfc,-2},
  {"mpfr_li2",Pmpfr_li2,-2},
  {"mpfr_gamma",Pmpfr_gamma,-2},
  {"mpfr_lngamma",Pmpfr_gamma,-2},
  {"mpfr_digamma",Pmpfr_gamma,-2},
  {"mpfr_floor",Pmpfr_floor,-2},
  {"mpfr_ceil",Pmpfr_ceil,-2},
  {"mpfr_round",Pmpfr_round,-2},
  {"rk_ratmat",Prk_ratmat,7},
  {0,0,0},
};

int mpfr_roundmode = MPFR_RNDN;

void todoublen(Num a,Num *rp)
{
  double r,i;
  Real real,imag;

  if ( !a ) {
    *rp = 0;
    return;
  }
  switch ( NID(a) ) {
    case N_R: case N_Q: case N_B:
      r = ToReal(a);
      MKReal(r,real);
      *rp = (Num)real;
      break;
    case N_C:
      r = ToReal(((C)a)->r);
      i = ToReal(((C)a)->i);
      MKReal(r,real);
      MKReal(i,imag);
      reimtocplx((Num)real,(Num)imag,rp);
      break;
    default:
      *rp = a;
      break;
  }
}

void todoublep(P a,P *rp)
{
  DCP dc,dcr,dcr0;

  if ( !a ) *rp = 0;
  else if ( OID(a) == O_N ) todoublen((Num)a,(Num *)rp);
  else {
    for ( dcr0 = 0, dc = DC(a); dc; dc = NEXT(dc) ) {
      NEXTDC(dcr0,dcr);
      DEG(dcr) = DEG(dc);
      todoublep(COEF(dc),&COEF(dcr));
    }
    NEXT(dcr) = 0;
    MKP(VR(a),dcr0,*rp);
  }
}

void todoubler(R a,R *rp)
{
  R b;

  if ( !a ) *rp = 0;
  else if ( OID(a) <= O_P ) todoublep((P)a,(P *)rp);
  else {
    NEWR(b);
    todoublep(a->nm,&b->nm);
    todoublep(a->dn,&b->dn);
    *rp = b;
  }
}

void todouble(Obj a,Obj *b)
{
  Obj t;
  LIST l;
  V v;
  int row,col,len;
  VECT vect;
  MAT mat;
  int i,j;
  NODE n0,n,nd;
  MP m,mp,mp0;
  DP d;

  if ( !a ) {
    *b = 0;
    return;
  }
  switch ( OID(a) ) {
    case O_N:
      todoublen((Num)a,(Num *)b);
      break;
    case O_P:
      todoublep((P)a,(P *)b);
      break;
    case O_R:
      todoubler((R)a,(R *)b);
      break;
    case O_LIST:
      n0 = 0;
      for ( nd = BDY((LIST)a); nd; nd = NEXT(nd) ) {
        NEXTNODE(n0,n);
        todouble((Obj)BDY(nd),(Obj *)&BDY(n));
      }
      if ( n0 )
        NEXT(n) = 0;
      MKLIST(l,n0);
      *b = (Obj)l;
      break;
    case O_VECT:
      len = ((VECT)a)->len;
      MKVECT(vect,len);
      for ( i = 0; i < len; i++ ) {
        todouble((Obj)BDY((VECT)a)[i],(Obj *)&BDY(vect)[i]);
      }
      *b = (Obj)vect;
      break;
    case O_MAT:
      row = ((MAT)a)->row;
      col = ((MAT)a)->col;
      MKMAT(mat,row,col);
      for ( i = 0; i < row; i++ )
        for ( j = 0; j < col; j++ ) {
          todouble((Obj)BDY((MAT)a)[i][j],(Obj *)&BDY(mat)[i][j]);
        }
      *b = (Obj)mat;
      break;
    case O_DP:
      mp0 = 0;
      for ( m = BDY((DP)a); m; m = NEXT(m) ) {
        todouble(C(m),&t);
        if ( t ) {
          NEXTMP(mp0,mp);
          C(mp) = t;
          mp->dl = m->dl;
        }
      }
      if ( mp0 ) {
        MKDP(NV((DP)a),mp0,d);
        d->sugar = ((DP)a)->sugar;
        *b = (Obj)d;
      } else
        *b = 0;

      break;
    default:
      error("todouble : invalid argument");
  }
}

void Ptodouble(NODE arg,Obj *rp)
{
  todouble((Obj)ARG0(arg),rp);
}

void Peval(NODE arg,Obj *rp)
{
  long prec;

  asir_assert(ARG0(arg),O_R,"eval");
  if ( argc(arg) == 2 ) {
    prec = ZTOS((Q)ARG1(arg))*3.32193;
    if ( prec < MPFR_PREC_MIN ) prec = MPFR_PREC_MIN;
    else if ( prec > MPFR_PREC_MAX ) prec = MPFR_PREC_MAX;
  } else
    prec = 0;
  evalr(CO,(Obj)ARG0(arg),prec,rp);
}

/* set/get decimal precision */

void Psetprec(NODE arg,Obj *rp)
{
  long p;
  Z q;
  long prec,dprec;
  
  prec = mpfr_get_default_prec();
  /* decimal precision */
  dprec = prec*0.30103;
  STOZ(dprec,q); *rp = (Obj)q;
  if ( arg ) {
    asir_assert(ARG0(arg),O_N,"setprec");
    p = ZTOS((Q)ARG0(arg))*3.32193;
    if ( p > 0 )
      prec = p;
  }
  if ( prec < MPFR_PREC_MIN ) prec = MPFR_PREC_MIN;
  else if ( prec > MPFR_PREC_MAX ) prec = MPFR_PREC_MAX;
  mpfr_set_default_prec(prec);
}

/* set/get bit precision */

void Psetbprec(NODE arg,Obj *rp)
{
  long p;
  Z q;
  long prec,dprec;
  
  prec = mpfr_get_default_prec();
  STOZ(prec,q); *rp = (Obj)q;
  if ( arg ) {
    asir_assert(ARG0(arg),O_N,"setbprec");
    p = ZTOS((Q)ARG0(arg));
    if ( p > 0 )
      prec = p;
  }
  if ( prec < MPFR_PREC_MIN ) prec = MPFR_PREC_MIN;
  else if ( prec > MPFR_PREC_MAX ) prec = MPFR_PREC_MAX;
  mpfr_set_default_prec(prec);
}

void Psetround(NODE arg,Z *rp)
{
  int round;

  STOZ(mpfr_roundmode,*rp);
  if ( arg ) {
    asir_assert(ARG0(arg),O_N,"setround");
    round = ZTOS((Q)ARG0(arg)); 
    switch ( round ) {
    case 0:
      mpfr_roundmode = MPFR_RNDN;
      break;
    case 1:
      mpfr_roundmode = MPFR_RNDZ;
      break;
    case 2:
      mpfr_roundmode = MPFR_RNDU;
      break;
    case 3:
      mpfr_roundmode = MPFR_RNDD;
      break;
    case 4:
      mpfr_roundmode = MPFR_RNDA;
      break;
    case 5:
      mpfr_roundmode = MPFR_RNDF;
      break;
    case 6:
      mpfr_roundmode = MPFR_RNDNA;
      break;
    default:
      error("setround : invalid rounding mode");
      break;
    }
  }
}

Num tobf(Num a,int prec);

void mp_pi(NODE arg,BF *rp)
{
  int prec;
  BF r;

  prec = arg ? ZTOS((Q)ARG0(arg)) : 0;
  NEWBF(r);
  prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
  mpfr_const_pi(r->body,mpfr_roundmode);
  if ( !cmpbf((Num)r,0) ) r = 0;
  *rp = r; 
}

void mp_e(NODE arg,BF *rp)
{
  int prec;
  mpfr_t one;
  BF r;

  prec = arg ? ZTOS((Q)ARG0(arg)) : 0;
  NEWBF(r);
  prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);
  mpfr_init(one);
  mpfr_set_ui(one,1,mpfr_roundmode);
  mpfr_exp(r->body,one,mpfr_roundmode);
  if ( !cmpbf((Num)r,0) ) r = 0;
  *rp = r; 
}

void mpfr_or_mpc(NODE arg,int (*mpfr_f)(),int (*mpc_f)(),Num *rp)
{
  Num a;
  int prec;
  BF r,re,im;
  C c;
  mpc_t mpc,a1;

  prec = NEXT(arg) ? ZTOS((Q)ARG1(arg)) : mpfr_get_default_prec();
  a = tobf(ARG0(arg),prec);
  if ( a && NID(a)==N_C ) {
    mpc_init2(mpc,prec); mpc_init2(a1,prec);
    re = (BF)((C)a)->r; im = (BF)((C)a)->i;
    mpc_set_fr_fr(a1,re->body,im->body,mpfr_roundmode);
    (*mpc_f)(mpc,a1,mpfr_roundmode);
    MPFRTOBF(mpc_realref(mpc),re);
    MPFRTOBF(mpc_imagref(mpc),im);
    if ( !cmpbf((Num)re,0) ) re = 0;
    if ( !cmpbf((Num)im,0) ) im = 0;
    if ( !im )
      *rp = (Num)re;
    else {
      NEWC(c); c->r = (Num)re; c->i = (Num)im;
      *rp = (Num)c; 
    }
  } else {
    NEWBF(r);
    mpfr_init2(r->body,prec);
    (*mpfr_f)(r->body,((BF)a)->body,mpfr_roundmode);
    if ( !cmpbf((Num)r,0) ) r = 0;
    *rp = (Num)r; 
  }
}

void mp_sin(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_sin,mpc_sin,rp);
}

void mp_cos(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_cos,mpc_cos,rp);
}

void mp_tan(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_tan,mpc_tan,rp);
}

void mp_asin(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_asin,mpc_asin,rp);
}

void mp_acos(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_acos,mpc_acos,rp);
}

void mp_atan(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_atan,mpc_atan,rp);
}

void mp_sinh(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_sinh,mpc_sinh,rp);
}

void mp_cosh(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_cosh,mpc_cosh,rp);
}

void mp_tanh(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_tanh,mpc_tanh,rp);
}

void mp_asinh(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_asinh,mpc_asinh,rp);
}

void mp_acosh(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_acosh,mpc_acosh,rp);
}

void mp_atanh(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_atanh,mpc_atanh,rp);
}

void mp_exp(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_exp,mpc_exp,rp);
}

void mp_log(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_log,mpc_log,rp);
}

void mp_abs(NODE arg,Num *rp)
{
  mpfr_or_mpc(arg,mpfr_abs,mpc_abs,rp);
}

void Pfac(NODE arg,Num *rp);

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
    Pmpfr_gamma(&arg0,(BF *)rp);
  }
}

void mp_pow(NODE arg,Num *rp)
{
  Num a,e;
  int prec;
  BF r,re,im;
  C c;
  mpc_t mpc,a1,e1;

  prec = NEXT(NEXT(arg)) ? ZTOS((Q)ARG2(arg)) : mpfr_get_default_prec();
  a = tobf(ARG0(arg),prec);
  e = tobf(ARG1(arg),prec);
  if ( NID(a) == N_C || NID(e) == N_C || MPFR_SIGN(((BF)a)->body) < 0 ) {
    mpc_init2(mpc,prec); mpc_init2(a1,prec); mpc_init2(e1,prec);
    if ( NID(a) == N_C ) {
      re = (BF)((C)a)->r; im = (BF)((C)a)->i;
      mpc_set_fr_fr(a1,re->body,im->body,mpfr_roundmode);
    } else {
      re = (BF)a;
      mpc_set_fr(a1,re->body,mpfr_roundmode);
    }
    if ( NID(e) == N_C ) {
      re = (BF)((C)e)->r; im = (BF)((C)e)->i;
      mpc_set_fr_fr(e1,re->body,im->body,mpfr_roundmode);
    } else {
      re = (BF)e;
      mpc_set_fr(e1,re->body,mpfr_roundmode);
    }
    mpc_pow(mpc,a1,e1,mpfr_roundmode);
    MPFRTOBF(mpc_realref(mpc),re);
    MPFRTOBF(mpc_imagref(mpc),im);
    if ( !cmpbf((Num)re,0) ) re = 0;
    if ( !cmpbf((Num)im,0) ) im = 0;
    if ( !im )
      *rp = (Num)re;
    else {
      NEWC(c); c->r = (Num)re; c->i = (Num)im;
      *rp = (Num)c; 
    }
  } else {
    NEWBF(r);
    mpfr_init2(r->body,prec);
    mpfr_pow(r->body,((BF)a)->body,((BF)e)->body,mpfr_roundmode);
    *rp = (Num)r; 
  }
}

#define SETPREC \
 (prec)=NEXT(arg)?ZTOS((Q)ARG1(arg)):0;\
 (prec)*=3.32193;\
 (a)=tobf(ARG0(arg),prec);\
 NEWBF(r);\
 prec ? mpfr_init2(r->body,prec) : mpfr_init(r->body);


void Pmpfr_gamma(NODE arg,BF *rp)
{
  Num a;
  int prec;
  BF r;

  SETPREC
  mpfr_gamma(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_lngamma(NODE arg,BF *rp)
{
  Num a;
  int prec;
  BF r;

  SETPREC
  mpfr_lngamma(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_digamma(NODE arg,BF *rp)
{
  Num a;
  int prec;
  BF r;

  SETPREC
  mpfr_digamma(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_zeta(NODE arg,BF *rp)
{
  Num a;
  int prec;
  BF r;

  SETPREC
  mpfr_zeta(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_eint(NODE arg,BF *rp)
{
  Num a;
  int prec;
  BF r;

  SETPREC
  mpfr_eint(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_erf(NODE arg,BF *rp)
{
  Num a;
  int prec;
  BF r;

  SETPREC
  mpfr_erf(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_erfc(NODE arg,BF *rp)
{
  Num a;
  int prec;
  BF r;

  SETPREC
  mpfr_erfc(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_j0(NODE arg,BF *rp)
{
  Num a;
  int prec;
  BF r;

  SETPREC
  mpfr_j0(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_j1(NODE arg,BF *rp)
{
  Num a;
  int prec;
  BF r;

  SETPREC
  mpfr_j1(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_y0(NODE arg,BF *rp)
{
  Num a;
  int prec;
  BF r;

  SETPREC
  mpfr_y0(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_y1(NODE arg,BF *rp)
{
  Num a;
  int prec;
  BF r;

  SETPREC
  mpfr_y1(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_li2(NODE arg,BF *rp)
{
  Num a;
  int prec;
  BF r;

  SETPREC
  mpfr_li2(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_ai(NODE arg,BF *rp)
{
  Num a;
  int prec;
  BF r;

  SETPREC
  mpfr_ai(r->body,((BF)a)->body,mpfr_roundmode);
  *rp = r; 
}

void Pmpfr_floor(NODE arg,Z *rp)
{
  Num a;
  long prec;
  BF r;
  mpz_t t;

  SETPREC
  mpfr_floor(r->body,((BF)a)->body);
  mpz_init(t);
  mpfr_get_z(t,r->body,mpfr_roundmode);
  MPZTOZ(t,*rp);
}

void Pmpfr_ceil(NODE arg,Z *rp)
{
  Num a;
  long prec;
  BF r;
  mpz_t t;
  Z rz;

  SETPREC
  mpfr_ceil(r->body,((BF)a)->body);
  mpz_init(t);
  mpfr_get_z(t,r->body,mpfr_roundmode);
  MPZTOZ(t,*rp);
}

void Pmpfr_round(NODE arg,Z *rp)
{
  Num a;
  long prec;
  BF r;
  mpz_t t;

  SETPREC
  mpfr_round(r->body,((BF)a)->body);
  mpz_init(t);
  mpfr_get_z(t,r->body,mpfr_roundmode);
  MPZTOZ(t,*rp);
}

double **almat_double(int n)
{
  int i;
  double **a;

  a = (double **)MALLOC(n*sizeof(double *));
  for ( i = 0; i < n; i++ )
    a[i] = (double *)MALLOC(n*sizeof(double));
  return a;
}

/*
 *  k <- (A(xi)-(sbeta-mn2/xi))f
 *  A(t) = (num[0]+num[1]t+...+num[d-1]*t^(d-1))/den(t)
 */

struct jv {
  int j;
  double v;
};

struct smat {
  int *rlen;
  struct jv **row;
};

void eval_pfaffian2(double *k,int n,int d,struct smat *num,P den,double xi,double *f)
{
  struct smat ma;
  struct jv *maj;
  int i,j,l,s;
  double t,dn;
  P r;
  Real u;

  memset(k,0,n*sizeof(double));
  for ( i = d-1; i >= 0; i-- ) {
    ma = num[i];
    for ( j = 0; j < n; j++ ) {
      maj = ma.row[j];
      l = ma.rlen[j];
      for ( t = 0, s = 0; s < l; s++, maj++ ) t += maj->v*f[maj->j];
      k[j] = k[j]*xi+t;
    }
  }
  MKReal(xi,u);
  substp(CO,den,den->v,(P)u,&r); dn = ToReal(r);
  for ( j = 0; j < n; j++ )
    k[j] /= dn;
}

void Prk_ratmat(NODE arg,LIST *rp)
{
  VECT mat;
  P den;
  int ord;
  double sbeta,x0,x1,xi,h,mn2,hd;
  double a2,a3,a4,a5,a6;
  double b21,b31,b32,b41,b42,b43,b51,b52,b53,b54,b61,b62,b63,b64,b65;
  double c1,c2,c3,c4,c5,c6,c7;
  VECT fv;
  int step,j,i,k,n,d,len,s;
  struct smat *num;
  Obj **b;
  MAT mati;
  double *f,*w,*k1,*k2,*k3,*k4,*k5,*k6;
  NODE nd,nd1;
  Real x,t;
  LIST l;

  ord = ZTOS((Q)ARG0(arg));
  mat = (VECT)ARG1(arg); den = (P)ARG2(arg);
  x0 = ToReal((Num)ARG3(arg)); x1 = ToReal((Num)ARG4(arg));
  step = ZTOS((Q)ARG5(arg)); fv = (VECT)ARG6(arg);
  h = (x1-x0)/step;

  n = fv->len;
  d = mat->len;
  num = (struct smat *)MALLOC(d*sizeof(struct smat)); 
  for ( i = 0; i < d; i++ ) {
    num[i].rlen = (int *)MALLOC_ATOMIC(n*sizeof(int));
    num[i].row = (struct jv **)MALLOC(n*sizeof(struct jv *));
    mati = (MAT)mat->body[i];
    b = (Obj **)mati->body;
    for ( j = 0; j < n; j++ ) {
      for ( len = k = 0; k < n; k++ )
        if ( b[j][k] ) len++;
      num[i].rlen[j] = len;
      if ( !len )
        num[i].row[j] = 0;
      else {
        num[i].row[j] = (struct jv *)MALLOC_ATOMIC((len)*sizeof(struct jv));
        for ( s = k = 0; k < n; k++ )
          if ( b[j][k] ) {
             num[i].row[j][s].j = k;
             num[i].row[j][s].v = ToReal((Num)b[j][k]);
             s++;
          }
      }
    }
  }
  f = (double *)MALLOC_ATOMIC(n*sizeof(double));
  for ( j = 0; j < n; j++ )
    f[j] = ToReal((Num)fv->body[j]);
  w = (double *)MALLOC_ATOMIC(n*sizeof(double));
  k1 = (double *)MALLOC_ATOMIC(n*sizeof(double));
  k2 = (double *)MALLOC_ATOMIC(n*sizeof(double));
  k3 = (double *)MALLOC_ATOMIC(n*sizeof(double));
  k4 = (double *)MALLOC_ATOMIC(n*sizeof(double));
  k5 = (double *)MALLOC_ATOMIC(n*sizeof(double));
  k6 = (double *)MALLOC(n*sizeof(double));
  nd = 0;
  switch ( ord ) {
  case 4:
    a2 = 1/2.0*h; b21 = 1/2.0*h;
    a3 = 1/2.0*h; b31 = 0.0;   b32 = 1/2.0*h;
    a4 = 1.0*h;   b41 = 0.0;   b42 = 0.0;    b43 = 1.0*h;
    c1 = 1/6.0*h; c2 = 1/3.0*h;     c3 =  1/3.0*h; c4 = 1/6.0*h;
    for ( i = 0; i < step; i++ ) {
      if ( !(i%100000) ) fprintf(stderr,"[%d]",i);
      xi = x0+i*h;
      eval_pfaffian2(k1,n,d,num,den,xi,f);
      memcpy(w,f,n*sizeof(double)); for ( j = 0; j < n; j++ ) w[j] += b21*k1[j];
      eval_pfaffian2(k2,n,d,num,den,xi+a2,w);
      memcpy(w,f,n*sizeof(double)); for ( j = 0; j < n; j++ ) w[j] += b31*k1[j]+b32*k2[j];
      eval_pfaffian2(k3,n,d,num,den,xi+a3,w);
      memcpy(w,f,n*sizeof(double)); for ( j = 0; j < n; j++ ) w[j] += b41*k1[j]+b42*k2[j]+b43*k3[j];
      eval_pfaffian2(k4,n,d,num,den,xi+a4,w);
      memcpy(w,f,n*sizeof(double)); for ( j = 0; j < n; j++ ) w[j] += c1*k1[j]+c2*k2[j]+c3*k3[j]+c4*k4[j];
      memcpy(f,w,n*sizeof(double));
      MKReal(f[0],t);
      MKReal(xi+h,x);
      nd1 = mknode(2,x,t);
      MKLIST(l,nd1);
      MKNODE(nd1,l,nd);
      nd = nd1;
      for ( hd = f[0], j = 0; j < n; j++ ) f[j] /= hd;
    }
    MKLIST(*rp,nd);
    break;
  case 5:
  default:
    a2 = 1/4.0*h; b21 = 1/4.0*h;
    a3 = 1/4.0*h; b31 = 1/8.0*h; b32 = 1/8.0*h;
    a4 = 1/2.0*h; b41 = 0.0;   b42 = 0.0;    b43 = 1/2.0*h;
    a5 = 3/4.0*h; b51 = 3/16.0*h;b52 = -3/8.0*h; b53 = 3/8.0*h;   b54 = 9/16.0*h;
    a6 = 1.0*h;   b61 = -3/7.0*h;b62 = 8/7.0*h;  b63 = 6/7.0*h;   b64 = -12/7.0*h; b65 = 8/7.0*h;
    c1 = 7/90.0*h; c2 = 0.0;     c3 =  16/45.0*h; c4 = 2/15.0*h;   c5 = 16/45.0*h; c6 = 7/90.0*h;
    for ( i = 0; i < step; i++ ) {
      if ( !(i%100000) ) fprintf(stderr,"[%d]",i);
      xi = x0+i*h;
      eval_pfaffian2(k1,n,d,num,den,xi,f);
      memcpy(w,f,n*sizeof(double)); for ( j = 0; j < n; j++ ) w[j] += b21*k1[j];
      eval_pfaffian2(k2,n,d,num,den,xi+a2,w);
      memcpy(w,f,n*sizeof(double)); for ( j = 0; j < n; j++ ) w[j] += b31*k1[j]+b32*k2[j];
      eval_pfaffian2(k3,n,d,num,den,xi+a3,w);
      memcpy(w,f,n*sizeof(double)); for ( j = 0; j < n; j++ ) w[j] += b41*k1[j]+b42*k2[j]+b43*k3[j];
      eval_pfaffian2(k4,n,d,num,den,xi+a4,w);
      memcpy(w,f,n*sizeof(double)); for ( j = 0; j < n; j++ ) w[j] += b51*k1[j]+b52*k2[j]+b53*k3[j]+b54*k4[j];
      eval_pfaffian2(k5,n,d,num,den,xi+a5,w);
      memcpy(w,f,n*sizeof(double)); for ( j = 0; j < n; j++ ) w[j] += b61*k1[j]+b62*k2[j]+b63*k3[j]+b64*k4[j]+b65*k5[j];
      eval_pfaffian2(k6,n,d,num,den,xi+a6,w);
      memcpy(w,f,n*sizeof(double)); for ( j = 0; j < n; j++ ) w[j] += c1*k1[j]+c2*k2[j]+c3*k3[j]+c4*k4[j]+c5*k5[j]+c6*k6[j];
      memcpy(f,w,n*sizeof(double));
      MKReal(f[0],t);
      MKReal(xi+h,x);
      nd1 = mknode(2,x,t);
      MKLIST(l,nd1);
      MKNODE(nd1,l,nd);
      nd = nd1;
      for ( hd = f[0], j = 0; j < n; j++ ) f[j] /= hd;
    }
    MKLIST(*rp,nd);
    break;
  }
}
