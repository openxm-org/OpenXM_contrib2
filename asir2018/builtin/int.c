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
 * $OpenXM: OpenXM_contrib2/asir2018/builtin/int.c,v 1.4 2018/09/28 08:20:27 noro Exp $
*/
#include "ca.h"
#include "parse.h"
#include "base.h"

void Pidiv(), Pirem(), Pigcd(), Pilcm(), Pfac(), Prandom(), Pinv();
void Pup2_inv(),Pgf2nton(), Pntogf2n();
void Pup2_init_eg(), Pup2_show_eg();
void Piqr(), Pprime(), Plprime(), Pinttorat();
void Piand(), Pior(), Pixor(), Pishift();
void Pisqrt();
void Plrandom();
void Pset_upkara(), Pset_uptkara(), Pset_up2kara(), Pset_upfft();
void Pmt_save(), Pmt_load();
void Psmall_jacobi();
void Pdp_set_mpi();
void Pntoint32(),Pint32ton();

void Pigcdbin(), Pigcdbmod(), PigcdEuc(), Pigcdacc(), Pigcdcntl();

void Pihex();
void Pimaxrsh(), Pilen();
void Ptype_t_NB();
void Plprime64();

struct ftab int_tab[] = {
  {"dp_set_mpi",Pdp_set_mpi,-1},
  {"isqrt",Pisqrt,1},
  {"idiv",Pidiv,2},
  {"irem",Pirem,2},
  {"iqr",Piqr,2},
  {"igcd",Pigcd,-2},
  {"ilcm",Pilcm,2},
  {"up2_inv",Pup2_inv,2},
  {"up2_init_eg",Pup2_init_eg,0},
  {"up2_show_eg",Pup2_show_eg,0},
  {"type_t_NB",Ptype_t_NB,2},
  {"gf2nton",Pgf2nton,1},
  {"ntogf2n",Pntogf2n,1},
  {"set_upkara",Pset_upkara,-1},
  {"set_uptkara",Pset_uptkara,-1},
  {"set_up2kara",Pset_up2kara,-1},
  {"set_upfft",Pset_upfft,-1},
  {"inv",Pinv,2},
  {"inttorat",Pinttorat,3},
  {"fac",Pfac,1},
  {"prime",Pprime,1},
  {"lprime",Plprime,1},
#if SIZEOF_LONG==8
  {"lprime64",Plprime64,1},
#endif
  {"random",Prandom,-1},
  {"lrandom",Plrandom,1},
  {"iand",Piand,2},
  {"ior",Pior,2},
  {"ixor",Pixor,2},
  {"ishift",Pishift,2},
  {"small_jacobi",Psmall_jacobi,2},

  {"igcdbin",Pigcdbin,2},    /* HM@CCUT extension */
  {"igcdbmod",Pigcdbmod,2},  /* HM@CCUT extension */
  {"igcdeuc",PigcdEuc,2},    /* HM@CCUT extension */
  {"igcdacc",Pigcdacc,2},    /* HM@CCUT extension */
  {"igcdcntl",Pigcdcntl,-1},  /* HM@CCUT extension */

  {"mt_save",Pmt_save,1},
  {"mt_load",Pmt_load,1},
  {"ntoint32",Pntoint32,1},
  {"int32ton",Pint32ton,1},
  {0,0,0},
};

static int is_prime_small(unsigned int);
static unsigned int gcd_small(unsigned int,unsigned int);
int TypeT_NB_check(unsigned int, unsigned int);
int mpi_mag;

void Pntoint32(NODE arg,USINT *rp)
{
  Z q,z;
  unsigned int t;

  asir_assert(ARG0(arg),O_N,"ntoint32");
  q = (Z)ARG0(arg);
  if ( !q ) {
    MKUSINT(*rp,0);
    return;
  }
  if ( !INT(q) || !smallz(q) )
    error("ntoint32 : invalid argument");
  absz(q,&z);
  t = ZTOS(z);
  if ( sgnz(q) < 0 )
    t = -(int)t;
  MKUSINT(*rp,t);
}

void Pint32ton(NODE arg,Z *rp)
{
  int t;

  asir_assert(ARG0(arg),O_USINT,"int32ton");
  t = (int)BDY((USINT)ARG0(arg));
  STOZ(t,*rp);
}

void Pdp_set_mpi(NODE arg,Z *rp)
{
  if ( arg ) {
    asir_assert(ARG0(arg),O_N,"dp_set_mpi");
    mpi_mag = ZTOS((Q)ARG0(arg));
  }
  STOZ(mpi_mag,*rp);
}

void Psmall_jacobi(NODE arg,Z *rp)
{
  Z a,m;
  int a0,m0,s;

  a = (Z)ARG0(arg);
  m = (Z)ARG1(arg);
  asir_assert(a,O_N,"small_jacobi");
  asir_assert(m,O_N,"small_jacobi");
  if ( !a )
     *rp = ONE;
  else if ( !m || !INT(m) || !INT(a) 
    || !smallz(m) || !smallz(a) || sgnz(m) < 0 || evenz(m) )
    error("small_jacobi : invalid input");
  else {
    a0 = ZTOS(a); m0 = ZTOS(m);
    s = small_jacobi(a0,m0);
    STOZ(s,*rp);
  }
}

int small_jacobi(int a,int m)
{
  int m4,m8,a4,j1,i,s;

  a %= m;
  if ( a == 0 || a == 1 )
    return 1;
  else if ( a < 0 ) {
    j1 = small_jacobi(-a,m);
    m4 = m%4;
    return m4==1?j1:-j1;
  } else { 
    for ( i = 0; a && !(a&1); i++, a >>= 1 );
    if ( i&1 ) {
      m8 = m%8;
      s = (m8==1||m8==7) ? 1 : -1;
    } else
      s = 1;
    /* a, m are odd */
    j1 = small_jacobi(m%a,a);
    m4 = m%4; a4 = a%4;
    s *= (m4==1||a4==1) ? 1 : -1; 
    return j1*s;
  }  
}

void Ptype_t_NB(NODE arg,Z *rp)
{
  if ( TypeT_NB_check(ZTOS((Q)ARG0(arg)),ZTOS((Q)ARG1(arg))))
    *rp = ONE;
  else
    *rp = 0;
}

int TypeT_NB_check(unsigned int m, unsigned int t)
{
  unsigned int p,k,u,h,d;

  if ( !(m%8) )
    return 0;
  p = t*m+1;
  if ( !is_prime_small(p) )
    return 0;
  for ( k = 1, u = 2%p; ; k++ )
    if ( u == 1 )
      break;
    else
      u = (2*u)%p;
  h = t*m/k;
  d = gcd_small(h,m);
  return d == 1 ? 1 : 0;
}

/*
 * a simple prime checker
 * return value: 1  ---  prime number
 *               0  ---  composite number
 */

static int is_prime_small(unsigned int a)
{
  unsigned int b,t,i;

  if ( !(a%2) ) return 0;
  for ( t = a, i = 0; t; i++, t >>= 1 );
  /* b >= sqrt(a) */
  b = 1<<((i+1)/2);

  /* divisibility test by all odd numbers <= b */
  for ( i = 3; i <= b; i += 2 )
    if ( !(a%i) )
      return 0;
  return 1;
}

/*
 * gcd for unsigned int as integers
 * return value: GCD(a,b)
 *
 */


static unsigned int gcd_small(unsigned int a,unsigned int b)
{
  unsigned int t;

  if ( b > a ) {
    t = a; a = b; b = t;
  }
  /* Euclid's algorithm */
  while ( 1 )
    if ( !(t = a%b) ) return b;
    else {
      a = b; b = t;
    }
}

void Pmt_save(NODE arg,Z *rp)
{
  int ret;

  ret = mt_save(BDY((STRING)ARG0(arg)));
  STOZ(ret,*rp);
}

void Pmt_load(NODE arg,Z *rp)
{
  int ret;

  ret = mt_load(BDY((STRING)ARG0(arg)));
  STOZ(ret,*rp);
}

void isqrt(Z a,Z *r);

void Pisqrt(NODE arg,Z *rp)
{
  Z a;
  Z r;

  a = (Z)ARG0(arg);
  asir_assert(a,O_N,"isqrt");
  if ( !a )
    *rp = 0;
  else if ( sgnz(a) < 0 )
    error("isqrt : negative argument");
  else {
    isqrt(a,rp);
  }
}

void Pidiv(NODE arg,Z *rp)
{
  Z r;
  Z dnd,dvr;
  
  dnd = (Z)ARG0(arg); dvr = (Z)ARG1(arg);
  asir_assert(dnd,O_N,"idiv");
  asir_assert(dvr,O_N,"idiv");
  if ( !dvr )
    error("idiv: division by 0");
  else if ( !dnd )
    *rp = 0;
  else
    divqrz(dnd,dvr,rp,&r);
}

void Pirem(NODE arg,Z *rp)
{
  Z q,dnd,dvr;

  dnd = (Z)ARG0(arg); dvr = (Z)ARG1(arg);
  asir_assert(dnd,O_N,"irem");
  asir_assert(dvr,O_N,"irem");
  if ( !dvr )
    error("irem: division by 0");
  else if ( !dnd )
    *rp = 0;
  else
    divqrz(dnd,dvr,&q,rp);
}

void iqrv(VECT a,Z dvr,LIST *rp);

void Piqr(NODE arg,LIST *rp)
{
  Z dnd,dvr,a,b;
  NODE node;

  dnd = (Z)ARG0(arg); dvr = (Z)ARG1(arg);
  if ( !dvr )
    error("iqr: division by 0");
  else if ( !dnd )
    a = b = 0;
  else if ( OID(dnd) == O_VECT ) {
    iqrv((VECT)dnd,dvr,rp); return;
  } else {
    asir_assert(dnd,O_N,"iqr");
    asir_assert(dvr,O_N,"iqr");
    divqrz(dnd,dvr,&a,&b);
  }
  node = mknode(2,a,b); MKLIST(*rp,node);
}

void Pinttorat(NODE arg,LIST *rp)
{
  int ret;
  Z c,m,t,b,nm,dn;
  NODE node;

  asir_assert(ARG0(arg),O_N,"inttorat");
  asir_assert(ARG1(arg),O_N,"inttorat");
  asir_assert(ARG2(arg),O_N,"inttorat");
  c = (Z)ARG0(arg); m = (Z)ARG1(arg); b = (Z)ARG2(arg);
  remz(c,m,&t); c = t;
  ret = inttorat(c,m,b,&nm,&dn);
  if ( !ret )
    *rp = 0;
  else {
    node = mknode(2,nm,dn); MKLIST(*rp,node);
  }
}

void Pigcd(NODE arg,Z *rp)
{
  Z n1,n2;
  
  if ( argc(arg) == 1 ) {
    gcdvz((VECT)ARG0(arg),rp);
    return;
  }
  n1 = (Z)ARG0(arg); n2 = (Z)ARG1(arg);
  asir_assert(n1,O_N,"igcd");
  asir_assert(n2,O_N,"igcd");
  gcdz(n1,n2,rp);
}

void iqrv(VECT a,Z dvr,LIST *rp)
{
  int i,n;
  VECT q,r;
  Z *b;
  NODE n0;

  if ( !dvr )
    error("iqrv: division by 0");
  n = a->len; b = (Z *)BDY(a);
  MKVECT(q,n); MKVECT(r,n);
  for ( i = 0; i < n; i++ )
    divqrz(b[i],dvr,(Z *)&BDY(q)[i],(Z *)&BDY(r)[i]);
  n0 = mknode(2,q,r); MKLIST(*rp,n0);
}

/*
 * gcd = GCD(a,b), ca = a/g, cb = b/g
 */

void igcd_cofactor(Z a,Z b,Z *gcd,Z *ca,Z *cb)
{
  Z g;

  if ( !a ) {
    if ( !b )
      error("igcd_cofactor : invalid input");
    else {
      *ca = 0;
      *cb = ONE;
      *gcd = b;
    }
  } else if ( !b ) {
    *ca = ONE;
    *cb = 0;
    *gcd = a;
  } else {
    gcdz(a,b,&g);
    divsz(a,g,ca);
    divsz(b,g,cb);
    *gcd = g;
  }
}

void Pilcm(NODE arg,Z *rp)
{
  Z n1,n2,g,q,l;

  n1 = (Z)ARG0(arg); n2 = (Z)ARG1(arg);
  asir_assert(n1,O_N,"ilcm");
  asir_assert(n2,O_N,"ilcm");
  if ( !n1 || !n2 )
    *rp = 0;
  else {
    gcdz(n1,n2,&g); divsz(n1,g,&q);
    mulz(q,n2,&l); absz(l,rp);
  }
}

void Piand(NODE arg,Z *rp)
{
  mpz_t t;
  Z n1,n2;

  n1 = (Z)ARG0(arg); n2 = (Z)ARG1(arg);
  asir_assert(n1,O_N,"iand");
  asir_assert(n2,O_N,"iand");
  if ( !n1 || !n2 ) *rp = 0;
  else {
    mpz_init(t);
    mpz_and(t,BDY(n1),BDY(n2));
    MPZTOZ(t,*rp);
  }
}

void Pior(NODE arg,Z *rp)
{
  Z n1,n2;
  mpz_t t;
  
  n1 = (Z)ARG0(arg); n2 = (Z)ARG1(arg);
  asir_assert(n1,O_N,"ior");
  asir_assert(n2,O_N,"ior");
  if ( !n1 ) *rp = n2;
  else if ( !n2 ) *rp = n1;
  else {
    mpz_init(t);
    mpz_ior(t,BDY(n1),BDY(n2));
    MPZTOZ(t,*rp);
  }
}

void Pixor(NODE arg,Z *rp)
{
  Z n1,n2;
  mpz_t t;
  
  n1 = (Z)ARG0(arg); n2 = (Z)ARG1(arg);
  asir_assert(n1,O_N,"ixor");
  asir_assert(n2,O_N,"ixor");
  if ( !n1 ) *rp = n2;
  else if ( !n2 ) *rp = n1;
  else {
    mpz_init(t);
    mpz_xor(t,BDY(n1),BDY(n2));
    MPZTOZ(t,*rp);
  }
}

void Pishift(NODE arg,Z *rp)
{
  int i;
  Z n1,s;
  mpz_t t;

  n1 = (Z)ARG0(arg);
  s = (Z)ARG1(arg);
  asir_assert(n1,O_N,"ixor");
  asir_assert(s,O_N,"ixor");
  bshiftz(n1,ZTOS(s),rp);
}

void isqrt(Z a,Z *r)
{
  int k;
  Z x,t,x2,xh,quo,rem;

  if ( !a )
    *r = 0;
  else if ( UNIZ(a) )
    *r = ONE;
  else {
    k = z_bits((Q)a); /* a <= 2^k-1 */
    bshiftz(ONE,-((k>>1)+(k&1)),&x); /* a <= x^2 */
    while ( 1 ) {
      mulz(x,x,&t);
      if ( cmpz(t,a) <= 0 ) {
        *r = x; return;
      } else {
        if ( tstbitz(x,0) )
          addz(x,a,&t);
        else
          t = a;
        bshiftz(x,-1,&x2); divqrz(t,x2,&quo,&rem);
        bshiftz(x,1,&xh); addz(quo,xh,&x);
      }
    }
  }
}

void Pup2_init_eg(Obj *rp)
{
  up2_init_eg();
  *rp = 0;
}

void Pup2_show_eg(Obj *rp)
{
  up2_show_eg();
  *rp = 0;
}

void Pgf2nton(NODE arg,Z *rp)
{
  if ( !ARG0(arg) )
    *rp = 0;
  else
    up2toz(((GF2N)ARG0(arg))->body,rp);
}

void Pntogf2n(NODE arg,GF2N *rp)
{
  UP2 t;

  ztoup2((Z)ARG0(arg),&t);
  MKGF2N(t,*rp);
}

void Pup2_inv(NODE arg,P *rp)
{
  UP2 a,b,t;

  ptoup2(ARG0(arg),&a);
  ptoup2(ARG1(arg),&b);
  invup2(a,b,&t);
  up2top(t,rp);
}

void Pinv(NODE arg,Num *rp)
{
  Num n;
  Z mod;
  MQ r;  
  int inv;

  n = (Num)ARG0(arg); mod = (Z)ARG1(arg);
  asir_assert(n,O_N,"inv");
  asir_assert(mod,O_N,"inv");
  if ( !n || !mod )
    error("inv: invalid input");
  else
    switch ( NID(n) ) {
      case N_Q:
        invz((Z)n,mod,(Z *)rp);
        break;
      case N_M:
        inv = invm(CONT((MQ)n),ZTOS(mod));
        STOMQ(inv,r);
        *rp = (Num)r;
        break;
      default:
        error("inv: invalid input");
    }
}

void Pfac(NODE arg,Z *rp)
{ 
  asir_assert(ARG0(arg),O_N,"fac");
  factorialz(ZTOS((Q)ARG0(arg)),rp); 
}

void Plrandom(NODE arg,Z *rp)
{
  asir_assert(ARG0(arg),O_N,"lrandom");
  randomz(ZTOS((Q)ARG0(arg)),rp);
}

void Prandom(NODE arg,Z *rp)
{
  unsigned int r;

#if 0
#if defined(_PA_RISC1_1)
  r = mrand48()&BMASK;
#else
  if ( arg )
    srandom(ZTOS((Q)ARG0(arg)));
  r = random()&BMASK;
#endif
#endif
  if ( arg )
    mt_sgenrand(ZTOS((Q)ARG0(arg)));
  r = mt_genrand();
  UTOZ(r,*rp);
}

#if defined(VISUAL) || defined(__MINGW32__)
void srandom(unsigned int);

static unsigned int R_Next;

unsigned int random() {
        if ( !R_Next )
                R_Next = 1;
        return R_Next = (R_Next * 1103515245 + 12345);
}

void srandom(unsigned int s)
{
    if ( s )
      R_Next = s;
        else if ( !R_Next )
            R_Next = 1;
}
#endif

void Pprime(NODE arg,Z *rp)
{
  int i;

  asir_assert(ARG0(arg),O_N,"prime");
  i = ZTOS((Q)ARG0(arg));
  if ( i < 0 || i >= 1900 )
    *rp = 0;
  else
    STOZ(sprime[i],*rp);
}

void Plprime(NODE arg,Z *rp)
{
  int i,p;

  asir_assert(ARG0(arg),O_N,"lprime");
  i = ZTOS((Q)ARG0(arg));
  if ( i < 0 )
    *rp = 0;
  else 
    p = get_lprime(i);
  STOZ(p,*rp);
}

#if SIZEOF_LONG==8
void Plprime64(NODE arg,Z *rp)
{
  int i;
  mp_limb_t p;

  asir_assert(ARG0(arg),O_N,"lprime64");
  i = ZTOS((Q)ARG0(arg));
  if ( i < 0 )
    *rp = 0;
  else 
    p = get_lprime64(i);
  STOZ(p,*rp);
}
#endif

extern int up_kara_mag, up_tkara_mag, up_fft_mag;

void Pset_upfft(NODE arg,Z *rp)
{
  if ( arg ) {
    asir_assert(ARG0(arg),O_N,"set_upfft");
    up_fft_mag = ZTOS((Q)ARG0(arg));
  }
  STOZ(up_fft_mag,*rp);
}

void Pset_upkara(NODE arg,Z *rp)
{
  if ( arg ) {
    asir_assert(ARG0(arg),O_N,"set_upkara");
    up_kara_mag = ZTOS((Q)ARG0(arg));
  }
  STOZ(up_kara_mag,*rp);
}

void Pset_uptkara(NODE arg,Z *rp)
{
  if ( arg ) {
    asir_assert(ARG0(arg),O_N,"set_uptkara");
    up_tkara_mag = ZTOS((Q)ARG0(arg));
  }
  STOZ(up_tkara_mag,*rp);
}

extern int up2_kara_mag;

void Pset_up2kara(NODE arg,Z *rp)
{
  if ( arg ) {
    asir_assert(ARG0(arg),O_N,"set_up2kara");
    up2_kara_mag = ZTOS((Q)ARG0(arg));
  }
  STOZ(up2_kara_mag,*rp);
}

void Pigcdbin(NODE arg,Z *rp)
{
  Pigcd(arg,rp);
}

void Pigcdbmod(NODE arg,Z *rp)
{
  Pigcd(arg,rp);
}

void Pigcdacc(NODE arg,Z *rp)
{
  Pigcd(arg,rp);
}

void PigcdEuc(NODE arg,Z *rp)
{
  Pigcd(arg,rp);
}

void Pigcdcntl(NODE arg,Z *rp)
{
  *rp = ONE;
}
