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
 * $OpenXM: OpenXM_contrib2/asir2018/engine/NEZ.c,v 1.1 2018/09/19 05:45:07 noro Exp $
*/
#include "ca.h"

void nezgcdnpz(vl,ps,m,pr)
VL vl;
P *ps,*pr;
int m;
{
  P t,s,mg;
  VL tvl,svl,avl,nvl;
  int i,j,k;
  Z cq,cq1;
  P *pl,*pm;
  DCP *ml;
  Z *cl;
  P **cp;
  int *cn;

  if ( m == 1 ) {
    *pr = ps[0]; return;
  }
  pl = (P *)ALLOCA(m*sizeof(P)); 
  ml = (DCP *)ALLOCA(m*sizeof(DCP)); 
  cl = (Z *)ALLOCA(m*sizeof(Z));
  for ( i = 0; i < m; i++ )
    monomialfctr(vl,ps[i],&pl[i],&ml[i]);
  gcdmonomial(vl,ml,m,&mg);
  for ( i = 0; i < m; i++ ) {
    ptozp(pl[i],1,(Q *)&cl[i],&t); pl[i] = t;
  }
  for ( i = 1, cq = cl[0]; i < m; i++ ) {
    gcdz(cl[i],cq,&cq1); cq = cq1;
  }
  for ( i = 0; i < m; i++ ) 
    if ( NUM(pl[i]) ) {
      mulp(vl,(P)cq,mg,pr); return;
    }
  for ( i = 0, nvl = vl, avl = 0; nvl && i < m; i++ ) {
    clctv(vl,pl[i],&tvl);
    intersectv(nvl,tvl,&svl); nvl = svl;
    mergev(vl,avl,tvl,&svl); avl = svl;
  }
  if ( !nvl ) {
    mulp(vl,(P)cq,mg,pr); return;
  }
  if ( !NEXT(avl) ) {
    nuezgcdnpzmain(vl,pl,m,&t); mulp(vl,t,(P)cq,&s); mulp(vl,s,mg,pr);
    return;
  }
  for ( tvl = nvl, i = 0; tvl; tvl = NEXT(tvl), i++ );
  for ( tvl = avl, j = 0; tvl; tvl = NEXT(tvl), j++ );
  if ( i == j ) {
    /* all the pl[i]'s have the same variables */
    sortplistbyhomdeg(pl,m); nezgcdnpzmain(nvl,pl,m,&t);
#if 0
    /* search the minimal degree poly */
    for ( i = 0; i < m; i++ ) {
          for ( tvl = nvl; tvl; tvl = NEXT(tvl) ) {
                dt = getdeg(tvl->v,pl[i]);
                if ( tvl == nvl || dt < d1 ) {
                        v1 = tvl->v; d1 = dt;
                }
          }
      if ( i == 0 || d1 < d ) {
        v = v1; d = d1; j = i;
      }
    }
    t = pl[0]; pl[0] = pl[j]; pl[j] = t;
    if ( v != nvl->v ) {
      reordvar(nvl,v,&mvl);
      for ( i = 0; i < m; i++ ) {
        reorderp(mvl,nvl,pl[i],&t); pl[i] = t;
      }
      nezgcdnpzmain(mvl,pl,m,&s); reorderp(nvl,mvl,s,&t);
    } else
      nezgcdnpzmain(nvl,pl,m,&t);
#endif
  } else {
    cp = (P **)ALLOCA(m*sizeof(P *));
    cn = (int *)ALLOCA(m*sizeof(int));
    for ( i = 0; i < m; i++ ) {
      cp[i] = (P *)ALLOCA(lengthp(pl[i])*sizeof(P));
      cn[i] = pcoef(vl,nvl,pl[i],cp[i]);
    }
    for ( i = j = 0; i < m; i++ )
      j += cn[i];
    pm = (P *)ALLOCA(j*sizeof(P));
    for ( i = k = 0; i < m; i++ )
      for ( j = 0; j < cn[i]; j++ )
        pm[k++] = cp[i][j];
    nezgcdnpz(vl,pm,k,&t);            
  }
  mulp(vl,t,(P)cq,&s); mulp(vl,s,mg,pr);
}

void sortplistbyhomdeg(p,n)
P *p;
int n;
{
  int i,j,k;
  int *l;
  P t;

  l = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0; i < n; i++ )
    l[i] = homdeg(p[i]);
  for ( i = 0; i < n; i++ )
    for ( j = i + 1; j < n; j++ )
      if ( l[j] < l[i] ) {
        t = p[i]; p[i] = p[j]; p[j] = t;
        k = l[i]; l[i] = l[j]; l[j] = k;
      }
}

void nuezgcdnpzmain(vl,ps,m,r)
VL vl;
P *ps;
int m;
P *r;
{
  P *tps;
  P f,t;
  int i;

  for ( i = 0; i < m; i++ )
    if ( NUM(ps[i]) ) {
      *r = (P)ONE; return;
    }
  tps = (P *) ALLOCA(m*sizeof(P));
  for ( i = 0; i < m; i++ )
    tps[i] = ps[i];
  sortplist(tps,m);  
  for ( i = 1, f = tps[0]; i < m && !NUM(f); i++ ) {
    uezgcdpz(vl,f,tps[i],&t); f = t;
  }
  *r = f;
}

void gcdmonomial(vl,dcl,m,r)
VL vl;
DCP *dcl;
int m;
P *r;
{
  int i,j,n;
  P g,x,s,t;
  Z d;
  DCP dc;
  VN vn;

  for ( i = 0; i < m; i++ ) 
    if ( !dcl[i] ) {
      *r = (P)ONE; return;
    }
  for ( n = 0, dc = dcl[0]; dc; dc = NEXT(dc), n++ );
  vn = (VN)ALLOCA(n*sizeof(struct oVN));
  for ( i = 0, dc = dcl[0]; i < n; dc = NEXT(dc), i++ ) {
    vn[i].v = VR(COEF(dc)); vn[i].n = ZTOS(DEG(dc));
  }
  for ( i = 1; i < m; i++ ) {
    for ( j = 0; j < n; j++ ) {
      for ( dc = dcl[i]; dc; dc = NEXT(dc) )
        if ( VR(COEF(dc)) == vn[j].v ) {
          vn[j].n = MIN(vn[j].n,ZTOS(DEG(dc))); break;
        }
      if ( !dc )
        vn[j].n = 0;
    }
    for ( j = n-1; j >= 0 && !vn[j].n; j-- );
    if ( j < 0 ) {
      *r = (P)ONE; return;
    } else
      n = j+1;
  }
  for ( j = 0, g = (P)ONE; j < n; j++ )
    if ( vn[j].n ) {
      MKV(vn[j].v,x); STOZ(vn[j].n,d); 
      pwrp(vl,x,d,&t); mulp(vl,t,g,&s); g = s;
    }
  *r = g;
}

void nezgcdnpzmain(vl,pl,m,pr)
VL vl;
P *pl,*pr;
int m;
{
  P *ppl,*pcl;
  int i;
  P cont,gcd,t,s;
  DCP dc;
  
  ppl = (P *)ALLOCA(m*sizeof(P));
  pcl = (P *)ALLOCA(m*sizeof(P));
  for ( i = 0; i < m; i++ )
    pcp(vl,pl[i],&ppl[i],&pcl[i]);
  nezgcdnpz(vl,pcl,m,&cont);            
  sqfrp(vl,ppl[0],&dc);
  for ( dc = NEXT(dc), gcd = (P)ONE; dc; dc = NEXT(dc) ) {
    if ( NUM(COEF(dc)) )
      continue;
    nezgcdnpp(vl,dc,ppl+1,m-1,&t);
    if ( NUM(t) ) 
      continue;
    mulp(vl,gcd,t,&s); gcd = s;
  }
  mulp(vl,gcd,cont,pr);
}

void nezgcdnpp(vl,dc,pl,m,r)
VL vl;
DCP dc;
P *pl;
int m;
P *r;
{
  int i,k;
  P g,t,s,gcd;
  P *pm;

  nezgcdnp_sqfr_primitive(vl,COEF(dc),pl,m,&gcd);
  if ( NUM(gcd) ) {
    *r = (P)ONE; return;
  }
  pm = (P *) ALLOCA(m*sizeof(P));
  for ( i = 0; i < m; i++ ) {
    divsp(vl,pl[i],gcd,&pm[i]);
    if ( NUM(pm[i]) ) {
      *r = gcd; return;
    }
  }
  for ( g = gcd, k = ZTOS(DEG(dc))-1; k > 0; k-- ) {
    nezgcdnp_sqfr_primitive(vl,g,pm,m,&t);
    if ( NUM(t) )
      break;
    mulp(vl,gcd,t,&s); gcd = s;
    for ( i = 0; i < m; i++ ) {
      divsp(vl,pm[i],t,&s);
      if ( NUM(s) ) {
        *r = gcd; return;
      }
      pm[i] = s;
    }
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

void nezgcdnp_sqfr_primitive(vl,p0,ps,m,pr)
VL vl;
int m;
P p0,*ps,*pr;
{
  /* variables */
  P p00,g,h,g0,h0,a0,b0;
  P lp0,lgp0,lp00,lg,lg0,cbd,tq,t;
  P *lc;
  P *tps;
  VL nvl1,nvl2,nvl,tvl;
  V v;
  int i,j,k,d0,dg,dg0,dmin,z;
  VN vn1;
  int nv,flag,max;

  /* set-up */
  if ( NUM(p0) ) {
    *pr = (P) ONE; return;
  }
  for ( v = VR(p0), i = 0; i < m; i++ )
    if ( NUM(ps[i]) || (v != VR(ps[i])) ) {
      *pr = (P)ONE; return;
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

  lp0 = LC(p0); dmin = d0 = deg(v,p0); lc = (P *)ALLOCA((m+1)*sizeof(P));
  for ( lc[0] = lp0, i = 0; i < m; i++ )
    lc[i+1] = LC(tps[i]);
  clctv(vl,p0,&nvl);
  for ( i = 0; i < m; i++ ) {
    clctv(vl,tps[i],&nvl1); mergev(vl,nvl,nvl1,&nvl2); nvl = nvl2;
  }
  nezgcdnpz(nvl,lc,m+1,&lg);

  mulp(nvl,p0,lg,&lgp0); k = dbound(v,lgp0)+1; cbound(nvl,lgp0,(Q *)&cbd);
  for ( nv = 0, tvl = nvl; tvl; tvl = NEXT(tvl), nv++ );
  W_CALLOC(nv,struct oVN,vn1);
  for ( i = 0, tvl = NEXT(nvl); tvl; tvl = NEXT(tvl), i++ )
    vn1[i].v = tvl->v;

  /* main loop */
  /* finally, 'max' random evaluations will be generated */
  for ( max = 1, dg = deg(v,tps[0]) + 1; ; max = 2*max )
    for ( z = 0; z < max; z++ ) {
      for ( i = 0; vn1[i].v; i++ )
        vn1[i].n = mt_genrand()%max;

      /* find b s.t. LC(p0)(b), LC(tps[i])(b) != 0 */
      
      substvp(nvl,p0,vn1,&p00);
      flag = (!zerovpchk(nvl,lp0,vn1) && sqfrchk(p00));
      for ( i = 0; flag && (i < m); i++ )
        flag &= (!zerovpchk(nvl,LC(tps[i]),vn1));
      if ( !flag ) 
        continue;

      /* substitute y -> b */
      substvp(nvl,lg,vn1,&lg0); lp00 = LC(p00);
      /* extended-gcd in 1-variable */
      uexgcdnp(nvl,p00,tps,m,vn1,(Q)cbd,&g0,&h0,&a0,&b0,(Z *)&tq);
      if ( NUM(g0) ) {
        *pr = (P)ONE; return;
      } else if ( dg > ( dg0 = deg(v,g0) ) ) {
        dg = dg0;
        if ( dg == d0 ) {
          if ( divcheck(nvl,tps,m,lp0,p0) ) {
            *pr = p0; return;
          }
        } else if ( dg == deg(v,tps[0]) ) {
          if ( divtpz(nvl,p0,tps[0],&t) &&
            divcheck(nvl,tps+1,m-1,LC(tps[0]),tps[0]) ) {
            *pr = tps[0]; return;
          } else
            break;
        } else {
          henmv(nvl,vn1,lgp0,g0,h0,a0,b0,lg,lp0,lg0,lp00,(Z)tq,k,&g,&h);
          if ( divtpz(nvl,lgp0,g,&t) &&
            divcheck(nvl,tps,m,lg,g) ) {
            pcp(nvl,g,pr,&t); return;
          }
        }
      }
    }
}

void intersectv(vl1,vl2,vlp)
VL vl1,vl2,*vlp;
{
  int i,n;
  VL tvl;
  VN tvn;

  if ( !vl1 || !vl2 ) {
    *vlp = 0; return;
  }
  for ( n = 0, tvl = vl1; tvl; tvl = NEXT(tvl), n++ );
  tvn = (VN) ALLOCA(n*sizeof(struct oVN));
  for ( i = 0, tvl = vl1; i < n; tvl = NEXT(tvl), i++ ) {
    tvn[i].v = tvl->v; tvn[i].n = 0;
  }
  for ( tvl = vl2; tvl; tvl = NEXT(tvl) )
    for ( i = 0; i < n; i++ )
      if ( tvn[i].v == tvl->v ) {
        tvn[i].n = 1; break;
      }
  vntovl(tvn,n,vlp);
}

int pcoef(vl,ivl,p,cp)
VL vl,ivl;
P p;
P *cp;
{
  VL nvl,tvl,svl,mvl,mvl0;
  P t;

  if ( NUM(p) ) {
    cp[0] = p; return 1;
  } else {
    clctv(vl,p,&nvl);
    for ( tvl = nvl, mvl0 = 0; tvl; tvl = NEXT(tvl) ) {
      for ( svl = ivl; svl; svl = NEXT(svl) )
        if ( tvl->v == svl->v )
          break;
      if ( !svl ) {
        if ( !mvl0 ) {
          NEWVL(mvl0); mvl = mvl0;
        } else {
          NEWVL(NEXT(mvl)); mvl = NEXT(mvl);  
        }
        mvl->v = tvl->v;
      }
    }
    if ( mvl0 )
      NEXT(mvl) = ivl; 
    else
      mvl0 = ivl;
    reorderp(mvl0,nvl,p,&t);
    return pcoef0(mvl0,ivl,t,cp);
  }
}

int pcoef0(vl,ivl,p,cp)
VL vl,ivl;
P p;
P *cp;
{
  int cn,n;
  DCP dc;
  V v;
  VL tvl;

  if ( NUM(p) ) {
    cp[0] = p; return 1;
  } else {
    for ( v = VR(p), tvl = ivl; tvl; tvl = NEXT(tvl) )
      if ( v == tvl->v )
        break;
    if ( tvl ) {
      cp[0] = p; return 1;
    } else {
      for ( dc = DC(p), n = 0; dc; dc = NEXT(dc) ) {
        cn = pcoef0(vl,ivl,COEF(dc),cp); cp += cn; n += cn;
      }
      return n;
    }
  }
}

int lengthp(p)
P p;
{
  int n;
  DCP dc;

  if ( NUM(p) )
    return 1;
  else {
    for ( dc = DC(p), n = 0; dc; dc = NEXT(dc) )
      n += lengthp(COEF(dc));
    return n;
  }
}

int nonzero_const_term(P p)
{
  DCP dc;

  if ( !p )
    return 0;
  else if ( NUM(p) )
    return 1;
  else {
    dc = DC(p);
    for ( ; NEXT(dc); dc = NEXT(dc) );
    if ( DEG(dc) )
      return 0;
    else
      return nonzero_const_term(COEF(dc));
  }
}
