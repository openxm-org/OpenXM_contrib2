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
 * $OpenXM: OpenXM_contrib2/asir2018/engine/A.c,v 1.1 2018/09/19 05:45:06 noro Exp $
*/
#include "ca.h"
#include "parse.h"

int get_lprime();

void pdiva(VL vl,P p0,P p1,P p2,P *pr)
{
  Z m,d;
  P t,q,r;

  if ( VR(p2) == VR(p0) ) {
    *pr = p1;
    return;
  }
  if ( VR(p1) == VR(p0) ) {
    error("pdiva : ???");
    return;
  }
  subz(DEG(DC(p1)),DEG(DC(p2)),&m); addz(m,ONE,&d);  
  pwrz((Z)COEF(DC(p2)),d,&m); mulp(vl,p1,(P)m,&t);
  divsrp(vl,t,p2,&q,&r); remsdcp(vl,q,p0,pr);
}

void rema(VL vl,P p0,P p1,P p2,P *pr)
{
  P m,m1,m2,g,lc,x,t;
  Z q;
  V v;
  int d1,d2;

  if ( !p1 || !p2 || NUM(p2) || ( VR(p2) == VR(p0) ) ) 
    *pr = 0;
  else if ( NUM(p1) ) 
    *pr = p1;
  else if ( ( v = VR(p1) ) == VR(p0) ) 
    remsdcp(vl,p1,p0,pr);
  else if ( ( d1 = deg(v,p1) ) < ( d2 = deg(v,p2) ) ) 
    *pr = p1;
  else if ( d1 == d2 ) {
    mulp(vl,p1,LC(p2),&m); mulp(vl,p2,LC(p1),&m1);
    subp(vl,m,m1,&m2); 
    remsdcp(vl,m2,p0,pr); 
  } else {
    g = p1; lc = LC(p2); MKV(v,x);
    while ( ( d1 = deg(v,g) ) >= d2 ) {
      mulp(vl,g,lc,&m); mulp(vl,p2,LC(g),&m1);
      STOZ(d1-d2,q); pwrp(vl,x,q,&t);
      mulp(vl,m1,t,&m2); subp(vl,m,m2,&m1); 
      remsdcp(vl,m1,p0,&g); 
    }
    *pr = g;
  }
}

void gcda(VL vl,P p0,P p1,P p2,P *pr)
{
  P g1,g2,r,t,c;

  remsdcp(vl,p1,p0,&t); pcp(vl,t,&g1,&c); remsdcp(vl,p2,p0,&g2);
  while ( 1 ) {
    if ( NUM(g2) || (VR(g2) == VR(p0)) ) {
      *pr = (P)ONE;
      return;
    }
    pcp(vl,g2,&t,&c); pmonic(vl,p0,t,&g2); rema(vl,p0,g1,g2,&r); 
    if ( !r ) {
      pmonic(vl,p0,g2,pr); 
      return;
    }
    g1 = g2; g2 = r;
  }
}

void sqa(VL vl,P p0,P p,DCP *dcp)
{
  V v;
  int i,j,n;
  P f,f1,f2,gcd,flat,q,r,t;
  P *a;
  int *b;
  DCP dc,dc0;

  if ( ( v = VR(p) ) == VR(p0) ) {
    NEWDC(dc); *dcp = dc; COEF(dc) = (P)ONE; DEG(dc) = ONE;
    NEXT(dc) = 0;
  } else {
    n = deg(v,p); W_CALLOC(n,P,a); W_CALLOC(n,int,b);
    for ( i = 0, f = p; ;i++ ) {
      if ( VR(f) != v ) 
        break;
      remsdcp(vl,f,p0,&f1); diffp(vl,f1,VR(f1),&f2);
      while ( 1 ) {
        pmonic(vl,p0,f2,&t); f2 = t; rema(vl,p0,f1,f2,&r); 
        if ( !r ) {
          pmonic(vl,p0,f2,&gcd); pqra(vl,p0,f,gcd,&flat,&r); 
          break;
        }
        f1 = f2; f2 = r;
      }
      if ( VR(gcd) != v ) {
        a[i] = f; b[i] = 1; 
        break;
      }
      for ( j = 1, f = gcd; ; j++ ) {
        pqra(vl,p0,f,flat,&q,&r);
        if ( r )
          break;
        else
          f = q;
      }
      a[i] = flat; b[i] = j;
    }
    for ( i = 0, j = 0, dc0 = 0; a[i]; i++ ) {
      NEXTDC(dc0,dc); j += b[i]; STOZ(j,DEG(dc));
      if ( a[i+1] ) 
        pqra(vl,p0,a[i],a[i+1],&COEF(dc),&t);
      else
        COEF(dc) = a[i];
    }
    NEXT(dc) = 0; *dcp = dc0;
  }
}

void pqra(VL vl,P p0,P p1,P p2,P *p,P *pr)
{
  V v,v1;
  P  m,m1,q,r;
  Z tq;
  int n;

  if ( ( v = VR(p1) ) != ( v1 = VR(p2) ) ) {
    while ( ( VR(vl) != v ) && ( VR(vl) != v1 ) ) 
      vl = NEXT(vl);
    if ( VR(vl) == v ) {
      *p = p1; *pr = 0;
    } else {
      *p = 0; *pr = p1;
    }
  } else if ( (n = deg(v,p1) - deg(v,p2)) < 0 ) {
    *pr = p1; *p = 0;
  } else {      
    n++; STOZ(n,tq);
    pwrp(vl,LC(p2),tq,&m); mulp(vl,m,p1,&m1); divsrp(vl,m1,p2,&q,&r); 
    remsdcp(vl,q,p0,p); remsdcp(vl,r,p0,pr); 
  }
}

void pmonic(VL vl, P p0, P p, P *pr)
{
  P d,m,pp,c,t;

  if ( NUM(p) || ( VR(p) == VR(p0) ) )
    *pr = (P)ONE;
  else if ( NUM(COEF(DC(p))) )
    *pr = p;
  else {
    ptozp(COEF(DC(p)),1,(Q *)&c,&pp); pinva(p0,pp,&d); 
    mulp(vl,p,d,&m); remsdcp(vl,m,p0,&t); ptozp(t,1,(Q *)&c,pr); 
  }
}

void remsdcp(VL vl, P p, P p0, P *pr)
{
  P q,r,c;
  DCP dc,dcr,dcr0;

  if ( !p ) 
    *pr = 0;
  else if ( NUM(p) ) 
    *pr = (P)ONE;
  else if ( VR(p) == VR(p0) ) {
    divsrp(vl,p,p0,&q,&r); ptozp(r,1,(Q *)&c,pr); 
  } else {
    for ( dcr = dcr0 = 0, dc = DC(p); dc; dc = NEXT(dc) ) {
      divsrp(vl,COEF(dc),p0,&q,&r); 
      if ( r ) {
        NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = r;
      }
    }
    if ( !dcr0 ) 
      *pr = 0;
    else {
      NEXT(dcr) = 0; MKP(VR(p),dcr0,r); ptozp(r,1,(Q *)&c,pr); 
    }
  }
}

void pinva(P p0, P p, P *pr)
{
  V v;
  Z w,q,q1;
  Q f;
  Q t,a,b,s,ts;
  P c,c1;
  P tg,th,tr;
  P z,zz,zzz;
  UM wg,wh,ws,wt,wm;
  int n,m,modres,mod,index,i;

  v = VR(p); n=deg(v,p); m=deg(v,p0);
  norm(p,&a); norm(p0,&b);
  STOZ(m,w); pwrq(a,(Q)w,&t); STOZ(n,w); pwrq(b,(Q)w,&s); 
  mulq(t,s,&ts); 
  factorialz(n+m,&w); mulq(ts,(Q)w,&s); addq(s,s,&f); 
  wg = W_UMALLOC(m+n); wh = W_UMALLOC(m+n);
  wt = W_UMALLOC(m+n); ws = W_UMALLOC(m+n);
  wm = W_UMALLOC(m+n); 
  for ( q = ONE, t = 0, c = 0, index = 0; ; ) {
    mod = get_lprime(index++);
    if ( !remqi((Q)LC(p),mod) || !remqi((Q)LC(p0),mod) ) 
      continue;
    ptomp(mod,p,&tg); ptomp(mod,p0,&th); srchump(mod,tg,th,&tr);
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
      mulp(CO,c,p,&z); divsrp(CO,z,p0,&zz,&zzz);
      if ( NUM(zzz) ) {
        q = q1; c = c1; break;
      }
    }
    q = q1; c = c1;
    if ( cmpq(f,(Q)q) < 0 ) 
      break;
  }
  ptozp(c,1,&s,pr); 
}

/* 
 * compute g s.t. h*p0+g*p = res(v,p0,p)
 */

void inva_chrem(P p0, P p, NODE *pr)
{
  V v;
  Z w,q,q1;
  Q f,ts;
  Q u,t,a,b,s,dn;
  P c,c0,c1;
  P tg,th,tr;
  P z,zz,zzz;
  UM wg,wh,ws,wt,wm;
  int n,m,modres,mod,index,i;

  v = VR(p); n=deg(v,p); m=deg(v,p0);
  norm(p,&a); norm(p0,&b);
  STOZ(m,w); pwrq(a,(Q)w,&t); STOZ(n,w); pwrq(b,(Q)w,&s); 
  mulq(t,s,&ts); 
  factorialz(n+m,&w); mulq(ts,(Q)w,&s); addq(s,s,&f); 
  wg = W_UMALLOC(m+n); wh = W_UMALLOC(m+n);
  wt = W_UMALLOC(m+n); ws = W_UMALLOC(m+n);
  wm = W_UMALLOC(m+n); 
  for ( q = ONE, t = 0, c = 0, index = 0; ; ) {
    mod = get_lprime(index++);
    if ( !remqi((Q)LC(p),mod) || !remqi((Q)LC(p0),mod) ) 
      continue;
    ptomp(mod,p,&tg); ptomp(mod,p0,&th); srchump(mod,tg,th,&tr);
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
      mulp(CO,c,p,&z); divsrp(CO,z,p0,&zz,&zzz);
      if ( NUM(zzz) ) {
        q = (Z)zzz; break;
      }
    }
    q = q1; c = c1;
    if ( cmpq(f,(Q)q) < 0 ) {
      mulp(CO,c,p,&z); divsrp(CO,z,p0,&zz,&zzz);
      q = (Z)zzz;
      break;
    }
  }
  /* c*p = q mod p0 */
  /* c = s*c0 */
  ptozp(c,1,&s,&c0); 
  /* c/q = c0/(q/s) */
  divq((Q)q,s,&dn);
  *pr = (NODE)mknode(2,c0,dn);
}
