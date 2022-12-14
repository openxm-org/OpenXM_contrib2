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
 * $OpenXM: OpenXM_contrib2/asir2018/engine/E.c,v 1.1 2018/09/19 05:45:06 noro Exp $
*/
#include "ca.h"

void henmv(VL vl,VN vn,P f,P g0,P h0,P a0,P b0,P lg,P lh,P lg0,P lh0,Z q,int k,P *gp,P *hp)
{
  P g1,h1,a1,b1;
  Z q2,r2,two;

  STOZ(2,two);
  divqrz(q,two,&q2,&r2);
  adjc(vl,g0,a0,lg0,q,&g1,&a1); adjc(vl,h0,b0,lh0,q,&h1,&b1);
  henmvmain(vl,vn,f,g1,h1,b1,a1,lg,lh,q,q2,k,gp,hp);
}

void henmvmain(VL vl,VN vn,P f,P fi0,P fi1,P gi0,P gi1,P l0,P l1,Z mod,Z mod2,int k,P *fr0,P *fr1)
{
  V v;
  int n,i,j;
  int *md;
  P x,m,m1,c,q,r,a,s,u,ff,f0,f1;
  P w0,w1,cf,cfi,t,q1,dvr;
  P *c0,*c1;
  P *f0h,*f1h;

  v = VR(f); n = deg(v,f); MKV(v,x);
  c0 = (P *)ALLOCA((n+1)*sizeof(P));
  c1 = (P *)ALLOCA((n+1)*sizeof(P));
  invz((Z)LC(fi1),mod,(Z *)&c); mulp(vl,fi1,c,&t); cmp(mod,t,&dvr); 
  cm2p(mod,mod2,gi0,&c0[0]); cm2p(mod,mod2,gi1,&c1[0]);
  for ( i = 1; i <= n; i++ ) {
    mulp(vl,x,c1[i-1],&m); divsrp(vl,m,dvr,&q,&r); mulp(vl,q,c,&q1);
    cm2p(mod,mod2,r,&c1[i]); 
    mulp(vl,x,c0[i-1],&m); mulp(vl,q1,fi0,&m1); addp(vl,m,m1,&a);
    cm2p(mod,mod2,a,&c0[i]); 
  }
  affine(vl,f,vn,&t); cm2p(mod,mod2,t,&ff); 
  for ( i = 0; vn[i].v; i++ );
  md = ( int *) ALLOCA((i+1)*sizeof(int));
  for ( i = 0; vn[i].v; i++ )
    md[i] = getdeg(vn[i].v,ff);
  cm2p(mod,mod2,fi0,&f0); affine(vl,l0,vn,&t);
  if ( NUM(f0) )
    cm2p(mod,mod2,t,&f0);
  else
    cm2p(mod,mod2,t,&COEF(DC(f0)));
  cm2p(mod,mod2,fi1,&f1); affine(vl,l1,vn,&t);
  if ( NUM(f1) )
    cm2p(mod,mod2,t,&f1);
  else
    cm2p(mod,mod2,t,&COEF(DC(f1)));
  W_CALLOC(k,P,f0h); W_CALLOC(k,P,f1h);
  for ( i = 0; i <= k; i++ ) {
    exthpc(vl,v,f0,i,&f0h[i]); exthpc(vl,v,f1,i,&f1h[i]);
  }
  for ( j = 1; j <= k; j++ ) {
    for ( i = 0; vn[i].v; i++ ) 
      if ( getdeg(vn[i].v,f0)+getdeg(vn[i].v,f1) > md[i] )
        goto END;
    for ( i = 0, s = 0; i <= j; i++ ) {
      mulp(vl,f0h[i],f1h[j-i],&t); addp(vl,s,t,&u); s = u;
    }
    cm2p(mod,mod2,s,&t);
    exthpc(vl,v,ff,j,&u); subm2p(vl,mod,mod2,u,t,&cf);
    for ( i = 0, w0 = 0, w1 = 0; i <= n; i++ ) {
      if ( !cf ) 
        cfi = 0;
      else if ( VR(cf) == v ) 
        coefp(cf,i,&cfi);
      else if ( i == 0 ) 
        cfi = cf;
      else 
        cfi = 0;
      if ( cfi ) {
        mulp(vl,cfi,c0[i],&m); addp(vl,w0,m,&a); w0 = a;
        mulp(vl,cfi,c1[i],&m); addp(vl,w1,m,&a); w1 = a;
      }
    }
    cm2p(mod,mod2,w0,&t); addm2p(vl,mod,mod2,f0,t,&a); 
    addm2p(vl,mod,mod2,f0h[j],t,&s); f0h[j] = s; f0 = a;
    cm2p(mod,mod2,w1,&u); addm2p(vl,mod,mod2,f1,u,&a); 
    addm2p(vl,mod,mod2,f1h[j],u,&s); f1h[j] = s; f1 = a;
    if ( !t ) {
      restore(vl,f0,vn,&s); cm2p(mod,mod2,s,&t);
      if ( divtpz(vl,f,t,&s) ) {
        *fr0 = t; *fr1 = s;  
        return;
      } 
    }
    if ( !u ) {
      restore(vl,f1,vn,&s); cm2p(mod,mod2,s,&t);
      if ( divtpz(vl,f,t,&s) ) {
        *fr0 = s; *fr1 = t;  
        return;
      } 
    }
  }
END:
  restore(vl,f0,vn,&t); cm2p(mod,mod2,t,fr0); 
  restore(vl,f1,vn,&t); cm2p(mod,mod2,t,fr1); 
}

/*
  input : f, fi0, fi1, p, k; f = fi0 * fi1 mod p; ( p, k : integer )
  output : gr1 * fr0 + gr0 * fr1 = 1 mod qr; ( qr = p^(2^(k-1)) )
*/

void henzq(P f,P i0,UM fi0,P i1,UM fi1,int p,int k,P *fr0p,P *fr1p,P *gr0p,P *gr1p,Z *qrp)
{
  Z q,qq,q2,two,r2;
  int n,i;
  UM wg0,wg1,wf0,wf1;
  P f0,f1,g0,g1,m,m1,s,c,d,q1,r1,rm,rm1,a,a1,a2;

  n = UDEG(f);
  wf0 = W_UMALLOC(n); wf1 = W_UMALLOC(n);
  wg0 = W_UMALLOC(n); wg1 = W_UMALLOC(n);
  cpyum(fi0,wf0); cpyum(fi1,wf1);
  eucum(p,wf0,wf1,wg1,wg0);
  umtop(VR(f),fi0,&f0); umtop(VR(f),fi1,&f1);
  umtop(VR(f),wg0,&g0); umtop(VR(f),wg1,&g1);

  STOZ(2,two); STOZ(p,q); divqrz(q,two,&q2,&r2);
  for ( i = 1; i < k; i++ ) {
  /*  c = ((f - f0*f1)/q) mod q;
    q1 = (c*g1) / f1; 
    r1 = (c*g1) mod f1;
    f1 += (r1 mod q)*q;
    f0 += ((c*g0 + q1*f0) mod q)*q;

    d = ((1 - (f1*g0 + f0*g1))/q) mod q;
    q1 = (d*g0) / f1;
    r1 = (d*g0) mod f1;
    g1 += (r1 mod q)*q;
    g0 += ((c*g0 + q1*f0) mod q)*q;
    q = q^2;
  */

  /* c = ((f - f0*f1)/q) mod q */
    mulp(CO,f0,f1,&m); subp(CO,f,m,&s); 
    divcp(s,(Q)q,&m); cm2p(q,q2,m,&c); 

  /* q1 = (c*g1) / f1; r1 = (c*g1) mod f1; */
    mulp(CO,c,g1,&m); cm2p(q,q2,m,&s); 
    udivpwm(q,s,f1,&q1,&r1); 

  /* f1 = f1 + (r1 mod q)*q; */
    cm2p(q,q2,r1,&rm); 
    mulpq(rm,(P)q,&rm1); addp(CO,f1,rm1,&a); 
    f1 = a;

  /* a1 = (c*g0 + q1*f0) mod q; */
    mulp(CO,c,g0,&m); mulp(CO,q1,f0,&m1); addp(CO,m,m1,&a);
    cm2p(q,q2,a,&a1); 

  /* f0 = f0 + a1*q; */
    mulpq(a1,(P)q,&a2); 
    addp(CO,f0,a2,&a); 
    f0 = a;

  /* d = ((1 - (f1*g0 + f0*g1))/q) mod q; */
    mulp(CO,f1,g0,&m); mulp(CO,f0,g1,&m1); addp(CO,m,m1,&a);
    subp(CO,(P)ONE,a,&s); 
    divcp(s,(Q)q,&m); cm2p(q,q2,m,&d); 

  /* q1 = (d*g0) / f1; r1 = (d*g0) mod f1; */
    mulp(CO,d,g1,&m); udivpwm(q,m,f1,&q1,&r1); 

  /* g1 = g1 + (r1 mod q )*q; */
    cm2p(q,q2,r1,&rm); 
    mulpq(rm,(P)q,&rm1); addp(CO,g1,rm1,&a); 
    g1 = a;

  /* g0 = g0 + (c*g0 + q1*f0 mod q)*q; */
    mulp(CO,d,g0,&m); mulp(CO,q1,f0,&m1); addp(CO,m,m1,&a);
    cm2p(q,q2,a,&a1); mulpq(a1,(P)q,&a2); 
    addp(CO,g0,a2,&a); 
    g0 = a;

  /* q = q^2; */
    mulz(q,q,&qq); q = qq; divqrz(q,two,&q2,&r2);
  } 
  *fr0p = f0; *fr1p = f1; *gr0p = g0; *gr1p = g1; *qrp = q;
}

void henzq1(P g,P h,Q bound,P *gcp,P *hcp,Z *qp)
{
  V v;
  Z w,q,q1;
  Q f;
  Q u,t,a,b,s,ts;
  P c,c1;
  P tg,th,tr;
  UM wg,wh,ws,wt,wm;
  int n,m,modres,mod,index,i;
  P gc0,hc0;
  P z,zz,zzz;


  v = VR(g); n=deg(v,g); m=deg(v,h);
  norm(g,&a); norm(h,&b);
  STOZ(m,w); pwrq(a,(Q)w,&t); 
  STOZ(n,w); pwrq(b,(Q)w,&s); 
  mulq(t,s,&ts); 

  factorialz(n+m,&w); mulq(ts,(Q)w,&s); 
  addq(s,s,&f); 

  wg = W_UMALLOC(m+n); wh = W_UMALLOC(m+n);
  wt = W_UMALLOC(m+n); ws = W_UMALLOC(m+n);
  wm = W_UMALLOC(m+n); 

  for ( q = ONE, t = 0, c = 0, index = 0; ; ) {
    mod = get_lprime(index++);
    if ( !remqi((Q)LC(g),mod) || !remqi((Q)LC(h),mod) ) 
      continue;
    ptomp(mod,g,&tg); ptomp(mod,h,&th);
    srchump(mod,tg,th,&tr);
    if ( !tr )
      continue;
    else
      modres = CONT((MQ)tr);

    mptoum(tg,wg); mptoum(th,wh); 
    eucum(mod,wg,wh,ws,wt); /* wg * ws + wh * wt = 1 */
    DEG(wm) = 0; COEF(wm)[0] = modres;
    mulum(mod,ws,wm,wt);
    for ( i = DEG(wt); i >= 0; i-- ) 
      if ( ( COEF(wt)[i] * 2 ) > mod ) 
        COEF(wt)[i] -= mod;
    chnrem(mod,v,c,q,wt,&c1,&q1); 
    if ( !ucmpp(c,c1) ) {
      mulp(CO,c,g,&z); divsrp(CO,z,h,&zz,&zzz);
      if ( NUM(zzz) ) {
        q = q1; c = c1;
        break;
      } 
    }
    q = q1; c = c1;

    if ( cmpq(f,(Q)q) < 0 ) 
      break;
  }
  ptozp(c,1,&s,&gc0); 
  /* gc0*g = zz*h + zzz -> c*g + (-zz)*h = zzz */
  mulp(CO,gc0,g,&z);
  divsrp(CO,z,h,&zz,&zzz);
  ptozp(zz,1,&s,(P *)&t); 
  if ( INT(s) )
    chsgnp(zz,&hc0);
  else {
    MPZTOZ(mpq_denref(BDY(s)),q); mulq((Q)q,(Q)zzz,(Q *)&q1); zzz = (P)q1;
    mulp(CO,gc0,(P)q,&c); gc0 = c; mulp(CO,zz,(P)q,&c); chsgnp(c,&hc0);
  }
  if ( !INT((Q)zzz) ) {
    MPZTOZ(mpq_denref(BDY((Q)zzz)),q); MPZTOZ(mpq_numref(BDY((Q)zzz)),q1); zzz = (P)q1;
    mulp(CO,gc0,(P)q,&c); gc0 = c; mulp(CO,hc0,(P)q,&c); hc0 = c;
  }
  for ( index = 0; ; ) {
    mod = get_lprime(index++);
    if ( !remqi((Q)zzz,mod) ||
      !remqi((Q)LC(g),mod) || 
      !remqi((Q)LC(h),mod) ) 
      continue;
    for ( STOZ(mod,q); cmpq((Q)q,bound) < 0; ) {
      mulz(q,q,&q1); q = q1;
    }
    *qp = q;
    invz((Z)zzz,q,&q1); 
    mulp(CO,gc0,(P)q1,hcp); mulp(CO,hc0,(P)q1,gcp);
    return;
  }
}

void addm2p(VL vl,Z mod,Z mod2,P n1,P n2,P *nr)
{
  P t;

  addp(vl,n1,n2,&t);
  if ( !t )
    *nr = 0;
  else 
    cm2p(mod,mod2,t,nr); 
}

void subm2p(VL vl,Z mod,Z mod2,P n1,P n2,P *nr)
{
  P t;

  subp(vl,n1,n2,&t);
  if ( !t )
    *nr = 0;
  else 
    cm2p(mod,mod2,t,nr); 
}

void mulm2p(VL vl,Z mod,Z mod2,P n1,P n2,P *nr)
{
  P t;

  mulp(vl,n1,n2,&t);
  if ( !t )
    *nr = 0;
  else 
    cm2p(mod,mod2,t,nr); 
}

void cmp(Z mod,P p,P *pr)
{
  P t;
  DCP dc,dcr,dcr0;

  if ( !p ) 
    *pr = 0;
  else if ( NUM(p) )
    remz((Z)p,mod,(Z *)pr);
  else {
    for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
      cmp(mod,COEF(dc),&t);
      if ( t ) {
        NEXTDC(dcr0,dcr);
        DEG(dcr) = DEG(dc);
        COEF(dcr) = t;
      }
    }
    if ( !dcr0 ) 
      *pr = 0;
    else {
      NEXT(dcr) = 0;
      MKP(VR(p),dcr0,*pr);
    }
  }
}

void cm2p(Z mod,Z m,P p,P *pr)
{
  P t;
  DCP dc,dcr,dcr0;

  if ( !p ) 
    *pr = 0;
  else if ( NUM(p) ) 
    rem2q((Z)p,mod,m,(Z *)pr);
  else {
    for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
      cm2p(mod,m,COEF(dc),&t);
      if ( t ) {
        NEXTDC(dcr0,dcr);
        DEG(dcr) = DEG(dc);
        COEF(dcr) = t;
      }
    }
    if ( !dcr0 ) 
      *pr = 0;
    else {
      NEXT(dcr) = 0;
      MKP(VR(p),dcr0,*pr);
    } 
  }
}

void addm2q(Z mod,Z mod2,Z n1,Z n2,Z *nr)
{
  Z t;

  addz(n1,n2,&t);
  if ( !t )
    *nr = 0;
  else 
    rem2q(t,mod,mod2,nr); 
}

void subm2q(Z mod,Z mod2,Z n1,Z n2,Z *nr)
{
  Z t;

  subz(n1,n2,&t);
  if ( !t )
    *nr = 0;
  else 
    rem2q(t,mod,mod2,nr); 
}

void mulm2q(Z mod,Z mod2,Z n1,Z n2,Z *nr)
{
  Z t;

  mulz(n1,n2,&t);
  if ( !t )
    *nr = 0;
  else 
    rem2q(t,mod,mod2,nr); 
}

void rem2q(Z n,Z m,Z m2,Z *nr)
{
  Z r;
  int sgn;

  remz(n,m,&r);
  if ( !r ) 
    *nr = 0;
  else {
    sgn = cmpz(r,m2);
    if ( sgn > 0 )
      subz(r,m,nr);
    else 
      *nr = r;
  }
}

/* 
  extract d-homogeneous part with respect to vl - {v}
*/

void exthpc_generic(VL vl,P p,int d,V v,P *pr)
{
  P w,x,t,t1,a,xd;
  V v0;
  DCP dc;

  if ( d < 0 || !p )
    *pr = 0;
  else if ( NUM(p) )
    if ( d == 0 )
      *pr = p;
    else
      *pr = 0;
  else if ( v == VR(p) )
    exthpc(vl,v,p,d,pr);
  else {
    v0 = VR(p);
    for ( MKV(v0,x), dc = DC(p), w = 0; dc; dc = NEXT(dc) ) {
      exthpc_generic(vl,COEF(dc),d-ZTOS(DEG(dc)),v,&t);
      pwrp(vl,x,DEG(dc),&xd);
      mulp(vl,xd,t,&t1); addp(vl,w,t1,&a); w = a;
    }
    *pr = w;
  }
}

void exthp(VL vl,P p,int d,P *pr)
{
  P t,t1,a,w,x,xd;
  DCP dc;

  if ( d < 0 ) 
    *pr = 0;
  else if ( NUM(p) )
    if ( d == 0 ) 
      *pr = p;
    else 
      *pr = 0;
  else {
    for ( MKV(VR(p),x), dc = DC(p), w = 0; dc; dc = NEXT(dc) ) {
      exthp(vl,COEF(dc),d - ZTOS(DEG(dc)),&t);
      pwrp(vl,x,DEG(dc),&xd); 
      mulp(vl,xd,t,&t1); addp(vl,w,t1,&a); w = a;
    }
    *pr = w;
  }
}

void exthpc(VL vl,V v,P p,int d,P *pr)
{
  P t,t1,a,w,x,xd;
  DCP dc;

  if ( v != VR(p) ) 
    exthp(vl,p,d,pr);
  else if ( d < 0 ) 
    *pr = 0;
  else {
    for ( MKV(v,x), dc = DC(p), w = 0; dc; dc = NEXT(dc) ) {
      exthp(vl,COEF(dc),d,&t);
      pwrp(vl,x,DEG(dc),&xd);
      mulp(vl,xd,t,&t1); addp(vl,w,t1,&a); w = a;
    }
    *pr = w;
  }
}

void cbound(VL vl,P p,Q *b)
{
  Q n,m;
  Z t,e;
  int k;

  cmax(p,&n);
  addq(n,n,&m); 

  k = geldb(vl,p);
  STOZ(3,t); STOZ(k,e);

  pwrq((Q)t,(Q)e,&n);
  mulq(m,n,b); 
}

int geldb(VL vl,P p)
{
  int m;

  for ( m = 0; vl; vl = NEXT(vl) ) 
    m += getdeg(vl->v,p);
  return ( m );
}

int getdeg(V v,P p)
{
  int m,t;
  DCP dc;
  
  if ( !p || NUM(p) ) 
    return ( 0 );
  else if ( v == VR(p) ) 
    return ( deg(v,p) );
  else {
    for ( dc = DC(p), m = 0; dc; dc = NEXT(dc) ) {
      t = getdeg(v,COEF(dc));
      m = MAX(m,t);
    }
    return ( m );
  }
}

/* YYY */

void cmax(P p,Q *b)
{
  DCP dc;
  Q m,m1;

  if ( NUM(p) )
    absq((Q)p,b);
  else {
    for ( dc = DC(p), m = 0; dc; dc = NEXT(dc) ) {
      cmax(COEF(dc),&m1);
      if ( cmpq(m1,m) > 0 ) 
        m = m1;
    }
    *b = m;
  }
}

int nextbin(VN vn,int n)
{
  int tmp,i,carry;

  if ( n == 0 ) 
    return ( 1 );

  for ( i = n - 1, carry = 1; i >= 0; i-- ) {
    tmp =  vn[i].n + carry;
    vn[i].n = tmp % 2;
    carry = tmp / 2;
  }
  return ( carry );
}

void mulsgn(VN vn,VN vnt,int n,VN vn1)
{
  int i;

  for ( i = 0; vn[i].v; i++ ) 
    vn1[i].n = vn[i].n;
  for ( i = 0; i < n; i++ ) 
    if ( vnt[i].n ) 
      vn1[(long)vnt[i].v].n *= -1;
}

void next(VN vn)
{
  int i,m,n,tmp,carry;

  for ( m = 0, i = 0; vn[i].v; i++ ) 
    m = MAX(m,ABS(vn[i].n));
  if ( m == 0 ) {
    vn[--i].n = 1;
    return;
  }
  for ( carry = 1, m++, i--; ( i >= 0 ) && carry; i-- ) {
    tmp = vn[i].n + carry;
    vn[i].n = tmp % m;
    carry = tmp / m;
  }
  if ( ( i == -1 ) && carry ) {
    for ( i = 0; vn[i].v; i++ ) 
      vn[i].n = 0;
    vn[--i].n = m;
  } else {
    for ( n = 0, i = 0; vn[i].v; i++ ) 
      n = MAX(n,ABS(vn[i].n));
    if ( n < m - 1 ) 
      vn[--i].n = m - 1;
  }
}
    
void clctv(VL vl,P p,VL *nvlp)
{
  int i,n;
  VL tvl;
  VN tvn;

  if ( !p || NUM(p) ) {
    *nvlp = 0;
    return;
  }

  for ( n = 0, tvl = vl; tvl; tvl = NEXT(tvl), n++ );
  tvn = (VN) ALLOCA((n+1)*sizeof(struct oVN));
  for ( i = 0, tvl = vl; tvl; tvl = NEXT(tvl), i++ ) {
    tvn[i].v = tvl->v;
    tvn[i].n = 0;
  }

  markv(tvn,n,p);
  vntovl(tvn,n,nvlp);
}

void markv(VN vn,int n,P p)
{
  V v;
  DCP dc;
  int i;

  if ( NUM(p) ) 
    return;
  v = VR(p);
  for ( i = 0, v = VR(p); i < n; i++ ) 
    if ( v == vn[i].v ) {
      vn[i].n = 1;
      break;
    }
  for ( dc = DC(p); dc; dc = NEXT(dc) ) 
    markv(vn,n,COEF(dc));
}
      
void vntovl(VN vn,int n,VL *vlp)
{
  int i;
  VL tvl,tvl0;

  for ( i = 0, tvl0 = 0; ; ) {
    while ( ( i < n ) && ( vn[i].n == 0 ) ) i++;
    if ( i == n ) 
      break;
    else {
      if ( !tvl0 ) {
        NEWVL(tvl0);
        tvl = tvl0;
      } else {
        NEWVL(NEXT(tvl));
        tvl = NEXT(tvl);
      }
      tvl->v = vn[i++].v;  
    }
  }
  if ( tvl0 ) 
    NEXT(tvl) = 0;
  *vlp = tvl0;
}

int dbound(V v,P f)
{
  int m,t;
  DCP dc;

  if ( !f ) 
    return ( -1 );
  else if ( v != VR(f) ) 
    return homdeg(f);
  else {
    for ( dc = DC(f), m = 0; dc; dc = NEXT(dc) ) {
      t = homdeg(COEF(dc));
      m = MAX(m,t);
    }
    return ( m );
  }
}

int homdeg(P f)
{
  int m,t;
  DCP dc;

  if ( !f ) 
    return ( -1 );
  else if ( NUM(f) ) 
    return( 0 );
  else {
    for ( dc = DC(f), m = 0; dc; dc = NEXT(dc) ) {
      t = ZTOS(DEG(dc))+homdeg(COEF(dc));
      m = MAX(m,t);
    }
    return ( m );
  }
}

int minhomdeg(P f)
{
  int m,t;
  DCP dc;

  if ( !f ) 
    return ( -1 );
  else if ( NUM(f) ) 
    return( 0 );
  else {
    for ( dc = DC(f), m = homdeg(f); dc; dc = NEXT(dc) ) {
      t = ZTOS(DEG(dc))+minhomdeg(COEF(dc));
      m = MIN(m,t);
    }
    return ( m );
  }
}

void adjc(VL vl,P f,P a,P lc0,Z q,P *fr,P *ar)
{
  Z m,t;
  P m1;

  invz((Z)LC(f),q,&t);
  mulz((Z)lc0,t,&m); 
  mulpq(f,(P)m,&m1); cmp(q,m1,fr); 
  invz(m,q,&t); 
  mulpq(a,(P)t,&m1); 
  cmp(q,m1,ar); 
}

void affinemain(VL vl,P p,V v0,int n,P *pl,P *pr)
{
  P x,t,m,c,s,a;
  DCP dc;
  Z d;

  if ( !p ) 
    *pr = 0;
  else if ( NUM(p) )
    *pr = p;
  else if ( VR(p) != v0 ) {
    MKV(VR(p),x);
    for ( c = 0, dc = DC(p); dc; dc = NEXT(dc) ) {
      affinemain(vl,COEF(dc),v0,n,pl,&t);
      if ( DEG(dc) ) {
        pwrp(vl,x,DEG(dc),&s); mulp(vl,s,t,&m); 
        addp(vl,m,c,&a); c = a;
      } else {
        addp(vl,t,c,&a); c = a;
      }
    }
    *pr = c;
  } else {
    dc = DC(p);
    c = COEF(dc);
    for ( d = DEG(dc), dc = NEXT(dc); 
      dc; d = DEG(dc), dc = NEXT(dc) ) {
        mulp(vl,pl[ZTOS(d)-ZTOS(DEG(dc))],c,&m); 
        addp(vl,m,COEF(dc),&c); 
    }
    if ( d ) {
      mulp(vl,pl[ZTOS(d)],c,&m); c = m;
    }
    *pr = c;
  }
}

void restore(VL vl,P f,VN vn,P *fr)
{
  int i;
  P vv,g,g1,t;
  Z s;

  g = f;
  for ( i = 0; vn[i].v; i++ ) {
    MKV(vn[i].v,t);
    if ( vn[i].n ) {
      STOZ(-vn[i].n,s);
      addp(vl,t,(P)s,&vv);
    } else 
      vv = t;

    substp(vl,g,vn[i].v,vv,&g1); g = g1;
  }
  *fr = g;
}

void mergev(VL vl,VL vl1,VL vl2,VL *nvlp)
{
  int i,n;
  VL tvl;
  VN vn;

  if ( !vl1 ) {
    *nvlp = vl2; return;
  } else if ( !vl2 ) {
    *nvlp = vl1; return;
  }
  for ( i = 0, tvl = vl; tvl; i++, tvl = NEXT(tvl) ); 
  n = i;
  W_CALLOC(n,struct oVN,vn);
  for ( i = 0, tvl = vl; tvl; tvl = NEXT(tvl), i++ )
    vn[i].v = tvl->v;
  for ( i = 0, tvl = vl1; (i < n) && tvl; i++, tvl = NEXT(tvl)) {
    while ( ( i < n ) && ( vn[i].v != tvl->v ) ) 
      i++;
    if ( i == n ) 
      break;
    else 
      vn[i].n = 1;
  }
  for ( i = 0, tvl = vl2; (i < n) && tvl; i++, tvl = NEXT(tvl) ) {
    while ( ( i < n ) && ( vn[i].v != tvl->v ) ) 
      i++;
    if ( i == n ) 
      break;
    else 
      vn[i].n = 1;
  }
  vntovl(vn,n,nvlp);
}
