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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/upo.c,v 1.5 2015/08/14 13:51:55 fujimoto Exp $ 
*/
#include "ca.h"
#include "base.h"

void find_root_up2();

#define INLINE

#if defined(VISUAL) || defined(__MINGW32__)
#undef INLINE
#define INLINE __inline
#endif

#if defined(linux)
#undef INLINE
#define INLINE inline
#endif

static int up2_bbtab_initialized;
static unsigned short up2_sqtab[256];
static unsigned short up2_bbtab[65536];

int up2_kara_mag = 10;
V up2_var;

extern GEN_UP2 current_mod_gf2n;
extern int lm_lazy;

#define GCD_COEF(q,p1,p2,p3,q1,q2,q3)\
switch (q){\
case 2:(p3)=(p1)^((p2)<<1);(q3)=(q1)^((q2)<<1);break;\
case 3:(p3)=(p1)^((p2)<<1)^(p2);(q3)=(q1)^((q2)<<1)^(q2);break;\
case 4:(p3)=(p1)^((p2)<<2);(q3)=(q1)^((q2)<<2);break;\
case 5:(p3)=(p1)^((p2)<<2)^(p2);(q3)=(q1)^((q2)<<2)^(q2);break;\
case 6:(p3)=(p1)^((p2)<<2)^((p2)<<1);(q3)=(q1)^((q2)<<2)^((q2)<<1);break;\
case 7:(p3)=(p1)^((p2)<<2)^((p2)<<1)^(p2);(q3)=(q1)^((q2)<<2)^((q2)<<1)^(q2);break;\
default:(p3)=(p1)^mulup2_bb((p2),(q));(q3)=(q1)^mulup2_bb((q2),(q));break;\
}\

#if defined(P5)

#define GF2M_MUL_1_HALF(high,low,a,b) (low)=NNgf2m_mul_1h(&(high),a,b);
#define GF2M_MUL_1(high,low,a,b) (low)=NNgf2m_mul_11(&(high),a,b);

#else

/*
 * 32bit x 16bit -> 64bit (48bit); for b <= 0xffff
 * a short-cut version of GF2M_MUL_1
 */

#define GF2M_MUL_1_HALF(high,low,a,b)\
{\
  unsigned int _a,_b,_c,_d,_t,_s;\
  unsigned int _ll,_cc;\
  _a = (a);\
  _b = (b);\
  _c = _a&0xff00ff00;  /* c = a4 0 a2 0 */\
  _a ^= _c;        /* a = 0 a3 0 a1 */\
  _d = _b&0xff00ff00;  /* d = 0 0 b2 0 */\
  _b ^= _d;        /* b = 0 0 0 b1 */\
  _c |= (_d>>8);    /* c = a4 00 a2 b2 */\
  _b |= (_a<<8);    /* b = a3 00 a1 b1 */\
  _a = (_c>>16);    /* a = a4 00 */\
  _c &= 0xffff;    /* c = a2 b2 */\
  _d = (_b>>16);    /* d = a3 00 */\
  _b &= 0xffff;    /* b = a1 b1 */\
  _t = up2_bbtab[_c]; _s = up2_bbtab[_b];\
  _ll = ((_t^_s^up2_bbtab[_c^_b])<<8)^(_t<<16)^_s;\
  _a ^= _c; _d^= _b;\
  _t = up2_bbtab[_a]; _s = up2_bbtab[_d];\
  _cc = ((_t^_s^up2_bbtab[_a^_d])<<8)^(_t<<16)^_s;\
  _cc ^= _ll;\
  (low) = (unsigned int)(_ll^(_cc<<16));\
  (high) = (unsigned int)(_cc>>16);\
}

/*
 * 32bit x 32bit -> 64bit
 * This is an inline expansion of 4byte x 4byte Karatsuba multiplication
 * Necessary indices for up2_bbtab are efficiently generated.
 */

#define GF2M_MUL_1(high,low,a,b)\
{\
  unsigned int _a,_b,_c,_d,_t,_s;\
  unsigned int _uu,_ll,_cc;\
  _a = (a);\
  _b = (b);\
  _c = _a&0xff00ff00;  /* _c = a4 0 a2 0 */\
  _a ^= _c;      /*  a = 0 a3 0 a1 */\
  _d = _b&0xff00ff00;  /* _d = b4 0 b2 0 */\
  _b ^= _d;      /*  b = 0 b3 0 b1 */\
  _c |= (_d>>8);    /* _c = a4 b4 a2 b2 */\
  _b |= (_a<<8);    /* b = a3 b3 a1 b1 */\
  _a = (_c>>16);    /* a = a4 b4 */\
  _c &= 0xffff;    /* _c = a2 b2 */\
  _d = (_b>>16);    /* _d = a3 b3 */\
  _b &= 0xffff;    /* b = a1 b1 */\
  _t = up2_bbtab[_a]; _s = up2_bbtab[_d];\
  _uu = ((_t^_s^up2_bbtab[_a^_d])<<8)^(_t<<16)^_s;\
  _t = up2_bbtab[_c]; _s = up2_bbtab[_b];\
  _ll = ((_t^_s^up2_bbtab[_c^_b])<<8)^(_t<<16)^_s;\
  _a ^= _c; _d^= _b;\
  _t = up2_bbtab[_a]; _s = up2_bbtab[_d];\
  _cc = ((_t^_s^up2_bbtab[_a^_d])<<8)^(_t<<16)^_s;\
  _cc ^= (_ll^_uu);\
  (low) = (unsigned int)(_ll^(_cc<<16));\
  (high) = (unsigned int)(_uu^(_cc>>16));\
}
#endif

#define REMUP2_ONESTEP(a,q,s,n,w)\
if ( !s ) a[q] ^= w;\
else {\
  a[q] ^= (w<<s);\
  if ( q+1 <= n )\
    a[q+1] ^= (w>>(32-s));\
}

void ptoup2(n,nr)
P n;
UP2 *nr;
{
  DCP dc;
  UP2 r,s;
  int d,w,i;

  if ( !n )
    *nr = 0;
  else if ( OID(n) == O_N )
    if ( EVENN(NM((Q)n)) )
      *nr = 0;
    else
      *nr = ONEUP2;
  else {
    d = UDEG(n); w = (d+1)/BSH + ((d+1)%BSH?1:0);
    up2_var = VR(n);
    W_NEWUP2(r,w);
    for ( dc = DC(n); dc; dc = NEXT(dc) )
      if ( !EVENN(NM((Q)COEF(dc))) ) {
        i = QTOS(DEG(dc));
        r->b[i/BSH] |= 1<<(i%BSH);
      }
    for ( i = w-1; i >= 0 && !r->b[i]; i-- );
    r->w = i+1;
    NEWUP2(s,r->w); *nr = s;
    _copyup2(r,s);
  }
}

void ptoup2_sparse(n,nr)
P n;
UP2 *nr;
{
  DCP dc;
  UP2 s;
  int d,i;

  if ( !n )
    *nr = 0;
  else if ( OID(n) == O_N )
    if ( EVENN(NM((Q)n)) )
      *nr = 0;
    else {
      NEWUP2(s,1); s->w = 1; s->b[0] = 0;
      *nr = s;
    }
  else {
    d = UDEG(n);
    for ( dc = DC(n), i = 0; dc; dc = NEXT(dc) )
      if ( !EVENN(NM((Q)COEF(dc))) )
        i++;
    NEWUP2(s,i); s->w = i; *nr = s;
    for ( dc = DC(n), i = 0; dc; dc = NEXT(dc) )
      if ( !EVENN(NM((Q)COEF(dc))) )
        s->b[i++] = QTOS(DEG(dc));
  }
}

void up2top(n,nr)
UP2 n;
P *nr;
{
  int i,d;
  DCP dc0,dc;

  if ( !n )
    *nr = 0;
  else if ( !(d = degup2(n)) )
    *nr = (P)ONE;
  else {
    for ( i = d, dc0 = 0; i >= 0; i-- )
      if ( n->b[i/BSH] & (1<<(i%BSH)) ) {
        NEXTDC(dc0,dc); STOQ(i,DEG(dc)); COEF(dc) = (P)ONE;
      }
    if ( !up2_var )
      up2_var = CO->v;
    MKP(up2_var,dc0,*nr);
  }
}

void up2tovect(n,nr)
UP2 n;
VECT *nr;
{
  int i,d;
  VECT v;

  if ( !n )
    *nr = 0;
  else {
    d = degup2(n);
    MKVECT(v,d+1); *nr = v;
    for ( i = d; i >= 0; i-- )
      if ( n->b[i/BSH] & (1<<(i%BSH)) )
        v->body[i] = (pointer)ONE;
  }
}

void up2ton(p,n)
UP2 p;
Q *n;
{
  N nm;
  int w;

  if ( !p )
    *n = 0;
  else {
    w = p->w;
    nm = NALLOC(w);
    nm->p = w;
    bcopy(p->b,nm->b,w*sizeof(unsigned int));
    NTOQ(nm,1,*n);
  }
}

void ntoup2(n,p)
Q n;
UP2 *p;
{
  N nm;
  UP2 t;
  int w;

  if ( !n )
    *p = 0;
  else {
    nm = NM(n); w = nm->p;
    NEWUP2(t,w); *p = t; t->w = w;
    bcopy(nm->b,t->b,w*sizeof(unsigned int));
  }
}

void gen_simpup2(p,m,r)
UP2 p;
GEN_UP2 m;
UP2 *r;
{
  if ( lm_lazy || !m )
    *r = p;
  else if ( m->id == UP2_DENSE )
    remup2(p,m->dense,r);
  else
    remup2_sparse(p,m->sparse,r);
  if ( *r && !((*r)->w) )
    *r = 0;
}

void gen_simpup2_destructive(p,m)
UP2 p;
GEN_UP2 m;
{
  UP2 t;

  if ( lm_lazy || !m )
    return;
  else if ( m->id == UP2_DENSE ) {
    remup2(p,m->dense,&t);
    _copyup2(t,p);
  } else
    remup2_sparse_destructive(p,m->sparse);
}

void gen_invup2(p,m,r)
UP2 p;
GEN_UP2 m;
UP2 *r;
{
  if ( !m )
    error("gen_invup2 : invalid modulus");
  else 
    invup2(p,m->dense,r);
}

void gen_pwrmodup2(a,b,m,c)
UP2 a;
Q b;
GEN_UP2 m;
UP2 *c;
{
  if ( !m )
    pwrmodup2(a,b,0,c);
  else if ( m->id == UP2_DENSE )
    pwrmodup2(a,b,m->dense,c);
  else
    pwrmodup2_sparse(a,b,m->sparse,c);
}

void simpup2(p,m,r)
UP2 p,m;
UP2 *r;
{
  if ( !lm_lazy && m )
    remup2(p,m,r);
  else
    *r = p;
}

int degup2(a)
UP2 a;
{
  unsigned int l,i,t;

  if ( !a || !a->w )
    return -1;
  else {
    l = a->w; t = a->b[l-1];
    for ( i = 0; t; t>>=1, i++);
    return i+(l-1)*BSH-1;
  }
}

int degup2_sparse(a)
UP2 a;
{
  if ( !a || !a->w )
    return -1;
  else
    return a->b[0];
}

int degup2_1(a)
unsigned int a;
{
  int i;

  for ( i = 0; a; a>>=1, i++);
  return i-1;
}

void addup2(a,b,c)
UP2 a,b;
UP2 *c;
{
  int i;
  UP2 t;
  int w,wa,wb;

  if ( !a )
    *c = b;
  else if ( !b )
    *c = a;
  else {
    wa = a->w; wb = b->w;
    if ( wa < wb ) {
      t = a; a = b; b = t;
      w = wa; wa = wb; wb = w;
    }
    NEWUP2(t,wa); 
    for ( i = 0; i < wb; i++ )
      t->b[i] = a->b[i]^b->b[i];
    for ( ; i < wa; i++ )
      t->b[i] = a->b[i];
    for ( i = wa-1; i >= 0 && !t->b[i]; i-- );
    if ( i < 0 )
      *c = 0;
    else {
      t->w = i+1;
      *c = t;
    }
  }
}

void subup2(a,b,c)
UP2 a,b;
UP2 *c;
{
  addup2(a,b,c);
}


/* 
       s[w-1] ... s[0]
x                   m
---------------------
   t[w]t[w-1] ... t[0]
+  r[w]r[w-1] ... r[0]
---------------------
*/

INLINE void mulup2_n1(s,w,m,r)
unsigned int *s,*r;
int w;
unsigned int m;
{
  int i;
  unsigned int _u,_l,t;

  for ( i = 0; i < w; i++ )
    if ( t = s[i] ) {
      GF2M_MUL_1(_u,_l,t,m);
      r[i] ^= _l; r[i+1] ^= _u;
    }
}

INLINE void mulup2_nh(s,w,m,r)
unsigned int *s,*r;
int w;
unsigned int m; /* 0 <= b <= 0xffff */
{
  int i;
  unsigned int u,l;

  for ( i = 0, r[0] = 0; i < w; i++ ) {
    GF2M_MUL_1_HALF(u,l,s[i],m);
    r[i] ^= l; r[i+1] = u;
  }
}

void _mulup2_1(a,b,c)
UP2 a,c;
unsigned int b;
{
  int w;

  if ( !a || !a->w || !b )
    c->w = 0;
  else if ( b <= 0xffff )
    _mulup2_h(a,b,c);
  else {
    w = a->w;
    bzero((char *)c->b,(w+1)*sizeof(unsigned int));
    mulup2_n1(a->b,w,b,c->b);
    c->w = c->b[w] ? w+1 : w;
  }
}

void _mulup2_h(a,b,c)
UP2 a,c;
unsigned int b; /* 0 <= b <= 0xffff */
{
  int w;

  if ( !a || !a->w || !b )
    c->w = 0;
  else {
    w = a->w;
    mulup2_nh(a->b,w,b,c->b);
    c->w = c->b[w] ? w+1 : w;
  }
}

void mulup2(a,b,c)
UP2 a,b;
UP2 *c;
{
  UP2 t;
  int wa,wb,w;

  if ( !a || !b )
    *c = 0;
  else if ( a->w==1 && a->b[0]==1 ) {
    NEWUP2(t,b->w); *c = t; _copyup2(b,t);
  } else if ( b->w==1 && b->b[0]==1 ) {
    NEWUP2(t,a->w); *c = t; _copyup2(a,t);
  } else {
    wa = a->w; wb = b->w;
    if ( wa != wb ) {
      if ( wb > wa ) {
        t = a; a = b; b = t;
        w = wa; wa = wb; wb = w;
      }
      W_NEWUP2(t,wa);
      _copyup2(b,t);
      bzero((char *)(t->b+wb),(wa-wb)*sizeof(unsigned int));
      t->w = wa;
      b = t;
    }
    NEWUP2(t,2*wa);
    _kmulup2_(a->b,b->b,wa,t->b);
    t->w = 2*wa; _adjup2(t);
    *c = t;
  }
}

void _kmulup2_(a,b,w,c)
unsigned int *a,*b,*c;
int w;
{
  switch ( w ) {
    case 1: GF2M_MUL_1(c[1],c[0],*a,*b); break;
    case 2: _mulup2_22(a,b,c); break;
    case 3: _mulup2_33(a,b,c); break;
    case 4: _mulup2_44(a,b,c); break;
    case 5: _mulup2_55(a,b,c); break;
    case 6: _mulup2_66(a,b,c); break;
    default: _mulup2_nn(a,b,w,c); break;
  }
}

void _mulup2_nn(a,b,w,c)
unsigned int *a,*b,*c;
int w;
{
  int wlow,whigh;
  struct _oUP2 ablow,abhigh,alow,ahigh,blow,bhigh,aa,bb,mid,cmid;

  /* wlow >= whigh */
  wlow = (w+1)/2; whigh = w-wlow; 

  alow.w = wlow; alow.b = a;
  ahigh.w = whigh; ahigh.b = a+wlow;

  blow.w = wlow; blow.b = b;
  bhigh.w = whigh; bhigh.b = b+wlow;

  ablow.b = c;
  abhigh.b = c+wlow*2;

  _kmulup2_(a,b,wlow,ablow.b); ablow.w = 2*wlow;
  _kmulup2_(a+wlow,b+wlow,whigh,abhigh.b); abhigh.w = 2*whigh;

  W_NEW_UP2(aa,wlow); 
  _addup2_(&alow,&ahigh,&aa); aa.w = wlow;

  W_NEW_UP2(bb,wlow);
  _addup2_(&blow,&bhigh,&bb); bb.w = wlow;

  W_NEW_UP2(mid,2*wlow);
  _kmulup2_(aa.b,bb.b,wlow,mid.b); mid.w = 2*wlow;

  _addtoup2_(&ablow,&mid); _addtoup2_(&abhigh,&mid);

  cmid.w = 2*wlow; cmid.b = c+wlow;
  _addtoup2_(&mid,&cmid);
}

void _mulup2(a,b,c)
UP2 a,b,c;
{
  int wa,wb,w;
  int i;
  unsigned int *ba,*bb;
  unsigned int mul;

  if ( !a || !b || !a->w || !b->w ) {
    c->w = 0; return;
  }
  wa = a->w; wb = b->w;
  w = wa + wb;
  bzero((char *)c->b,w*sizeof(unsigned int));
  for ( i = 0, ba = a->b, bb = b->b; i < b->w; i++, bb++ )
    if ( mul = *bb )
      mulup2_n1(ba,wa,mul,c->b+i);
  c->w = w;
  _adjup2(c);
}

void _mulup2_(a,b,c)
_UP2 a,b,c;
{
  int wa,wb,w;
  int i;
  unsigned int *ba,*bb;
  unsigned int mul;

  if ( !a || !b || !a->w || !b->w ) {
    c->w = 0; return;
  }
  wa = a->w; wb = b->w;
  w = wa + wb;
  bzero((char *)c->b,w*sizeof(unsigned int));
  for ( i = 0, ba = a->b, bb = b->b; i < b->w; i++, bb++ )
    if ( mul = *bb )
      mulup2_n1(ba,wa,mul,c->b+i);
  c->w = w;
  _adjup2_(c);
}

void squareup2(n,nr)
UP2 n;
UP2 *nr;
{
  int w,w2,i;
  unsigned int s;
  unsigned int *tb,*nb;
  UP2 t;

  if ( !n )
    *nr = 0;
  else {
    w = n->w; w2 = 2*w;
    NEWUP2(t,w2); *nr = t;
    tb = t->b;
    nb = n->b;
    for ( i = 0; i < w; i++ ) {
      s = nb[i];
      tb[2*i] = up2_sqtab[s&0xff]|(up2_sqtab[(s>>8)&0xff]<<16);
      tb[2*i+1] = up2_sqtab[(s>>16)&0xff]|(up2_sqtab[s>>24]<<16);
    }
    t->w = tb[w2-1]?w2:w2-1;
  }
}

void _squareup2(n,nr)
UP2 n;
UP2 nr;
{
  int w,w2,i;
  unsigned int s;
  unsigned int *tb,*nb;
  UP2 t;

  if ( !n )
    nr->w = 0;
  else {
    w = n->w; w2 = 2*w;
    t = nr;
    tb = t->b;
    nb = n->b;
    for ( i = 0; i < w; i++ ) {
      s = nb[i];
      tb[2*i] = up2_sqtab[s&0xff]|(up2_sqtab[(s>>8)&0xff]<<16);
      tb[2*i+1] = up2_sqtab[(s>>16)&0xff]|(up2_sqtab[s>>24]<<16);
    }
    t->w = tb[w2-1]?w2:w2-1;
  }
}

void _adjup2(n)
UP2 n;
{
  int i;
  unsigned int *nb;

  nb = n->b;
  for ( i = n->w - 1; i >= 0 && !nb[i]; i-- );
  i++;
  n->w = i;
}

void _adjup2_(n)
_UP2 n;
{
  int i;
  unsigned int *nb;

  nb = n->b;
  for ( i = n->w - 1; i >= 0 && !nb[i]; i-- );
  i++;
  n->w = i;
}

void _addup2(a,b,c)
UP2 a,b,c;
{
  int i,wa,wb,w;
  UP2 t;
  unsigned int *ab,*bb,*cb;

  if ( !a || !a->w ) {
    _copyup2(b,c); return;
  } else if ( !b || !b->w ) {
    _copyup2(a,c); return;
  }
  wa = a->w; wb = b->w;
  if ( wa < wb ) {
    t = a; a = b; b = t;
    w = wa; wa = wb; wb = w;
  }
  ab = a->b; bb = b->b; cb = c->b;
  for ( i = 0; i < wb; i++ )
    *cb++ = (*ab++)^(*bb++);
  for ( ; i < wa; i++ )
    *cb++ = *ab++;
  c->w = wa;
  _adjup2(c);
}

/* a += b */

void _addup2_destructive(a,b)
UP2 a,b;
{
  int i,wa,wb;
  unsigned int *ab,*bb;

  if ( !a->w ) {
    _copyup2(b,a); return;
  } else if ( !b->w )
    return;
  else {
    wa = a->w; wb = b->w;
    ab = a->b; bb = b->b;
    if ( wa >= wb ) {
      for ( i = 0; i < wb; i++ )
        *ab++ ^= *bb++;
    } else {
      for ( i = 0; i < wa; i++ )
        *ab++ ^= *bb++;
      for ( ; i < wb; i++ )
        *ab++ = *bb++;
      a->w = wb;
    }
    _adjup2(a);
  }
}

void _addup2_(a,b,c)
_UP2 a,b,c;
{
  int i,wa,wb,w;
  _UP2 t;
  unsigned int *ab,*bb,*cb;

  wa = a->w; wb = b->w;
  if ( wa < wb ) {
    t = a; a = b; b = t;
    w = wa; wa = wb; wb = w;
  }
  ab = a->b; bb = b->b; cb = c->b;
  for ( i = 0; i < wb; i++ )
    *cb++ = (*ab++)^(*bb++);
  for ( ; i < wa; i++ )
    *cb++ = *ab++;
  c->w = wa;
}

void _addtoup2_(a,b)
_UP2 a,b;
{
  int i,wa;
  unsigned int *ab,*bb;

  wa = a->w;
  ab = a->b; bb = b->b;
  for ( i = 0; i < wa; i++ )
    *bb++ ^= *ab++;
}

/* 8bit x 8bit; also works if deg(a*b) < 32 */

unsigned int mulup2_bb(a,b)
unsigned int a,b;
{
  unsigned int t;

  if ( !a || !b )
    return 0;
  else {
    t = 0;
    while ( b ) {
      if ( b & 1 )
        t ^= a;
      a <<= 1;
      b >>= 1;
    }
    return t;
  }
}

void init_up2_tab()
{
  unsigned int i,j;

  for ( i = 0; i < 256; i++ )
    for ( j = 0; j <= i; j++ ) {
      up2_bbtab[i<<8|j] = mulup2_bb(i,j);
      up2_bbtab[j<<8|i] = up2_bbtab[i<<8|j];
    }
  for ( i = 0; i < 256; i++ )
    up2_sqtab[i] = mulup2_bb(i,i);
}

/*
  compute q s.t. a*x^(BSH-1) = b*q+r 
  deg(b)=BSH-1, deg(a)<=BSH-1 => deg(q)<=BSH-1, deg(r)<=BSH-2
*/

INLINE unsigned int quoup2_11(a,b)
unsigned int a,b;
{
  unsigned int q,i;

  q = 0;
  for ( i = ((unsigned int)1)<<(BSH-1); i; i >>=1, b >>= 1 )
    if ( i & a ) {
      a ^= b;
      q |= i;
    }
  return q;
}

void divup2_1(a1,a2,e1,e2,qp,rp)
unsigned int a1,a2;
int e1,e2;
unsigned int *qp,*rp;
{
  int i;
  unsigned t,q;

  for ( i=e1-e2, t=1<<e1, a2<<=i, q = 0; i>=0; i--, t>>=1, a2>>=1 ) {
    q<<=1;
    if ( a1 & t ) {
      q |= 1;
      a1 ^= a2;
    }
  }
  *qp = q; *rp = a1;
}

void qrup2(a,b,q,r)
UP2 a,b;
UP2 *q,*r;
{
  unsigned int msa,msb,t,q0;
  int s,i,wq,wb;
  UP2 as,bs,_q;

  if ( !b )
    error("qrup2 : division by 0");
  else if ( !a ) {
    *q = 0; *r = 0; return;
  } else if ( degup2(a) < degup2(b) ) {
    *q = 0; *r = a; return;
  }
  msb = b->b[b->w-1];
  for ( t = msb, s = 0; t; t>>=1, s++ );
  s = BSH-s;

  W_NEWUP2(as,a->w+2); W_NEWUP2(bs,b->w+1);
  _bshiftup2(a,-s,as); _bshiftup2(b,-s,bs);
  as->b[as->w] = 0;

  wb = bs->w;
  wq = as->w-wb;
  NEWUP2(_q,wq+1); *q = _q;

  msb = bs->b[bs->w-1];
  for ( i = wq; i >= 0; i-- ) {
    msa = (as->b[wb+i]<<1) | (as->b[wb+i-1]>>(BSH-1));
    if ( msa ) {
      q0 = quoup2_11(msa,msb);
      mulup2_n1(bs->b,wb,q0,as->b+i);
    } else
      q0 = 0;
    _q->b[i] = q0;
  }
  for ( i = wq; i >= 0 && !_q->b[i]; i-- );
  _q->w = i+1;

  for ( i = wb-1; i >= 0 && !as->b[i]; i-- );
  if ( i < 0 )
    *r = 0;
  else {
    as->w = i+1;
    NEWUP2(*r,as->w);
    _bshiftup2(as,s,*r);
  }
}

/* q->w >= a->w-b->w+2, r->w >= b->w */

void _qrup2(a,b,q,r)
UP2 a,b;
UP2 q,r;
{
  unsigned int msa,msb,t,q0;
  int s,i,wq,wb;

  if ( !b || !b->w )
    error("_qrup2 : division by 0");
  else if ( !a || !a->w ) {
    q->w = 0; r->w = 0; return;
  } else if ( degup2(a) < degup2(b) ) {
    q->w = 0; _copyup2(a,r); return;
  }
  msb = b->b[b->w-1];
  for ( t = msb, s = BSH; t; t>>=1, s-- );

  _copyup2(a,r);
  wb = b->w;
  wq = r->w-wb;
  r->b[r->w] = 0;

  msb = (b->b[b->w-1]<<s)|(b->w==1||!s?0:b->b[b->w-2]>>(BSH-s));
  for ( i = wq; i >= 0; i-- ) {
    msa = (s==BSH-1?0:r->b[wb+i]<<(s+1))|(wb+i-1<0?0:r->b[wb+i-1]>>(BSH-1-s));
    if ( msa ) {
      q0 = quoup2_11(msa,msb);
      mulup2_n1(b->b,wb,q0,r->b+i);
    } else
      q0 = 0;
    q->b[i] = q0;
  }
  for ( i = wq; i >= 0 && !q->b[i]; i-- );
  q->w = i+1;

  for ( i = wb-1; i >= 0 && !r->b[i]; i-- );
  if ( i < 0 )
    r->w = 0;
  else
    r->w = i+1;
}

void remup2(a,b,c)
UP2 a,b;
UP2 *c;
{
  unsigned int msa,msb,t,q;
  int s,i,wq,wb;
  UP2 as,bs,r;

  if ( !b || !b->w )
    error("remup2 : division by 0");
  else if ( !a || !a->w ) {
    *c = 0; return;
  } else if ( degup2(a) < degup2(b) ) {
    *c = a; return;
  }
  msb = b->b[b->w-1];
  for ( t = msb, s = 0; t; t>>=1, s++ );
  s = BSH-s;

  W_NEWUP2(as,a->w+2); W_NEWUP2(bs,b->w+1);
  _bshiftup2(a,-s,as); _bshiftup2(b,-s,bs);
  as->b[as->w] = 0;

  wb = bs->w;
  wq = as->w-wb;
  msb = bs->b[bs->w-1];
  for ( i = wq; i >= 0; i-- ) {
    msa = (as->b[wb+i]<<1) | (as->b[wb+i-1]>>(BSH-1));
    if ( msa ) {
      q = quoup2_11(msa,msb);
      mulup2_n1(bs->b,wb,q,as->b+i);
    }
  }
  for ( i = wb-1; i >= 0 && !as->b[i]; i-- );
  if ( i < 0 )
    *c = 0;
  else {
    as->w = i+1;
    NEWUP2(r,as->w); *c = r;
    _bshiftup2(as,s,r);
  }
}

void _remup2(a,b,c)
UP2 a,b,c;
{
  unsigned int msa,msb,t,q;
  int s,i,wq,wb;
  UP2 as,bs;

  if ( !b || !b->w )
    error("_remup2 : division by 0");
  else if ( !a || !a->w ) {
    c->w = 0; return;
  } else if ( degup2(a) < degup2(b) ) {
    _copyup2(a,c); return;
  }
  msb = b->b[b->w-1];
  for ( t = msb, s = 0; t; t>>=1, s++ );
  s = BSH-s;

  W_NEWUP2(as,a->w+2); W_NEWUP2(bs,b->w+1);
  _bshiftup2(a,-s,as); _bshiftup2(b,-s,bs);
  as->b[as->w] = 0;

  wb = bs->w;
  wq = as->w-wb;
  msb = bs->b[bs->w-1];
  for ( i = wq; i >= 0; i-- ) {
    msa = (as->b[wb+i]<<1) | (as->b[wb+i-1]>>(BSH-1));
    if ( msa ) {
      q = quoup2_11(msa,msb);
      mulup2_n1(bs->b,wb,q,as->b+i);
    }
  }
  for ( i = wb-1; i >= 0 && !as->b[i]; i-- );
  if ( i < 0 )
    c->w = 0;
  else {
    as->w = i+1;
    _bshiftup2(as,s,c);
  }
}

/* b = b->w|b->b[0]|b->b[1]|...  -> b = x^b->[0]+x^b->[1]+... (b->w terms) */

void remup2_sparse(a,b,c)
UP2 a,b;
UP2 *c;
{
  int i,j,k,wa,wb,d,ds,db,dr,r;
  unsigned int ha,hb;
  unsigned int *tb;

  UP2 t,s;

  if ( !b )
    error("remup2_sparse : division by 0");
  else if ( !a ) {
    *c = 0; return;
  } else if ( degup2(a) < (db = degup2_sparse(b)) ) {
    *c = a; return;
  } else if ( b->w == (int)(b->b[0])+1 ) {
    NEWUP2(*c,a->w);
    _copyup2(a,*c);
    remup2_type1_destructive(*c,b->b[0]);
    if ( (*c)->w <= 0 )
      *c = 0;
  } else {
    W_NEWUP2(t,a->w); _copyup2(a,t);
    wa = a->w; wb = b->w; tb = t->b;
    for ( i = wa-1; (ds = BSH*i-db) >= 0;  ) {
      if ( !(ha = tb[i]) )
        i--;
      else {
        tb[i] = 0;
        for ( j = 1; j < wb; j++ ) {
          d = ds+b->b[j];
          k = d/BSH; r = d%BSH;
          if ( !r )
            tb[k] ^= ha;
          else {
            tb[k] ^= (ha<<r);
            if ( k+1 <= i )
              tb[k+1] ^= (ha>>(BSH-r));
          }
        }
        if ( !tb[i] )
          i--;
      }
    }
    dr = db%BSH;
    hb = 1<<dr;
    i = db/BSH;
    while ( (ha=tb[i]) >= hb ) {
      ha >>= dr;
      t->b[i] &= (hb-1);
      for ( j = 1; j < wb; j++ ) {
        d = b->b[j];
        k = d/BSH; r = d%BSH;
        if ( !r )
          tb[k] ^= ha;
        else {
          tb[k] ^= (ha<<r);
          if ( k+1 <= i )
            tb[k+1] ^= (ha>>(BSH-r));
        }
      }
    }
    t->w = i+1;
    _adjup2(t);
    if ( t->w == 0 )
      *c = 0;      
    else {
      NEWUP2(s,t->w); *c = s;
      _copyup2(t,s);
    }
  }
}

void remup2_sparse_destructive(a,b)
UP2 a,b;
{
  int i,j,k,wb,d,ds,db,dr,r;
  unsigned int ha,hb;
  unsigned int *ab,*bb;

  if ( !b )
    error("remup2_sparse_destructive : division by 0");
  else if ( !a || !a->w || ( degup2(a) < (db = degup2_sparse(b)) ) )
    return;
  else if ( b->w == 3 )
    remup2_3_destructive(a,b);
  else if ( b->w == 5 )
    remup2_5_destructive(a,b);
  else if ( b->w == (int)(b->b[0])+1 )
    remup2_type1_destructive(a,b->b[0]);
  else {
    wb = b->w; ab = a->b; bb = b->b;
    for ( i = a->w-1; (ds = (i<<5)-db) >= 0;  ) {
      if ( !(ha = ab[i]) )
        i--;
      else {
        ab[i] = 0;
        for ( j = 1; j < wb; j++ ) {
          d = ds+bb[j]; k = d>>5; r = d&31;
          REMUP2_ONESTEP(ab,k,r,i,ha)
        }
        if ( !ab[i] )
          i--;
      }
    }
    i = db>>5; dr = db&31;
    for ( hb = (1<<dr)-1; (ha=ab[i]) > hb; ) {
      ha >>= dr;
      ab[i] &= hb;
      for ( j = 1; j < wb; j++ ) {
        d = bb[j]; k = d>>5; r = d&31;
        REMUP2_ONESTEP(ab,k,r,i,ha)
      }
    }
    a->w = i+1;
    _adjup2(a);
  }
}

/* b = x^d+x^(d-1)+...+1 */

void remup2_type1_destructive(a,d)
UP2 a;
int d;
{
  int i,k,ds,db,dr;
  unsigned int ha,hb,r;
  unsigned int *ab;

  ab = a->b; db = d+1;
  for ( i = a->w-1; (ds = (i<<5)-db) >= 0;  ) {
    if ( !(ha = ab[i]) ) i--;
    else {
      ab[i] = 0;
      k = ds>>5; r = ds&31;
      REMUP2_ONESTEP(ab,k,r,i,ha)
      if ( !ab[i] ) i--;
    }
  }
  i = db>>5; dr = db&31;
  for ( hb = (1<<dr)-1; (ha=ab[i]) > hb; ) {
    ha >>= dr; ab[i] &= hb; ab[0] ^= ha;
  }
  i = d>>5; hb = 1<<(d&31);
  if ( ab[i]&hb ) {
    ab[i] = (ab[i]^hb)^(hb-1);
    for ( k = i-1; k >= 0; k-- ) ab[k] ^= 0xffffffff;
  }
  a->w = i+1;
  _adjup2(a);
}

/* b = x^b->b[0]+x^b->b[1]+1 */

void remup2_3_destructive(a,b)
UP2 a,b;
{
  int i,k,d,ds,db,db1,dr;
  unsigned int ha,hb,r;
  unsigned int *ab,*bb;

  ab = a->b; bb = b->b;
  db = bb[0]; db1 = bb[1];
  for ( i = a->w-1; (ds = (i<<5)-db) >= 0;  ) {
    if ( !(ha = ab[i]) )
      i--;
    else {
      ab[i] = 0;
      d = ds+db1; k = d>>5; r = d&31;
      REMUP2_ONESTEP(ab,k,r,i,ha)
      d = ds; k = d>>5; r = d&31;
      REMUP2_ONESTEP(ab,k,r,i,ha)
      if ( !ab[i] )
        i--;
    }
  }
  i = db>>5; dr = db&31;
  k = db1>>5; r = db1&31;
  for ( hb = (1<<dr)-1; (ha=ab[i]) > hb; ) {
    ha >>= dr;
    ab[i] &= hb;
    REMUP2_ONESTEP(ab,k,r,i,ha)
    ab[0] ^= ha;
  }
  a->w = i+1;
  _adjup2(a);
}

/* b = x^b->b[0]+x^b->b[1]+x^b->b[2]+x^b->b[3]+1 */

void remup2_5_destructive(a,b)
UP2 a,b;
{
  int i,d,ds,db,db1,db2,db3,dr;
  int k,k1,k2,k3;
  unsigned int ha,hb,r,r1,r2,r3;
  unsigned int *ab,*bb;

  ab = a->b; bb = b->b;
  db = bb[0]; db1 = bb[1]; db2 = bb[2]; db3 = bb[3];
  for ( i = a->w-1; (ds = (i<<5)-db) >= 0;  ) {
    if ( !(ha = ab[i]) )
      i--;
    else {
      ab[i] = 0;
      d = ds+db1; k = d>>5; r = d&31;
      REMUP2_ONESTEP(ab,k,r,i,ha)
      d = ds+db2; k = d>>5; r = d&31;
      REMUP2_ONESTEP(ab,k,r,i,ha)
      d = ds+db3; k = d>>5; r = d&31;
      REMUP2_ONESTEP(ab,k,r,i,ha)
      d = ds; k = d>>5; r = d&31;
      REMUP2_ONESTEP(ab,k,r,i,ha)
      if ( !ab[i] )
        i--;
    }
  }
  i = db>>5; dr = db&31;
  k1 = db1>>5; r1 = db1&31;
  k2 = db2>>5; r2 = db2&31;
  k3 = db3>>5; r3 = db3&31;
  for ( hb = (1<<dr)-1; (ha=ab[i]) > hb; ) {
    ha >>= dr;
    ab[i] &= hb;
    REMUP2_ONESTEP(ab,k1,r1,i,ha)
    REMUP2_ONESTEP(ab,k2,r2,i,ha)
    REMUP2_ONESTEP(ab,k3,r3,i,ha)
    ab[0] ^= ha;
  }
  a->w = i+1;
  _adjup2(a);
}

void _invup2_1(f1,f2,a1,b1)
unsigned int f1,f2,*a1,*b1;
{
  unsigned int p1,p2,p3,q1,q2,q3,g1,g2,q,r;
  int d1,d2;

  p1 = 1; p2 = 0;
  q1 = 0; q2 = 1;
  g1 = f1; g2 = f2;
  d1 = degup2_1(g1); d2 = degup2_1(g2);
  while ( g2 ) {
    divup2_1(g1,g2,d1,d2,&q,&r);
    g1 = g2; g2 = r;
    GCD_COEF(q,p1,p2,p3,q1,q2,q3)
    p1 = p2; p2 = p3;
    q1 = q2; q2 = q3;
    d1 = d2; d2 = degup2_1(g2);
  }
  *a1 = p1; *b1 = q1;
}

void _gcdup2_1(f1,f2,gcd)
unsigned int f1,f2,*gcd;
{
  unsigned int g1,g2,q,r;
  int d1,d2;

  if ( !f1 )
    *gcd = f2;
  else if ( !f2 )
    *gcd = f1;
  else {
    g1 = f1; g2 = f2;
    d1 = degup2_1(g1); d2 = degup2_1(g2);
    while ( 1 ) {
      divup2_1(g1,g2,d1,d2,&q,&r);
      if ( !r ) {
        *gcd = g2;
        return;
      }
      g1 = g2; g2 = r;
      d1 = d2; d2 = degup2_1(g2);
    }
  }
}

static struct oEGT eg_a,eg_b;

void up2_init_eg() {
  init_eg(&eg_a); init_eg(&eg_b);
}

void up2_show_eg() {
  print_eg("A",&eg_a);
  print_eg("B",&eg_b);
  printf("\n");
}

void invup2(a,m,inv)
UP2 a,m;
UP2 *inv;
{
  int w,e1,e2,d1,d2;
  UP2 g1,g2,g3,a1,a2,a3,q,r,w1,w2,t;
  unsigned int p1,p2,p3,q0,q1,q2,q3,h1,h2,h3;

  if ( degup2(a) >= degup2(m) ) {
    remup2(a,m,&t); a = t;
  }
  w = m->w+2;
  W_NEWUP2(g1,w); W_NEWUP2(g2,w); W_NEWUP2(g3,w);
  W_NEWUP2(q,w); W_NEWUP2(r,w);
  W_NEWUP2(a1,w); W_NEWUP2(a2,w); W_NEWUP2(a3,w);
  W_NEWUP2(w1,w); W_NEWUP2(w2,w);

  a1->w = 1; a1->b[0] = 1;
  a2->w = 0;
  _copyup2(a,g1); _copyup2(m,g2);

  while ( 1 ) {
    d1 = degup2(g1); d2 = degup2(g2);
    if ( d1 < d2 ) {
      t = g1; g1 = g2; g2 = t;
      t = a1; a1 = a2; a2 = t;
    } else if ( d1 < 32 ) {
      _invup2_1(g1->b[0],g2->b[0],&p1,&p2);
      _mulup2_1(a1,p1,w1);
      _mulup2_1(a2,p2,w2);
      addup2(w1,w2,inv);
      return;
    } else if ( d1-d2 >= 16 ) {
      _qrup2(g1,g2,q,g3);
      t = g1; g1 = g2;
      if ( !g3->w ) {
        NEWUP2(t,a2->w); *inv = t;
        _copyup2(a2,t);
        return;
      } else {
        g2 = g3; g3 = t;
      }
      _mulup2(a2,q,w1); _addup2(a1,w1,a3);
      t = a1; a1 = a2; a2 = a3; a3 = t;
    } else {
      _bshiftup2(g1,d1-31,w1); h1 = w1->b[0];
      _bshiftup2(g2,d1-31,w2); h2 = w2->b[0];
      p1 = 1; p2 = 0;
      q1 = 0; q2 = 1;
/*      get_eg(&eg0); */
      e1 = degup2_1(h1); /* e1 must be 31 */
      while ( 1 ) {
        e2 = degup2_1(h2);
        if ( e2 <= 15 )
          break;
        divup2_1(h1,h2,e1,e2,&q0,&h3);
        GCD_COEF(q0,p1,p2,p3,q1,q2,q3)
        p1 = p2; p2 = p3;
        q1 = q2; q2 = q3;
        h1 = h2; h2 = h3;
        e1 = e2;
      }
/*      get_eg(&eg1); */
      _mulup2_1(g1,p1,w1); _mulup2_1(g2,q1,w2); _addup2(w1,w2,g3);
      _mulup2_1(g1,p2,w1); _mulup2_1(g2,q2,w2); _addup2(w1,w2,g2);
      t = g1; g1 = g3; g3 = t;
      _mulup2_1(a1,p1,w1); _mulup2_1(a2,q1,w2); _addup2(w1,w2,a3);
      _mulup2_1(a1,p2,w1); _mulup2_1(a2,q2,w2); _addup2(w1,w2,a2);
      t = a1; a1 = a3; a3 = t;
/*      get_eg(&eg2); */
/*      add_eg(&eg_a,&eg0,&eg1); */
/*      add_eg(&eg_b,&eg1,&eg2); */
    }
  }
}

void gcdup2(a,m,gcd)
UP2 a,m;
UP2 *gcd;
{
  int w,e1,e2,d1,d2;
  UP2 g1,g2,g3,q,r,w1,w2,t;
  unsigned int p1,p2,p3,q0,q1,q2,q3,h1,h2,h3;

  if ( !a ) {
    *gcd = m; return;
  } else if ( !m ) {
    *gcd = a; return;
  }
  if ( degup2(a) >= degup2(m) ) {
    remup2(a,m,&t); a = t;
    if ( !a ) {
      *gcd = m; return;
    }
  }
  w = m->w+2;
  W_NEWUP2(g1,w); W_NEWUP2(g2,w); W_NEWUP2(g3,w);
  W_NEWUP2(q,w); W_NEWUP2(r,w);
  W_NEWUP2(w1,w); W_NEWUP2(w2,w);

  _copyup2(a,g1); _copyup2(m,g2);

  while ( 1 ) {
    d1 = degup2(g1); d2 = degup2(g2);
    if ( d1 < d2 ) {
      t = g1; g1 = g2; g2 = t;
    } else if ( d2 < 0 ) {
      NEWUP2(t,g1->w); *gcd = t;
      _copyup2(g1,t);
      return;
    } else if ( d1 < 32 ) {
      NEWUP2(t,1); t->w = 1; *gcd = t;
      _gcdup2_1(g1->b[0],g2->b[0],&t->b[0]);
      return;
    } else if ( d1-d2 >= 16 ) {
      _qrup2(g1,g2,q,g3);
      t = g1; g1 = g2;
      if ( !g3->w ) {
        NEWUP2(t,g2->w); *gcd = t;
        _copyup2(g2,t);
        return;
      } else {
        g2 = g3; g3 = t;
      }
    } else {
      _bshiftup2(g1,d1-31,w1); h1 = w1->b[0];
      _bshiftup2(g2,d1-31,w2); h2 = w2->b[0];
      p1 = 1; p2 = 0;
      q1 = 0; q2 = 1;
      e1 = degup2_1(h1); /* e1 must be 31 */
      while ( 1 ) {
        e2 = degup2_1(h2);
        if ( e2 <= 15 )
          break;
        divup2_1(h1,h2,e1,e2,&q0,&h3);
        GCD_COEF(q0,p1,p2,p3,q1,q2,q3)
        p1 = p2; p2 = p3;
        q1 = q2; q2 = q3;
        h1 = h2; h2 = h3;
        e1 = e2;
      }
      _mulup2_h(g1,p1,w1); _mulup2_h(g2,q1,w2); _addup2(w1,w2,g3);
      _mulup2_h(g1,p2,w1); _mulup2_h(g2,q2,w2); _addup2(w1,w2,g2);
      t = g1; g1 = g3; g3 = t;
    }
  }
}

void chsgnup2(a,c)
UP2 a,*c;
{
  *c = a;
}

void pwrmodup2(a,b,m,c)
UP2 a;
Q b;
UP2 m;
UP2 *c;
{
  N n;
  UP2 y,t,t1;
  int k;

  if ( !b )
    *c = (UP2)ONEUP2;
  else if ( !a )
    *c = 0;
  else {
    y = (UP2)ONEUP2;
    n = NM(b);
    if ( !m )
      for ( k = n_bits(n)-1; k >= 0; k-- ) {
        squareup2(y,&t);
        if ( n->b[k/BSH] & (1<<(k%BSH)) )
          mulup2(t,a,&y);
        else
          y = t;
      }
    else
      for ( k = n_bits(n)-1; k >= 0; k-- ) {
        squareup2(y,&t);
        remup2(t,m,&t1); t = t1;
        if ( n->b[k/BSH] & (1<<(k%BSH)) ) {
          mulup2(t,a,&y);
          remup2(y,m,&t1); y = t1;
        }
        else
          y = t;
      }
    *c = y;
  }
}

void pwrmodup2_sparse(a,b,m,c)
UP2 a;
Q b;
UP2 m;
UP2 *c;
{
  N n;
  UP2 y,t,t1;
  int k;

  if ( !b )
    *c = (UP2)ONEUP2;
  else if ( !a )
    *c = 0;
  else {
    y = (UP2)ONEUP2;
    n = NM(b);
    if ( !m )
      for ( k = n_bits(n)-1; k >= 0; k-- ) {
        squareup2(y,&t);
        if ( n->b[k/BSH] & (1<<(k%BSH)) )
          mulup2(t,a,&y);
        else
          y = t;
      }
    else
      for ( k = n_bits(n)-1; k >= 0; k-- ) {
        squareup2(y,&t);
        remup2_sparse(t,m,&t1); t = t1;
        if ( n->b[k/BSH] & (1<<(k%BSH)) ) {
          mulup2(t,a,&y);
          remup2_sparse(y,m,&t1); y = t1;
        }
        else
          y = t;
      }
    *c = y;
  }
}

int compup2(n1,n2)
UP2 n1,n2;
{
  int i;
  unsigned int *m1,*m2;

  if ( !n1 ) 
    if ( !n2 ) 
      return 0;
    else 
      return -1;
  else if ( !n2 ) 
    return 1;
  else if ( n1->w > n2->w )
    return 1;
  else if ( n1->w < n2->w ) 
    return -1;
  else {  
    for ( i = n1->w-1, m1 = n1->b+i, m2 = n2->b+i;
      i >= 0; i--, m1--, m2-- ) 
      if ( *m1 > *m2 ) 
        return 1;
      else if ( *m1 < *m2 )
        return -1;
    return 0;
  }
}

void _copyup2(n,r)
UP2 n,r;
{
  r->w = n->w;
  bcopy(n->b,r->b,n->w*sizeof(unsigned int));
}

void _bshiftup2(n,b,r)
UP2 n;
int b;
UP2 r;
{
  int w,l,nl,i,j;
  unsigned int msw;
  unsigned int *p,*pr;

  if ( b == 0 ) {
    _copyup2(n,r); return;
  }
  if ( b > 0 ) { /* >> */
    w = b / BSH; l = n->w-w;
    if ( l <= 0 ) {
      r->w = 0; return;
    }
    b %= BSH; p = n->b+w;
    if ( !b ) {
      r->w = l;
      bcopy(p,r->b,l*sizeof(unsigned int));
      return;
    }
    msw = p[l-1];
    for ( i = BSH-1; !(msw&(((unsigned int)1)<<i)); i-- ); i++;
    if ( b >= i ) {
      l--;
      if ( !l ) {
        r->w = 0; return;
      }
      r->w = l; pr = r->b;
      for ( j = 0; j < l; j++, p++ )
        *pr++ = (*(p+1)<<(BSH-b))|(*p>>b);
    } else {
      r->w = l; pr = r->b;
      for ( j = 1; j < l; j++, p++ )
        *pr++ = (*(p+1)<<(BSH-b))|(*p>>b);
      *pr = *p>>b;
    }
  } else { /* << */
    b = -b;
    w = b / BSH; b %= BSH; l = n->w; p = n->b;
    if ( !b ) {
      nl = l+w; r->w = nl;
      bzero((char *)r->b,w*sizeof(unsigned int));
      bcopy(p,r->b+w,l*sizeof(unsigned int));
      return;
    }
    msw = p[l-1];
    for ( i = BSH-1; !(msw&(((unsigned int)1)<<i)); i-- ); i++;
    if ( b + i > BSH ) {
      nl = l+w+1;
      r->w = nl; pr = r->b+w;
      bzero((char *)r->b,w*sizeof(unsigned int));
      *pr++ = *p++<<b;
      for ( j = 1; j < l; j++, p++ )
        *pr++ = (*p<<b)|(*(p-1)>>(BSH-b));
      *pr = *(p-1)>>(BSH-b);
    } else {
      nl = l+w;
      r->w = nl; pr = r->b+w;
      bzero((char *)r->b,w*sizeof(unsigned int));
      *pr++ = *p++<<b;
      for ( j = 1; j < l; j++, p++ )
        *pr++ = (*p<<b)|(*(p-1)>>(BSH-b));
    }
  }
}

void _bshiftup2_destructive(n,b)
UP2 n;
int b;
{
  int w,l,nl,i,j;
  unsigned int msw;
  unsigned int *p,*pr;

  if ( b == 0 || !n->w )
    return;
  if ( b > 0 ) { /* >> */
    w = b / BSH; l = n->w-w;
    if ( l <= 0 ) {
      n->w = 0; return;
    }
    b %= BSH; p = n->b+w;
    if ( !b ) {
      n->w = l;
      bcopy(p,n->b,l*sizeof(unsigned int));
      return;
    }
    msw = p[l-1];
    for ( i = BSH-1; !(msw&(((unsigned int)1)<<i)); i-- ); i++;
    if ( b >= i ) {
      l--;
      if ( !l ) {
        n->w = 0; return;
      }
      n->w = l; pr = n->b;
      for ( j = 0; j < l; j++, p++ )
        *pr++ = (*(p+1)<<(BSH-b))|(*p>>b);
    } else {
      n->w = l; pr = n->b;
      for ( j = 1; j < l; j++, p++ )
        *pr++ = (*(p+1)<<(BSH-b))|(*p>>b);
      *pr = *p>>b;
    }
  } else { /* << */
    b = -b;
    w = b / BSH; b %= BSH; l = n->w; p = n->b;
    if ( !b ) {
      nl = l+w; n->w = nl;
      bcopy(p,n->b+w,l*sizeof(unsigned int));
      bzero((char *)n->b,w*sizeof(unsigned int));
      return;
    }
    msw = p[l-1];
    for ( i = BSH-1; !(msw&(((unsigned int)1)<<i)); i-- ); i++;
    if ( b + i > BSH ) {
      nl = l+w+1;
      n->w = nl; p = n->b+l-1; pr = n->b+w+l;
      *pr-- = *p>>(BSH-b);
      for ( j = l-1; j >= 1; j--, p-- )
        *pr-- = (*p<<b)|(*(p-1)>>(BSH-b));
      *pr = *p<<b;
      bzero((char *)n->b,w*sizeof(unsigned int));
    } else {
      nl = l+w;
      n->w = nl; p = n->b+l-1; pr = n->b+w+l-1;
      for ( j = l-1; j >= 1; j--, p-- )
        *pr-- = (*p<<b)|(*(p-1)>>(BSH-b));
      *pr = *p<<b;
      bzero((char *)n->b,w*sizeof(unsigned int));
    }
  }
}

void diffup2(f,r)
UP2 f;
UP2 *r;
{
  int d,i,w;
  UP2 t;

  d = degup2(f);
  w = f->w;
  NEWUP2(t,w);
  _bshiftup2(f,1,t);
  for ( i = 0; i < d; i++ )
    if ( i%2 )
      t->b[i/BSH] &= ~(1<<(i%BSH));
  for ( i = w-1; i >= 0 && !t->b[i]; i-- );
  if ( i < 0 )
    *r = 0;
  else {
    *r = t;
    t->w = i+1;
  }
}

int sqfrcheckup2(f)
UP2 f;
{
  UP2 df,g;

  diffup2(f,&df);
  gcdup2(f,df,&g);
  if ( degup2(g) >= 1 )
    return 0;
  else
    return 1;
}

int irredcheckup2(f)
UP2 f;
{
  int n,w,i,j,k,hcol;
  unsigned int hbit;
  unsigned int *u;
  unsigned int **mat;
  UP2 p,t;

  if ( !sqfrcheckup2(f) )
    return 0;
  n = degup2(f);
  w = n/BSH + (n%BSH?1:0);
  mat = (unsigned int **)almat(n,w);
  W_NEWUP2(p,w+1);
  W_NEWUP2(t,w+1);
  p->w = 1; p->b[0] = 1; /*  p = 1 mod f */
  for ( i = 1; i < n; i++ ) {
    _bshiftup2(p,-2,t); _remup2(t,f,p);
    _copy_up2bits(p,mat,i);
    
  }
/*  _print_frobmat(mat,n,w); */
  for ( j = 1; j < n; j++ ) {
    hcol = j/BSH; hbit = 1<<(j%BSH);
    for ( i = j-1; i < n; i++ )
      if ( mat[i][hcol] & hbit )
        break;
    if ( i == n )
      return 0;
    if ( i != j-1 ) {
      u = mat[i]; mat[i] = mat[j-1]; mat[j-1] = u;
    }
    for ( i = j; i < n; i++ )
      if ( mat[i][hcol] & hbit )
        for ( k = hcol; k < w; k++ )
          mat[i][k] ^= mat[j-1][k];
  }
  return 1;
}

int irredcheck_dddup2(f)
UP2 f;
{
  UP2 x,u,t,s,gcd;
  int n,i;

  if ( !sqfrcheckup2(f) )
    return -1;
  n = degup2(f);

  W_NEWUP2(u,1); u->w = 1; u->b[0] = 2; /* u = x mod f */
  x = u;
  for ( i = 1; 2*i <= n; i++ ) {
    squareup2(u,&t); remup2(t,f,&u); addup2(u,x,&s);
    gcdup2(f,s,&gcd);
    if ( degup2(gcd) > 0 )
      return 0;
  }
  return 1;
}

void _copy_up2bits(p,mat,pos)
UP2 p;
unsigned int **mat;
int pos;
{
  int d,col,j,jcol,jsh;
  unsigned int bit;

  d = degup2(p);
  col = pos/BSH; bit = 1<<(pos%BSH);

  for ( j = 0; j <= d; j++ ) {
    jcol = j/BSH; jsh = j%BSH;
    if ( p->b[jcol]&(1<<jsh) )
      mat[j][col] |= bit;
  }
  mat[pos][col] ^= bit;
}

int compute_multiplication_matrix(p0,mp)
P p0;
GF2MAT *mp;
{
  UP2 p;
  int n,w,i,j,k,l;
  unsigned int **a,**b,**c,**d,**t,**m;
  GF2MAT am,bm;

  ptoup2(p0,&p);
  n = degup2(p);
  w = p->w;
  if ( !compute_representation_conversion_matrix(p0,&am,&bm) )
    return 0;
  a = am->body; b = bm->body;
/*  _print_frobmat(a,n,w); printf("\n"); */
/*  _print_frobmat(b,n,w); printf("\n"); */
  c = (unsigned int **)almat(n,w);
  for ( i = 0; i < n-1; i++ ) {
    j = i+1;
    c[i][j/BSH] |= 1<<(j%BSH);
  }
  bcopy(p->b,c[n-1],p->w*sizeof(unsigned int));

/*  _print_frobmat(b,n,w); printf("\n"); */
  t = (unsigned int **)almat(n,w);
  mulgf2mat(n,a,c,t);
  d = (unsigned int **)almat(n,w);
  mulgf2mat(n,t,b,d);
/*  _print_frobmat(d,n,w); printf("\n"); */

  m = (unsigned int **)almat(n,w);
  for ( i = 0; i < n; i++ )
    for ( j = 0; j < n; j++ ) {
      k = (n-1-(j-i))%n; l = (n-1+i)%n;
      if ( d[k][l/BSH] & 1<<(l%BSH) )
        m[n-1-i][(n-1-j)/BSH] |= 1<<((n-1-j)%BSH);
    }
/*  _print_frobmat(m,n,w); printf("\n"); */
  TOGF2MAT(n,n,m,*mp);
  return 1;
}

#define GF2N_PBTOPB 0
#define GF2N_NBTOPB 1

void compute_change_of_basis_matrix_with_root(P,P,int,GF2N,GF2MAT *,GF2MAT *);

/*
 * if 'to' = GF2N_NBTOPB then p0 must be a normal poly.
 * rep0 x m01 -> rep1, rep1 x m10 -> rep0
 */

void compute_change_of_basis_matrix(p0,p1,to,m01,m10)
P p0,p1;
int to;
GF2MAT *m01,*m10;
{
  UP2 up0;
  int n,w;
  unsigned int **p01,**p10;
  GF2N root;

  setmod_gf2n(p1);

  ptoup2(p0,&up0);
  n = degup2(up0); w = up0->w;
  find_root_up2(up0,&root);
  compute_change_of_basis_matrix_with_root(p0,p1,to,root,m01,m10);
}

void compute_change_of_basis_matrix_with_root(p0,p1,to,root,m01,m10)
P p0,p1;
int to;
GF2N root;
GF2MAT *m01,*m10;
{
  UP2 up0,t,u,s;
  int n,w,i;
  unsigned int **a,**b,**g,**h;
  unsigned int **p01,**p10;
  P tmp;

  setmod_gf2n(p1);

  ptoup2(p0,&up0);
  n = degup2(up0); w = up0->w;
  substp(CO,p0,p0->v,(P)root,&tmp);
  if ( tmp )
    error("error in find_root_up2");
  u = root->body;

  p01 = (unsigned int **)almat(n,w);
  p10 = (unsigned int **)almat(n,w);
  if ( to == GF2N_PBTOPB ) {
    t = ONEUP2;
    bcopy(t->b,p01[0],t->w*sizeof(unsigned int));
    for ( i = 1; i < n; i++ ) {
      mulup2(t,u,&s); gen_simpup2_destructive(s,current_mod_gf2n); t = s;
      bcopy(t->b,p01[i],t->w*sizeof(unsigned int));
    }
  } else {
    t = u;
    bcopy(t->b,p01[n-1],t->w*sizeof(unsigned int));
    for ( i = 1; i < n; i++ ) {
      squareup2(t,&s); gen_simpup2_destructive(s,current_mod_gf2n); t = s;
      bcopy(t->b,p01[n-1-i],t->w*sizeof(unsigned int));
    }
  }
  if ( !invgf2mat(n,p01,p10) )
    error("compute_change_of_basis_matrix : something is wrong");
  TOGF2MAT(n,n,p01,*m01);
  TOGF2MAT(n,n,p10,*m10);
}

/*
 * 
 * computation of representation conversion matrix
 * repn x np -> repp, repp x pn -> repn
 *
 */

int compute_representation_conversion_matrix(p0,np,pn)
P p0;
GF2MAT *np,*pn;
{
  UP2 x,s;
  int n,w,i;
  unsigned int **ntop,**pton;

  setmod_gf2n(p0);

  n = UDEG(p0); w = n/BSH + (n%BSH?1:0);

  /* x = alpha */
  NEWUP2(x,1); x->w = 1; x->b[0]=2; 
  gen_simpup2_destructive(x,current_mod_gf2n);

  ntop = (unsigned int **)almat(n,w);
  pton = (unsigned int **)almat(n,w);

  bcopy(x->b,ntop[n-1],x->w*sizeof(unsigned int));
  for ( i = 1; i < n; i++ ) {
    squareup2(x,&s); gen_simpup2_destructive(s,current_mod_gf2n); x = s;
    bcopy(x->b,ntop[n-1-i],x->w*sizeof(unsigned int));
  }
  if ( !invgf2mat(n,ntop,pton) )
    return 0;
  TOGF2MAT(n,n,ntop,*np);
  TOGF2MAT(n,n,pton,*pn);
  return 1;
}

void mul_nb(mat,a,b,c)
GF2MAT mat;
unsigned int *a,*b,*c;
{
  int n,w,i;
  unsigned int *wa,*wb,*t;
  unsigned int **m;

  n = mat->row;
  m = mat->body;
  w = n/BSH + (n%BSH?1:0);
  wa = (unsigned int *)ALLOCA(w*sizeof(unsigned int));
  wb = (unsigned int *)ALLOCA(w*sizeof(unsigned int));
  t = (unsigned int *)ALLOCA(w*sizeof(unsigned int));
  bcopy(a,wa,w*sizeof(unsigned int));
  bcopy(b,wb,w*sizeof(unsigned int));
  bzero((char *)c,w*sizeof(unsigned int));
  for ( i = n-1; i >= 0; i-- ) {
    mulgf2vectmat(n,wa,m,t);
    if ( mulgf2vectvect(n,t,wb) )
      c[i/BSH] |= 1<<(i%BSH);
    leftshift(wa,n);
    leftshift(wb,n);
  }
}


/* 
  a=(c0,c1,...,c{n-1})->(c1,c2,...,c{n-1},c0)
*/

void leftshift(a,n)
unsigned int *a;
int n;
{
  int r,w,i;
  unsigned int msb;

  r = n%BSH;
  w = n/BSH + (r?1:0);
  msb = a[(n-1)/BSH]&(1<<((n-1)%BSH));
  for ( i = w-1; i > 0; i-- )
    a[i] = (a[i]<<1)|(a[i-1]>>(BSH-1));
  a[0] = (a[0]<<1)|(msb?1:0);
  if ( r )
    a[w-1] &= (1<<r)-1;
}

void mat_to_gf2mat(a,b)
MAT a;
unsigned int ***b;
{
  int n,w,i,j;
  unsigned int **m;

  n = a->row;
  w = n/BSH + (n%BSH?1:0);
  *b = m = (unsigned int **)almat(n,w);
  for ( i = 0; i < n; i++ )
    for ( j = 0; j < n; j++ )
      if ( a->body[i][j] )
        m[i][j/BSH] |= 1<<(j%BSH);
}

void gf2mat_to_mat(a,n,b)
unsigned int **a;
MAT *b;
{
  int w,i,j;
  MAT m;

  MKMAT(m,n,n); *b = m;
  w = n/BSH + (n%BSH?1:0);
  for ( i = 0; i < n; i++ )
    for ( j = 0; j < n; j++ )
      if ( a[i][j/BSH] & 1<<(j%BSH) )
        m->body[i][j] = (pointer)ONE;
}

void mulgf2mat(n,a,b,c)
int n;
unsigned int **a,**b,**c;
{
  int i,j,k,w;

  w = n/BSH + (n%BSH?1:0);
  for ( i = 0; i < n; i++ ) {
    bzero((char *)c[i],w*sizeof(unsigned int));
    for ( j = 0; j < n; j++ )
      if ( a[i][j/BSH] & 1<<(j%BSH) )  
        for ( k = 0; k < w; k++ )
          c[i][k] ^= b[j][k];
  }
}

/* c = a*b; where a, c are row vectors */

void mulgf2vectmat(n,a,b,c)
int n;
unsigned int *a;
unsigned int **b;
unsigned int *c;
{
  int j,k,w;

  w = n/BSH + (n%BSH?1:0);
  bzero((char *)c,w*sizeof(unsigned int));
  for ( j = 0; j < n; j++ )
    if ( a[j/BSH] & 1<<(j%BSH) )  
      for ( k = 0; k < w; k++ )
        c[k] ^= b[j][k];
}

int mulgf2vectvect(n,a,b)
int n;
unsigned int *a,*b;
{
  unsigned int t,r;
  int i,w;

  w = n/BSH + (n%BSH?1:0);
  for ( i = 0, r = 0; i < w; i++ )
    for ( t = a[i]&b[i]; t; t >>=1 )
      r ^= (t&1);
  return r;
}

int invgf2mat(n,a,b)
int n;
unsigned int **a,**b;
{
  int i,j,k,hcol,hbit,w;
  unsigned int *u;
  unsigned int **s;

  w = n/BSH + (n%BSH?1:0);
  s = (unsigned int **)almat(n,w);
  for ( i = 0; i < n; i++ )
    bcopy(a[i],s[i],w*sizeof(unsigned int));

  for ( i = 0; i < n; i++ )
    b[i][i/BSH] |= 1<<(i%BSH);

  for ( j = 0; j < n; j++ ) {
    hcol = j/BSH; hbit = 1<<(j%BSH);
    for ( i = j; i < n; i++ )
      if ( s[i][hcol] & hbit )
        break;
    if ( i == n )
      return 0;
    if ( i != j ) {
      u = s[i]; s[i] = s[j]; s[j] = u;
      u = b[i]; b[i] = b[j]; b[j] = u;
    }
    for ( i = 0; i < n; i++ ) {
      if ( i == j )
        continue;
      if ( s[i][hcol] & hbit ) {
        for ( k = hcol; k < w; k++ )
          s[i][k] ^= s[j][k];
        for ( k = 0; k < w; k++ )
          b[i][k] ^= b[j][k];
      }
    }
  }
  return 1;
}

INLINE void _mulup2_11(a1,a2,ar)
unsigned int a1,a2;
unsigned int *ar;
{
  GF2M_MUL_1(ar[1],ar[0],a1,a2);
}

void _mulup2_22(a1,a2,ar)
unsigned int *a1,*a2,*ar;
{
  unsigned int m[2];

  _mulup2_11(*a1,*a2,ar);
  _mulup2_11(*(a1+1),*(a2+1),ar+2);
  _mulup2_11(a1[0]^a1[1],a2[0]^a2[1],m);
  m[0] ^= ar[0]^ar[2]; m[1] ^= ar[1]^ar[3];

  ar[1] ^= m[0]; ar[2] ^= m[1];
}

#if 0
void _mulup2_33(a1,a2,ar)
unsigned int *a1,*a2,*ar;
{
  unsigned int m[4];
  unsigned int c1[2],c2[2];

  c1[0] = a1[2]^a1[0]; c1[1] = a1[1];
  c2[0] = a2[2]^a2[0]; c2[1] = a2[1];

  _mulup2_22(a1,a2,ar);
  _mulup2_11(*(a1+2),*(a2+2),ar+4);
  _mulup2_22(c1,c2,m);

  m[0] ^= ar[0]^ar[4]; m[1] ^= ar[1]^ar[5];
  m[2] ^= ar[2]; m[3] ^= ar[3];

  ar[2] ^= m[0]; ar[3] ^= m[1]; ar[4] ^= m[2]; ar[5] ^= m[3];
}
#else
/* (ar[5]...ar[0]) = (a1[2]a1[1]a1[0])*(a2[2]a2[1]a2[0])  */

void _mulup2_33(a1,a2,ar)
unsigned int *a1,*a2,*ar;
{
  unsigned int m[4];
  unsigned int c[2];
  unsigned int t,s;

  /* (ar[1],ar[0]) = a1[0]*a2[0] */
  _mulup2_11(a1[0],a2[0],ar);

  /* (ar[3],ar[2]) = hi(m) = a1[1]*a1[1] */
  _mulup2_11(a1[1],a2[1],ar+2); m[2] = ar[2]; m[3] = ar[3];

  /* (ar[5],ar[4]) = a1[2]*a2[2] */
  _mulup2_11(a1[2],a2[2],ar+4);

  /*
   * (ar[3],ar[2],ar[1],ar[0]) = (a1[1],a1[0])*(a2[1],a2[0])
   * a1[1]*a2[1] is already in (ar[3],ar[2])
   */
  /* c = (a1[1]+a1[0])*(a2[1]+a2[0]) */
  t = a1[1]^a1[0]; s = a2[1]^a2[0]; _mulup2_11(t,s,c);
  /* c += (ar[1],ar[0])+(ar[3],ar[2]) */
  c[0] ^= ar[2]^ar[0]; c[1] ^= ar[3]^ar[1];
  /* (ar[2],ar[1]) += c */
  ar[1] ^= c[0]; ar[2] ^= c[1];

  /*
   * m = (a1[1],a1[2]+a1[0])*(a2[1],a2[2]+a2[0])
   * a1[1]*a2[1] is already in hi(m)
   *
   */
  /* low(m) = (a1[0]+a1[2])*(a2[0]+a2[2]) */
  t = a1[2]^a1[0]; s = a2[2]^a2[0]; _mulup2_11(t,s,m);
  /* c = (a1[1]+(a1[0]+a1[2]))*(a2[1]+(a2[0]+a2[2])) */
  t ^= a1[1]; s ^= a2[1]; _mulup2_11(t,s,c);
  /* c += low(m)+hi(m) */
  c[0] ^= m[2]^m[0]; c[1] ^= m[3]^m[1];
  /* mid(m) += c */
  m[1] ^= c[0]; m[2] ^= c[1];

  /* m += (ar[3],ar[2],ar[1],ar[0])+(ar[5],ar[4]) */
  m[0] ^= ar[0]^ar[4]; m[1] ^= ar[1]^ar[5];
  m[2] ^= ar[2]; m[3] ^= ar[3];

  /* (ar[5],ar[4],ar[3],ar[2]) += m */
  ar[2] ^= m[0]; ar[3] ^= m[1]; ar[4] ^= m[2]; ar[5] ^= m[3];
}
#endif

void _mulup2_44(a1,a2,ar)
unsigned int *a1,*a2,*ar;
{
  unsigned int m[4];
  unsigned int c1[2],c2[2];

  c1[0] = a1[2]^a1[0]; c1[1] = a1[3]^a1[1];
  c2[0] = a2[2]^a2[0]; c2[1] = a2[3]^a2[1];

  _mulup2_22(a1,a2,ar);
  _mulup2_22(a1+2,a2+2,ar+4);
  _mulup2_22(c1,c2,m);

  m[0] ^= ar[0]^ar[4]; m[1] ^= ar[1]^ar[5];
  m[2] ^= ar[2]^ar[6]; m[3] ^= ar[3]^ar[7];

  ar[2] ^= m[0]; ar[3] ^= m[1]; ar[4] ^= m[2]; ar[5] ^= m[3];
}

void _mulup2_55(a1,a2,ar)
unsigned int *a1,*a2,*ar;
{
  unsigned int m[6];
  unsigned int c1[3],c2[3];

  c1[0]=a1[3]^a1[0]; c1[1]=a1[4]^a1[1]; c1[2]=a1[2];
  c2[0]=a2[3]^a2[0]; c2[1]=a2[4]^a2[1]; c2[2]=a2[2];

  _mulup2_33(a1,a2,ar);
  _mulup2_22(a1+3,a2+3,ar+6);
  _mulup2_33(c1,c2,m);

  m[0] ^= ar[0]^ar[6]; m[1] ^= ar[1]^ar[7]; m[2] ^= ar[2]^ar[8];
  m[3] ^= ar[3]^ar[9]; m[4] ^= ar[4]; m[5] ^= ar[5];

  ar[3] ^= m[0]; ar[4] ^= m[1]; ar[5] ^= m[2];
  ar[6] ^= m[3]; ar[7] ^= m[4]; ar[8] ^= m[5];
}

void _mulup2_66(a1,a2,ar)
unsigned int *a1,*a2,*ar;
{
  unsigned int m[6];
  unsigned int c1[3],c2[3];

  c1[0]=a1[3]^a1[0]; c1[1]=a1[4]^a1[1]; c1[2]=a1[5]^a1[2];
  c2[0]=a2[3]^a2[0]; c2[1]=a2[4]^a2[1]; c2[2]=a2[5]^a2[2];

  _mulup2_33(a1,a2,ar);
  _mulup2_33(a1+3,a2+3,ar+6);
  _mulup2_33(c1,c2,m);

  m[0] ^= ar[0]^ar[6]; m[1] ^= ar[1]^ar[7]; m[2] ^= ar[2]^ar[8];
  m[3] ^= ar[3]^ar[9]; m[4] ^= ar[4]^ar[10]; m[5] ^= ar[5]^ar[11];

  ar[3] ^= m[0]; ar[4] ^= m[1]; ar[5] ^= m[2];
  ar[6] ^= m[3]; ar[7] ^= m[4]; ar[8] ^= m[5];
}

#if 0
void printup2_(f,l)
unsigned int *f;
int l;
{
  int i;

  for ( i = 32*l-1; i >= 0; i-- ) {
    if ( f[i>>5]&(1<<(i&31)) )
      fprintf(stderr,"+x^%d",i);
  }
  fprintf(stderr,"\n");
}
#endif

void type1_bin_invup2(a,n,inv)
UP2 a;
int n;
UP2 *inv;
{
  int lf,lg,i,j,k,lg2,df,dg,l,w;
  unsigned int r;
  UP2 wf,wg,wb,wc,ret,t;
  unsigned int *p;

  lf = a->w;
  lg = (n>>5)+1;

  W_NEWUP2(wf,lf+1); _copyup2(a,wf);
  W_NEWUP2(wg,lg+1); wg->w = lg;
  for ( i = lg-1, p = wg->b; i>=0; i-- )
    p[i] = 0xffffffff;
  if ( r = (n+1)&31 )
    p[lg-1] &= (1<<r)-1;
  lg2 = lg*2;
  W_NEWUP2(wb,lg2+2); wb->w = 1; wb->b[0] = 1;
  W_NEWUP2(wc,lg2+2); wc->w = 0;
  k = 0;
  df = degup2(wf); dg = degup2(wg);
  while ( 1 ) {
    lf = wf->w;
    p = wf->b;
    for ( j = 0; j < lf && !p[j]; j++ );
    for ( l = j<<5, w = p[j]; !(w&1); w >>=1, l++ );
    _bshiftup2_destructive(wf,l);
    _bshiftup2_destructive(wc,-l);
    k += l;
    df -= l;
    if ( wf->w == 1 && wf->b[0] == 1 ) {
      k %= (n+1);
      if ( k != 1 ) {
        _bshiftup2_destructive(wb,k-(n+1));
        remup2_type1_destructive(wb,n);
      }
      NEWUP2(ret,wb->w);
      _copyup2(wb,ret);
      *inv = ret;
      return;
    }
    if ( df < dg ) {
      i = df; df = dg; dg = i;
      t = wf; wf = wg; wg = t;
      t = wb; wb = wc; wc = t;
    }
    /* df >= dg */
    _addup2_destructive(wf,wg);
    df = degup2(wf);
    _addup2_destructive(wb,wc);
  }
}

UP2 *compute_tab_gf2n(f)
UP2 f;
{
  GEN_UP2 mod;
  int m,n,w,i;
  UP2 t;
  UP2 *tab;

  mod = current_mod_gf2n;
  m = degup2(mod->dense);
  n = degup2(f);
  w = f->w;
  tab = (UP2 *)CALLOC(m,sizeof(UP2));
  NEWUP2(tab[0],w); tab[0]->w = 1; tab[0]->b[0] = 2;
  for ( i = 1; i < m; i++ ) {
    squareup2(tab[i-1],&t); remup2(t,f,&tab[i]);
  }
  return tab;
}

UP compute_trace_gf2n(tab,c,n)
UP2 *tab;
GF2N c;
int n;
{
  GEN_UP2 mod;
  int w,m,i,j;
  UP2 *trace;
  UP2 c1,c2,s,t;
  UP r;
  GF2N a;

  mod = current_mod_gf2n;
  w = mod->dense->w;
  m = degup2(mod->dense);
  trace = (UP2 *)ALLOCA(n*sizeof(UP2));
  for ( i = 0; i < n; i++ ) {
    W_NEWUP2(trace[i],w); trace[i]->w = 0;
  }
  W_NEWUP2(c1,2*w); _copyup2(c->body,c1);
  W_NEWUP2(c2,2*w);

  for ( i = 0; i < m; i++ ) {
    t = tab[i];
    /* trace += c^(2^i)*tab[i] */
    for ( j = degup2(t); j >= 0; j-- )
      if ( t->b[j/BSH] & (1<<(j%BSH)) )
        _addup2_destructive(trace[j],c1);
    _squareup2(c1,c2); gen_simpup2_destructive(c2,mod);
    t = c1; c1 = c2; c2 = t;
  }
  for ( i = n-1; i >= 0 && !trace[i]->w; i-- );
  if ( i < 0 )
    r = 0;
  else {
    r = UPALLOC(i); r->d = i;
    for ( j = 0; j <= i; j++ )
      if ( t = trace[j] ) {
        NEWUP2(s,t->w); _copyup2(t,s);
        MKGF2N(s,a); r->c[j] = (Num)a;
      }
  }
  return r;      
}

void up2toup(f,r)
UP2 f;
UP *r;
{
  int d,i;
  UP2 c;
  UP t;
  GF2N a;

  if ( !f || !f->w )
    *r = 0;
  else {
    d = degup2(f);
    t = UPALLOC(d); *r = t;
    t->d = d;
    for ( i = 0; i <= d; i++ ) {
      if ( f->b[i/BSH] & (1<<(i%BSH)) ) {
        NEWUP2(c,1);
        c->w = 1; c->b[0] = 1;
        MKGF2N(c,a);
        t->c[i] = (Num)a;
      }
    }
  }
}

void find_root_up2(f,r)
UP2 f;
GF2N *r;
{
  int n;
  UP2 *tab;
  UP uf,trace,gcd,quo,rem;
  GF2N c;
  int i;

  /* computeation of tab : tab[i] = t^(2^i) mod f (i=0,...,n-1) */
  n = degup2(f);
  tab = compute_tab_gf2n(f);
  up2toup(f,&uf);
  while ( uf->d > 1 ) {
    randomgf2n(&c);
    if ( !c )
      continue;
    /* trace = (ct)+(ct)^2+...+(ct)^(2^(n-1)) */
    trace = compute_trace_gf2n(tab,c,n);
    gcdup(trace,uf,&gcd);
    if ( gcd->d > 0 && gcd->d < uf->d ) {
/*      fprintf(stderr,"deg(GCD)=%d\n",gcd->d); */
      if ( 2*gcd->d > uf->d ) {
        qrup(uf,gcd,&quo,&rem);
        uf = quo;
      } else {
        uf = gcd;
      }
    }
  }
  /* now deg(uf) = 1. The root of uf is ug->c[0]/uf->c[1] */
  divgf2n((GF2N)uf->c[0],(GF2N)uf->c[1],r);
}
