#include "ca.h"
#include "gmp.h"
#include "base.h"
#include "inline.h"

mpz_t ONEMPZ;
Z ONE;
int lf_lazy;
Z current_mod_lf;
int current_mod_lf_size;
gmp_randstate_t GMP_RAND;

void isqrtz(Z a,Z *r);
void bshiftz(Z a,int n,Z *r);

void *gc_realloc(void *p,size_t osize,size_t nsize)
{
  return (void *)Risa_GC_realloc(p,nsize);  
}

void gc_free(void *p,size_t size)
{
  Risa_GC_free(p);
}

void init_gmpq()
{
  mp_set_memory_functions(Risa_GC_malloc_atomic,gc_realloc,gc_free);

  mpz_init(ONEMPZ); mpz_set_ui(ONEMPZ,1); MPZTOZ(ONEMPZ,ONE);
  gmp_randinit_default(GMP_RAND);
}

Z utoz(unsigned int u)
{
  mpz_t z;
  Z r;

  if ( !u ) return 0;
  mpz_init(z); mpz_set_ui(z,u); MPZTOZ(z,r); return r;
}

Z stoz(int s)
{
  mpz_t z;
  Z r;

  if ( !s ) return 0;
  mpz_init(z); mpz_set_si(z,s); MPZTOZ(z,r); return r;
}

int sgnz(Z z)
{
  if ( !z ) return 0;
  else return mpz_sgn(BDY(z));
}

void nmq(Q q,Z *r)
{
  if ( !q ) *r = 0;
  else if ( INT(q) ) *r = (Z)q;
  else {
    MPZTOZ(mpq_numref(BDY(q)),*r);
  }
}

void dnq(Q q,Z *r)
{
  if ( !q ) *r = 0;
  else if ( INT(q) ) *r = ONE;
  else {
    MPZTOZ(mpq_denref(BDY(q)),*r);
  }
}

int sgnq(Q q)
{
  if ( !q ) return 0;
  else if ( q->z ) return mpz_sgn(BDY((Z)q));
  else return mpz_sgn(mpq_numref(BDY(q)));
}

Q mpqtozq(mpq_t a)
{
  Z z;
  Q q;

  if ( INTMPQ(a) ) {
    MPZTOZ(mpq_numref(a),z); return (Q)z;
  } else {
    MPQTOQ(a,q); return q;
  }
}

void dupz(Z a,Z *b)
{
  mpz_t t;

  if ( !a ) *b = a;
  else {
    mpz_init(t); mpz_set(t,BDY(a)); MPZTOZ(t,*b);
  }
}

int n_bits_z(Z a)
{
  return a ? mpz_sizeinbase(BDY(a),2) : 0;
}

void addz(Z n1,Z n2,Z *nr)
{
  mpz_t t;
  int s1,s2;

  if ( !n1 ) *nr = n2;
  else if ( !n2 ) *nr = n1;
  else if ( !n1->z || !n2->z )
    error("addz : invalid argument");
  else {
    mpz_init(t); mpz_add(t,BDY(n1),BDY(n2)); MPZTOZ(t,*nr);
  }
}

void subz(Z n1,Z n2,Z *nr)
{
  mpz_t t;

  if ( !n1 ) {
    if ( !n2 ) 
      *nr = 0;
    else 
      chsgnz(n2,nr);
  } else if ( !n2 ) 
    *nr = n1;
  else if ( n1 == n2 ) 
    *nr = 0;
  else if ( !n1->z || !n2->z )
    error("subz : invalid argument");
  else {
    mpz_init(t); mpz_sub(t,BDY(n1),BDY(n2)); MPZTOZ(t,*nr);
  }
}

void mulz(Z n1,Z n2,Z *nr)
{
  mpz_t t;

  if ( !n1 || !n2 ) *nr = 0;
  else if ( !n1->z || !n2->z )
    error("mulz : invalid argument");
  else if ( UNIQ(n1) ) *nr = n2;
  else if ( UNIQ(n2) ) *nr = n1;
  else if ( MUNIQ(n1) ) chsgnz(n2,nr);
  else if ( MUNIQ(n2) ) chsgnz(n1,nr);
  else {
    mpz_init(t); mpz_mul(t,BDY(n1),BDY(n2)); MPZTOZ(t,*nr);
  }
}

/* nr += n1*n2 */

void muladdtoz(Z n1,Z n2,Z *nr)
{
  Z t;

  if ( n1 && n2 ) {
        if ( !(*nr) ) {
          NEWZ(t); mpz_init(BDY(t)); *nr = t;
        }
        mpz_addmul(BDY(*nr),BDY(n1),BDY(n2));
    }
}

/* nr += n1*u */

void mul1addtoz(Z n1,long u,Z *nr)
{
  Z t;

  if ( n1 && u ) {
        if ( !(*nr) ) {
          NEWZ(t); mpz_init(BDY(t)); *nr = t;
        }
        if ( u >= 0 )
          mpz_addmul_ui(BDY(*nr),BDY(n1),(unsigned long)u);
        else
          mpz_submul_ui(BDY(*nr),BDY(n1),(unsigned long)(-u));
    }
}

void mul1z(Z n1,long n2,Z *nr)
{
  mpz_t t;

  if ( !n1 || !n2 ) *nr = 0;
  else {
    mpz_init(t); mpz_mul_si(t,BDY(n1),n2); MPZTOZ(t,*nr);
  }
}

void divz(Z n1,Z n2,Z *nq)
{
  mpz_t t;
  mpq_t a, b, q;

  if ( !n2 ) {
    error("division by 0");
    *nq = 0;
  } else if ( !n1 ) 
    *nq = 0;
  else if ( n1 == n2 ) {
    mpz_init(t); mpz_set_ui(t,1); MPZTOZ(t,*nq);
  } else {
    MPZTOMPQ(BDY(n1),a); MPZTOMPQ(BDY(n2),b);
    mpq_init(q); mpq_div(q,a,b); *nq = (Z)mpqtozq(q);
  }
}

void remz(Z n1,Z n2,Z *nr)
{
  mpz_t r;

  if ( !n2 ) {
    error("division by 0");
    *nr = 0;
  } else if ( !n1 || n1 == n2 )
    *nr = 0;
  else if ( !n1->z || !n2->z )
    error("remz : invalid argument");
  else {
    mpz_init(r);
    mpz_mod(r,BDY(n1),BDY(n2));
    if ( !mpz_sgn(r) ) *nr = 0;
    else MPZTOZ(r,*nr);
  }
}

void divqrz(Z n1,Z n2,Z *nq,Z *nr)
{
  mpz_t t, a, b, q, r;

  if ( !n2 ) {
    error("division by 0");
    *nq = 0; *nr = 0;
  } else if ( !n1 ) {
    *nq = 0; *nr = 0;
  } else if ( !n1->z || !n2->z )
    error("divqrz : invalid argument");
  else if ( n1 == n2 ) {
    mpz_init(t); mpz_set_ui(t,1); MPZTOZ(t,*nq); *nr = 0;
  } else {
    mpz_init(q); mpz_init(r);
    mpz_fdiv_qr(q,r,BDY(n1),BDY(n2));
    if ( !mpz_sgn(q) ) *nq = 0;
    else MPZTOZ(q,*nq);
    if ( !mpz_sgn(r) ) *nr = 0;
    else MPZTOZ(r,*nr);
  }
}

void divsz(Z n1,Z n2,Z *nq)
{
  mpz_t t;
  mpq_t a, b, q;

  if ( !n2 ) {
    error("division by 0");
    *nq = 0;
  } else if ( !n1 ) 
    *nq = 0;
  else if ( !n1->z || !n2->z )
    error("divsz : invalid argument");
  else if ( n1 == n2 ) {
    mpz_init(t); mpz_set_ui(t,1); MPZTOZ(t,*nq);
  } else {
    mpz_init(t); mpz_divexact(t,BDY(n1),BDY(n2)); MPZTOZ(t,*nq);
  }
}

void chsgnz(Z n,Z *nr)
{
  mpz_t t;

  if ( !n )
    *nr = 0;
  else if ( !n->z )
    error("chsgnz : invalid argument");
  else {
    t[0] = BDY(n)[0]; mpz_neg(t,t); MPZTOZ(t,*nr);
  }
}

void absz(Z n,Z *nr)
{
  if ( !n ) *nr = 0;
  else if ( !n->z )
    error("absz : invalid argument");
  else if ( sgnz(n) < 0 ) chsgnz(n,nr);
  else *nr = n;
}

int evenz(Z n)
{
  return !n ? 1 : mpz_even_p(BDY(n));
}

int smallz(Z n)
{
  if ( !n ) return 1;
  else if ( INT(n) && mpz_fits_sint_p(BDY(n)) ) return 1;
  else return 0;
}

void pwrz(Z n1,Z n,Z *nr)
{
  mpq_t t,q;
  mpz_t z;
  Q p,r;

  if ( !n || UNIQ(n1) ) *nr = ONE;
  else if ( !n1 ) *nr = 0;
  else if ( !n->z || !n1->z )
    error("pwrz : invalid argument");
  else if ( MUNIQ(n1) ) {
    if ( mpz_even_p(BDY((Z)n)) ) *nr = ONE;
    else *nr = n1;
  } else if ( !smallz(n) ) {
    error("exponent too big."); *nr = 0;
  } else if ( n1->z && mpz_sgn(BDY((Z)n))>0 ) {
    mpz_init(z); mpz_pow_ui(z,BDY(n1),QTOS(n)); MPZTOZ(z,*nr);
  } else {
    MPZTOMPQ(BDY(n1),q); MPQTOQ(q,r);
    pwrq(r,(Q)n,&p); *nr = (Z)p;
  }
}

int cmpz(Z q1,Z q2)
{
  int sgn;

  if ( !q1 ) {
    if ( !q2 ) 
      return 0;
    else
      return -mpz_sgn(BDY(q2));
  } else if ( !q2 ) 
    return mpz_sgn(BDY(q1));
  else if ( !q1->z || !q2->z )
    error("mpqz : invalid argument");
  else if ( (sgn = mpz_sgn(BDY(q1))) != mpz_sgn(BDY(q2)) ) 
      return sgn;
  else {
    sgn = mpz_cmp(BDY(q1),BDY(q2));
    if ( sgn > 0 ) return 1;
    else if ( sgn < 0 ) return -1;
    else return 0;
  }
}

void gcdz(Z n1,Z n2,Z *nq)
{
  mpz_t t;

  if ( !n1 ) *nq = n2;
  else if ( !n2 ) *nq = n1;
  else if ( !n1->z || !n2->z )
    error("gcdz : invalid argument");
  else {
    mpz_init(t); mpz_gcd(t,BDY(n1),BDY(n2));
    MPZTOZ(t,*nq);
  }
}

void invz(Z n1,Z n2,Z *nq)
{
  mpz_t t;

  if ( !n1 || !n2 || !n1->z || !n2->z )
    error("invz : invalid argument");
  mpz_init(t); mpz_invert(t,BDY(n1),BDY(n2));
  MPZTOZ(t,*nq);
}

void lcmz(Z n1,Z n2,Z *nq)
{
  Z g,t;

  if ( !n1 || !n2 ) *nq = 0;
  else if ( !n1->z || !n2->z )
    error("lcmz : invalid argument");
  else {
    gcdz(n1,n2,&g); divsz(n1,g,&t);
    mulz(n2,t,nq);
  }
}

void gcdvz(VECT v,Z *q)
{
  int n,i;
  Z *b;
  Z g,g1;

  n = v->len;
  b = (Z *)v->body;
  g = b[0];
  for ( i = 1; i < n; i++ ) {
    gcdz(g,b[i],&g1); g = g1;
  }
  *q = g;
}

void gcdvz_estimate(VECT v,Z *q)
{
  int n,m,i;
  Z s,t,u;
  Z *b;

  n = v->len;
  b = (Z *)v->body;
  if ( n == 1 ) {
    if ( mpz_sgn(BDY(b[0]))<0 ) chsgnz(b[0],q);
    else *q = b[0];
  }
  m = n/2;
  for ( i = 0, s = 0; i < m; i++ ) {
    if ( b[i] && mpz_sgn(BDY(b[i]))<0 ) subz(s,b[i],&u);
    else addz(s,b[i],&u);
    s = u;
  }
  for ( i = 0, t = 0; i < n; i++ ) {
    if ( b[i] && mpz_sgn(BDY(b[i]))<0 ) subz(t,b[i],&u);
    else addz(t,b[i],&u);
    t = u;
  }
  gcdz(s,t,q);
}

void factorialz(unsigned int n,Z *nr)
{
  mpz_t a;
  mpz_init(a);
  mpz_fac_ui(a,n);
  MPZTOZ(a,*nr);
}

void randomz(int blen,Z *nr)
{
  mpz_t z;

  mpz_init(z);
  mpz_urandomb(z,GMP_RAND,blen);
  MPZTOZ(z,*nr);
}

int tstbitz(Z n,int k)
{
   if ( !n || !n->z )
    error("tstbitz : invalid argument");
   return !n ? 0 : mpz_tstbit(BDY(n),k); 
}

void addq(Q n1,Q n2,Q *nr)
{
  mpq_t q1,q2,t;

  if ( !n1 ) *nr = n2;
  else if ( !n2 ) *nr = n1;
  else if ( n1->z && n2->z )
    addz((Z)n1,(Z)n2,(Z *)nr);
  else {
    if ( n1->z ) MPZTOMPQ(BDY((Z)n1),q1);
    else q1[0] = BDY(n1)[0];
    if ( n2->z ) MPZTOMPQ(BDY((Z)n2),q2);
    else q2[0] = BDY(n2)[0];
    mpq_init(t); mpq_add(t,q1,q2); *nr = mpqtozq(t);
  }
}

void subq(Q n1,Q n2,Q *nr)
{
  mpq_t q1,q2,t;

  if ( !n1 ) {
    if ( !n2 ) *nr = 0;
    else if ( n1->z ) chsgnz((Z)n1,(Z *)nr);
    else {
        mpq_init(t); mpq_neg(t,BDY(n2)); MPQTOQ(t,*nr);
      }
  } else if ( !n2 ) *nr = n1;
  else if ( n1 == n2 ) *nr = 0;
  else if ( n1->z && n2->z )
    subz((Z)n1,(Z)n2,(Z *)nr);
  else {
    if ( n1->z ) MPZTOMPQ(BDY((Z)n1),q1);
    else q1[0] = BDY(n1)[0];
    if ( n2->z ) MPZTOMPQ(BDY((Z)n2),q2);
    else q2[0] = BDY(n2)[0];
    mpq_init(t); mpq_sub(t,q1,q2); *nr = mpqtozq(t);
  }
}

void mulq(Q n1,Q n2,Q *nr)
{
  mpq_t t,q1,q2;

  if ( !n1 || !n2 ) *nr = 0;
  else if ( n1->z && n2->z )
    mulz((Z)n1,(Z)n2,(Z *)nr);
  else {
    if ( n1->z ) MPZTOMPQ(BDY((Z)n1),q1);
    else q1[0] = BDY(n1)[0];
    if ( n2->z ) MPZTOMPQ(BDY((Z)n2),q2);
    else q2[0] = BDY(n2)[0];
    mpq_init(t); mpq_mul(t,q1,q2); *nr = mpqtozq(t);
  }
}

void divq(Q n1,Q n2,Q *nq)
{
  mpq_t t,q1,q2;

  if ( !n2 ) {
    error("division by 0");
    *nq = 0;
    return;
  } else if ( !n1 ) *nq = 0;
  else if ( n1 == n2 ) *nq = (Q)ONE;
  else {
    if ( n1->z ) MPZTOMPQ(BDY((Z)n1),q1);
    else q1[0] = BDY(n1)[0];
    if ( n2->z ) MPZTOMPQ(BDY((Z)n2),q2);
    else q2[0] = BDY(n2)[0];
    mpq_init(t); mpq_div(t,q1,q2); *nq = mpqtozq(t);
  }
}

void invq(Q n,Q *nr)
{
  Z nm,dn;

  if ( INT(n) )
    divq((Q)ONE,n,nr);
  else {
    nmq(n,&nm);
    dnq(n,&dn);
    divq((Q)dn,(Q)nm,nr);
  }
}

void chsgnq(Q n,Q *nr)
{
  mpq_t t;

  if ( !n ) *nr = 0;
  else if (n->z ) chsgnz((Z)n,(Z *)nr);
  else {
    mpq_init(t); mpq_neg(t,BDY(n)); MPQTOQ(t,*nr);
  }
}

void absq(Q n,Q *nr)
{
  if ( !n ) *nr = 0;
  else if ( n->z ) absz((Z)n,(Z *)nr);
  else if ( sgnq(n) < 0 ) chsgnq(n,nr);
  else *nr = n;
}

void pwrq(Q n1,Q n,Q *nr)
{
  int e;
  mpz_t nm,dn;
  mpq_t t;

  if ( !n || UNIQ((Z)n1) || UNIQ(n1) ) *nr = (Q)ONE;
  else if ( !n1 ) *nr = 0;
  else if ( !INT(n) ) {
    error("can't calculate fractional power."); *nr = 0;
  } else if ( !smallz((Z)n) ) {
    error("exponent too big."); *nr = 0;
  } else {
    e = QTOS(n);
    if ( e < 0 ) {
      e = -e;
      if ( n1->z ) {
        nm[0] = ONEMPZ[0];
        dn[0] = BDY((Z)n1)[0];
      } else {
        nm[0] = mpq_denref(BDY(n1))[0];
        dn[0] = mpq_numref(BDY(n1))[0];
      }
    } else {
      if ( n1->z ) {
        nm[0] = BDY((Z)n1)[0];
        dn[0] = ONEMPZ[0];
      } else {
        nm[0] = mpq_numref(BDY(n1))[0];
        dn[0] = mpq_denref(BDY(n1))[0];
      }
    }
    mpq_init(t);
    mpz_pow_ui(mpq_numref(t),nm,e); mpz_pow_ui(mpq_denref(t),dn,e);
    *nr = mpqtozq(t);
  }
}

int cmpq(Q n1,Q n2)
{
  mpq_t q1,q2;
  int sgn;

  if ( !n1 ) {
    if ( !n2 ) return 0;
    else return (n2->z) ? -mpz_sgn(BDY((Z)n2)) : -mpq_sgn(BDY(n2));
  } if ( !n2 ) return (n1->z) ? mpz_sgn(BDY((Z)n1)) : mpq_sgn(BDY(n1));
  else if ( n1->z && n2->z )
    return cmpz((Z)n1,(Z)n2);
  else if ( (sgn = mpq_sgn(BDY(n1))) != mpq_sgn(BDY(n2)) ) return sgn;
  else {
    if ( n1->z ) MPZTOMPQ(BDY((Z)n1),q1);
    else q1[0] = BDY(n1)[0];
    if ( n2->z ) MPZTOMPQ(BDY((Z)n2),q2);
    else q2[0] = BDY(n2)[0];
    sgn = mpq_cmp(q1,q2);
    if ( sgn > 0 ) return 1;
    else if ( sgn < 0 ) return -1;
    else return 0;
  }
}

/* t = [nC0 nC1 ... nCn] */

void mkbc(int n,Z *t)
{
  int i;
  Z c,d,iq;

  for ( t[0] = ONE, i = 1; i <= n/2; i++ ) {
    STOQ(n-i+1,c); mulz(t[i-1],c,&d); 
    STOQ(i,iq); divsz(d,iq,&t[i]);
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

/* mod m table */
/* XXX : should be optimized */

void mkwcm(int k,int l,int m,int *t)
{
  int i,n;
  Z *s;

  n = MIN(k,l);
  s = (Z *)ALLOCA((n+1)*sizeof(Q));
  mkwc(k,l,s);
  for ( i = 0; i <= n; i++ ) {
    t[i] = remqi((Q)s[i],m);
  }
}

void mkwc(int k,int l,Z *t)
{
  mpz_t a,b,q,nm,z,u;
  int i,n;

  n = MIN(k,l);
  mpz_init_set_ui(z,1);
  mpz_init(u); mpz_set(u,z); MPZTOZ(u,t[0]);
  mpz_init(a); mpz_init(b); mpz_init(nm);
  for ( i = 1; i <= n; i++ ) {
    mpz_set_ui(a,k-i+1); mpz_set_ui(b,l-i+1); mpz_mul(nm,a,b);
    mpz_mul(z,BDY(t[i-1]),nm); mpz_fdiv_q_ui(z,z,i);
    mpz_init(u); mpz_set(u,z); MPZTOZ(u,t[i]);
  }
}

void lgp(P p,Z *g,Z *l);

void ptozp(P p,int sgn,Q *c,P *pr)
{
  Z nm,dn;

  if ( !p ) {
    *c = 0; *pr = 0;
  } else {
    lgp(p,&nm,&dn);
    divz(nm,dn,(Z *)c);
    divsp(CO,p,(P)*c,pr);
  }
}

void lgp(P p,Z *g,Z *l)
{
  DCP dc;
  Z g1,g2,l1,l2,l3,l4;

  if ( NUM(p) ) {
    if ( ((Q)p)->z ) {
      MPZTOZ(BDY((Z)p),*g);
      *l = ONE;
    } else {
      MPZTOZ(mpq_numref(BDY((Q)p)),*g);
      MPZTOZ(mpq_denref(BDY((Q)p)),*l);
    }
  } else {
    dc = DC(p); lgp(COEF(dc),g,l);
    for ( dc = NEXT(dc); dc; dc = NEXT(dc) ) {
      lgp(COEF(dc),&g1,&l1); gcdz(*g,g1,&g2); *g = g2;
      gcdz(*l,l1,&l2); mulz(*l,l1,&l3); divz(l3,l2,l);
    }
  } 
}

void qltozl(Q *w,int n,Z *dvr)
{
  Z nm,dn;
  Z g,g1,l1,l2,l3;
  Q c;
  int i;
  struct oVECT v;

  for ( i = 0; i < n; i++ )
    if ( w[i] && !w[i]->z )
      break;
  if ( i == n ) {
    v.id = O_VECT; v.len = n; v.body = (pointer *)w;
    gcdvz(&v,dvr); return;
  }
  for ( i = 0; !w[i]; i++ );
  c = w[i]; 
  if ( !c->z ) {
    MPZTOZ(mpq_numref(BDY(c)),nm); MPZTOZ(mpq_denref(BDY(c)),dn);
  } else {
    MPZTOZ(BDY((Z)c),nm); dn = ONE;
  }
  for ( i++; i < n; i++ ) {
    c = w[i]; 
    if ( !c ) continue;
    if ( !c->z ) {
      MPZTOZ(mpq_numref(BDY(c)),g1); MPZTOZ(mpq_denref(BDY(c)),l1);
    } else {
      MPZTOZ(BDY((Z)c),g1); l1 = ONE;
    }
    gcdz(nm,g1,&g); nm = g;
    gcdz(dn,l1,&l2); mulz(dn,l1,&l3); divz(l3,l2,&dn);
  }
  divz(nm,dn,dvr);
}

int z_bits(Q q)
{
  if ( !q ) return 0;
  else if ( q->z ) return mpz_sizeinbase(BDY((Z)q),2);
  else
    return mpz_sizeinbase(mpq_numref(BDY(q)),2)
      + mpz_sizeinbase(mpq_denref(BDY(q)),2);
}

int zp_mag(P p)
{
  int s;
  DCP dc;
  
  if ( !p ) return 0;
  else if ( OID(p) == O_N ) return z_bits((Q)p);
  else {
    for ( dc = DC(p), s = 0; dc; dc = NEXT(dc) ) s += zp_mag(COEF(dc));
    return s;
  }
}

void makesubstz(VL v,NODE *s)
{
  NODE r,r0;
  Z q;  
  unsigned int n;

  for ( r0 = 0; v; v = NEXT(v) ) {
    NEXTNODE(r0,r); BDY(r) = (pointer)v->v;
#if defined(_PA_RISC1_1)
    n = mrand48()&BMASK; q = utoz(n);
#else
    n = random(); q = utoz(n);
#endif
    NEXTNODE(r0,r); BDY(r) = (pointer)q;
  }
  if ( r0 ) NEXT(r) = 0;
  *s = r0;
}

unsigned int remqi(Q a,unsigned int mod)
{
  unsigned int c,nm,dn;
  mpz_t r;

  if ( !a ) return 0;
  else if ( a->z ) {
    mpz_init(r);
    c = mpz_fdiv_r_ui(r,BDY((Z)a),mod);
  } else {
    mpz_init(r);
    nm = mpz_fdiv_r_ui(r,mpq_numref(BDY(a)),mod);
    dn = mpz_fdiv_r_ui(r,mpq_denref(BDY(a)),mod);
    dn = invm(dn,mod);    
    DMAR(nm,dn,0,mod,c);
  }
  return c;
}

extern int DP_Print;

#define F4_INTRAT_PERIOD 8 

int generic_gauss_elim(MAT mat,MAT *nm,Z *dn,int **rindp,int **cindp)
{
  int **wmat;
  Z **bmat,**tmat,*bmi,*tmi;
  Z q,m1,m2,m3,s,u;
  int *wmi,*colstat,*wcolstat,*rind,*cind;
  int row,col,ind,md,i,j,k,l,t,t1,rank,rank0,inv;
  MAT r,crmat;
  int ret;

  bmat = (Z **)mat->body;
  row = mat->row; col = mat->col;
  wmat = (int **)almat(row,col);
  colstat = (int *)MALLOC_ATOMIC(col*sizeof(int));
  wcolstat = (int *)MALLOC_ATOMIC(col*sizeof(int));
  for ( ind = 0; ; ind++ ) {
    if ( DP_Print ) {
      fprintf(asir_out,"."); fflush(asir_out);
    }
    md = get_lprime(ind);
    for ( i = 0; i < row; i++ )
      for ( j = 0, bmi = bmat[i], wmi = wmat[i]; j < col; j++ )
        wmi[j] = remqi((Q)bmi[j],md);
    rank = generic_gauss_elim_mod(wmat,row,col,md,wcolstat);
    if ( !ind ) {
RESET:
      m1 = utoz(md);
      rank0 = rank;
      bcopy(wcolstat,colstat,col*sizeof(int));
      MKMAT(crmat,rank,col-rank);
      MKMAT(r,rank,col-rank); *nm = r;
      tmat = (Z **)crmat->body;
      for ( i = 0; i < rank; i++ )
        for ( j = k = 0, tmi = tmat[i], wmi = wmat[i]; j < col; j++ )
          if ( !colstat[j] ) tmi[k++] = utoz(wmi[j]);
    } else {
      if ( rank < rank0 ) {
        if ( DP_Print ) {
          fprintf(asir_out,"lower rank matrix; continuing...\n");
          fflush(asir_out);
        }
        continue;
      } else if ( rank > rank0 ) {
        if ( DP_Print ) {
          fprintf(asir_out,"higher rank matrix; resetting...\n");
          fflush(asir_out);
        }
        goto RESET;
      } else {
        for ( j = 0; (j<col) && (colstat[j]==wcolstat[j]); j++ );
        if ( j < col ) {
          if ( DP_Print ) {
            fprintf(asir_out,"inconsitent colstat; resetting...\n");
            fflush(asir_out);
          }
          goto RESET;
        }
      }

      inv = invm(remqi((Q)m1,md),md);
      m2 = utoz(md); mulz(m1,m2,&m3);
      for ( i = 0; i < rank; i++ )      
        for ( j = k = 0, tmi = tmat[i], wmi = wmat[i]; j < col; j++ )
          if ( !colstat[j] ) {
            if ( tmi[k] ) {
            /* f3 = f1+m1*(m1 mod m2)^(-1)*(f2 - f1 mod m2) */
              t = remqi((Q)tmi[k],md);
              if ( wmi[j] >= t ) t = wmi[j]-t;
              else t = md-(t-wmi[j]);
              DMAR(t,inv,0,md,t1) 
              u = utoz(t1); mulz(m1,u,&s); 
              addz(tmi[k],s,&u); tmi[k] = u;
            } else if ( wmi[j] ) {
            /* f3 = m1*(m1 mod m2)^(-1)*f2 */
              DMAR(wmi[j],inv,0,md,t)
              u = utoz(t); mulz(m1,u,&s); tmi[k] = s;
            }
            k++;
          }
      m1 = m3;
      if ( ind % F4_INTRAT_PERIOD ) 
        ret = 0;
      else 
        ret = intmtoratm(crmat,m1,*nm,dn);
      if ( ret ) {
        *rindp = rind = (int *)MALLOC_ATOMIC(rank*sizeof(int));
        *cindp = cind = (int *)MALLOC_ATOMIC((col-rank)*sizeof(int));
        for ( j = k = l = 0; j < col; j++ )
          if ( colstat[j] ) rind[k++] = j;  
          else cind[l++] = j;
        if ( gensolve_check(mat,*nm,*dn,rind,cind) )
          return rank;
      }
    }
  }
}

int generic_gauss_elim2(MAT mat,MAT *nm,Z *dn,int **rindp,int **cindp)
{

  MAT full;
  Z **bmat,**b;
  Z *bmi;
  Z dn0;
  int row,col,md,i,j,rank,ret;
  int **wmat;
  int *wmi;
  int *colstat,*rowstat;

  bmat = (Z **)mat->body;
  row = mat->row; col = mat->col;
  wmat = (int **)almat(row,col);
  colstat = (int *)MALLOC_ATOMIC(col*sizeof(int));
  rowstat = (int *)MALLOC_ATOMIC(row*sizeof(int));
  /* XXX */
  md = get_lprime(0);
  for ( i = 0; i < row; i++ )
    for ( j = 0, bmi = bmat[i], wmi = wmat[i]; j < col; j++ )
      wmi[j] = remqi((Q)bmi[j],md);
  rank = generic_gauss_elim_mod2(wmat,row,col,md,colstat,rowstat);
  b = (Z **)MALLOC(rank*sizeof(Z));
  for ( i = 0; i < rank; i++ ) b[i] = bmat[rowstat[i]];
  NEWMAT(full); full->row = rank; full->col = col; full->body = (pointer **)b;
  ret = generic_gauss_elim_full(full,nm,dn,rindp,cindp);
  if ( !ret ) {
    rank = generic_gauss_elim(mat,nm,&dn0,rindp,cindp);
    for ( i = 0; i < rank; i++ ) dn[i] = dn0;
  }
  return rank;
}

int generic_gauss_elim_full(MAT mat,MAT *nm,Z *dn,int **rindp,int **cindp)
{
  int **wmat;
  int *stat;
  Z **bmat,**tmat,*bmi,*tmi,*ri;
  Z q,m1,m2,m3,s,u;
  int *wmi,*colstat,*wcolstat,*rind,*cind;
  int row,col,ind,md,i,j,k,l,t,t1,rank,rank0,inv,h;
  MAT r,crmat;
  int ret,initialized,done;

  initialized = 0;
  bmat = (Z **)mat->body;
  row = mat->row; col = mat->col;
  wmat = (int **)almat(row,col);
  stat = (int *)MALLOC_ATOMIC(row*sizeof(int));
  for ( i = 0; i < row; i++ ) stat[i] = 0;
  colstat = (int *)MALLOC_ATOMIC(col*sizeof(int));
  wcolstat = (int *)MALLOC_ATOMIC(col*sizeof(int));
  for ( ind = 0; ; ind++ ) {
    if ( DP_Print ) {
      fprintf(asir_out,"."); fflush(asir_out);
    }
    md = get_lprime(ind);
    for ( i = 0; i < row; i++ )
      for ( j = 0, bmi = bmat[i], wmi = wmat[i]; j < col; j++ )
        wmi[j] = remqi((Q)bmi[j],md);
    rank = generic_gauss_elim_mod(wmat,row,col,md,wcolstat);
    if ( rank < row ) continue;
    if ( !initialized ) {
      m1 = utoz(md);
      bcopy(wcolstat,colstat,col*sizeof(int));
      MKMAT(crmat,row,col-row);
      MKMAT(r,row,col-row); *nm = r;
      tmat = (Z **)crmat->body;
      for ( i = 0; i < row; i++ )
        for ( j = k = 0, tmi = tmat[i], wmi = wmat[i]; j < col; j++ )
          if ( !colstat[j] ) tmi[k++] = utoz(wmi[j]);
      initialized = 1;
    } else {
      for ( j = 0; (j<col) && (colstat[j]==wcolstat[j]); j++ );
      if ( j < col ) continue;

      inv = invm(remqi((Q)m1,md),md);
      m2 = utoz(md); mulz(m1,m2,&m3);
      for ( i = 0; i < row; i++ )
        switch ( stat[i] ) {
          case 1: 
            /* consistency check */
            ri = (Z *)BDY(r)[i]; wmi = wmat[i];
            for ( j = 0; j < col; j++ ) if ( colstat[j] ) break;
            h = md-remqi((Q)dn[i],md);
            for ( j++, k = 0; j < col; j++ )
              if ( !colstat[j] ) {
                t = remqi((Q)ri[k],md);
                DMAR(wmi[i],h,t,md,t1);
                if ( t1 ) break;
              }
            if ( j == col ) { stat[i]++; break; }
            else {
              /* fall to the case 0 */
              stat[i] = 0;
            }
          case 0:
            tmi = tmat[i]; wmi = wmat[i];
            for ( j = k = 0; j < col; j++ )
              if ( !colstat[j] ) {
                if ( tmi[k] ) {
                /* f3 = f1+m1*(m1 mod m2)^(-1)*(f2 - f1 mod m2) */
                  t = remqi((Q)tmi[k],md);
                  if ( wmi[j] >= t ) t = wmi[j]-t;
                  else t = md-(t-wmi[j]);
                  DMAR(t,inv,0,md,t1) 
                  u = utoz(t1); mulz(m1,u,&s); 
                  addz(tmi[k],s,&u); tmi[k] = u;
                } else if ( wmi[j] ) {
                /* f3 = m1*(m1 mod m2)^(-1)*f2 */
                  DMAR(wmi[j],inv,0,md,t)
                  u = utoz(t); mulz(m1,u,&s); tmi[k] = s;
                }
                k++;
              }
            break;
          case 2: default:
            break;
        }
      m1 = m3;
      if ( ind % 4 )
        ret = 0;
      else 
        ret = intmtoratm2(crmat,m1,*nm,dn,stat);
      if ( ret ) {
        *rindp = rind = (int *)MALLOC_ATOMIC(row*sizeof(int));
        *cindp = cind = (int *)MALLOC_ATOMIC((col-row)*sizeof(int));
        for ( j = k = l = 0; j < col; j++ )
          if ( colstat[j] ) rind[k++] = j;  
          else cind[l++] = j;
        return gensolve_check2(mat,*nm,dn,rind,cind);
      }
    }
  }
}

int generic_gauss_elim_direct(MAT mat,MAT *nm,Z *dn,int **rindp,int **cindp){
  Z **bmat,*s;
  Z u,v,w,x,d,t,y;
  int row,col,i,j,k,l,m,rank;
  int *colstat,*colpos,*cind;
  MAT r,in;

  row = mat->row; col = mat->col;
  MKMAT(in,row,col);
  for ( i = 0; i < row; i++ )
    for ( j = 0; j < col; j++ ) in->body[i][j] = mat->body[i][j];
  bmat = (Z **)in->body;
  colstat = (int *)MALLOC_ATOMIC(col*sizeof(int));
  *rindp = colpos = (int *)MALLOC_ATOMIC(row*sizeof(int));
  for ( j = 0, rank = 0, d = ONE; j < col; j++ ) {
    for ( i = rank; i < row && !bmat[i][j]; i++  );
    if ( i == row ) { colstat[j] = 0; continue; }
    else { colstat[j] = 1; colpos[rank] = j; }
    if ( i != rank )
      for ( k = j; k < col; k++ ) {
        t = bmat[i][k]; bmat[i][k] = bmat[rank][k]; bmat[rank][k] = t;
      }
    for ( i = rank+1, v = bmat[rank][j]; i < row; i++ )
      for ( k = j, u = bmat[i][j]; k < col; k++ ) {
        mulz(bmat[i][k],v,&w); mulz(bmat[rank][k],u,&x);
        subz(w,x,&y); divsz(y,d,&bmat[i][k]);
      }
    d = v; rank++;
  }
  *dn = d;
  s = (Z *)MALLOC(col*sizeof(Z));
  for ( i = rank-1; i >= 0; i-- ) {
    for ( k = colpos[i]; k < col; k++ ) mulz(bmat[i][k],d,&s[k]); 
    for ( m = rank-1; m > i; m-- ) {
      for ( k = colpos[m], u = bmat[i][k]; k < col; k++ ) {
        mulz(bmat[m][k],u,&w); subz(s[k],w,&x); s[k] = x;
      }
    }
    for ( k = colpos[i], u = bmat[i][k]; k < col; k++ )
      divz(s[k],u,&bmat[i][k]);
  }
  *cindp = cind = (int *)MALLOC_ATOMIC((col-rank)*sizeof(int));
  MKMAT(r,rank,col-rank); *nm = r;
  for ( j = 0, k = 0; j < col; j++ )
    if ( !colstat[j] ) {
      cind[k] = j;
      for ( i = 0; i < rank; i++ ) r->body[i][k] = bmat[i][j];
      k++;
    }
  return rank;
}

int intmtoratm(MAT mat,Z md,MAT nm,Z *dn)
{
  Z t,s,b,dn0,dn1,nm1,q,u,unm,udn,dmy;
  int i,j,k,l,row,col,sgn,ret;
  Z **rmat,**tmat,*tmi,*nmk;

  if ( UNIQ(md) )
    return 0;
  row = mat->row; col = mat->col;
  bshiftz(md,1,&t);
  isqrtz(t,&s);
  bshiftz(s,64,&b);
  if ( !b ) b = ONE;
  dn0 = ONE;
  tmat = (Z **)mat->body;
  rmat = (Z **)nm->body;
  for ( i = 0; i < row; i++ )
    for ( j = 0, tmi = tmat[i]; j < col; j++ )
      if ( tmi[j] ) {
        mulz(tmi[j],dn0,&s);
        divqrz(s,md,&dmy,&u);
        ret = inttorat(u,md,b,&nm1,&dn1);
        if ( !ret ) return 0;
        else {
          if ( !UNIQ(dn1) ) {
            for ( k = 0; k < i; k++ )
              for ( l = 0, nmk = rmat[k]; l < col; l++ ) {
                mulz(nmk[l],dn1,&q); nmk[l] = q;
              }
            for ( l = 0, nmk = rmat[i]; l < j; l++ ) {
              mulz(nmk[l],dn1,&q); nmk[l] = q;
            }
          }
          rmat[i][j] = nm1;
          mulz(dn0,dn1,&q); dn0 = q;
        }
      }
  *dn = dn0;
  return 1;
}

int intmtoratm2(MAT mat,Z md,MAT nm,Z *dn,int *stat)
{
  int row,col,i,j,ret;
  Z dn0,dn1,t,s,b;
  Z *w,*tmi;
  Z **tmat;

  bshiftz(md,1,&t);
  isqrtz(t,&s);
  bshiftz(s,64,&b);
  tmat = (Z **)mat->body;
  if ( UNIQ(md) ) return 0;
  row = mat->row; col = mat->col;
  dn0 = ONE;
  for ( i = 0; i < row; i++ )
    if ( cmpz(dn[i],dn0) > 0 ) dn0 = dn[i];
  w = (Z *)MALLOC(col*sizeof(Z));
  for ( i = 0; i < row; i++ )
    if ( stat[i] == 0 ) {
      for ( j = 0, tmi = tmat[i]; j < col; j++ )
          mulz(tmi[j],dn0,&w[j]);
      ret = intvtoratv(w,col,md,b,(Z *)BDY(nm)[i],&dn[i]);
      if ( ret ) {
        stat[i] = 1;
        mulz(dn0,dn[i],&t); dn[i] = t; dn0 = t;
      }
    }
  for ( i = 0; i < row; i++ ) if ( !stat[i] ) break;
  if ( i == row ) return 1;
  else return 0;
}

int intvtoratv(Z *v,int n,Z md,Z b,Z *nm,Z *dn)
{
  Z dn0,dn1,q,s,u,nm1,unm,udn,dmy;
  Z *nmk;
  int j,l,col,ret,sgn;

  for ( j = 0; j < n; j++ ) nm[j] = 0;
  dn0 = ONE;
  for ( j = 0; j < n; j++ ) {
    if ( !v[j] ) continue;
    mulz(v[j],dn0,&s);
    divqrz(s,md,&dmy,&u);
    ret = inttorat(u,md,b,&nm1,&dn1);
    if ( !ret ) return 0;
    if ( !UNIQ(dn1) )
      for ( l = 0; l < j; l++ ) {
        mulz(nm[l],dn1,&q); nm[l] = q;
      }
    nm[j] = nm1;
    mulz(dn0,dn1,&q); dn0 = q;
  }
  *dn = dn0;
  return 1;
}

/* assuming 0 < c < m */

int inttorat(Z c,Z m,Z b,Z *nmp,Z *dnp)
{
  Z qq,t,u1,v1,r1;
  Z q,u2,v2,r2;

  u1 = 0; v1 = ONE; u2 = m; v2 = c;
  while ( cmpz(v2,b) >= 0 ) {
    divqrz(u2,v2,&q,&r2); u2 = v2; v2 = r2;
    mulz(q,v1,&t); subz(u1,t,&r1); u1 = v1; v1 = r1; 
  }
  if ( cmpz(v1,b) >= 0 ) return 0;
  else {
    if ( mpz_sgn(BDY(v1))<0  ) {
      chsgnz(v1,dnp); chsgnz(v2,nmp);
    } else {
      *dnp = v1; *nmp = v2;
    }
    return 1;
  }
}

extern int f4_nocheck;

int gensolve_check(MAT mat,MAT nm,Z dn,int *rind,int *cind)
{
  int row,col,rank,clen,i,j,k,l;
  Z s,t;
  Z *w;
  Z *mati,*nmk;

  if ( f4_nocheck ) return 1;
  row = mat->row; col = mat->col; rank = nm->row; clen = nm->col;  
  w = (Z *)MALLOC(clen*sizeof(Z));
  for ( i = 0; i < row; i++ ) {
    mati = (Z *)mat->body[i];
    bzero(w,clen*sizeof(Z));
    for ( k = 0; k < rank; k++ )
      for ( l = 0, nmk = (Z *)nm->body[k]; l < clen; l++ ) {
        mulz(mati[rind[k]],nmk[l],&t); addz(w[l],t,&s); w[l] = s;
      }
    for ( j = 0; j < clen; j++ ) {
      mulz(dn,mati[cind[j]],&t);
      if ( cmpz(w[j],t) ) break;
    }
    if ( j != clen ) break;
  }
  if ( i != row ) return 0;
  else return 1;
}

int gensolve_check2(MAT mat,MAT nm,Z *dn,int *rind,int *cind)
{
  int row,col,rank,clen,i,j,k,l;
  Z s,t,u,d;
  Z *w,*m;
  Z *mati,*nmk;

  if ( f4_nocheck ) return 1;
  row = mat->row; col = mat->col; rank = nm->row; clen = nm->col;  
  w = (Z *)MALLOC(clen*sizeof(Z));
  m = (Z *)MALLOC(clen*sizeof(Z));
  for ( d = dn[0], i = 1; i < rank; i++ ) {
    lcmz(d,dn[i],&t); d = t;
  }
  for ( i = 0; i < rank; i++ ) divsz(d,dn[i],&m[i]);
  for ( i = 0; i < row; i++ ) {
    mati = (Z *)mat->body[i];
    bzero(w,clen*sizeof(Z));
    for ( k = 0; k < rank; k++ ) {
      mulz(mati[rind[k]],m[k],&u);
      for ( l = 0, nmk = (Z *)nm->body[k]; l < clen; l++ ) {
        mulz(u,nmk[l],&t); addz(w[l],t,&s); w[l] = s;
      }
    }
    for ( j = 0; j < clen; j++ ) {
      mulz(d,mati[cind[j]],&t);
      if ( cmpz(w[j],t) ) break;
    }
    if ( j != clen ) break;
  }
  if ( i != row ) return 0;
  else return 1;
}

void isqrtz(Z a,Z *r)
{
  int k;
  Z x,t,x2,xh,quo,rem;
  Z two;

  if ( !a ) *r = 0;
  else if ( UNIQ(a) ) *r = ONE;
  else {
    k = z_bits((Q)a); /* a <= 2^k-1 */
    bshiftz(ONE,-((k>>1)+(k&1)),&x); /* a <= x^2 */
    STOQ(2,two);
    while ( 1 ) {
      pwrz(x,two,&t);
      if ( cmpz(t,a) <= 0 ) {
        *r = x; return;
      } else {
        if ( mpz_tstbit(BDY(x),0) ) addz(x,a,&t);
        else t = a;
        bshiftz(x,-1,&x2); divqrz(t,x2,&quo,&rem);
        bshiftz(x,1,&xh); addz(quo,xh,&x);
      }
    }
  }
}

void bshiftz(Z a,int n,Z *r)
{
  mpz_t t;

  if ( !a ) *r = 0;
  else if ( n == 0 ) *r = a;
  else if ( n < 0 ) {
    mpz_init(t); mpz_mul_2exp(t,BDY(a),-n); MPZTOZ(t,*r);
  } else { 
    mpz_init(t); mpz_fdiv_q_2exp(t,BDY(a),n);
    if ( !mpz_sgn(t) ) *r = 0;
    else MPZTOZ(t,*r);
  }
}

void addlf(Z a,Z b,Z *c)
{
  addz(a,b,c);
  if ( !lf_lazy ) {
    if ( cmpz(*c,current_mod_lf) >= 0 ) {
      subz(*c,current_mod_lf,c);
    }
  }
}

void sublf(Z a,Z b,Z *c)
{
  subz(a,b,c);
  if ( !lf_lazy ) {
    remz(*c,current_mod_lf,c);
  }
}

void mullf(Z a,Z b,Z *c)
{
  mulz(a,b,c);
  if ( !lf_lazy ) {
    remz(*c,current_mod_lf,c);
  }
}

void divlf(Z a,Z b,Z *c)
{
  Z inv;

  invz(b,current_mod_lf,&inv);
  mulz(a,inv,c);
  if ( !lf_lazy ) {
    remz(*c,current_mod_lf,c);
  }
}

void chsgnlf(Z a,Z *c)
{
  chsgnz(a,c);
  if ( !lf_lazy ) {
    remz(*c,current_mod_lf,c);
  }
}

void lmtolf(LM a,Z *b)
{
  if ( !a ) *b = 0;
  else {
    MPZTOZ(BDY(a),*b);
  }
}

void setmod_lf(Z p)
{
    current_mod_lf = p;
    current_mod_lf_size = mpz_size(BDY(current_mod_lf))+1;
}

void simplf_force(Z a,Z *b)
{
    remz(a,current_mod_lf,b);
}

int generic_gauss_elim_hensel(MAT mat,MAT *nmmat,Z *dn,int **rindp,int **cindp)
{
  MAT bmat,xmat;
  Z **a0,**a,**b,**x,**nm;
  Z *ai,*bi,*xi;
  int row,col;
  int **w;
  int *wi;
  int **wc;
  Z mdq,q,s,u;
  Z tn;
  int ind,md,i,j,k,l,li,ri,rank;
  unsigned int t;
  int *cinfo,*rinfo;
  int *rind,*cind;
  int count;
  int ret;
  struct oEGT eg_mul,eg_inv,eg_intrat,eg_check,tmp0,tmp1;
  int period;
  int *wx,*ptr;
  int wxsize,nsize;
  Z wn;
  Z wq;

  a0 = (Z **)mat->body;
  row = mat->row; col = mat->col;
  w = (int **)almat(row,col);
  for ( ind = 0; ; ind++ ) {
    md = get_lprime(ind);
    STOQ(md,mdq);
    for ( i = 0; i < row; i++ )
      for ( j = 0, ai = a0[i], wi = w[i]; j < col; j++ )
        wi[j] = remqi((Q)ai[j],md);

    if ( DP_Print > 3 ) {
      fprintf(asir_out,"LU decomposition.."); fflush(asir_out);
    }
    rank = find_lhs_and_lu_mod((unsigned int **)w,row,col,md,&rinfo,&cinfo);
    if ( DP_Print > 3 ) {
      fprintf(asir_out,"done.\n"); fflush(asir_out);
    }
    a = (Z **)almat_pointer(rank,rank); /* lhs mat */
    MKMAT(bmat,rank,col-rank); b = (Z **)bmat->body; /* lhs mat */
    for ( j = li = ri = 0; j < col; j++ )
      if ( cinfo[j] ) {
        /* the column is in lhs */
        for ( i = 0; i < rank; i++ ) {
          w[i][li] = w[i][j];
          a[i][li] = a0[rinfo[i]][j];
        }
        li++;
      } else {
        /* the column is in rhs */
        for ( i = 0; i < rank; i++ )
          b[i][ri] = a0[rinfo[i]][j];
        ri++;
      }

      /* solve Ax=B; A: rank x rank, B: rank x ri */
      /* algorithm
         c <- B
         x <- 0
         q <- 1
         do 
           t <- A^(-1)c mod p
           x <- x+qt
           c <- (c-At)/p
           q <- qp
         end do
         then Ax-B=0 mod q and b=(B-Ax)/q hold after "do".
      */
      MKMAT(xmat,rank,ri); x = (Z **)(xmat)->body;
      MKMAT(*nmmat,rank,ri); nm = (Z **)(*nmmat)->body;
      wc = (int **)almat(rank,ri);
      for ( i = 0; i < rank; i++ )
        wc[i] = w[i]+rank;
      *rindp = rind = (int *)MALLOC_ATOMIC(rank*sizeof(int));
      *cindp = cind = (int *)MALLOC_ATOMIC((ri)*sizeof(int));

      period = F4_INTRAT_PERIOD;
      for ( q = ONE, count = 0; ; ) {
        if ( DP_Print > 3 )
          fprintf(stderr,"o");
        /* wc = b mod md */
        for ( i = 0; i < rank; i++ )
          for ( j = 0, bi = b[i], wi = wc[i]; j < ri; j++ ) {
            wi[j] = remqi((Q)bi[j],md);
            if ( wi[j] && sgnz(bi[j]) < 0 )
              wi[j] = md-wi[j];
          }
        /* wc = A^(-1)wc; wc is normalized */
        solve_by_lu_mod(w,rank,md,wc,ri,1);
        /* x += q*wc */
        for ( i = 0; i < rank; i++ )
          for ( j = 0, wi = wc[i]; j < ri; j++ ) mul1addtoz(q,wi[j],&x[i][j]);
        /* b =(A*wc+b)/md */
        for ( i = 0; i < rank; i++ )
          for ( j = 0; j < ri; j++ ) {
            u = b[i][j];
            for ( k = 0; k < rank; k++ ) mul1addtoz(a[i][k],wc[k][j],&u);
            divsz(u,mdq,&b[i][j]);
          }
        count++;
        /* q = q*md */
        mulz(q,mdq,&u); q = u;
        if ( count == period ) {
          ret = intmtoratm(xmat,q,*nmmat,dn);
          if ( ret ) {
            for ( j = k = l = 0; j < col; j++ )
              if ( cinfo[j] )
                rind[k++] = j;  
              else
                cind[l++] = j;
            ret = gensolve_check(mat,*nmmat,*dn,rind,cind);
            if ( ret ) {
              *rindp = rind;
              *cindp = cind;
              for ( j = k = 0; j < col; j++ )
                if ( !cinfo[j] )
                  cind[k++] = j;
              return rank;
            }
          } else {
            period = period*3/2;
            count = 0;
          }
        }
      }
  }
}

/* for inv_or_split_dalg */

int generic_gauss_elim_hensel_dalg(MAT mat,DP *mb,MAT *nmmat,Z *dn,int **rindp,int **cindp)
{
  MAT bmat,xmat;
  Z **a0,**a,**b,**x,**nm;
  Z *ai,*bi,*xi;
  int row,col;
  int **w;
  int *wi;
  int **wc;
  Z mdq,q,s,u;
  Z tn;
  int ind,md,i,j,k,l,li,ri,rank;
  unsigned int t;
  int *cinfo,*rinfo;
  int *rind,*cind;
  int count;
  int ret;
  struct oEGT eg_mul,eg_inv,eg_intrat,eg_check,tmp0,tmp1;
  int period;
  int *wx,*ptr;
  int wxsize,nsize;
  Z wn;
  Z wq;
  DP m;

  a0 = (Z **)mat->body;
  row = mat->row; col = mat->col;
  w = (int **)almat(row,col);
  for ( ind = 0; ; ind++ ) {
    md = get_lprime(ind);
    STOQ(md,mdq);
    for ( i = 0; i < row; i++ )
      for ( j = 0, ai = a0[i], wi = w[i]; j < col; j++ )
        wi[j] = remqi((Q)ai[j],md);

    if ( DP_Print > 3 ) {
      fprintf(asir_out,"LU decomposition.."); fflush(asir_out);
    }
    rank = find_lhs_and_lu_mod((unsigned int **)w,row,col,md,&rinfo,&cinfo);
    if ( DP_Print > 3 ) {
      fprintf(asir_out,"done.\n"); fflush(asir_out);
    }

    /* this part is added for inv_or_split_dalg */
    for ( i = 0; i < col-1; i++ ) {
      if ( !cinfo[i] ) {
        m = mb[i];
        for ( j = i+1; j < col-1; j++ )
          if ( dp_redble(mb[j],m) )
            cinfo[j] = -1;
      }
    }

    a = (Z **)almat_pointer(rank,rank); /* lhs mat */
    MKMAT(bmat,rank,col-rank); b = (Z **)bmat->body; /* lhs mat */
    for ( j = li = ri = 0; j < col; j++ )
      if ( cinfo[j] ) {
        /* the column is in lhs */
        for ( i = 0; i < rank; i++ ) {
          w[i][li] = w[i][j];
          a[i][li] = a0[rinfo[i]][j];
        }
        li++;
      } else {
        /* the column is in rhs */
        for ( i = 0; i < rank; i++ )
          b[i][ri] = a0[rinfo[i]][j];
        ri++;
      }

      /* solve Ax=B; A: rank x rank, B: rank x ri */
      /* algorithm
         c <- B
         x <- 0
         q <- 1
         do 
           t <- A^(-1)c mod p
           x <- x+qt
           c <- (c-At)/p
           q <- qp
         end do
         then Ax-B=0 mod q and b=(B-Ax)/q hold after "do".
      */
      MKMAT(xmat,rank,ri); x = (Z **)(xmat)->body;
      MKMAT(*nmmat,rank,ri); nm = (Z **)(*nmmat)->body;
      wc = (int **)almat(rank,ri);
      for ( i = 0; i < rank; i++ )
        wc[i] = w[i]+rank;
      *rindp = rind = (int *)MALLOC_ATOMIC(rank*sizeof(int));
      *cindp = cind = (int *)MALLOC_ATOMIC((ri)*sizeof(int));

      period = F4_INTRAT_PERIOD;
      for ( q = ONE, count = 0; ; ) {
        if ( DP_Print > 3 )
          fprintf(stderr,"o");
        /* wc = b mod md */
        for ( i = 0; i < rank; i++ )
          for ( j = 0, bi = b[i], wi = wc[i]; j < ri; j++ ) {
            wi[j] = remqi((Q)bi[j],md);
            if ( wi[j] && sgnz(bi[j]) < 0 )
              wi[j] = md-wi[j];
          }
        /* wc = A^(-1)wc; wc is normalized */
        solve_by_lu_mod(w,rank,md,wc,ri,1);
        /* x += q*wc */
        for ( i = 0; i < rank; i++ )
          for ( j = 0, wi = wc[i]; j < ri; j++ ) mul1addtoz(q,wi[j],&x[i][j]);
        /* b =(A*wc+b)/md */
        for ( i = 0; i < rank; i++ )
          for ( j = 0; j < ri; j++ ) {
            u = b[i][j];
            for ( k = 0; k < rank; k++ ) mul1addtoz(a[i][k],wc[k][j],&u);
            divsz(u,mdq,&b[i][j]);
          }
        count++;
        /* q = q*md */
        mulz(q,mdq,&u); q = u;
        if ( count == period ) {
          ret = intmtoratm(xmat,q,*nmmat,dn);
          if ( ret ) {
            for ( j = k = l = 0; j < col; j++ )
              if ( cinfo[j] > 0 )
                rind[k++] = j;  
              else if ( !cinfo[j] )
                cind[l++] = j;
            ret = gensolve_check(mat,*nmmat,*dn,rind,cind);
            if ( ret ) {
              *rindp = rind;
              *cindp = cind;
              for ( j = k = 0; j < col; j++ )
                if ( !cinfo[j] )
                  cind[k++] = j;
              return rank;
            }
          } else {
            period = period*3/2;
            count = 0;
          }
        }
      }
  }
}