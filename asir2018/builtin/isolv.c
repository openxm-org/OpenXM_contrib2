/*
 * $OpenXM: OpenXM_contrib2/asir2018/builtin/isolv.c,v 1.3 2019/10/17 03:03:12 kondoh Exp $
 */

#include "ca.h"
#include "parse.h"
#include "version.h"

#if defined(INTERVAL) 

/* in Q.c */
void dupz(Z, Z*);

static void Solve(NODE, Obj *);
static void NSolve(NODE, Obj *);

/* in builtin/vars.c */
void Pvars();

/* */
void Solve1(P, Q, pointer *);
void Sturm(P, VECT *);
void boundbody(P, Q *);
void binary(int , MAT);
void separate(Q, Q, VECT, Q, Q, int, int, MAT, int *);
void ueval(P, Q, Q *);
int stumq(VECT, Q);


// in engine/bf.c
Num tobf(Num,int);

struct ftab isolv_tab[] = {
  {"solve", Solve, 2},
  {"nsolve", NSolve, 2},
  {0,0,0},
};

static void
Solve(arg, rp)
NODE arg;
Obj  *rp;
{
  //pointer p, Eps;
  pointer	root;
  V			v;
  Q			eps;
  Q			Eps;
  P			p;


  p = (P)ARG0(arg);
  if ( !p ) {
    *rp = 0;
    return;
  }
  Eps = (Q)ARG1(arg);
  asir_assert(Eps, O_N, "solve");
  if ( NID(Eps) != N_Q ) {
    fprintf(stderr,"solve arg 2 is required a rational number");
    error(" : invalid argument");
    return;
  }
  //DUPQ((Q)Eps, eps);
  //SGN(eps) = 1;
  NEWQ(eps); mpq_abs(BDY(eps),BDY(Eps));
 
  switch (OID(p)) {
    case O_N:
      *rp = 0;
      break;
    case O_P:
      Pvars(arg, &root);
      if (NEXT(BDY((LIST)root)) != 0) {
        fprintf(stderr,"solve arg 1 is univariate polynormial");
        error(" : invalid argument");
        break;
      }
      Solve1(p, eps, &root);
      *rp = (Obj)root;
      break;
    case O_LIST:
      fprintf(stderr,"solve,");
      error(" : Sorry, not yet implement of multivars");
      break;
    default:
      *rp = 0;
  }
}

static void
NSolve(NODE arg, Obj *rp)
{
  P			p;
  Q			Eps;
  Q			eps;
  pointer  root;
  LIST    listp;
  V      v;
  NODE    n, n0, m0, m, ln0;
  Num    r;
  Itv    iv;
  BF      breal;

  p = (P)ARG0(arg);
  if ( !p ) {
    *rp = 0;
    return;
  }
  Eps = (Q)ARG1(arg);
  asir_assert(Eps, O_N, "solve");
  if ( NID(Eps) != N_Q ) {
    fprintf(stderr,"solve arg 2 is required a rational number");
    error(" : invalid argument");
    return;
  }
  //DUPQ((Q)Eps, eps);
  //SGN(eps) = 1;
  NEWQ(eps); mpq_abs(BDY(eps),BDY(Eps));

  switch (OID(p)) {
    case O_N:
      *rp = 0;
      break;
    case O_P:
      Pvars(arg, &root);
      if (NEXT(BDY((LIST)root)) != 0) {
        fprintf(stderr,"solve arg 1 is univariate polynormial");
        error(" : invalid argument");
        break;
      }
      Solve1(p, eps, &root);
      for (m0 = BDY((LIST)root), n0 = 0; m0; m0 = NEXT(m0)) {
        m = BDY((LIST)BDY(m0));
        miditvp(BDY(m), &r);
        //ToBf(r, &breal);
        breal = (BF)tobf(r, DEFAULTPREC);
        NEXTNODE( n0, n );
        MKNODE(ln0, breal, NEXT(m));
        MKLIST(listp, ln0);
        BDY(n) = (pointer)listp;
      }
      NEXT(n) = 0;
      MKLIST(listp,n0);
      *rp = (pointer)listp;
      break;
    case O_LIST:
      fprintf(stderr,"solve,");
      error(" : Sorry, not yet implement of multivars");
      break;
    default:
      *rp = 0;
  }
}

void
Solve1(inp, eps, rt)
P    inp;
Q    eps;
pointer *rt;
{
  P    p;
  Q    up, low, a;
  DCP fctp, onedeg, zerodeg;
  LIST listp;
  VECT sseq;
  MAT  root;
  int  chvu, chvl, pad, tnumb, numb, i, j;
  Itv  iv;
  NODE n0, n, ln0, ln;

  boundbody(inp, &up);
  if (!up) {
    *rt = 0;
    return;
  }
  Sturm(inp, &sseq);
  //DUPQ(up,low);
  //SGN(low) = -1;
  NEWQ(low); mpq_neg(BDY(low),BDY(up));
  chvu = stumq(sseq, up);
  chvl = stumq(sseq, low);
  tnumb = abs(chvu - chvl);
  MKMAT(root, tnumb, 4);
  pad = -1;

  fctrp(CO,inp,&fctp);
  for (fctp = NEXT(fctp), i = 0; fctp; fctp = NEXT(fctp)) {
    p = COEF(fctp);
    onedeg = DC(p);
    if ( !cmpz(DEG(onedeg), ONE) ) {
      pad++;
      if ( !NEXT(onedeg) ) {
        BDY(root)[pad][0] = 0;
        BDY(root)[pad][1] = 0;
        BDY(root)[pad][2] = DEG(fctp);
        BDY(root)[pad][3] = p;
      } else {
        divq((Q)COEF(NEXT(onedeg)),(Q)COEF(onedeg),&a);
        BDY(root)[pad][0] = a;
        BDY(root)[pad][1] = BDY(root)[pad][0];
        BDY(root)[pad][2] = DEG(fctp);
        BDY(root)[pad][3] = p;
      }
      continue;
    }
    boundbody(p, &up);
    Sturm(p, &sseq);
    //DUPQ(up,low);
    //SGN(low) = -1;
    NEWQ(low); mpq_neg(BDY(low),BDY(up));
    chvu = stumq(sseq, up);
    chvl = stumq(sseq, low);
    numb = abs(chvu - chvl);
    separate((Q)DEG(fctp), eps, sseq, up, low, chvu, chvl, root, &pad);
  }
  for (i = 0; i < pad; i++) {
    for (j = i; j <= pad; j++) {
      if (cmpq(BDY(root)[i][0], BDY(root)[j][0]) > 0) {
        a = BDY(root)[i][0];
        BDY(root)[i][0] = BDY(root)[j][0];
        BDY(root)[j][0] = a;
        a = BDY(root)[i][1];
        BDY(root)[i][1] = BDY(root)[j][1];
        BDY(root)[j][1] = a;
        a = BDY(root)[i][2];
        BDY(root)[i][2] = BDY(root)[j][2];
        BDY(root)[j][2] = a;
        a = BDY(root)[i][3];
        BDY(root)[i][3] = BDY(root)[j][3];
        BDY(root)[j][3] = a;
      }
    }
  }
  for (i = 0; i < pad; i++) {
    while(cmpq(BDY(root)[i][1], BDY(root)[i+1][0]) > 0 ) {
      binary(i, root);
      binary(i+1, root);
      if ( cmpq(BDY(root)[i][0], BDY(root)[i+1][1]) > 0 ) {
        a = BDY(root)[i][0];
        BDY(root)[i][0] = BDY(root)[i+1][0];
        BDY(root)[i+1][0] = a;
        a = BDY(root)[i][1];
        BDY(root)[i][1] = BDY(root)[i+1][1];
        BDY(root)[i+1][1] = a;
        a = BDY(root)[i][2];
        BDY(root)[i][2] = BDY(root)[i+1][2];
        BDY(root)[i+1][2] = a;
        a = BDY(root)[i][3];
        BDY(root)[i][3] = BDY(root)[i+1][3];
        BDY(root)[i+1][3] = a;
        break;
      }
    }
  }
  for (i = 0, n0 = 0; i <= pad; i++) {
    istoitv(BDY(root)[i][0], BDY(root)[i][1], &iv);
    NEXTNODE(n0,n);
    MKNODE(ln, BDY(root)[i][2], 0); MKNODE(ln0, iv, ln);
    MKLIST(listp, ln0);BDY(n) = (pointer)listp;
  }
  NEXT(n) = 0;
  MKLIST(listp,n0);
  *rt = (pointer)listp;
}

void
separate(mult, eps, sseq, up, low, upn, lown, root, padp)
VECT sseq;
Q    mult, eps, up, low;
int  upn, lown;
MAT  root;
int  *padp;
{
  int de, midn;
  Q   mid, e;
  P   p;
  Q tmp_e;

  de = abs(lown - upn);
  if (de == 0) return;
  if (de == 1) {
    (*padp)++;
    BDY(root)[*padp][0] = up;
    BDY(root)[*padp][1] = low;
    BDY(root)[*padp][3] = (P *)sseq->body[0];
    subq( BDY(root)[*padp][1], BDY(root)[*padp][0], &tmp_e );
    //SGN(e) = 1;
    absq(tmp_e, &e);
    while (cmpq(e, eps) > 0) {
      binary(*padp, root);
      subq( BDY(root)[*padp][1], BDY(root)[*padp][0], &tmp_e);
      //SGN(e) = 1;
      absq(tmp_e, &e);
    }
    BDY(root)[*padp][2] = mult;
    return;
  }
  addq(up, low, &mid);
  divq(mid, (Q)TWO, &mid);
  midn = stumq(sseq, mid);
  separate(mult, eps, sseq, low, mid, lown, midn, root, padp);
  separate(mult, eps, sseq, mid, up, midn, upn, root, padp);
}

void
binary(indx, root)
int indx;
MAT root;
{
  Q  a, b, c, d, e;
  P  p;
  p = (P)BDY(root)[indx][3];
  addq(BDY(root)[indx][0], BDY(root)[indx][1], &c);
  divq(c, (Q)TWO, &d);
  ueval(p, BDY(root)[indx][1], &a);
  ueval(p, d, &b);
  if (sgnq(a) == sgnq(b)){
    BDY(root)[indx][1] = d;
  } else {
    BDY(root)[indx][0] = d;
  }
}

void
Sturm(p, ret)
P    p;
VECT *ret;
{
  P    g1,g2,q,r,s, *t;
  Q    a,b,c,d,h,l,m,x;
  V    v;
  VECT seq;
  int  i,j;

  v = VR(p);
  t = (P *)ALLOCA((deg(v,p)+1)*sizeof(P));
  g1 = t[0] = p; diffp(CO,p,v,(P *)&a); ptozp((P)a,1,&c,&g2); t[1] = g2;
  NEWQ(h);
  NEWQ(x);
  dupz(ONE, (Z *)&h);
  dupz(ONE, (Z *)&x);
  for ( i = 1; ; ) {
    if ( NUM(g2) ) break;
    subz(DEG(DC(g1)),DEG(DC(g2)),(Z*)&d);
    l = (Q)LC(g2);
    if ( sgnq(l) < 0 ) {
      chsgnq(l,&a); l = a;
    }
    addq(d,(Q)ONE,&a); pwrq(l,a,&b); mulp(CO,(P)b,g1,(P *)&a);
    divsrp(CO,(P)a,g2,&q,&r);
    if ( !r ) break;
    chsgnp(r,&s); r = s; i++;
    if ( NUM(r) ) {
      t[i] = r; break;
    }
    pwrq(h,d,&m); g1 = g2;
    mulq(m,x,&a); divsp(CO,r,(P)a,&g2); t[i] = g2;
    x = (Q)LC(g1);
    if ( sgnq(x) < 0 ) {
      chsgnq(x,&a); x = a;
    }
    pwrq(x,d,&a); mulq(a,h,&b); divq(b,m,&h);
  }
  MKVECT(seq,i+1);
  for ( j = 0; j <= i; j++ ) seq->body[j] = (pointer)t[j];
  *ret = seq;
}

int
stumq(VECT s, Q val)
{
  int len, i, j, c;
  P   *ss;
  Q   a, a0;

  len = s->len;
  ss = (P *)s->body;
  for ( j = 0; j < len; j++ ){
    ueval(ss[j],val,&a0);
    if (a0) break;
  }
  for ( i = j++, c =0; i < len; i++) {
    ueval( ss[i], val, &a);
    if ( a ) {
      if( (sgnq(a) > 0 && sgnq(a0) < 0) || (sgnq(a) < 0 && sgnq(a0) > 0) ){
        c++;
        a0 = a;
      }
    }
  }
  return c;
}

void
boundbody(p, q)
P  p;
Q *q;
{
  Q    t, max, tmp;
  DCP  dc;

  if ( !p )
    *q = 0;
  else if ( p->id == O_N )
    *q = 0;
  else {
    //NEWQ(tmp);
    //SGN(tmp)=1;
    NEWQ(max);
    for ( dc = DC(p); dc; dc = NEXT(dc) ) {
      t = (Q)COEF(dc);
      if ( cmpq(t, max) > 0 ) { //DUPQ(tmp, max);
        mpq_abs(BDY(max),BDY(t));
      }
    }
    addq((Q)ONE, max, q);
    //mpq_clear(max);
  }
}

void
ueval(p, q, rp)
P p;
Q q;
Q *rp;
{
  Z   d, d1;
  Z   deg;
  Q   da;
  Q   a, b, t;
  Z   nm, dn;
  DCP dc;

  if ( !p ) *rp = 0;
  else if ( NUM(p) ) *rp = (Q)p;
  else {
    if ( q ) {
      //NTOQ( DN(q), 1, dn );
      dnq(q, &dn);
      //NTOQ( NM(q), sgnq(q), nm );
      nmq(q, &nm);
    } else {
      dn = 0;
      nm = 0;
    }
    if ( !dn ) {
      dc = DC(p); t = (Q)COEF(dc);
      for ( d = DEG(dc), dc = NEXT(dc); dc; d = DEG(dc), dc= NEXT(dc) ) {
        subz(d, DEG(dc), &d1); pwrq((Q)nm, (Q)d1, &a);
        mulq(t,a,&b); addq(b,(Q)COEF(dc),&t);
      }
      if ( d ) {
        pwrq((Q)nm,(Q)d,&a); mulq(t,a,&b); t = b;
      }
      *rp = t;
    } else {
      dc = DC(p); t = (Q)COEF(dc);
      for ( d=deg= DEG(dc), dc = NEXT(dc); dc; d = DEG(dc), dc= NEXT(dc) ) {
        subz(d, DEG(dc), &d1); pwrq((Q)nm, (Q)d1, &a);
        mulq(t,a,&b);
        subz(deg, DEG(dc), &d1); pwrq((Q)dn, (Q)d1, &a);
        mulq(a, (Q)COEF(dc), &da);
        addq(b,da,&t);
      }
      if ( d ) {
        pwrz(nm,d,(Z*)&a); mulq(t,a,&b); t = b;
      }
      *rp = t;
    }
  }
}
#endif
