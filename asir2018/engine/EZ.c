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
 * $OpenXM: OpenXM_contrib2/asir2018/engine/EZ.c,v 1.1 2018/09/19 05:45:06 noro Exp $
*/
#include "ca.h"

void ezgcdp(VL vl,P p1,P p2,P *pr)
{
  P f1,f2;
  Q c;

  if ( !p1 )
    if ( !p2 )
      *pr = 0;
    else
      *pr = p2;
  else if ( !p2 )
    *pr = p1;
  else {
    if ( !qpcheck((Obj)p1) || !qpcheck((Obj)p2) )
      error("ezgcdp : invalid argument");
    ptozp(p1,1,&c,&f1);
    ptozp(p2,1,&c,&f2);
    ezgcdpz(vl,f1,f2,pr);
  }
}

/*
 * p1, p2 : integer coefficient 
 * returns gcd(pp(p1),pp(p2)) * gcd(cont(p1),cont(p2))
 */

void ezgcdpz(VL vl,P p1,P p2,P *pr)
{
  P f1,f2,t,s,g,gcd;
  P fc1,fc2,cont;
  VL tvl,svl;
  V v1,v2;
  DCP dc,dc1,dc2;
  Z c1,c2,c;
  P g1,g2;
  P mgcd;
  extern int nez;
  P pm[2];

  if ( nez ) {
    pm[0] = p1; pm[1] = p2; nezgcdnpz(vl,pm,2,pr); return;
  }
  monomialfctr(vl,p1,&f1,&dc1); p1 = f1;
  monomialfctr(vl,p2,&f2,&dc2); p2 = f2;
  for ( mgcd = (P)ONE; dc1; dc1 = NEXT(dc1) )
    for ( v1 = VR(dc1->c), dc = dc2; dc; dc = NEXT(dc) )
      if ( v1 == VR(dc->c) ) {
        pwrp(vl,dc->c,cmpz(dc1->d,dc->d)>=0?dc->d:dc1->d,&t);
        mulp(vl,mgcd,t,&s); mgcd = s; break;
      }

  if ( NUM(p1) ) {
    if ( NUM(p2) ) {
      gcdz((Z)p1,(Z)p2,&c);
    } else {
      ptozp(p2,1,(Q *)&c2,&t);
      gcdz((Z)p1,c2,&c);
    }
    mulp(vl,(P)c,mgcd,pr);
    return;
  } else if ( NUM(p2) ) {
    ptozp(p1,1,(Q *)&c1,&t);
    gcdz(c1,(Z)p2,&c);
    mulp(vl,(P)c,mgcd,pr);
    return;
  }

  ptozp(p1,1,(Q *)&c1,&g1); ptozp(p2,1,(Q *)&c2,&g2);
  gcdz(c1,c2,&c);

  if ( ( v1 = VR(f1) ) != ( v2 = VR(f2) ) ) {
    while ( v1 != VR(vl) && v2 != VR(vl) )
      vl = NEXT(vl);
    if ( v1 == VR(vl) ) {
      pcp(vl,f1,&g,&fc1);
      ezgcdpz(vl,fc1,f2,&t);
    } else {
      pcp(vl,f2,&g,&fc2);
      ezgcdpz(vl,fc2,f1,&t);
    }
    mulp(vl,t,(P)c,&s); mulp(vl,s,mgcd,pr);
    return;
  }

  pcp(vl,f1,&g1,&fc1); pcp(vl,f2,&g2,&fc2);
  ezgcdpz(vl,fc1,fc2,&cont);
  clctv(vl,g1,&tvl); clctv(vl,g2,&svl);
  if ( !NEXT(tvl) && !NEXT(svl) ) {
    uezgcdpz(vl,g1,g2,&t);
    mulp(vl,t,cont,&s); mulp(vl,s,(P)c,&t); mulp(vl,t,mgcd,pr);
    return;
  }

  if ( homdeg(g1) > homdeg(g2) ) {
    t = g1; g1 = g2; g2 = t;
  }
  sqfrp(vl,g1,&dc);
  for ( g = g2, gcd = (P)ONE; dc && !NUM(g); dc = NEXT(dc) ) {
    if ( NUM(COEF(dc)) )
      continue;
    ezgcdpp(vl,dc,g,&t); 
    if ( NUM(t) ) 
      continue;
    mulp(vl,gcd,t,&s); gcd = s;
    divsp(vl,g,t,&s); g = s;
  }
  mulp(vl,gcd,cont,&t); mulp(vl,t,(P)c,&s); mulp(vl,s,mgcd,pr);
}

void uezgcdpz(VL vl,P p1,P p2,P *pr)
{
  P g1,g2,gcd,t,s,g;
  DCP dc;
  Z c1,c2,cq;
  P f1,f2;

  if ( NUM(p1) ) {
    if ( NUM(p2) ) {
      gcdz((Z)p1,(Z)p2,&cq); *pr = (P)cq;
      return;
    } else {
      ptozp(p2,1,(Q *)&c2,&t);
      gcdz((Z)p1,c2,&cq); *pr = (P)cq;
      return;
    }
  } else if ( NUM(p2) ) {
    ptozp(p1,1,(Q *)&c1,&t);
    gcdz(c1,(Z)p2,&cq); *pr = (P)cq;
    return;
  }

  ptozp(p1,1,(Q *)&c1,&f1); ptozp(p2,1,(Q *)&c2,&f2);
  gcdz(c1,c2,&cq);
  if ( UDEG(f1) > UDEG(f2) ) {
    g1 = f2; g2 = f1;
  } else {
    g1 = f1; g2 = f2;
  }
  usqp(g1,&dc);
  for ( g = g2, gcd = (P)ONE; dc && !NUM(g); dc = NEXT(dc) ) {
    if ( NUM(COEF(dc)) )
      continue;
    uezgcdpp(dc,g,&t); 
    if ( NUM(t) ) 
      continue;
    mulp(CO,gcd,t,&s); gcd = s;
    divsp(CO,g,t,&s); g = s;
  }
  mulp(vl,gcd,(P)cq,pr);
}

/*
  dc : dc pair c^d
  r = gcd(c^d,f)
*/

void ezgcdpp(VL vl,DCP dc,P f,P *r)
{
  int k;
  P g,h,t,s,gcd;

  if ( NUM(f) ) {
    *r = (P)ONE;
    return;
  }
  k = ZTOS(DEG(dc)) - 1;
/*  ezgcd1p(vl,COEF(dc),f,&gcd); */
  ezgcdnp(vl,COEF(dc),&f,1,&gcd);
  g = gcd; divsp(vl,f,gcd,&h);
  for ( ; k > 0; k-- ) {
/*    ezgcd1p(vl,g,h,&t); */
    ezgcdnp(vl,g,&h,1,&t);
    if ( NUM(t) )
      break;
    mulp(vl,gcd,t,&s); gcd = s;
    divsp(vl,h,t,&s); h = s;
  }
  *r = gcd;
}

void ezgcd1p(VL vl,P p0,P p1,P *gcdp)
{
  VL nvl,tvl,vl0,vl1,vl2;
  P f0,f1,t;

  clctv(vl,p0,&vl0); clctv(vl,p1,&vl1); mergev(vl,vl0,vl1,&vl2);
  minchdegp(vl,p0,&tvl,&t); reordvar(vl2,tvl->v,&nvl);
  reorderp(nvl,vl,p0,&f0); reorderp(nvl,vl,p1,&f1);
  ezgcdnp(nvl,f0,&f1,1,&t);
  reorderp(vl,nvl,t,gcdp);
}

void uezgcdpp(DCP dc,P f,P *r)
{
  int k;
  P g,h,t,s,gcd;

  if ( NUM(f) ) {
    *r = (P)ONE;
    return;
  }

  k = ZTOS(DEG(dc)) - 1;
  uezgcd1p(COEF(dc),f,&gcd);
  g = gcd; divsp(CO,f,gcd,&h);
  for ( ; k > 0; k-- ) {
    uezgcd1p(g,h,&t);
    if ( NUM(t) )
      break;
    mulp(CO,gcd,t,&s); gcd = s;
    divsp(CO,h,t,&s); h = s;
  }
  *r = gcd;
}

/*
 * pr = gcd(p0,ps[0],...,ps[m-1])
 *
 * p0 is already square-free and primitive.
 * ps[i] is at least primitive.
 *
 */

void ezgcdnp(VL vl,P p0,P *ps,int m,P *pr)
{
  /* variables */
  P p00,g,h,g0,h0,a0,b0;
  P lp0,lgp0,lp00,lg,lg0,t;
  Q cbd;
  Z tq;
  P *tps;
  VL nvl1,nvl2,nvl,tvl;
  V v;
  int i,j,k,d0,dg,dg0,dmin;
  VN vn0,vn1,vnt;
  int nv,flag;

  /* set-up */
  if ( NUM(p0) ) {
    *pr = (P) ONE;
    return;
  }
  for ( v = VR(p0), i = 0; i < m; i++ )
    if ( NUM(ps[i]) || (v != VR(ps[i])) ) {
      *pr = (P)ONE;
      return;
    }
  tps = (P *) ALLOCA(m*sizeof(P));
  for ( i = 0; i < m; i++ )
    tps[i] = ps[i];
  sortplist(tps,m);  
  /* deg(tps[0]) <= deg(tps[1]) <= ... */
  
  if ( !cmpz(DEG(DC(p0)),ONE) ) {
    if ( divcheck(vl,tps,m,(P)ONE,p0) )
      *pr = p0;
    else
      *pr = (P)ONE;
    return;
  }

  lp0 = LC(p0); lg = lp0; dmin = d0 = deg(v,p0);
  clctv(vl,p0,&nvl);
  for ( i = 0; i < m; i++ ) {
    clctv(vl,tps[i],&nvl1); mergev(vl,nvl,nvl1,&nvl2);
    nvl = nvl2;
    ezgcdpz(nvl,lg,LC(tps[i]),&t); lg = t;
  }

  mulp(nvl,p0,lg,&lgp0);
  k = dbound(v,lgp0) + 1;
  cbound(nvl,lgp0,(Q *)&cbd);
  for ( nv = 0, tvl = nvl; tvl; tvl = NEXT(tvl), nv++ );
  W_CALLOC(nv,struct oVN,vn0); W_CALLOC(nv,struct oVN,vnt);
  W_CALLOC(nv,struct oVN,vn1);
  for ( i = 0, tvl = NEXT(nvl); tvl; tvl = NEXT(tvl), i++ )
    vn1[i].v = vn0[i].v = tvl->v;

  /* main loop */
  for ( dg = deg(v,tps[0]) + 1; ; next(vn0) )
    do {
      for ( i = 0, j = 0; vn0[i].v; i++ ) 
        if ( vn0[i].n ) vnt[j++].v = (V)((unsigned long)i);
      vnt[j].n = 0;

      /* find b s.t. LC(p0)(b), LC(tps[i])(b) != 0 */
      mulsgn(vn0,vnt,j,vn1);
      substvp(nvl,p0,vn1,&p00);
      flag = (!zerovpchk(nvl,lp0,vn1) && sqfrchk(p00));
      for ( i = 0; flag && (i < m); i++ )
        flag &= (!zerovpchk(nvl,LC(tps[i]),vn1));
      if ( !flag ) 
        continue;

      /* substitute y -> b */
      substvp(nvl,lg,vn1,&lg0);
      lp00 = LC(p00);
      /* extended-gcd in 1-variable */
      uexgcdnp(nvl,p00,tps,m,vn1,(Q)cbd,&g0,&h0,&a0,&b0,&tq);
      if ( NUM(g0) ) {
        *pr = (P)ONE;
        return;
      } else if ( dg > ( dg0 = deg(v,g0) ) ) {
        dg = dg0;
        if ( dg == d0 ) {
          if ( divcheck(nvl,tps,m,lp0,p0) ) {
            *pr = p0;
             return;
          }
        } else if ( dg == deg(v,tps[0]) ) {
          if ( divtpz(nvl,p0,tps[0],&t) &&
            divcheck(nvl,tps+1,m-1,LC(tps[0]),tps[0]) ) {
            *pr = tps[0];
             return;
          } else
            break;
        } else {
          henmv(nvl,vn1,lgp0,g0,h0,a0,b0,
            lg,lp0,lg0,lp00,tq,k,&g,&h);
          if ( divtpz(nvl,lgp0,g,&t) &&
            divcheck(nvl,tps,m,lg,g) ) {
            pcp(nvl,g,pr,&t);
            return;
          }  
        }
      }
    } while ( !nextbin(vnt,j) );
}

/*
  f : sqfr
*/

void uezgcd1p(P f,P g,P *r)
{
  UM wf,wf1,wf2,wg,wg1,wgcd,wcof;
  int d1,d2,dg,m,index,n;
  ML list;
  DCP dc;
  P t;
  Q c;

  if ( NUM(f) || NUM(g) ) {
    *r = (P)ONE;
    return;
  }
  ptozp(f,1,&c,&t); f = t; ptozp(g,1,&c,&t); g = t;
  d1 = UDEG(f); d2 = UDEG(g);
  n = MAX(d1,d2)+1;
  wf = W_UMALLOC(n); wf1 = W_UMALLOC(n);
  wf2 = W_UMALLOC(n); wg = W_UMALLOC(n);
  wg1 = W_UMALLOC(n); wgcd = W_UMALLOC(n);
  wcof = W_UMALLOC(n);

  for ( index = INDEX, dg = MIN(d1,d2)+1; ; ) {
    m = sprime[index++];
    if ( !remqi((Q)UCOEF(f),m) || !remqi((Q)UCOEF(g),m)) 
      continue;
    ptoum(m,f,wf); cpyum(wf,wf1);
    diffum(m,wf1,wf2); gcdum(m,wf1,wf2,wgcd);
    if ( DEG(wgcd) > 0 ) 
      continue;
    ptoum(m,g,wg); cpyum(wg,wg1); cpyum(wf,wf1);
    gcdum(m,wf1,wg1,wgcd);
    if ( dg <= DEG(wgcd) )
      continue;
    else
      dg = DEG(wgcd);
    if ( dg == 0 ) {
      *r = (P)ONE;
      return;
    } else if ( dg == d1 ) {
      if ( divtpz(CO,g,f,&t) ) {
        *r = f;
        return;
      }
    } else if ( dg == d2 ) {
      if ( divtpz(CO,f,g,&t) ) {
        *r = g;
        return; 
      } 
    } else {
      divum(m,wf,wgcd,wcof);
      ezgcdhensel(f,m,wcof,wgcd,&list);
      dtest(f,list,1,&dc);
      if ( NEXT(dc) ) {
        if ( divtpz(CO,g,COEF(dc),&t) ) {
          *r = COEF(dc);
          return;
        }
      }
    }
  }
}
  
void uexgcdnp(VL vl,P p1,P *ps,int n,VN vn,Q cbd,P *g,P *h,P *a,P *b,Z *q)
{
  UM wg,wh,wg1,wh1,wgcd,wt;
  P t,s,gcd,cof,gk,hk,ak,bk;
  Q c,bb;
  Z tq,tq1,qk;
  int mod,k,i,index,d;

  ptozp(p1,1,&c,&gcd);
  for ( i = 0; i < n; i++ ) {
    substvp(vl,ps[i],vn,&t);
    uezgcd1p(gcd,t,&s);
    if ( NUM(s) ) {
      *g = (P)ONE; *h = p1; *a = 0; *b = (P)ONE;
      return;
    } else
      gcd = s;
  }

  if ( !dcomp(p1,gcd) ) {
    *g = p1; *h = (P)ONE; *a = 0; *b = (P)ONE;
    return;
  }
    
  divsp(CO,p1,gcd,&cof);
  d = UDEG(p1)+1;
  wg = W_UMALLOC(d); wh = W_UMALLOC(d);
  wg1 = W_UMALLOC(d); wh1 = W_UMALLOC(d);
  wgcd = W_UMALLOC(d); wt = W_UMALLOC(d);
  for ( index = INDEX; ; ) {
    mod = sprime[index++];
    if ( !remqi((Q)LC(p1),mod) )
      continue;
    ptoum(mod,gcd,wg); ptoum(mod,cof,wh);
    cpyum(wg,wg1); cpyum(wh,wh1);
    gcdum(mod,wg,wh,wt); cpyum(wt,wgcd);
    if ( DEG(wgcd) > 0 )
      continue;
    STOZ(mod,tq); addq(cbd,cbd,&bb);
    for ( k = 1; cmpq((Q)tq,bb) < 0; k++ ) {
      mulz(tq,tq,&tq1); tq = tq1;
    }
    henzq(p1,gcd,wg1,cof,wh1,mod,k,&gk,&hk,&bk,&ak,&qk);
    break;
  }
  *g = gk; *h = hk; *a = ak; *b = bk; *q = tq;
}

void ezgcdhensel(P f,int mod,UM cof,UM gcd,ML *listp)
{
  register int i,j;
  int q,n,bound;
  int *p;
  int **pp;
  ML blist,clist,bqlist,cqlist,rlist;
  UM *b;
  LUM fl,tl;
  LUM *l;
  LUM LUMALLOC();
  
  blist = W_MLALLOC(2);
  blist->n = 2; blist->mod = mod;
  blist->c[0] = (pointer)cof; blist->c[1] = (pointer)gcd;
  gcdgen(f,blist,&clist);
  henprep(f,blist,clist,&bqlist,&cqlist);
  n = bqlist->n; q = bqlist->mod;
  bqlist->bound = cqlist->bound = bound = mignotte(q,f);

  if ( bound == 1 ) {
    *listp = rlist = MLALLOC(n);
    rlist->n = n;
    rlist->mod = q;
    rlist->bound = bound;

    for ( i = 0, b = (UM *)bqlist->c, l = (LUM *)rlist->c; 
      i < n; i++ ) {
      tl = LUMALLOC(DEG(b[i]),1);
      l[i] = tl;
      p = COEF(b[i]);

      for ( j = 0, pp = COEF(tl); 
          j <= DEG(tl); j++ )
          pp[j][0] = p[j];
    }
  } else {
    W_LUMALLOC((int)UDEG(f),bound,fl);
    ptolum(q,bound,f,fl);
    henmain(fl,bqlist,cqlist,listp);
  }
}

void ezgcdnpz(VL vl,P *ps,int m,P *pr)
{
  P t,s,gcd;
  P cont;
  VL tvl,svl,nvl;
  int i;
  DCP dc;
  Z cq;
  P *pl,*ppl,*pcl;
  Z *cl;
  V v;

  pl = (P *)ALLOCA((m+1)*sizeof(P));
  cl = (Z *)ALLOCA((m+1)*sizeof(Q));
  for ( i = 0; i < m; i++ ) 
    ptozp(ps[i],1,(Q *)&cl[i],&pl[i]); 
  for ( i = 1, cq = cl[0]; i < m; i++ ) {
    gcdz(cl[i],cq,&cq);
  }
  for ( i = 0; i < m; i++ ) 
    if ( NUM(pl[i]) ) {
      *pr = (P)cq;
      return;
    }

  for ( i = 0, nvl = 0; i < m; i++ ) {
    clctv(vl,ps[i],&tvl); mergev(vl,nvl,tvl,&svl); nvl = svl;
  }

  ppl = (P *)ALLOCA((m+1)*sizeof(P));
  pcl = (P *)ALLOCA((m+1)*sizeof(P));
  for ( i = 0, v = nvl->v; i < m; i++ )
    if ( v == VR(pl[i]) )
      pcp(nvl,pl[i],&ppl[i],&pcl[i]);
    else {
      ppl[i] = (P)ONE; pcl[i] = pl[i];
    }
  ezgcdnpz(nvl,pcl,m,&cont);            

  for ( i = 0; i < m; i++ ) 
    if ( NUM(ppl[i]) ) {
      mulp(nvl,cont,(P)cq,pr);
      return;
    }
  sortplist(ppl,m);
  sqfrp(nvl,ppl[0],&dc);
  for ( dc = NEXT(dc), gcd = (P)ONE; dc; dc = NEXT(dc) ) {
    if ( NUM(COEF(dc)) )
      continue;
    ezgcdnpp(vl,dc,ppl+1,m-1,&t); 
    if ( NUM(t) ) 
      continue;
    mulp(vl,gcd,t,&s); gcd = s;
  }
  mulp(vl,gcd,cont,&t); mulp(vl,t,(P)cq,pr);
}

void ezgcdnpp(VL vl,DCP dc,P *pl,int m,P *r)
{
  int i,k;
  P g,t,s,gcd;
  P *pm;

  ezgcdnp(vl,COEF(dc),pl,m,&gcd);
  if ( NUM(gcd) ) {
    *r = (P)ONE;
    return;
  }
  pm = (P *) ALLOCA((m+1)*sizeof(P));
  for ( i = 0; i < m; i++ ) {
    divsp(vl,pl[i],gcd,&pm[i]);
    if ( NUM(pm[i]) ) {
      *r = gcd;
      return;
    }
  }
  for ( g = gcd, k = ZTOS(DEG(dc)) - 1; k > 0; k-- ) {
    ezgcdnp(vl,g,pm,m,&t);
    if ( NUM(t) )
      break;
    mulp(vl,gcd,t,&s); gcd = s;
    for ( i = 0; i < m; i++ ) {
      divsp(vl,pm[i],t,&s);
      if ( NUM(s) ) {
        *r = gcd;
        return;
      }
      pm[i] = s;
    }
  }
  *r = gcd;
}

void pcp(VL vl,P p,P *pp,P *c)
{
  P f,g,n;
  DCP dc;
  P *pl;
  int i,m;
  extern int nez;

  if ( NUM(p) ) {
    *c = p;
    *pp = (P)ONE;
  } else {
    for ( m = 0, dc = DC(p); dc; dc = NEXT(dc), m++ );
    if ( m == 1 ) {
      *c = COEF(DC(p));
      divsp(vl,p,*c,pp);
      return;
    }
    ptozp(p,1,(Q *)&n,&f);
    pl = (P *)ALLOCA((m+1)*sizeof(P));
    for ( i = 0, dc = DC(f); i < m; dc = NEXT(dc), i++ )
      pl[i] = COEF(dc);
    if ( nez )
      nezgcdnpz(vl,pl,m,&g);  
    else
      ezgcdnpz(vl,pl,m,&g);  
    mulp(vl,g,n,c); divsp(vl,f,g,pp);
  }
}

int divcheck(VL vl,P *ps,int m,P l,P p)
{
  int i;
  P r,t;

  for ( i = 0; i < m; i++ ) {
    mulp(vl,ps[i],l,&t);
    if ( !divtpz(vl,t,p,&r) )
      return ( 0 );
  }
  return ( 1 );
}

void sortplist(P *plist,int n)
{
  int i,j;
  P t;

  for ( i = 0; i < n; i++ )
    for ( j = i + 1; j < n; j++ )
      if ( cmpz(DEG(DC(plist[i])),DEG(DC(plist[j]))) > 0 ) {
        t = plist[i]; plist[i] = plist[j]; plist[j] = t;
      }
}

int dcomp(P p1,P p2)
{
  return ( cmpz(DEG(DC(p1)),DEG(DC(p2))) );
}
