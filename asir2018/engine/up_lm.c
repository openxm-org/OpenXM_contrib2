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
 * $OpenXM: OpenXM_contrib2/asir2018/engine/up_lm.c,v 1.2 2018/09/28 08:20:28 noro Exp $
*/
#include "ca.h"
#include <math.h>

extern struct oEGT eg_chrem,eg_fore,eg_back;
extern int debug_up;
extern int up_lazy;
extern int current_ff;

void fft_mulup_lm(UP n1,UP n2,UP *nr)
{
  ModNum *f1,*f2,*w,*fr;
  ModNum *frarray[NPrimes];
  int modarray[NPrimes];
  int frarray_index = 0;
  Z m,m1,m2,lm_mod;
  int d1,d2,dmin,i,mod,root,d,cond,bound;
  UP r;

  if ( !n1 || !n2 ) {
    *nr = 0; return;
  }
  d1 = n1->d; d2 = n2->d; dmin = MIN(d1,d2);
  if ( !d1 || !d2 ) {
    mulup(n1,n2,nr); return;
  }
  getmod_lm(&lm_mod);
  if ( !lm_mod )
    error("fft_mulup_lm : current_mod_lm is not set");
  m = ONE;
  bound = maxblenup(n1)+maxblenup(n2)+int_bits(dmin)+2;
  f1 = (ModNum *)ALLOCA((d1+d2+1)*sizeof(ModNum));
  f2 = (ModNum *)ALLOCA((d1+d2+1)*sizeof(ModNum));
  w = (ModNum *)ALLOCA(6*(1<<int_bits(d1+d2+1))*sizeof(ModNum));
  for ( i = 0; i < NPrimes; i++ ) {
    FFT_primes(i,&mod,&root,&d);
    if ( (1<<d) < d1+d2+1 )
      continue;
    modarray[frarray_index] = mod;
    frarray[frarray_index++] = fr 
      = (ModNum *)ALLOCA((d1+d2+1)*sizeof(ModNum));
    uptofmarray(mod,n1,f1); 
    uptofmarray(mod,n2,f2); 
    cond = FFT_pol_product(d1,f1,d2,f2,fr,i,w);
    if ( cond )
      error("fft_mulup : error in FFT_pol_product");
    STOZ(mod,m1); mulz(m,m1,&m2); m = m2;
    if ( z_bits((Q)m) > bound ) {
      crup_lm(frarray,d1+d2,modarray,frarray_index,m,lm_mod,&r);
      uptolmup(r,nr);
      return;
    }
  }
  error("fft_mulup : FFT_primes exhausted");
}

void fft_squareup_lm(UP n1,UP *nr)
{
  ModNum *f1,*w,*fr;
  ModNum *frarray[NPrimes];
  int modarray[NPrimes];
  int frarray_index = 0;
  Z m,m1,m2,lm_mod;
  int d1,dmin,i,mod,root,d,cond,bound;
  UP r;

  if ( !n1 ) {
    *nr = 0; return;
  }
  d1 = n1->d; dmin = d1;
  if ( !d1 ) {
    mulup(n1,n1,nr); return;
  }
  getmod_lm(&lm_mod);
  if ( !lm_mod )
    error("fft_squareup_lm : current_mod_lm is not set");
  m = ONE;
  bound = 2*maxblenup(n1)+int_bits(d1)+2;
  f1 = (ModNum *)ALLOCA((2*d1+1)*sizeof(ModNum));
  w = (ModNum *)ALLOCA(6*(1<<int_bits(2*d1+1))*sizeof(ModNum));
  for ( i = 0; i < NPrimes; i++ ) {
    FFT_primes(i,&mod,&root,&d);
    if ( (1<<d) < 2*d1+1 )
      continue;
    modarray[frarray_index] = mod;
    frarray[frarray_index++] = fr 
      = (ModNum *)ALLOCA((2*d1+1)*sizeof(ModNum));
    uptofmarray(mod,n1,f1); 
    cond = FFT_pol_square(d1,f1,fr,i,w);
    if ( cond )
      error("fft_mulup : error in FFT_pol_product");
    STOZ(mod,m1); mulz(m,m1,&m2); m = m2;
    if ( z_bits((Q)m) > bound ) {
      crup_lm(frarray,2*d1,modarray,frarray_index,m,lm_mod,&r);
      uptolmup(r,nr);
      return;
    }
  }
  error("fft_squareup : FFT_primes exhausted");
}

void trunc_fft_mulup_lm(UP n1,UP n2,int dbd,UP *nr)
{
  ModNum *f1,*f2,*fr,*w;
  ModNum *frarray[NPrimes];
  int modarray[NPrimes];
  int frarray_index = 0;
  Z m,m1,m2,lm_mod;
  int d1,d2,dmin,i,mod,root,d,cond,bound;
  UP r;

  if ( !n1 || !n2 ) {
    *nr = 0; return;
  }
  d1 = n1->d; d2 = n2->d; dmin = MIN(d1,d2);
  if ( !d1 || !d2 ) {
    tmulup(n1,n2,dbd,nr); return;
  }
  getmod_lm(&lm_mod);
  if ( !lm_mod )
    error("trunc_fft_mulup_lm : current_mod_lm is not set");
  m = ONE;
  bound = maxblenup(n1)+maxblenup(n2)+int_bits(dmin)+2;
  f1 = (ModNum *)ALLOCA((d1+d2+1)*sizeof(ModNum));
  f2 = (ModNum *)ALLOCA((d1+d2+1)*sizeof(ModNum));
  w = (ModNum *)ALLOCA(6*(1<<int_bits(d1+d2+1))*sizeof(ModNum));
  for ( i = 0; i < NPrimes; i++ ) {
    FFT_primes(i,&mod,&root,&d);
    if ( (1<<d) < d1+d2+1 )
      continue;

    modarray[frarray_index] = mod;
    frarray[frarray_index++] = fr 
      = (ModNum *)ALLOCA((d1+d2+1)*sizeof(ModNum));
    uptofmarray(mod,n1,f1); 
    uptofmarray(mod,n2,f2);
    cond = FFT_pol_product(d1,f1,d2,f2,fr,i,w);
    if ( cond )
      error("fft_mulup : error in FFT_pol_product");
    STOZ(mod,m1); mulz(m,m1,&m2); m = m2;
    if ( z_bits((Q)m) > bound ) {
      crup_lm(frarray,MIN(dbd-1,d1+d2),modarray,frarray_index,m,lm_mod,&r);
      uptolmup(r,nr);
      return;
    }
  }
  error("trunc_fft_mulup : FFT_primes exhausted");
}

void crup_lm(ModNum **f,int d,int *mod,int index,Z m,Z lm_mod,UP *r)
{
  UP w;
  Z t;
  int i;

  crup(f,d,mod,index,m,&w);
  for ( i = 0; i <= d; i++ ) {
    remz((Z)w->c[i],lm_mod,&t); w->c[i] = (Num)t;
  }
  for ( i = d; (i >= 0) && (w->c[i] == 0); i-- );
  if ( i < 0 ) *r = 0;
  else {
    w->d = i;
    *r = w;
  }
}

void fft_rembymulup_special_lm(UP n1,UP n2,UP inv2,UP *nr)
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
    trunc_fft_mulup_lm(r1,inv2,d+1,&t);
    truncup(t,d+1,&s);
    reverseup(s,d,&q);
    trunc_fft_mulup_lm(q,n2,d2,&t); truncup(t,d2,&u);
    truncup(n1,d2,&s);
    subup(s,u,nr);
  }
}

void uptolmup(UP n,UP *nr)
{
  int i,d;
  Q *c;
  LM *cr;
  UP r;

  if ( !n )
    *nr = 0;
  else {
    d = n->d; c = (Q *)n->c;
    *nr = r = UPALLOC(d); cr = (LM *)r->c;
    for ( i = 0; i <= d; i++ )
      qtolm(c[i],&cr[i]);
    for ( i = d; i >= 0 && !cr[i]; i-- );
    if ( i < 0 )
      *nr = 0;
    else
      r->d = i;
  }
}

void save_up(UP obj,char *name)
{
  P p;
  Obj ret;
  NODE n0,n1;
  STRING s;
  void Pbsave();

  uptop(obj,&p);
  MKSTR(s,name);
  MKNODE(n1,s,0); MKNODE(n0,p,n1);
  Pbsave(n0,&ret);
}

void hybrid_powermodup(UP f,UP *xp)
{
  Z n;
  UP x,y,t,invf,s,s1;
  int k;
  LM lm;

  getmod_lm(&n);
  if ( !n )
    error("hybrid_powermodup : current_mod_lm is not set");
  lm = ONELM;
  x = UPALLOC(1); x->d = 1; x->c[1] = (Num)lm;
  y = UPALLOC(0); y->d = 0; y->c[0] = (Num)lm;

  reverseup(f,f->d,&t);
  invmodup(t,f->d,&s); uptolmup(s,&invf);
  for ( k = z_bits((Q)n)-1; k >= 0; k-- ) {
    hybrid_squareup(FF_GFP,y,&t);
    hybrid_rembymulup_special(FF_GFP,t,f,invf,&s);
    y = s;
    if ( tstbitz(n,k) ) {
      mulup(y,x,&t);
      remup(t,f,&s);
      y = s;
    }
  }
  *xp = y;
}

void powermodup(UP f,UP *xp)
{
  Z n;
  UP x,y,t,invf,s;
  int k;
  Num c;

  if ( !current_ff )
    error("powermodup : current_ff is not set");
  field_order_ff(&n);
  one_ff(&c);
  x = UPALLOC(1); x->d = 1; x->c[1] = c;
  y = UPALLOC(0); y->d = 0; y->c[0] = c;

  reverseup(f,f->d,&t);
  invmodup(t,f->d,&s); 
  switch ( current_ff ) {
    case FF_GFP:
    case FF_GFPN:
      uptolmup(s,&invf);
      break;
    case FF_GFS:
    case FF_GFSN:
      invf = s; /* XXX */
      break;
    default:
      error("powermodup : not implemented yet");
  }
  for ( k = z_bits((Q)n)-1; k >= 0; k-- ) {
    ksquareup(y,&t);
    rembymulup_special(t,f,invf,&s);
    y = s;
    if ( tstbitz(n,k) ) {
      mulup(y,x,&t);
      remup(t,f,&s);
      y = s;
    }
  }
  *xp = y;
}

/* g^d mod f */

void hybrid_generic_powermodup(UP g,UP f,Z e,UP *xp)
{
  UP x,y,t,invf,s;
  int k;
  LM lm;

  lm = ONELM;
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
    hybrid_squareup(FF_GFP,y,&t);
    hybrid_rembymulup_special(FF_GFP,t,f,invf,&s);
    y = s;
    if ( tstbitz(e,k) ) {
      mulup(y,x,&t);
      remup(t,f,&s);
      y = s;
    }
  }
  *xp = y;
}

void generic_powermodup(UP g,UP f,Z e,UP *xp)
{
  UP x,y,t,invf,s;
  int k;
  Num c;

  one_ff(&c);
  y = UPALLOC(0); y->d = 0; y->c[0] = c;
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
    ksquareup(y,&t);
    rembymulup_special(t,f,invf,&s);
    y = s;
    if ( tstbitz(e,k) ) {
      mulup(y,x,&t);
      remup(t,f,&s);
      y = s;
    }
  }
  *xp = y;
}

void hybrid_powertabup(UP f,UP xp,UP *tab)
{
  UP y,t,invf;
  int i,d;
  LM lm;

  d = f->d;
  lm = ONELM;
  y = UPALLOC(0); y->d = 0; y->c[0] = (Num)lm;
  tab[0] = y;
  tab[1] = xp;

  reverseup(f,f->d,&t);
  invmodup(t,f->d,&invf);

  for ( i = 2; i < d; i++ ) {
    if ( debug_up ){
      fprintf(stderr,".");
    }
    hybrid_mulup(FF_GFP,tab[i-1],xp,&t);
    hybrid_rembymulup_special(FF_GFP,t,f,invf,&tab[i]);
  }
}

void powertabup(UP f,UP xp,UP *tab)
{
  UP y,t,invf;
  int i,d;
  Num c;

  d = f->d;
  one_ff(&c);
  y = UPALLOC(0); y->d = 0; y->c[0] = c;
  tab[0] = y;
  tab[1] = xp;

  reverseup(f,f->d,&t);
  invmodup(t,f->d,&invf);

  for ( i = 2; i < d; i++ ) {
    if ( debug_up ){
      fprintf(stderr,".");
    }
    kmulup(tab[i-1],xp,&t);
    rembymulup_special(t,f,invf,&tab[i]);
  }
}
