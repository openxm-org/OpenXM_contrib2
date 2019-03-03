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
 * $OpenXM: OpenXM_contrib2/asir2018/engine/up.c,v 1.2 2018/09/28 08:20:28 noro Exp $
*/
#include "ca.h"
#include <math.h>

struct oEGT eg_chrem,eg_fore,eg_back;

/*
int up_kara_mag=15;
int up_tkara_mag=15;
*/
/*
int up_kara_mag=30;
int up_tkara_mag=20;
*/
int up_kara_mag=20;
int up_tkara_mag=15;

#if defined(sparc)
int up_fft_mag=50;
#else
int up_fft_mag=80;
#endif

int up_rem_mag=1;
int debug_up;
int up_lazy;
extern int lm_lazy;
extern int current_ff;
extern int GC_dont_gc;

void monicup(UP a,UP *b)
{
  UP w;

  if ( !a )
    *b = 0;
  else {
    w = W_UPALLOC(0); w->d = 0;
    divnum(0,(Num)ONE,(Num)a->c[a->d],(Num *)&w->c[0]);
    mulup(a,w,b);
  }
}

void simpup(UP a,UP *b)
{
  int i,d;
  UP c;

  if ( !a )
    *b = 0;
  else {
    d = a->d;
    c = UPALLOC(d);
  
    for ( i = 0; i <= d; i++ )
      simpnum(a->c[i],&c->c[i]);
    for ( i = d; i >= 0 && !c->c[i]; i-- );
    if ( i < 0 )
      *b = 0;
    else {
      c->d = i;
      *b = c;
    }
  }
}

void simpnum(Num a,Num *b)
{
  LM lm;
  GF2N gf;
  GFPN gfpn;

  if ( !a )
    *b = 0;
  else
    switch ( NID(a) ) {
      case N_LM:
        simplm((LM)a,&lm); *b = (Num)lm; break;
      case N_GF2N:
        simpgf2n((GF2N)a,&gf); *b = (Num)gf; break;
      case N_GFPN:
        simpgfpn((GFPN)a,&gfpn); *b = (Num)gfpn; break;
      default:
        *b = a; break;
    }
}

void uremp(P p1,P p2,P *rp)
{
  UP n1,n2,r;

  if ( !p1 || NUM(p1) )
    *rp = p1;  
  else {
    ptoup(p1,&n1); ptoup(p2,&n2);
    remup(n1,n2,&r);
    uptop(r,rp);
  }
}

void ugcdp(P p1,P p2,P *rp)
{
  UP n1,n2,r;

  ptoup(p1,&n1); ptoup(p2,&n2);
  gcdup(n1,n2,&r);
  uptop(r,rp);
}

void reversep(P p1,Q d,P *rp)
{
  UP n1,r;

  if ( !p1 )
    *rp = 0;
  else {
    ptoup(p1,&n1);
    reverseup(n1,ZTOS(d),&r);
    uptop(r,rp);
  }
}

void invmodp(P p1,Q d,P *rp)
{
  UP n1,r;

  if ( !p1 )
    error("invmodp : invalid input");
  else {
    ptoup(p1,&n1);
    invmodup(n1,ZTOS(d),&r);
    uptop(r,rp);
  }
}

void addup(UP n1,UP n2,UP *nr)
{
  UP r,t;
  int i,d1,d2;
  Num *c,*c1,*c2;

  if ( !n1 ) {
    *nr = n2;
  } else if ( !n2 ) {
    *nr = n1;
  } else {
    if ( n2->d > n1->d ) {
      t = n1; n1 = n2; n2 = t;
    }
    d1 = n1->d; d2 = n2->d;
    *nr = r = UPALLOC(d1);
    c1 = n1->c; c2 = n2->c; c = r->c;
    for ( i = 0; i <= d2; i++ )
      addnum(0,*c1++,*c2++,c++);
    for ( ; i <= d1; i++ )
      *c++ = *c1++;
    for ( i = d1, c = r->c; i >=0 && !c[i]; i-- );
    if ( i < 0 )
      *nr = 0;
    else
      r->d = i;
  }
}

void subup(UP n1,UP n2,UP *nr)
{
  UP r;
  int i,d1,d2,d;
  Num *c,*c1,*c2;

  if ( !n1 ) {
    chsgnup(n2,nr);
  } else if ( !n2 ) {
    *nr = n1;
  } else {
    d1 = n1->d; d2 = n2->d; d = MAX(d1,d2);
    *nr = r = UPALLOC(d);
    c1 = n1->c; c2 = n2->c; c = r->c;
    if ( d1 >= d2 ) {
      for ( i = 0; i <= d2; i++ )
        subnum(0,*c1++,*c2++,c++);
      for ( ; i <= d1; i++ )
        *c++ = *c1++;
    } else {
      for ( i = 0; i <= d1; i++ )
        subnum(0,*c1++,*c2++,c++);
      for ( ; i <= d2; i++ )
        chsgnnum(*c2++,c++);
    }
    for ( i = d, c = r->c; i >=0 && !c[i]; i-- );
    if ( i < 0 )
      *nr = 0;
    else
      r->d = i;
  }
}

void chsgnup(UP n1,UP *nr)
{
  UP r;
  int d1,i;
  Num *c1,*c;

  if ( !n1 ) {
    *nr = 0;
  } else {
    d1 = n1->d;
    *nr = r = UPALLOC(d1); r->d = d1;
    c1 = n1->c; c = r->c;
    for ( i = 0; i <= d1; i++ )
      chsgnnum(*c1++,c++);
  }
}

void hybrid_mulup(int ff,UP n1,UP n2,UP *nr)
{
  if ( !n1 || !n2 )
    *nr = 0;
  else if ( MAX(n1->d,n2->d) < up_fft_mag )
    kmulup(n1,n2,nr);
  else
    switch ( ff ) {
      case FF_GFP:
        fft_mulup_lm(n1,n2,nr); break;
      case FF_GF2N:
        kmulup(n1,n2,nr); break;
      default:
        fft_mulup(n1,n2,nr); break;
    }
}

void hybrid_squareup(int ff,UP n1,UP *nr)
{
  if ( !n1 )
    *nr = 0;
  else if ( n1->d < up_fft_mag )
    ksquareup(n1,nr);
  else
    switch ( ff ) {
      case FF_GFP:
        fft_squareup_lm(n1,nr); break;
      case FF_GF2N:
        ksquareup(n1,nr); break;
      default:
        fft_squareup(n1,nr); break;
    }
}

void hybrid_tmulup(int ff,UP n1,UP n2,int d,UP *nr)
{
  UP t;

  if ( !n1 || !n2 )
    *nr = 0;
  else if ( MAX(n1->d,n2->d) < up_fft_mag )
    tkmulup(n1,n2,d,nr);
  else 
    switch ( ff ) {
      case FF_GFP:
        trunc_fft_mulup_lm(n1,n2,d,nr); break;
      case FF_GF2N:
        tkmulup(n1,n2,d,nr); break;
      default:
        trunc_fft_mulup(n1,n2,d,nr); break;
    }
}

void mulup(UP n1,UP n2,UP *nr)
{
  UP r;
  Num *pc1,*pc,*c1,*c2,*c;
  Num mul,t,s;
  int i,j,d1,d2;

  if ( !n1 || !n2 )
    *nr = 0;
  else {
    d1 = n1->d; d2 = n2->d;
    *nr = r = UPALLOC(d1+d2); r->d = d1+d2;
    c1 = n1->c; c2 = n2->c; c = r->c;
    lm_lazy = 1;
    for ( i = 0; i <= d2; i++, c++ ) 
      if ( (mul = *c2++) != 0 ) 
        for ( j = 0, pc1 = c1, pc = c; j <= d1; j++ ) {
          mulnum(0,*pc1++,mul,&t); addnum(0,*pc,t,&s); *pc++ = s;
      }
    lm_lazy = 0;
    if ( !up_lazy )
      for ( i = 0, c = r->c; i <= r->d; i++ ) {
        simpnum(c[i],&t); c[i] = t;
      }
  }
}

/* nr = c*n1 */

void mulcup(Num c,UP n1,UP *nr)
{
  int d;
  UP r;
  Num t;
  Num *c1,*cr;
  int i;

  if ( !c || !n1 )
    *nr = 0;
  else {
    d = n1->d;
    *nr = r = UPALLOC(d); r->d = d;
    c1 = n1->c; cr = r->c;
    lm_lazy = 1;
    for ( i = 0; i <= d; i++ )
      mulnum(0,c1[i],c,&cr[i]);
    lm_lazy = 0;
    if ( !up_lazy )
      for ( i = 0; i <= d; i++ ) {
        simpnum(cr[i],&t); cr[i] = t;
      }
  }
}

void tmulup(UP n1,UP n2,int d,UP *nr)
{
  UP r;
  Num *pc1,*pc,*c1,*c2,*c;
  Num mul,t,s;
  int i,j,d1,d2,dr;

  if ( !n1 || !n2 )
    *nr = 0;
  else {
    d1 = n1->d; d2 = n2->d;
    dr = MAX(d-1,d1+d2);
    *nr = r = UPALLOC(dr);
    c1 = n1->c; c2 = n2->c; c = r->c;
    lm_lazy = 1;
    for ( i = 0; i <= d2; i++, c++ ) 
      if ( ( mul = *c2++ ) != 0 ) 
        for ( j = 0, pc1 = c1, pc = c; (j <= d1) && (i+j < d); j++ ) {
          mulnum(0,*pc1++,mul,&t); addnum(0,*pc,t,&s); *pc++ = s;
      }
    lm_lazy = 0;
    c = r->c;
    if ( !up_lazy )
      for ( i = 0; i < d; i++ ) {
        simpnum(c[i],&t); c[i] = t;
      }
    for ( i = d-1; i >= 0 && !c[i]; i-- );
    if ( i < 0 )
      *nr = 0;
    else
      r->d = i;
  }
}

void squareup(UP n1,UP *nr)
{
  UP r;
  Num *c1,*c;
  Num t,s,u;
  int i,j,h,d1,d;

  if ( !n1 )
    *nr = 0;
  else if ( !n1->d ) {
    *nr = r = UPALLOC(0); r->d = 0;
    mulnum(0,n1->c[0],n1->c[0],&r->c[0]);
  } else {
    d1 = n1->d;
    d = 2*d1;
    *nr = r = UPALLOC(2*d); r->d = d;
    c1 = n1->c; c = r->c;
    lm_lazy = 1;
    for ( i = 0; i <= d1; i++ )
      mulnum(0,c1[i],c1[i],&c[2*i]);
    for ( j = 1; j < d; j++ ) {
      h = (j-1)/2;
      for ( i = MAX(j-d1,0), s = 0; i <= h; i++ ) {
        mulnum(0,c1[i],c1[j-i],&t); addnum(0,t,s,&u); s = u;
      }
      addnum(0,s,s,&t); addnum(0,t,c[j],&u); c[j] = u;
    }
    lm_lazy = 0;
    if ( !up_lazy )
      for ( i = 0, c = r->c; i <= d; i++ ) {
        simpnum(c[i],&t); c[i] = t;
      }
  }
}

void remup(UP n1,UP n2,UP *nr)
{
  UP w,r;

  if ( !n2 )
    error("remup : division by 0");
  else if ( !n1 || !n2->d )
    *nr = 0;
  else if ( n1->d < n2->d )
    *nr = n1;
  else {
    w = W_UPALLOC(n1->d); copyup(n1,w);
    remup_destructive(w,n2);
    if ( w->d < 0 )
      *nr = 0;
    else {
      *nr = r = UPALLOC(w->d); copyup(w,r);
    }
  }
}

void remup_destructive(UP n1,UP n2)
{
  Num *c1,*c2;
  int d1,d2,i,j;
  Num m,t,s,mhc;

  c1 = n1->c; c2 = n2->c;
  d1 = n1->d; d2 = n2->d;
  chsgnnum(c2[d2],&mhc);
  for ( i = d1; i >= d2; i-- ) {
    simpnum(c1[i],&t); c1[i] = t;
    if ( !c1[i] )
      continue;
    divnum(0,c1[i],mhc,&m);
    lm_lazy = 1;
    for ( j = d2; j >= 0; j-- ) {
      mulnum(0,c2[j],m,&t); addnum(0,c1[i+j-d2],t,&s);
      c1[i+j-d2] = s;
    }
    lm_lazy = 0;
  }
  for ( i = 0; i < d2; i++ ) {
    simpnum(c1[i],&t); c1[i] = t;
  }
  for ( i = d2-1; i >= 0 && !c1[i]; i-- );
  n1->d = i;
}

void qrup(UP n1,UP n2,UP *nq,UP *nr)
{
  UP w,r,q;
  struct oUP t;
  Num m;
  int d;

  if ( !n2 )
    error("qrup : division by 0");
  else if ( !n1 ) {
    *nq = 0; *nr = 0;
  } else if ( !n2->d )
    if ( !n1->d ) {
      divnum(0,n1->c[0],n2->c[0],(Num *)nq); *nr = 0;
    } else {
      divnum(0,(Num)ONE,n2->c[0],&m);
      t.d = 0; t.c[0] = m;
      mulup(n1,&t,nq); *nr = 0;
    }
  else if ( n1->d < n2->d ) {
    *nq = 0; *nr = n1;
  } else {
    w = W_UPALLOC(n1->d); copyup(n1,w);
    qrup_destructive(w,n2);
    d = n1->d-n2->d; 
    *nq = q = UPALLOC(d); q->d = d;
    bcopy(w->c+n2->d,q->c,(d+1)*sizeof(Num));
    if ( w->d < 0 )
      *nr = 0;
    else {
      *nr = r = UPALLOC(w->d); copyup(w,r);
    }
  }
}

void qrup_destructive(UP n1,UP n2)
{
  Num *c1,*c2;
  int d1,d2,i,j;
  Num q,t,s,hc;

  c1 = n1->c; c2 = n2->c;
  d1 = n1->d; d2 = n2->d;
  hc = c2[d2];
  for ( i = d1; i >= d2; i-- ) {
    simpnum(c1[i],&t); c1[i] = t;
    if ( c1[i] ) {
      divnum(0,c1[i],hc,&q);
      lm_lazy = 1;
      for ( j = d2; j >= 0; j-- ) {
        mulnum(0,c2[j],q,&t); subnum(0,c1[i+j-d2],t,&s);
        c1[i+j-d2] = s;
      }
      lm_lazy = 0;
    } else
      q = 0;
    c1[i] = q;
  }
  for ( i = 0; i < d2; i++ ) {
    simpnum(c1[i],&t); c1[i] = t;
  }
  for ( i = d2-1; i >= 0 && !c1[i]; i-- );
  n1->d = i;
}

void gcdup(UP n1,UP n2,UP *nr)
{
  UP w1,w2,t,r;
  int d1,d2;

  if ( !n1 )
    *nr = n2;
  else if ( !n2 )
    *nr = n1;
  else if ( !n1->d || !n2->d ) {
    *nr = r = UPALLOC(0); r->d = 0;
    divnum(0,n1->c[n1->d],n1->c[n1->d],&r->c[0]);
  } else {
    d1 = n1->d; d2 = n2->d;
    if ( d2 > d1 ) {
      w1 = W_UPALLOC(d2); copyup(n2,w1);
      w2 = W_UPALLOC(d1); copyup(n1,w2);
    } else {
      w1 = W_UPALLOC(d1); copyup(n1,w1);
      w2 = W_UPALLOC(d2); copyup(n2,w2);
    }
    d1 = w1->d; d2 = w2->d;
    while ( 1 ) {
      remup_destructive(w1,w2);
      if ( w1->d < 0 ) {
        *nr = r = UPALLOC(w2->d); copyup(w2,r); return;
      } else if ( !w1->d ) {
        *nr = r = UPALLOC(0); r->d = 0;
        divnum(0,n1->c[n1->d],n1->c[n1->d],&r->c[0]); return;
      } else {
        t = w1; w1 = w2; w2 = t;
      }
    }
  }
}

/* compute r s.t. a*r = 1 mod m */

void extended_gcdup(UP a,UP m,UP *r)
{
  UP one,g1,g2,a1,a2,a3,b1,b2,b3,inv,quo,rem,t;
  Num i;

  one = UPALLOC(0); one->d = 0; one->c[0] = (Num)ONELM;
  g1 = m; g2 = a;
  a1 = one; a2 = 0;
  b1 = 0; b2 = one;
  /* a2*m+b2*a = g2 always holds */
  while ( g2->d ) {
    qrup(g1,g2,&quo,&rem); g1 = g2; g2 = rem;
    mulup(a2,quo,&t); subup(a1,t,&a3); a1 = a2; a2 = a3;
    mulup(b2,quo,&t); subup(b1,t,&b3); b1 = b2; b2 = b3;
  }
  /* now a2*m+b2*a = g2 (const) */
  divnum(0,(Num)ONE,g2->c[0],&i);
  inv = UPALLOC(0); inv->d = 0; inv->c[0] = i;
  mulup(b2,inv,r);
}

void reverseup(UP n1,int d,UP *nr)
{
  Num *c1,*c;
  int i,d1;
  UP r;

  if ( !n1 )
    *nr = 0;
  else if ( d < (d1 = n1->d) )
    error("reverseup : invalid input");
  else {
    *nr = r = UPALLOC(d);
    c = r->c;
    bzero((char *)c,(d+1)*sizeof(Num));
    c1 = n1->c;
    for ( i = 0; i <= d1; i++ )
      c[d-i] = c1[i];
    for ( i = d; i >= 0 && !c[i]; i-- );
    r->d = i;
    if ( i < 0 )
      *nr = 0;
  }
}

void invmodup(UP n1,int d,UP *nr)
{
  UP r;
  Num s,t,u,hinv;
  int i,j,dmin;
  Num *w,*c,*cr;

  if ( !n1 || !n1->c[0] )
    error("invmodup : invalid input");
  else if ( !n1->d ) {
    *nr = r = UPALLOC(0); r->d = 0;
    divnum(0,(Num)ONE,n1->c[0],&r->c[0]);
  } else {
    *nr = r = UPALLOC(d);
    w = (Num *)ALLOCA((d+1)*sizeof(Q));
    bzero((char *)w,(d+1)*sizeof(Q));
    dmin = MIN(d,n1->d);
    divnum(0,(Num)ONE,n1->c[0],&hinv);
    for ( i = 0, c = n1->c; i <= dmin; i++ )
      mulnum(0,c[i],hinv,&w[i]);
    cr = r->c;
    cr[0] = w[0];
    for ( i = 1; i <= d; i++ ) {
      for ( j = 1, s = w[i]; j < i; j++ ) {
        mulnum(0,cr[j],w[i-j],&t);
        addnum(0,s,t,&u);
        s = u;
      }
      chsgnnum(s,&cr[i]);
    }
    for ( i = 0; i <= d; i++ ) {
      mulnum(0,cr[i],hinv,&t); cr[i] = t;
    }
    for ( i = d; i >= 0 && !cr[i]; i-- );
    r->d = i;
  }
}

void pwrup(UP n,Z e,UP *nr)
{
  UP y,z,t;
  Z q,r,two;

  y = UPALLOC(0); y->d = 0; y->c[0] = (Num)ONE;
  z = n;
  if ( !e )
    *nr = y;
  else if ( UNIQ(e) )
    *nr = n;
  else {
    STOZ(2,two);
    while ( 1 ) {
      divqrz(e,two,&q,&r); e = q;
      if ( r ) {
        mulup(z,y,&t); y = t;
        if ( !e ) {
          *nr = y;
          return;
        }
      }
      mulup(z,z,&t); z = t;
    }
  }
}

int compup(UP n1,UP n2)
{
  int i,r;

  if ( !n1 )
    return n2 ? -1 : 0;
  else if ( !n2 )
    return 1;
  else if ( n1->d > n2->d )
    return 1;
  else if ( n1->d < n2->d )
    return -1;
  else {
    for ( i = n1->d; i >= 0; i-- ) {
      r = compnum(CO,n1->c[i],n2->c[i]);    
      if ( r )
        return r;
    }
    return 0;
  }
}

void kmulp(VL vl,P n1,P n2,P *nr)
{
  UP b1,b2,br;

  if ( !n1 || !n2 )
    *nr = 0;
  else if ( OID(n1) == O_N || OID(n2) == O_N )
    mulp(vl,n1,n2,nr);
  else {
    ptoup(n1,&b1); ptoup(n2,&b2);
    kmulup(b1,b2,&br);
    uptop(br,nr);
  }
}

void ksquarep(VL vl,P n1,P *nr)
{
  UP b1,br;

  if ( !n1 )
    *nr = 0;
  else if ( OID(n1) == O_N )
    mulp(vl,n1,n1,nr);
  else {
    ptoup(n1,&b1);
    ksquareup(b1,&br);
    uptop(br,nr);
  }
}

void kmulup(UP n1,UP n2,UP *nr)
{
  int d1,d2;

  if ( !n1 || !n2 ) {
    *nr = 0; return;
  }
  d1 = DEG(n1)+1; d2 = DEG(n2)+1;
  if ( (d1 < up_kara_mag) && (d2 < up_kara_mag) )
    mulup(n1,n2,nr);
  else
    kmulupmain(n1,n2,nr);
}

void ksquareup(UP n1,UP *nr)
{
  int d1;

  if ( !n1 ) {
    *nr = 0; return;
  }
  d1 = DEG(n1)+1;
  if ( (d1 < up_kara_mag) )
    squareup(n1,nr);
  else
    ksquareupmain(n1,nr);
}

void copyup(UP n1,UP n2)
{
  n2->d = n1->d;
  bcopy((char *)n1->c,(char *)n2->c,(n1->d+1)*sizeof(Q));
}

void c_copyup(UP n,int len,pointer *p)
{
  if ( n )
    bcopy((char *)COEF(n),(char *)p,MIN((DEG(n)+1),len)*sizeof(pointer));
}

void kmulupmain(UP n1,UP n2,UP *nr)
{
  int d1,d2,h;
  UP n1lo,n1hi,n2lo,n2hi,hi,lo,mid1,mid2,mid,s1,s2,t1,t2;

  d1 = DEG(n1)+1; d2 = DEG(n2)+1; h = (MAX(d1,d2)+1)/2;
  decompup(n1,h,&n1lo,&n1hi);
  decompup(n2,h,&n2lo,&n2hi);
  kmulup(n1lo,n2lo,&lo);
  kmulup(n1hi,n2hi,&hi);
  shiftup(hi,2*h,&t2);
  addup(lo,t2,&t1);
  addup(hi,lo,&mid1);
  subup(n1hi,n1lo,&s1); subup(n2lo,n2hi,&s2); kmulup(s1,s2,&mid2);
  addup(mid1,mid2,&mid);
  shiftup(mid,h,&t2);
  addup(t1,t2,nr);
}

void ksquareupmain(UP n1,UP *nr)
{
  int d1,h;
  UP n1lo,n1hi,hi,lo,mid1,mid2,mid,s1,t1,t2;

  d1 = DEG(n1)+1; h = (d1+1)/2;
  decompup(n1,h,&n1lo,&n1hi);
  ksquareup(n1hi,&hi); ksquareup(n1lo,&lo);
  shiftup(hi,2*h,&t2);
  addup(lo,t2,&t1);
  addup(hi,lo,&mid1);
  subup(n1hi,n1lo,&s1); ksquareup(s1,&mid2);
  subup(mid1,mid2,&mid);
  shiftup(mid,h,&t2);
  addup(t1,t2,nr);
}

void rembymulup(UP n1,UP n2,UP *nr)
{
  int d1,d2,d;
  UP r1,r2,inv2,t,s,q;

  if ( !n2 )
    error("rembymulup : division by 0");
  else if ( !n1 || !n2->d )
    *nr = 0;
  else if ( (d1 = n1->d) < (d2 = n2->d) )
    *nr = n1;
  else {
    d = d1-d2;
    reverseup(n1,d1,&t); truncup(t,d+1,&r1);
    reverseup(n2,d2,&r2);
    invmodup(r2,d,&inv2);
    kmulup(r1,inv2,&t);
    truncup(t,d+1,&s);
    reverseup(s,d,&q);
    kmulup(q,n2,&t); subup(n1,t,nr);
  }
}

/*
  deg n2 = d
  deg n1 <= 2*d-1
  inv2 = inverse of reversep(n2) mod x^d
*/

void hybrid_rembymulup_special(int ff,UP n1,UP n2,UP inv2,UP *nr)
{
  int d1,d2,d;
  UP r1,t,s,q,t1;

  if ( !n2 )
    error("hybrid_rembymulup : division by 0");
  else if ( !n1 || !n2->d )
    *nr = 0;
  else if ( (d1 = n1->d) < (d2 = n2->d) )
    *nr = n1;
  else {
    d = d1-d2;
    reverseup(n1,d1,&t); truncup(t,d+1,&r1);
    hybrid_tmulup(ff,r1,inv2,d+1,&t);
    truncup(t,d+1,&s);
    reverseup(s,d,&q);
    
    hybrid_tmulup(ff,q,n2,d2,&t);
    truncup(n1,d2,&s);
    subup(s,t,nr);
  }
}

void rembymulup_special(UP n1,UP n2,UP inv2,UP *nr)
{
  int d1,d2,d;
  UP r1,t,s,q;

  if ( !n2 )
    error("rembymulup : division by 0");
  else if ( !n1 || !n2->d )
    *nr = 0;
  else if ( (d1 = n1->d) < (d2 = n2->d) )
    *nr = n1;
  else {
    d = d1-d2;
    reverseup(n1,d1,&t); truncup(t,d+1,&r1);
    tkmulup(r1,inv2,d+1,&t);
    truncup(t,d+1,&s);
    reverseup(s,d,&q);
    
    tkmulup(q,n2,d2,&t);
    truncup(n1,d2,&s);
    subup(s,t,nr);
  }
}

/* *nr = n1*n2 mod x^d */

void tkmulup(UP n1,UP n2,int d,UP *nr)
{
  int m;
  UP n1l,n1h,n2l,n2h,l,h,t,s,u;

  if ( d < 0 )
    error("tkmulup : invalid argument");
  else if ( d == 0 )
    *nr = 0;
  else {
    truncup(n1,d,&t); n1 = t;
    truncup(n2,d,&t); n2 = t;
    if ( !n1 || !n2 )
      *nr = 0;
    else if ( n1->d < up_tkara_mag || n2->d < up_tkara_mag )
      tmulup(n1,n2,d,nr);
    else {
      m = (d+1)/2;
      decompup(n1,m,&n1l,&n1h);
      decompup(n2,m,&n2l,&n2h);
      kmulup(n1l,n2l,&l);
      tkmulup(n1l,n2h,d-m,&t);
      tkmulup(n2l,n1h,d-m,&s);
      addup(t,s,&u);
      shiftup(u,m,&h);
      addup(l,h,&t);
      truncup(t,d,nr);
    }
  }
}

/* n->n*x^d */

void shiftup(UP n,int d,UP *nr)
{
  int dr;
  UP r;

  if ( !n )
    *nr = 0;
  else {
    dr = n->d+d;
    *nr = r = UPALLOC(dr); r->d = dr;
    bzero(r->c,(dr+1)*sizeof(Num));
    bcopy(n->c,r->c+d,(n->d+1)*sizeof(Num));
  }
}

void fft_rembymulup_special(UP n1,UP n2,UP inv2,UP *nr)
{
  int d1,d2,d;
  UP r1,t,s,q,u;

  if ( !n2 )
    error("rembymulup : division by 0");
  else if ( !n1 || !n2->d )
    *nr = 0;
  else if ( (d1 = n1->d) < (d2 = n2->d) )
    *nr = n1;
  else {
    d = d1-d2;
    reverseup(n1,d1,&t); truncup(t,d+1,&r1);
    trunc_fft_mulup(r1,inv2,d+1,&t);
    truncup(t,d+1,&s);
    reverseup(s,d,&q);
    trunc_fft_mulup(q,n2,d2,&t); truncup(t,d2,&u);
    truncup(n1,d2,&s);
    subup(s,u,nr);
  }
}

void set_degreeup(UP n,int d)
{
  int i;

  for ( i = d; i >= 0 && !n->c[i]; i-- );
  n->d = i;
}

/* n -> n0 + x^d n1 */

void decompup(UP n,int d,UP *n0,UP *n1)
{
  int dn;
  UP r0,r1;

  if ( !n || d > n->d ) {
    *n0 = n; *n1 = 0;
  } else if ( d < 0 )
    error("decompup : invalid argument");
  else if ( d == 0 ) {
    *n0 = 0; *n1 = n;
  } else {
    dn = n->d;
    *n0 = r0 = UPALLOC(d-1);
    *n1 = r1 = UPALLOC(dn-d);
    bcopy(n->c,r0->c,d*sizeof(Num));
    bcopy(n->c+d,r1->c,(dn-d+1)*sizeof(Num));
    set_degreeup(r0,d-1);
    if ( r0->d < 0 )
      *n0 = 0;
    set_degreeup(r1,dn-d);
    if ( r1->d < 0 )
      *n1 = 0;
  }
}

/* n -> n mod x^d */

void truncup(UP n1,int d,UP *nr)
{
  int i;
  UP r;

  if ( !n1 || d > n1->d )
    *nr = n1;
  else if ( d < 0 )
    error("truncup : invalid argument");
  else if ( d == 0 )
    *nr = 0;
  else {
     *nr = r = UPALLOC(d-1);
    bcopy(n1->c,r->c,d*sizeof(Num));
    for ( i = d-1; i >= 0 && !r->c[i]; i-- );
    if ( i < 0 )
      *nr = 0;
    else
      r->d = i;
  }
}

int int_bits(int t)
{
  int k;

  for ( k = 0; t; t>>=1, k++);
  return k;
}

/* n is assumed to be LM or integer coefficient */

int maxblenup(UP n)
{
  int m,r,i,d;
  Num *c;

  if ( !n )
    return 0;
  d = n->d; c = (Num *)n->c;
  for ( r = 0, i = 0; i <= d; i++ )
    if ( c[i] ) {
      switch ( NID(c[i]) ) {
        case N_LM:
          m = mpz_sizeinbase(BDY((LM)c[i]),2);
          break;
        case N_Q:
          m = mpz_sizeinbase(BDY((Z)c[i]),2);
          break;
        default:
          error("maxblenup : invalid coefficient");
      }
      r = MAX(m,r);
    }
  return r;
}

/* YYY */

void uptofmarray(int mod,UP n,ModNum *f)
{
  int d,i;
  unsigned int r;
  Num *c;
  Z z;

  if ( !n )
    return;
  else {
    d = n->d; c = (Num *)n->c;
    for ( i = 0; i <= d; i++ ) {
      if ( c[i] ) {
        switch ( NID(c[i]) ) {
          case N_LM:
            MPZTOZ(BDY((LM)c[i]),z);
            f[i] = remqi((Q)z,mod);
            break;
          case N_Q:
            f[i] = remqi((Q)c[i],mod);
            break;
          default:
            error("uptofmarray : invalid coefficient");
        }
      } else
        f[i] = 0;
    }
  }
}

void fmarraytoup(ModNum *f,int d,UP *nr)
{
  int i;
  Z *c;
  UP r;

  if ( d < 0 ) {
    *nr = 0;
  } else {
    *nr = r = UPALLOC(d); c = (Z *)r->c;
    for ( i = 0; i <= d; i++ ) {
      UTOZ((unsigned int)f[i],c[i]);
    }
    for ( i = d; i >= 0 && !c[i]; i-- );
    if ( i < 0 )
      *nr = 0;
    else
      r->d = i;
  }
}

/* f[i]: an array of length n */

void uiarraytoup(unsigned int **f,int n,int d,UP *nr)
{
  int i,j;
  Z *c;
  UP r;

  if ( d < 0 ) {
    *nr = 0;
  } else {
    *nr = r = UPALLOC(d); c = (Z *)r->c;
    for ( i = 0; i <= d; i++ ) 
      intarraytoz(n,f[i],&c[i]); 
    for ( i = d; i >= 0 && !c[i]; i-- );
    if ( i < 0 )
      *nr = 0;
    else
      r->d = i;
  }
}

void adj_coefup(UP n,Z m,Z m2,UP *nr)
{
  int d;
  Z *c,*cr;
  Z mq;
  int i;
  UP r;

  if ( !n )
    *nr = 0;
  else {
    d = n->d; c = (Z *)n->c;
    *nr = r = UPALLOC(d); cr = (Z *)r->c;
    for ( i = 0; i <= d; i++ ) {
      if ( !c[i] )
        continue;
      if ( cmpz(c[i],m2) > 0 )
        subz(c[i],m,&cr[i]);
      else
        cr[i] = c[i];
    }
    for ( i = d; i >= 0 && !cr[i]; i-- );
    if ( i < 0 )
      *nr = 0;
    else
      r->d = i;
  }
}

/* n is assumed to have positive integer coefficients. */

void remcup(UP n,Z mod,UP *nr)
{
  int i,d;
  Z *c,*cr;
  UP r;
  Z t;

  if ( !n )
    *nr = 0;
  else {
    d = n->d; c = (Z *)n->c;
    *nr = r = UPALLOC(d); cr = (Z *)r->c;
    for ( i = 0; i <= d; i++ )
      remz(c[i],mod,&cr[i]);
    for ( i = d; i >= 0 && !cr[i]; i-- );
    if ( i < 0 )
      *nr = 0;
    else
      r->d = i;
  }
}

void fft_mulup_main(UP,UP,int,UP *);

void fft_mulup(UP n1,UP n2,UP *nr)
{
  int d1,d2,d,b1,b2,h;
  UP n1lo,n1hi,n2lo,n2hi,lo,hi,t1,t2,mid1,mid2,mid,s1,s2;

  if ( !n1 || !n2 )
    *nr = 0;
  else {
    d1 = n1->d; d2 = n2->d;
    if ( MAX(d1,d2) < up_fft_mag )
      kmulup(n1,n2,nr);
    else {
      b1 = maxblenup(n1); b2 = maxblenup(n2);
      if ( fft_available(d1,b1,d2,b2) )
        fft_mulup_main(n1,n2,0,nr);
      else {
        d = MAX(d1,d2)+1; h = (d+1)/2;
        decompup(n1,h,&n1lo,&n1hi);
        decompup(n2,h,&n2lo,&n2hi);
        fft_mulup(n1lo,n2lo,&lo);
        fft_mulup(n1hi,n2hi,&hi);
        shiftup(hi,2*h,&t2);
        addup(lo,t2,&t1);
        addup(hi,lo,&mid1);
        subup(n1hi,n1lo,&s1); subup(n2lo,n2hi,&s2);
        fft_mulup(s1,s2,&mid2);
        addup(mid1,mid2,&mid);
        shiftup(mid,h,&t2);
        addup(t1,t2,nr);
      }
    }
  }
}

void trunc_fft_mulup(UP n1,UP n2,int dbd,UP *nr)
{
  int d1,d2,b1,b2,m;
  UP n1l,n1h,n2l,n2h,l,h,t,s,u;

  if ( !n1 || !n2 )
    *nr = 0;
  else if ( dbd < 0 )
    error("trunc_fft_mulup : invalid argument");
  else if ( dbd == 0 )
    *nr = 0;
  else {
    truncup(n1,dbd,&t); n1 = t;
    truncup(n2,dbd,&t); n2 = t;
    d1 = n1->d; d2 = n2->d;
    if ( MAX(d1,d2) < up_fft_mag )
      tkmulup(n1,n2,dbd,nr);
    else {
      b1 = maxblenup(n1); b2 = maxblenup(n2);
      if ( fft_available(d1,b1,d2,b2) )
        fft_mulup_main(n1,n2,dbd,nr);
      else {
        m = (dbd+1)/2;
        decompup(n1,m,&n1l,&n1h);
        decompup(n2,m,&n2l,&n2h);
        fft_mulup(n1l,n2l,&l);
        trunc_fft_mulup(n1l,n2h,dbd-m,&t);
        trunc_fft_mulup(n2l,n1h,dbd-m,&s);
        addup(t,s,&u);
        shiftup(u,m,&h);
        addup(l,h,&t);
        truncup(t,dbd,nr);
      }
    }
  }
}

void fft_squareup(UP n1,UP *nr)
{
  int d1,d,h,b1;
  UP n1lo,n1hi,hi,lo,mid1,mid2,mid,s1,t1,t2;

  if ( !n1 )
    *nr = 0;
  else {
    d1 = n1->d;
    if ( d1 < up_fft_mag )
      ksquareup(n1,nr);
    else {
      b1 = maxblenup(n1);
      if ( fft_available(d1,b1,d1,b1) )
        fft_mulup_main(n1,n1,0,nr);
      else {
        d = d1+1; h = (d1+1)/2;
        decompup(n1,h,&n1lo,&n1hi);
        fft_squareup(n1hi,&hi);
        fft_squareup(n1lo,&lo);
        shiftup(hi,2*h,&t2);
        addup(lo,t2,&t1);
        addup(hi,lo,&mid1);
        subup(n1hi,n1lo,&s1);
        fft_squareup(s1,&mid2);
        subup(mid1,mid2,&mid);
        shiftup(mid,h,&t2);
        addup(t1,t2,nr);
      }
    }
  }
}

/* 
 * dbd == 0 => n1 * n2
 * dbd > 0  => n1 * n2 mod x^dbd
 * n1 == n2 => squaring
 */

void fft_mulup_main(UP n1,UP n2,int dbd,UP *nr)
{
  ModNum *f1,*f2,*w,*fr;
  ModNum **frarray,**fa;
  int *modarray,*ma;
  int modarray_len;
  int frarray_index = 0;
  Z m,m1,m2,two,rem;
  int d1,d2,dmin,i,mod,root,d,cond,bound;
  UP r;

  if ( !n1 || !n2 ) {
    *nr = 0; return;
  }
  d1 = n1->d; d2 = n2->d; dmin = MIN(d1,d2);
  if ( !d1 || !d2 ) {
    mulup(n1,n2,nr); return;
  }
  m = ONE;
  bound = maxblenup(n1)+maxblenup(n2)+int_bits(dmin)+1;
  f1 = (ModNum *)MALLOC_ATOMIC((d1+d2+1)*sizeof(ModNum));
  if ( n1 == n2 )
    f2 = 0;
  else
    f2 = (ModNum *)MALLOC_ATOMIC((d1+d2+1)*sizeof(ModNum));
  w = (ModNum *)MALLOC_ATOMIC(6*(1<<int_bits(d1+d2+1))*sizeof(ModNum));
  modarray_len = 1024;
  modarray = (int *)MALLOC_ATOMIC(modarray_len*sizeof(unsigned int));
  frarray = (ModNum **)MALLOC(modarray_len*sizeof(ModNum *));
  for ( i = 0; i < NPrimes; i++ ) {
    FFT_primes(i,&mod,&root,&d);
    if ( (1<<d) < d1+d2+1 )
      continue;
    if ( frarray_index == modarray_len ) {
      ma = (int *)MALLOC_ATOMIC(2*modarray_len*sizeof(unsigned int));
      bcopy(modarray,ma,modarray_len*sizeof(unsigned int));
      modarray = ma;
      fa = (ModNum **)MALLOC(2*modarray_len*sizeof(ModNum *));
      bcopy(frarray,fa,modarray_len*sizeof(ModNum *));
      frarray = fa;
      modarray_len *= 2;
    }
    modarray[frarray_index] = mod;
    frarray[frarray_index++] = fr 
      = (ModNum *)MALLOC_ATOMIC((d1+d2+1)*sizeof(ModNum));
    uptofmarray(mod,n1,f1); 
    if ( !f2 )
      cond = FFT_pol_square(d1,f1,fr,i,w);
    else {
      uptofmarray(mod,n2,f2); 
      cond = FFT_pol_product(d1,f1,d2,f2,fr,i,w);
    }
    if ( cond )
      error("fft_mulup : error in FFT_pol_product");
    STOZ(mod,m1); mulz(m,m1,&m2); m = m2;
    if ( z_bits((Q)m) > bound ) {
      if ( !dbd )
        dbd = d1+d2+1;
      crup(frarray,MIN(d1+d2,dbd-1),modarray,frarray_index,m,&r);
      STOZ(2,two);
      divqrz(m,two,&m2,&rem);
      adj_coefup(r,m,m2,nr);
      return;
    }
  }
  error("fft_mulup : FFT_primes exhausted");
}

/* improved version */

void crup(ModNum **f,int d,int *mod,int index,Z m,UP *r)
{
  mpz_t cof,c,rem;
  mpz_t *s;
  UP u;
  Z z;
  int inv,i,j,t;

  mpz_init(c); mpz_init(cof); mpz_init(rem);
  s = (mpz_t *)MALLOC((d+1)*sizeof(mpz_t));
  for ( j = 0; j <= d; j++ )
    mpz_init_set_ui(s[j],0);
  for ( i = 0; i < index; i++ ) {
    mpz_fdiv_q_ui(cof,BDY(m),mod[i]);
    t = mpz_fdiv_r_ui(rem,cof,mod[i]);
    inv = invm(t,mod[i]);
    mpz_mul_ui(c,cof,inv);
    /* s += c*f[i] */
    for ( j = 0; j <= d; j++ )
      if ( f[i][j] )
        mpz_addmul_ui(s[j],c,f[i][j]);
  }
  for ( i = d; i >= 0; i-- )
    if ( s[i] )
      break;
  if ( i < 0 )
    *r = 0;
  else {
    u = UPALLOC(i);
    DEG(u) = i;
    for ( j = 0; j <= i; j++ ) {
      MPZTOZ(s[j],z); COEF(u)[j] = (Num)z;
    }
    remcup(u,m,r);
  }
}

/* 
 * dbd == 0 => n1 * n2
 * dbd > 0  => n1 * n2 mod x^dbd
 * n1 == n2 => squaring
 * return: n1*n2 mod Primes[modind[0]]*.prime...*Primes[modind[nmod-1]].prime
 */

void fft_mulup_specialmod_main(UP n1,UP n2,int dbd,int *modind,int nmod,UP *nr)
{
  ModNum *f1,*f2,*w,*fr;
  ModNum **frarray;
  Z m,m1,m2;
  int *modarray;
  int d1,d2,dmin,i,root,d,cond,bound;

  if ( !n1 || !n2 ) {
    *nr = 0; return;
  }
  d1 = n1->d; d2 = n2->d; dmin = MIN(d1,d2);
  if ( !d1 || !d2 ) {
    mulup(n1,n2,nr); return;
  }
  m = ONE;
  bound = maxblenup(n1)+maxblenup(n2)+int_bits(dmin)+1;
  f1 = (ModNum *)MALLOC_ATOMIC((d1+d2+1)*sizeof(ModNum));
  if ( n1 == n2 )
    f2 = 0;
  else
    f2 = (ModNum *)MALLOC_ATOMIC((d1+d2+1)*sizeof(ModNum));
  w = (ModNum *)MALLOC_ATOMIC(6*(1<<int_bits(d1+d2+1))*sizeof(ModNum));
  frarray = (ModNum **)MALLOC(nmod*sizeof(ModNum *));
  modarray = (int *)MALLOC_ATOMIC(nmod*sizeof(unsigned int *));

  for ( i = 0; i < nmod; i++ ) {
    FFT_primes(modind[i],&modarray[i],&root,&d);
      if ( (1<<d) < d1+d2+1 )
        error("fft_mulup_specialmod_main : invalid modulus");
    frarray[i] = fr 
      = (ModNum *)MALLOC_ATOMIC((d1+d2+1)*sizeof(ModNum));
    uptofmarray(modarray[i],n1,f1); 
    if ( !f2 )
      cond = FFT_pol_square(d1,f1,fr,modind[i],w);
    else {
      uptofmarray(modarray[i],n2,f2); 
      cond = FFT_pol_product(d1,f1,d2,f2,fr,modind[i],w);
    }
    if ( cond )
      error("fft_mulup_specialmod_main : error in FFT_pol_product");
    STOZ(modarray[i],m1); mulz(m,m1,&m2); m = m2;
  }
  if ( !dbd )
    dbd = d1+d2+1;
  crup(frarray,MIN(d1+d2,dbd-1),modarray,nmod,m,nr);
}
