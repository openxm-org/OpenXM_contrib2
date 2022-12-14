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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/Q.c,v 1.9 2002/08/14 04:49:52 noro Exp $ 
*/
#include "ca.h"
#include "base.h"
#include "inline.h"

void addq(Q n1,Q n2,Q *nr)
{
  N nm,dn,nm1,nm2,nm3,dn0,dn1,dn2,g,g1,g0,m;
  int sgn;
  unsigned t,t1;

  if ( !n1 ) 
    *nr = n2;
  else if ( !n2 ) 
    *nr = n1;
  else if ( INT(n1) ) 
    if ( INT(n2) ) {
      nm1 = NM(n1); nm2 = NM(n2);
      if ( SGN(n1) == SGN(n2) ) {
        if ( PL(nm1) == 1 && PL(nm2) == 1 ) {
          t1 = BD(nm1)[0]; t = t1+BD(nm2)[0];
          if ( t < t1 ) {
            m = NALLOC(2); PL(m) = 2; BD(m)[0] = t; BD(m)[1] = 1;
          } else
            UTON(t,m);
        } else
          addn(NM(n1),NM(n2),&m);
        NTOQ(m,SGN(n1),*nr);
      } else {
        if ( PL(nm1) == 1 && PL(nm2) == 1 ) {
          t1 = BD(nm1)[0]; t = t1-BD(nm2)[0];
          if ( !t )
            sgn = 0;
          else if ( t > t1 ) {
            sgn = -1; t = -((int)t); UTON(t,m);
          } else {
            sgn = 1; UTON(t,m);
          }
        } else
          sgn = subn(NM(n1),NM(n2),&m);
        if ( sgn ) {
          if ( SGN(n1) == sgn )
            NTOQ(m,1,*nr);
          else
            NTOQ(m,-1,*nr);
        } else 
          *nr = 0;
      }
    } else {
      kmuln(NM(n1),DN(n2),&m);
      if ( SGN(n1) == SGN(n2) ) {
        sgn = SGN(n1); addn(m,NM(n2),&nm);
      } else
        sgn = SGN(n1)*subn(m,NM(n2),&nm);
      if ( sgn ) {
        dn = DN(n2); NDTOQ(nm,dn,sgn,*nr);
      } else 
        *nr = 0;
    }
  else if ( INT(n2) ) {
    kmuln(NM(n2),DN(n1),&m);
    if ( SGN(n1) == SGN(n2) ) {
      sgn = SGN(n1); addn(m,NM(n1),&nm);
    } else 
      sgn = SGN(n1)*subn(NM(n1),m,&nm);
    if ( sgn ) {
      dn = DN(n1); NDTOQ(nm,dn,sgn,*nr);
    } else 
      *nr = 0;
  } else {
    gcdn(DN(n1),DN(n2),&g); divsn(DN(n1),g,&dn1); divsn(DN(n2),g,&dn2);
    kmuln(NM(n1),dn2,&nm1); kmuln(NM(n2),dn1,&nm2);
    if ( SGN(n1) == SGN(n2) ) {
      sgn = SGN(n1); addn(nm1,nm2,&nm3);
    } else 
      sgn = SGN(n1)*subn(nm1,nm2,&nm3);
    if ( sgn ) {
      gcdn(nm3,g,&g1); divsn(nm3,g1,&nm); divsn(g,g1,&g0);
      kmuln(dn1,dn2,&dn0); kmuln(g0,dn0,&dn);
      if ( UNIN(dn) )
        NTOQ(nm,sgn,*nr);
      else 
        NDTOQ(nm,dn,sgn,*nr);
    } else 
      *nr = 0;
  }
}

void subq(Q n1,Q n2,Q *nr)
{
  Q m;

  if ( !n1 ) 
    if ( !n2 ) 
      *nr = 0;
    else {
      DUPQ(n2,*nr); SGN(*nr) = -SGN(n2);
    }
  else if ( !n2 ) 
    *nr = n1;
  else if ( n1 == n2 ) 
    *nr = 0;
  else {
    DUPQ(n2,m); SGN(m) = -SGN(n2); addq(n1,m,nr);
  }
}

void mulq(Q n1,Q n2,Q *nr)
{
  N nm,nm1,nm2,dn,dn1,dn2,g;
  int sgn;
  unsigned u,l;

  if ( !n1 || !n2 ) *nr = 0;
  else if ( INT(n1) ) 
    if ( INT(n2) ) {
      nm1 = NM(n1); nm2 = NM(n2);
      if ( PL(nm1) == 1 && PL(nm2) == 1 ) {
        DM(BD(NM(n1))[0],BD(NM(n2))[0],u,l)
        if ( u ) {
          nm = NALLOC(2); PL(nm) = 2; BD(nm)[0] = l; BD(nm)[1] = u;
        } else {
          nm = NALLOC(1); PL(nm) = 1; BD(nm)[0] = l;
        }
      } else
        kmuln(nm1,nm2,&nm);
      sgn = (SGN(n1)==SGN(n2)?1:-1); NTOQ(nm,sgn,*nr);
    } else {
      gcdn(NM(n1),DN(n2),&g); divsn(NM(n1),g,&nm1); divsn(DN(n2),g,&dn);
      kmuln(nm1,NM(n2),&nm); sgn = SGN(n1)*SGN(n2);
      if ( UNIN(dn) )
        NTOQ(nm,sgn,*nr);
      else 
        NDTOQ(nm,dn,sgn,*nr);
    }
  else if ( INT(n2) ) {
    gcdn(NM(n2),DN(n1),&g); divsn(NM(n2),g,&nm2); divsn(DN(n1),g,&dn);
    kmuln(nm2,NM(n1),&nm); sgn = SGN(n1)*SGN(n2);
    if ( UNIN(dn) )
      NTOQ(nm,sgn,*nr);
    else 
      NDTOQ(nm,dn,sgn,*nr);
  } else {
    gcdn(NM(n1),DN(n2),&g); divsn(NM(n1),g,&nm1); divsn(DN(n2),g,&dn2);
    gcdn(DN(n1),NM(n2),&g); divsn(DN(n1),g,&dn1); divsn(NM(n2),g,&nm2);
    kmuln(nm1,nm2,&nm); kmuln(dn1,dn2,&dn); sgn = SGN(n1) * SGN(n2);
    if ( UNIN(dn) )
      NTOQ(nm,sgn,*nr);
    else 
      NDTOQ(nm,dn,sgn,*nr);
  }
}

void divq(Q n1,Q n2,Q *nq)
{
  Q m;

  if ( !n2 ) {
    error("division by 0");
    *nq = 0;
    return;
  } else if ( !n1 ) 
    *nq = 0;
  else if ( n1 == n2 ) 
    *nq = ONE;
  else {
    invq(n2,&m); mulq(n1,m,nq);
  }
}

void divsq(Q n1,Q n2,Q *nq)
{
  int sgn;
  N tn;

  if ( !n2 ) {
    error("division by 0");
    *nq = 0;
    return;
  } else if ( !n1 ) 
    *nq = 0;
  else if ( n1 == n2 ) 
    *nq = ONE;
  else {
    divsn(NM(n1),NM(n2),&tn);
    sgn = SGN(n1)*SGN(n2);
    NTOQ(tn,sgn,*nq);
  }
}

void invq(Q n,Q *nr)
{
  N nm,dn;

  if ( INT(n) )
    if ( UNIN(NM(n)) )
      if ( SGN(n) > 0 )
        *nr = ONE;
      else {
        nm = ONEN; NTOQ(nm,SGN(n),*nr);
      }
    else {
      nm = ONEN; dn = NM(n); NDTOQ(nm,dn,SGN(n),*nr);
    }
  else if ( UNIN(NM(n)) ) {
    nm = DN(n); NTOQ(nm,SGN(n),*nr);
  } else {
    nm = DN(n); dn = NM(n); NDTOQ(nm,dn,SGN(n),*nr);
  }
}

void chsgnq(Q n,Q *nr)
{
  Q t;

  if ( !n )
    *nr = 0;
  else {
    DUPQ(n,t); SGN(t) = -SGN(t); *nr = t;
  }
}

void pwrq(Q n1,Q n,Q *nr)
{
  N nm,dn;
  int sgn;

  if ( !n || UNIQ(n1) ) 
    *nr = ONE;
  else if ( !n1 ) 
    *nr = 0;
  else if ( !INT(n) ) {
    error("can't calculate fractional power."); *nr = 0;
  } else if ( MUNIQ(n1) )
    *nr = BD(NM(n))[0]%2 ? n1 : ONE;
  else if ( PL(NM(n)) > 1 ) {
    error("exponent too big."); *nr = 0;
  } else {
    sgn = ((SGN(n1)>0)||EVENN(NM(n))?1:-1);
    pwrn(NM(n1),BD(NM(n))[0],&nm);
    if ( INT(n1) ) {
      if ( UNIN(nm) )
        if ( sgn == 1 )
          *nr = ONE;
        else
          NTOQ(ONEN,sgn,*nr);
      else if ( SGN(n) > 0 )
        NTOQ(nm,sgn,*nr);
      else
        NDTOQ(ONEN,nm,sgn,*nr);
    } else {
      pwrn(DN(n1),BD(NM(n))[0],&dn);
      if ( SGN(n) > 0 )
        NDTOQ(nm,dn,sgn,*nr);
      else if ( UNIN(nm) )
        NTOQ(dn,sgn,*nr);
      else
        NDTOQ(dn,nm,sgn,*nr);
    }
  }
}

int cmpq(Q q1,Q q2)
{
  int sgn;
  N t,s;

  if ( !q1 ) 
    if ( !q2 ) 
      return ( 0 );
    else 
      return ( -SGN(q2) );
  else if ( !q2 ) 
    return ( SGN(q1) );
  else if ( SGN(q1) != SGN(q2) ) 
      return ( SGN(q1) );
  else if ( INT(q1) )
      if ( INT(q2) ) {
        sgn = cmpn(NM(q1),NM(q2));
        if ( !sgn )
          return ( 0 );
        else
          return ( SGN(q1)==sgn?1:-1 );
      } else {
        kmuln(NM(q1),DN(q2),&t); sgn = cmpn(t,NM(q2));
        return ( SGN(q1) * sgn );
      }
  else if ( INT(q2) ) {
    kmuln(NM(q2),DN(q1),&t); sgn = cmpn(NM(q1),t);
    return ( SGN(q1) * sgn );
  } else {
    kmuln(NM(q1),DN(q2),&t); kmuln(NM(q2),DN(q1),&s); sgn = cmpn(t,s);
    return ( SGN(q1) * sgn );
  }
}

void remq(Q n,Q m,Q *nr)
{
  N q,r,s;

  if ( !n ) {
    *nr = 0;
    return;
  }
  divn(NM(n),NM(m),&q,&r); 
  if ( !r )
    *nr = 0;
  else if ( SGN(n) > 0 )
    NTOQ(r,1,*nr);
  else {
    subn(NM(m),r,&s); NTOQ(s,1,*nr);
  }
}

/* t = [nC0 nC1 ... nCn] */

void mkbc(int n,Q *t)
{
  int i;
  N c,d;

  for ( t[0] = ONE, i = 1; i <= n/2; i++ ) {
    c = NALLOC(1); PL(c) = 1; BD(c)[0] = n-i+1;
    kmuln(NM(t[i-1]),c,&d); divin(d,i,&c); NTOQ(c,1,t[i]);
  }
  for ( ; i <= n; i++ )
    t[i] = t[n-i];
}

/*
 *  Dx^k*x^l = W(k,l,0)*x^l*Dx^k+W(k,l,1)*x^(l-1)*x^(k-1)*+...
 *
 *  t = [W(k,l,0) W(k,l,1) ... W(k,l,min(k,l)]
 *  where W(k,l,i) = i! * kCi * lCi
 */

void mkwc(int k,int l,Q *t)
{
  int i,n,up,low;
  N nm,d,c;

  n = MIN(k,l);
  for ( t[0] = ONE, i = 1; i <= n; i++ ) {
    DM(k-i+1,l-i+1,up,low);
    if ( up ) {
      nm = NALLOC(2); PL(nm) = 2; BD(nm)[0] = low; BD(nm)[1] = up;
    } else {
      nm = NALLOC(1); PL(nm) = 1; BD(nm)[0] = low;
    }
    kmuln(NM(t[i-1]),nm,&d); divin(d,i,&c); NTOQ(c,1,t[i]);
  }
}

/* mod m table */
/* XXX : should be optimized */

void mkwcm(int k,int l,int m,int *t)
{
  int i,n;
  Q *s;

  n = MIN(k,l);
  s = (Q *)ALLOCA((n+1)*sizeof(Q));
  mkwc(k,l,s);
  for ( i = 0; i <= n; i++ ) {
    t[i] = rem(NM(s[i]),m);
  }
}

#if 0
void factorial(int n,Q *r)
{
  Q t,iq,s;
  unsigned int i,m,m0;
  unsigned int c;

  if ( !n )
    *r = ONE;
  else if ( n < 0 ) 
    *r = 0;
  else {
    for ( i = 1, t = ONE; (int)i <= n; ) {
      for ( m0 = m = 1, c = 0; ((int)i <= n) && !c; i++ ) {
        m0 = m; DM(m0,i,c,m)
      }
      if ( c ) {
        m = m0; i--;
      }
      UTOQ(m,iq); mulq(t,iq,&s); t = s;
    }
    *r = t;
  }
}
#endif

void partial_factorial(int s,int e,N *r);

void factorial(int n,Q *r)
{
  N nm;

  if ( !n )
    *r = ONE;
  else if ( n < 0 ) 
    *r = 0;
  else {
    partial_factorial(1,n,&nm);
    NTOQ(nm,1,*r);
  }
}

/* s*(s+1)*...*e */
void partial_factorial(int s,int e,N *r)
{
  unsigned int len,i,m,m0,c;
  unsigned int *p,*p1,*p2;
  N nm,r1,r2;

  /* XXX */
  if ( e-s > 2000 ) {
    m = (e+s)/2;
    partial_factorial(s,m,&r1);
    partial_factorial(m+1,e,&r2);
    kmuln(r1,r2,r);  
    return;
  }
  /* find i s.t. 2^(i-1) < m <= 2^i */
  for ( i = 0, m = 1; m < e; m <<=1, i++ );
   /* XXX estimate of word length of the result */
  len = (i*(e-s+1)+1+31)/32;
  p = ALLOCA((len+1)*sizeof(int));
  p1 = ALLOCA((len+1)*sizeof(int));
  p[0] = s;
  len = 1;
  for ( i = s+1; (int)i <= e; ) {
    for ( m0 = m = 1, c = 0; ((int)i <= e) && !c; i++ ) {
      m0 = m; DM(m0,i,c,m)
    }
    if ( c ) {
      m = m0; i--;
    }
    bzero(p1,(len+1)*sizeof(int));
    muln_1(p,len,m,p1);
    if ( p1[len] )
      len++;
    p2 = p; p = p1; p1 = p2;
  }
  *r = nm = NALLOC(len);
  bcopy(p,BD(nm),sizeof(int)*len);
  PL(nm) = len;
}

void invl(Q a,Q mod,Q *ar)
{
  Q f1,f2,a1,a2,q,m,s;
  N qn,rn;

  a1 = ONE; a2 = 0;
  DUPQ(a,f1); SGN(f1) = 1; f2 = mod;

  while ( 1 ) {
    divn(NM(f1),NM(f2),&qn,&rn); 
    if ( !qn )
      q = 0;
    else 
      NTOQ(qn,1,q);

    f1 = f2; 
    if ( !rn ) 
      break;
    else 
      NTOQ(rn,1,f2);

    mulq(a2,q,&m); subq(a1,m,&s); a1 = a2;
    if ( !s ) 
      a2 = 0;
    else
      remq(s,mod,&a2);
  }
  if ( SGN(a) < 0 ) 
    chsgnq(a2,&m); 
  else
    m = a2;

  if ( SGN(m) >= 0 ) 
    *ar = m;
  else 
    addq(m,mod,ar); 
}

int kara_mag=100;

void kmuln(N n1,N n2,N *nr)
{
  N n,t,s,m,carry;
  int d,d1,d2,len,i,l;
  int *r,*r0;

  if ( !n1 || !n2 ) {
    *nr = 0; return;
  }
  d1 = PL(n1); d2 = PL(n2);
  if ( (d1 < kara_mag) || (d2 < kara_mag) ) {
    muln(n1,n2,nr); return;
  }
  if ( d1 < d2 ) {
    n = n1; n1 = n2; n2 = n;
    d = d1; d1 = d2; d2 = d;
  }
  if ( d2 > (d1+1)/2 ) {
    kmulnmain(n1,n2,nr); return;
  }
  d = (d1/d2)+((d1%d2)!=0);
  len = (d+1)*d2;
  r0 = (int *)ALLOCA(len*sizeof(int));
  bzero((char *)r0,len*sizeof(int));
  for ( carry = 0, i = 0, r = r0; i < d; i++, r += d2 ) {
    extractn(n1,i*d2,d2,&m);
    if ( m ) {
      kmuln(m,n2,&t);
      addn(t,carry,&s);
      copyn(s,d2,r);
      extractn(s,d2,d2,&carry);
    } else {
      copyn(carry,d2,r);
      carry = 0;
    }
  }
  copyn(carry,d2,r);
  for ( l = len - 1; !r0[l]; l-- );
  l++;
  *nr = t = NALLOC(l); PL(t) = l;
  bcopy((char *)r0,(char *)BD(t),l*sizeof(int));
}

void extractn(N n,int index,int len,N *nr)
{
  unsigned int *m;
  int l;
  N t;

  if ( !n ) {
    *nr = 0; return;
  }
  m = BD(n)+index;
  if ( (l = PL(n)-index) >= len ) {
    for ( l = len - 1; (l >= 0) && !m[l]; l-- );
    l++;
  }
  if ( l <= 0 ) {
    *nr = 0; return;
  } else {
    *nr = t = NALLOC(l); PL(t) = l;
    bcopy((char *)m,(char *)BD(t),l*sizeof(int));
  }
}

void copyn(N n,int len,int *p)
{
  if ( n )
    bcopy((char *)BD(n),(char *)p,MIN(PL(n),len)*sizeof(int));
}

void dupn(N n,N p)
{
  if ( n )
    bcopy((char *)n,(char *)p,(1+PL(n))*sizeof(int));
}

void kmulnmain(N n1,N n2,N *nr)
{
  int d1,d2,h,sgn,sgn1,sgn2,len;
  N n1lo,n1hi,n2lo,n2hi,hi,lo,mid1,mid2,mid,s1,s2,t1,t2;

  d1 = PL(n1); d2 = PL(n2); h = (d1+1)/2;
  extractn(n1,0,h,&n1lo); extractn(n1,h,d1-h,&n1hi);
  extractn(n2,0,h,&n2lo); extractn(n2,h,d2-h,&n2hi);
  kmuln(n1hi,n2hi,&hi); kmuln(n1lo,n2lo,&lo);
  len = PL(hi)+2*h; t1 = NALLOC(len); PL(t1) = len;
  bzero((char *)BD(t1),len*sizeof(int));
  if ( lo )
    bcopy((char *)BD(lo),(char *)BD(t1),PL(lo)*sizeof(int));
  if ( hi )
    bcopy((char *)BD(hi),(char *)(BD(t1)+2*h),PL(hi)*sizeof(int));

  addn(hi,lo,&mid1);
  sgn1 = subn(n1hi,n1lo,&s1); sgn2 = subn(n2lo,n2hi,&s2);
  kmuln(s1,s2,&mid2); sgn = sgn1*sgn2;
  if ( sgn > 0 )
    addn(mid1,mid2,&mid);
  else
    subn(mid1,mid2,&mid);
  if ( mid ) {
    len = PL(mid)+h; t2 = NALLOC(len); PL(t2) = len;
    bzero((char *)BD(t2),len*sizeof(int));
    bcopy((char *)BD(mid),(char *)(BD(t2)+h),PL(mid)*sizeof(int));
    addn(t1,t2,nr);
  } else
    *nr = t1;
}
