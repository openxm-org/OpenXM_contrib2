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
 * $OpenXM: OpenXM_contrib2/asir2018/builtin/pf.c,v 1.2 2018/09/28 08:20:27 noro Exp $
*/
#include "ca.h"
#include "math.h"
#include "parse.h"
#if 0
#include <alloca.h>
#endif

double const_pi(),const_e(), double_factorial();

void make_ihyp(void);
void make_hyp(void);
void make_itri(void);
void make_tri(void);
void make_exp(void);
void simplify_pow(PFINS,Obj *);
FNODE partial_eval(FNODE f);

void Pfunctor(),Pargs(),Pfunargs(),Pvtype(),Pcall(),Pdeval(),Pfunargs_ext();
void Pregister_handler();
void Peval_quote();
void Pmapat(), Pmap();
void Padd_handler();
void Plist_handler();
void Pclear_handler();

struct ftab puref_tab[] = {
  {"mapat",Pmapat,-99999999},
  {"map",Pmap,-99999999},
  {"functor",Pfunctor,1},
  {"args",Pargs,1},
  {"funargs",Pfunargs,1},
  {"funargs_ext",Pfunargs_ext,1},
  {"register_handler",Pregister_handler,1},
  {"add_handler",Padd_handler,2},
  {"list_handler",Plist_handler,1},
  {"clear_handler",Pclear_handler,1},
  {"call",Pcall,2},
  {"vtype",Pvtype,1},
  {"deval",Pdeval,1},
  {"eval_quote",Peval_quote,-2},
  {0,0,0},
};

int mp_pi(),mp_e();
int mp_exp(), mp_log(), mp_pow();
int mp_sin(),mp_cos(),mp_tan(),mp_asin(),mp_acos(),mp_atan();
int mp_sinh(),mp_cosh(),mp_tanh(),mp_asinh(),mp_acosh(),mp_atanh();
int mp_factorial();

static V *uarg,*darg;
static P x,y;
static PF pidef,edef;
static PF logdef,expdef,powdef;
static PF sindef,cosdef,tandef;
static PF asindef,acosdef,atandef;
static PF sinhdef,coshdef,tanhdef;
static PF asinhdef,acoshdef,atanhdef;
static PF factorialdef,absdef;

#define OALLOC(p,n) ((p)=(Obj *)CALLOC((n),sizeof(Obj)))

double const_pi() { return 3.14159265358979323846264338327950288; }
double const_e() { return 2.718281828459045235360287471352662497; }

double double_factorial(double x)
{
  return tgamma(x+1);
}

int simplify_elemfunc_ins();
int simplify_factorial_ins();
int simplify_abs_ins();

void pf_init() {
  uarg = (V *)CALLOC(1,sizeof(V));
  uarg[0] = &oVAR[26]; MKV(uarg[0],x);

  darg = (V *)CALLOC(2,sizeof(V));
  darg[0] = &oVAR[26];
  darg[1] = &oVAR[27]; MKV(darg[1],y);

#if defined(INTERVAL)
  mkpf("@pi",0,0,0,(int (*)())mp_pi,const_pi,simplify_elemfunc_ins,pi_itv_ft,&pidef);
  mkpf("@e",0,0,0,(int (*)())mp_e,const_e,simplify_elemfunc_ins,e_itv_ft,&edef);

  mkpf("factorial",0,1,uarg,(int (*)())mp_factorial,double_factorial,simplify_factorial_ins,0,&factorialdef);
  mkpf("abs",0,1,uarg,(int (*)())mp_abs,fabs,simplify_abs_ins,abs_itv_ft,&absdef);

  mkpf("log",0,1,uarg,(int (*)())mp_log,log,simplify_elemfunc_ins,log_itv_ft,&logdef);
  mkpf("exp",0,1,uarg,(int (*)())mp_exp,exp,simplify_elemfunc_ins,exp_itv_ft,&expdef);
  mkpf("pow",0,2,darg,(int (*)())mp_pow,pow,(int (*)())simplify_pow,pow_itv_ft,&powdef);

  mkpf("sin",0,1,uarg,(int (*)())mp_sin,sin,simplify_elemfunc_ins,sin_itv_ft,&sindef);
  mkpf("cos",0,1,uarg,(int (*)())mp_cos,cos,simplify_elemfunc_ins,cos_itv_ft,&cosdef);
  mkpf("tan",0,1,uarg,(int (*)())mp_tan,tan,simplify_elemfunc_ins,tan_itv_ft,&tandef);
  mkpf("asin",0,1,uarg,(int (*)())mp_asin,asin,simplify_elemfunc_ins,asin_itv_ft,&asindef);
  mkpf("acos",0,1,uarg,(int (*)())mp_acos,acos,simplify_elemfunc_ins,acos_itv_ft,&acosdef);
  mkpf("atan",0,1,uarg,(int (*)())mp_atan,atan,simplify_elemfunc_ins,atan_itv_ft,&atandef);

  mkpf("sinh",0,1,uarg,(int (*)())mp_sinh,sinh,simplify_elemfunc_ins,sinh_itv_ft,&sinhdef);
  mkpf("cosh",0,1,uarg,(int (*)())mp_cosh,cosh,simplify_elemfunc_ins,cosh_itv_ft,&coshdef);
  mkpf("tanh",0,1,uarg,(int (*)())mp_tanh,tanh,simplify_elemfunc_ins,tanh_itv_ft,&tanhdef);
#if !defined(VISUAL) && !defined(__MINGW32__)
  mkpf("asinh",0,1,uarg,(int (*)())mp_asinh,asinh,simplify_elemfunc_ins,asinh_itv_ft,&asinhdef);
  mkpf("acosh",0,1,uarg,(int (*)())mp_acosh,acosh,simplify_elemfunc_ins,acosh_itv_ft,&acoshdef);
  mkpf("atanh",0,1,uarg,(int (*)())mp_atanh,atanh,simplify_elemfunc_ins,atanh_itv_ft,&atanhdef);
#endif
#else
  mkpf("@pi",0,0,0,(int (*)())mp_pi,const_pi,simplify_elemfunc_ins,&pidef);
  mkpf("@e",0,0,0,(int (*)())mp_e,const_e,simplify_elemfunc_ins,&edef);

  mkpf("factorial",0,1,uarg,(int (*)())mp_factorial,double_factorial,simplify_factorial_ins,&factorialdef);
  mkpf("abs",0,1,uarg,(int (*)())mp_abs,fabs,simplify_abs_ins,&absdef);

  mkpf("log",0,1,uarg,(int (*)())mp_log,log,simplify_elemfunc_ins,&logdef);
  mkpf("exp",0,1,uarg,(int (*)())mp_exp,exp,simplify_elemfunc_ins,&expdef);
  mkpf("pow",0,2,darg,(int (*)())mp_pow,pow,(int (*)())simplify_pow,&powdef);

  mkpf("sin",0,1,uarg,(int (*)())mp_sin,sin,simplify_elemfunc_ins,&sindef);
  mkpf("cos",0,1,uarg,(int (*)())mp_cos,cos,simplify_elemfunc_ins,&cosdef);
  mkpf("tan",0,1,uarg,(int (*)())mp_tan,tan,simplify_elemfunc_ins,&tandef);
  mkpf("asin",0,1,uarg,(int (*)())mp_asin,asin,simplify_elemfunc_ins,&asindef);
  mkpf("acos",0,1,uarg,(int (*)())mp_acos,acos,simplify_elemfunc_ins,&acosdef);
  mkpf("atan",0,1,uarg,(int (*)())mp_atan,atan,simplify_elemfunc_ins,&atandef);

  mkpf("sinh",0,1,uarg,(int (*)())mp_sinh,sinh,simplify_elemfunc_ins,&sinhdef);
  mkpf("cosh",0,1,uarg,(int (*)())mp_cosh,cosh,simplify_elemfunc_ins,&coshdef);
  mkpf("tanh",0,1,uarg,(int (*)())mp_tanh,tanh,simplify_elemfunc_ins,&tanhdef);
#if !defined(VISUAL) && !defined(__MINGW32__)
  mkpf("asinh",0,1,uarg,(int (*)())mp_asinh,asinh,simplify_elemfunc_ins,&asinhdef);
  mkpf("acosh",0,1,uarg,(int (*)())mp_acosh,acosh,simplify_elemfunc_ins,&acoshdef);
  mkpf("atanh",0,1,uarg,(int (*)())mp_atanh,atanh,simplify_elemfunc_ins,&atanhdef);
#endif
#endif
  make_exp();
  make_tri();
  make_itri();
  make_hyp();
#if !defined(VISUAL) && !defined(__MINGW32__)
  make_ihyp();
#endif
}

void make_exp() {
  V v;
  P u,vexp,vlog,vpow;
  Obj *args;

  mkpfins(expdef,uarg,&v); MKV(v,vexp);
  mkpfins(powdef,darg,&v); MKV(v,vpow);
  mkpfins(logdef,uarg,&v); MKV(v,vlog);

  /* d/dx(log(x)) = 1/x */
  OALLOC(logdef->deriv,1); divr(CO,(Obj)ONE,(Obj)x,&logdef->deriv[0]);

  /* d/dx(exp(x)) = exp(x) */
  OALLOC(expdef->deriv,1); expdef->deriv[0] = (Obj)vexp;

  /* d/dy(x^y) = log(x)*x^y */
  OALLOC(powdef->deriv,2); mulp(CO,vpow,vlog,(P *)&powdef->deriv[1]);

  /* d/dx(x^y) = y*x^(y-1) */
  args = (Obj *)ALLOCA(2*sizeof(Obj));
  args[0] = (Obj)x; subp(CO,y,(P)ONE,(P *)&args[1]);
  _mkpfins(powdef,args,&v); MKV(v,u);
  mulr(CO,(Obj)u,(Obj)y,&powdef->deriv[0]);
}

void make_tri() {
  V v;
  P vcos,vsin,vtan,t;

  mkpfins(cosdef,uarg,&v); MKV(v,vcos);
  mkpfins(sindef,uarg,&v); MKV(v,vsin);
  mkpfins(tandef,uarg,&v); MKV(v,vtan);

  /* d/dx(sin(x)) = cos(x) */
  OALLOC(sindef->deriv,1); sindef->deriv[0] = (Obj)vcos;

  /* d/dx(cos(x)) = -sin(x) */
  OALLOC(cosdef->deriv,1); chsgnp(vsin,(P *)&cosdef->deriv[0]);

  /* d/dx(tan(x)) = 1+tan(x)^2 */
  OALLOC(tandef->deriv,1);
  mulr(CO,(Obj)vtan,(Obj)vtan,(Obj *)&t); addp(CO,(P)ONE,t,(P *)&tandef->deriv[0]);
}

void make_itri() {
  P t,xx;
  Z mtwo;
  V v;
  Obj *args;

  /* d/dx(asin(x)) = (1-x^2)^(-1/2) */
  OALLOC(asindef->deriv,1);
  args = (Obj *)ALLOCA(2*sizeof(Obj));
  mulp(CO,x,x,&xx); subp(CO,(P)ONE,xx,(P *)&args[0]);
  STOZ(-2,mtwo); divz(ONE,mtwo,(Z *)&args[1]);
  _mkpfins(powdef,args,&v); MKV(v,t);
  asindef->deriv[0] = (Obj)t;

  /* d/dx(acos(x)) = -(1-x^2)^(-1/2) */
  OALLOC(acosdef->deriv,1); chsgnp((P)asindef->deriv[0],(P *)&acosdef->deriv[0]);

  /* d/dx(atan(x)) = 1/(x^2+1) */
  OALLOC(atandef->deriv,1);
  addp(CO,(P)ONE,xx,&t); divr(CO,(Obj)ONE,(Obj)t,&atandef->deriv[0]);
}

void make_hyp() {
  V v;
  P vcosh,vsinh,vtanh,t;

  mkpfins(coshdef,uarg,&v); MKV(v,vcosh);
  mkpfins(sinhdef,uarg,&v); MKV(v,vsinh);
  mkpfins(tanhdef,uarg,&v); MKV(v,vtanh);

  /* d/dx(sinh(x)) = cosh(x) */
  OALLOC(sinhdef->deriv,1); sinhdef->deriv[0] = (Obj)vcosh;

  /* d/dx(cosh(x)) = sinh(x) */
  OALLOC(coshdef->deriv,1); coshdef->deriv[0] = (Obj)vsinh;

  /* d/dx(tanh(x)) = 1-tanh(x)^2 */
  OALLOC(tanhdef->deriv,1);
  mulr(CO,(Obj)vtanh,(Obj)vtanh,(Obj *)&t); subp(CO,(P)ONE,t,(P *)&tanhdef->deriv[0]);
}

void make_ihyp() {
  P t,xx;
  Z mtwo;
  V v;
  Obj *args;

  /* d/dx(asinh(x)) = (1+x^2)^(-1/2) */
  OALLOC(asinhdef->deriv,1);
  args = (Obj *)ALLOCA(2*sizeof(Obj));
  mulp(CO,x,x,&xx); addp(CO,(P)ONE,xx,(P *)&args[0]);
  STOZ(-2,mtwo); divz(ONE,mtwo,(Z *)&args[1]);
  _mkpfins(powdef,args,&v); MKV(v,t);
  asinhdef->deriv[0] = (Obj)t;

  /* d/dx(acosh(x)) = (x^2-1)^(-1/2) */
  OALLOC(acoshdef->deriv,1);
  subp(CO,xx,(P)ONE,(P *)&args[0]);
  _mkpfins(powdef,args,&v); MKV(v,t);
  acoshdef->deriv[0] = (Obj)t;

  /* d/dx(atanh(x)) = 1/(1-x^2) */
  OALLOC(atanhdef->deriv,1);
  subp(CO,(P)ONE,xx,&t); divr(CO,(Obj)ONE,(Obj)t,&atanhdef->deriv[0]);
}

void mkpow(VL vl,Obj a,Obj e,Obj *r)
{
  PFINS ins;
  PFAD ad;

  ins = (PFINS)CALLOC(1,sizeof(PF)+2*sizeof(struct oPFAD));
  ins->pf = powdef; ad = ins->ad;
  ad[0].d = 0; ad[0].arg = a; ad[1].d = 0; ad[1].arg = e;
  simplify_ins(ins,r);
}

extern int evalef;

void simplify_pow(PFINS ins,Obj * rp)
{
  PF pf;
  PFAD ad;
  Obj a0,a1;
  V v;
  P t;

  if ( evalef ) {
    simplify_elemfunc_ins(ins,rp);
    return;
  }
  pf = ins->pf; ad = ins->ad; a0 = ad[0].arg; a1 = ad[1].arg;  
  if ( !a1 )
    *rp = (Obj)ONE;
  else if ( !a0 ) {
    if ( RATN(a1) && sgnq((Q)a1)>0 )
      *rp = 0;
    else if ( RATN(a1) && sgnq((Q)a1) < 0 )
      error("simplify_pow : division by 0");
    else {
      instoobj(ins,rp);
    }
  } else if ( NUM(a1) && INT(a1) )
    arf_pwr(CO,a0,a1,rp);
  else {
    instoobj(ins,rp);
  }
}

#define ISPFINS(p)\
((p)&&(ID(p) == O_P)&&((long)VR((P)p)->attr==V_PF)&&\
UNIQ(DEG(DC((P)p)))&&UNIQ(COEF(DC((P)p))))

void Pfunctor(NODE arg,P *rp)
{
  P p;
  FUNC t;
  PF pf;
  PFINS ins;

  p = (P)ARG0(arg);
  if ( !ISPFINS(p) )
    *rp = 0;
  else {
    ins = (PFINS)VR(p)->priv; pf = ins->pf;
    t = (FUNC)MALLOC(sizeof(struct oFUNC));
    t->name = t->fullname = pf->name; t->id = A_PURE; t->argc = pf->argc;
    t->f.puref = pf;
    makesrvar(t,rp);
  }
}

void Pargs(NODE arg,LIST *rp)
{
  P p;
  PF pf;
  PFAD ad;
  PFINS ins;
  NODE n,n0;
  int i;

  p = (P)ARG0(arg);
  if ( !ISPFINS(p) )
    *rp = 0;
  else {
    ins = (PFINS)VR(p)->priv; ad = ins->ad; pf = ins->pf;
    for ( i = 0, n0 = 0; i < pf->argc; i++ ) {
      NEXTNODE(n0,n); BDY(n) = (pointer)ad[i].arg;
    }
    if ( n0 )
      NEXT(n) = 0;
    MKLIST(*rp,n0);  
  }
}

void Pfunargs(NODE arg,LIST *rp)
{
  P p;
  P f;
  FUNC t;
  PF pf;
  PFINS ins;
  PFAD ad;
  NODE n,n0;
  int i;

  p = (P)ARG0(arg);
  if ( !ISPFINS(p) )
    *rp = 0;
  else {
    ins = (PFINS)VR(p)->priv; ad = ins->ad; pf = ins->pf;
    t = (FUNC)MALLOC(sizeof(struct oFUNC));
    t->name = t->fullname = pf->name; t->id = A_PURE; t->argc = pf->argc;
    t->f.puref = pf;
    makesrvar(t,&f);
    n = n0 = 0; NEXTNODE(n0,n); BDY(n) = (pointer)f;
    for ( i = 0; i < pf->argc; i++ ) {
      NEXTNODE(n0,n); BDY(n) = (pointer)ad[i].arg;
    }
    NEXT(n) = 0;
    MKLIST(*rp,n0);  
  }
}

void Pfunargs_ext(NODE arg,LIST *rp)
{
  P p;
  P f;
  FUNC t;
  PF pf;
  PFINS ins;
  PFAD ad;
  NODE n,n0,d,d0,a,a0;
  LIST alist,dlist;
  Z q;
  int i;

  p = (P)ARG0(arg);
  if ( !ISPFINS(p) )
    *rp = 0;
  else {
    ins = (PFINS)VR(p)->priv; ad = ins->ad; pf = ins->pf;
    t = (FUNC)MALLOC(sizeof(struct oFUNC));
    t->name = t->fullname = pf->name; t->id = A_PURE; t->argc = pf->argc;
    t->f.puref = pf;
    makesrvar(t,&f);

    d0 = a0 = 0;
    for ( i = 0; i < pf->argc; i++ ) {
      NEXTNODE(d0,d); STOZ(ad[i].d,q); BDY(d) = (pointer)q;
      NEXTNODE(a0,a); BDY(a) = (pointer)ad[i].arg;
    }
    NEXT(d) = 0; NEXT(a) = 0; MKLIST(alist,a0); MKLIST(dlist,d0);

    n0 = mknode(3,f,dlist,alist);
    MKLIST(*rp,n0);  
  }
}

void Pvtype(NODE arg,Z *rp)
{
  P p;

  p = (P)ARG0(arg);
  if ( !p || ID(p) != O_P )
    *rp = 0;
  else
    STOZ((long)VR(p)->attr,*rp);
}

extern NODE user_int_handler,user_quit_handler;

void Pregister_handler(NODE arg,Z *rp)
{
  P p;
  V v;
  NODE n;
  FUNC func;

  p = (P)ARG0(arg);
  if ( !p ) {
    user_int_handler = 0;
    *rp = 0;
    return;
  } else if ( OID(p) != 2 )
    error("register_hanlder : invalid argument");
  v = VR(p);
  if ( (long)v->attr != V_SR )  
    error("register_hanlder : no such function");
  else {
    func = (FUNC)v->priv;
    if ( func->argc )
      error("register_hanlder : the function must be with no argument");
    else {
      MKNODE(n,(pointer)func,user_int_handler);
      user_int_handler = n;
      *rp = ONE;
    }
  }
}

void Padd_handler(NODE arg,Z *rp)
{
  P p;
  V v;
  NODE n;
  FUNC func;
  char *name;
  NODE *hlistp;

  asir_assert(ARG0(arg),O_STR,"add_handler");
  name = BDY((STRING)ARG0(arg));
  p = (P)ARG1(arg);
  if ( !strcmp(name,"intr") )
    hlistp = &user_int_handler;
  else if ( !strcmp(name,"quit") )
    hlistp = &user_quit_handler;
  else
    error("add_handler : invalid keyword (must be \"intr\" or \"quit\")");
  if ( !p ) {
    *hlistp = 0; *rp = 0;
    return;
  }
  if ( OID(p) == 2 ) {
    v = VR(p);
    if ( (long)v->attr != V_SR )  
      error("add_hanlder : no such function");
    func = (FUNC)v->priv;
  } else if ( OID(p) == O_STR ) {
    gen_searchf_searchonly(BDY((STRING)p),&func,0);
    if ( !func )
      error("add_hanlder : no such function");
  }
  if ( func->argc )
    error("register_hanlder : the function must be with no argument");
  else {
    MKNODE(n,(pointer)func,*hlistp);
    *hlistp = n;
    *rp = ONE;
  }
}

void Plist_handler(NODE arg,LIST *rp)
{
  NODE r0,r,t;
  char *name;
  NODE hlist;
  STRING fname;

  asir_assert(ARG0(arg),O_STR,"list_handler");
  name = BDY((STRING)ARG0(arg));
  if ( !strcmp(name,"intr") )
    hlist = user_int_handler;
  else if ( !strcmp(name,"quit") )
    hlist = user_quit_handler;
  else
    error("list_handler : invalid keyword (must be \"intr\" or \"quit\")");
  for ( r0 = 0, t = hlist; t; t = NEXT(t) ) {
    NEXTNODE(r0,r);
    MKSTR(fname,((FUNC)BDY(t))->fullname);
    BDY(r) = (pointer)fname;
  }
  if ( r0 ) NEXT(r) = 0;
  MKLIST(*rp,r0);
}

void Pclear_handler(NODE arg,Z *rp)
{
  NODE r0,r,t;
  char *name;
  NODE hlist;
  STRING fname;

  asir_assert(ARG0(arg),O_STR,"clear_handler");
  name = BDY((STRING)ARG0(arg));
  if ( !strcmp(name,"intr") )
    user_int_handler = 0;
  else if ( !strcmp(name,"quit") )
    user_quit_handler = 0;
  else
    error("clear_handler : invalid keyword (must be \"intr\" or \"quit\")");
  *rp = 0;
}

void Pcall(NODE arg,Obj *rp)
{
  P p;
  V v;
    NODE n,n1;
    LIST list;
    VECT vect;
    pointer *a;
    int len,i;

  p = (P)ARG0(arg);
  if ( !p || OID(p) != 2 )
    error("call : invalid argument");
  v = VR(p);
  if ( (long)v->attr != V_SR )  
    error("call : no such function");
  else {
        list = (LIST) ARG1(arg);
        if ( list ) {
            switch (OID(list)) {
            case O_VECT:
                vect = (VECT)list; len = vect->len; a = BDY(vect);
                for ( i = len - 1, n = 0; i >= 0; i-- ) {
                    MKNODE(n1,a[i],n); n = n1;
                }
                MKLIST(list,n);
                /* falling next case */
            case O_LIST:
                *rp = (Obj)bevalf_with_opts((FUNC)v->priv,BDY(list),current_option);
                return;
            default:
                break;
            }
        }           
        error("call : invalid argument");
  }
}

/* at=position of arg to be used for iteration */

void Pmapat(NODE arg,Obj *rp)
{
  LIST args;
  NODE node,rest,t0,t,n,r,r0;
  P fpoly;
  V fvar;
  FUNC f;
  VECT v,rv;
  MAT m,rm;
  LIST rl;
  int len,row,col,i,j,pos;
  Obj iter;
  pointer val;
  NODE option;

  option = current_option;

  if ( argc(arg) < 3 )
    error("mapat : too few arguments");

  fpoly = (P)ARG0(arg);
  if ( !fpoly || OID(fpoly) != O_P )
    error("mapat : invalid function specification");
  fvar = VR(fpoly);
  if ( fvar->attr != (pointer)V_SR || !(f=(FUNC)fvar->priv) )
    error("mapat : invalid function specification");
  if ( !INT(ARG1(arg)) )
    error("mapat : invalid position");
  pos = ZTOS((Q)ARG1(arg));
  node = NEXT(NEXT(arg));
  len = length(node);
  if ( pos >= len )
    error("evalmapatf : invalid position");
  r0 = 0;
  for ( i = 0, t = node; i < pos; i++, t = NEXT(t) ) {
    NEXTNODE(r0,r);
    BDY(r) = BDY(t);
  }
  NEXTNODE(r0,r);
  iter = BDY(t); rest = NEXT(t);
  if ( !iter ) {
    *rp = bevalf_with_opts(f,node,option);
    return;
  }
  switch ( OID(iter) ) {
    case O_VECT:
      v = (VECT)iter; len = v->len; MKVECT(rv,len);
      for ( i = 0; i < len; i++ ) {
        BDY(r) = BDY(v)[i]; NEXT(r) = rest;
        BDY(rv)[i] = bevalf_with_opts(f,r0,option);
      }
      *rp = (Obj)rv;
      break;
    case O_MAT:
      m = (MAT)iter; row = m->row; col = m->col; MKMAT(rm,row,col);
      for ( i = 0; i < row; i++ )
        for ( j = 0; j < col; j++ ) {
          BDY(r) = BDY(m)[i][j]; NEXT(r) = rest;
          BDY(rm)[i][j] = bevalf_with_opts(f,r0,option);
        }
      *rp = (Obj)rm;
      break;
    case O_LIST:
      n = BDY((LIST)iter);
      for ( t0 = t = 0; n; n = NEXT(n) ) {
        BDY(r) = BDY(n); NEXT(r) = rest;
        NEXTNODE(t0,t); BDY(t) = bevalf_with_opts(f,r0,option);
      }
      if ( t0 )
        NEXT(t) = 0;
      MKLIST(rl,t0);
      *rp = (Obj)rl;
      break;
    default:
      *rp = bevalf_with_opts(f,node,option);
      break;
  }
}

/* An implementation of 'map' as builtin function. */
void Pmap(NODE arg,Obj *rp)
{
  LIST args;
  NODE node,rest,t0,t,n,r,r0;
  P fpoly;
  V fvar;
  FUNC f;
  VECT v,rv;
  MAT m,rm;
  LIST rl;
  int len,row,col,i,j;
  Obj iter;
  pointer val;
  NODE option;

  option = current_option;

  if ( argc(arg) < 2 )
    error("mapat : too few arguments");

  fpoly = (P)ARG0(arg);
  if ( !fpoly || OID(fpoly) != O_P )
    error("map : invalid function specification");
  fvar = VR(fpoly);
  if ( fvar->attr != (pointer)V_SR || !(f=(FUNC)fvar->priv) )
    error("map : invalid function specification");

  node = NEXT(arg);
  len = length(node);
  if ( 0 >= len )
    error("evalmapf : invalid position");
  r0 = 0;
  NEXTNODE(r0,r);
  iter = BDY(node); rest = NEXT(node);
  if ( !iter ) {
    *rp = bevalf_with_opts(f,node,option);
    return;
  }
  switch ( OID(iter) ) {
    case O_VECT:
      v = (VECT)iter; len = v->len; MKVECT(rv,len);
      for ( i = 0; i < len; i++ ) {
        BDY(r) = BDY(v)[i]; NEXT(r) = rest;
        BDY(rv)[i] = bevalf_with_opts(f,r0,option);
      }
      *rp = (Obj)rv;
      break;
    case O_MAT:
      m = (MAT)iter; row = m->row; col = m->col; MKMAT(rm,row,col);
      for ( i = 0; i < row; i++ )
        for ( j = 0; j < col; j++ ) {
          BDY(r) = BDY(m)[i][j]; NEXT(r) = rest;
          BDY(rm)[i][j] = bevalf_with_opts(f,r0,option);
        }
      *rp = (Obj)rm;
      break;
    case O_LIST:
      n = BDY((LIST)iter);
      for ( t0 = t = 0; n; n = NEXT(n) ) {
        BDY(r) = BDY(n); NEXT(r) = rest;
        NEXTNODE(t0,t); BDY(t) = bevalf_with_opts(f,r0,option);
      }
      if ( t0 )
        NEXT(t) = 0;
      MKLIST(rl,t0);
      *rp = (Obj)rl;
      break;
    default:
      *rp = bevalf_with_opts(f,node,option);
      break;
  }
}

void Pdeval(NODE arg,Obj *rp)
{
  asir_assert(ARG0(arg),O_R,"deval");
  devalr(CO,(Obj)ARG0(arg),rp);
}

void Peval_quote(NODE arg,Obj *rp)
{
  FNODE a;
  QUOTE q;
  Obj f;

  f = (Obj)ARG0(arg);
  if ( !f || OID(f) != O_QUOTE ) {
    *rp = f;
    return;
  }
  if ( argc(arg) == 2 && ARG1(arg) ) {
    a = partial_eval((FNODE)BDY((QUOTE)ARG0(arg)));
    MKQUOTE(q,a);
    *rp = (Obj)q;
  } else
    *rp = eval((FNODE)BDY((QUOTE)ARG0(arg)));
}
