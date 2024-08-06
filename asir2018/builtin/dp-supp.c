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
 * $OpenXM: OpenXM_contrib2/asir2018/builtin/dp-supp.c,v 1.18 2022/09/10 04:04:50 noro Exp $
*/
#include "ca.h"
#include "base.h"
#include "inline.h"
#include "parse.h"
#include "ox.h"

#define HMAG(p) (p_mag((P)BDY(p)->c))

extern int (*cmpdl)(int,DL,DL);
extern double pz_t_e,pz_t_d,pz_t_d1,pz_t_c;
extern int dp_nelim,dp_fcoeffs;
extern int NoGCD;
extern int GenTrace;
extern NODE TraceList;

int show_orderspec;

void print_composite_order_spec(struct order_spec *spec);
void dpm_rest(DPM,DPM *);
void _muldc(VL vl,DP *p,Obj c);

/* 
 * content reduction
 *
 */

static NODE RatDenomList;

void init_denomlist()
{
  RatDenomList = 0;
}

void add_denomlist(P f)
{
  NODE n;

  if ( OID(f)==O_P ) {
    MKNODE(n,f,RatDenomList); RatDenomList = n;
  }
}

LIST get_denomlist()
{
  LIST l;

  MKLIST(l,RatDenomList); RatDenomList = 0;
  return l;
}

int dp_iszp(DP p)
{
  MP m;

  if ( !p ) return 1;
  for ( m = BDY(p); m; m = NEXT(m))
    if ( !INT(m->c) ) return 0;
  return 1;
}

void dp_ptozp(DP p,DP *rp)
{
  MP m,mr,mr0;
  int i,n;
  Q *w;
  Z dvr;
  P t;

  if ( !p )
    *rp = 0;
  else {
    for ( m =BDY(p), n = 0; m; m = NEXT(m), n++ );
    w = (Q *)ALLOCA(n*sizeof(Q));
    for ( m =BDY(p), i = 0; i < n; m = NEXT(m), i++ )
      if ( NUM(m->c) )
        w[i] = (Q)m->c;
      else
        ptozp((P)m->c,1,&w[i],&t);
    sortbynm(w,n);  
    qltozl(w,n,&dvr);
    for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
      NEXTMP(mr0,mr); divsp(CO,(P)m->c,(P)dvr,(P *)&mr->c); mr->dl = m->dl;
    }
    NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
  }
}

void dp_ptozp2(DP p0,DP p1,DP *hp,DP *rp)
{
  DP t,s,h,r;
  MP m,mr,mr0,m0;

  addd(CO,p0,p1,&t); dp_ptozp(t,&s);
  if ( !p0 ) {
    h = 0; r = s;
  } else if ( !p1 ) {
    h = s; r = 0;
  } else {
    for ( mr0 = 0, m = BDY(s), m0 = BDY(p0); m0;
      m = NEXT(m), m0 = NEXT(m0) ) {
      NEXTMP(mr0,mr); mr->c = m->c; mr->dl = m->dl;
    }
    NEXT(mr) = 0; MKDP(p0->nv,mr0,h); MKDP(p0->nv,m,r);
  }
  if ( h )
    h->sugar = p0->sugar;
  if ( r )
    r->sugar = p1->sugar;
  *hp = h; *rp = r;
}

int dpm_iszp(DPM p)
{
  DMM m;

  if ( !p ) return 1;
  for ( m = BDY(p); m; m = NEXT(m))
    if ( !INT(m->c) ) return 0;
  return 1;
}

void dpm_ptozp(DPM p,Z *cont,DPM *rp)
{
  DMM m,mr,mr0;
  int i,n;
  Q *w;
  Z dvr;
  P t;

  if ( !p ) {
    *rp = 0; *cont = ONE;
  } else {
    for ( m =BDY(p), n = 0; m; m = NEXT(m), n++ );
    w = (Q *)ALLOCA(n*sizeof(Q));
    for ( m =BDY(p), i = 0; i < n; m = NEXT(m), i++ )
      if ( NUM(m->c) )
        w[i] = (Q)m->c;
      else
        ptozp((P)m->c,1,&w[i],&t);
    sortbynm(w,n);  
    qltozl(w,n,&dvr);
    for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
      NEXTDMM(mr0,mr); divsp(CO,(P)m->c,(P)dvr,(P *)&mr->c); mr->dl = m->dl; mr->pos = m->pos;
    }
    NEXT(mr) = 0; MKDPM(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
    *cont = dvr;
  }
}

void dpm_monic_sf(DPM p,DPM *rp);

void dpm_prim(DPM p,DPM *rp)
{
  P t,g;
  DPM p1;
  DMM m,mr,mr0;
  int i,n;
  P *w;
  Q *c;
  Z dvr;
  NODE tn;

  if ( !p )
    *rp = 0;
  else if ( dp_fcoeffs == N_GFS ) {
    for ( m = BDY(p); m; m = NEXT(m) )
      if ( OID(m->c) == O_N ) {
        /* GCD of coeffs = 1 */
        dpm_monic_sf(p,rp);
        return;
      } else break;
    /* compute GCD over the finite fieid */
    for ( m = BDY(p), n = 0; m; m = NEXT(m), n++ );
    w = (P *)ALLOCA(n*sizeof(P));
    for ( m = BDY(p), i = 0; i < n; m = NEXT(m), i++ )
      w[i] = (P)m->c;
    gcdsf(CO,w,n,&g);
    if ( NUM(g) )
      dpm_monic_sf(p,rp);
    else {
      for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
        NEXTDMM(mr0,mr); divsp(CO,(P)m->c,g,(P *)&mr->c); mr->dl = m->dl; mr->pos = m->pos;
      }
      NEXT(mr) = 0; MKDPM(p->nv,mr0,p1); p1->sugar = p->sugar;
      dpm_monic_sf(p1,rp);
    }
    return;
  } else if ( dp_fcoeffs )
    *rp = p;
  else if ( NoGCD )
    dpm_ptozp(p,&dvr,rp);
  else {
    dpm_ptozp(p,&dvr,&p1); p = p1;
    for ( m = BDY(p), n = 0; m; m = NEXT(m), n++ );
    if ( n == 1 ) {
      m = BDY(p);
      NEWDMM(mr); mr->dl = m->dl; mr->pos = m->pos; mr->c = (Obj)ONE; NEXT(mr) = 0;
      MKDPM(p->nv,mr,*rp); (*rp)->sugar = p->sugar;
      return;
    }
    w = (P *)ALLOCA(n*sizeof(P));
    c = (Q *)ALLOCA(n*sizeof(Q));
    for ( m =BDY(p), i = 0; i < n; m = NEXT(m), i++ )
      if ( NUM(m->c) ) {
        c[i] = (Q)m->c; w[i] = (P)ONE;
      } else
        ptozp((P)m->c,1,&c[i],&w[i]);
    qltozl(c,n,&dvr); heu_nezgcdnpz(CO,w,n,&t); mulp(CO,t,(P)dvr,&g);
    if ( NUM(g) )
      *rp = p;
    else {
      for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
        NEXTDMM(mr0,mr); divsp(CO,(P)m->c,g,(P *)&mr->c); mr->dl = m->dl; mr->pos = m->pos;
      }
      NEXT(mr) = 0; MKDPM(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
      add_denomlist(g);
    }
  }
}

void dpm_ptozp2(DPM p0,DPM p1,DPM *hp,DPM *rp)
{
  DPM t,s,h,r;
  DMM m,mr,mr0,m0;
  Z cont;

  adddpm(CO,p0,p1,&t); dpm_ptozp(t,&cont,&s);
  if ( !p0 ) {
    h = 0; r = s;
  } else if ( !p1 ) {
    h = s; r = 0;
  } else {
    for ( mr0 = 0, m = BDY(s), m0 = BDY(p0); m0;
      m = NEXT(m), m0 = NEXT(m0) ) {
      NEXTDMM(mr0,mr); mr->c = m->c; mr->dl = m->dl; mr->pos = m->pos;
    }
    NEXT(mr) = 0; MKDPM(p0->nv,mr0,h); MKDPM(p0->nv,m,r);
  }
  if ( h )
    h->sugar = p0->sugar;
  if ( r )
    r->sugar = p1->sugar;
  *hp = h; *rp = r;
}


void dp_ptozp3(DP p,Z *dvr,DP *rp)
{
  MP m,mr,mr0;
  int i,n;
  Q *w;
  P t;

  if ( !p ) {
    *rp = 0; *dvr = 0;
  }else {
    for ( m =BDY(p), n = 0; m; m = NEXT(m), n++ );
    w = (Q *)ALLOCA(n*sizeof(Q));
    for ( m =BDY(p), i = 0; i < n; m = NEXT(m), i++ )
      if ( NUM(m->c) )
        w[i] = (Q)m->c;
      else
        ptozp((P)m->c,1,&w[i],&t);
    sortbynm(w,n);  
    qltozl(w,n,dvr);
    for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
      NEXTMP(mr0,mr); divsp(CO,(P)m->c,(P)(*dvr),(P *)&mr->c); mr->dl = m->dl;
    }
    NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
  }
}

void dp_idiv(DP p,Z c,DP *rp)
{
  MP mr0,m,mr;

  if ( !p )
    *rp = 0;
  else if ( MUNIQ((Q)c) )
    *rp = p;
  else if ( MUNIQ((Q)c) )
    chsgnd(p,rp);
  else {
    for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
      NEXTMP(mr0,mr);
      divsz((Z)(m->c),c,(Z *)&mr->c);
      mr->dl = m->dl;
    }
    NEXT(mr) = 0; MKDP(p->nv,mr0,*rp);
    if ( *rp )
      (*rp)->sugar = p->sugar;
  }
}

void dp_mbase(NODE hlist,NODE *mbase)
{
  DL *dl;
  DL d;
  int *t;
  int i,j,k,n,nvar,td;

  n = length(hlist); nvar = ((DP)BDY(hlist))->nv;
  dl = (DL *)MALLOC(n*sizeof(DL));
  NEWDL(d,nvar); *mbase = 0;
  for ( i = 0; i < n; i++, hlist = NEXT(hlist) ) {
    dl[i] = BDY((DP)BDY(hlist))->dl;
    /* trivial ideal check */
    if ( (*cmpdl)(nvar,d,dl[i]) == 0 ) {
      return;
    }
  }
  /* zero-dim. ideal check */
  for ( i = 0; i < nvar; i++ ) {
    for ( j = 0; j < n; j++ ) {
      for ( k = 0, t = dl[j]->d; k < nvar; k++ )
        if ( k != i && t[k] != 0 ) break;
      if ( k == nvar ) break;
    }
    if ( j == n )
      error("dp_mbase : input ideal is not zero-dimensional");
  }
  while ( 1 ) {
    insert_to_node(d,mbase,nvar);
    for ( i = nvar-1; i >= 0; ) {
      d->d[i]++;
      d->td += MUL_WEIGHT(1,i);
      for ( j = 0; j < n; j++ ) {
        if ( _dl_redble(dl[j],d,nvar) )
          break;
      }
      if ( j < n ) {
        for ( j = nvar-1; j >= i; j-- )
          d->d[j] = 0;
        for ( j = 0, td = 0; j < i; j++ )
          td += MUL_WEIGHT(d->d[j],j);
        d->td = td;
        i--;
      } else
        break;
    }
    if ( i < 0 )
      break;
  }
}

int _dl_redble(DL d1,DL d2,int nvar)
{
  int i;

  if ( d1->td > d2->td )
    return 0;
  for ( i = 0; i < nvar; i++ )
    if ( d1->d[i] > d2->d[i] )
      break;
  if ( i < nvar )
    return 0;
  else
    return 1;
}

int _dl_redble_ext(DL d1,DL d2,DL quo,int nvar)
{
  int i;

  if ( (quo->td = d2->td-d1->td) < 0 )
    return 0;
  for ( i = 0; i < nvar; i++ )
    if ( (quo->d[i] = d2->d[i]-d1->d[i]) < 0 )
      break;
  if ( i < nvar )
    return 0;
  else
    return 1;
}

void insert_to_node(DL d,NODE *n,int nvar)
{
  DL d1;
  MP m;
  DP dp;
  NODE n0,n1,n2;

  NEWDL(d1,nvar); d1->td = d->td;
  bcopy((char *)d->d,(char *)d1->d,nvar*sizeof(int));
  NEWMP(m); m->dl = d1; m->c = (Obj)ONE; NEXT(m) = 0;
  MKDP(nvar,m,dp); dp->sugar = d->td;
  if ( !(*n) ) {
    MKNODE(n1,dp,0); *n = n1;
  } else {
    for ( n1 = *n, n0 = 0; n1; n0 = n1, n1 = NEXT(n1) )
      if ( (*cmpdl)(nvar,d,BDY((DP)BDY(n1))->dl) > 0 ) {
        MKNODE(n2,dp,n1);
        if ( !n0 )
          *n = n2;
        else
          NEXT(n0) = n2;
        break;
      }
    if ( !n1 ) {
      MKNODE(n2,dp,0); NEXT(n0) = n2;
    }
  }
}

void dp_vtod(Q *c,DP p,DP *rp)
{
  MP mr0,m,mr;
  int i;

  if ( !p )
    *rp = 0;
  else {
    for ( mr0 = 0, m = BDY(p), i = 0; m; m = NEXT(m), i++ ) {
      NEXTMP(mr0,mr); mr->c = (Obj)c[i]; mr->dl = m->dl;
    }
    NEXT(mr) = 0; MKDP(p->nv,mr0,*rp);
    (*rp)->sugar = p->sugar;
  }
}

int have_sf_coef(P p)
{
  DCP dc;

  if ( !p )
    return 0;
  else if ( NUM(p) )
    return NID((Num)p) == N_GFS ? 1 : 0;
  else {
    for ( dc = DC(p); dc; dc = NEXT(dc) )
      if ( have_sf_coef(COEF(dc)) )
        return 1;
    return 0;
  }
}

void head_coef(P p,Num *c)
{
  if ( !p )
    *c = 0;
  else if ( NUM(p) )
    *c = (Num)p;
  else
    head_coef(COEF(DC(p)),c);
}

void dp_monic_sf(DP p,DP *rp)
{
  Num c;

  if ( !p )
    *rp = 0;
  else {
    head_coef((P)BDY(p)->c,&c);
    divsdc(CO,p,(P)c,rp);
  }
}

void divsdpmc(VL vl,DPM p,P c,DPM *pr);

void dpm_monic_sf(DPM p,DPM *rp)
{
  Num c;

  if ( !p )
    *rp = 0;
  else {
    head_coef((P)BDY(p)->c,&c);
    divsdpmc(CO,p,(P)c,rp);
  }
}

void dp_prim(DP p,DP *rp)
{
  P t,g;
  DP p1;
  MP m,mr,mr0;
  int i,n;
  P *w;
  Q *c;
  Z dvr;
  NODE tn;

  if ( !p )
    *rp = 0;
  else if ( dp_fcoeffs == N_GFS ) {
    for ( m = BDY(p); m; m = NEXT(m) )
      if ( OID(m->c) == O_N ) {
        /* GCD of coeffs = 1 */
        dp_monic_sf(p,rp);
        return;
      } else break;
    /* compute GCD over the finite fieid */
    for ( m = BDY(p), n = 0; m; m = NEXT(m), n++ );
    w = (P *)ALLOCA(n*sizeof(P));
    for ( m = BDY(p), i = 0; i < n; m = NEXT(m), i++ )
      w[i] = (P)m->c;
    gcdsf(CO,w,n,&g);
    if ( NUM(g) )
      dp_monic_sf(p,rp);
    else {
      for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
        NEXTMP(mr0,mr); divsp(CO,(P)m->c,g,(P *)&mr->c); mr->dl = m->dl;
      }
      NEXT(mr) = 0; MKDP(p->nv,mr0,p1); p1->sugar = p->sugar;
      dp_monic_sf(p1,rp);
    }
    return;
  } else if ( dp_fcoeffs )
    *rp = p;
  else if ( NoGCD )
    dp_ptozp(p,rp);
  else {
    dp_ptozp(p,&p1); p = p1;
    for ( m = BDY(p), n = 0; m; m = NEXT(m), n++ );
    if ( n == 1 ) {
      m = BDY(p);
      NEWMP(mr); mr->dl = m->dl; mr->c = (Obj)ONE; NEXT(mr) = 0;
      MKDP(p->nv,mr,*rp); (*rp)->sugar = p->sugar;
      return;
    }
    w = (P *)ALLOCA(n*sizeof(P));
    c = (Q *)ALLOCA(n*sizeof(Q));
    for ( m =BDY(p), i = 0; i < n; m = NEXT(m), i++ )
      if ( NUM(m->c) ) {
        c[i] = (Q)m->c; w[i] = (P)ONE;
      } else
        ptozp((P)m->c,1,&c[i],&w[i]);
    qltozl(c,n,&dvr); heu_nezgcdnpz(CO,w,n,&t); mulp(CO,t,(P)dvr,&g);
    if ( NUM(g) )
      *rp = p;
    else {
      for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
        NEXTMP(mr0,mr); divsp(CO,(P)m->c,g,(P *)&mr->c); mr->dl = m->dl;
      }
      NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
      add_denomlist(g);
    }
  }
}

void heu_nezgcdnpz(VL vl,P *pl,int m,P *pr)
{
  int i,r;
  P gcd,t,s1,s2,u;
  Z rq;
  DCP dc;
  extern int DP_Print;

  while ( 1 ) {
    for ( i = 0, s1 = 0; i < m; i++ ) {
      r = random(); UTOZ(r,rq);
      mulp(vl,pl[i],(P)rq,&t); addp(vl,s1,t,&u); s1 = u;
    }
    for ( i = 0, s2 = 0; i < m; i++ ) {
      r = random(); UTOZ(r,rq);
      mulp(vl,pl[i],(P)rq,&t); addp(vl,s2,t,&u); s2 = u;
    }
    ezgcdp(vl,s1,s2,&gcd);
    if ( DP_Print > 2 ) 
      { fprintf(asir_out,"(%d)",nmonop(gcd)); fflush(asir_out); }
    for ( i = 0; i < m; i++ ) {
      if ( !divtpz(vl,pl[i],gcd,&t) )
        break;
    }
    if ( i == m )
      break;
  }
  *pr = gcd;
}

void dp_prim_mod(DP p,int mod,DP *rp)
{
  P t,g;
  MP m,mr,mr0;

  if ( !p )
    *rp = 0;
  else if ( NoGCD )
    *rp = p;
  else {
    for ( m = BDY(p), g = (P)m->c, m = NEXT(m); m; m = NEXT(m) ) {
      gcdprsmp(CO,mod,g,(P)m->c,&t); g = t;
    }
    for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
      NEXTMP(mr0,mr); divsmp(CO,mod,(P)m->c,g,(P *)&mr->c); mr->dl = m->dl;
    }
    NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
  }
}

void dp_cont(DP p,Z *rp)
{
  VECT v;

  dp_dtov(p,&v); gcdvz(v,rp);
}

void dp_dtov(DP dp,VECT *rp)
{
  MP m,t;
  int i,n;
  VECT v;
  pointer *p;

  m = BDY(dp);
  for ( t = m, n = 0; t; t = NEXT(t), n++ );
  MKVECT(v,n);
  for ( i = 0, p = BDY(v), t = m; i < n; t = NEXT(t), i++ )
    p[i] = (pointer)(t->c);
  *rp = v;
}

/*
 * s-poly computation
 *
 */

void dp_sp(DP p1,DP p2,DP *rp)
{
  int i,n,td;
  int *w;
  DL d1,d2,d;
  MP m;
  DP t,s1,s2,u;
  Z c,c1,c2;
  Z gn;

  n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
  w = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, td = 0; i < n; i++ ) {
    w[i] = MAX(d1->d[i],d2->d[i]); td += MUL_WEIGHT(w[i],i);
  }

  NEWDL(d,n); d->td = td - d1->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = w[i] - d1->d[i];
  c1 = (Z)BDY(p1)->c; c2 = (Z)BDY(p2)->c;
  if ( INT(c1) && INT(c2) ) {
    gcdz(c1,c2,&gn);
    if ( !UNIQ(gn) ) {
      divsz(c1,gn,&c); c1 = c;
      divsz(c2,gn,&c);c2 = c;
    }
  }

  NEWMP(m); m->dl = d; m->c = (Obj)c2; NEXT(m) = 0;
  MKDP(n,m,s1); s1->sugar = d->td; muld(CO,s1,p1,&t);

  NEWDL(d,n); d->td = td - d2->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = w[i] - d2->d[i];
  NEWMP(m); m->dl = d; m->c = (Obj)c1; NEXT(m) = 0;
  MKDP(n,m,s2); s2->sugar = d->td; muld(CO,s2,p2,&u);

  subd(CO,t,u,rp);
  if ( GenTrace ) {
    LIST hist;
    NODE node;

    node = mknode(4,ONE,NULLP,s1,ONE);
    MKLIST(hist,node);
    MKNODE(TraceList,hist,0);

    node = mknode(4,ONE,NULLP,NULLP,ONE);
    chsgnd(s2,(DP *)&ARG2(node));
    MKLIST(hist,node);
    MKNODE(node,hist,TraceList); TraceList = node;
  }
}

void dpm_sp(DPM p1,DPM p2,DPM *rp,DP *mul1,DP *mul2)
{
  int i,n,td;
  int *w;
  DL d1,d2,d;
  MP m;
  DP s1,s2;
  DPM t,u;
  Z c,c1,c2;
  Z gn;

  n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
  if ( BDY(p1)->pos != BDY(p2)->pos ) {
    *mul1 = 0; *mul2 = 0; *rp = 0;
    return;
  }
  w = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, td = 0; i < n; i++ ) {
    w[i] = MAX(d1->d[i],d2->d[i]); td += MUL_WEIGHT(w[i],i);
  }

  NEWDL(d,n); d->td = td - d1->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = w[i] - d1->d[i];
  c1 = (Z)BDY(p1)->c; c2 = (Z)BDY(p2)->c;
  if ( INT(c1) && INT(c2) ) {
    gcdz(c1,c2,&gn);
    if ( !UNIQ(gn) ) {
      divsz(c1,gn,&c); c1 = c;
      divsz(c2,gn,&c);c2 = c;
    }
  }

  NEWMP(m); m->dl = d; m->c = (Obj)c2; NEXT(m) = 0;
  MKDP(n,m,s1); s1->sugar = d->td; mulobjdpm(CO,(Obj)s1,p1,&t);
  *mul1 = s1;

  NEWDL(d,n); d->td = td - d2->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = w[i] - d2->d[i];
  NEWMP(m); m->dl = d; m->c = (Obj)c1; NEXT(m) = 0;
  MKDP(n,m,s2); s2->sugar = d->td; mulobjdpm(CO,(Obj)s2,p2,&u);
  *mul2 = s2;

  subdpm(CO,t,u,rp);
  if ( GenTrace ) {
    LIST hist;
    NODE node;

    node = mknode(4,ONE,NULLP,s1,ONE);
    MKLIST(hist,node);
    MKNODE(TraceList,hist,0);

    node = mknode(4,ONE,NULLP,NULLP,ONE);
    chsgnd(s2,(DP *)&ARG2(node));
    MKLIST(hist,node);
    MKNODE(node,hist,TraceList); TraceList = node;
  }
}

DP dpm_sp_hm(DPM p1,DPM p2)
{
  int i,n,td;
  int *w;
  DL d1,d2,d;
  MP m;
  DP s1;

  n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
  if ( BDY(p1)->pos != BDY(p2)->pos ) {
    return 0;
  }
  w = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, td = 0; i < n; i++ ) {
    w[i] = MAX(d1->d[i],d2->d[i]); td += MUL_WEIGHT(w[i],i);
  }

  NEWDL(d,n); d->td = td - d1->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = w[i] - d1->d[i];

  NEWMP(m); m->dl = d; m->c = (Obj)ONE; NEXT(m) = 0;
  MKDP(n,m,s1); s1->sugar = d->td;
  return s1;
}

void _dp_sp_dup(DP p1,DP p2,DP *rp)
{
  int i,n,td;
  int *w;
  DL d1,d2,d;
  MP m;
  DP t,s1,s2,u;
  Z c,c1,c2;
  Z gn;

  n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
  w = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, td = 0; i < n; i++ ) {
    w[i] = MAX(d1->d[i],d2->d[i]); td += MUL_WEIGHT(w[i],i);
  }

  _NEWDL(d,n); d->td = td - d1->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = w[i] - d1->d[i];
  c1 = (Z)BDY(p1)->c; c2 = (Z)BDY(p2)->c;
  if ( INT(c1) && INT(c2) ) {
    gcdz(c1,c2,&gn);
    if ( !UNIQ(gn) ) {
      divsz(c1,gn,&c); c1 = c;
      divsz(c2,gn,&c);c2 = c;
    }
  }

  _NEWMP(m); m->dl = d; m->c = (Obj)c2; NEXT(m) = 0;
  _MKDP(n,m,s1); s1->sugar = d->td; _muld_dup(CO,s1,p1,&t); _free_dp(s1);

  _NEWDL(d,n); d->td = td - d2->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = w[i] - d2->d[i];
  _NEWMP(m); m->dl = d; chsgnp((P)c1,(P *)&m->c); NEXT(m) = 0;
  _MKDP(n,m,s2); s2->sugar = d->td; _muld_dup(CO,s2,p2,&u); _free_dp(s2);

  _addd_destructive(CO,t,u,rp);
  if ( GenTrace ) {
    LIST hist;
    NODE node;

    node = mknode(4,ONE,NULLP,s1,ONE);
    MKLIST(hist,node);
    MKNODE(TraceList,hist,0);

    node = mknode(4,ONE,NULLP,NULLP,ONE);
    chsgnd(s2,(DP *)&ARG2(node));
    MKLIST(hist,node);
    MKNODE(node,hist,TraceList); TraceList = node;
  }
}

void dp_sp_mod(DP p1,DP p2,int mod,DP *rp)
{
  int i,n,td;
  int *w;
  DL d1,d2,d;
  MP m;
  DP t,s,u;

  n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
  w = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, td = 0; i < n; i++ ) {
    w[i] = MAX(d1->d[i],d2->d[i]); td += MUL_WEIGHT(w[i],i);
  }
  NEWDL_NOINIT(d,n); d->td = td - d1->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = w[i] - d1->d[i];
  NEWMP(m); m->dl = d; m->c = (Obj)BDY(p2)->c; NEXT(m) = 0;
  MKDP(n,m,s); s->sugar = d->td; mulmd(CO,mod,p1,s,&t);
  NEWDL_NOINIT(d,n); d->td = td - d2->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = w[i] - d2->d[i];
  NEWMP(m); m->dl = d; m->c = (Obj)BDY(p1)->c; NEXT(m) = 0;
  MKDP(n,m,s); s->sugar = d->td; mulmd(CO,mod,p2,s,&u);
  submd(CO,mod,t,u,rp);
}

void _dp_sp_mod_dup(DP p1,DP p2,int mod,DP *rp)
{
  int i,n,td;
  int *w;
  DL d1,d2,d;
  MP m;
  DP t,s,u;

  n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
  w = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, td = 0; i < n; i++ ) {
    w[i] = MAX(d1->d[i],d2->d[i]); td += MUL_WEIGHT(w[i],i);
  }
  _NEWDL(d,n); d->td = td - d1->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = w[i] - d1->d[i];
  _NEWMP(m); m->dl = d; m->c = BDY(p2)->c; NEXT(m) = 0;
  _MKDP(n,m,s); s->sugar = d->td; _mulmd_dup(mod,s,p1,&t); _free_dp(s);
  _NEWDL(d,n); d->td = td - d2->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = w[i] - d2->d[i];
  _NEWMP(m); m->dl = d; m->c = (Obj)STOI(mod - ITOS(BDY(p1)->c)); NEXT(m) = 0;
  _MKDP(n,m,s); s->sugar = d->td; _mulmd_dup(mod,s,p2,&u); _free_dp(s);
  _addmd_destructive(mod,t,u,rp);
}

void _dp_sp_mod(DP p1,DP p2,int mod,DP *rp)
{
  int i,n,td;
  int *w;
  DL d1,d2,d;
  MP m;
  DP t,s,u;

  n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
  w = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, td = 0; i < n; i++ ) {
    w[i] = MAX(d1->d[i],d2->d[i]); td += MUL_WEIGHT(w[i],i);
  }
  NEWDL(d,n); d->td = td - d1->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = w[i] - d1->d[i];
  NEWMP(m); m->dl = d; m->c = BDY(p2)->c; NEXT(m) = 0;
  MKDP(n,m,s); s->sugar = d->td; mulmd_dup(mod,s,p1,&t);
  NEWDL(d,n); d->td = td - d2->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = w[i] - d2->d[i];
  NEWMP(m); m->dl = d; m->c = (Obj)STOI(mod - ITOS(BDY(p1)->c)); NEXT(m) = 0;
  MKDP(n,m,s); s->sugar = d->td; mulmd_dup(mod,s,p2,&u);
  addmd_destructive(mod,t,u,rp);
}

/*
 * m-reduction
 * do content reduction over Z or Q(x,...)
 * do nothing over finite fields
 *
 * head+rest = dn*(p0+p1)+mult*p2
 */

void dp_red(DP p0,DP p1,DP p2,DP *head,DP *rest,P *dnp,DP *multp)
{
  int i,n;
  DL d1,d2,d;
  MP m;
  DP t,s,r,h;
  Z c,c1,c2,gn;
  P g,a;
  P p[2];

  n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
  NEWDL(d,n); d->td = d1->td - d2->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = d1->d[i]-d2->d[i];
  c1 = (Z)BDY(p1)->c; c2 = (Z)BDY(p2)->c;
  if ( dp_fcoeffs == N_GFS ) {
    p[0] = (P)c1; p[1] = (P)c2;
    gcdsf(CO,p,2,&g);
    divsp(CO,(P)c1,g,&a); c1 = (Z)a; divsp(CO,(P)c2,g,&a); c2 = (Z)a;
  } else if ( dp_fcoeffs ) {
    /* do nothing */
  } else if ( INT(c1) && INT(c2) ) {
    gcdz(c1,c2,&gn);
    if ( !UNIQ(gn) ) {
      divsz(c1,gn,&c); c1 = c;
      divsz(c2,gn,&c); c2 = c;
    }
  } else {
    ezgcdp(CO,(P)c1,(P)c2,&g);
    divsp(CO,(P)c1,g,&a); c1 = (Z)a; divsp(CO,(P)c2,g,&a); c2 = (Z)a;
    add_denomlist(g);
  }
  NEWMP(m); m->dl = d; chsgnp((P)c1,(P *)&m->c); NEXT(m) = 0; MKDP(n,m,s); s->sugar = d->td;
  *multp = s;
  muld(CO,s,p2,&t); muldc(CO,p1,(Obj)c2,&s); addd(CO,s,t,&r);
  muldc(CO,p0,(Obj)c2,&h);
  *head = h; *rest = r; *dnp = (P)c2;
}

// head+rest = dn*(p0+p1)-mult*p2
void dpm_red(DPM p0,DPM p1,DPM p2,DPM *head,DPM *rest,P *dnp,DP *multp)
{
  int i,n,pos;
  DL d1,d2,d;
  MP m;
  DP s,ms;
  DPM t,r,h,u,w;
  Z c,c1,c2,gn;
  P g,a;
  P p[2];

  n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl; pos = BDY(p1)->pos;
  if ( pos != BDY(p2)->pos )
    error("dpm_red : cannot happen");
  NEWDL(d,n); d->td = d1->td - d2->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = d1->d[i]-d2->d[i];
  c1 = (Z)BDY(p1)->c; c2 = (Z)BDY(p2)->c;
  if ( dp_fcoeffs == N_GFS ) {
    p[0] = (P)c1; p[1] = (P)c2;
    gcdsf(CO,p,2,&g);
    divsp(CO,(P)c1,g,&a); c1 = (Z)a; divsp(CO,(P)c2,g,&a); c2 = (Z)a;
  } else if ( dp_fcoeffs ) {
    /* do nothing */
  } else if ( INT(c1) && INT(c2) ) {
    gcdz(c1,c2,&gn);
    if ( !UNIQ(gn) ) {
      divsz(c1,gn,&c); c1 = c;
      divsz(c2,gn,&c); c2 = c;
    }
  } else {
    ezgcdp(CO,(P)c1,(P)c2,&g);
    divsp(CO,(P)c1,g,&a); c1 = (Z)a; divsp(CO,(P)c2,g,&a); c2 = (Z)a;
    add_denomlist(g);
  }
  NEWMP(m); m->dl = d; m->c = (Obj)c1; NEXT(m) = 0; MKDP(n,m,s); s->sugar = d->td;
  *multp = s;
  chsgnd(s,&ms); mulobjdpm(CO,(Obj)ms,p2,&u); mulobjdpm(CO,(Obj)c2,p1,&w); adddpm(CO,w,u,&r);
  mulobjdpm(CO,(Obj)c2,p0,&h);
  *head = h; *rest = r; *dnp = (P)c2;
}

void dpm_red2(DPM p1,DPM p2,DPM *rest,P *dnp,DP *multp)
{
  int i,n,pos;
  DL d1,d2,d;
  MP m;
  DP s,ms;
  DPM t,r,h,u,w;
  Z c,c1,c2,gn;
  P g,a;
  P p[2];

  n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl; pos = BDY(p1)->pos;
  if ( pos != BDY(p2)->pos )
    error("dpm_red : cannot happen");
  NEWDL(d,n); d->td = d1->td - d2->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = d1->d[i]-d2->d[i];
  c1 = (Z)BDY(p1)->c; c2 = (Z)BDY(p2)->c;
  if ( dp_fcoeffs == N_GFS ) {
    p[0] = (P)c1; p[1] = (P)c2;
    gcdsf(CO,p,2,&g);
    divsp(CO,(P)c1,g,&a); c1 = (Z)a; divsp(CO,(P)c2,g,&a); c2 = (Z)a;
  } else if ( dp_fcoeffs ) {
    /* do nothing */
  } else if ( INT(c1) && INT(c2) ) {
    gcdz(c1,c2,&gn);
    if ( !UNIQ(gn) ) {
      divsz(c1,gn,&c); c1 = c;
      divsz(c2,gn,&c); c2 = c;
    }
  } else {
    ezgcdp(CO,(P)c1,(P)c2,&g);
    divsp(CO,(P)c1,g,&a); c1 = (Z)a; divsp(CO,(P)c2,g,&a); c2 = (Z)a;
    add_denomlist(g);
  }
  NEWMP(m); m->dl = d; m->c = (Obj)c1; NEXT(m) = 0; MKDP(n,m,s); s->sugar = d->td;
  *multp = s;
  chsgnd(s,&ms); mulobjdpm(CO,(Obj)ms,p2,&u); mulobjdpm(CO,(Obj)c2,p1,&w); adddpm(CO,w,u,&r);
  *rest = r; *dnp = (P)c2;
}

/*
 * m-reduction by a marked poly
 * do content reduction over Z or Q(x,...)
 * do nothing over finite fields
 *
 */


void dp_red_marked(DP p0,DP p1,DP p2,DP hp2,DP *head,DP *rest,P *dnp,DP *multp)
{
  int i,n;
  DL d1,d2,d;
  MP m;
  DP t,s,r,h;
  Z c,c1,c2,gn;
  P g,a;
  P p[2];

  n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(hp2)->dl;
  NEWDL(d,n); d->td = d1->td - d2->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = d1->d[i]-d2->d[i];
  c1 = (Z)BDY(p1)->c; c2 = (Z)BDY(hp2)->c;
  if ( dp_fcoeffs == N_GFS ) {
    p[0] = (P)c1; p[1] = (P)c2;
    gcdsf(CO,p,2,&g);
    divsp(CO,(P)c1,g,&a); c1 = (Z)a; divsp(CO,(P)c2,g,&a); c2 = (Z)a;
  } else if ( dp_fcoeffs ) {
    /* do nothing */
  } else if ( INT(c1) && INT(c2) ) {
    gcdz(c1,c2,&gn);
    if ( !UNIQ(gn) ) {
      divsz(c1,gn,&c); c1 = c;
      divsz(c2,gn,&c); c2 = c;
    }
  } else {
    ezgcdp(CO,(P)c1,(P)c2,&g);
    divsp(CO,(P)c1,g,&a); c1 = (Z)a; divsp(CO,(P)c2,g,&a); c2 = (Z)a;
  }
  NEWMP(m); m->dl = d; m->c = (Obj)c1; NEXT(m) = 0; MKDP(n,m,s); s->sugar = d->td;
  *multp = s;
  muld(CO,s,p2,&t); muldc(CO,p1,(Obj)c2,&s); subd(CO,s,t,&r);
  muldc(CO,p0,(Obj)c2,&h);
  *head = h; *rest = r; *dnp = (P)c2;
}

// eliminate the monomial in p1 at pos by a marked poly 
void dp_red_marked_pos(DP p1,MP pos,DP p2,DP hp2,DP *rest,P *dnp)
{
  int i,n;
  DL d1,d2,d;
  MP m;
  DP t,s,r,h;
  Z c,c1,c2,gn;
  P g,a;
  P p[2];

  n = p1->nv; d1 = pos->dl; d2 = BDY(hp2)->dl;
  NEWDL(d,n); d->td = d1->td - d2->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = d1->d[i]-d2->d[i];
  c1 = (Z)pos->c; c2 = (Z)BDY(hp2)->c;
  if ( dp_fcoeffs == N_GFS ) {
    p[0] = (P)c1; p[1] = (P)c2;
    gcdsf(CO,p,2,&g);
    divsp(CO,(P)c1,g,&a); c1 = (Z)a; divsp(CO,(P)c2,g,&a); c2 = (Z)a;
  } else if ( dp_fcoeffs ) {
    /* do nothing */
  } else if ( INT(c1) && INT(c2) ) {
    gcdz(c1,c2,&gn);
    if ( !UNIQ(gn) ) {
      divsz(c1,gn,&c); c1 = c;
      divsz(c2,gn,&c); c2 = c;
    }
  } else {
    ezgcdp(CO,(P)c1,(P)c2,&g);
    divsp(CO,(P)c1,g,&a); c1 = (Z)a; divsp(CO,(P)c2,g,&a); c2 = (Z)a;
  }
  NEWMP(m); m->dl = d; m->c = (Obj)c1; NEXT(m) = 0; MKDP(n,m,s); s->sugar = d->td;
  muld(CO,s,p2,&t); muldc(CO,p1,(Obj)c2,&s); subd(CO,s,t,&r);
  *rest = r; *dnp = (P)c2;
}

// p0 *= const
void _dp_red_marked(DP *p0,DP p1,DP p2,DP hp2,DP *rest,P *dnp,DP *multp)
{
  int i,n;
  DL d1,d2,d;
  MP m;
  DP t,s,r,h;
  Z c,c1,c2,gn;
  P g,a;
  P p[2];

  n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(hp2)->dl;
  NEWDL(d,n); d->td = d1->td - d2->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = d1->d[i]-d2->d[i];
  c1 = (Z)BDY(p1)->c; c2 = (Z)BDY(hp2)->c;
  if ( dp_fcoeffs == N_GFS ) {
    p[0] = (P)c1; p[1] = (P)c2;
    gcdsf(CO,p,2,&g);
    divsp(CO,(P)c1,g,&a); c1 = (Z)a; divsp(CO,(P)c2,g,&a); c2 = (Z)a;
  } else if ( dp_fcoeffs ) {
    /* do nothing */
  } else if ( INT(c1) && INT(c2) ) {
    gcdz(c1,c2,&gn);
    if ( !UNIQ(gn) ) {
      divsz(c1,gn,&c); c1 = c;
      divsz(c2,gn,&c); c2 = c;
    }
  } else {
    ezgcdp(CO,(P)c1,(P)c2,&g);
    divsp(CO,(P)c1,g,&a); c1 = (Z)a; divsp(CO,(P)c2,g,&a); c2 = (Z)a;
  }
  NEWMP(m); m->dl = d; m->c = (Obj)c1; NEXT(m) = 0; MKDP(n,m,s); s->sugar = d->td;
  *multp = s;
  muld(CO,s,p2,&t); muldc(CO,p1,(Obj)c2,&s); subd(CO,s,t,&r);
  _muldc(CO,p0,(Obj)c2);
  *rest = r; *dnp = (P)c2;
}

void dp_red_marked_mod(DP p0,DP p1,DP p2,DP hp2,int mod,DP *head,DP *rest,P *dnp,DP *multp)
{
  int i,n;
  DL d1,d2,d;
  MP m;
  DP t,s,r,h;
  P c1,c2,g,u;

  n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(hp2)->dl;
  NEWDL(d,n); d->td = d1->td - d2->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = d1->d[i]-d2->d[i];
  c1 = (P)BDY(p1)->c; c2 = (P)BDY(hp2)->c;
  gcdprsmp(CO,mod,c1,c2,&g);
  divsmp(CO,mod,c1,g,&u); c1 = u; divsmp(CO,mod,c2,g,&u); c2 = u;
  if ( NUM(c2) ) {
    divsmp(CO,mod,c1,c2,&u); c1 = u; c2 = (P)ONEM;
  }
  NEWMP(m); m->dl = d; m->c = (Obj)c1; NEXT(m) = 0;
  MKDP(n,m,s); s->sugar = d->td;
  *multp = s;
  mulmd(CO,mod,s,p2,&t);
  if ( NUM(c2) ) {
    submd(CO,mod,p1,t,&r); h = p0;
  } else {
    mulmdc(CO,mod,p1,c2,&s); submd(CO,mod,s,t,&r); mulmdc(CO,mod,p0,c2,&h);
  }
  *head = h; *rest = r; *dnp = c2;
}

/* m-reduction over a field */

void dp_red_f(DP p1,DP p2,DP *rest)
{
  int i,n;
  DL d1,d2,d;
  MP m;
  DP t,s;
  Obj a,b;

  n = p1->nv;
  d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;

  NEWDL(d,n); d->td = d1->td - d2->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = d1->d[i]-d2->d[i];

  NEWMP(m); m->dl = d;
  divr(CO,(Obj)BDY(p1)->c,(Obj)BDY(p2)->c,&a); chsgnr(a,&b);
  C(m) = (Obj)b;
  NEXT(m) = 0; MKDP(n,m,s); s->sugar = d->td;

  muld(CO,s,p2,&t); addd(CO,p1,t,rest);
}

void dpm_red_f(DPM p1,DPM p2,DPM *rest)
{
  int i,n;
  DL d1,d2,d;
  MP m;
  DPM t;
  DP s;
  Obj a,b;

  n = p1->nv;
  d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;

  NEWDL(d,n); d->td = d1->td - d2->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = d1->d[i]-d2->d[i];

  NEWMP(m); m->dl = d;
  arf_div(CO,(Obj)BDY(p1)->c,(Obj)BDY(p2)->c,&a); arf_chsgn(a,&b);
  C(m) = b;
  NEXT(m) = 0; MKDP(n,m,s); s->sugar = d->td;

  mulobjdpm(CO,(Obj)s,p2,&t); adddpm(CO,p1,t,rest);
}


void dp_red_mod(DP p0,DP p1,DP p2,int mod,DP *head,DP *rest,P *dnp)
{
  int i,n;
  DL d1,d2,d;
  MP m;
  DP t,s,r,h;
  P c1,c2,g,u;

  n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
  NEWDL(d,n); d->td = d1->td - d2->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = d1->d[i]-d2->d[i];
  c1 = (P)BDY(p1)->c; c2 = (P)BDY(p2)->c;
  gcdprsmp(CO,mod,c1,c2,&g);
  divsmp(CO,mod,c1,g,&u); c1 = u; divsmp(CO,mod,c2,g,&u); c2 = u;
  if ( NUM(c2) ) {
    divsmp(CO,mod,c1,c2,&u); c1 = u; c2 = (P)ONEM;
  }
  NEWMP(m); m->dl = d; chsgnmp(mod,(P)c1,(P *)&m->c); NEXT(m) = 0;
  MKDP(n,m,s); s->sugar = d->td; mulmd(CO,mod,s,p2,&t);
  if ( NUM(c2) ) {
    addmd(CO,mod,p1,t,&r); h = p0;
  } else {
    mulmdc(CO,mod,p1,c2,&s); addmd(CO,mod,s,t,&r); mulmdc(CO,mod,p0,c2,&h);
  }
  *head = h; *rest = r; *dnp = c2;
}

struct oEGT eg_red_mod;

void _dp_red_mod_destructive(DP p1,DP p2,int mod,DP *rp)
{
  int i,n;
  DL d1,d2,d;
  MP m;
  DP t,s;
  int c,c1,c2;
  extern int do_weyl;

  n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
  _NEWDL(d,n); d->td = d1->td - d2->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = d1->d[i]-d2->d[i];
  c = invm(ITOS(BDY(p2)->c),mod); 
  c2 = ITOS(BDY(p1)->c);
  DMAR(c,c2,0,mod,c1);
  _NEWMP(m); m->dl = d; m->c = (Obj)STOI(mod-c1); NEXT(m) = 0;
#if 0
  _MKDP(n,m,s); s->sugar = d->td;
  _mulmd_dup(mod,s,p2,&t); _free_dp(s);
#else
  if ( do_weyl ) {
    _MKDP(n,m,s); s->sugar = d->td;
    _mulmd_dup(mod,s,p2,&t); _free_dp(s);
  } else {
    _mulmdm_dup(mod,p2,m,&t); _FREEMP(m);
  }
#endif
/* get_eg(&t0); */
  _addmd_destructive(mod,p1,t,rp);
/* get_eg(&t1); add_eg(&eg_red_mod,&t0,&t1); */
}

/*
 * normal form computation
 *
 */

void dp_true_nf(NODE b,DP g,DP *ps,int full,DP *rp,P *dnp)
{
  DP u,p,d,s,t,dmy;
  NODE l;
  MP m,mr;
  int i,n;
  int *wb;
  int sugar,psugar;
  P dn,tdn,tdn1;

  dn = (P)ONE;
  if ( !g ) {
    *rp = 0; *dnp = dn; return;
  }
  for ( n = 0, l = b; l; l = NEXT(l), n++ );
  wb = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, l = b; i < n; l = NEXT(l), i++ )
    wb[i] = ZTOS((Q)BDY(l));
  sugar = g->sugar;
  for ( d = 0; g; ) {
    for ( u = 0, i = 0; i < n; i++ ) {
      if ( dp_redble(g,p = ps[wb[i]]) ) {
        dp_red(d,g,p,&t,&u,&tdn,&dmy);
        psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
        sugar = MAX(sugar,psugar);
        if ( !u ) {
          if ( d )
            d->sugar = sugar;
          *rp = d; *dnp = dn; return;
        } else {
          d = t;
          mulp(CO,dn,tdn,&tdn1); dn = tdn1;
        }
        break;
      }
    }
    if ( u )
      g = u;
    else if ( !full ) {
      if ( g ) {
        MKDP(g->nv,BDY(g),t); t->sugar = sugar; g = t;
      }
      *rp = g; *dnp = dn; return;
    } else {
      m = BDY(g); NEWMP(mr); mr->dl = m->dl; mr->c = m->c;
      NEXT(mr) = 0; MKDP(g->nv,mr,t); t->sugar = mr->dl->td;
      addd(CO,d,t,&s); d = s;
      dp_rest(g,&t); g = t;
    }
  }
  if ( d )
    d->sugar = sugar;
  *rp = d; *dnp = dn;
}

// normal form by a linear base

void dp_true_lnf(DP f,NODE g,DP *rp,P *dnp)
{
  P dn,tdn,tdn1;
  int sugar,nv;
  DP d,u,gi,dt,w,s,dmy;
  MP m,mr;
  NODE t;

  dn = (P)ONE;
  if ( !f ) {
    *rp = 0; *dnp = dn; return;
  }
  sugar = f->sugar;
  nv = f->nv;
  for ( d = 0; f; ) {
    for ( u = 0, t = g; t; t = NEXT(t) ) {
      gi = (DP)BDY(t);
      if ( dl_equal(nv,BDY(f)->dl,BDY(gi)->dl) ) {
        dp_red(d,f,gi,&dt,&u,&tdn,&dmy);
        sugar = MAX(sugar,gi->sugar);
        if ( !u ) {
          if ( d )
            d->sugar = sugar;
          *rp = d; *dnp = dn; return;
        } else {
          d = dt;
          mulp(CO,dn,tdn,&tdn1); dn = tdn1;
        }
        break;
      }
    }
    if ( u )
      f = u;
    else {
      m = BDY(f); NEWMP(mr); mr->dl = m->dl; mr->c = m->c;
      NEXT(mr) = 0; MKDP(nv,mr,w); w->sugar = mr->dl->td;
      addd(CO,d,w,&s); d = s;
      dp_rest(f,&w); f = w;
    }
  }
  if ( d )
    d->sugar = sugar;
  *rp = d; *dnp = dn;
}

// normal form by a linear base using the sorted g
void dp_true_lnf_marked(DP f,NODE g,NODE h,DP *rp,P *dnp)
{
  P dn,tdn,tdn1;
  int sugar,nv;
  DP r,u,gi,dt,w,s,dmy;
  DL dl;
  MP m,mr;
  NODE t,th;

  dn = (P)ONE;
  if ( !f ) {
    *rp = 0; *dnp = dn; return;
  }
  sugar = f->sugar;
  nv = f->nv;
  r = f;
  for ( t = g, th = h; t; t = NEXT(t), th = NEXT(th) ) {
    if ( !r ) break;
    dl = BDY((DP)BDY(th))->dl;
    for ( u = 0, m = BDY(r); m; m = NEXT(m) )
      if ( dl_equal(nv,m->dl,dl) ) break;
    if ( m ) {
      gi = (DP)BDY(t);
      dp_red_marked_pos(r,m,gi,(DP)BDY(th),&u,&tdn);
      sugar = MAX(sugar,gi->sugar);
      if ( !u ) {
        *rp = 0; *dnp = (P)ONE; return;
      } else {
        mulp(CO,dn,tdn,&tdn1); dn = tdn1;
        r = u;
      }
    }
  }
  *rp = r;
  *dnp = dn;
}

void dp_removecont2(DP p1,DP p2,DP *r1p,DP *r2p,Z *contp)
{
  struct oVECT v;
  int i,n1,n2,n;
  MP m,m0,t;
  Z *w;
  Z h;

  if ( p1 ) {
    for ( i = 0, m = BDY(p1); m; m = NEXT(m), i++ );
    n1 = i;
  } else
    n1 = 0;
  if ( p2 ) {
    for ( i = 0, m = BDY(p2); m; m = NEXT(m), i++ );
    n2 = i;
  } else
    n2 = 0;
  n = n1+n2;
  if ( !n ) {
    *r1p = 0; *r2p = 0; *contp = ONE; return;
  }
  w = (Z *)ALLOCA(n*sizeof(Q));
  v.len = n;
  v.body = (pointer *)w;
  i = 0;
  if ( p1 )
    for ( m = BDY(p1); i < n1; m = NEXT(m), i++ ) w[i] = (Z)m->c;
  if ( p2 )
    for ( m = BDY(p2); i < n; m = NEXT(m), i++ ) w[i] = (Z)m->c;
  h = w[0]; removecont_array((P *)w,n,1); divsz(h,w[0],contp);
  i = 0;
  if ( p1 ) {
    for ( m0 = 0, t = BDY(p1); i < n1; i++, t = NEXT(t) ) {
      NEXTMP(m0,m); m->c = (Obj)w[i]; m->dl = t->dl;
    }
    NEXT(m) = 0;
    MKDP(p1->nv,m0,*r1p); (*r1p)->sugar = p1->sugar;
  } else
    *r1p = 0;
  if ( p2 ) {
    for ( m0 = 0, t = BDY(p2); i < n; i++, t = NEXT(t) ) {
      NEXTMP(m0,m); m->c = (Obj)w[i]; m->dl = t->dl;
    }
    NEXT(m) = 0;
    MKDP(p2->nv,m0,*r2p); (*r2p)->sugar = p2->sugar;
  } else
    *r2p = 0;
}

void dpm_removecont2(DPM p1,DPM p2,DPM *r1p,DPM *r2p,Z *contp)
{
  struct oVECT v;
  int i,n1,n2,n;
  DMM m,m0,t;
  Z *w;
  Z h;

  if ( p1 ) {
    for ( i = 0, m = BDY(p1); m; m = NEXT(m), i++ );
    n1 = i;
  } else
    n1 = 0;
  if ( p2 ) {
    for ( i = 0, m = BDY(p2); m; m = NEXT(m), i++ );
    n2 = i;
  } else
    n2 = 0;
  n = n1+n2;
  if ( !n ) {
    *r1p = 0; *r2p = 0; *contp = ONE; return;
  }
  w = (Z *)ALLOCA(n*sizeof(Q));
  v.len = n;
  v.body = (pointer *)w;
  i = 0;
  if ( p1 )
    for ( m = BDY(p1); i < n1; m = NEXT(m), i++ ) w[i] = (Z)m->c;
  if ( p2 )
    for ( m = BDY(p2); i < n; m = NEXT(m), i++ ) w[i] = (Z)m->c;
  h = w[0]; removecont_array((P *)w,n,1); divsz(h,w[0],contp);
  i = 0;
  if ( p1 ) {
    for ( m0 = 0, t = BDY(p1); i < n1; i++, t = NEXT(t) ) {
      NEXTDMM(m0,m); m->c = (Obj)w[i]; m->dl = t->dl; m->pos = t->pos;
    }
    NEXT(m) = 0;
    MKDPM(p1->nv,m0,*r1p); (*r1p)->sugar = p1->sugar;
  } else
    *r1p = 0;
  if ( p2 ) {
    for ( m0 = 0, t = BDY(p2); i < n; i++, t = NEXT(t) ) {
      NEXTDMM(m0,m); m->c = (Obj)w[i]; m->dl = t->dl; m->pos = t->pos;
    }
    NEXT(m) = 0;
    MKDPM(p2->nv,m0,*r2p); (*r2p)->sugar = p2->sugar;
  } else
    *r2p = 0;
}

/* true nf by a marked GB */

void dp_true_nf_marked(NODE b,DP g,DP *ps,DP *hps,DP *rp,P *nmp,P *dnp)
{
  DP u,p,d,s,t,dmy,hp;
  NODE l;
  MP m,mr;
  int i,n,hmag;
  int *wb;
  int sugar,psugar,multiple;
  P nm,tnm1,dn,tdn,tdn1;
  Z cont;

  multiple = 0;
  hmag = multiple*HMAG(g);
  nm = (P)ONE;
  dn = (P)ONE;
  if ( !g ) {
    *rp = 0; *dnp = dn; return;
  }
  for ( n = 0, l = b; l; l = NEXT(l), n++ );
  wb = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, l = b; i < n; l = NEXT(l), i++ )
    wb[i] = ZTOS((Z)BDY(l));
  sugar = g->sugar;
  for ( d = 0; g; ) {
    for ( u = 0, i = 0; i < n; i++ ) {
      if ( dp_redble(g,hp = hps[wb[i]]) ) {
        p = ps[wb[i]];
        dp_red_marked(d,g,p,hp,&t,&u,&tdn,&dmy);
        psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
        sugar = MAX(sugar,psugar);
        if ( !u ) {
          goto last;
        } else {
          d = t;
          mulp(CO,dn,tdn,&tdn1); dn = tdn1;
        }
        break;
      }
    }
    if ( u ) {
      g = u;
      if ( multiple && ((d && HMAG(d)>hmag) || (HMAG(g)>hmag)) ) {
        dp_removecont2(d,g,&t,&u,&cont); d = t; g = u;
        mulp(CO,nm,(P)cont,&tnm1); nm = tnm1;
        if ( d )
          hmag = multiple*HMAG(d);
        else
          hmag = multiple*HMAG(g);
      }
    } else {
      m = BDY(g); NEWMP(mr); mr->dl = m->dl; mr->c = m->c;
      NEXT(mr) = 0; MKDP(g->nv,mr,t); t->sugar = mr->dl->td;
      addd(CO,d,t,&s); d = s;
      dp_rest(g,&t); g = t;
    }
  }
last:
  if ( d ) {
    dp_removecont2(d,0,&t,&u,&cont); d = t;
    mulp(CO,nm,(P)cont,&tnm1); nm = tnm1;
    d->sugar = sugar;
  }
  *rp = d; *nmp = nm; *dnp = dn;
}

void dp_separate_normal(DP g,DP *hps,int *ind,int n,DP *d,DP *r)
{
  int nv,i;
  MP md0,md,mr0,mr,m;
  DP t;

  if ( g == 0 ) {
    *d = 0; *r = 0; return;
  }
  nv = g->nv;
  md0 = 0; mr0 = 0;
  for ( m = BDY(g); m; m = NEXT(m) ) {
    for ( i = 0; i < n; i++ )
      if ( _dl_redble(BDY(hps[ind[i]])->dl,m->dl,nv) ) break;
    if ( i == n ) {
      NEXTMP(md0,md); md->c = m->c; md->dl = m->dl;
    } else {
      NEXTMP(mr0,mr); mr->c = m->c; mr->dl = m->dl;
    }
  } 
  if ( md0 != 0 ) {
    md->next = 0; MKDP(nv,md0,t); t->sugar = g->sugar; *d = t;
  } else
     *d = 0;
  if ( mr0 != 0 ) {
    mr->next = 0; MKDP(nv,mr0,t); t->sugar = g->sugar; *r = t;
  } else
     *r = 0;
}

extern Obj VOIDobj;

void dp_true_nf_marked_check(NODE b,DP g,DP *ps,DP *hps,DP *rp,P *nmp,P *dnp)
{
  DP u,p,d,s,t,dmy,hp;
  NODE l,done;
  MP m,mr;
  int i,n,hmag;
  int *wb;
  int sugar,psugar,multiple;
  P nm,tnm1,dn,tdn,tdn1;
  Z cont;
  int count = 0;

  nm = (P)ONE;
  dn = (P)ONE;
  if ( !g ) {
    *rp = 0; *dnp = dn; return;
  }
  for ( n = 0, l = b; l; l = NEXT(l), n++ );
  wb = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, l = b; i < n; l = NEXT(l), i++ )
    wb[i] = ZTOS((Z)BDY(l));
  sugar = g->sugar;
  done = 0;
  dp_separate_normal(g,hps,wb,n,&d,&u); g = u;
  for ( ; g; ) {
    for ( u = 0, i = 0; i < n; i++ ) {
      if ( dp_redble(g,hp = hps[wb[i]]) ) {
        for ( l = done; l; l = NEXT(l) )
          if ( dl_equal(g->nv,BDY(g)->dl,(DL)BDY(l)) ) { count++; break; }
        if ( l != 0 && count > 100 ) {
          *rp = (DP)VOIDobj;
          *nmp = (P)ONE;
          *dnp = (P)ONE;
          return;
        }
        MKNODE(l,BDY(g)->dl,done); done = l;
        p = ps[wb[i]];
//        dp_red_marked(d,g,p,hp,&t,&u,&tdn,&dmy);
        _dp_red_marked(&d,g,p,hp,&u,&tdn,&dmy);
        psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
        sugar = MAX(sugar,psugar);
        if ( !u ) {
          goto last;
        } else {
//          d = t;
          mulp(CO,dn,tdn,&tdn1); dn = tdn1;
        }
        break;
      }
    }
    dp_separate_normal(u,hps,wb,n,&t,&g);
    addd(CO,d,t,&u); d = u;
  }
last:
  if ( d ) {
    dp_removecont2(d,0,&t,&u,&cont); d = t;
    mulp(CO,nm,(P)cont,&tnm1); nm = tnm1;
    d->sugar = sugar;
  }
  *rp = d; *nmp = nm; *dnp = dn;
}

void dp_true_nf_marked_mod(NODE b,DP g,DP *ps,DP *hps,int mod,DP *rp,P *dnp)
{
  DP hp,u,p,d,s,t,dmy;
  NODE l;
  MP m,mr;
  int i,n;
  int *wb;
  int sugar,psugar;
  P dn,tdn,tdn1;

  dn = (P)ONEM;
  if ( !g ) {
    *rp = 0; *dnp = dn; return;
  }
  for ( n = 0, l = b; l; l = NEXT(l), n++ )
    ;
  wb = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, l = b; i < n; l = NEXT(l), i++ )
    wb[i] = ZTOS((Q)BDY(l));
  sugar = g->sugar;
  for ( d = 0; g; ) {
    for ( u = 0, i = 0; i < n; i++ ) {
      if ( dp_redble(g,hp = hps[wb[i]]) ) {
        p = ps[wb[i]];
        dp_red_marked_mod(d,g,p,hp,mod,&t,&u,&tdn,&dmy);  
        psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
        sugar = MAX(sugar,psugar);
        if ( !u ) {
          if ( d )
            d->sugar = sugar;
          *rp = d; *dnp = dn; return;
        } else {
          d = t;
          mulmp(CO,mod,dn,tdn,&tdn1); dn = tdn1;
        }
        break;
      }
    }
    if ( u )
      g = u;
    else {
      m = BDY(g); NEWMP(mr); mr->dl = m->dl; mr->c = m->c;
      NEXT(mr) = 0; MKDP(g->nv,mr,t); t->sugar = mr->dl->td;
      addmd(CO,mod,d,t,&s); d = s;
      dp_rest(g,&t); g = t;
    }
  }
  if ( d )
    d->sugar = sugar;
  *rp = d; *dnp = dn;
}

/* true nf by a marked GB and collect quotients */

DP *dp_true_nf_and_quotient_marked (NODE b,DP g,DP *ps,DP *hps,DP *rp,P *dnp)
{
  DP u,p,d,s,t,dmy,hp,mult;
  DP *q;
  NODE l;
  MP m,mr;
  int i,n,j;
  int *wb;
  int sugar,psugar,multiple;
  P nm,tnm1,dn,tdn,tdn1;
  Q cont;

  dn = (P)ONE;
  if ( !g ) {
    *rp = 0; *dnp = dn; return 0;
  }
  for ( n = 0, l = b; l; l = NEXT(l), n++ );
  wb = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, l = b; i < n; l = NEXT(l), i++ )
    wb[i] = ZTOS((Q)BDY(l));
  q = (DP *)MALLOC(n*sizeof(DP));
  for ( i = 0; i < n; i++ ) q[i] = 0;
  sugar = g->sugar;
  for ( d = 0; g; ) {
    for ( u = 0, i = 0; i < n; i++ ) {
      if ( dp_redble(g,hp = hps[wb[i]]) ) {
        p = ps[wb[i]];
        _dp_red_marked(&d,g,p,hp,&u,&tdn,&mult);
        psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
        sugar = MAX(sugar,psugar);
        for ( j = 0; j < n; j++ ) {
          _muldc(CO,&q[j],(Obj)tdn);
        }
        addd(CO,q[wb[i]],mult,&dmy); q[wb[i]] = dmy;
        mulp(CO,dn,tdn,&tdn1); dn = tdn1;
        if ( !u ) goto last;
        break;
      }
    }
    if ( u ) {
      g = u;
    } else {
      if ( d == 0 ) {
        m = BDY(g); NEWMP(mr); mr->dl = m->dl; mr->c = m->c;
        NEXT(mr) = 0; MKDP(g->nv,mr,t); t->sugar = mr->dl->td;
        d = t;
      } else {
        for ( m = BDY(d); NEXT(m) != 0; m = NEXT(m) );
        NEWMP(mr); *mr = *BDY(g); NEXT(mr) = 0; NEXT(m) = mr;
        d->sugar = MAX(d->sugar,mr->dl->td);
      }
      dp_rest(g,&t); g = t;
    }
  }
last:
  if ( d ) d->sugar = sugar;
  *rp = d; *dnp = dn;
  return q;
}

struct oEGT egred;

void mulcmp(Obj c,MP m);
void mulcdmm(Obj c,DMM m);

DP appendd(DP d,DP m)
{
  MP t;

  if ( !d ) return m;
  for ( t = BDY(d); NEXT(t); t = NEXT(t) );
  NEXT(t) = BDY(m);
  return d;
}

DPM appenddpm(DPM d,DPM m)
{
  DMM t;

  if ( !d ) return m;
  for ( t = BDY(d); NEXT(t); t = NEXT(t) );
  NEXT(t) = BDY(m);
  return d;
}

DP *dpm_nf_and_quotient(NODE b,DPM g,VECT psv,DPM *rp,P *dnp)
{
  DPM u,p,s,t,d;
  DP dmy,mult,zzz;
  DPM *ps;
  DP *q;
  NODE l;
  DMM m,mr;
  MP mp;
  int i,n,j,len,nv;
  int *wb;
  int sugar,psugar,multiple;
  P nm,tnm1,dn,tdn,tdn1;
  Q cont;
  struct oEGT eg0,eg1;

  dn = (P)ONE;
  if ( !g ) {
    *rp = 0; *dnp = dn; return 0;
  }
  nv = NV(g);
  ps = (DPM *)BDY(psv);
  len = psv->len;
  if ( b ) {
    for ( n = 0, l = b; l; l = NEXT(l), n++ )
      ;
    wb = (int *)ALLOCA(n*sizeof(int));
    for ( i = 0, l = b; i < n; l = NEXT(l), i++ )
      wb[i] = ZTOS((Q)BDY(l));
  } else {
    wb = (int *)ALLOCA(len*sizeof(int));
    for ( i = j = 0; i < len; i++ ) 
      if ( ps[i] ) wb[j++] = i;
    n = j;
  }
  q = (DP *)MALLOC(len*sizeof(DP));
  for ( i = 0; i < len; i++ ) q[i] = 0;
  sugar = g->sugar;
  for ( d = 0; g; ) {
    for ( u = 0, i = 0; i < n; i++ ) {
      if ( dpm_redble(g,p = ps[wb[i]]) ) {
// get_eg(&eg0);
        dpm_red2(g,p,&u,&tdn,&mult);
// get_eg(&eg1); add_eg(&egred,&eg0,&eg1);
        psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
        sugar = MAX(sugar,psugar);
        for ( j = 0; j < len; j++ ) {
          if ( q[j] ) { mulcmp((Obj)tdn,BDY(q[j])); }
        }
        q[wb[i]] = appendd(q[wb[i]],mult);
        mulp(CO,dn,tdn,&tdn1); dn = tdn1;
        if ( d ) mulcdmm((Obj)tdn,BDY(d));
        if ( !u ) goto last;
        break;
      }
    }
    if ( u ) {
      g = u;
    } else {
      m = BDY(g); NEWDMM(mr); mr->dl = m->dl; mr->c = m->c; mr->pos = m->pos;
      NEXT(mr) = 0; MKDPM(g->nv,mr,t); t->sugar = mr->dl->td;
      d = appenddpm(d,t);
      dpm_rest(g,&t); g = t;
    }
  }
last:
  if ( d ) d->sugar = sugar;
  *rp = d; *dnp = dn;
  return q;
}

DPM dpm_nf_and_quotient2(NODE b,DPM g,VECT psv,DPM *rp,P *dnp)
{
  DPM u,p,s,t,d,q;
  DP dmy,mult,zzz;
  DPM *ps;
  NODE l;
  DMM mr0,mq0,mr,mq,m;
  MP mp;
  int i,n,j,len,nv;
  int *wb;
  int sugar,psugar,multiple;
  P nm,tnm1,dn,tdn,tdn1;
  Q cont;
  Obj c1;
  struct oEGT eg0,eg1;

  dn = (P)ONE;
  if ( !g ) {
    *rp = 0; *dnp = dn; return 0;
  }
  nv = NV(g);
  ps = (DPM *)BDY(psv);
  len = psv->len;
  if ( b ) {
    for ( n = 0, l = b; l; l = NEXT(l), n++ )
      ;
    wb = (int *)ALLOCA(n*sizeof(int));
    for ( i = 0, l = b; i < n; l = NEXT(l), i++ )
      wb[i] = ZTOS((Q)BDY(l));
  } else {
    wb = (int *)ALLOCA(len*sizeof(int));
    for ( i = j = 0; i < len; i++ ) 
      if ( ps[i] ) wb[j++] = i;
    n = j;
  }
  sugar = g->sugar;
  mq0 = 0;
  mr0 = 0;
  for ( ; g; ) {
    for ( u = 0, i = 0; i < n; i++ ) {
      if ( dpm_redble(g,p = ps[wb[i]]) ) {
        dpm_red2(g,p,&u,&tdn,&mult);
        psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
        sugar = MAX(sugar,psugar);
        for ( m = mq0; m; m = NEXT(m) ) {
          arf_mul(CO,(Obj)tdn,m->c,&c1); m->c = c1;
        }
        for ( m = mr0; m; m = NEXT(m) ) {
          arf_mul(CO,(Obj)tdn,m->c,&c1); m->c = c1;
        }
        NEXTDMM(mq0,mq); 
        mq->c = BDY(mult)->c; mq->dl = BDY(mult)->dl; mq->pos = wb[i]+1;
        mulp(CO,dn,tdn,&tdn1); dn = tdn1;
        if ( !u ) goto last;
        break;
      }
    }
    if ( u ) {
      g = u;
    } else {
      m = BDY(g);
      NEXTDMM(mr0,mr);
      mr->dl = m->dl; mr->c = m->c; mr->pos = m->pos;
      dpm_rest(g,&t); g = t;
    }
  }
last:
  if ( mr0 ) {
    NEXT(mr) = 0;
    MKDPM(nv,mr0,d); d->sugar = sugar;
  } else
    d = 0;
  if ( mq0 ) {
    NEXT(mq) = 0;
    MKDPM(nv,mq0,q); q->sugar = sugar;
  } else
    q = 0;
  *rp = d; *dnp = dn;
  return q;
}

DPM dpm_nf_and_quotient3(DPM g,VECT psv,DPM *rp,P *dnp)
{
  DPM u,p,s,t,d,q;
  DP dmy,mult,zzz;
  DPM *ps;
  NODE2 nd;
  DMM mr0,mq0,mr,mq,m;
  MP mp;
  int i,n,j,len,nv,pos,max;
  int *wb;
  int sugar,psugar,multiple;
  P nm,tnm1,dn,tdn,tdn1;
  Q cont;
  Obj c1;
  struct oEGT eg0,eg1;

  dn = (P)ONE;
  if ( !g ) {
    *rp = 0; *dnp = dn; return 0;
  }
  nv = NV(g);
  sugar = g->sugar;
  mq0 = 0;
  mr0 = 0;
  max = psv->len;
  for ( ; g; ) {
    pos = BDY(g)->pos;
    u = 0;
    if ( pos < max ) {
      nd = (NODE2)BDY(psv)[pos];
      for ( u = 0; nd; nd = NEXT(nd) ) {
        if ( dpm_redble(g,p = (DPM)(nd->body1)) ) {
          dpm_red2(g,p,&u,&tdn,&mult);
          psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
          sugar = MAX(sugar,psugar);
          if ( !UNIZ(tdn) ) {
            for ( m = mq0; m; m = NEXT(m) ) {
              arf_mul(CO,(Obj)tdn,m->c,&c1); m->c = c1;
            }
            for ( m = mr0; m; m = NEXT(m) ) {
              arf_mul(CO,(Obj)tdn,m->c,&c1); m->c = c1;
            }
          }
          NEXTDMM(mq0,mq); 
          mq->c = BDY(mult)->c; mq->dl = BDY(mult)->dl; mq->pos = (long)nd->body2;
          mulp(CO,dn,tdn,&tdn1); dn = tdn1;
          if ( !u ) goto last;
          break;
        }
      }
    }
    if ( u ) {
      g = u;
    } else {
      m = BDY(g);
      NEXTDMM(mr0,mr);
      mr->dl = m->dl; mr->c = m->c; mr->pos = m->pos;
      dpm_rest(g,&t); g = t;
    }
  }
last:
  if ( mr0 ) {
    NEXT(mr) = 0;
    MKDPM(nv,mr0,d); d->sugar = sugar;
  } else
    d = 0;
  if ( mq0 ) {
    NEXT(mq) = 0;
    MKDPM(nv,mq0,q); q->sugar = sugar;
  } else
    q = 0;
  *rp = d; *dnp = dn;
  return q;
}

DPM dpm_nf_and_quotient4(DPM g,DPM *ps,VECT psiv,DPM head,DPM *rp,P *dnp)
{
  DPM u,p,s,t,d,q;
  DP dmy,mult,zzz;
  NODE nd;
  DMM mr0,mq0,mr,mq,m;
  MP mp;
  int i,n,j,len,nv,pos,max;
  int *wb;
  int sugar,psugar,multiple;
  P nm,tnm1,dn,tdn,tdn1,c;
  Q cont;
  Obj c1;
  struct oEGT eg0,eg1;

  dn = (P)ONE;
  if ( !g ) {
    *rp = 0; *dnp = dn; return 0;
  }
  nv = NV(g);
  sugar = g->sugar;
  mq0 = 0;
  if ( head ) {
    for ( m = BDY(head); m; m = NEXT(m) ) {
      NEXTDMM(mq0,mq); 
      mq->c = m->c; mq->dl = m->dl; mq->pos = m->pos;
    }
  }
  mr0 = 0;
  max = psiv->len;
  for ( ; g; ) {
    pos = BDY(g)->pos;
    u = 0;
    if ( pos < max ) {
      nd = (NODE)BDY(psiv)[pos];
      for ( u = 0; nd; nd = NEXT(nd) ) {
        if ( dpm_redble(g,p = ps[(long)(BDY(nd))-1]) ) {
          dpm_red2(g,p,&u,&tdn,&mult);
          psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
          sugar = MAX(sugar,psugar);
          if ( !UNIZ(tdn) ) {
            for ( m = mq0; m; m = NEXT(m) ) {
              arf_mul(CO,(Obj)tdn,m->c,&c1); m->c = c1;
            }
            for ( m = mr0; m; m = NEXT(m) ) {
              arf_mul(CO,(Obj)tdn,m->c,&c1); m->c = c1;
            }
          }
          NEXTDMM(mq0,mq); 
          mq->c = BDY(mult)->c; 
          mq->dl = BDY(mult)->dl; mq->pos = (long)BDY(nd);
          mulp(CO,dn,tdn,&tdn1); dn = tdn1;
          if ( !u ) goto last;
          break;
        }
      }
    }
    if ( u ) {
      g = u;
    } else {
      m = BDY(g);
      NEXTDMM(mr0,mr);
      mr->dl = m->dl; mr->c = m->c; mr->pos = m->pos;
      dpm_rest(g,&t); g = t;
    }
  }
last:
  if ( mr0 ) {
    NEXT(mr) = 0;
    MKDPM(nv,mr0,d); d->sugar = sugar;
  } else
    d = 0;
  if ( mq0 ) {
    NEXT(mq) = 0;
    MKDPM(nv,mq0,q); q->sugar = sugar;
  } else
    q = 0;
  *rp = d; *dnp = dn;
  return q;
}

/* an intermediate version for calling from the user language */
/* XXX : i, j must be positive */

DPM dpm_sp_nf_asir(VECT psv,int i,int j,DPM *nf)
{
  DPM *ps;
  int n,k,nv,s1,s2,sugar,max,pos,psugar;
  DPM g,u,p,d,q,t;
  DMM mq0,mq,mr0,mr,m;
  DP mult,t1,t2;
  P dn,tdn,tdn1;
  NODE nd;
  Obj c1;

  ps = (DPM *)BDY(psv);
  n = psv->len;
  nv = ps[1]->nv;
  dpm_sp(ps[i],ps[j],&g,&t1,&t2);
  mq0 = 0;
  NEXTDMM(mq0,mq); mq->c = BDY(t1)->c; mq->pos = i; mq->dl = BDY(t1)->dl;
  NEXTDMM(mq0,mq); chsgnp((P)BDY(t2)->c,(P *)&mq->c); mq->pos = j; mq->dl = BDY(t2)->dl;

  if ( !g ) {
    NEXT(mq) = 0;
    MKDPM(nv,mq0,d);
    s1 = BDY(t1)->dl->td + ps[i]->sugar;
    s2 = BDY(t2)->dl->td + ps[j]->sugar;
    d->sugar = MAX(s1,s2);
    *nf = 0; 
    return d;
  }

  dn = (P)ONE;
  sugar = g->sugar;
  mr0 = 0;
  while ( g ) {
    pos = BDY(g)->pos;
    for ( u = 0, k = 1; k < n; k++ ) {
      if ( (p=ps[k])!=0 && pos == BDY(p)->pos && dpm_redble(g,p) ) {
        dpm_red2(g,p,&u,&tdn,&mult);
        psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
        sugar = MAX(sugar,psugar);
        if ( !UNIZ(tdn) ) {
          for ( m = mq0; m; m = NEXT(m) ) {
            arf_mul(CO,(Obj)tdn,m->c,&c1); m->c = c1;
          }
          for ( m = mr0; m; m = NEXT(m) ) {
            arf_mul(CO,(Obj)tdn,m->c,&c1); m->c = c1;
          }
        }
        NEXTDMM(mq0,mq); 
        chsgnp((P)BDY(mult)->c,(P *)&mq->c);
        mq->dl = BDY(mult)->dl; mq->pos = k;
        mulp(CO,dn,tdn,&tdn1); dn = tdn1;
        if ( !u ) goto last;
        break;
      }
    }
    if ( u ) {
      g = u;
    } else {
      m = BDY(g);
      NEXTDMM(mr0,mr);
      mr->dl = m->dl; mr->c = m->c; mr->pos = m->pos;
      dpm_rest(g,&t); g = t;
    }
  }
last:
  if ( mr0 ) {
    NEXT(mr) = 0; MKDPM(nv,mr0,d); d->sugar = sugar;
  } else
    d = 0;
  NEXT(mq) = 0; MKDPM(nv,mq0,q); q->sugar = sugar;
  *nf = d;
  return q;
}

DPM dpm_sp_nf(VECT psv,VECT psiv,int i,int j,DPM *nf)
{
  DPM *ps;
  int n,nv,s1,s2,sugar,max,pos,psugar;
  DPM g,u,p,d,q,t;
  DMM mq0,mq,mr0,mr,m;
  DP mult,t1,t2;
  P dn,tdn,tdn1;
  NODE nd;
  Obj c1;

  ps = (DPM *)BDY(psv);
  n = psv->len;
  nv = ps[i]->nv;
  dpm_sp(ps[i],ps[j],&g,&t1,&t2);
  mq0 = 0;
  NEXTDMM(mq0,mq); mq->c = BDY(t1)->c; mq->pos = i; mq->dl = BDY(t1)->dl;
  NEXTDMM(mq0,mq); chsgnp((P)BDY(t2)->c,(P *)&mq->c); mq->pos = j; mq->dl = BDY(t2)->dl;

  if ( !g ) {
    NEXT(mq) = 0;
    MKDPM(nv,mq0,d);
    s1 = BDY(t1)->dl->td + ps[i]->sugar;
    s2 = BDY(t2)->dl->td + ps[j]->sugar;
    d->sugar = MAX(s1,s2);
    *nf = 0; 
    return d;
  }

  dn = (P)ONE;
  sugar = g->sugar;
  mr0 = 0;
  max = psiv->len;
  while ( g ) {
    pos = BDY(g)->pos;
    u = 0;
    if ( pos < max ) {
      nd = (NODE)BDY(psiv)[pos];
      for ( u = 0; nd; nd = NEXT(nd) ) {
        if ( dpm_redble(g,p = ps[(long)(BDY(nd))]) ) {
          dpm_red2(g,p,&u,&tdn,&mult);
          psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
          sugar = MAX(sugar,psugar);
          if ( !UNIZ(tdn) ) {
            for ( m = mq0; m; m = NEXT(m) ) {
              arf_mul(CO,(Obj)tdn,m->c,&c1); m->c = c1;
            }
            for ( m = mr0; m; m = NEXT(m) ) {
              arf_mul(CO,(Obj)tdn,m->c,&c1); m->c = c1;
            }
          }
          NEXTDMM(mq0,mq); 
          chsgnp((P)BDY(mult)->c,(P *)&mq->c);
          mq->dl = BDY(mult)->dl; mq->pos = (long)BDY(nd);
          mulp(CO,dn,tdn,&tdn1); dn = tdn1;
          if ( !u ) goto last;
          break;
        }
      }
    }
    if ( u ) {
      g = u;
    } else {
      m = BDY(g);
      NEXTDMM(mr0,mr);
      mr->dl = m->dl; mr->c = m->c; mr->pos = m->pos;
      dpm_rest(g,&t); g = t;
    }
  }
last:
  if ( mr0 ) {
    NEXT(mr) = 0; MKDPM(nv,mr0,d); d->sugar = sugar;
  } else
    d = 0;
  NEXT(mq) = 0; MKDPM(nv,mq0,q); q->sugar = sugar;
  *nf = d;
  return q;
}

/* psiv is a vector of lists of Z */

DPM dpm_sp_nf_zlist(VECT psv,VECT psiv,int i,int j,int top,DPM *nf)
{
  DPM *ps;
  int n,nv,s1,s2,sugar,max,pos,psugar;
  DPM g,u,p,d,q,t;
  DMM mq0,mq,mr0,mr,m;
  DP mult,t1,t2;
  P dn,tdn,tdn1;
  NODE nd;
  Obj c1;

  ps = (DPM *)BDY(psv);
  n = psv->len;
  nv = ps[i]->nv;
  dpm_sp(ps[i],ps[j],&g,&t1,&t2);
  mq0 = 0;
  NEXTDMM(mq0,mq); mq->c = BDY(t1)->c; mq->pos = i; mq->dl = BDY(t1)->dl;
  NEXTDMM(mq0,mq); chsgnp((P)BDY(t2)->c,(P *)&mq->c); mq->pos = j; mq->dl = BDY(t2)->dl;

  if ( !g ) {
    NEXT(mq) = 0;
    MKDPM(nv,mq0,d);
    s1 = BDY(t1)->dl->td + ps[i]->sugar;
    s2 = BDY(t2)->dl->td + ps[j]->sugar;
    d->sugar = MAX(s1,s2);
    *nf = 0; 
    return d;
  }

  dn = (P)ONE;
  sugar = g->sugar;
  mr0 = 0;
  max = psiv->len;
  while ( g ) {
    pos = BDY(g)->pos;
    u = 0;
    if ( pos < max ) {
      nd = BDY((LIST)BDY(psiv)[pos]);
      for ( u = 0; nd; nd = NEXT(nd) ) {
        if ( dpm_redble(g,p = ps[ZTOS((Q)BDY(nd))]) ) {
          dpm_red2(g,p,&u,&tdn,&mult);
          psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
          sugar = MAX(sugar,psugar);
          if ( !UNIZ(tdn) ) {
            for ( m = mq0; m; m = NEXT(m) ) {
              arf_mul(CO,(Obj)tdn,m->c,&c1); m->c = c1;
            }
            for ( m = mr0; m; m = NEXT(m) ) {
              arf_mul(CO,(Obj)tdn,m->c,&c1); m->c = c1;
            }
          }
          NEXTDMM(mq0,mq); 
          chsgnp((P)BDY(mult)->c,(P *)&mq->c);
          mq->dl = BDY(mult)->dl; mq->pos = ZTOS((Q)BDY(nd));
          mulp(CO,dn,tdn,&tdn1); dn = tdn1;
          if ( !u ) goto last;
          break;
        }
      }
    }
    if ( u ) {
      g = u;
    } else if ( !top ) {
      m = BDY(g);
      NEXTDMM(mr0,mr);
      mr->dl = m->dl; mr->c = m->c; mr->pos = m->pos;
      dpm_rest(g,&t); g = t;
    } else {
      *nf = g;
      if ( mq0 ) {
        NEXT(mq) = 0; MKDPM(nv,mq0,q); q->sugar = sugar;
      } else
        q = 0;
      return q;
    }
  }
last:
  if ( mr0 ) {
    NEXT(mr) = 0; MKDPM(nv,mr0,d); d->sugar = sugar;
  } else
    d = 0;
  NEXT(mq) = 0; MKDPM(nv,mq0,q); q->sugar = sugar;
  *nf = d;
  return q;
}

DP *dp_true_nf_and_quotient_marked_mod(NODE b,DP g,DP *ps,DP *hps,int mod,DP *rp,P *dnp)
{
  DP u,p,d,s,t,dmy,hp,mult;
  DP *q;
  NODE l;
  MP m,mr;
  int i,n,j;
  int *wb;
  int sugar,psugar;
  P dn,tdn,tdn1;

  for ( n = 0, l = b; l; l = NEXT(l), n++ );
  q = (DP *)MALLOC(n*sizeof(DP));
  for ( i = 0; i < n; i++ ) q[i] = 0;
  dn = (P)ONEM;
  if ( !g ) {
    *rp = 0; *dnp = dn; return 0;
  }
  wb = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, l = b; i < n; l = NEXT(l), i++ )
    wb[i] = ZTOS((Q)BDY(l));
  sugar = g->sugar;
  for ( d = 0; g; ) {
    for ( u = 0, i = 0; i < n; i++ ) {
      if ( dp_redble(g,hp = hps[wb[i]]) ) {
        p = ps[wb[i]];
        dp_red_marked_mod(d,g,p,hp,mod,&t,&u,&tdn,&mult);  
        psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
        sugar = MAX(sugar,psugar);
        for ( j = 0; j < n; j++ ) {
          mulmdc(CO,mod,q[j],(P)tdn,&dmy); q[j] = dmy;
        }
        addmd(CO,mod,q[wb[i]],mult,&dmy); q[wb[i]] = dmy;
        mulmp(CO,mod,dn,tdn,&tdn1); dn = tdn1;
        d = t;
        if ( !u ) goto last;
        break;
      }
    }
    if ( u )
      g = u;
    else {
      m = BDY(g); NEWMP(mr); mr->dl = m->dl; mr->c = m->c;
      NEXT(mr) = 0; MKDP(g->nv,mr,t); t->sugar = mr->dl->td;
      addmd(CO,mod,d,t,&s); d = s;
      dp_rest(g,&t); g = t;
    }
  }
last:
  if ( d )
    d->sugar = sugar;
  *rp = d; *dnp = dn;
  return q;
}

/* nf computation over Z */

void dp_nf_z(NODE b,DP g,DP *ps,int full,int multiple,DP *rp)
{
  DP u,p,d,s,t,dmy1;
  P dmy;
  NODE l;
  MP m,mr;
  int i,n;
  int *wb;
  int hmag;
  int sugar,psugar;

  if ( !g ) {
    *rp = 0; return;
  }
  for ( n = 0, l = b; l; l = NEXT(l), n++ );
  wb = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, l = b; i < n; l = NEXT(l), i++ )
    wb[i] = ZTOS((Q)BDY(l));

  hmag = multiple*HMAG(g);
  sugar = g->sugar;

  for ( d = 0; g; ) {
    for ( u = 0, i = 0; i < n; i++ ) {
      if ( dp_redble(g,p = ps[wb[i]]) ) {
        dp_red(d,g,p,&t,&u,&dmy,&dmy1);
        psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
        sugar = MAX(sugar,psugar);
        if ( !u ) {
          if ( d )
            d->sugar = sugar;
          *rp = d; return;
        }
        d = t;
        break;
      }
    }
    if ( u ) {
      g = u;
      if ( d ) {
        if ( multiple && HMAG(d) > hmag ) {
          dp_ptozp2(d,g,&t,&u); d = t; g = u;
          hmag = multiple*HMAG(d);
        }
      } else {
        if ( multiple && HMAG(g) > hmag ) {
          dp_ptozp(g,&t); g = t;
          hmag = multiple*HMAG(g);
        }
      }
    }
    else if ( !full ) {
      if ( g ) {
        MKDP(g->nv,BDY(g),t); t->sugar = sugar; g = t;
      }
      *rp = g; return;
    } else {
      m = BDY(g); NEWMP(mr); mr->dl = m->dl; mr->c = m->c;
      NEXT(mr) = 0; MKDP(g->nv,mr,t); t->sugar = mr->dl->td;
      addd(CO,d,t,&s); d = s;
      dp_rest(g,&t); g = t;
        
    }
  }
  if ( d )
    d->sugar = sugar;
  *rp = d;
}

void dpm_nf_z(NODE b,DPM g,VECT psv,int full,int multiple,DPM *rp)
{
  DPM *ps;
  DPM u,p,d,s,t;
  DP dmy1;
  P dmy;
  Z cont;
  NODE l;
  DMM m,mr;
  int i,n;
  int *wb;
  int hmag;
  int sugar,psugar;

  if ( !g ) {
    *rp = 0; return;
  }
  if ( b ) {
    for ( n = 0, l = b; l; l = NEXT(l), n++ );
    wb = (int *)ALLOCA(n*sizeof(int));
    for ( i = 0, l = b; i < n; l = NEXT(l), i++ )
      wb[i] = ZTOS((Q)BDY(l));
    ps = (DPM *)BDY(psv);
  } else {
    n = psv->len;
    wb = (int *)MALLOC(n*sizeof(int));
    for ( i = 0; i < n; i++ ) wb[i] = i;
    ps = (DPM *)BDY(psv);
  }

  hmag = multiple*HMAG(g);
  sugar = g->sugar;

  for ( d = 0; g; ) {
    for ( u = 0, i = 0; i < n; i++ ) {
      if ( (p=ps[wb[i]])!=0 && dpm_redble(g,p) ) {
        dpm_red2(g,p,&u,&dmy,&dmy1);
        psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
        sugar = MAX(sugar,psugar);
        if ( d ) mulcdmm((Obj)dmy,BDY(d));
        if ( !u ) {
          if ( d )
            d->sugar = sugar;
          *rp = d; return;
        }
        break;
      }
    }
    if ( u ) {
      g = u;
      if ( d ) {
        if ( multiple && HMAG(d) > hmag ) {
          dpm_ptozp2(d,g,&t,&u); d = t; g = u;
          hmag = multiple*HMAG(d);
        }
      } else {
        if ( multiple && HMAG(g) > hmag ) {
          dpm_ptozp(g,&cont,&t); g = t;
          hmag = multiple*HMAG(g);
        }
      }
    }
    else if ( !full ) {
      if ( g ) {
        MKDPM(g->nv,BDY(g),t); t->sugar = sugar; g = t;
      }
      *rp = g; return;
    } else {
      m = BDY(g); NEWDMM(mr); mr->dl = m->dl; mr->c = m->c; mr->pos = m->pos;
      NEXT(mr) = 0; MKDPM(g->nv,mr,t); t->sugar = mr->dl->td;
      d = appenddpm(d,t);
      dpm_rest(g,&t); g = t;
    }
  }
  if ( d )
    d->sugar = sugar;
  *rp = d;
}

void dpm_shift(DPM p,int s,DPM *r)
{
  DMM m,mr0,mr;
  DPM t;

  if ( !p ) *r = 0;
  else {
    for ( m = BDY(p), mr0 = 0; m; m = NEXT(m) ) {
      NEXTDMM(mr0,mr);
      mr->dl = m->dl; mr->c = m->c; mr->pos = m->pos-s;
      if ( mr->pos <= 0 )
        error("dpm_shift : too large shift value");
    }
    NEXT(mr) = 0;
    MKDPM(p->nv,mr0,t); t->sugar = p->sugar;
    *r = t;
  }
}

// up=sum{c*<<...:i>>|i<=s}, lo=sum{c*<<...:i>>|i>s}

void dpm_split(DPM p,int s,DPM *up,DPM *lo)
{
  DMM m,mu0,mu,ml0,ml;
  DPM t;

  if ( !p ) {
    *up = 0; *lo = 0;
  } else {
    for ( m = BDY(p), mu0 = ml0 = 0; m; m = NEXT(m) ) {
      if ( m->pos <= s ) {
        NEXTDMM(mu0,mu);
        mu->dl = m->dl; mu->c = m->c; mu->pos = m->pos;
      } else {
        NEXTDMM(ml0,ml);
        ml->dl = m->dl; ml->c = m->c; ml->pos = m->pos;
      }
    }
    if ( mu0 ) {
      NEXT(mu) = 0; MKDPM(p->nv,mu0,t); t->sugar = p->sugar;
      *up = t;
    } else
      *up = 0;
    if ( ml0 ) {
      NEXT(ml) = 0; MKDPM(p->nv,ml0,t); t->sugar = p->sugar;
      *lo = t;
    } else
      *lo = 0;
  }
}

/* extract the component in DP of position s */
void dpm_extract(DPM p,int s,DP *r)
{
  DMM m;
  MP mu0,mu;
  DP t;

  if ( !p ) {
    *r = 0; return;
  }
  for ( m = BDY(p), mu0 = 0; m; m = NEXT(m) ) {
    if ( m->pos == s ) {
      NEXTMP(mu0,mu);
      mu->dl = m->dl; mu->c = m->c;
    }
  }
  if ( mu0 ) {
    NEXT(mu) = 0; MKDP(p->nv,mu0,t); t->sugar = p->sugar;
    *r = t;
  } else
    *r = 0;
}

/* nf computation over a field */

void dp_nf_f(NODE b,DP g,DP *ps,int full,DP *rp)
{
  DP u,p,d,s,t;
  NODE l;
  MP m,mr;
  int i,n;
  int *wb;
  int sugar,psugar;

  if ( !g ) {
    *rp = 0; return;
  }
  for ( n = 0, l = b; l; l = NEXT(l), n++ );
  wb = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, l = b; i < n; l = NEXT(l), i++ )
    wb[i] = ZTOS((Q)BDY(l));

  sugar = g->sugar;
  for ( d = 0; g; ) {
    for ( u = 0, i = 0; i < n; i++ ) {
      if ( dp_redble(g,p = ps[wb[i]]) ) {
        dp_red_f(g,p,&u);
        psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
        sugar = MAX(sugar,psugar);
        if ( !u ) {
          if ( d )
            d->sugar = sugar;
          *rp = d; return;
        }
        break;
      }
    }
    if ( u )
      g = u;
    else if ( !full ) {
      if ( g ) {
        MKDP(g->nv,BDY(g),t); t->sugar = sugar; g = t;
      }
      *rp = g; return;
    } else {
      m = BDY(g); NEWMP(mr); mr->dl = m->dl; mr->c = m->c;
      NEXT(mr) = 0; MKDP(g->nv,mr,t); t->sugar = mr->dl->td;
      addd(CO,d,t,&s); d = s;
      dp_rest(g,&t); g = t;
    }
  }
  if ( d )
    d->sugar = sugar;
  *rp = d;
}

void dpm_nf_f(NODE b,DPM g,VECT psv,int full,DPM *rp)
{
  DPM *ps;
  DPM u,p,d,s,t;
  NODE l;
  DMM m,mr;
  int i,n;
  int *wb;
  int sugar,psugar;

  if ( !g ) {
    *rp = 0; return;
  }
  if ( b ) {
    for ( n = 0, l = b; l; l = NEXT(l), n++ );
    wb = (int *)ALLOCA(n*sizeof(int));
    for ( i = 0, l = b; i < n; l = NEXT(l), i++ )
      wb[i] = ZTOS((Q)BDY(l));
    ps = (DPM *)BDY(psv);
  } else {
    n = psv->len;
    wb = (int *)MALLOC(n*sizeof(int));
    for ( i = 0; i < n; i++ ) wb[i] = i;
    ps = (DPM *)BDY(psv);
  }

  sugar = g->sugar;
  for ( d = 0; g; ) {
    for ( u = 0, i = 0; i < n; i++ ) {
      if ( ( (p=ps[wb[i]]) != 0 ) && dpm_redble(g,p) ) {
        dpm_red_f(g,p,&u);
        psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
        sugar = MAX(sugar,psugar);
        if ( !u ) {
          if ( d )
            d->sugar = sugar;
          *rp = d; return;
        }
        break;
      }
    }
    if ( u )
      g = u;
    else if ( !full ) {
      if ( g ) {
        MKDPM(g->nv,BDY(g),t); t->sugar = sugar; g = t;
      }
      *rp = g; return;
    } else {
      m = BDY(g); NEWDMM(mr); mr->dl = m->dl; mr->c = m->c; mr->pos = m->pos;
      NEXT(mr) = 0; MKDPM(g->nv,mr,t); t->sugar = mr->dl->td;
      adddpm(CO,d,t,&s); d = s;
      dpm_rest(g,&t); g = t;
    }
  }
  if ( d )
    d->sugar = sugar;
  *rp = d;
}

/* nf computation over GF(mod) (only for internal use) */

void dp_nf_mod(NODE b,DP g,DP *ps,int mod,int full,DP *rp)
{
  DP u,p,d,s,t;
  P dmy;
  NODE l;
  MP m,mr;
  int sugar,psugar;

  if ( !g ) {
    *rp = 0; return;
  }
  sugar = g->sugar;
  for ( d = 0; g; ) {
    for ( u = 0, l = b; l; l = NEXT(l) ) {
      if ( dp_redble(g,p = ps[(long)BDY(l)]) ) {
        dp_red_mod(d,g,p,mod,&t,&u,&dmy);
        psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
        sugar = MAX(sugar,psugar);
        if ( !u ) {
          if ( d )
            d->sugar = sugar;
          *rp = d; return;
        }
        d = t;
        break;
      }
    }
    if ( u )
      g = u;
    else if ( !full ) {
      if ( g ) {
        MKDP(g->nv,BDY(g),t); t->sugar = sugar; g = t;
      }
      *rp = g; return;
    } else {
      m = BDY(g); NEWMP(mr); mr->dl = m->dl; mr->c = m->c;
      NEXT(mr) = 0; MKDP(g->nv,mr,t); t->sugar = mr->dl->td;
      addmd(CO,mod,d,t,&s); d = s;
      dp_rest(g,&t); g = t;
    }
  }
  if ( d )
    d->sugar = sugar;
  *rp = d;
}

void dp_true_nf_mod(NODE b,DP g,DP *ps,int mod,int full,DP *rp,P *dnp)
{
  DP u,p,d,s,t;
  NODE l;
  MP m,mr;
  int i,n;
  int *wb;
  int sugar,psugar;
  P dn,tdn,tdn1;

  dn = (P)ONEM;
  if ( !g ) {
    *rp = 0; *dnp = dn; return;
  }
  for ( n = 0, l = b; l; l = NEXT(l), n++ )
    ;
  wb = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, l = b; i < n; l = NEXT(l), i++ )
    wb[i] = ZTOS((Q)BDY(l));
  sugar = g->sugar;
  for ( d = 0; g; ) {
    for ( u = 0, i = 0; i < n; i++ ) {
      if ( dp_redble(g,p = ps[wb[i]]) ) {
        dp_red_mod(d,g,p,mod,&t,&u,&tdn);  
        psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
        sugar = MAX(sugar,psugar);
        if ( !u ) {
          if ( d )
            d->sugar = sugar;
          *rp = d; *dnp = dn; return;
        } else {
          d = t;
          mulmp(CO,mod,dn,tdn,&tdn1); dn = tdn1;
        }
        break;
      }
    }
    if ( u )
      g = u;
    else if ( !full ) {
      if ( g ) {
        MKDP(g->nv,BDY(g),t); t->sugar = sugar; g = t;
      }
      *rp = g; *dnp = dn; return;
    } else {
      m = BDY(g); NEWMP(mr); mr->dl = m->dl; mr->c = m->c;
      NEXT(mr) = 0; MKDP(g->nv,mr,t); t->sugar = mr->dl->td;
      addmd(CO,mod,d,t,&s); d = s;
      dp_rest(g,&t); g = t;
    }
  }
  if ( d )
    d->sugar = sugar;
  *rp = d; *dnp = dn;
}

void _dp_nf_mod_destructive(NODE b,DP g,DP *ps,int mod,int full,DP *rp)
{
  DP u,p,d;
  NODE l;
  MP m,mrd;
  int sugar,psugar,n,h_reducible;

  if ( !g ) {
    *rp = 0; return;
  }
  sugar = g->sugar;
  n = g->nv;
  for ( d = 0; g; ) {
    for ( h_reducible = 0, l = b; l; l = NEXT(l) ) {
      if ( dp_redble(g,p = ps[(long)BDY(l)]) ) {
        h_reducible = 1;
        psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
        _dp_red_mod_destructive(g,p,mod,&u); g = u;
        sugar = MAX(sugar,psugar);
        if ( !g ) {
          if ( d )
            d->sugar = sugar;
          _dptodp(d,rp); _free_dp(d); return;
        }
        break;
      }
    }
    if ( !h_reducible ) {
      /* head term is not reducible */
      if ( !full ) {
        if ( g )
          g->sugar = sugar;
        _dptodp(g,rp); _free_dp(g); return;
      } else {
        m = BDY(g); 
        if ( NEXT(m) ) {
          BDY(g) = NEXT(m); NEXT(m) = 0;
        } else {
          _FREEDP(g); g = 0;
        }
        if ( d ) {
          for ( mrd = BDY(d); NEXT(mrd); mrd = NEXT(mrd) );
          NEXT(mrd) = m;
        } else {
          _MKDP(n,m,d);
        }
      }
    }
  }
  if ( d )
    d->sugar = sugar;
  _dptodp(d,rp); _free_dp(d);
}

/* reduction by linear base over a field */

void dp_lnf_f(DP p1,DP p2,NODE g,DP *r1p,DP *r2p)
{
  DP r1,r2,b1,b2,t,s;
  Obj c,c1,c2;
  NODE l,b;
  int n;

  if ( !p1 ) {
    *r1p = p1; *r2p = p2; return;
  }
  n = p1->nv;
  for ( l = g, r1 = p1, r2 = p2; l; l = NEXT(l) ) {
      if ( !r1 ) {
        *r1p = r1; *r2p = r2; return;
      }
      b = BDY((LIST)BDY(l)); b1 = (DP)BDY(b);
      if ( dl_equal(n,BDY(r1)->dl,BDY(b1)->dl) ) {
        b2 = (DP)BDY(NEXT(b));
        divr(CO,(Obj)ONE,(Obj)BDY(b1)->c,&c1);
        mulr(CO,c1,(Obj)BDY(r1)->c,&c2); chsgnr(c2,&c);
        muldc(CO,b1,(Obj)c,&t); addd(CO,r1,t,&s); r1 = s;
        muldc(CO,b2,(Obj)c,&t); addd(CO,r2,t,&s); r2 = s;
      }
  }
  *r1p = r1; *r2p = r2;
}

/* reduction by linear base over GF(mod) */

void dp_lnf_mod(DP p1,DP p2,NODE g,int mod,DP *r1p,DP *r2p)
{
  DP r1,r2,b1,b2,t,s;
  P c;
  MQ c1,c2;
  NODE l,b;
  int n;

  if ( !p1 ) {
    *r1p = p1; *r2p = p2; return;
  }
  n = p1->nv;
  for ( l = g, r1 = p1, r2 = p2; l; l = NEXT(l) ) {
      if ( !r1 ) {
        *r1p = r1; *r2p = r2; return;
      }
      b = BDY((LIST)BDY(l)); b1 = (DP)BDY(b);
      if ( dl_equal(n,BDY(r1)->dl,BDY(b1)->dl) ) {
        b2 = (DP)BDY(NEXT(b));
        invmq(mod,(MQ)BDY(b1)->c,&c1);
        mulmq(mod,c1,(MQ)BDY(r1)->c,&c2); chsgnmp(mod,(P)c2,&c);
        mulmdc(CO,mod,b1,c,&t); addmd(CO,mod,r1,t,&s); r1 = s;
        mulmdc(CO,mod,b2,c,&t); addmd(CO,mod,r2,t,&s); r2 = s;
      }
  }
  *r1p = r1; *r2p = r2;
}

void dp_nf_tab_mod(DP p,LIST *tab,int mod,DP *rp)
{
  DP s,t,u;
  MP m;
  DL h;
  int i,n;

  if ( !p ) {
    *rp = p; return;
  }
  n = p->nv;
  for ( s = 0, i = 0, m = BDY(p); m; m = NEXT(m) ) {
    h = m->dl;
    while ( !dl_equal(n,h,BDY((DP)BDY(BDY(tab[i])))->dl ) )
      i++;
    mulmdc(CO,mod,(DP)BDY(NEXT(BDY(tab[i]))),(P)m->c,&t);
    addmd(CO,mod,s,t,&u); s = u;
  }
  *rp = s;
}

void dp_nf_tab_f(DP p,LIST *tab,DP *rp)
{
  DP s,t,u;
  MP m;
  DL h;
  int i,n;

  if ( !p ) {
    *rp = p; return;
  }
  n = p->nv;
  for ( s = 0, i = 0, m = BDY(p); m; m = NEXT(m) ) {
    h = m->dl;
    while ( !dl_equal(n,h,BDY((DP)BDY(BDY(tab[i])))->dl ) )
      i++;
    muldc(CO,(DP)BDY(NEXT(BDY(tab[i]))),m->c,&t);
    addd(CO,s,t,&u); s = u;
  }
  *rp = s;
}

/*
 * setting flags
 * call create_order_spec with vl=0 to set old type order.
 *
 */

int create_order_spec(VL vl,Obj obj,struct order_spec **specp)
{
  int i,j,n,s,row,col,ret,wlen;
  struct order_spec *spec;
  struct order_pair *l;
  Obj wp,wm;
  NODE node,t,tn,wpair;
  MAT m;
  VECT v;
  pointer **b,*bv;
  int **w;

  if ( vl && obj && OID(obj) == O_LIST ) {
    ret = create_composite_order_spec(vl,(LIST)obj,specp);
    if ( show_orderspec )
      print_composite_order_spec(*specp);
    return ret;
  }

  *specp = spec = (struct order_spec *)MALLOC(sizeof(struct order_spec));
  if ( !obj || NUM(obj) ) {
    spec->id = 0; spec->obj = obj;
    spec->ord.simple = ZTOS((Q)obj);
    return 1;
  } else if ( OID(obj) == O_LIST ) {
    /* module order */
    node = BDY((LIST)obj); 
    if ( !BDY(node) || NUM(BDY(node)) ) {
      switch ( length(node) ) {
      case 2: /* [n,ord] */
        create_order_spec(0,(Obj)BDY(NEXT(node)),&spec);
        spec->id += 256; spec->obj = obj;
        spec->top_weight = 0;
        spec->module_rank = 0;
        spec->module_top_weight = 0;
        spec->module_ordtype = ZTOS((Z)BDY(node));
        if ( spec->module_ordtype < 0  ) {
            spec->pot_nelim = -spec->module_ordtype;
            spec->module_ordtype = 1;
        } else
            spec->pot_nelim = 0;
        break;

      case 3: /* [n,[mlist1,mlist2,...],ord] or [n,[wv,wm],ord] */
        spec->module_ordtype = ZTOS((Z)BDY(node));
        if ( spec->module_ordtype < 0  ) {
            spec->pot_nelim = -spec->module_ordtype;
            spec->module_ordtype = 1;
        } else
            spec->pot_nelim = 0;

        if ( spec->module_ordtype == 3 ) { /* schreyer order */
          Obj baseobj;
          struct order_spec *basespec;
          int len;
          NODE in;
          LIST *la;
          DMMstack stack;
          DMMstack push_schreyer_order(LIST l,DMMstack s);

          spec->id = 300; spec->obj = obj;
          node = NEXT(node);
          if ( !BDY(node) || OID(BDY(node)) != O_LIST ) 
            error("create_order_spec : [mlist1,mlist,...] must be specified for defining a schreyer order");
          stack = 0;
          in = BDY((LIST)BDY(node));
          len = length(in);
          la = (LIST *)MALLOC(len*sizeof(LIST));
          for ( i = 0; i < len; i++, in = NEXT(in) ) la[i] = (LIST)(BDY(in));
          for ( i = len-1; i >= 0; i-- ) stack = push_schreyer_order(la[i],stack);
          spec->dmmstack = stack;

          node = NEXT(node);
          baseobj = (Obj)BDY(node);
          create_order_spec(0,baseobj,&basespec);
          basespec->obj = baseobj;
          spec->base = basespec;
        } else if ( spec->module_ordtype == 4 ) {  /* POT with base order [n,bord,ord] */
          NODE base_ord;
          int rank;

          create_order_spec(0,(Obj)BDY(NEXT(NEXT(node))),&spec);
          spec->id += 256; spec->obj = obj;
          spec->top_weight = 0;
          spec->module_rank = 0;
          spec->module_top_weight = 0;
          spec->pot_nelim = 0;
          spec->module_ordtype = 4;
          node = NEXT(node);
          if ( !BDY(node) || OID(BDY(node)) != O_LIST ) 
            error("create_order_spec : a permitation list must be specified");
          base_ord = BDY((LIST)BDY(node));
          spec->module_rank = rank = length(base_ord);
          spec->module_base_ord = (int *)MALLOC_ATOMIC((rank+1)*sizeof(int));
          for ( i = 1, t = base_ord; i <= rank; i++, t = NEXT(t) )
            spec->module_base_ord[ZTOS((Q)BDY(t))] = i;
          break;
        } else {  /* weighted order * [n,[wv,wm],ord] */
          int ordtype;

          ordtype = spec->module_ordtype;
          create_order_spec(0,(Obj)BDY(NEXT(NEXT(node))),&spec);
          spec->module_ordtype = ordtype;
          spec->id += 256; spec->obj = obj;
          node = NEXT(node);
          if ( !BDY(node) || OID(BDY(node)) != O_LIST ) 
            error("create_order_spec : [weight_for_poly,weight_for_modlue] must be specified as a module topweight");
          wpair = BDY((LIST)BDY(node));
          if ( length(wpair) != 2 )
            error("create_order_spec : [weight_for_poly,weight_for_modlue] must be specified as a module topweight");
  
          wp = BDY(wpair);
          wm = BDY(NEXT(wpair));
          if ( !wp || OID(wp) != O_LIST || !wm || OID(wm) != O_LIST )
            error("create_order_spec : [weight_for_poly,weight_for_modlue] must be specified as a module topweight");
          spec->nv = length(BDY((LIST)wp)); 
          spec->top_weight = (int *)MALLOC_ATOMIC(spec->nv*sizeof(int));
          for ( i = 0, t = BDY((LIST)wp); i < spec->nv; t = NEXT(t), i++ )
            spec->top_weight[i] = ZTOS((Q)BDY(t));
  
          spec->module_rank = length(BDY((LIST)wm));
          spec->module_top_weight = (int *)MALLOC_ATOMIC(spec->module_rank*sizeof(int));
          for ( i = 0, t = BDY((LIST)wm); i < spec->module_rank; t = NEXT(t), i++ )
            spec->module_top_weight[i] = ZTOS((Q)BDY(t));
        }
        break;

      default:
        error("create_order_spec : invalid arguments for module order");
      }

      *specp = spec;
      return 1;
    } else {
      /* block order in polynomial ring */
      for ( n = 0, t = node; t; t = NEXT(t), n++ );
      l = (struct order_pair *)MALLOC_ATOMIC(n*sizeof(struct order_pair));
      for ( i = 0, t = node, s = 0; i < n; t = NEXT(t), i++ ) {
        tn = BDY((LIST)BDY(t)); l[i].order = ZTOS((Q)BDY(tn));
        tn = NEXT(tn); l[i].length = ZTOS((Q)BDY(tn));
        s += l[i].length;
      }
      spec->id = 1; spec->obj = obj;
      spec->ord.block.order_pair = l;
      spec->ord.block.length = n; spec->nv = s;
      return 1;
    }
  } else if ( OID(obj) == O_MAT ) {
    m = (MAT)obj; row = m->row; col = m->col; b = BDY(m);
    w = almat(row,col);
    for ( i = 0; i < row; i++ )
      for ( j = 0; j < col; j++ )
        w[i][j] = ZTOS((Q)b[i][j]);
    spec->id = 2; spec->obj = obj;
    spec->nv = col; spec->ord.matrix.row = row;
    spec->ord.matrix.matrix = w;
    return 1;
  } else
    return 0;
}

void print_composite_order_spec(struct order_spec *spec)
{
  int nv,n,len,i,j,k,start;
  struct weight_or_block *worb;

  nv = spec->nv;
  n = spec->ord.composite.length;
  worb = spec->ord.composite.w_or_b;
  for ( i = 0; i < n; i++, worb++ ) {
    len = worb->length;
    printf("[ ");
    switch ( worb->type ) {
      case IS_DENSE_WEIGHT:
        for ( j = 0; j < len; j++ )
          printf("%d ",worb->body.dense_weight[j]);
        for ( ; j < nv; j++ )
          printf("0 ");
        break;
      case IS_SPARSE_WEIGHT:
        for ( j = 0, k = 0; j < nv; j++ )
          if ( j == worb->body.sparse_weight[k].pos )
            printf("%d ",worb->body.sparse_weight[k++].value);
          else
            printf("0 ");
        break;
      case IS_BLOCK:
        start = worb->body.block.start;
        for ( j = 0; j < start; j++ ) printf("0 ");
        switch ( worb->body.block.order ) {
          case 0:
            for ( k = 0; k < len; k++, j++ ) printf("R ");
            break;
          case 1:
            for ( k = 0; k < len; k++, j++ ) printf("G ");
            break;
          case 2:
            for ( k = 0; k < len; k++, j++ ) printf("L ");
            break;
        }
        for ( ; j < nv; j++ ) printf("0 ");
        break;
    }
    printf("]\n");
  }
}

struct order_spec *append_block(struct order_spec *spec,
  int nv,int nalg,int ord)
{
  MAT m,mat;
  int i,j,row,col,n;
  Z **b,**wp;
  int **w;
  NODE t,s,s0;
  struct order_pair *l,*l0;
  int n0,nv0;
  LIST list0,list1,list;
  Z oq,nq;
  struct order_spec *r;

  r = (struct order_spec *)MALLOC(sizeof(struct order_spec));
  switch ( spec->id ) {
    case 0:
      STOZ(spec->ord.simple,oq); STOZ(nv,nq);
      t = mknode(2,oq,nq); MKLIST(list0,t);
      STOZ(ord,oq); STOZ(nalg,nq);
      t = mknode(2,oq,nq); MKLIST(list1,t);
      t = mknode(2,list0,list1); MKLIST(list,t);
      l = (struct order_pair *)MALLOC_ATOMIC(2*sizeof(struct order_pair));
      l[0].order = spec->ord.simple; l[0].length = nv;
      l[1].order = ord; l[1].length = nalg;
      r->id = 1;  r->obj = (Obj)list;
      r->ord.block.order_pair = l;
      r->ord.block.length = 2;
      r->nv = nv+nalg;
      break;
    case 1:
      if ( spec->nv != nv )
        error("append_block : number of variables mismatch");
      l0 = spec->ord.block.order_pair;
      n0 = spec->ord.block.length;
      nv0 = spec->nv;
      list0 = (LIST)spec->obj;
      n = n0+1;
      l = (struct order_pair *)MALLOC_ATOMIC(n*sizeof(struct order_pair));
      for ( i = 0; i < n0; i++ )
        l[i] = l0[i];
      l[i].order = ord; l[i].length = nalg;
       for ( t = BDY(list0), s0 = 0; t; t = NEXT(t) ) {
        NEXTNODE(s0,s); BDY(s) = BDY(t);
      }
      STOZ(ord,oq); STOZ(nalg,nq);
      t = mknode(2,oq,nq); MKLIST(list,t);
      NEXTNODE(s0,s); BDY(s) = (pointer)list; NEXT(s) = 0;
      MKLIST(list,s0);
      r->id = 1;  r->obj = (Obj)list;
      r->ord.block.order_pair = l;
      r->ord.block.length = n;
      r->nv = nv+nalg;
      break;
    case 2:
      if ( spec->nv != nv )
        error("append_block : number of variables mismatch");
      m = (MAT)spec->obj;
      row = m->row; col = m->col; b = (Z **)BDY(m);
      w = almat(row+nalg,col+nalg);
      MKMAT(mat,row+nalg,col+nalg); wp = (Z **)BDY(mat);
      for ( i = 0; i < row; i++ )
        for ( j = 0; j < col; j++ ) {
          w[i][j] = ZTOS(b[i][j]);
          wp[i][j] = b[i][j];
        }
      for ( i = 0; i < nalg; i++ ) {
        w[i+row][i+col] = 1;
        wp[i+row][i+col] = ONE;
      }
      r->id = 2; r->obj = (Obj)mat;
      r->nv = col+nalg; r->ord.matrix.row = row+nalg;
      r->ord.matrix.matrix = w;
      break;
    case 3:
    default:
      /* XXX */
      error("append_block : not implemented yet");
  }
  return r;
}

int comp_sw(struct sparse_weight *a, struct sparse_weight *b)
{
  if ( a->pos > b->pos ) return 1;
  else if ( a->pos < b->pos ) return -1;
  else return 0;
}

/* order = [w_or_b, w_or_b, ... ] */
/* w_or_b = w or b                */
/* w = [1,2,...] or [x,1,y,2,...] */
/* b = [@lex,x,y,...,z] etc       */

int create_composite_order_spec(VL vl,LIST order,struct order_spec **specp)
{
  NODE wb,t,p;
  struct order_spec *spec;
  VL tvl;
  int n,i,j,k,l,start,end,len,w;
  int *dw;
  struct sparse_weight *sw;
  struct weight_or_block *w_or_b;
  Obj a0;
  NODE a;
  V v,sv,ev;
  SYMBOL sym;
  int *top;

  /* l = number of vars in vl */
  for ( l = 0, tvl = vl; tvl; tvl = NEXT(tvl), l++ );
  /* n = number of primitives in order */
  wb = BDY(order);
  n = length(wb);
  *specp = spec = (struct order_spec *)MALLOC(sizeof(struct order_spec));
  spec->id = 3;
  spec->obj = (Obj)order;
  spec->nv = l;
  spec->ord.composite.length = n;
  w_or_b = spec->ord.composite.w_or_b = (struct weight_or_block *)
    MALLOC(sizeof(struct weight_or_block)*(n+1));

  /* top : register the top variable in each w_or_b specification */
  top = (int *)ALLOCA(l*sizeof(int));
  for ( i = 0; i < l; i++ ) top[i] = 0;

  for ( t = wb, i = 0; t; t = NEXT(t), i++ ) {
    if ( !BDY(t) || OID((Obj)BDY(t)) != O_LIST )
      error("a list of lists must be specified for the key \"order\"");
    a = BDY((LIST)BDY(t));
    len = length(a);
    a0 = (Obj)BDY(a);
    if ( !a0 || OID(a0) == O_N ) {
      /* a is a dense weight vector */
      dw = (int *)MALLOC(sizeof(int)*len);
      for ( j = 0, p = a; j < len; p = NEXT(p), j++ ) {
        if ( !INT((Q)BDY(p)) )  
          error("a dense weight vector must be specified as a list of integers");
        dw[j] = ZTOS((Q)BDY(p));
      }
      w_or_b[i].type = IS_DENSE_WEIGHT;
      w_or_b[i].length = len;
      w_or_b[i].body.dense_weight = dw;

      /* find the top */
      for ( k = 0; k < len && !dw[k]; k++ );
      if ( k < len ) top[k] = 1;

    } else if ( OID(a0) == O_P ) {
      /* a is a sparse weight vector */
      len >>= 1;
      sw = (struct sparse_weight *)
        MALLOC(sizeof(struct sparse_weight)*len);
      for ( j = 0, p = a; j < len; j++ ) {
        if ( !BDY(p) || OID((P)BDY(p)) != O_P )
          error("a sparse weight vector must be specified as [var1,weight1,...]");
        v = VR((P)BDY(p)); p = NEXT(p);
        for ( tvl = vl, k = 0; tvl && tvl->v != v;
          k++, tvl = NEXT(tvl) );
        if ( !tvl )
          error("invalid variable name in a sparse weight vector");
        sw[j].pos = k;
        if ( !INT((Q)BDY(p)) )
          error("a sparse weight vector must be specified as [var1,weight1,...]");
        sw[j].value = ZTOS((Q)BDY(p)); p = NEXT(p);
      }
      qsort(sw,len,sizeof(struct sparse_weight),
        (int (*)(const void *,const void *))comp_sw);
      w_or_b[i].type = IS_SPARSE_WEIGHT;
      w_or_b[i].length = len;
      w_or_b[i].body.sparse_weight = sw;

      /* find the top */
      for ( k = 0; k < len && !sw[k].value; k++ );
      if ( k < len ) top[sw[k].pos] = 1;
    } else if ( OID(a0) == O_RANGE ) {
      /* [range(v1,v2),w] */
      sv = VR((P)(((RANGE)a0)->start));
      ev = VR((P)(((RANGE)a0)->end));
      for ( tvl = vl, start = 0; tvl && tvl->v != sv; start++, tvl = NEXT(tvl) );
      if ( !tvl )
        error("invalid range");
      for ( end = start; tvl && tvl->v != ev; end++, tvl = NEXT(tvl) );
      if ( !tvl )
        error("invalid range");
      len = end-start+1;
      sw = (struct sparse_weight *)
        MALLOC(sizeof(struct sparse_weight)*len);
      w = ZTOS((Q)BDY(NEXT(a)));
      for ( tvl = vl, k = 0; k < start; k++, tvl = NEXT(tvl) );
      for ( j = 0 ; k <= end; k++, tvl = NEXT(tvl), j++ ) {
        sw[j].pos = k;
        sw[j].value = w;
      }
      w_or_b[i].type = IS_SPARSE_WEIGHT;
      w_or_b[i].length = len;
      w_or_b[i].body.sparse_weight = sw;

      /* register the top */
      if ( w ) top[start] = 1;
    } else if ( OID(a0) == O_SYMBOL ) {
      /* a is a block */
      sym = (SYMBOL)a0; a = NEXT(a); len--;
      if ( OID((Obj)BDY(a)) == O_RANGE ) {
        sv = VR((P)(((RANGE)BDY(a))->start));
        ev = VR((P)(((RANGE)BDY(a))->end));
        for ( tvl = vl, start = 0; tvl && tvl->v != sv; start++, tvl = NEXT(tvl) );
        if ( !tvl )
          error("invalid range");
        for ( end = start; tvl && tvl->v != ev; end++, tvl = NEXT(tvl) );
        if ( !tvl )
          error("invalid range");
        len = end-start+1;
      } else {
        for ( start = 0, tvl = vl; tvl->v != VR((P)BDY(a));
        tvl = NEXT(tvl), start++ );
        for ( p = NEXT(a), tvl = NEXT(tvl); p;
          p = NEXT(p), tvl = NEXT(tvl) ) {
          if ( !BDY(p) || OID((P)BDY(p)) != O_P )
            error("a block must be specified as [ordsymbol,var1,var2,...]");
          if ( tvl->v != VR((P)BDY(p)) ) break;
        }
        if ( p )
          error("a block must be contiguous in the variable list");
      }
      w_or_b[i].type = IS_BLOCK;
      w_or_b[i].length = len;
      w_or_b[i].body.block.start = start;
      if ( !strcmp(sym->name,"@grlex") )
        w_or_b[i].body.block.order = 0;
      else if ( !strcmp(sym->name,"@glex") )
        w_or_b[i].body.block.order = 1;
      else if ( !strcmp(sym->name,"@lex") )
        w_or_b[i].body.block.order = 2;
      else
        error("invalid ordername");
      /* register the tops */
      for ( j = 0, k = start; j < len; j++, k++ )
        top[k] = 1;
    }
  }
  for ( k = 0; k < l && top[k]; k++ );
  if ( k < l ) {
    /* incomplete order specification; add @grlex */
    w_or_b[n].type = IS_BLOCK;
    w_or_b[n].length = l;
    w_or_b[n].body.block.start = 0;
    w_or_b[n].body.block.order = 0;
    spec->ord.composite.length = n+1;
  }
  return 1;
}

/* module order spec */

void create_modorder_spec(int id,LIST shift,struct modorder_spec **s)
{
  struct modorder_spec *spec;
  NODE n,t;
  LIST list;
  int *ds;
  int i,l;
  Z q;

  *s = spec = (struct modorder_spec *)MALLOC(sizeof(struct modorder_spec));
  spec->id = id;
  if ( shift ) {
    n = BDY(shift);
    spec->len = l = length(n);
    spec->degree_shift = ds = (int *)MALLOC_ATOMIC(l*sizeof(int));
    for ( t = n, i = 0; t; t = NEXT(t), i++ )
      ds[i] = ZTOS((Q)BDY(t));
  } else {
    spec->len = 0;
    spec->degree_shift = 0;
  }
  STOZ(id,q);
  n = mknode(2,q,shift);
  MKLIST(list,n);
  spec->obj = (Obj)list;
}

/*
 * converters
 *
 */

void dpm_homo(DPM p,DPM *rp)
{
  DMM m,mr,mr0,t;
  int i,n,nv,td;
  DL dl,dlh;

  if ( !p )
    *rp = 0;
  else {
    n = p->nv; nv = n + 1;
    m = BDY(p);
    td = 0;
    for ( t = m; t; t = NEXT(t) )
      if ( m->dl->td > td ) td = m->dl->td;
    for ( mr0 = 0; m; m = NEXT(m) ) {
      NEXTDMM(mr0,mr); mr->c = m->c; mr->pos = m->pos;
      dl = m->dl;
      mr->dl = dlh = (DL)MALLOC_ATOMIC((nv+1)*sizeof(int));
      dlh->td = td;
      for ( i = 0; i < n; i++ )
        dlh->d[i] = dl->d[i];
      dlh->d[n] = td - dl->td;
    }
    NEXT(mr) = 0; MKDPM(nv,mr0,*rp); (*rp)->sugar = p->sugar;
  }
}

void dpm_dehomo(DPM p,DPM *rp)
{
  DMM m,mr,mr0;
  int i,n,nv;
  DL dl,dlh;

  if ( !p )
    *rp = 0;
  else {
    n = p->nv; nv = n - 1;
    m = BDY(p);
    for ( mr0 = 0; m; m = NEXT(m) ) {
      NEXTDMM(mr0,mr); mr->c = m->c; mr->pos = m->pos;
      dlh = m->dl;
      mr->dl = dl = (DL)MALLOC_ATOMIC((nv+1)*sizeof(int));
      dl->td = dlh->td - dlh->d[nv];
      for ( i = 0; i < nv; i++ )
        dl->d[i] = dlh->d[i];
    }
    NEXT(mr) = 0; MKDPM(nv,mr0,*rp); (*rp)->sugar = p->sugar;
  }
}

void dp_homo(DP p,DP *rp)
{
  MP m,mr,mr0;
  int i,n,nv,td;
  DL dl,dlh;

  if ( !p )
    *rp = 0;
  else {
    n = p->nv; nv = n + 1;
    m = BDY(p); td = sugard(m);
    for ( mr0 = 0; m; m = NEXT(m) ) {
      NEXTMP(mr0,mr); mr->c = m->c;
      dl = m->dl;
      mr->dl = dlh = (DL)MALLOC_ATOMIC((nv+1)*sizeof(int));
      dlh->td = td;
      for ( i = 0; i < n; i++ )
        dlh->d[i] = dl->d[i];
      dlh->d[n] = td - dl->td;
    }
    NEXT(mr) = 0; MKDP(nv,mr0,*rp); (*rp)->sugar = p->sugar;
  }
}

void dp_dehomo(DP p,DP *rp)
{
  MP m,mr,mr0;
  int i,n,nv;
  DL dl,dlh;

  if ( !p )
    *rp = 0;
  else {
    n = p->nv; nv = n - 1;
    m = BDY(p);
    for ( mr0 = 0; m; m = NEXT(m) ) {
      NEXTMP(mr0,mr); mr->c = m->c;
      dlh = m->dl;
      mr->dl = dl = (DL)MALLOC_ATOMIC((nv+1)*sizeof(int));
      dl->td = dlh->td - dlh->d[nv];
      for ( i = 0; i < nv; i++ )
        dl->d[i] = dlh->d[i];
    }
    NEXT(mr) = 0; MKDP(nv,mr0,*rp); (*rp)->sugar = p->sugar;
  }
}


void dp_mod(DP p,int mod,NODE subst,DP *rp)
{
  MP m,mr,mr0;
  P t,s,s1;
  V v;
  NODE tn;

  if ( !p )
    *rp = 0;
  else {
    for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
      for ( tn = subst, s = (P)m->c; tn; tn = NEXT(tn) ) {
        v = VR((P)BDY(tn)); tn = NEXT(tn);
        substp(CO,s,v,(P)BDY(tn),&s1); s = s1;
      }
      ptomp(mod,s,&t);
      if ( t ) {
        NEXTMP(mr0,mr); mr->c = (Obj)t; mr->dl = m->dl;
      }
    }
    if ( mr0 ) {
      NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
    } else
      *rp = 0;
  }
}

void dpm_mod(DPM p,int mod,DPM *rp)
{
  DMM m,mr,mr0;
  P t;
  V v;
  NODE tn;

  if ( !p )
    *rp = 0;
  else {
    for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
      ptomp(mod,(P)m->c,&t);
      if ( t ) {
        NEXTDMM(mr0,mr); mr->c = (Obj)t; mr->dl = m->dl;
      }
    }
    if ( mr0 ) {
      NEXT(mr) = 0; MKDPM(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
    } else
      *rp = 0;
  }
}

void dp_rat(DP p,DP *rp)
{
  MP m,mr,mr0;

  if ( !p )
    *rp = 0;
  else {
    for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
      NEXTMP(mr0,mr); mptop((P)m->c,(P *)&mr->c); mr->dl = m->dl;
    }
    if ( mr0 ) {
      NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
    } else
      *rp = 0;
  }
}


void homogenize_order(struct order_spec *old,int n,struct order_spec **newp)
{
  struct order_pair *l;
  int length,nv,row,i,j;
  int **newm,**oldm;
  struct order_spec *new;
  int onv,nnv,nlen,olen,owlen;
  struct weight_or_block *owb,*nwb;

  *newp = new = (struct order_spec *)MALLOC(sizeof(struct order_spec));
  bcopy((char *)old,(char *)new,sizeof(struct order_spec));
  switch ( old->id ) {
    case 0:
      switch ( old->ord.simple ) {
        case 0:
          break;
        case 1:
          l = (struct order_pair *)
            MALLOC_ATOMIC(2*sizeof(struct order_pair));
          l[0].length = n; l[0].order = 1;
          l[1].length = 1; l[1].order = 2;
          new->id = 1;
          new->ord.block.order_pair = l;
          new->ord.block.length = 2; new->nv = n+1;
          break;
        case 2:
          new->ord.simple = 1; break;
        case 3: case 4: case 5:
          new->ord.simple = old->ord.simple+3;
          dp_nelim = n-1; break;
        case 6: case 7: case 8: case 9:
          break;
        default:
          error("homogenize_order : invalid input");
      }
      break;
    case 1: case 257:
      length = old->ord.block.length;
      l = (struct order_pair *)
        MALLOC_ATOMIC((length+1)*sizeof(struct order_pair));
      bcopy((char *)old->ord.block.order_pair,(char *)l,length*sizeof(struct order_pair));
      l[length].order = 2; l[length].length = 1;
      new->nv = n+1;
      new->ord.block.order_pair = l;
      new->ord.block.length = length+1;
      break;
    case 2: case 258:
      nv = old->nv; row = old->ord.matrix.row;
      oldm = old->ord.matrix.matrix; newm = almat(row+1,nv+1);
      for ( i = 0; i <= nv; i++ )
        newm[0][i] = 1;
      for ( i = 0; i < row; i++ ) {
        for ( j = 0; j < nv; j++ )
          newm[i+1][j] = oldm[i][j];
        newm[i+1][j] = 0;
      }
      new->nv = nv+1;
      new->ord.matrix.row = row+1; new->ord.matrix.matrix = newm;
      break;
    case 3: case 259:
      onv = old->nv;
      nnv = onv+1;
      olen = old->ord.composite.length;
      nlen = olen+1;
      owb = old->ord.composite.w_or_b;
      nwb = (struct weight_or_block *)
        MALLOC(nlen*sizeof(struct weight_or_block));
      for ( i = 0; i < olen; i++ ) {
        nwb[i].type = owb[i].type;
        switch ( owb[i].type ) {
          case IS_DENSE_WEIGHT:
            owlen = owb[i].length;
            nwb[i].length = owlen+1;
            nwb[i].body.dense_weight = (int *)MALLOC((owlen+1)*sizeof(int));
            for ( j = 0; j < owlen; j++ )
              nwb[i].body.dense_weight[j] = owb[i].body.dense_weight[j];
            nwb[i].body.dense_weight[owlen] = 0;
            break;
          case IS_SPARSE_WEIGHT:
            nwb[i].length = owb[i].length;
            nwb[i].body.sparse_weight = owb[i].body.sparse_weight;
            break;
          case IS_BLOCK:
            nwb[i].length = owb[i].length;
            nwb[i].body.block = owb[i].body.block;
            break;
        }
      }
      nwb[i].type = IS_SPARSE_WEIGHT;
      nwb[i].body.sparse_weight = 
        (struct sparse_weight *)MALLOC(sizeof(struct sparse_weight));
      nwb[i].body.sparse_weight[0].pos = onv;
      nwb[i].body.sparse_weight[0].value = 1;
      new->nv = nnv;
      new->ord.composite.length = nlen;
      new->ord.composite.w_or_b = nwb;
      print_composite_order_spec(new);
      break;
    case 256: /* simple module order */
      switch ( old->ord.simple ) {
        case 0:
          break;
        case 1:
          l = (struct order_pair *)
            MALLOC_ATOMIC(2*sizeof(struct order_pair));
          l[0].length = n; l[0].order = old->ord.simple;
          l[1].length = 1; l[1].order = 2;
          new->id = 257;
          new->ord.block.order_pair = l;
          new->ord.block.length = 2; new->nv = n+1;
          break;
        case 2:
          new->ord.simple = 1; break;
        default:
          error("homogenize_order : invalid input");
      }
      break;
    default:
      error("homogenize_order : invalid input");
  }
}

int comp_nm(Q *a,Q *b)
{
  Z z,nma,nmb;

  nmq(*a,&z); absz(z,&nma);
  nmq(*b,&z); absz(z,&nmb);
  return cmpz(nma,nmb);
}

void sortbynm(Q *w,int n)
{
  qsort(w,n,sizeof(Q),(int (*)(const void *,const void *))comp_nm);
}


/*
 * simple operations
 *
 */

int dp_redble(DP p1,DP p2)
{
  int i,n;
  DL d1,d2;

  d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
  if ( d1->td < d2->td )
    return 0;
  else {
    for ( i = 0, n = p1->nv; i < n; i++ )
      if ( d1->d[i] < d2->d[i] )
        return 0;
    return 1;
  }
}

int dpm_redble(DPM p1,DPM p2)
{
  int i,n;
  DL d1,d2;

  if ( BDY(p1)->pos != BDY(p2)->pos ) return 0;
  d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
  if ( d1->td < d2->td )
    return 0;
  else {
    for ( i = 0, n = p1->nv; i < n; i++ )
      if ( d1->d[i] < d2->d[i] )
        return 0;
    return 1;
  }
}


void dp_subd(DP p1,DP p2,DP *rp)
{
  int i,n;
  DL d1,d2,d;
  MP m;
  DP s;

  n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
  NEWDL(d,n); d->td = d1->td - d2->td;
  for ( i = 0; i < n; i++ )
    d->d[i] = d1->d[i]-d2->d[i];
  NEWMP(m); m->dl = d; m->c = (Obj)ONE; NEXT(m) = 0;
  MKDP(n,m,s); s->sugar = d->td;
  *rp = s;
}

void dltod(DL d,int n,DP *rp)
{
  MP m;
  DP s;

  NEWMP(m); m->dl = d; m->c = (Obj)ONE; NEXT(m) = 0;
  MKDP(n,m,s); s->sugar = d->td;
  *rp = s;
}

void dp_hm(DP p,DP *rp)
{
  MP m,mr;

  if ( !p )
    *rp = 0;
  else {
    m = BDY(p);
    NEWMP(mr); mr->dl = m->dl; mr->c = m->c; NEXT(mr) = 0;
    MKDP(p->nv,mr,*rp); (*rp)->sugar = mr->dl->td;   /* XXX */
  }
}

void dp_ht(DP p,DP *rp)
{
  MP m,mr;

  if ( !p )
    *rp = 0;
  else {
    m = BDY(p);
    NEWMP(mr); mr->dl = m->dl; mr->c = (Obj)ONE; NEXT(mr) = 0;
    MKDP(p->nv,mr,*rp); (*rp)->sugar = mr->dl->td;   /* XXX */
  }
}

void dpm_hm(DPM p,DPM *rp)
{
  DMM m,mr;

  if ( !p )
    *rp = 0;
  else {
    m = BDY(p);
    NEWDMM(mr); mr->dl = m->dl; mr->c = m->c; mr->pos = m->pos; NEXT(mr) = 0;
    MKDPM(p->nv,mr,*rp); (*rp)->sugar = mr->dl->td;   /* XXX */
  }
}

void dpm_ht(DPM p,DPM *rp)
{
  DMM m,mr;

  if ( !p )
    *rp = 0;
  else {
    m = BDY(p);
    NEWDMM(mr); mr->dl = m->dl; mr->pos = m->pos; mr->c = (Obj)ONE; NEXT(mr) = 0;
    MKDPM(p->nv,mr,*rp); (*rp)->sugar = mr->dl->td;   /* XXX */
  }
}


void dp_rest(DP p,DP *rp)
{
  MP m;

  m = BDY(p);
  if ( !NEXT(m) )
    *rp = 0;
  else {
    MKDP(p->nv,NEXT(m),*rp);
    if ( *rp )
      (*rp)->sugar = p->sugar;
  }
}

void dpm_rest(DPM p,DPM *rp)
{
  DMM m;

  m = BDY(p);
  if ( !NEXT(m) )
    *rp = 0;
  else {
    MKDPM(p->nv,NEXT(m),*rp);
    if ( *rp )
      (*rp)->sugar = p->sugar;
  }
}

int dp_getdeg(DP p)
{
  int max,n,i;
  MP m;
  int *d;

  if ( !p ) return 0;
  n = p->nv;
  max = 0;
  for ( m = BDY(p); m; m = NEXT(m) ) {
    d = m->dl->d; 
    for ( i = 0; i < n; i++ )
      if ( d[i] > max ) max = d[i];
  }
  return max;
}

int dpm_getdeg(DPM p,int *r)
{
  int max,n,i,rank;
  DMM m;
  int *d;

  if ( !p ) return 0;
  n = p->nv;
  max = 0;
  rank = 0;
  for ( m = BDY(p); m; m = NEXT(m) ) {
    d = m->dl->d; 
    for ( i = 0; i < n; i++ )
      if ( d[i] > max ) max = d[i];
    rank = MAX(rank,m->pos);
  }
  *r = rank;
  return max;
}

DL lcm_of_DL(int nv,DL dl1,DL dl2,DL dl)
{
  register int i, *d1, *d2, *d, td;

  if ( !dl ) NEWDL(dl,nv);
  d = dl->d,  d1 = dl1->d,  d2 = dl2->d;
  for ( td = 0, i = 0; i < nv; d1++, d2++, d++, i++ ) {
    *d = *d1 > *d2 ? *d1 : *d2;
    td += MUL_WEIGHT(*d,i);
  }
  dl->td = td;
  return dl;
}

int dl_equal(int nv,DL dl1,DL dl2)
{
    register int *d1, *d2, n;

    if ( dl1->td != dl2->td ) return 0;
    for ( d1 = dl1->d, d2 = dl2->d, n = nv; --n >= 0; d1++, d2++ )
        if ( *d1 != *d2 ) return 0;
    return 1;
}

int dp_nt(DP p)
{
  int i;
  MP m;

  if ( !p )
    return 0;
  else {
    for ( i = 0, m = BDY(p); m; m = NEXT(m), i++ );
    return i;
  }
}

int dp_homogeneous(DP p)
{
  MP m;
  int d;

  if ( !p )
    return 1;
  else {
    m = BDY(p);
    d = m->dl->td;
    m = NEXT(m);
    for ( ; m; m = NEXT(m) ) {
      if ( m->dl->td != d )
        return 0;
    }
    return 1;
  }
}

void _print_mp(int nv,MP m)
{
  int i;

  if ( !m )
    return;
  for ( ; m; m = NEXT(m) ) {
    fprintf(stderr,"%ld<",ITOS(C(m)));
    for ( i = 0; i < nv; i++ ) {
      fprintf(stderr,"%d",m->dl->d[i]);
      if ( i != nv-1 )
        fprintf(stderr," ");
    }
    fprintf(stderr,">");
  }
  fprintf(stderr,"\n");
}

static int cmp_mp_nvar;

int comp_mp(MP *a,MP *b)
{
  return -(*cmpdl)(cmp_mp_nvar,(*a)->dl,(*b)->dl);
}

void dp_sort(DP p,DP *rp)
{
  MP t,mp,mp0;
  int i,n;
  DP r;
  MP *w;

  if ( !p ) {
    *rp = 0;
    return;
  }
  for ( t = BDY(p), n = 0; t; t = NEXT(t), n++ );
  w = (MP *)ALLOCA(n*sizeof(MP));
  for ( t = BDY(p), i = 0; i < n; t = NEXT(t), i++ )
    w[i] = t;
  cmp_mp_nvar = NV(p);
  qsort(w,n,sizeof(MP),(int (*)(const void *,const void *))comp_mp);
  mp0 = 0;
  for ( i = n-1; i >= 0; i-- ) {
    NEWMP(mp); mp->dl = w[i]->dl; C(mp) = C(w[i]);
    NEXT(mp) = mp0; mp0 = mp;
  }
  MKDP(p->nv,mp0,r);
  r->sugar = p->sugar;
  *rp = r;
}

DP extract_initial_term_from_dp(DP p,int *weight,int n);
LIST extract_initial_term(LIST f,int *weight,int n);

DP extract_initial_term_from_dp(DP p,int *weight,int n)
{
  int w,t,i,top;
  MP m,r0,r;
  DP dp;

  if ( !p ) return 0;
  top = 1;
  for ( m = BDY(p); m; m = NEXT(m) ) {
    for ( i = 0, t = 0; i < n; i++ )  
      t += weight[i]*m->dl->d[i];
    if ( top || t > w ) {
      r0 = 0;
      w = t;
      top = 0;
    }
    if ( t == w ) {
      NEXTMP(r0,r);
      r->dl = m->dl;
      r->c = m->c;
    }
  }
  NEXT(r) = 0;
  MKDP(p->nv,r0,dp);
  return dp;
}

LIST extract_initial_term(LIST f,int *weight,int n)
{
  NODE nd,r0,r;
  Obj p;
  LIST l;

  nd = BDY(f);
  for ( r0 = 0; nd; nd = NEXT(nd) ) {
    NEXTNODE(r0,r);
    p = (Obj)BDY(nd);
    BDY(r) = (pointer)extract_initial_term_from_dp((DP)p,weight,n);
  }
  if ( r0 ) NEXT(r) = 0;
  MKLIST(l,r0);
  return l;
}

LIST dp_initial_term(LIST f,struct order_spec *ord)
{
  int n,l,i;
  struct weight_or_block *worb;
  int *weight;

  switch ( ord->id ) {
    case 2: /* matrix order */
      /* extract the first row */
      n = ord->nv;
      weight = ord->ord.matrix.matrix[0];
      return extract_initial_term(f,weight,n);
    case 3: /* composite order */
      /* the first w_or_b */
      worb = ord->ord.composite.w_or_b;
      switch ( worb->type ) {
        case IS_DENSE_WEIGHT:
          n = worb->length;
          weight = worb->body.dense_weight;
          return extract_initial_term(f,weight,n);
        case IS_SPARSE_WEIGHT:
          n = ord->nv;
          weight = (int *)ALLOCA(n*sizeof(int));
          for ( i = 0; i < n; i++ ) weight[i] = 0;
          l = worb->length;
          for ( i = 0; i < l; i++ )
            weight[worb->body.sparse_weight[i].pos]
              =  worb->body.sparse_weight[i].value;
          return extract_initial_term(f,weight,n);
        default:
          error("dp_initial_term : unsupported order");
      }
    default:
      error("dp_initial_term : unsupported order");
  }
  return 0;
}

int highest_order_dp(DP p,int *weight,int n);
LIST highest_order(LIST f,int *weight,int n);

int highest_order_dp(DP p,int *weight,int n)
{
  int w,t,i,top;
  MP m;

  if ( !p ) return -1;
  top = 1;
  for ( m = BDY(p); m; m = NEXT(m) ) {
    for ( i = 0, t = 0; i < n; i++ )  
      t += weight[i]*m->dl->d[i];
    if ( top || t > w ) {
      w = t;
      top = 0;
    }
  }
  return w;
}

LIST highest_order(LIST f,int *weight,int n)
{
  int h;
  NODE nd,r0,r;
  Obj p;
  LIST l;
  Z q;

  nd = BDY(f);
  for ( r0 = 0; nd; nd = NEXT(nd) ) {
    NEXTNODE(r0,r);
    p = (Obj)BDY(nd);
    h = highest_order_dp((DP)p,weight,n);
    STOZ(h,q);
    BDY(r) = (pointer)q;
  }
  if ( r0 ) NEXT(r) = 0;
  MKLIST(l,r0);
  return l;
}

LIST dp_order(LIST f,struct order_spec *ord)
{
  int n,l,i;
  struct weight_or_block *worb;
  int *weight;

  switch ( ord->id ) {
    case 2: /* matrix order */
      /* extract the first row */
      n = ord->nv;
      weight = ord->ord.matrix.matrix[0];
      return highest_order(f,weight,n);
    case 3: /* composite order */
      /* the first w_or_b */
      worb = ord->ord.composite.w_or_b;
      switch ( worb->type ) {
        case IS_DENSE_WEIGHT:
          n = worb->length;
          weight = worb->body.dense_weight;
          return highest_order(f,weight,n);
        case IS_SPARSE_WEIGHT:
          n = ord->nv;
          weight = (int *)ALLOCA(n*sizeof(int));
          for ( i = 0; i < n; i++ ) weight[i] = 0;
          l = worb->length;
          for ( i = 0; i < l; i++ )
            weight[worb->body.sparse_weight[i].pos]
              =  worb->body.sparse_weight[i].value;
          return highest_order(f,weight,n);
        default:
          error("dp_initial_term : unsupported order");
      }
    default:
      error("dp_initial_term : unsupported order");
  }
  return 0;
}

int dpv_ht(DPV p,DP *h)
{
  int len,max,maxi,i,t;
  DP *e;
  MP m,mr;

  len = p->len;
  e = p->body;
  max = -1;
  maxi = -1;
  for ( i = 0; i < len; i++ )
    if ( e[i] && (t = BDY(e[i])->dl->td) > max ) {
      max = t;
      maxi = i;
    }
  if ( max < 0 ) {
    *h = 0;
    return -1;
  } else {
    m = BDY(e[maxi]);
    NEWMP(mr); mr->dl = m->dl; mr->c = (Obj)ONE; NEXT(mr) = 0;
    MKDP(e[maxi]->nv,mr,*h); (*h)->sugar = mr->dl->td;  /* XXX */
    return maxi;
  }
}

/* return 1 if 0 <_w1 v && v <_w2 0 */

int in_c12(int n,int *v,int row1,int **w1,int row2, int **w2)
{
  int t1,t2;

  t1 = compare_zero(n,v,row1,w1);
  t2 = compare_zero(n,v,row2,w2);
  if ( t1 > 0 && t2 < 0 ) return 1;
  else return 0;
}

/* 0 < u => 1, 0 > u => -1 */

int compare_zero(int n,int *u,int row,int **w)
{
  int i,j,t;
  int *wi;

  for ( i = 0; i < row; i++ ) {
    wi = w[i];
    for ( j = 0, t = 0; j < n; j++ ) t += u[j]*wi[j];
    if ( t > 0 ) return 1;
    else if ( t < 0 ) return -1;
  }
  return 0;
}

/* functions for generic groebner walk */
/* u=0 means u=-infty */

int compare_facet_preorder(int n,int *u,int *v,
  int row1,int **w1,int row2,int **w2,int *uv)
{
  int i,j,s,t,tu,tv;
  int *w2i;

  if ( !u ) return 1;
  for ( i = 0; i < row2; i++ ) {
    w2i = w2[i];
    for ( j = 0, tu = tv = 0; j < n; j++ )
      if ( (s = w2i[j]) != 0 ) {
        tu += s*u[j]; tv += s*v[j];
      }
    for ( j = 0; j < n; j++ ) uv[j] = u[j]*tv-v[j]*tu;
    t = compare_zero(n,uv,row1,w1);
    if ( t > 0 ) return 1;
    else if ( t < 0 ) return 0;
  }
  return 1;
}

Q inner_product_with_small_vector(VECT w,int *v)
{
  int n,i;
  Z q;
  Q s,t,u;

  n = w->len;
  s = 0;
  for ( i = 0; i < n; i++ ) {
    STOZ(v[i],q); mulq((Q)w->body[i],(Q)q,&t); addq(t,s,&u); s = u;
  }
  return s;
}

Q compute_last_t(NODE g,NODE gh,Q t,VECT w1,VECT w2,NODE *homo,VECT *wp)
{
  int n,i;
  int *wt;
  Q last,d1,d2,dn,nm,s,t1;
  VECT wd,wt1,wt2,w;
  NODE tg,tgh;
  MP f;
  int *h;
  NODE r0,r;
  MP m0,m;
  DP d;

  n = w1->len;
  wt = W_ALLOC(n);
  last = (Q)ONE;
  /* t1 = 1-t */
  for ( tg = g, tgh = gh; tg; tg = NEXT(tg), tgh = NEXT(tgh ) ) {
    f = BDY((DP)BDY(tg));
    h = BDY((DP)BDY(tgh))->dl->d;
    for ( ; f; f = NEXT(f) ) {
      for ( i = 0; i < n; i++ ) wt[i] = h[i]-f->dl->d[i];
      for ( i = 0; i < n && !wt[i]; i++ );
      if ( i == n ) continue;
      d1 = inner_product_with_small_vector(w1,wt);
      d2 = inner_product_with_small_vector(w2,wt);
      nm = d1; subq(d1,d2,&dn);
      /* if d1=d2 then nothing happens */
      if ( !dn ) continue;
      /* s satisfies ds = 0*/
      divq(nm,dn,&s);

      if ( cmpq(s,t) > 0 && cmpq(s,last) < 0 ) 
        last = s;
      else if ( !cmpq(s,t) ) {
        if ( cmpq(d2,0) < 0 ) {
          last = t;
          break;
        }
      }
    }
  }
  nmq(last,(Z *)&nm);
  dnq(last,(Z *)&dn);
  /* (1-n/d)*w1+n/d*w2 -> w=(d-n)*w1+n*w2 */
  subq(dn,nm,&t1); mulvect(CO,(Obj)w1,(Obj)t1,(Obj *)&wt1); 
  mulvect(CO,(Obj)w2,(Obj)nm,(Obj *)&wt2); addvect(CO,wt1,wt2,&w);

  r0 = 0;
  for ( tg = g, tgh = gh; tg; tg = NEXT(tg), tgh = NEXT(tgh ) ) {
    f = BDY((DP)BDY(tg));
    h = BDY((DP)BDY(tgh))->dl->d;
    for ( m0 = 0; f; f = NEXT(f) ) {
      for ( i = 0; i < n; i++ ) wt[i] = h[i]-f->dl->d[i];
      for ( i = 0; i < n && !wt[i]; i++ );
      if ( !inner_product_with_small_vector(w,wt) ) {
        NEXTMP(m0,m); m->c = f->c; m->dl = f->dl;
      }
    }
    NEXT(m) = 0;
    MKDP(((DP)BDY(tg))->nv,m0,d);  d->sugar = ((DP)BDY(tg))->sugar;
    NEXTNODE(r0,r); BDY(r) = (pointer)d;
  }
  NEXT(r) = 0;
  *homo = r0;
  *wp = w;
  return last;
}

/* return 0 if last_w = infty */

NODE compute_last_w(NODE g,NODE gh,int n,int **w,
  int row1,int **w1,int row2,int **w2)
{
  DP d;
  MP f,m0,m;
  int *wt,*v,*h,*uv;
  NODE t,s,n0,tn,n1,r0,r;
  int i;

  wt = W_ALLOC(n);
  uv = W_ALLOC(n);
  n0 = 0;
  for ( t = g, s = gh; t; t = NEXT(t), s = NEXT(s) ) {
    f = BDY((DP)BDY(t));
    h = BDY((DP)BDY(s))->dl->d;
    for ( ; f; f = NEXT(f) ) {
      for ( i = 0; i < n; i++ ) wt[i] = h[i]-f->dl->d[i];
      for ( i = 0; i < n && !wt[i]; i++ );
      if ( i == n ) continue;

      if ( in_c12(n,wt,row1,w1,row2,w2) && 
        compare_facet_preorder(n,*w,wt,row1,w1,row2,w2,uv) ) {
        v = (int *)MALLOC_ATOMIC(n*sizeof(int));
        for ( i = 0; i < n; i++ ) v[i] = wt[i];
        MKNODE(n1,v,n0); n0 = n1;
      }
    }
  }
  if ( !n0 ) return 0;
  for ( t = n0; t; t = NEXT(t) ) {
    v = (int *)BDY(t);
    for ( s = n0; s; s = NEXT(s) )
      if ( !compare_facet_preorder(n,v,(int *)BDY(s),row1,w1,row2,w2,uv) )
        break;
    if ( !s ) {
      *w = v;
      break;
    }
  }
  if ( !t )
    error("compute_last_w : cannot happen");
  r0 = 0;
  for ( t = g, s = gh; t; t = NEXT(t), s = NEXT(s) ) {
    f = BDY((DP)BDY(t));
    h = BDY((DP)BDY(s))->dl->d;
    for ( m0 = 0; f; f = NEXT(f) ) {
      for ( i = 0; i < n; i++ ) wt[i] = h[i]-f->dl->d[i];
      for ( i = 0; i < n && !wt[i]; i++ );
      if ( i == n  ||
        (compare_facet_preorder(n,wt,*w,row1,w1,row2,w2,uv)
        && compare_facet_preorder(n,*w,wt,row1,w1,row2,w2,uv)) ) {
        NEXTMP(m0,m); m->c = f->c; m->dl = f->dl;
      }
    }
    NEXT(m) = 0;
    MKDP(((DP)BDY(t))->nv,m0,d);  d->sugar = ((DP)BDY(t))->sugar;
    NEXTNODE(r0,r); BDY(r) = (pointer)d;
  }
  NEXT(r) = 0;
  return r0;
}

/* compute a sufficient set of d(f)=u-v */

NODE compute_essential_df(DP *g,DP *gh,int ng)
{
  int nv,i,j,k,t,lj;
  NODE r,r1,ri,rt,r0;
  MP m;
  MP *mj;
  DL di,hj,dl,dlt;
  int *d,*dt;
  LIST l;
  Z q;

  nv = g[0]->nv;
  r = 0;
  for ( j = 0; j < ng; j++ ) {
    for ( m = BDY(g[j]), lj = 0; m; m = NEXT(m), lj++ );
    mj = (MP *)ALLOCA(lj*sizeof(MP));
    for ( m = BDY(g[j]), k = 0; m; m = NEXT(m), k++ )
      mj[k] = m;
    for ( i = 0; i < lj; i++ ) {
      for ( di = mj[i]->dl, k = i+1; k < lj; k++ )
        if ( _dl_redble(di,mj[k]->dl,nv) ) break;
      if ( k < lj ) mj[i] = 0;
    }
    hj = BDY(gh[j])->dl;
    _NEWDL(dl,nv); d = dl->d;
    r0 = r;
    for ( i = 0; i < lj; i++ ) {
      if ( mj[i] && !dl_equal(nv,di=mj[i]->dl,hj) ) {
        for ( k = 0, t = 0; k < nv; k++ ) {
          d[k] = hj->d[k]-di->d[k];
          t += d[k];
        }
        dl->td = t;  
#if 1
        for ( rt = r0; rt; rt = NEXT(rt) ) {
          dlt = (DL)BDY(rt);
#if 1
          if ( _dl_redble(dlt,dl,nv) ) break;
          if ( _dl_redble(dl,dlt,nv) ) {
            BDY(rt) = dl;
            _NEWDL(dl,nv); d = dl->d;
            break;
          }
#else
          if ( dlt->td != dl->td ) continue;
          for ( dt = dlt->d, k = 0; k < nv; k++ )
            if ( d[k] != dt[k] ) break;
          if ( k == nv ) break;
#endif
        }
#else
        rt = 0;
#endif
        if ( !rt ) {
          MKNODE(r1,dl,r); r = r1;
          _NEWDL(dl,nv); d = dl->d;
        }
      }
    }
  }
  for ( rt = r; rt; rt = NEXT(rt) ) {
    dl = (DL)BDY(rt); d = dl->d;
    ri = 0;
    for ( k = nv-1; k >= 0; k-- ) {
      STOZ(d[k],q);
      MKNODE(r1,q,ri); ri = r1;
    }
    MKNODE(r1,0,ri); MKLIST(l,r1);
    BDY(rt) = (pointer)l;
  }
  return r;
}

int comp_bits_divisible(int *a,int *b,int n)
{
  int bpi,i,wi,bi;

  bpi = (sizeof(int)/sizeof(char))*8;
  for ( i = 0; i < n; i++ ) {
    wi = i/bpi; bi = i%bpi;
    if ( !(a[wi]&(1<<bi)) && (b[wi]&(1<<bi)) ) return 0;
  }
  return 1;
}

int comp_bits_lex(int *a,int *b,int n)
{
  int bpi,i,wi,ba,bb,bi;

  bpi = (sizeof(int)/sizeof(char))*8;
  for ( i = 0; i < n; i++ ) {
    wi = i/bpi; bi = i%bpi;
    ba = (a[wi]&(1<<bi))?1:0;
    bb = (b[wi]&(1<<bi))?1:0;
    if ( ba > bb ) return 1;
    else if ( ba < bb ) return -1;
  }
  return 0;
}

NODE mono_raddec(NODE ideal)
{
  DP p;
  int nv,w,i,bpi,di,c,len;
  int *d,*s,*u,*new;
  NODE t,t1,v,r,rem,prev;

  if( !ideal ) return 0;
  p = (DP)BDY(ideal);
  nv = NV(p);
  bpi = (sizeof(int)/sizeof(char))*8;
  w = (nv+(bpi-1))/bpi;
  d = p->body->dl->d;
  if ( !NEXT(ideal) )  {
    for ( t = 0, i = nv-1; i >= 0; i-- ) {
      if ( d[i] ) {
        s = (int *)CALLOC(w,sizeof(int));
        s[i/bpi] |= 1<<(i%bpi);
        MKNODE(t1,s,t);
        t = t1;
      }
    }
    return t;
  }
  rem = mono_raddec(NEXT(ideal));
  r = 0;
  len = w*sizeof(int);
  u = (int *)CALLOC(w,sizeof(int));
  for ( i = nv-1; i >= 0; i-- ) {
    if ( d[i] ) {
      for ( t = rem; t; t = NEXT(t) ) {
        bcopy((char *)BDY(t),(char *)u,len);
        u[i/bpi] |= 1<<(i%bpi);
        for ( v = r; v; v = NEXT(v) ) {
          if ( comp_bits_divisible(u,(int *)BDY(v),nv) ) break;
        }
        if ( v ) continue;
        for ( v = r, prev = 0; v; v = NEXT(v) ) {
          if ( comp_bits_divisible((int *)BDY(v),u,nv) ) {
            if ( prev ) NEXT(prev) = NEXT(v);
            else r = NEXT(r); 
          } else prev =v;
        }
        for ( v = r, prev = 0; v; prev = v, v = NEXT(v) ) {
          if ( comp_bits_lex(u,(int *)BDY(v),nv) < 0 ) break;
        }
        new = (int *)CALLOC(w,sizeof(int));
        bcopy((char *)u,(char *)new,len);
        MKNODE(t1,new,v);
        if ( prev ) NEXT(prev) = t1;
        else r = t1;
      }
    }
  }
  return r;
}
