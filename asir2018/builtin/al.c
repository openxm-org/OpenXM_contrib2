/* $OpenXM: OpenXM_contrib2/asir2018/builtin/al.c,v 1.2 2018/09/28 08:20:27 noro Exp $ */
/* ----------------------------------------------------------------------
   $Id$
   ----------------------------------------------------------------------
   File al.c: Real quantifier elimination code for RISA/ASIR

   Copyright (c) 1996-2001 by
   Andreas Dolzmann and Thomas Sturm, University of Passau, Germany
   dolzmann@uni-passau.de, sturm@uni-passau.de
   ----------------------------------------------------------------------
*/

#include <ca.h>
#include <parse.h>
#include <al.h>

void Preverse();
void Phugo();
void Pex();
void Pall();
void constrq();
void Pfop();
void Pfargs();
void Pfopargs();
void Pcompf();
void Patnum();
int gauss_abc();
int compf();
void Patl();
void Pqevar();
void Pqe();
void Psimpl();
void Psubf();
void Pnnf();
void smkjf();
void simpl();
void simpl1();
void simpl_gand();
void simpl_th2atl();
int simpl_gand_udnargls();
int simpl_gand_thupd();
int simpl_gand_thprsism();
int simpl_gand_smtbdlhs();
int simpl_gand_smtbelhs();
void lbc();
void replaceq();
void deleteq();
void simpl_gand_insert_a();
void simpl_gand_insert_c();
int compaf();
int comprel();
int synequalf();
void simpl_impl();
void simpl_equiv();
void simpl_a();
void simpl_a_o();
void simpl_a_no();
void qe();
void blocksplit();
void qeblock();
int qeblock_verbose1a();
void qeblock_verbose1b();
void qeblock_verbose2();
void qeblock_verbose0();
int getmodulus();
int qevar();
int gausselim();
int delv();
int translate();
int translate_a();
void translate_a1();
void mklgp();
void translate_a2();
void mkqgp();
void getqcoeffs();
void mkdiscr();
int al_reorder();
void indices();
void mkeset();
int selectside();
int cmp2n();
void add2eset();
void seproots();
void sp_add2eset();
void subgpf();
void subref();
void subref_a();
void substd_a();
void substd_a1();
void substd_a2();
void substd_a21();
void substd_a21_equal();
void substd_a21_leq();
void substd_a21_lessp();
void getrecoeffs();
void subinf_a();
void subinf_a_o();
void subinf_a_o1();
void subtrans_a_no();
void subpme_a();
void subpme_a_o();
void subpme_a_o1();
int comember();
void coadd();
int coget();
int colen();
void apply2ats();
void atl();
void atl1();
void atnum();
void atnum1();
void pnegate();
void subf();
void subf_a();
void nnf();
void nnf1();
void ap();
void freevars();
void freevars1();
void freevars1_a();
void rep();
void gpp();
void esetp();
void nodep();
void gauss_mkeset1();
void gauss_mkeset2();

extern int Verbose;

struct oRE {
  P p;
  P discr;
        int rootno;
  int itype;
};

typedef struct oRE *RE;

struct oGP {
  F g;
  RE p;
};

typedef struct oGP *GP;

struct oCEL {
  VL vl;
  F mat;
};

typedef struct oCEL *CEL;

struct oCONT {
  NODE first;
  NODE last;
};

typedef struct oCONT *CONT;

struct oQVL {
  oFOP q;
  VL vl;
};

typedef struct oQVL *QVL;

#define GUARD(x) ((x)->g)
#define POINT(x) ((x)->p)
#define NEWGP(x) ((x)=(GP)MALLOC(sizeof(struct oGP)))
#define MKGP(x,g,p) (NEWGP(x),GUARD(x)=g,POINT(x)=p)

#define NEWRE(x) ((x)=(RE)MALLOC(sizeof(struct oRE)))
#define MKRE(x,pp,d,rn,i) \
(NEWRE(x),(x)->p=pp,(x)->discr=d,(x)->rootno=rn,(x)->itype=i)
#define DISC(re) ((re)->discr)
#define ROOTNO(re) ((re)->rootno)
#define ITYPE(re) ((re)->itype)

#define STD 0
#define EPS 1
#define PEPS 2
#define MEPS -2
#define PINF 3
#define MINF -3

#define NEWQVL(x) ((x)=(QVL)MALLOC(sizeof(struct oQVL)))
#define MKQVL(x,qq,vvl) (NEWQVL(x),(x)->q=qq,(x)->vl=vvl)
#define VARL(x) (x)->vl
#define QUANT(x) (x)->q

#define NUMBER(p) (p==0 || NUM(p))
#define NZNUMBER(p) (p && NUM(p))

#define MKVL(a,b,c) \
(NEWVL(a),(a)->v=(V)b,NEXT(a)=(VL)(c))
#define NEXTVL(r,c) \
if(!(r)){NEWVL(r);(c)=(r);}else{NEWVL(NEXT(c));(c)=NEXT(c);}

#define NEWCEL(x) ((x)=(CEL)MALLOC(sizeof(struct oCEL)))
#define MKCEL(x,vvl,mmat) (NEWCEL(x),(x)->vl=vvl,(x)->mat=mmat)
#define VRL(x) ((x)->vl)

#define FIRST(x) ((x)->first)
#define LAST(x) ((x)->last)
#define MKCONT(x) ((x)=(CONT)MALLOC(sizeof(struct oCONT)),FIRST(x)=LAST(x)=NULL)

struct ftab al_tab[] = {
  {"simpl",Psimpl,-2},
  {"ex",Pex,-2},
  {"all",Pall,-2},
  {"fop",Pfop,1},
  {"fargs",Pfargs,1},
  {"fopargs",Pfopargs,1},
  {"compf",Pcompf,2},
  {"atl",Patl,1},
  {"qevar",Pqevar,2},
  {"qe",Pqe,1},
  {"atnum",Patnum,1},
  {"subf",Psubf,3},
  {"nnf",Pnnf,1},
  {"hugo",Phugo,4},
  {0,0,0}
};

void Phugo(arg,rp)
NODE arg;
F *rp;
{
  substd_a21_equal(BDY(arg),BDY(NEXT(arg)),BDY(NEXT(NEXT(arg))),BDY(NEXT(NEXT(NEXT(arg)))),rp);
  ap(*rp);
  substd_a21_leq(BDY(arg),BDY(NEXT(arg)),BDY(NEXT(NEXT(arg))),BDY(NEXT(NEXT(NEXT(arg)))),rp);
  ap(*rp);
  substd_a21_lessp(BDY(arg),BDY(NEXT(arg)),BDY(NEXT(NEXT(arg))),BDY(NEXT(NEXT(NEXT(arg)))),rp);
  ap(*rp);
}

void Pex(arg,rp)
NODE arg;
F *rp;
{
  if (argc(arg) == 1)
    constrq(AL_EX,0,(F)BDY(arg),rp);
  else
    constrq(AL_EX,BDY(arg),(F)BDY(NEXT(arg)),rp);
}

void Pall(arg,rp)
NODE arg;
F *rp;
{
  if (argc(arg) == 1)
    constrq(AL_ALL,0,(F)BDY(arg),rp);
  else
    constrq(AL_ALL,BDY(arg),(F)BDY(NEXT(arg)),rp);
}

void constrq(q,vars,m,rp)
oFOP q;
Obj vars;
F m,*rp;
{
  VL sc;
  NODE varl=NULL,varlc,arg;
  P p;

  if (!vars) {
    for (freevars(m,&sc); sc; sc=NEXT(sc)) {
      NEXTNODE(varl,varlc);
      MKV(VR(sc),p);
      BDY(varlc) = (pointer)p;
    }
  } else if (OID(vars) == O_LIST) {
    MKNODE(arg,vars,NULL);
    Preverse(arg,&vars);
    varl = BDY((LIST)vars);
  } else
    MKNODE(varl,vars,NULL);
  for (; varl; varl=NEXT(varl)) {
    MKQF(*rp,q,VR((P)BDY(varl)),m);
    m = *rp;
  }
}

void Pfop(arg,rp)
NODE arg;
Z *rp;
{
  oFOP op;

  op = FOP((F)ARG0(arg));
  STOZ((int)op,*rp);
}

void Pfargs(arg,rp)
NODE arg;
LIST *rp;
{
  oFOP op;
  LIST l;
  NODE n1,n2;
  F f;
  P x;

  f = (F)ARG0(arg);
  op = FOP(f);
  if ( AL_TVAL(op) )
    n1 = 0;
  else if ( AL_JUNCT(op) )
    n1 = FJARG(f);
  else if ( AL_QUANT(op) ) {
    MKV(FQVR(f),x);
    MKNODE(n2,FQMAT(f),0); MKNODE(n1,x,n2);
  } else if (AL_ATOMIC(op) )
    MKNODE(n1,FPL(f),0);
  else if ( AL_UNI(op) ) 
    MKNODE(n1,FARG(f),0);
  else if ( AL_EXT(op) ) {
    MKNODE(n2,FRHS(f),0); MKNODE(n1,FLHS(f),n2);
  }
  MKLIST(l,n1);
  *rp = l;
}

void Pfopargs(arg,rp)
NODE arg;
LIST *rp;
{
  oFOP op;
  LIST l;
  NODE n0,n1,n2;
  F f;
  P x;
  Z op1;

  f = (F)ARG0(arg);
  op = FOP(f);
  STOZ((int)op,op1);
  if ( AL_TVAL(op) )
    n1 = 0;
  else if ( AL_JUNCT(op) )
    n1 = FJARG(f);
  else if ( AL_QUANT(op) ) {
    MKV(FQVR(f),x);
    MKNODE(n2,FQMAT(f),0); MKNODE(n1,x,n2);
  } else if (AL_ATOMIC(op) )
    MKNODE(n1,FPL(f),0);
  else if ( AL_UNI(op) ) 
    MKNODE(n1,FARG(f),0);
  else if ( AL_EXT(op) ) {
    MKNODE(n2,FRHS(f),0); MKNODE(n1,FLHS(f),n2);
  }
  MKNODE(n0,op1,n1);
  MKLIST(l,n0);
  *rp = l;
}

void Pcompf(arg,rp)
NODE arg;
Z *rp;
{
  STOZ(compf(CO,BDY(arg),BDY(NEXT(arg))),*rp);
}

void Patnum(arg,rp)
NODE arg;
Q *rp;
{
  atnum(BDY(arg),rp);
}

void Patl(arg,rp)
NODE arg;
LIST *rp;
{
  NODE h;

  atl(BDY(arg),&h);
  MKLIST(*rp,h);
}

void Pqevar(arg,rp)
NODE arg;
F *rp;
{
  qevar(BDY(arg),VR((P)BDY(NEXT(arg))),rp);
}

void Pqe(arg,rp)
NODE arg;
F *rp;
{
  qe(BDY(arg),rp);
}

void Psubf(arg,rp)
NODE arg;
F *rp;
{
  subf(CO,(F)BDY(arg),VR((P)BDY(NEXT(arg))),(P)BDY(NEXT(NEXT(arg))),rp);
}

void Pnnf(arg,rp)
NODE arg;
F *rp;
{
  nnf((F)BDY(arg),rp);
}

/* Simplification */

/* Return values of simpl_gand_udnargls() */
#define GINCONSISTENT 0
#define OK 1
#define NEWAT 2

/* Return values of THPRSISM() */
#define CONTINUE 10
#define DROP 11
#define REPLACE 12
#define KILL 13

void Psimpl(argl,rp)
NODE argl;
F *rp;
{
  if (argc(argl) == 1)
    simpl(BDY(argl),(NODE)NULL,rp);
  else
    simpl(BDY(argl),BDY((LIST)BDY(NEXT(argl))),rp);
}

void smkjf(pf,j,argl)
F *pf;
oFOP j;
NODE argl;
{
  if (!argl)
    MKTV(*pf,AL_NEUTRAL(j));
  else if (!NEXT(argl))
    *pf = (F)BDY(argl);
  else
    MKJF(*pf,j,argl);
}

void simpl(f,th,pnf)
F f,*pnf;
NODE th;
{
  simpl1(f,th,0,pnf);
}

void simpl1(f,th,n,pnf)
F f,*pnf;
NODE th;
int n;
{
  F h;
  oFOP op=FOP(f);

  if (AL_ATOMIC(op)) {
    simpl_a(f,pnf);
    return;
  }
  if (AL_JUNCT(op)) {
    simpl_gand(op,AL_NEUTRAL(op),AL_OMNIPOT(op),FJARG(f),th,n,pnf);
    return;
  }
  if (AL_TVAL(op)) {
    *pnf = f;
    return;
  }
  if (AL_QUANT(op)) {
    simpl1(FQMAT(f),(NODE)NULL,n+1,&h);
    MKQF(*pnf,op,FQVR(f),h);
    return;
  }
  if (op == AL_NOT) {
    simpl1(FARG(f),th,n+1,&h);
    switch (FOP(h)) {
    case AL_TRUE:
      *pnf = F_FALSE;
      break;
    case AL_FALSE:
      *pnf = F_TRUE;
      break;
    default:
      MKUF(*pnf,AL_NOT,h);
    }
    return;
  }
  if (op == AL_IMPL) {
    simpl_impl(AL_IMPL,FLHS(f),FRHS(f),th,n,pnf);
    return;
  }
  if (op == AL_REPL) {
    simpl_impl(AL_REPL,FRHS(f),FLHS(f),th,n,pnf);
    return;
  }
  if (op == AL_EQUIV) {
    simpl_equiv(FLHS(f),FRHS(f),th,n,pnf);
    return;
  }
  else
    error("unknown operator in simpl1");
}

void simpl_gand(gand,gtrue,gfalse,argl,oth,n,pnf)
oFOP gand,gtrue,gfalse;
NODE argl,oth;
int n;
F *pnf;
{
  NODE cnargl=NULL,cc=NULL,cnargl2=NULL,cc2=NULL,th=NULL,thc=NULL,nargl,narglc;
  F fgfalse,h;
  int st;

  for (; oth; oth = NEXT(oth)) {
   NEXTNODE(th,thc);
   BDY(thc) = BDY(oth);
  }
  for (; argl; argl = NEXT(argl)) {
    if (FOP((F)BDY(argl)) == gfalse) {
    *pnf = (F)BDY(argl);
      return;
   }
    if (AL_ATOMIC(FOP((F)BDY(argl)))) {
      simpl_a((F)BDY(argl),&h);
      if (FOP(h) == gfalse) {
      *pnf = h;
      return;
    }
      st = simpl_gand_udnargls(gand,gtrue,h,n,&th,&thc,&cnargl,&cc);
      if (st == GINCONSISTENT) {
      MKTV(fgfalse,gfalse);
      *pnf = fgfalse;
      return;
    }
    } else
    simpl_gand_insert_c((F)BDY(argl),&cnargl,&cc);
  }   
  for (; cnargl != NULL; cnargl = NEXT(cnargl)) {
    simpl1((F)BDY(cnargl),th,n+1,&h);
    if (FOP(h) == gfalse) {
    *pnf = h;
      return;
   }
    st = simpl_gand_udnargls(gand,gtrue,h,n,&th,&thc,&cnargl2,&cc2);
    switch (st) {
    case GINCONSISTENT:
    MKTV(fgfalse,gfalse);
    *pnf = fgfalse;
    return;
    case NEWAT:
      if (cnargl2 != NULL) {
      if (cnargl != NULL)
       NEXT(cc) = cnargl2;
      else
       cnargl = cnargl2;
      cc = cc2;
      cnargl2 = cc2 = NULL;
    }
      break;
    }
  }
  simpl_th2atl(gand,th,n,&nargl,&narglc);
  if (nargl == NULL)
   nargl = cnargl2;
  else
   NEXT(narglc) = cnargl2;
  smkjf(pnf,gand,nargl);
}

void simpl_th2atl(gand,th,n,patl,patlc)
oFOP gand;
NODE th,*patl,*patlc;
int n;
{
  NODE atl=NULL,atlc=NULL;
  F at,negat;

  switch (gand) {
  case AL_AND:
   for (; th; th = NEXT(th)) {
    if (LBFLB((LBF)BDY(th)) == n) {
      NEXTNODE(atl,atlc);
      BDY(atlc) = (pointer)LBFF((LBF)BDY(th));
    }
   }
   break;
  case AL_OR:
   for (; th; th = NEXT(th)) {
    if (LBFLB((LBF)BDY(th)) == n) {
      at = LBFF((LBF)BDY(th));
      MKAF(negat,AL_LNEGOP(FOP(at)),FPL(at));
      NEXTNODE(atl,atlc);
      BDY(atlc) = (pointer)negat;
    }
   }
   break;
  }
  *patl = atl;
  *patlc = atlc;
}

int simpl_gand_udnargls(gand,gtrue,narg,n,pth,pthc,pcnargl,pcc)
oFOP gand,gtrue;
F narg;
int n;
NODE *pth,*pthc,*pcnargl,*pcc;
{
  NODE sargl;
  F h;
  oFOP op;
  int st,found=OK;

  op = FOP(narg);
  if (op == gtrue)
    return(OK);
  if (AL_ATOMIC(op))
    return(simpl_gand_thupd(gand,narg,n,pth,pthc));
  if (op == gand) {
    sargl = FJARG(narg);
   for (; sargl; sargl = NEXT(sargl)) {
    h = (F)BDY(sargl);
      if (AL_ATOMIC(FOP(h))) {
      st = simpl_gand_thupd(gand,h,n,pth,pthc);
      switch (st) {
      case NEWAT:
       found = NEWAT;
       break;
      case GINCONSISTENT:
       return(GINCONSISTENT);
      }
      } else
      simpl_gand_insert_c(h,pcnargl,pcc);
    }
    return(found);
  }
  simpl_gand_insert_c(narg,pcnargl,pcc);
  return(OK);
}

int simpl_gand_thupd(top,at,n,pth,pthc)
oFOP top;
F at;
int n;
NODE *pth,*pthc;
{
  LBF atpr,thpr;
  NODE scth;
  int st;
  F h;
  
  if (top == AL_OR) {
    MKAF(h,AL_LNEGOP(FOP(at)),FPL(at));
    at = h;
  }
  MKLBF(atpr,at,n);
  for (scth = *pth; scth; scth = NEXT(scth)) {
    thpr = (LBF)BDY(scth);
    st = simpl_gand_thprsism(thpr,&atpr);
    switch (st) {
    case GINCONSISTENT:
      return(GINCONSISTENT);
    case DROP:
      return(OK);
    case REPLACE:
/*      replaceq(*pth,(pointer)thpr,(pointer)atpr,pth,pthc); */
/*      return(NEWAT); */
    case KILL:
      deleteq(*pth,(pointer)thpr,(pointer)pth,pthc);
    }
  }
  NEXTNODE(*pth,*pthc);
  BDY(*pthc) = (pointer)atpr;
  return(NEWAT);
}

int simpl_gand_thprsism(thpr,patpr)
LBF thpr,*patpr;
{
  P thlbc,atlbc,thlhs1,atlhs1,difference;
  oFOP natfop;
  F nat;
  LBF natpr;
  int st;

  lbc(FPL(LBFF(*patpr)),&atlbc);
  mulp(CO,FPL(LBFF(thpr)),atlbc,&thlhs1);
  lbc(FPL(LBFF(thpr)),&thlbc);
  mulp(CO,FPL(LBFF(*patpr)),thlbc,&atlhs1);
  subp(CO,thlhs1,atlhs1,&difference);
  if (!NUMBER(difference))
    return(CONTINUE);
  if (difference == NULL) {
    st = simpl_gand_smtbelhs(FOP(LBFF(thpr)),FOP(LBFF(*patpr)),&natfop);
    if (st == REPLACE) {
      MKAF(nat,natfop,FPL(LBFF(*patpr)));
      MKLBF(natpr,nat,LBFLB(*patpr));
      *patpr = natpr;
    };
    return(st);
  }
  return(simpl_gand_smtbdlhs(FOP(LBFF(thpr)),FOP(LBFF(*patpr)),difference));
}

int simpl_gand_smtbelhs(thop,atop,pnatop)
     oFOP thop,atop,*pnatop;
{
  if (atop == thop)
    return(DROP);

  switch (thop) {
  case AL_EQUAL:
    switch (atop) {
    case AL_NEQ:
    case AL_LESSP:
    case AL_GREATERP:
      return(GINCONSISTENT);
    case AL_LEQ:
    case AL_GEQ:
      return(DROP);
    }
  case AL_NEQ:
    switch (atop) {
    case AL_EQUAL:
      return(GINCONSISTENT);
    case AL_LEQ:
      *pnatop = AL_LESSP;
      return(REPLACE);
    case AL_GEQ:
      *pnatop = AL_GREATERP;
      return(REPLACE);
    case AL_LESSP:
    case AL_GREATERP:
      *pnatop = atop;
      return(REPLACE);
    }
  case AL_LEQ:
    switch (atop) {
    case AL_EQUAL:
    case AL_GEQ:
      *pnatop = AL_EQUAL;
      return(REPLACE);
    case AL_NEQ:
    case AL_LESSP:
      *pnatop = AL_LESSP;
      return(REPLACE);
    case AL_GREATERP:
      return(GINCONSISTENT);
    }
  case AL_GEQ:
    switch (atop) {
    case AL_EQUAL:
    case AL_LEQ:
      *pnatop = AL_EQUAL;
      return(REPLACE);
    case AL_NEQ:
    case AL_GREATERP:
      *pnatop = AL_GREATERP;
      return(REPLACE);
    case AL_LESSP:
      return(GINCONSISTENT);
    }
  case AL_LESSP:
    switch (atop) {
    case AL_EQUAL:
    case AL_GEQ:
    case AL_GREATERP:
      return(GINCONSISTENT);
    case AL_NEQ:
    case AL_LEQ:
      return(DROP);
    }
  case AL_GREATERP:
    switch (atop) {
    case AL_EQUAL:
    case AL_LEQ:
    case AL_LESSP:
      return(GINCONSISTENT);
    case AL_NEQ:
    case AL_GEQ:
      return(DROP);
    }
  }
  /* XXX */
  return 0;
}

int simpl_gand_smtbdlhs(thop,atop,difference)
     oFOP thop,atop;
     P difference;
{
  oFOP op1,op2;
  int drop1,drop2;

  if (cmpz((Z)difference,0) == 1) {  /* good luck with the next compiler */
    op1 = atop;
    op2 = thop;
    drop1 = DROP;
    drop2 = KILL;
  } else {
    op1 = thop;
    op2 = atop;
    drop1 = KILL;
    drop2 = DROP;
  }
  switch (op1) {
  case AL_EQUAL:
    switch (op2) {
    case AL_EQUAL:
    case AL_LEQ:
    case AL_LESSP:
      return(GINCONSISTENT);
    default:
      return(drop2);
    }
  case AL_NEQ:
  case AL_LEQ:
  case AL_LESSP:
    switch (op2) {
    case AL_EQUAL:
    case AL_LEQ:
    case AL_LESSP:
      return(drop1);
    default:
      return(CONTINUE);
    }
  case AL_GEQ:
    switch(op2) {
    case AL_EQUAL:
    case AL_LEQ:
    case AL_LESSP:
      return(GINCONSISTENT);
    default:
      return(drop2);
    }
  case AL_GREATERP:
    switch (op2) {
    case AL_EQUAL:
    case AL_LEQ:
    case AL_LESSP:
      return(GINCONSISTENT);
    default:
      return(drop2);
    }
  }
  /* XXX */
  return 0;
}

void lbc(f,pc)
P f,*pc;
{
  for (*pc = f; !NUM(*pc); *pc = COEF(DC(*pc)))
    ;
}

void replaceq(l,old,new,pnl,pnlc)
NODE l,*pnl,*pnlc;
pointer old,new;
{
  *pnl = NULL;
  for (; l; l = NEXT(l)) {
   NEXTNODE(*pnl,*pnlc);
   if(BDY(l) == old)
    BDY(*pnlc) = new;
   else
    BDY(*pnlc) = BDY(l);
  }
}

void deleteq(l,obj,pnl,pnlc)
NODE l,*pnl,*pnlc;
pointer obj;
{
  *pnl = NULL;
  for (; l; l = NEXT(l))
   if(BDY(l) != obj) {
    NEXTNODE(*pnl,*pnlc);
    BDY(*pnlc) = BDY(l);
   }
}

void simpl_gand_insert_a(f,paargl,pac)
F f;
NODE *paargl,*pac;
{
  int w;
  NODE n,sc,prev;

  if (*paargl == 0) {
    NEXTNODE(*paargl,*pac);
    BDY(*pac) = (pointer)f;
    return;
  }
  w = compaf(CO,BDY(*pac),f);
  if (w == 1) {
    NEXTNODE(*paargl,*pac);
    BDY(*pac) = (pointer)f;
    return;
  }
  if (w == 0)
    return;
  w = compaf(CO,f,BDY(*paargl));
  if (w == 1) {
    MKNODE(n,f,*paargl);
    *paargl = n;
    return;
  }
  if (w == 0)
    return;
  /* f belongs strictly inside the existing list */
  for (sc=*paargl; (w=compaf(CO,BDY(sc),f))==1; sc=NEXT(sc))
    prev = sc;
  if (w == 0)
    return;
  MKNODE(n,f,sc);
  NEXT(prev) = n;
}

void simpl_gand_insert_c(f,pcargl,pcc)
F f;
NODE *pcargl,*pcc;
{
  NODE sc;

  for (sc=*pcargl; sc; sc=NEXT(sc))
    if (synequalf(f,(F)BDY(sc)))
      return;
  NEXTNODE(*pcargl,*pcc);
  BDY(*pcc) = (pointer)f;
}

int compaf(vl,f1,f2)
VL vl;
F f1,f2;
{
  int w;

  w = compp(vl,FPL(f1),FPL(f2));
  if (w)
    return w;
  return comprel(FOP(f1),FOP(f2));
}

int comprel(op1,op2)
oFOP op1,op2;
/* implement order: =, <>, <=, <, >=, > */
{
  if (op1 == op2)
    return 0;
  switch (op1) {
  case AL_EQUAL:
    return 1;
  case AL_NEQ:
    switch (op2) {
    case AL_EQUAL:
      return -1;
    default:
      return 1;
    }
  case AL_LEQ:
    switch (op2) {
    case AL_EQUAL:
      return -1;
    case AL_NEQ:
      return -1;
    default:
      return 1;
    }
  case AL_LESSP:
    switch (op2) {
    case AL_GEQ:
      return 1;
    case AL_GREATERP:
      return 1;
    default:
      return -1;
    }
  case AL_GEQ:
    switch (op2) {
    case AL_GREATERP:
      return 1;
    default:
      return -1;
    }
  case AL_GREATERP:
    return -1;
  }
  error("unknown relation in comprel");
  return 0;
}

int synequalf(f1,f2)
F f1,f2;
{
  oFOP op=FOP(f1);

  if (op != FOP(f2))
    return 0;
  if (AL_ATOMIC(op))
    return (compp(CO,FPL(f1),FPL(f2)) == 0);
  if (AL_JUNCT(op)) {
    NODE sc1,sc2;
    for (sc1=FJARG(f1),sc2=FJARG(f2); sc1 && sc2; sc1=NEXT(sc1),sc2=NEXT(sc2))
      if (! synequalf(BDY(sc1),BDY(sc2)))
        return 0;
    if (sc1 || sc2)
      return 0;
    return 1;
  }
  /* XXX */
  return 0;
}

void simpl_impl(op,prem,concl,th,n,pf)
oFOP op;
F prem,concl,*pf;
NODE th;
int n;
{
  F h,hh;
  
  simpl1(prem,th,n+1,&h);
  if (FOP(h) == AL_FALSE) {
    *pf = F_TRUE;
    return;
  }
  simpl1(concl,th,n+1,&hh);
  if (FOP(hh) == AL_TRUE) {
    *pf = F_TRUE;
    return;
  }
  if (FOP(h) == AL_TRUE) {
    *pf = hh;
    return;
  }
  if (FOP(hh) == AL_FALSE) {
    pnegate(h,pf);
    return;
  }
  if (op == AL_IMPL) {
    MKBF(*pf,AL_IMPL,h,hh);
    return;
  }
  MKBF(*pf,AL_REPL,hh,h);
}

void simpl_equiv(lhs,rhs,th,n,pf)
F lhs,rhs,*pf;
NODE th;
int n;
{
  F h,hh;

  simpl1(lhs,th,n+1,&h);
  simpl1(rhs,th,n+1,&hh);
  if (FOP(h) == AL_TRUE) {
    *pf = hh;
    return;
  }
  if (FOP(hh) == AL_TRUE) {
    *pf = h;
    return;
  }
  if (FOP(h) == AL_FALSE) {
    pnegate(hh,pf);
    return;
  }
  if (FOP(hh) == AL_FALSE) {
    pnegate(h,pf);
    return;
  }
  MKBF(*pf,AL_EQUIV,h,hh);
}

void simpl_a(f,pnf)
F f,*pnf;
{
  oFOP r=FOP(f);
  P lhs=(P)FPL(f);

  if (NUMBER(lhs)) {
#if 0
     lhs = (Q)lhs; /* good luck with the next compiler */
#endif
    switch (r) {
    case AL_EQUAL:
      *pnf = (lhs == 0) ? F_TRUE : F_FALSE;
      return;
    case AL_NEQ:
      *pnf = (lhs != 0) ? F_TRUE : F_FALSE;
      return;
    case AL_LESSP:
      *pnf = (cmpq((Q)lhs,0) == -1) ? F_TRUE : F_FALSE;
      return;
    case AL_GREATERP:
      *pnf = (cmpq((Q)lhs,0) == 1) ? F_TRUE : F_FALSE;
      return;
    case AL_LEQ:
      *pnf = (cmpq((Q)lhs,0) != 1) ? F_TRUE : F_FALSE;
      return;
    case AL_GEQ:
      *pnf = (cmpq((Q)lhs,0) != -1) ? F_TRUE : F_FALSE;
      return;
    default:
      error("unknown operator in simpl_a");
    }
  }
  if (AL_ORDER(r))
    simpl_a_o(&r,&lhs);
  else
    simpl_a_no(&lhs);
  MKAF(*pnf,r,lhs);
}

void simpl_a_o(ar,alhs)
oFOP *ar;
P *alhs;
{
  DCP dec;

  sqfrp(CO,*alhs,&dec);
  if (sgnq((Q)COEF(dec)) == -1)
    *ar = AL_ANEGREL(*ar);
  *alhs=(P)ONE;
  for (dec = NEXT(dec); dec; dec = NEXT(dec)) {
    mulp(CO,*alhs,COEF(dec),alhs);
    if (evenz(DEG(dec)))
      mulp(CO,*alhs,COEF(dec),alhs);
  }
}

void simpl_a_no(alhs)
P *alhs;
{
  DCP dec;

  sqfrp(CO,*alhs,&dec);
  *alhs=(P)ONE;
  for (dec = NEXT(dec); dec; dec = NEXT(dec))
    mulp(CO,*alhs,COEF(dec),alhs);
}

/* QE */

#define BTMIN 0
#define BTEQUAL 0
#define BTWO 1
#define BTLEQ 2
#define BTGEQ 3
#define BTSO 4
#define BTLESSP 5
#define BTGREATERP 6
#define BTNEQ 7
#define BTMAX 7

#define IPURE 0
#define IPE 1
#define IME 2
#define II 3

void qe(f,pnf)
F f,*pnf;
{
  NODE bl,sc;
  F h;

  simpl(f,(NODE)NULL,&h);
  nnf(h,&h);
  blocksplit(h,&bl,&h);
  for (sc=bl; sc; sc=NEXT(sc)) {
    if (QUANT(((QVL)BDY(sc))) == AL_EX)
      qeblock(VARL(((QVL)BDY(sc))),h,&h);
    else {
      pnegate(h,&h);
      qeblock(VARL(((QVL)BDY(sc))),h,&h);
      pnegate(h,&h);
    }
  }
  *pnf = h;
}

void blocksplit(f,pbl,pmat)
F f,*pmat;
NODE *pbl;
{
  oFOP cq;
  NODE bl=NULL,blh;
  VL vl,vlh;
  QVL qvl;

  while (AL_QUANT(cq=FOP(f))) {
    NEWNODE(blh);
    vl = NULL;
    while (FOP(f) == cq) {
      NEWVL(vlh);
      VR(vlh) = FQVR(f);
      NEXT(vlh) = vl;
      vl = vlh;
      f = FQMAT(f);
    }
    MKQVL(qvl,cq,vl);
    BDY(blh) = (pointer)qvl;
    NEXT(blh) = bl;
    bl = blh;
  }
  *pbl = bl;
  *pmat = f;
}

void qeblock(vl,f,pnf)
VL vl;
F f,*pnf;
{
  CONT co;
  CEL cel;
  VL cvl;
  NODE n,sc;
  NODE nargl=NULL,narglc;
  int w,pr;
  int left=0,modulus;

  qeblock_verbose0(vl);
  simpl(f,(NODE)NULL,&f);
  MKCONT(co);
  MKCEL(cel,vl,f);
  coadd(co,cel);
  while (coget(co,&cel)) {
    cvl = VRL(cel);
    pr = qeblock_verbose1a(co,cvl,&left,&modulus);
    w = qevar(MAT(cel),&cvl,&n);
    qeblock_verbose1b(w,pr);
    for (sc=n; sc; sc=NEXT(sc))
      if ((F)BDY(sc) != F_FALSE) {
        if (cvl) {
          MKCEL(cel,cvl,(F)BDY(sc));
          if (!comember(co,cel))
            coadd(co,cel);
        } else {
          NEXTNODE(nargl,narglc);
          BDY(narglc) = BDY(sc);
        }
      }
  }
  qeblock_verbose2();
  smkjf(pnf,AL_OR,nargl);
  simpl(*pnf,(NODE)NULL,pnf);
}

void qeblock_verbose0(vl)
VL vl;
{
  if (!Verbose)
    return;
  printf("eliminating");
  for (; vl; vl=NEXT(vl))
    printf(" %s",NAME(VR(vl)));
}

int qeblock_verbose1a(co,cvl,pleft,pmodulus)
CONT co;
VL cvl;
int *pleft,*pmodulus;
{
  int i=0;
  
  if (!Verbose)
  /* added by noro */
    return 0;
  if (*pleft == 0) {
    for (; cvl; cvl=NEXT(cvl))
      i++;
    printf("\nleft %d\n",i);
    *pleft = colen(co) + 1;
    *pmodulus = getmodulus(*pleft);
    printf("(%d",(*pleft)--);
    fflush(asir_out);
    return 1;
  } else if (*pleft % *pmodulus == 0) {
    printf("(%d",(*pleft)--);
    fflush(asir_out);
    return 1;
  }
  (*pleft)--;
  return 0;
}

void qeblock_verbose1b(g,print)
int g,print;
{
  if (!(Verbose && print))
    return;
  printf("%s) ",g ? (g==2) ? "qg" : "lg" : "e");
  fflush(asir_out);
}

void qeblock_verbose2()
{
  if (!Verbose)
    return;
  printf("\n");
}

int getmodulus(n)
int n;
{
  int pow=1;
  
  while (n >= pow*100) {
    pow *= 10;
  }
  return pow;
}


int qevar(f,pcvl,pfl)
F f;
VL *pcvl;
NODE *pfl;
{
  int w;
  V x;
  F h;
  NODE trans[8],eset,sc,r=NULL,rc;

  w = gausselim(f,pcvl,&x,&eset);
  if (!w) {
    x = VR(*pcvl);
    *pcvl = NEXT(*pcvl);
    translate(f,x,trans);
    mkeset(trans,x,&eset);
  }
  for (sc=eset; sc; sc=NEXT(sc)) {
    NEXTNODE(r,rc);
    subgpf(f,x,BDY(sc),&h);
    simpl(h,(NODE)NULL,(F *)BDY(rc));
  }
  *pfl = r;
  return w;
}

int gausselim(f,pvl,px,peset)
F f;
VL *pvl;
V *px;
NODE *peset;
{
  Z deg,two;
  P rlhs,a,b,c;
  V v;
  VL scvl;
  NODE sc;
  int w;

  if (FOP(f) != AL_AND)
    return 0;
  STOZ(2,two);
  for (deg=ONE; cmpz(two,deg) >= 0; addz(deg,ONE,&deg))
    for (scvl=*pvl; scvl; scvl=NEXT(scvl)) {
      v = VR(scvl);
      for (sc=FJARG(f); sc; sc=NEXT(sc)) {
        if (FOP((F)BDY(sc)) != AL_EQUAL)
          continue;
        al_reorder(FPL((F)BDY(sc)),v,&rlhs);
        if (VR(rlhs) != v)
          continue;
        w = gauss_abc(rlhs,v,deg,&a,&b,&c);
        if (!w)
          continue;
        *px = v;
        delv(v,*pvl,pvl);
        if (a) {
          gauss_mkeset2(rlhs,a,b,c,peset);
          return 2;
        }
        gauss_mkeset1(rlhs,b,peset);
        return 1;
      }
    }
  return 0;
}

int gauss_abc(rlhs,v,deg,pa,pb,pc)
P rlhs,*pa,*pb,*pc;
V v;
Z deg;
{
  Z two;
  DCP rld;
  
  rld = DC(rlhs);
  if (cmpz(DEG(rld),deg) != 0)
    return 0;
  STOZ(2,two);
  if (cmpz(deg,two) == 0) {
    *pa = COEF(rld);
    rld = NEXT(rld);
  } else
    *pa = 0;
  if (rld && cmpz(DEG(rld),ONE) == 0) {
    *pb = COEF(rld);
    rld = NEXT(rld);
  } else
    *pb = 0;
  if (rld)
    *pc = COEF(rld);
  else
    *pc = 0;
  return (NZNUMBER(*pa) || NZNUMBER(*pb) || NZNUMBER(*pc));
}

void gauss_mkeset1(rlhs,b,peset)
P rlhs,b;
NODE *peset;
{
  GP hgp;

  mklgp(rlhs,b,STD,&hgp);
  MKNODE(*peset,hgp,NULL);
}

void gauss_mkeset2(rlhs,a,b,c,peset)
P rlhs,a,b,c;
NODE *peset;
{
  GP hgp;
  NODE esetc=NULL;

  *peset = NULL;
  if (!NUM(a)) {
    NEXTNODE(*peset,esetc);
    mklgp(rlhs,b,STD,&hgp);
    BDY(esetc) = (pointer)hgp;
  }
  NEXTNODE(*peset,esetc);
  mkqgp(rlhs,a,b,c,1,STD,&hgp);
  BDY(esetc) = (pointer)hgp;
  NEXTNODE(*peset,esetc);
  mkqgp(rlhs,a,b,c,2,STD,&hgp);
  BDY(esetc) = (pointer)hgp;
}

int delv(v,vl,pnvl)
V v;
VL vl,*pnvl;
{
        VL nvl=NULL,nvlc;

  if (v == VR(vl)) {
          *pnvl = NEXT(vl);
    return 1;
  }
  for (; vl && (VR(vl) != v); vl=NEXT(vl)) {
                NEXTVL(nvl,nvlc);
    VR(nvlc) = VR(vl);
  }
  if (vl) {
                NEXT(nvlc) = NEXT(vl);
    *pnvl = nvl;
    return 1;
  }
  *pnvl = nvl;
  return 0;
}

int translate(f,x,trans)
F f;
V x;
NODE trans[];
{
  NODE sc,transc[8];
  int bt,w=0;

  for (bt=BTMIN; bt<=BTMAX; bt++)
    trans[bt] = NULL;
  for (atl(f,&sc); sc; sc=NEXT(sc))
    w = (translate_a(BDY(sc),x,trans,transc) || w);
  return w;
}

int translate_a(at,v,trans,transc)
F at;
V v;
NODE trans[],transc[];
{
  P mp;
  Z two;
  int w;

  w = al_reorder(FPL(at),v,&mp);
  if (w == 0)
    return 0;
  if (cmpz(ONE,DEG(DC(mp))) == 0) {
    translate_a1(FOP(at),mp,trans,transc);
    return 1;
  };
  STOZ(2,two);
  if (cmpz(two,DEG(DC(mp))) == 0) {
    translate_a2(FOP(at),mp,trans,transc);
    return 1;
  };
  error("degree violation in translate_a");
  /* XXX : NOTREACHED */
  return -1;
}

void translate_a1(op,mp,trans,transc)
oFOP op;
P mp;
NODE trans[],transc[];
{
  P b;
  int itype,btype;
  GP hgp;
  
  b = COEF(DC(mp));
  indices(op,NUM(b) ? sgnq((Q)b) : 0,&itype,&btype);
  NEXTNODE(trans[btype],transc[btype]);
  mklgp(mp,b,itype,&hgp);
  BDY(transc[btype]) = (pointer)hgp;
}

void mklgp(mp,b,itype,pgp)
P mp,b;
int itype;
GP *pgp;
{
  RE hre;
  F hf;
  
  MKRE(hre,mp,(P)ONE,1,itype);
  MKAF(hf,AL_NEQ,b);
  MKGP(*pgp,hf,hre);
}

void translate_a2(op,mp,trans,transc)
oFOP op;
P mp;
NODE trans[],transc[];
{
  P a,b,c,linred;
  int itype,btype;
  GP hgp;

  getqcoeffs(mp,&a,&b,&c);
  if (!NUM(a) && b) {
    MKP(VR(mp),NEXT(DC(mp)),linred);
    translate_a1(op,linred,trans,transc);
  }
  indices(op,0,&itype,&btype);
  NEXTNODE(trans[btype],transc[btype]);
  mkqgp(mp,a,b,c,-1,itype,&hgp);
  BDY(transc[btype]) = (pointer)hgp;
}

void mkqgp(mp,a,b,c,rootno,itype,pgp)
P mp,a,b,c;
int rootno;
int itype;
GP *pgp;
{
  P discr;
  RE hre;
  F hf;
  NODE n=NULL,nc=NULL;
  
  mkdiscr(a,b,c,&discr);
  MKRE(hre,mp,discr,rootno,itype);
  NEXTNODE(n,nc);
  MKAF(hf,AL_NEQ,a);
  BDY(nc) = (pointer)hf;
  NEXTNODE(n,nc);
  MKAF(hf,AL_GEQ,discr);
  BDY(nc) = (pointer)hf;
  MKJF(hf,AL_AND,n);
  MKGP(*pgp,hf,hre);
}

void getqcoeffs(mp,pa,pb,pc)
P mp,*pa,*pb,*pc;
{
  DCP hdcp;
  
  *pa = COEF(DC(mp));
  hdcp = NEXT(DC(mp));
  if (hdcp && cmpz(DEG(hdcp),ONE) == 0) {
    *pb = COEF(hdcp);
    hdcp = NEXT(hdcp);
  } else
    *pb = 0;
  if (hdcp && DEG(hdcp) == 0) {
    *pc = COEF(hdcp);
  } else
    *pc = 0;
}

void mkdiscr(a,b,c,pd)
P a,b,c,*pd;
{
  P h1,h2;
  Z four;
  
  mulp(CO,a,c,&h1);
  STOZ(4,four);
  mulp(CO,(P)four,h1,&h2);
  mulp(CO,b,b,&h1);
  subp(CO,h1,h2,pd);
}

int al_reorder(p,v,pnp)
P p,*pnp;
V v;
{
  VL tvl;

  reordvar(CO,v,&tvl);
  reorderp(tvl,CO,p,pnp);
  if (*pnp && !NUM(*pnp) && strcmp(NAME(VR(*pnp)),NAME(v)) == 0)
    return 1;
  else
    return 0;
}

void indices(op,s,pit,pbt)
oFOP op;
int s,*pit,*pbt;
{
  switch (op) {
  case AL_EQUAL:
    *pit = STD; *pbt = BTEQUAL; return;
  case AL_NEQ:
    *pit = EPS; *pbt = BTNEQ; return;
  case AL_LEQ:
    *pit = STD;
    switch (s) {
    case 1:
      *pbt = BTLEQ; return;
    case -1:
      *pbt = BTGEQ; return;
    case 0:
      *pbt = BTWO; return;
    }
  case AL_GEQ:
    *pit = STD;
    switch (s) {
    case 1:
      *pbt = BTGEQ; return;
    case -1:
      *pbt = BTLEQ; return;
    case 0:
      *pbt = BTWO; return;
    }
  case AL_LESSP:
    switch (s) {
    case 1:
      *pit = MEPS; *pbt = BTLESSP; return;
    case -1:
      *pit = PEPS; *pbt = BTGREATERP; return;
    case 0:
      *pit = EPS; *pbt = BTSO; return;
    }
  case AL_GREATERP:
    switch (s) {
    case 1:
      *pit = PEPS; *pbt = BTGREATERP; return;
    case -1:
      *pit = MEPS; *pbt = BTLESSP; return;
    case 0:
      *pit = EPS; *pbt = BTSO; return;
    }
  default:
    error("unknown relation or sign in indices");
  }
}

void mkeset(trans,x,peset)
NODE trans[],*peset;
V x;
{
  NODE esetc=NULL;
  P h;
  RE hre;
  GP hgp;
  int cw,cs,deps,dinf,ord;

  *peset = NULL;
  ord = selectside(trans,&cw,&cs,&deps,&dinf);
  if (ord) {
    add2eset(trans[cw],peset,&esetc);
    add2eset(trans[BTWO],peset,&esetc);
    add2eset(trans[cs],peset,&esetc);
    sp_add2eset(trans[BTSO],deps,peset,&esetc);
    NEXTNODE(*peset,esetc);
    MKRE(hre,0,0,0,dinf);
    MKGP(hgp,F_TRUE,hre);
    BDY(esetc) = (pointer)hgp;
  } else {
    NEXTNODE(*peset,esetc);
    MKV(x,h);
    MKRE(hre,h,(P)ONE,1,STD);
    MKGP(hgp,F_TRUE,hre);
    BDY(esetc) = (pointer)hgp;
  }
  add2eset(trans[BTEQUAL],peset,&esetc);
  sp_add2eset(trans[BTNEQ],deps,peset,&esetc);
}

int selectside(trans,pcw,pcs,pdeps,pdinf)
NODE trans[];
int *pcw,*pcs,*pdeps,*pdinf;
{
  if (cmp2n(trans[BTLEQ],trans[BTLESSP],trans[BTGEQ],trans[BTGREATERP])==1) {
    *pcw = BTGEQ;
    *pcs = BTGREATERP;
    *pdeps = PEPS;
    *pdinf = MINF;
  } else {
    *pcw = BTLEQ;
    *pcs = BTLESSP;
    *pdeps = MEPS;
    *pdinf = PINF;
  }
  if (!(trans[BTLEQ] || trans[BTLESSP] || trans[BTGEQ] ||
      trans[BTGREATERP] || trans[BTWO] || trans[BTSO]))
    return 0;
  return 1;
}
  
int cmp2n(n1a,n1b,n2a,n2b)
NODE n1a,n1b,n2a,n2b;
{
  NODE n1,n2;
  int n1bleft=1,n2bleft=1;

  n1 = n1a;
  n2 = n2a;
  while (n1 && n2) {
    n1 = NEXT(n1);
    if (n1 == NULL && n1bleft) {
      n1 = n1b;
      n1bleft = 0;
    }
    n2 = NEXT(n2);
    if (n2 == NULL && n2bleft) {
      n2 = n2b;
      n2bleft = 0;
    }
  }
  if (n1 || n2)
    return n1 ? 1 : -1;
  return 0;
}

void add2eset(trfield,peset,pesetc)
NODE trfield,*peset,*pesetc;
{
        NODE ntrfield,ntrfieldc;

  if (trfield == NULL)
    return;
  seproots(trfield,&ntrfield,&ntrfieldc);
  if (*peset == NULL) {
    *peset = ntrfield;
    *pesetc = ntrfieldc;
  } else {
    NEXT(*pesetc) = ntrfield;
    *pesetc = ntrfieldc;
  }
}

void seproots(trfield,pntrfield,pntrfieldc)
NODE trfield,*pntrfield,*pntrfieldc;
{
  NODE sc;
  NODE ntrf=NULL,ntrfc;
  RE hre,hre2;
  GP hgp,hgp2;

  for (sc=trfield; sc; sc=NEXT(sc)) {
    hgp = (GP)BDY(sc);
    hre = POINT(hgp);
    if (ROOTNO(hre) == -1) {
      NEXTNODE(ntrf,ntrfc);
      MKRE(hre2,hre->p,DISC(hre),1,ITYPE(hre));
      MKGP(hgp2,GUARD(hgp),hre2);
      BDY(ntrfc) = (pointer)hgp2;
      NEXTNODE(ntrf,ntrfc);
      ROOTNO(hre) = 2;
      BDY(ntrfc) = (pointer)hgp;
    } else {
      NEXTNODE(ntrf,ntrfc);
      BDY(ntrfc) = (pointer)hgp;
    }
  }
  *pntrfield = ntrf;
  *pntrfieldc = ntrfc;
}

void sp_add2eset(trfield,itype,peset,pesetc)
NODE trfield,*peset,*pesetc;
int itype;
{
  NODE sc;
  GP hgp;

  for (sc=trfield; sc; sc=NEXT(sc)) {
    hgp = (GP)BDY(sc);
    ITYPE(POINT(hgp)) = itype;
  }
  add2eset(trfield,peset,pesetc);
}

void subgpf(f,v,gp,pnf)
F f,*pnf;
V v;
GP gp;
{
  NODE argl=NULL,arglc=NULL;

  NEXTNODE(argl,arglc);
  BDY(arglc) = (pointer)GUARD(gp);
  NEXTNODE(argl,arglc);
  subref(f,v,POINT(gp),&BDY(arglc));
  MKJF(*pnf,AL_AND,argl);
}

void subref(f,v,r,pnf)
F f,*pnf;
V v;
RE r;
{
  pointer argv[2];

  argv[0] = (pointer)v;
  argv[1] = (pointer)r;
  apply2ats(f,subref_a,argv,pnf);
}

void subref_a(at,argv,pnat)
F at,*pnat;
pointer argv[];
{
  switch (ITYPE((RE)argv[1])) {
  case STD:
    substd_a(at,argv[0],argv[1],pnat);
    return;
  case EPS:
    error("unspecified RE in subref_a()");
  case PEPS:
  case MEPS:
    subpme_a(at,argv[0],argv[1],pnat);
    return;
  case PINF:
  case MINF:
    subinf_a(at,argv[0],argv[1],pnat);
    return;
  default:
    error("unknown itype in subref_a()");
  }
}

void substd_a(at,v,re,pnf)
F at,*pnf;
V v;
RE re;
{
  VL no;
  P rlhs,prem,nlhs;
  Z dd,dndeg;
  
  reordvar(CO,v,&no);
  reorderp(no,CO,FPL(at),&rlhs);
  if (!rlhs || NUM(rlhs) || VR(rlhs) != v) {
    *pnf = at;
    return;
  }
  premp(no,rlhs,re->p,&prem);
  if (prem && !NUM(prem) && VR(prem) == v) {
    /* quadratic case */
    substd_a2(FOP(at),prem,DEG(DC(rlhs)),re,pnf);
    return;
  }
  subz(DEG(DC(rlhs)),DEG(DC(re->p)),&dd);
  addz(dd,ONE,&dndeg);
  if (AL_ORDER(FOP(at)) && (!evenz(dndeg)))
    mulp(CO,prem,COEF(DC(re->p)),&nlhs);
  else
    nlhs = prem;
  MKAF(*pnf,FOP(at),nlhs);
}
    
void substd_a2(op,prem,fdeg,re,pf)
oFOP op;
F prem;
Q fdeg;
RE re;
F *pf;
{
  P a,b,c,ld;

  getrecoeffs(prem,fdeg,re,&a,&b,&c,&ld);
  if (ROOTNO(re) == 1)
    chsgnp(b,&b);
  else if (ROOTNO(re) != 2)
    error("unspecified quadratic root in substd_a2");
  substd_a21(op,a,b,c,ld,pf);
}

void substd_a21(op,a,b,c,d,pf)
oFOP op;
P a,b,c,d;
F *pf;
{
  switch (op) {
  case AL_EQUAL:
    substd_a21_equal(a,b,c,d,pf);
    return;
  case AL_NEQ:
    substd_a21_equal(a,b,c,d,pf);
    pnegate(*pf,pf);
    return;
  case AL_LEQ:
    substd_a21_leq(a,b,c,d,pf);
    return;
  case AL_LESSP:
    substd_a21_lessp(a,b,c,d,pf);
    return;
  case AL_GEQ:
    substd_a21_lessp(a,b,c,d,pf);
    pnegate(*pf,pf);
    return;
  case AL_GREATERP:
    substd_a21_leq(a,b,c,d,pf);
    pnegate(*pf,pf);
    return;
  default:
    error("unknown operator in substd_a21");
  }
}

void substd_a21_equal(a,b,c,d,pf)
P a,b,c,d;
F *pf;
{
  F hf;
  NODE cj=NULL,cjc=NULL;
  P hp1,hp2;

  NEXTNODE(cj,cjc);
  mulp(CO,a,a,&hp1);
  mulp(CO,b,b,&hp2);
  mulp(CO,hp2,c,&hp2);
  subp(CO,hp1,hp2,&hp1);
  MKAF(hf,AL_EQUAL,hp1);
  BDY(cjc) = (pointer)hf;
  NEXTNODE(cj,cjc);
  mulp(CO,a,b,&hp1);
  MKAF(hf,AL_LEQ,hp1);
  BDY(cjc) = (pointer)hf;
  MKJF(*pf,AL_AND,cj);
}

void substd_a21_leq(a,b,c,d,pf)
P a,b,c,d;
F *pf;
{
  F hf;
  NODE cj=NULL,cjc=NULL,dj=NULL,djc=NULL;
  P hp1,hp2;

  NEXTNODE(dj,djc);
  NEXTNODE(cj,cjc);
  mulp(CO,a,d,&hp1);
  MKAF(hf,AL_LEQ,hp1);
  BDY(cjc) = (pointer)hf;
  NEXTNODE(cj,cjc);
  mulp(CO,a,a,&hp1);
  mulp(CO,b,b,&hp2);
  mulp(CO,hp2,c,&hp2);
  subp(CO,hp1,hp2,&hp1);
  MKAF(hf,AL_GEQ,hp1);
  BDY(cjc) = (pointer)hf;
  MKJF(hf,AL_AND,cj);
  BDY(djc) = (pointer)hf;
  NEXTNODE(dj,djc);
  cj = NULL;
  NEXTNODE(cj,cjc);
  MKAF(hf,AL_LEQ,hp1);
  BDY(cjc) = (pointer)hf;
  NEXTNODE(cj,cjc);
  mulp(CO,b,d,&hp1);
  MKAF(hf,AL_LEQ,hp1);
  BDY(cjc) = (pointer)hf;
  MKJF(hf,AL_AND,cj);
  BDY(djc) = (pointer)hf;
  MKJF(*pf,AL_OR,dj);
}

void substd_a21_lessp(a,b,c,d,pf)
P a,b,c,d;
F *pf;
{
  F hf,hf0;
  NODE cj=NULL,cjc=NULL,d1=NULL,d1c=NULL,d2=NULL,d2c=NULL;
  P hp1,hp2;

  NEXTNODE(d1,d1c);
  NEXTNODE(cj,cjc);
  mulp(CO,a,d,&hp1);
  MKAF(hf0,AL_LESSP,hp1);
  BDY(cjc) = (pointer)hf0;
  NEXTNODE(cj,cjc);
  mulp(CO,a,a,&hp1);
  mulp(CO,b,b,&hp2);
  mulp(CO,hp2,c,&hp2);
  subp(CO,hp1,hp2,&hp1);
  MKAF(hf,AL_GREATERP,hp1);
  BDY(cjc) = (pointer)hf;
  MKJF(hf,AL_AND,cj);
  BDY(d1c) = (pointer)hf;
  NEXTNODE(d1,d1c);
  cj = NULL;
  NEXTNODE(cj,cjc);
  NEXTNODE(d2,d2c);
  MKAF(hf,AL_LESSP,hp1);
  BDY(d2c) = (pointer)hf;
  NEXTNODE(d2,d2c);
  BDY(d2c) = (pointer)hf0;
  MKJF(hf,AL_OR,d2);
  BDY(cjc) = (pointer)hf;
  NEXTNODE(cj,cjc);
  mulp(CO,b,d,&hp1);
  MKAF(hf,AL_LEQ,hp1);
  BDY(cjc) = (pointer)hf;
  MKJF(hf,AL_AND,cj);
  BDY(d1c) = (pointer)hf;
  MKJF(*pf,AL_OR,d1);
}

void getrecoeffs(prem,fdeg,re,pa,pb,pc,pld)
P prem,*pa,*pb,*pc,*pld;
Z fdeg;
RE re;
{
  P a,b,c,alpha,beta,h1,h2;
  Z two;

  alpha = COEF(DC(prem));
  beta = (NEXT(DC(prem))) ? COEF(NEXT(DC(prem))) : 0;
  getqcoeffs(re->p,&a,&b,&c);
  STOZ(2,two);
  mulp(CO,(P)two,a,&h1);
  mulp(CO,h1,beta,&h2);
  mulp(CO,b,alpha,&h1);
  subp(CO,h2,h1,pa);
  *pb = alpha;
  *pc = DISC(re);
  *pld = (evenz(fdeg)) ? (P)ONE : a;
}
           
void subinf_a(f,v,re,pnf)
F f,*pnf;
V v;
RE re;
{
  if (AL_ORDER(FOP(f)))
    subinf_a_o(f,v,re,pnf);
  else
    subtrans_a_no(f,v,pnf);
}

void subinf_a_o(f,v,ire,pnf)
F f,*pnf;
V v;
RE ire;
{
  P rlhs;
  
  if (!al_reorder(FPL(f),v,&rlhs))
    *pnf = f;
  else
    subinf_a_o1(FOP(f),DC(rlhs),ire,pnf);
}

void subinf_a_o1(op,lhsdcp,ire,pnf)
oFOP op;
DCP lhsdcp;
RE ire;
F *pnf;
{
  P an;
  F h;
  NODE c=NULL,cc=NULL,d=NULL,dc=NULL;

  if (lhsdcp == 0) {
    MKAF(*pnf,op,0);
    return;
  }
  if (DEG(lhsdcp) == 0) {
    MKAF(*pnf,op,COEF(lhsdcp));
    return;
  }
  if (ITYPE(ire) == MINF && !evenz(DEG(lhsdcp)))
    chsgnp(COEF(lhsdcp),&an);
  else
    an = COEF(lhsdcp);
  NEXTNODE(d,dc);
  MKAF(h,AL_MKSTRICT(op),an);
  BDY(dc) = (pointer)h;
  NEXTNODE(d,dc);
  NEXTNODE(c,cc);
  MKAF(h,AL_EQUAL,an);
  BDY(cc) = (pointer)h;
  NEXTNODE(c,cc);
  subinf_a_o1(op,NEXT(lhsdcp),ire,&h);
  BDY(cc) = (pointer)h;
  MKJF(h,AL_AND,c);
  BDY(dc) = (pointer)h;
  MKJF(*pnf,AL_OR,d);
}

void subtrans_a_no(f,v,pnf)
F f,*pnf;
V v;
{
  P rlhs;
  DCP sc;
  F h;
  NODE nargl=NULL,narglc;
  oFOP op=FOP(f);
  
  if (!al_reorder(FPL(f),v,&rlhs)) {
    *pnf = f;
    return;
  }
  for (sc=DC(rlhs); sc; sc=NEXT(sc)) {
    NEXTNODE(nargl,narglc);
    MKAF(h,op,COEF(sc));
    BDY(narglc) = (pointer)h;
  }
  smkjf(pnf,AL_TRSUBEXP(op),nargl);
}

void subpme_a(af,v,re,pnf)
F af,*pnf;
V v;
RE re;
{
  if (AL_ORDER(FOP(af)))
    subpme_a_o(af,v,re,pnf);
  else
    subtrans_a_no(af,v,pnf);
}

void subpme_a_o(af,v,r,pnf)
F af,*pnf;
V v;
RE r;
{
  F h;
  RE stdre;
  
  subpme_a_o1(FOP(af),FPL(af),v,ITYPE(r)==MEPS,&h);
  MKRE(stdre,r->p,DISC(r),ROOTNO(r),STD);
  subref(h,v,stdre,pnf);
}

void subpme_a_o1(op,lhs,v,minus,pnf)
oFOP op;
P lhs;
V v;
int minus;
F *pnf;
{
  Z deg;
  F h;
  NODE c=NULL,cc=NULL,d=NULL,dc=NULL;
  P df;
  
  degp(v,lhs,&deg);
  if (deg == 0) {
    MKAF(*pnf,op,lhs);
    return;
  };
  NEXTNODE(d,dc);
  MKAF(h,AL_MKSTRICT(op),lhs);
  BDY(dc) = (pointer)h;
  NEXTNODE(d,dc);
  NEXTNODE(c,cc);
  MKAF(h,AL_EQUAL,lhs);
  BDY(cc) = (pointer)h;
  NEXTNODE(c,cc);
  diffp(CO,lhs,v,&df);
  if (minus)
    chsgnp(df,&df);
  subpme_a_o1(op,df,v,minus,&h);
  BDY(cc) = (pointer)h;
  MKJF(h,AL_AND,c);
  BDY(dc) = (pointer)h;
  MKJF(*pnf,AL_OR,d);
}

int comember(co,x)
CONT co;
CEL x;
{
  NODE sc;
  
  for (sc=FIRST(co); sc; sc=NEXT(sc))
    if (synequalf(MAT(x),MAT((CEL)BDY(sc))))
      return 1;
  return 0;
}

void coadd(co,x)
CONT co;
CEL x;
{
  NEXTNODE(FIRST(co),LAST(co));
  BDY(LAST(co)) = (pointer)x;
}

int coget(co,px)
CONT co;
CEL *px;
{
  if (FIRST(co) == 0)
    return 0;
  *px = (CEL)BDY(FIRST(co));
  FIRST(co) = NEXT(FIRST(co));
  return 1;
}

int colen(co)
CONT co;
{
  NODE sc;
  int n=0;
  
  for (sc=FIRST(co); sc; sc=NEXT(sc))
    n++;
  return n;
}

/* Misc */

void apply2ats(f,client,argv,pnf)
F f,*pnf;
void (*client)();
pointer argv[];
{
  if (AL_ATOMIC(FOP(f)))
    (*client)(f,argv,pnf);
  else if (AL_JUNCT(FOP(f))) {
    NODE sc,n=NULL,c;
    for (sc=FJARG(f); sc; sc=NEXT(sc)) {
      NEXTNODE(n,c);
      apply2ats(BDY(sc),client,argv,(F *)&BDY(c));
    }
    MKJF(*pnf,FOP(f),n);
  }
  else if (AL_TVAL(FOP(f)))
    *pnf = f;
  else if (AL_QUANT(FOP(f))) {
    F h;
    apply2ats(FQMAT(f),client,argv,&h);
    MKQF(*pnf,FOP(f),FQVR(f),h);
  } else
    error("unknown operator in apply2ats");
}

void atl(f,pn)
F f;
NODE *pn;
{
  NODE c;

  *pn = NULL;
  atl1(f,pn,&c);
}

void atl1(f,pn,pc)
F f;
NODE *pn,*pc;
{
  NODE sc;

  if (AL_ATOMIC(FOP(f))) {
    simpl_gand_insert_a(f,pn,pc);
    return;
  }  
  if (AL_JUNCT(FOP(f)))
    for (sc=FJARG(f); sc; sc=NEXT(sc))
      atl1(BDY(sc),pn,pc);
}

void atnum(f,pn)
F f;
Q *pn;
{
  *pn = 0;
  atnum1(f,pn);
}

void atnum1(f,pn)
F f;
Q *pn;
{
  NODE sc;
  
  if (AL_ATOMIC(FOP(f)))
    addq(*pn,(Q)ONE,pn);
  else if (AL_JUNCT(FOP(f)))
    for (sc=FJARG(f); sc; sc=NEXT(sc))
      atnum1(BDY(sc),pn);
}

void pnegate(f,pnf)
F f,*pnf;
{
  F h;
  NODE sc,n=NULL,c;
  oFOP op=FOP(f);
  
  if (AL_QUANT(op)) {
    pnegate(FQMAT(f),&h);
    MKQF(*pnf,AL_LNEGOP(op),FQVR(f),h);
    return;
  }
  if (AL_JUNCT(op)) {
    for (sc=FJARG(f); sc; sc=NEXT(sc)) {
      NEXTNODE(n,c);
      pnegate((F)BDY(sc),(F*)&BDY(c));
    }
    MKJF(*pnf,AL_LNEGOP(op),n);
    return;
  }
  if (AL_ATOMIC(op)) {
    MKAF(*pnf,AL_LNEGOP(op),FPL(f));
    return;
  }
  if (op == AL_TRUE) {
    *pnf = F_FALSE;
    return;
  }
  if (op == AL_FALSE) {
    *pnf = F_TRUE;
    return;
  }
  error("unknown operator in pnegate()");
}

void subf(o,f,v,p,pf)
VL o;
F f,*pf;
V v;
P p;
{
  pointer argv[3];

  argv[0] = (pointer)o;
  argv[1] = (pointer)v;
  argv[2] = (pointer)p;
  apply2ats(f,subf_a,argv,pf);
}

void subf_a(at,argv,pat)
F at,*pat;
pointer argv[];
{
  P nlhs;

  substp((VL)argv[0],FPL(at),(V)argv[1],(P)argv[2],&nlhs);
  MKAF(*pat,FOP(at),nlhs);
}

void nnf(f,pf)
F f,*pf;
{
  nnf1(f,0,0,pf);
}

void nnf1(f,neg,disj,pf)
F f,*pf;
int neg,disj;
{
  F h;
  NODE sc,nargl=NULL,narglc=NULL;
  oFOP op=FOP(f);

  if (AL_ATOMIC(op) || AL_TVAL(op)) {
    if (neg)
      pnegate(f,pf);
    else
      *pf = f;
    return;
  }
  if (AL_JUNCT(op)) {
    if (neg)
      op = AL_LNEGOP(op);
    for (sc=FJARG(f); sc; sc=NEXT(sc)) {
      NEXTNODE(nargl,narglc);
      nnf1((F)BDY(sc),neg,op==AL_OR,(F*)&BDY(narglc));
    }
    MKJF(*pf,op,nargl);
    return;
  }
  if (op == AL_IMPL) {
    op = neg ? AL_AND : AL_OR;
    NEXTNODE(nargl,narglc);
    nnf1(FLHS(f),!neg,op==AL_OR,(F*)&BDY(narglc));
    NEXTNODE(nargl,narglc);
    nnf1(FRHS(f),neg,op==AL_OR,(F*)&BDY(narglc));
    MKJF(*pf,op,nargl);
    return;
  }
  if (op == AL_REPL) {
    op = neg ? AL_AND : AL_OR;
    NEXTNODE(nargl,narglc);
    nnf1(FLHS(f),neg,op==AL_OR,(F*)&BDY(narglc));
    NEXTNODE(nargl,narglc);
    nnf1(FRHS(f),!neg,op==AL_OR,(F*)&BDY(narglc));
    MKJF(*pf,op,nargl);
    return;
  }
  if (op == AL_EQUIV) {
    /* should consider disj and its arguments ops */
    NEXTNODE(nargl,narglc);
    MKBF(h,AL_IMPL,FLHS(f),FRHS(f));
    BDY(narglc) = (pointer)h;
    NEXTNODE(nargl,narglc);
    MKBF(h,AL_REPL,FLHS(f),FRHS(f));
    BDY(narglc) = (pointer)h;
    MKJF(h,AL_AND,nargl);
    nnf1(h,neg,disj,pf);
    return;
  }
  if (AL_QUANT(op)) {
    nnf1(FQMAT(f),neg,0,&h);
    MKQF(*pf,neg ? AL_LNEGOP(op) : op,FQVR(f),h);
    return;
  }
  if (op == AL_NOT) {
    nnf1(FARG(f),!neg,disj,pf);
    return;
  }
  error("unknown operator in nnf1()");
}

void freevars(f,pvl)
F f;
VL *pvl;
{
  *pvl = NULL;
  freevars1(f,pvl,NULL);
}

void freevars1(f,pvl,cbvl)
F f;
VL *pvl,cbvl;
{
  VL hvl;
  NODE sc;
  oFOP op=FOP(f);

  if (AL_ATOMIC(op)) {
    freevars1_a(f,pvl,cbvl);
    return;
  }
  if (AL_JUNCT(op)) {
    for (sc=FJARG(f); sc; sc=NEXT(sc))
      freevars1((F)BDY(sc),pvl,cbvl);
    return;
  }
  if (AL_QUANT(op)) {
    MKVL(hvl,FQVR(f),cbvl);
    freevars1(FQMAT(f),pvl,hvl);
    return;
  }
  if (AL_UNI(op)) {
    freevars1(FARG(f),pvl,cbvl);
    return;
  }
  if (AL_EXT(op)) {
    freevars1(FLHS(f),pvl,cbvl);
    freevars1(FRHS(f),pvl,cbvl);
    return;
  }
  if (AL_TVAL(op))
    return;
  error("unknown operator in freevars1()");
}

void freevars1_a(f,pvl,cbvl)
F f;
VL *pvl,cbvl;
{
  VL sc,sc2,last;

  for (get_vars((Obj)FPL(f),&sc); sc; sc=NEXT(sc)) {
    for(sc2=cbvl; sc2; sc2=NEXT(sc2))
      if (VR(sc) == VR(sc2))
        break;
    if (sc2)
      continue;
    if (!*pvl) {
      MKVL(*pvl,VR(sc),NULL);
      continue;
    }
    for (sc2=*pvl; sc2; sc2=NEXT(sc2)) {
      if (VR(sc) == VR(sc2))
        break;
      last = sc2;
    }
    if (sc2)
      continue;
    MKVL(NEXT(last),VR(sc),NULL);
  }
}

int compf(vl,f1,f2)
VL vl;
F f1,f2;
{
  if (AL_ATOMIC(FOP(f1)) && AL_ATOMIC(FOP(f2)))
    return compaf(vl,f1,f2);
  if (AL_ATOMIC(FOP(f1)))
    return 1;
  if (AL_ATOMIC(FOP(f2)))
    return -1;
  if (synequalf(f1,f2))
    return 0;
  return 2;
}

/* Debug */

void ap(x)
pointer *x;
{
  printexpr(CO,(Obj)x);
  printf("\n");
}

void rep(re)
RE re;
{
  printf("(");
  printexpr(CO,(Obj)re->p);
  printf(",");
  printexpr(CO,(Obj)DISC(re));
  printf(",");
  printf("%d)\n",re->itype);
}

void gpp(gp)
GP gp;
{
  ap((pointer *)gp->g);
  rep(gp->p);
}

void esetp(eset)
NODE eset;
{
  NODE sc;

  for (sc=eset; sc; sc=NEXT(sc))
    gpp(BDY(sc));
}

void nodep(n)
NODE n;
{
  NODE sc;

  for (sc=n; sc; sc=NEXT(sc))
    ap(BDY(sc));
}

void lbfp(x)
LBF x;
{
  printf("(%d,",LBFLB(x));
  printexpr(CO,(Obj)LBFF(x));
  printf(")");
}

void thp(x)
NODE x;
{
  if (x == NULL) {
   printf("[]\n");
   return;
  }
  printf("[");
  lbfp((LBF)BDY(x));
  x = NEXT(x);
  for (; x != NULL; x = NEXT(x)) {
   printf(",");
   lbfp((LBF)BDY(x));
  }
  printf("]\n");
}
