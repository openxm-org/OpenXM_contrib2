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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/gf.c,v 1.15 2002/03/15 02:52:09 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"

struct resf_dlist {
  int ib,id;
};

int resf_degtest(int,int *,int,struct resf_dlist *);
void uhensel(P,NODE,int,int,NODE *);
void uhensel_incremental(P,NODE,int,int,int,NODE *);
void resf_hensel(int,P,int,P *,ML *);
void resf_dtest(P,ML,int,int *,int *,DCP *);
void resf_dtest_special(P,ML,int,int *,int *,DCP *);
void dtest_special(P,ML,P *);
void hensel_special(int,int,P,P *,ML *);

void nullspace(UM **,UM,int,int,int *);
void nullspace_lm(LM **,int,int *);
void nullspace_gf2n(GF2N **,int,int *);
void nullspace_gfpn(GFPN **,int,int *);
void nullspace_gfs(GFS **,int,int *);
void nullspace_gfsn(GFSN **,int,int *);
void null_to_sol(int **,int *,int,int,UM *);

void showgfmat(UM **,int);
void pwr_mod(P,P,V,P,int,N,P *);
void rem_mod(P,P,V,P,int,P *);

void Pnullspace(),Pgcda_mod(),Pftest(),Presfmain(),Ppwr_mod(),Puhensel();
void Puhensel_incremental();
void Psfuhensel();

void Pnullspace_ff();

void Psolve_linear_equation_gf2n();
void Plinear_form_to_vect(),Pvect_to_linear_form();

void solve_linear_equation_gf2n(GF2N **,int,int,int *);
void linear_form_to_array(P,VL,int,Num *);
void array_to_linear_form(Num *,VL,int,P *);
void sfuhensel(P,NODE,GFS,int,NODE *);

extern int current_ff;

struct ftab gf_tab[] = {
  {"solve_linear_equation_gf2n",Psolve_linear_equation_gf2n,1},
  {"nullspace",Pnullspace,3},
  {"nullspace_ff",Pnullspace_ff,1},
/*  {"gcda_mod",Pgcda_mod,5}, */
  {"ftest",Pftest,2},
  {"resfmain",Presfmain,4},
  {"pwr_mod",Ppwr_mod,6},
  {"uhensel",Puhensel,4},
  {"uhensel_incremental",Puhensel_incremental,5},
  {"sfuhensel",Psfuhensel,4},
  {0,0,0},
};

int resf_degtest(k,in,nb,dlist)
int k;
int *in;
int nb;
struct resf_dlist *dlist;
{
  int i,d0,d;
  int dl_i;
  struct resf_dlist *t;

  if ( k < nb )
    return 0;
  if ( nb == 1 )
    return 1;
  d0 = 0; d = 0; dl_i = 0;
  for ( i = 0; i < k; i++ ) {
    t = &dlist[in[i]];
    if ( t->ib > dl_i + 1 )
      return 0;
    else if ( t->ib == dl_i )
      d += t->id;
    else if ( !d || (dl_i && d0 != d) )
      return 0;
    else {
      d0 = d; dl_i++; d = t->id;
    }
  }
  if ( dl_i != nb - 1 || d0 != d )
    return 0;
  else
    return 1;
}

void Puhensel(arg,rp)
NODE arg;
LIST *rp;
{
  P f;
  NODE mfl,r;
  int mod,bound;

  f = (P)ARG0(arg);
  mfl = BDY((LIST)ARG1(arg));
  mod = QTOS((Q)ARG2(arg));
  bound = QTOS((Q)ARG3(arg));
  uhensel(f,mfl,mod,bound,&r);
  MKLIST(*rp,r);
}

void Puhensel_incremental(arg,rp)
NODE arg;
LIST *rp;
{
  P f;
  NODE mfl,r;
  int mod,bound,start;

  f = (P)ARG0(arg);
  mfl = BDY((LIST)ARG1(arg));
  mod = QTOS((Q)ARG2(arg));
  start = QTOS((Q)ARG3(arg));
  bound = QTOS((Q)ARG4(arg));
  uhensel_incremental(f,mfl,mod,start,bound,&r);
  MKLIST(*rp,r);
}

void uhensel(f,mfl,mod,bound,rp)
P f;
NODE mfl;
int mod,bound;
NODE *rp;
{
  ML blist,clist,rlist;
  LUM fl;
  int nf,i;
  P s;
  V v;
  NODE t,top;

  nf = length(mfl);
  blist = MLALLOC(nf); blist->n = nf; blist->mod = mod;
  for ( i = 0, t = mfl; i < nf; i++, t = NEXT(t) ) {
    blist->c[i] = (pointer)UMALLOC(UDEG((P)BDY(t)));
    ptoum(mod,(P)BDY(t),blist->c[i]);
  }
  gcdgen(f,blist,&clist);  
  blist->bound = clist->bound = bound;
  W_LUMALLOC((int)UDEG(f),bound,fl);
  ptolum(mod,bound,f,fl);
  henmain(fl,blist,clist,&rlist);
  v = VR(f);
  for ( i = nf-1, top = 0; i >= 0; i-- ) {
    lumtop(v,mod,bound,rlist->c[i],&s);
    MKNODE(t,s,top); top = t;
  }
  *rp = top;
}

void uhensel_incremental(f,mfl,mod,start,bound,rp)
P f;
NODE mfl;
int mod,start,bound;
NODE *rp;
{
  ML blist,clist,rlist;
  LUM fl;
  LUM *lblist;
  int nf,i,j,k;
  int **p;
  P s;
  V v;
  NODE t,top;

  nf = length(mfl);
  blist = MLALLOC(nf); blist->n = nf; blist->mod = mod;
  lblist = (LUM *)MALLOC(nf*sizeof(LUM));
  for ( i = 0, t = mfl; i < nf; i++, t = NEXT(t) ) {
    blist->c[i] = (pointer)UMALLOC(UDEG((P)BDY(t)));
    ptoum(mod,(P)BDY(t),blist->c[i]);
    W_LUMALLOC((int)UDEG((P)BDY(t)),bound,lblist[i]);
    ptolum(mod,start,(P)BDY(t),lblist[i]);
    p = lblist[i]->c;
    for ( j = DEG(lblist[i]); j >= 0; j-- )
      for ( k = start; k < bound; k++ )
        p[j][k] = 0;
  }
  gcdgen(f,blist,&clist);  
  clist->bound = bound;
  W_LUMALLOC((int)UDEG(f),bound,fl);
  ptolum(mod,bound,f,fl);
  henmain_incremental(fl,lblist,clist,nf,mod,start,bound);
  v = VR(f);
  for ( i = nf-1, top = 0; i >= 0; i-- ) {
    lumtop_unsigned(v,mod,bound,lblist[i],&s);
    MKNODE(t,s,top); top = t;
  }
  *rp = top;
}

void Psfuhensel(arg,rp)
NODE arg;
LIST *rp;
{
  P f;
  int bound;
  NODE r,mfl;
  GFS ev;

  f = (P)ARG0(arg);
  mfl = BDY((LIST)ARG1(arg));
  ev = (GFS)ARG2(arg);
  bound = QTOS((Q)ARG3(arg));
  sfuhensel(f,mfl,ev,bound,&r);
  MKLIST(*rp,r);
}

void sfuhensel(f,mfl,ev,bound,rp)
P f;
NODE mfl;
GFS ev;
int bound;
NODE *rp;
{
  BM fl;
  BM *r;
  VL vl,nvl;
  int i,fn,dx,dy,d;
  NODE t,top;
  UM fm,hm,q;
  UM *gm;
  V x,y;
  P g,s,u;

  clctv(CO,f,&vl);
  if ( !vl || !vl->next || vl->next->next )
    error("sfuhensel : f must be a bivariate poly");

  for ( i = 0, t = mfl; t; i++, t = NEXT(t) );
  fn = i;

  gm = (UM *)MALLOC(fn*sizeof(UM));

  /* XXX : more severe check is necessary */
  x = VR((P)BDY(mfl));
  y = vl->v == x ? vl->next->v : vl->v;

  for ( i = 0, t = mfl, d = 0; i < fn; i++, t = NEXT(t) ) {
    gm[i] = (pointer)UMALLOC(getdeg(x,(P)BDY(t)));
    ptosfum((P)BDY(t),gm[i]);
    d += DEG(gm[i]);
  }

  /* reorder f if necessary */
  if ( vl->v != x ) {
    reordvar(vl,x,&nvl); reorderp(nvl,vl,f,&g);
    vl = nvl; f = g;
  }
  dx = getdeg(x,f);
  if ( dx != d )
    error("sfuhensel : product of factors has incompatible degree");

  dy = getdeg(y,f);
  dy = MAX(dy,bound);
  fl = BMALLOC(dx,dy);
  ptosfbm(dy,f,fl);
  if ( ev ) shiftsfbm(fl,FTOIF(CONT(ev)));

  /* fm = fl mod y */
  fm = W_UMALLOC(dx);
  cpyum(COEF(fl)[0],fm);
  hm = W_UMALLOC(dx);

  q = W_UMALLOC(dx);
  r = (BM *)MLALLOC(fn*sizeof(BM));
  for ( i = 0; i < fn-1; i++ ) {
    /* fl = gm[i]*hm mod y */
    divsfum(fm,gm[i],hm);
    /* fl is replaced by the cofactor of gk mod y^bound */
    /* r[i] = gk */
    sfhenmain2(fl,gm[i],hm,bound,r+i);
    cpyum(hm,fm);
  }
  /* finally, fl must be the lift of gm[fn-1] */
  r[i] = fl;

  for ( i = fn-1, top = 0; i >= 0; i-- ) {
    sfbmtop(r[i],x,y,&s);
    reorderp(CO,vl,s,&u);
    MKNODE(t,u,top); top = t;
  }
  *rp = top;
}

void Presfmain(arg,rp)
NODE arg;
LIST *rp;
{
  P f;
  int mod,n,nb,i,j,k;
  int *nf,*md;
  P *mf;
  NODE mfl,mdl,t,s,u;
  ML list;
  DCP dc;
  int sflag;

  f = (P)ARG0(arg);
  mod = QTOS((Q)ARG1(arg));
  mfl = BDY((LIST)ARG2(arg));
  mdl = BDY((LIST)ARG3(arg));
  for ( n = nb = 0, t = mfl; t; nb++, t = NEXT(t) )
    for ( s = BDY((LIST)BDY(t)); s; n++, s = NEXT(s) );
  if ( n == nb ) {
    /* f must be irreducible! */
    NEWDC(dc);  
    dc->c = f; dc->d = ONE;
  } else {
    nf = W_ALLOC(nb); md = W_ALLOC(nb); mf = (P *)ALLOCA(n*sizeof(P));
    for ( k = i = 0, t = mfl, u = mdl, sflag = 1; t;
      i++, t = NEXT(t), u = NEXT(u) ) {
      if ( sflag && length(BDY((LIST)BDY(t))) != 2 )
        sflag = 0;
      for ( j = 0, s = BDY((LIST)BDY(t)); s; j++, s = NEXT(s) )
        mf[k++] = (P)BDY(s);
      nf[i] = j; md[i] = QTOS((Q)BDY(u));
    }
    resf_hensel(mod,f,n,mf,&list);
    if ( sflag )
      resf_dtest_special(f,list,nb,nf,md,&dc);
    else
      resf_dtest(f,list,nb,nf,md,&dc);
  }
  dcptolist(dc,rp);
}

void resf_hensel(mod,f,nf,mfl,listp)
int mod;
P f;
int nf;
P *mfl;
ML *listp;
{
  register int i,j;
  int q,n,bound,inv,lc;
  int *p;
  int **pp;
  ML blist,clist,bqlist,cqlist,rlist;
  UM *b;
  LUM fl,tl;
  LUM *l;
  UM w;

  w = W_UMALLOC(UDEG(f));
  blist = MLALLOC(nf); blist->n = nf; blist->mod = mod;

  /* c[0] must have lc(f) */
  blist->c[0] = (pointer)UMALLOC(UDEG(mfl[0]));
  ptoum(mod,mfl[0],w);
  inv = invm(w->c[UDEG(mfl[0])],mod);
  lc = rem(NM((Q)LC(f)),mod);
  if ( SGN((Q)LC(f)) < 0 )
    lc = (mod-lc)%mod;
  lc = dmar(inv,lc,0,mod);
  if ( lc == 1 )
    copyum(w,blist->c[0]);
  else
    mulsum(mod,w,lc,blist->c[0]);

  /* c[i] (i=1,...,nf-1) must be monic */
  for ( i = 1; i < nf; i++ ) {
    blist->c[i] = (pointer)UMALLOC(UDEG(mfl[i]));
    ptoum(mod,mfl[i],w);
    inv = invm(w->c[UDEG(mfl[i])],mod);
    if ( inv == 1 )
      copyum(w,blist->c[i]);
    else
      mulsum(mod,w,inv,blist->c[i]);
  }

  gcdgen(f,blist,&clist); henprep(f,blist,clist,&bqlist,&cqlist);
  n = bqlist->n; q = bqlist->mod;
  bqlist->bound = cqlist->bound = bound = mignotte(q,f);
  if ( bound == 1 ) {
    *listp = rlist = MLALLOC(n); 
    rlist->n = n; rlist->mod = q; rlist->bound = bound;
    for ( i = 0, b = (UM *)bqlist->c, l = (LUM *)rlist->c; i < n; i++ ) {
      tl = LUMALLOC(DEG(b[i]),1); l[i] = tl; p = COEF(b[i]);
      for ( j = 0, pp = COEF(tl); j <= DEG(tl); j++ )
          pp[j][0] = p[j];
    }
  } else {
    W_LUMALLOC((int)UDEG(f),bound,fl);
    ptolum(q,bound,f,fl); henmain(fl,bqlist,cqlist,listp);
  }
}

void resf_dtest(f,list,nb,nfl,mdl,dcp)
P f;
ML list;
int nb;
int *nfl,*mdl;
DCP *dcp;
{
  int n,np,bound,q;
  int i,j,k;
  int *win;
  P g,factor,cofactor;
  Q csum,csumt;
  DCP dcf,dcf0;
  LUM *c;
  ML wlist;
  struct resf_dlist *dlist;
  int z;

  n = UDEG(f); np = list->n; bound = list->bound; q = list->mod;
  win = W_ALLOC(np+1);
  ucsump(f,&csum); mulq(csum,(Q)COEF(DC(f)),&csumt); csum = csumt;
  wlist = W_MLALLOC(np); wlist->n = list->n;
  wlist->mod = list->mod; wlist->bound = list->bound;
  c = (LUM *)COEF(wlist);
  bcopy((char *)COEF(list),(char *)c,(int)(sizeof(LUM)*np));
  dlist = (struct resf_dlist *)ALLOCA(np*sizeof(struct resf_dlist));
  for ( i = 0, j = 0; j < nb; j++ )
    for ( k = 0; k < nfl[j]; k++, i++ ) {
      dlist[i].ib = j; dlist[i].id = DEG(c[i])/mdl[j];
    }
  k = nb;
  for ( i = 0; i < nb; i++ )
    win[i] = i+1;
  for ( g = f, dcf = dcf0 = 0, --np, z = 0; ; ) {
#if 0
    if ( !(z++ % 10000) )
      fputc('.',stderr);
#endif
    if ( resf_degtest(k,win,nb,dlist) &&
      dtestmain(g,csum,wlist,k,win,&factor,&cofactor) ) {
      NEXTDC(dcf0,dcf); DEG(dcf) = ONE; COEF(dcf) = factor;
      g = cofactor;
      ucsump(g,&csum); mulq(csum,(Q)COEF(DC(g)),&csumt); csum = csumt;
      for ( i = 0; i < k - 1; i++ ) 
        for ( j = win[i] + 1; j < win[i + 1]; j++ ) {
          c[j-i-1] = c[j];
          dlist[j-i-1] = dlist[j];
        }
      for ( j = win[k-1] + 1; j <= np; j++ ) {
        c[j-k] = c[j];
        dlist[j-k] = dlist[j];
      }
      if ( ( np -= k ) < k ) 
        break;
      if ( np - win[0] + 1 < k ) 
        if ( ++k > np )
          break;
        else
          for ( i = 0; i < k; i++ ) 
            win[i] = i + 1;
      else 
        for ( i = 1; i < k; i++ ) 
          win[i] = win[0] + i;
    } else if ( !ncombi(1,np,k,win) ) 
      if ( k == np ) 
        break;
      else
        for ( i = 0, ++k; i < k; i++ ) 
          win[i] = i + 1;
  }
  NEXTDC(dcf0,dcf); COEF(dcf) = g;
  DEG(dcf) = ONE; NEXT(dcf) = 0;*dcp = dcf0;
}

void resf_dtest_special(f,list,nb,nfl,mdl,dcp)
P f;
ML list;
int nb;
int *nfl,*mdl;
DCP *dcp;
{
  int n,np,bound,q;
  int i,j;
  int *win;
  P g,factor,cofactor;
  Q csum,csumt;
  DCP dcf,dcf0;
  LUM *c;
  ML wlist;
  int max;

  n = UDEG(f); np = list->n; bound = list->bound; q = list->mod;
  win = W_ALLOC(np+1);
  ucsump(f,&csum); mulq(csum,(Q)COEF(DC(f)),&csumt); csum = csumt;
  wlist = W_MLALLOC(np); wlist->n = list->n;
  wlist->mod = list->mod; wlist->bound = list->bound;
  c = (LUM *)COEF(wlist); bcopy((char *)COEF(list),(char *)c,(int)(sizeof(LUM)*np));
  max = 1<<nb;
  for ( g = f, dcf = dcf0 = 0, i = 0; i < max; i++ ) {
#if 0
    if ( !(i % 1000) )
      fprintf(stderr,"i=%d\n",i);
#endif
    for ( j = 0; j < nb; j++ )
      win[j] = (i&(1<<j)) ? 2*j+1 : 2*j;
    if ( dtestmain(g,csum,wlist,nb,win,&factor,&cofactor) ) {
#if 0
      fprintf(stderr,"success : i=%d\n",i);
#endif
      NEXTDC(dcf0,dcf); DEG(dcf) = ONE; COEF(dcf) = factor;
      NEXTDC(dcf0,dcf); DEG(dcf) = ONE; COEF(dcf) = cofactor;
      NEXT(dcf) = 0;*dcp = dcf0;
      return;
    }
  }
  NEXTDC(dcf0,dcf); COEF(dcf) = g;
  DEG(dcf) = ONE; NEXT(dcf) = 0;*dcp = dcf0;
}

#if 0
void Pftest(arg,rp)
NODE arg;
P *rp;
{
  ML list;
  DCP dc;
  P p;
  P *mfl;

  p = (P)ARG0(arg); mfl = (P *)(((VECT)ARG1(arg))->body);
  hensel_special(4,1,p,mfl,&list);
  dtest_special(p,list,rp);
}

void dtest_special(f,list,pr)
P f;
ML list;
P *pr;
{
  int n,np,bound,q;
  int i,j,k;
  int *win;
  P g,factor,cofactor;
  Q csum,csumt;
  DCP dc,dcf,dcf0;
  LUM *c;
  ML wlist;

  n = UDEG(f); np = list->n; bound = list->bound; q = list->mod;
  win = W_ALLOC(np+1);
  ucsump(f,&csum); mulq(csum,(Q)COEF(DC(f)),&csumt); csum = csumt;
  wlist = W_MLALLOC(np); wlist->n = list->n;
  wlist->mod = list->mod; wlist->bound = list->bound;
  c = (LUM *)COEF(wlist); bcopy((char *)COEF(list),(char *)c,(int)(sizeof(LUM)*np));
  for ( g = f, i = 130000; i < (1<<20); i++ ) {
#if 0
    if ( !(i % 1000) )
      fprintf(stderr,"i=%d\n",i);
#endif
    for ( j = 0; j < 20; j++ )
      win[j] = (i&(1<<j)) ? 2*j+1 : 2*j;
    if ( dtestmain(g,csum,wlist,20,win,&factor,&cofactor) ) {
#if 0
      fprintf(stderr,"success : i=%d\n",i);
#endif
      *pr = factor; return;
    }
  }
}

void hensel_special(index,count,f,mfl,listp)
int index,count;
P f;
P *mfl;
ML *listp;
{
  register int i,j;
  int q,n,t,d,r,u,br,tmp,bound;
  int *c,*p,*m,*w;
  int **pp;
  DCP dc;
  ML blist,clist,bqlist,cqlist,rlist;
  UM *b;
  LUM fl,tl;
  LUM *l;

  blist = MLALLOC(40); blist->n = 40; blist->mod = 11;
  for ( i = 0; i < 40; i++ ) {
    blist->c[i] = (pointer)UMALLOC(6);
    ptoum(11,mfl[i],blist->c[i]);
  }
  gcdgen(f,blist,&clist); henprep(f,blist,clist,&bqlist,&cqlist);
  n = bqlist->n; q = bqlist->mod;
  bqlist->bound = cqlist->bound = bound = mignotte(q,f);
  if ( bound == 1 ) {
    *listp = rlist = MLALLOC(n); 
    rlist->n = n; rlist->mod = q; rlist->bound = bound;
    for ( i = 0, b = (UM *)bqlist->c, l = (LUM *)rlist->c; i < n; i++ ) {
      tl = LUMALLOC(DEG(b[i]),1); l[i] = tl; p = COEF(b[i]);
      for ( j = 0, pp = COEF(tl); j <= DEG(tl); j++ )
          pp[j][0] = p[j];
    }
  } else {
    W_LUMALLOC(UDEG(f),bound,fl);
    ptolum(q,bound,f,fl); henmain(fl,bqlist,cqlist,listp);
  }
}
#endif

#if 0
void Pftest(arg,rp)
NODE arg;
P *rp;
{
  ML list;
  DCP dc;
  P p;
  P *mfl;

  p = (P)ARG0(arg); mfl = (P *)(((VECT)ARG1(arg))->body);
  hensel_special(2,1,p,mfl,&list);
  dtest_special(p,list,rp);
}

void dtest_special(f,list,pr)
P f;
ML list;
P *pr;
{
  int n,np,bound,q;
  int i,j,k,t,b0;
  int *win;
  P g,factor,cofactor;
  Q csum,csumt;
  DCP dc,dcf,dcf0;
  LUM *c;
  ML wlist;
  static int nbits[16] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};

  n = UDEG(f); np = list->n; bound = list->bound; q = list->mod;
  win = W_ALLOC(np+1);
  ucsump(f,&csum); mulq(csum,(Q)COEF(DC(f)),&csumt); csum = csumt;
  wlist = W_MLALLOC(np); wlist->n = list->n;
  wlist->mod = list->mod; wlist->bound = list->bound;
  c = (LUM *)COEF(wlist); bcopy((char *)COEF(list),(char *)c,(int)(sizeof(LUM)*np));
  for ( g = f, i = 0; i < (1<<23); i++ ) {
#if 0
    if ( !(i % 1000) )
    fprintf(stderr,"i=%d\n",i);
#endif
    t = i>>20; b0 = nbits[t];
    if ( !b0 )
      continue;
    for ( j = 1; j < 6; j++ ) {
      t = (i>>(20-4*j))&0xf;
      if ( nbits[t] != b0 )
        break;
    }
    if ( j != 6 )
      continue;
    for ( j = k = 0; j < 24; j++ )
      if ( i & (1<<(23-j)) )
        win[k++] = j;
    if ( dtestmain(g,csum,wlist,k,win,&factor,&cofactor) ) {
#if 0
      fprintf(stderr,"success : i=%d\n",i);
#endif
      *pr = factor; return;
    }
  }
  *pr = f;
}

void hensel_special(index,count,f,mfl,listp)
int index,count;
P f;
P *mfl;
ML *listp;
{
  register int i,j;
  int q,n,t,d,r,u,br,tmp,bound;
  int *c,*p,*m,*w;
  int **pp;
  DCP dc;
  ML blist,clist,bqlist,cqlist,rlist;
  UM *b;
  LUM fl,tl;
  LUM *l;

  blist = MLALLOC(24); blist->n = 24; blist->mod = 5;
  for ( i = 0; i < 24; i++ ) {
    blist->c[i] = (pointer)UMALLOC(7);
    ptoum(5,mfl[i],blist->c[i]);
  }
  gcdgen(f,blist,&clist); henprep(f,blist,clist,&bqlist,&cqlist);
  n = bqlist->n; q = bqlist->mod;
  bqlist->bound = cqlist->bound = bound = mignotte(q,f);
  if ( bound == 1 ) {
    *listp = rlist = MLALLOC(n); 
    rlist->n = n; rlist->mod = q; rlist->bound = bound;
    for ( i = 0, b = (UM *)bqlist->c, l = (LUM *)rlist->c; i < n; i++ ) {
      tl = LUMALLOC(DEG(b[i]),1); l[i] = tl; p = COEF(b[i]);
      for ( j = 0, pp = COEF(tl); j <= DEG(tl); j++ )
          pp[j][0] = p[j];
    }
  } else {
    W_LUMALLOC(UDEG(f),bound,fl);
    ptolum(q,bound,f,fl); henmain(fl,bqlist,cqlist,listp);
  }
}
#endif

void Pftest(arg,rp)
NODE arg;
P *rp;
{
  ML list;
  P p;
  P *mfl;

  p = (P)ARG0(arg); mfl = (P *)(((VECT)ARG1(arg))->body);
  hensel_special(5,1,p,mfl,&list);
  dtest_special(p,list,rp);
}

int nbits(a)
int a;
{
  int i,s;

  for ( i = 0, s = 0; a && (i < 20); i++, a >>= 1 )
    if ( a & 1 ) s++;
  return s;
}

void dtest_special(f,list,pr)
P f;
ML list;
P *pr;
{
  int n,np,bound,q;
  int i,j,k,b0;
  int *win;
  P g,factor,cofactor;
  Q csum,csumt;
  LUM *c;
  ML wlist;

  n = UDEG(f); np = list->n; bound = list->bound; q = list->mod;
  win = W_ALLOC(np+1);
  ucsump(f,&csum); mulq(csum,(Q)COEF(DC(f)),&csumt); csum = csumt;
  wlist = W_MLALLOC(np); wlist->n = list->n;
  wlist->mod = list->mod; wlist->bound = list->bound;
  c = (LUM *)COEF(wlist); bcopy((char *)COEF(list),(char *)c,(int)(sizeof(LUM)*np));
  for ( g = f, i = 0; i < (1<<19); i++ ) {
#if 0
    if ( !(i % 10000) )
    fprintf(stderr,"i=%d\n",i);
#endif
    b0 = nbits(i>>10);
    if ( !b0 || (nbits(i&0x3ff) != b0) )
      continue;
    for ( j = k = 0; j < 20; j++ )
      if ( i & (1<<(19-j)) )
        win[k++] = j;
    if ( dtestmain(g,csum,wlist,k,win,&factor,&cofactor) ) {
#if 0
      fprintf(stderr,"success : i=%d\n",i);
#endif
      *pr = factor; return;
    }
  }
  *pr = f;
}

void hensel_special(index,count,f,mfl,listp)
int index,count;
P f;
P *mfl;
ML *listp;
{
  register int i,j;
  int q,n,bound;
  int *p;
  int **pp;
  ML blist,clist,bqlist,cqlist,rlist;
  UM *b;
  LUM fl,tl;
  LUM *l;

  blist = MLALLOC(20); blist->n = 20; blist->mod = 11;
  for ( i = 0; i < 20; i++ ) {
    blist->c[i] = (pointer)UMALLOC(10);
    ptoum(11,mfl[i],blist->c[i]);
  }
  gcdgen(f,blist,&clist); henprep(f,blist,clist,&bqlist,&cqlist);
  n = bqlist->n; q = bqlist->mod;
  bqlist->bound = cqlist->bound = bound = mignotte(q,f);
  if ( bound == 1 ) {
    *listp = rlist = MLALLOC(n); 
    rlist->n = n; rlist->mod = q; rlist->bound = bound;
    for ( i = 0, b = (UM *)bqlist->c, l = (LUM *)rlist->c; i < n; i++ ) {
      tl = LUMALLOC(DEG(b[i]),1); l[i] = tl; p = COEF(b[i]);
      for ( j = 0, pp = COEF(tl); j <= DEG(tl); j++ )
          pp[j][0] = p[j];
    }
  } else {
    W_LUMALLOC((int)UDEG(f),bound,fl);
    ptolum(q,bound,f,fl); henmain(fl,bqlist,cqlist,listp);
  }
}

void Pnullspace(arg,rp)
NODE arg;
LIST *rp;
{
  int i,j,n,mod;
  MAT mat,r;
  VECT u;
  V v;
  P p,z;
  Q q;
  UM **w;
  UM mp;
  P *t;
  UM *s;
  int *ind;
  NODE n0,n1;

  mat = (MAT)ARG0(arg);
  p = (P)ARG1(arg);
  v = VR(p);
  mod = QTOS((Q)ARG2(arg));
  n = mat->row;
  w = (UM **)almat_pointer(n,n);
  for ( i = 0; i < n; i++ )
    for ( j = 0, t = (P *)mat->body[i], s = w[i]; j < n; j++ ) {
      ptomp(mod,t[j],&z);
      s[j] = W_UMALLOC((z&&!NUM(z))?UDEG(z):0);
      mptoum(z,s[j]);
    }
  mp = W_UMALLOC(UDEG(p)); ptoum(mod,p,mp);
  ind = (int *)ALLOCA(n*sizeof(int));
  nullspace(w,mp,mod,n,ind);
  MKMAT(r,n,n);
  for ( i = 0; i < n; i++ )
    for ( j = 0, t = (P *)r->body[i], s = w[i]; j < n; j++ )
      umtop(v,s[j],&t[j]);
  MKVECT(u,n);
  for ( i = 0; i < n; i++ ) {
    STOQ(ind[i],q); u->body[i] = (pointer)q;
  }
  MKNODE(n1,u,0); MKNODE(n0,r,n1); MKLIST(*rp,n0);
}

void nullspace(mat,p,mod,n,ind)
UM **mat;
UM p;
int mod,n;
int *ind;
{
  int i,j,l,s,d;
  UM q,w,w1,h,inv;
  UM *t,*u;

  d = DEG(p); inv = W_UMALLOC(d); q = W_UMALLOC(2*d);
  w = W_UMALLOC(2*d); w1 = W_UMALLOC(2*d); h = W_UMALLOC(d);
  bzero(ind,n*sizeof(int));
  ind[0] = 0;
  for ( i = j = 0; j < n; i++, j++ ) {
    for ( ; j < n; j++ ) {
      for ( l = i; l < n; l++ )
        if ( DEG(mat[l][j])>=0 )
          break;
      if ( l < n ) {
        t = mat[i]; mat[i] = mat[l]; mat[l] = t; break;
      } else
        ind[j] = 1;
    }
    if ( j == n )
      break;
    invum(mod,p,mat[i][j],inv);
    for ( s = j, t = mat[i]; s < n; s++ ) {
      mulum(mod,t[s],inv,w);
      DEG(w) = divum(mod,w,p,q);
      cpyum(w,t[s]);
    }
    for ( l = 0; l < n; l++ ) {
      if ( l == i )
        continue;
      u = mat[l]; DEG(w) = -1; subum(mod,w,u[j],h);
      for ( s = j; s < n; s++ ) {
        mulum(mod,h,t[s],w); addum(mod,w,u[s],w1);
        DEG(w1) = divum(mod,w1,p,q); cpyum(w1,u[s]);
      }
    }
  }
}

void Pnullspace_ff(arg,rp)
NODE arg;
LIST *rp;
{
  int i,j,n;
  MAT mat,r;
  VECT u;
  Q q;
  Obj **w;
  Obj *t;
  Obj *s;
  int *ind;
  NODE n0,n1;

  mat = (MAT)ARG0(arg);
  n = mat->row;
  w = (Obj **)almat_pointer(n,n);
  for ( i = 0; i < n; i++ )
    for ( j = 0, t = (Obj *)mat->body[i], s = w[i]; j < n; j++ )
      s[j] = t[j];
  ind = (int *)ALLOCA(n*sizeof(int));
  switch ( current_ff ) {
    case FF_GFP:
      nullspace_lm((LM **)w,n,ind); break;
    case FF_GF2N:
      nullspace_gf2n((GF2N **)w,n,ind); break;
    case FF_GFPN:
      nullspace_gfpn((GFPN **)w,n,ind); break;
    case FF_GFS:
      nullspace_gfs((GFS **)w,n,ind); break;
    case FF_GFSN:
      nullspace_gfsn((GFSN **)w,n,ind); break;
    default:
      error("nullspace_ff : current_ff is not set");
  }
  MKMAT(r,n,n);
  for ( i = 0; i < n; i++ )
    for ( j = 0, t = (Obj *)r->body[i], s = w[i]; j < n; j++ )
      t[j] = s[j];
  MKVECT(u,n);
  for ( i = 0; i < n; i++ ) {
    STOQ(ind[i],q); u->body[i] = (pointer)q;
  }
  MKNODE(n1,u,0); MKNODE(n0,r,n1); MKLIST(*rp,n0);
}

void nullspace_lm(mat,n,ind)
LM **mat;
int n;
int *ind;
{
  int i,j,l,s;
  Q q,mod;
  N lm;
  LM w,w1,h,inv;
  LM *t,*u;

  getmod_lm(&lm); NTOQ(lm,1,mod);

  bzero(ind,n*sizeof(int));
  ind[0] = 0;
  for ( i = j = 0; j < n; i++, j++ ) {
    for ( ; j < n; j++ ) {
      for ( l = i; l < n; l++ ) {
        simplm(mat[l][j],&w); mat[l][j] = w;
        if ( mat[l][j] )
          break;
      }
      if ( l < n ) {
        t = mat[i]; mat[i] = mat[l]; mat[l] = t; break;
      } else
        ind[j] = 1;
    }
    if ( j == n )
      break;
    NTOQ(mat[i][j]->body,1,q); invl(q,mod,(Q *)&inv);
    for ( s = j, t = mat[i]; s < n; s++ ) {
      mullm(t[s],inv,&w); t[s] = w;
    }
    for ( l = 0; l < n; l++ ) {
      if ( l == i )
        continue;
      u = mat[l]; chsgnlm(u[j],&h);
      for ( s = j; s < n; s++ ) {
        mullm(h,t[s],&w); addlm(w,u[s],&w1); u[s] = w1;
      }
    }
  }
}

void nullspace_gf2n(mat,n,ind)
GF2N **mat;
int n;
int *ind;
{
  int i,j,l,s;
  GF2N w,w1,h,inv;
  GF2N *t,*u;
  extern gf2n_lazy;

  bzero(ind,n*sizeof(int));
  ind[0] = 0;
  for ( i = j = 0; j < n; i++, j++ ) {
    for ( ; j < n; j++ ) {
      for ( l = i; l < n; l++ ) {
        simpgf2n(mat[l][j],&w); mat[l][j] = w;
        if ( mat[l][j] )
          break;
      }
      if ( l < n ) {
        t = mat[i]; mat[i] = mat[l]; mat[l] = t; break;
      } else
        ind[j] = 1;
    }
    if ( j == n )
      break;
    invgf2n(mat[i][j],&inv);
    for ( s = j, t = mat[i]; s < n; s++ ) {
      mulgf2n(t[s],inv,&w); t[s] = w;
    }
    for ( l = 0; l < n; l++ ) {
      if ( l == i )
        continue;
      u = mat[l]; h = u[j];
      for ( s = j; s < n; s++ ) {
        mulgf2n(h,t[s],&w); addgf2n(w,u[s],&w1); u[s] = w1;
      }
    }
  }
}

void nullspace_gfpn(mat,n,ind)
GFPN **mat;
int n;
int *ind;
{
  int i,j,l,s;
  GFPN w,w1,h,inv;
  GFPN *t,*u;

  bzero(ind,n*sizeof(int));
  ind[0] = 0;
  for ( i = j = 0; j < n; i++, j++ ) {
    for ( ; j < n; j++ ) {
      for ( l = i; l < n; l++ ) {
        simpgfpn(mat[l][j],&w); mat[l][j] = w;
        if ( mat[l][j] )
          break;
      }
      if ( l < n ) {
        t = mat[i]; mat[i] = mat[l]; mat[l] = t; break;
      } else
        ind[j] = 1;
    }
    if ( j == n )
      break;
    divgfpn((GFPN)ONE,(GFPN)mat[i][j],&inv);
    for ( s = j, t = mat[i]; s < n; s++ ) {
      mulgfpn(t[s],inv,&w); t[s] = w;
    }
    for ( l = 0; l < n; l++ ) {
      if ( l == i )
        continue;
      u = mat[l]; chsgngfpn(u[j],&h);
      for ( s = j; s < n; s++ ) {
        mulgfpn(h,t[s],&w); addgfpn(w,u[s],&w1); u[s] = w1;
      }
    }
  }
}

void nullspace_gfs(mat,n,ind)
GFS **mat;
int n;
int *ind;
{
  int i,j,l,s;
  GFS w,w1,h,inv;
  GFS *t,*u;
  GFS one;

  bzero(ind,n*sizeof(int));
  ind[0] = 0;
  mqtogfs(ONEM,&one);

  for ( i = j = 0; j < n; i++, j++ ) {
    for ( ; j < n; j++ ) {
      for ( l = i; l < n; l++ )
        if ( mat[l][j] )
          break;
      if ( l < n ) {
        t = mat[i]; mat[i] = mat[l]; mat[l] = t; break;
      } else
        ind[j] = 1;
    }
    if ( j == n )
      break;
    divgfs(one,mat[i][j],&inv);
    for ( s = j, t = mat[i]; s < n; s++ ) {
      mulgfs(t[s],inv,&w); t[s] = w;
    }
    for ( l = 0; l < n; l++ ) {
      if ( l == i )
        continue;
      u = mat[l]; 
      chsgngfs(u[j],&h);
      for ( s = j; s < n; s++ ) {
        mulgfs(h,t[s],&w); addgfs(w,u[s],&w1); u[s] = w1;
      }
    }
  }
}

void nullspace_gfsn(mat,n,ind)
GFSN **mat;
int n;
int *ind;
{
  int i,j,l,s;
  GFSN w,w1,h,inv;
  GFSN *t,*u;

  bzero(ind,n*sizeof(int));
  ind[0] = 0;

  for ( i = j = 0; j < n; i++, j++ ) {
    for ( ; j < n; j++ ) {
      for ( l = i; l < n; l++ )
        if ( mat[l][j] )
          break;
      if ( l < n ) {
        t = mat[i]; mat[i] = mat[l]; mat[l] = t; break;
      } else
        ind[j] = 1;
    }
    if ( j == n )
      break;
    invgfsn(mat[i][j],&inv);
    for ( s = j, t = mat[i]; s < n; s++ ) {
      mulgfsn(t[s],inv,&w); t[s] = w;
    }
    for ( l = 0; l < n; l++ ) {
      if ( l == i )
        continue;
      u = mat[l]; 
      chsgngfsn(u[j],&h);
      for ( s = j; s < n; s++ ) {
        mulgfsn(h,t[s],&w); addgfsn(w,u[s],&w1); u[s] = w1;
      }
    }
  }
}

/* p = a(0)vl[0]+a(1)vl[1]+...+a(m-1)vl[m-1]+a(m) -> array = [a(0) a(1) ... a(m)] */

void linear_form_to_array(p,vl,m,array)
P p;
VL vl;
int m;
Num *array;
{
  int i;
  DCP dc;

  bzero((char *)array,(m+1)*sizeof(Num *));
  for ( i = 0; p && vl; vl = NEXT(vl), i++ ) {
    if ( ID(p) == O_N )
      break;
    else if ( VR(p) == vl->v ) {
      dc = DC(p);
      array[i] = (Num)COEF(dc);
      dc = NEXT(dc);
      p = dc ? COEF(dc) : 0;
    }
  }
  array[m] = (Num)p;
}

void array_to_linear_form(array,vl,m,r)
Num *array;
VL vl;
int m;
P *r;
{
  P t;
  DCP dc0,dc1;

  if ( !m )
    *r = (P)array[0];
  else {
    array_to_linear_form(array+1,NEXT(vl),m-1,&t);
    if ( !array[0] )
      *r = t;
    else {
      NEWDC(dc0); DEG(dc0) = ONE; COEF(dc0) = (P)array[0];
      if ( !t )
        NEXT(dc0) = 0;
      else {
        NEWDC(dc1); DEG(dc1) = 0; COEF(dc1) = t;
        NEXT(dc1) = 0;
        NEXT(dc0) = dc1;
      }
      MKP(vl->v,dc0,*r);        
    }
  }
}

void Psolve_linear_equation_gf2n(arg,rp)
NODE arg;
LIST *rp;
{
  NODE eqs,tn;
  VL vars,tvl;
  int i,j,n,m,dim,codim;
  GF2N **w;
  int *ind;
  NODE n0,n1;

  get_vars(ARG0(arg),&vars);
  eqs = BDY((LIST)ARG0(arg));
  for ( n = 0, tn = eqs; tn; tn = NEXT(tn), n++);
  for ( m = 0, tvl = vars; tvl; tvl = NEXT(tvl), m++);
  w = (GF2N **)almat_pointer(n,m+1);
  for ( i = 0, tn = eqs; i < n; i++, tn = NEXT(tn) )
    linear_form_to_array(BDY(tn),vars,m,(Num *)w[i]);
  ind = (int *)ALLOCA(m*sizeof(int));
  solve_linear_equation_gf2n(w,n,m,ind);
  for ( j = 0, dim = 0; j < m; j++ )
    if ( ind[j] )
      dim++;
  codim = m-dim;
  for ( i = codim; i < n; i++ )
    if ( w[i][m] ) {
      MKLIST(*rp,0); return;
    }
  for ( i = 0, n0 = 0; i < codim; i++ ) {
    NEXTNODE(n0,n1);
    array_to_linear_form((Num *)w[i],vars,m,(P *)&BDY(n1));
  }
  if ( n0 )
    NEXT(n1) = 0;
  MKLIST(*rp,n0);
}

void solve_linear_equation_gf2n(mat,n,m,ind)
GF2N **mat;
int n;
int *ind;
{
  int i,j,l,s;
  GF2N w,w1,h,inv;
  GF2N *t,*u;
  extern gf2n_lazy;

  bzero(ind,m*sizeof(int));
  ind[0] = 0;
  for ( i = j = 0; j < m; i++, j++ ) {
    for ( ; j < m; j++ ) {
      for ( l = i; l < n; l++ ) {
        simpgf2n(mat[l][j],&w); mat[l][j] = w;
        if ( mat[l][j] )
          break;
      }
      if ( l < n ) {
        t = mat[i]; mat[i] = mat[l]; mat[l] = t; break;
      } else
        ind[j] = 1;
    }
    if ( j == m )
      break;
    invgf2n(mat[i][j],&inv);
    for ( s = j, t = mat[i]; s <= m; s++ ) {
      mulgf2n(t[s],inv,&w); t[s] = w;
    }
    for ( l = 0; l < n; l++ ) {
      if ( l == i )
        continue;
      u = mat[l]; h = u[j];
      for ( s = j; s <= m; s++ ) {
        mulgf2n(h,t[s],&w); addgf2n(w,u[s],&w1); u[s] = w1;
      }
    }
  }
}

/*
void null_to_sol(mat,ind,mod,n,r)
int **mat;
int *ind;
int mod,n;
UM *r;
{
  int i,j,k,l;
  int *c;
  UM w;

  for ( i = 0, l = 0; i < n; i++ ) {
    if ( !ind[i] )
      continue;
    w = UMALLOC(n);
    for ( j = k = 0, c = COEF(w); j < n; j++ )
      if ( ind[j] )
        c[j] = 0;
      else
        c[j] = mat[k++][i];
    c[i] = mod-1;
    for ( j = n; j >= 0; j-- )
      if ( c[j] )
        break;
    DEG(w) = j;
    r[l++] = w;
  }
}
*/

void showgfmat(mat,n)
UM **mat;
int n;
{
  int i,j,k;
  int *c;
  UM p;

  for ( i = 0; i < n; i++ ) {
    for ( j = 0; j < n; j++ ) {
      p = mat[i][j];
      if ( DEG(p) < 0 )
        fprintf(asir_out,"0");
      else 
        for ( p = mat[i][j], k = DEG(p), c = COEF(p); k >= 0; k-- ) {
          if ( c[k] )
            fprintf(asir_out,"+%d",c[k]);
          if ( k > 1 )
            fprintf(asir_out,"a^%d",k);
          else if ( k == 1 )
            fprintf(asir_out,"a",k);
        }
      fprintf(asir_out," ");
    }
    fprintf(asir_out,"\n");
  }
}

#if 0
void Pgcda_mod(arg,rp)
NODE arg;
P *rp;
{
  p1 = (P)ARG0(arg);
  p2 = (P)ARG1(arg);
  v = VR((P)ARG2(arg));
  d = (P)ARG3(arg);
  m = QTOS((Q)ARG4(arg));
  reordvar(CO,v,&vl);
  reorderp(vl,CO,p1,&t); ptomp(m,t,&m1);
  reorderp(vl,CO,p2,&t); ptomp(m,t,&m2);
  if ( NUM(m1) || NUM(m2) || VR(m1) != v || VR(m2) != v ) {
    *rp = ONE; return;
  }
  if ( deg(v,m1) >= deg(v,m2) ) {
    t = m1; m1 = m2; m2 = t;
  }
  while ( 1 ) {
    inva_mod(COEF(DC(m2)),d,m,&inv);
    NEWDC(dc); NEXT(dc) = 0; MKP(v,dc,h);
    d0 = deg(v,m1)-deg(v,m2); STOQ(d0,DEG(dc));
    mulgq(m,d,inv,COEF(DC(m1)),&COEF(dc));
    mulgp(vl,m,d,m2,h,&t); subgp(vl,m,d,m1,t,&s);
  }
}
#endif

void Ppwr_mod(arg,rp)
NODE arg;
P *rp;
{
  P p,a,d,r;
  int m;
  Q q;
  N n;

  m = QTOS((Q)ARG4(arg)); q = (Q)ARG5(arg); n = q ? NM(q) : 0;
  ptomp(m,(P)ARG0(arg),&p); ptomp(m,(P)ARG1(arg),&a);
  ptomp(m,(P)ARG3(arg),&d);
  pwr_mod(p,a,VR((P)ARG2(arg)),d,m,n,&r);
  mptop(r,rp);
}

void pwr_mod(p,a,v,d,m,n,rp)
P p,a,d;
V v;
int m;
N n;
P *rp;
{
  int b;
  P t,s,r;
  N n1;

  if ( !n )
    *rp = (P)ONEM;
  else if ( UNIN(n) )
    *rp = p;
  else {
    b = divin(n,2,&n1);
    pwr_mod(p,a,v,d,m,n1,&t);
    mulmp(CO,m,t,t,&s); rem_mod(s,a,v,d,m,&r);
    if ( b ) {
      mulmp(CO,m,r,p,&t); rem_mod(t,a,v,d,m,rp);
    } else
      *rp = r;
  }
}

void rem_mod(p,a,v,d,m,rp)
P p,a,d;
V v;
int m;
P *rp;
{
  P tmp,r1,r2;

  divsrmp(CO,m,p,d,&tmp,&r1);
  divsrmp(CO,m,r1,a,&tmp,&r2);
  divsrmp(CO,m,r2,d,&tmp,rp);
}
