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
 * $OpenXM: OpenXM_contrib2/asir2018/engine/up_gf2n.c,v 1.1 2018/09/19 05:45:07 noro Exp $
*/
#include "ca.h"
#include <math.h>

extern int debug_up;
extern int up_lazy;
extern GEN_UP2 current_mod_gf2n;

void squarep_gf2n(VL vl,P n1,P *nr)
{
  UP b1,br;

  if ( !n1 )
    *nr = 0;
  else if ( OID(n1) == O_N )
    mulp(vl,n1,n1,nr);
  else {
    ptoup(n1,&b1);
    squareup_gf2n(b1,&br);
    uptop(br,nr);
  }
}

void squareup_gf2n(UP n1,UP *nr)
{
  UP r;
  GF2N *c1,*c;
  int i,d1,d;

  if ( !n1 )
    *nr = 0;
  else if ( !n1->d ) {
    *nr = r = UPALLOC(0); r->d = 0;
    squaregf2n((GF2N)n1->c[0],(GF2N *)(&r->c[0]));
  } else {
    d1 = n1->d;
    d = 2*d1;
    *nr = r = UPALLOC(d); r->d = d;
    c1 = (GF2N *)n1->c; c = (GF2N *)r->c;
    bzero((char *)c,(d+1)*sizeof(GF2N *));
    for ( i = 0; i <= d1; i++ )
      squaregf2n(c1[i],&c[2*i]);
  }
}

/* x^(2^n) mod f */

void powermodup_gf2n(UP f,UP *xp)
{
  UP x,t,invf;
  int k,n;
  GF2N lm;

  n = degup2(current_mod_gf2n->dense);
  MKGF2N(ONEUP2,lm);
  x = UPALLOC(1); x->d = 1; x->c[1] = (Num)lm;

  reverseup(f,f->d,&t);
  invmodup(t,f->d,&invf);
  for ( k = 0; k < n; k++ ) {
    squareup_gf2n(x,&t);
    rembymulup_special(t,f,invf,&x);
/*    remup(t,f,&x); */
  }
  *xp = x;
}

/* g^d mod f */

void generic_powermodup_gf2n(UP g,UP f,Z e,UP *xp)
{
  UP x,y,t,invf,s;
  int k;
  GF2N lm;

  MKGF2N(ONEUP2,lm);
  y = UPALLOC(0); y->d = 0; y->c[0] = (Num)lm;
  remup(g,f,&x);
  if ( !x ) {
    *xp = !e ? y : 0;
    return;
  } else if ( !x->d ) {
    pwrup(x,e,xp);
    return;
  }
  reverseup(f,f->d,&t);
  invmodup(t,f->d,&invf);
  for ( k = z_bits((Q)e)-1; k >= 0; k-- ) {
    squareup_gf2n(y,&t);
    rembymulup_special(t,f,invf,&s);
    y = s;
    if ( mpz_tstbit(BDY(e),k) ) {
      mulup(y,x,&t);
      remup(t,f,&s);
      y = s;
    }
  }
  *xp = y;
}

/* g+g^2+...+g^(2^(nd-1)) mod f; where e = deg(mod) */

void tracemodup_gf2n(UP g,UP f,Z d,UP *xp)
{
  UP x,t,s,u,invf;
  int en,i;

  en = ZTOS(d)*degup2(current_mod_gf2n->dense);
  remup(g,f,&x);
  if ( !x ) {
    *xp = 0;
    return;
  }
  reverseup(f,f->d,&t);
  invmodup(t,f->d,&invf);
  for ( i = 1, t = s = x; i < en; i++ ) {
    squareup_gf2n(t,&u);
    rembymulup_special(u,f,invf,&t);
    addup(s,t,&u); s = u;
  }
  *xp = s;
}

void tracemodup_gf2n_slow(UP g,UP f,Z d,UP *xp)
{
  UP x,t,s,u;
  int en,i;

  en = ZTOS(d)*degup2(current_mod_gf2n->dense);
  remup(g,f,&x);
  if ( !x ) {
    *xp = 0;
    return;
  }
  for ( i = 1, t = s = x; i < en; i++ ) {
    squareup_gf2n(t,&u);
    remup(u,f,&t);
    addup(s,t,&u); s = u;
  }
  *xp = s;
}

void tracemodup_gf2n_tab(UP g,UP f,Z d,UP *xp)
{
  UP x0,x2,t,s,u;
  int en,i;
  UP *tab;
  GF2N one;

  en = ZTOS(d)*degup2(current_mod_gf2n->dense);
  remup(g,f,&t); g = t;
  if ( !g ) {
    *xp = 0;
    return;
  }

  MKGF2N(ONEUP2,one);
  x0 = UPALLOC(0); x0->d = 0; x0->c[0] = (Num)one;
  x2 = UPALLOC(2); x2->d = 2; x2->c[2] = (Num)one;

  tab = (UP *)ALLOCA(en*sizeof(UP));
  tab[0] = x0;
  remup(x2,f,&tab[1]);

  for ( i = 2; i < en; i++ ) {
    mulup(tab[i-1],tab[1],&t); remup(t,f,&tab[i]);
  }

  for ( i = 1, t = s = g; i < en; i++ ) {
    square_rem_tab_up_gf2n(t,tab,&u); t = u;
    addup(s,t,&u); s = u;
  }
  *xp = s;
}

void square_rem_tab_up_gf2n(UP f,UP *tab,UP *rp)
{
  UP s,t,u,n;
  Num *c;
  int i,d;

  n = UPALLOC(0); n->d = 0;
  if ( !f )
    *rp = 0;
  else {
    d = f->d; c = f->c;
    up_lazy = 1;
    for ( i = 0, s = 0; i <= d; i++ ) {
      squaregf2n((GF2N)c[i],(GF2N *)(&n->c[0]));
      mulup(tab[i],n,&t); addup(s,t,&u); s = u;
    }
    up_lazy = 0;
    simpup(s,rp);
  }
}

void powertabup_gf2n(UP f,UP xp,UP *tab)
{
  UP y,t,invf;
  int i,d;
  GF2N lm;

  d = f->d;
  MKGF2N(ONEUP2,lm);
  y = UPALLOC(0); y->d = 0; y->c[0] = (Num)lm;
  tab[0] = y;
  tab[1] = xp;

  reverseup(f,f->d,&t);
  invmodup(t,f->d,&invf);

  for ( i = 2; i < d; i++ ) {
    if ( debug_up ){
      fprintf(stderr,".");
    }
    if ( !(i%2) )
      squareup_gf2n(tab[i/2],&t);
    else
      kmulup(tab[i-1],xp,&t);
    rembymulup_special(t,f,invf,&tab[i]);
/*    remup(t,f,&tab[i]); */
  }
}

void find_root_gf2n(UP f,GF2N *r)
{
  UP g,ut,c,t,h,rem;
  int n;
  GF2N rn;

  n = degup2(current_mod_gf2n->dense);
  g = f;
  while ( g->d > 1 ) {
    ut = UPALLOC(1); ut->c[0] = 0; 
    randomgf2n(&rn);
    if ( !rn )
      continue;
    ut->c[1] = (Num)rn; ut->d = 1;
    tracemodup_gf2n_tab(ut,f,ONE,&c);
    gcdup(c,g,&h);
    if ( h->d && h->d < g->d ) {
      if ( 2*h->d > g->d ) {
        qrup(g,h,&t,&rem); g = t;
        if ( rem )
          error("find_root_gf2n : cannot happen");
      } else
        g = h;
    }
    monicup(g,&t); g = t;
    printf("deg(g)=%d\n",g->d);
  }
  divgf2n((GF2N)g->c[0],(GF2N)g->c[1],r);
}
