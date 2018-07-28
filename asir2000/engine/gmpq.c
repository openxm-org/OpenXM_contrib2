#include "ca.h"
#include "gmp.h"
#include "base.h"
#include "inline.h"

mpz_t ONEMPZ;
GZ ONEGZ;
int lf_lazy;
GZ current_mod_lf;
int current_mod_lf_size;

void isqrtgz(GZ a,GZ *r);
void bshiftgz(GZ a,int n,GZ *r);

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
  mp_set_memory_functions(Risa_GC_malloc,gc_realloc,gc_free);

  mpz_init(ONEMPZ); mpz_set_ui(ONEMPZ,1); MPZTOGZ(ONEMPZ,ONEGZ);
}

GZ utogz(unsigned int u)
{
  mpz_t z;
  GZ r;

  if ( !u ) return 0;
  mpz_init(z); mpz_set_ui(z,u); MPZTOGZ(z,r); return r;
}

GZ stogz(int s)
{
  mpz_t z;
  GZ r;

  if ( !s ) return 0;
  mpz_init(z); mpz_set_si(z,s); MPZTOGZ(z,r); return r;
}

GQ mpqtogzq(mpq_t a)
{
  GZ z;
  GQ q;

  if ( INTMPQ(a) ) {
    MPZTOGZ(mpq_numref(a),z); return (GQ)z;
  } else {
    MPQTOGQ(a,q); return q;
  }
}

GZ ztogz(Q a)
{
  mpz_t z;
  mpq_t b;
  N nm;
  GZ s;

  if ( !a ) return 0;
  nm = NM(a);
  mpz_init(z);
  mpz_import(z,PL(nm),-1,sizeof(BD(nm)[0]),0,0,BD(nm));
  if ( SGN(a)<0 ) mpz_neg(z,z);
  MPZTOGZ(z,s);
  return s;
}

Q gztoz(GZ a)
{
  N nm;
  Q q;
  int sgn;
  size_t len;

  if ( !a ) return 0;
  len = WORDSIZE_IN_N(BDY((GZ)a)); nm = NALLOC(len);
  mpz_export(BD(nm),&len,-1,sizeof(int),0,0,BDY((GZ)a));
  PL(nm) = len;
  sgn = mpz_sgn(BDY((GZ)a)); NTOQ(nm,sgn,q);
  return q;
}

void dupgz(GZ a,GZ *b)
{
  mpz_t t;

  if ( !a ) *b = a;
  else {
    mpz_init(t); mpz_set(t,BDY(a)); MPZTOGZ(t,*b);
  }
}

int n_bits_gz(GZ a)
{
  return a ? mpz_sizeinbase(BDY(a),2) : 0;
}

GQ qtogq(Q a)
{
  mpz_t z;
  mpq_t b;
  N nm,dn;
  GZ s;
  GQ r;

  if ( !a ) return 0;
  if ( INT(a) ) {
    nm = NM(a);
    mpz_init(z);
    mpz_import(z,PL(nm),-1,sizeof(BD(nm)[0]),0,0,BD(nm));
    if ( SGN(a)<0 ) mpz_neg(z,z);
    MPZTOGZ(z,s);
    return (GQ)s;
  } else {
    nm = NM(a); dn = DN(a);
    mpq_init(b);
    mpz_import(mpq_numref(b),PL(nm),-1,sizeof(BD(nm)[0]),0,0,BD(nm));
    mpz_import(mpq_denref(b),PL(dn),-1,sizeof(BD(dn)[0]),0,0,BD(dn));
    if ( SGN(a)<0 ) mpq_neg(b,b);
    MPQTOGQ(b,r);
    return r;
  }
}

Q gqtoq(GQ a)
{
  N nm,dn;
  Q q;
  int sgn;
  size_t len;

  if ( !a ) return 0;
  if ( NID(a) == N_GZ ) {
    len = WORDSIZE_IN_N(BDY((GZ)a)); nm = NALLOC(len);
    mpz_export(BD(nm),&len,-1,sizeof(int),0,0,BDY((GZ)a));
    PL(nm) = len;
    sgn = mpz_sgn(BDY((GZ)a)); NTOQ(nm,sgn,q);
  } else {
    len = WORDSIZE_IN_N(mpq_numref(BDY(a))); nm = NALLOC(len);
    mpz_export(BD(nm),&len,-1,sizeof(int),0,0,mpq_numref(BDY(a)));
    PL(nm) = len;
    len = WORDSIZE_IN_N(mpq_denref(BDY(a))); dn = NALLOC(len);
    mpz_export(BD(dn),&len,-1,sizeof(int),0,0,mpq_denref(BDY(a)));
    PL(dn) = len;
    sgn = mpz_sgn(mpq_numref(BDY(a))); NDTOQ(nm,dn,sgn,q);
  }
  return q;
}

P ptogp(P a)
{
  DCP dc,dcr,dcr0;
  P b;

  if ( !a ) return 0;
  if ( NUM(a) ) return (P)qtogq((Q)a);
  for ( dc = DC(a), dcr0 = 0; dc; dc = NEXT(dc) ) {
    NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = (P)ptogp(COEF(dc));
  }
  NEXT(dcr) = 0; MKP(VR(a),dcr0,b);
  return b;
}

P gptop(P a)
{
  DCP dc,dcr,dcr0;
  P b;

  if ( !a ) return 0;
  if ( NUM(a) ) b = (P)gqtoq((GQ)a); 
  else {
    for ( dc = DC(a), dcr0 = 0; dc; dc = NEXT(dc) ) {
      NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); 
      COEF(dcr) = (P)gptop(COEF(dc));
    }
    NEXT(dcr) = 0; MKP(VR(a),dcr0,b);
  }
  return b;
}

void addgz(GZ n1,GZ n2,GZ *nr)
{
  mpz_t t;
  int s1,s2;

  if ( !n1 ) *nr = n2;
  else if ( !n2 ) *nr = n1;
  else {
    mpz_init(t); mpz_add(t,BDY(n1),BDY(n2)); MPZTOGZ(t,*nr);
  }
}

void subgz(GZ n1,GZ n2,GZ *nr)
{
  mpz_t t;

  if ( !n1 ) 
    if ( !n2 ) 
      *nr = 0;
    else {
      t[0] = BDY(n2)[0]; mpz_neg(t,t); MPZTOGZ(t,*nr);
    }
  else if ( !n2 ) 
    *nr = n1;
  else if ( n1 == n2 ) 
    *nr = 0;
  else {
    mpz_init(t); mpz_sub(t,BDY(n1),BDY(n2)); MPZTOGZ(t,*nr);
  }
}

void mulgz(GZ n1,GZ n2,GZ *nr)
{
  mpz_t t;

  if ( !n1 || !n2 ) *nr = 0;
#if 1
  else if ( UNIGZ(n1) ) *nr = n2;
  else if ( UNIGZ(n2) ) *nr = n1;
  else if ( MUNIGZ(n1) ) chsgngz(n2,nr);
  else if ( MUNIGZ(n2) ) chsgngz(n1,nr);
#endif
  else {
    mpz_init(t); mpz_mul(t,BDY(n1),BDY(n2)); MPZTOGZ(t,*nr);
  }
}

/* nr += n1*n2 */

void muladdtogz(GZ n1,GZ n2,GZ *nr)
{
    GZ t;

  if ( n1 && n2 ) {
        if ( !(*nr) ) {
          NEWGZ(t); mpz_init(BDY(t)); *nr = t;
        }
        mpz_addmul(BDY(*nr),BDY(n1),BDY(n2));
    }
}

void mul1gz(GZ n1,int n2,GZ *nr)
{
  mpz_t t;

  if ( !n1 || !n2 ) *nr = 0;
  else {
    mpz_init(t); mpz_mul_ui(t,BDY(n1),(long)n2); MPZTOGZ(t,*nr);
  }
}

void divgz(GZ n1,GZ n2,GZ *nq)
{
  mpz_t t;
  mpq_t a, b, q;

  if ( !n2 ) {
    error("division by 0");
    *nq = 0;
  } else if ( !n1 ) 
    *nq = 0;
  else if ( n1 == n2 ) {
    mpz_init(t); mpz_set_ui(t,1); MPZTOGZ(t,*nq);
  } else {
    MPZTOMPQ(BDY(n1),a); MPZTOMPQ(BDY(n2),b);
    mpq_init(q); mpq_div(q,a,b); *nq = (GZ)mpqtogzq(q);
  }
}

void remgz(GZ n1,GZ n2,GZ *nr)
{
  mpz_t r;

  if ( !n2 ) {
    error("division by 0");
    *nr = 0;
  } else if ( !n1 || n1 == n2 )
    *nr = 0;
  else {
    mpz_init(r);
    mpz_mod(r,BDY(n1),BDY(n2));
    if ( !mpz_sgn(r) ) *nr = 0;
    else MPZTOGZ(r,*nr);
  }
}

void divqrgz(GZ n1,GZ n2,GZ *nq,GZ *nr)
{
  mpz_t t, a, b, q, r;

  if ( !n2 ) {
    error("division by 0");
    *nq = 0; *nr = 0;
  } else if ( !n1 ) {
    *nq = 0; *nr = 0;
  } else if ( n1 == n2 ) {
    mpz_init(t); mpz_set_ui(t,1); MPZTOGZ(t,*nq); *nr = 0;
  } else {
    mpz_init(q); mpz_init(r);
    mpz_fdiv_qr(q,r,BDY(n1),BDY(n2));
    if ( !mpz_sgn(q) ) *nq = 0;
    else MPZTOGZ(q,*nq);
    if ( !mpz_sgn(r) ) *nr = 0;
    else MPZTOGZ(r,*nr);
  }
}

void divsgz(GZ n1,GZ n2,GZ *nq)
{
  mpz_t t;
  mpq_t a, b, q;

  if ( !n2 ) {
    error("division by 0");
    *nq = 0;
  } else if ( !n1 ) 
    *nq = 0;
  else if ( n1 == n2 ) {
    mpz_init(t); mpz_set_ui(t,1); MPZTOGZ(t,*nq);
  } else {
    mpz_init(t); mpz_divexact(t,BDY(n1),BDY(n2)); MPZTOGZ(t,*nq);
  }
}

void chsgngz(GZ n,GZ *nr)
{
  mpz_t t;

  if ( !n )
    *nr = 0;
  else {
    t[0] = BDY(n)[0]; mpz_neg(t,t); MPZTOGZ(t,*nr);
  }
}

void pwrgz(GZ n1,Q n,GZ *nr)
{
  mpq_t t,q;
  mpz_t z;
  GQ p,r;

  if ( !n || UNIGZ(n1) ) *nr = ONEGZ;
  else if ( !n1 ) *nr = 0;
  else if ( !INT(n) ) {
    error("can't calculate fractional power."); *nr = 0;
  } else if ( MUNIGZ(n1) ) *nr = BD(NM(n))[0]%2 ? n1 : ONEGZ;
  else if ( PL(NM(n)) > 1 ) {
    error("exponent too big."); *nr = 0;
  } else if ( NID(n1)==N_GZ && SGN(n)>0 ) {
    mpz_init(z); mpz_pow_ui(z,BDY(n1),QTOS(n)); MPZTOGZ(z,*nr);
  } else {
    MPZTOMPQ(BDY(n1),q); MPQTOGQ(q,r);
    pwrgq(r,n,&p); *nr = (GZ)p;
  }
}

int cmpgz(GZ q1,GZ q2)
{
  int sgn;

  if ( !q1 ) 
    if ( !q2 ) 
      return 0;
    else
      return -mpz_sgn(BDY(q2));
  else if ( !q2 ) 
    return mpz_sgn(BDY(q1));
  else if ( (sgn = mpz_sgn(BDY(q1))) != mpz_sgn(BDY(q2)) ) 
      return sgn;
  else {
    sgn = mpz_cmp(BDY(q1),BDY(q2));
    if ( sgn > 0 ) return 1;
    else if ( sgn < 0 ) return -1;
    else return 0;
  }
}

void gcdgz(GZ n1,GZ n2,GZ *nq)
{
  mpz_t t;

  if ( !n1 ) *nq = n2;
  else if ( !n2 ) *nq = n1;
  else {
    mpz_init(t); mpz_gcd(t,BDY(n1),BDY(n2));
    MPZTOGZ(t,*nq);
  }
}

void invgz(GZ n1,GZ *nq)
{
  mpz_t t;

  mpz_init(t); mpz_invert(t,BDY(n1),BDY(current_mod_lf));
  MPZTOGZ(t,*nq);
}

void lcmgz(GZ n1,GZ n2,GZ *nq)
{
  GZ g,t;

  if ( !n1 || !n2 ) *nq = 0;
  else {
    gcdgz(n1,n2,&g); divsgz(n1,g,&t);
    mulgz(n2,t,nq);
  }
}

void gcdvgz(VECT v,GZ *q)
{
  int n,i;
  GZ *b;
  GZ g,g1;

  n = v->len;
  b = (GZ *)v->body;
  g = b[0];
  for ( i = 1; i < n; i++ ) {
    gcdgz(g,b[i],&g1); g = g1;
  }
  *q = g;
}

void gcdvgz_estimate(VECT v,GZ *q)
{
  int n,m,i;
  GZ s,t,u;
  GZ *b;

  n = v->len;
  b = (GZ *)v->body;
  if ( n == 1 ) {
    if ( mpz_sgn(BDY(b[0]))<0 ) chsgngz(b[0],q);
    else *q = b[0];
  }
  m = n/2;
  for ( i = 0, s = 0; i < m; i++ ) {
    if ( b[i] && mpz_sgn(BDY(b[i]))<0 ) subgz(s,b[i],&u);
    else addgz(s,b[i],&u);
    s = u;
  }
  for ( i = 0, t = 0; i < n; i++ ) {
    if ( b[i] && mpz_sgn(BDY(b[i]))<0 ) subgz(t,b[i],&u);
    else addgz(t,b[i],&u);
    t = u;
  }
  gcdgz(s,t,q);
}

void addgq(GQ n1,GQ n2,GQ *nr)
{
  mpq_t q1,q2,t;

  if ( !n1 ) *nr = n2;
  else if ( !n2 ) *nr = n1;
  else {
    if ( NID(n1) == N_GZ ) MPZTOMPQ(BDY((GZ)n1),q1);
    else q1[0] = BDY(n1)[0];
    if ( NID(n2) == N_GZ ) MPZTOMPQ(BDY((GZ)n2),q2);
    else q2[0] = BDY(n2)[0];
    mpq_init(t); mpq_add(t,q1,q2); *nr = mpqtogzq(t);
  }
}

void subgq(GQ n1,GQ n2,GQ *nr)
{
  mpq_t q1,q2,t;

  if ( !n1 ) 
    if ( !n2 ) *nr = 0;
    else {
      if ( NID(n1) == N_GZ ) chsgngz((GZ)n1,(GZ *)nr);
      else {
        mpq_init(t); mpq_neg(t,BDY(n2)); MPQTOGQ(t,*nr);
      }
    }
  else if ( !n2 ) *nr = n1;
  else if ( n1 == n2 ) *nr = 0;
  else {
    if ( NID(n1) == N_GZ ) MPZTOMPQ(BDY((GZ)n1),q1);
    else q1[0] = BDY(n1)[0];
    if ( NID(n2) == N_GZ ) MPZTOMPQ(BDY((GZ)n2),q2);
    else q2[0] = BDY(n2)[0];
    mpq_init(t); mpq_sub(t,q1,q2); *nr = mpqtogzq(t);
  }
}

void mulgq(GQ n1,GQ n2,GQ *nr)
{
  mpq_t t,q1,q2;

  if ( !n1 || !n2 ) *nr = 0;
  else {
    if ( NID(n1) == N_GZ ) MPZTOMPQ(BDY((GZ)n1),q1);
    else q1[0] = BDY(n1)[0];
    if ( NID(n2) == N_GZ ) MPZTOMPQ(BDY((GZ)n2),q2);
    else q2[0] = BDY(n2)[0];
    mpq_init(t); mpq_mul(t,q1,q2); *nr = mpqtogzq(t);
  }
}

void divgq(GQ n1,GQ n2,GQ *nq)
{
  mpq_t t,q1,q2;

  if ( !n2 ) {
    error("division by 0");
    *nq = 0;
    return;
  } else if ( !n1 ) *nq = 0;
  else if ( n1 == n2 ) *nq = (GQ)ONEGZ;
  else {
    if ( NID(n1) == N_GZ ) MPZTOMPQ(BDY((GZ)n1),q1);
    else q1[0] = BDY(n1)[0];
    if ( NID(n2) == N_GZ ) MPZTOMPQ(BDY((GZ)n2),q2);
    else q2[0] = BDY(n2)[0];
    mpq_init(t); mpq_div(t,q1,q2); *nq = mpqtogzq(t);
  }
}

void chsgngq(GQ n,GQ *nr)
{
  mpq_t t;

  if ( !n ) *nr = 0;
  else if ( NID(n) == N_GZ ) chsgngz((GZ)n,(GZ *)nr);
  else {
    mpq_init(t); mpq_neg(t,BDY(n)); MPQTOGQ(t,*nr);
  }
}

void pwrgq(GQ n1,Q n,GQ *nr)
{
  int e;
  mpz_t nm,dn;
  mpq_t t;

  if ( !n || UNIGZ((GZ)n1) || UNIGQ(n1) ) *nr = (GQ)ONEGZ;
  else if ( !n1 ) *nr = 0;
  else if ( !INT(n) ) {
    error("can't calculate fractional power."); *nr = 0;
  } else if ( PL(NM(n)) > 1 ) {
    error("exponent too big."); *nr = 0;
  } else {
    e = QTOS(n);
    if ( e < 0 ) {
      e = -e;
      if ( NID(n1)==N_GZ ) {
        nm[0] = ONEMPZ[0];
        dn[0] = BDY((GZ)n1)[0];
      } else {
        nm[0] = mpq_denref(BDY(n1))[0]; dn[0] = mpq_numref(BDY(n1))[0];
      }
    } else {
      if ( NID(n1)==N_GZ ) {
        nm[0] = BDY((GZ)n1)[0]; dn[0] = ONEMPZ[0];
      } else {
        nm[0] = mpq_numref(BDY(n1))[0]; dn[0] = mpq_denref(BDY(n1))[0];
      }
    }
    mpq_init(t);
    mpz_pow_ui(mpq_numref(t),nm,e); mpz_pow_ui(mpq_denref(t),dn,e);
    *nr = mpqtogzq(t);
  }
}

int cmpgq(GQ n1,GQ n2)
{
  mpq_t q1,q2;
  int sgn;

  if ( !n1 ) 
    if ( !n2 ) return 0;
    else return (NID(n2)==N_GZ) ? -mpz_sgn(BDY((GZ)n2)) : -mpq_sgn(BDY(n2));
  if ( !n2 ) return (NID(n1)==N_GZ) ? mpz_sgn(BDY((GZ)n1)) : mpq_sgn(BDY(n1));
  else if ( (sgn = mpq_sgn(BDY(n1))) != mpq_sgn(BDY(n2)) ) return sgn;
  else {
    if ( NID(n1) == N_GZ ) MPZTOMPQ(BDY((GZ)n1),q1);
    else q1[0] = BDY(n1)[0];
    if ( NID(n2) == N_GZ ) MPZTOMPQ(BDY((GZ)n2),q2);
    else q2[0] = BDY(n2)[0];
    sgn = mpq_cmp(q1,q2);
    if ( sgn > 0 ) return 1;
    else if ( sgn < 0 ) return -1;
    else return 0;
  }
}

void mkgwc(int k,int l,GZ *t)
{
  mpz_t a,b,q,nm,z,u;
  int i,n;

  n = MIN(k,l);
  mpz_init_set_ui(z,1);
  mpz_init(u); mpz_set(u,z); MPZTOGZ(u,t[0]);
  mpz_init(a); mpz_init(b); mpz_init(nm);
  for ( i = 1; i <= n; i++ ) {
    mpz_set_ui(a,k-i+1); mpz_set_ui(b,l-i+1); mpz_mul(nm,a,b);
    mpz_mul(z,BDY(t[i-1]),nm); mpz_fdiv_q_ui(z,z,i);
    mpz_init(u); mpz_set(u,z); MPZTOGZ(u,t[i]);
  }
}

void gz_lgp(P p,GZ *g,GZ *l);

void gz_ptozp(P p,int sgn,GQ *c,P *pr)
{
  GZ nm,dn;

  if ( !p ) {
    *c = 0; *pr = 0;
  } else {
    gz_lgp(p,&nm,&dn);
    divgz(nm,dn,(GZ *)c);
    divsp(CO,p,(P)*c,pr);
  }
}

void gz_lgp(P p,GZ *g,GZ *l)
{
  DCP dc;
  GZ g1,g2,l1,l2,l3,l4;

  if ( NUM(p) ) {
    if ( NID((GZ)p)==N_GZ ) {
      MPZTOGZ(BDY((GZ)p),*g);
      *l = ONEGZ;
    } else {
      MPZTOGZ(mpq_numref(BDY((GQ)p)),*g);
      MPZTOGZ(mpq_denref(BDY((GQ)p)),*l);
    }
  } else {
    dc = DC(p); gz_lgp(COEF(dc),g,l);
    for ( dc = NEXT(dc); dc; dc = NEXT(dc) ) {
      gz_lgp(COEF(dc),&g1,&l1); gcdgz(*g,g1,&g2); *g = g2;
      gcdgz(*l,l1,&l2); mulgz(*l,l1,&l3); divgz(l3,l2,l);
    }
  } 
}

void gz_qltozl(GQ *w,int n,GZ *dvr)
{
  GZ nm,dn;
  GZ g,g1,l1,l2,l3;
  GQ c;
  int i;
  struct oVECT v;

  for ( i = 0; i < n; i++ )
    if ( w[i] && NID(w[i])==N_GQ )
      break;
  if ( i == n ) {
    v.id = O_VECT; v.len = n; v.body = (pointer *)w;
    gcdvgz(&v,dvr); return;
  }
  for ( i = 0; !w[i]; i++ );
  c = w[i]; 
  if ( NID(c)==N_GQ ) {
    MPZTOGZ(mpq_numref(BDY(c)),nm); MPZTOGZ(mpq_denref(BDY(c)),dn);
  } else {
    MPZTOGZ(BDY((GZ)c),nm); dn = ONEGZ;
  }
  for ( i++; i < n; i++ ) {
    c = w[i]; 
    if ( !c ) continue;
    if ( NID(c)==N_GQ ) {
      MPZTOGZ(mpq_numref(BDY(c)),g1); MPZTOGZ(mpq_denref(BDY(c)),l1);
    } else {
      MPZTOGZ(BDY((GZ)c),g1); l1 = ONEGZ;
    }
    gcdgz(nm,g1,&g); nm = g;
    gcdgz(dn,l1,&l2); mulgz(dn,l1,&l3); divgz(l3,l2,&dn);
  }
  divgz(nm,dn,dvr);
}

int gz_bits(GQ q)
{
  if ( !q ) return 0;
  else if ( NID(q)==N_Q ) 
    return n_bits(NM((Q)q))+(INT((Q)q)?0:n_bits(DN((Q)q)));
  else if ( NID(q)==N_GZ ) return mpz_sizeinbase(BDY((GZ)q),2);
  else
    return mpz_sizeinbase(mpq_numref(BDY(q)),2)
      + mpz_sizeinbase(mpq_denref(BDY(q)),2);
}

int gzp_mag(P p)
{
  int s;
  DCP dc;
  
  if ( !p ) return 0;
  else if ( OID(p) == O_N ) return gz_bits((GQ)p);
  else {
    for ( dc = DC(p), s = 0; dc; dc = NEXT(dc) ) s += gzp_mag(COEF(dc));
    return s;
  }
}

void makesubstgz(VL v,NODE *s)
{
  NODE r,r0;
  GZ q;  
  unsigned int n;

  for ( r0 = 0; v; v = NEXT(v) ) {
    NEXTNODE(r0,r); BDY(r) = (pointer)v->v;
#if defined(_PA_RISC1_1)
    n = mrand48()&BMASK; q = utogz(n);
#else
    n = random(); q = utogz(n);
#endif
    NEXTNODE(r0,r); BDY(r) = (pointer)q;
  }
  if ( r0 ) NEXT(r) = 0;
  *s = r0;
}

unsigned int remgq(GQ a,unsigned int mod)
{
  unsigned int c,nm,dn;
  mpz_t r;

  if ( !a ) return 0;
  else if ( NID(a)==N_GZ ) {
    mpz_init(r);
    c = mpz_fdiv_r_ui(r,BDY((GZ)a),mod);
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

#define GZ_F4_INTRAT_PERIOD 8 

int gz_generic_gauss_elim(MAT mat,MAT *nm,GZ *dn,int **rindp,int **cindp)
{
  int **wmat;
  GZ **bmat,**tmat,*bmi,*tmi;
  GZ q,m1,m2,m3,s,u;
  int *wmi,*colstat,*wcolstat,*rind,*cind;
  int row,col,ind,md,i,j,k,l,t,t1,rank,rank0,inv;
  MAT r,crmat;
  int ret;

  bmat = (GZ **)mat->body;
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
        wmi[j] = remgq((GQ)bmi[j],md);
    rank = generic_gauss_elim_mod(wmat,row,col,md,wcolstat);
    if ( !ind ) {
RESET:
      m1 = utogz(md);
      rank0 = rank;
      bcopy(wcolstat,colstat,col*sizeof(int));
      MKMAT(crmat,rank,col-rank);
      MKMAT(r,rank,col-rank); *nm = r;
      tmat = (GZ **)crmat->body;
      for ( i = 0; i < rank; i++ )
        for ( j = k = 0, tmi = tmat[i], wmi = wmat[i]; j < col; j++ )
          if ( !colstat[j] ) tmi[k++] = utogz(wmi[j]);
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

      inv = invm(remgq((GQ)m1,md),md);
      m2 = utogz(md); mulgz(m1,m2,&m3);
      for ( i = 0; i < rank; i++ )      
        for ( j = k = 0, tmi = tmat[i], wmi = wmat[i]; j < col; j++ )
          if ( !colstat[j] ) {
            if ( tmi[k] ) {
            /* f3 = f1+m1*(m1 mod m2)^(-1)*(f2 - f1 mod m2) */
              t = remgq((GQ)tmi[k],md);
              if ( wmi[j] >= t ) t = wmi[j]-t;
              else t = md-(t-wmi[j]);
              DMAR(t,inv,0,md,t1) 
              u = utogz(t1); mulgz(m1,u,&s); 
              addgz(tmi[k],s,&u); tmi[k] = u;
            } else if ( wmi[j] ) {
            /* f3 = m1*(m1 mod m2)^(-1)*f2 */
              DMAR(wmi[j],inv,0,md,t)
              u = utogz(t); mulgz(m1,u,&s); tmi[k] = s;
            }
            k++;
          }
      m1 = m3;
      if ( ind % GZ_F4_INTRAT_PERIOD ) 
        ret = 0;
      else 
        ret = gz_intmtoratm(crmat,m1,*nm,dn);
      if ( ret ) {
        *rindp = rind = (int *)MALLOC_ATOMIC(rank*sizeof(int));
        *cindp = cind = (int *)MALLOC_ATOMIC((col-rank)*sizeof(int));
        for ( j = k = l = 0; j < col; j++ )
          if ( colstat[j] ) rind[k++] = j;  
          else cind[l++] = j;
        if ( gz_gensolve_check(mat,*nm,*dn,rind,cind) )
          return rank;
      }
    }
  }
}

int gz_generic_gauss_elim2(MAT mat,MAT *nm,GZ *dn,int **rindp,int **cindp)
{

  MAT full;
  GZ **bmat,**b;
  GZ *bmi;
  GZ dn0;
  int row,col,md,i,j,rank,ret;
  int **wmat;
  int *wmi;
  int *colstat,*rowstat;

  bmat = (GZ **)mat->body;
  row = mat->row; col = mat->col;
  wmat = (int **)almat(row,col);
  colstat = (int *)MALLOC_ATOMIC(col*sizeof(int));
  rowstat = (int *)MALLOC_ATOMIC(row*sizeof(int));
  /* XXX */
  md = get_lprime(0);
  for ( i = 0; i < row; i++ )
    for ( j = 0, bmi = bmat[i], wmi = wmat[i]; j < col; j++ )
      wmi[j] = remgq((GQ)bmi[j],md);
  rank = generic_gauss_elim_mod2(wmat,row,col,md,colstat,rowstat);
  b = (GZ **)MALLOC(rank*sizeof(GZ));
  for ( i = 0; i < rank; i++ ) b[i] = bmat[rowstat[i]];
  NEWMAT(full); full->row = rank; full->col = col; full->body = (pointer **)b;
  ret = gz_generic_gauss_elim_full(full,nm,dn,rindp,cindp);
  if ( !ret ) {
    rank = gz_generic_gauss_elim(mat,nm,&dn0,rindp,cindp);
    for ( i = 0; i < rank; i++ ) dn[i] = dn0;
  }
  return rank;
}

int gz_generic_gauss_elim_full(MAT mat,MAT *nm,GZ *dn,int **rindp,int **cindp)
{
  int **wmat;
  int *stat;
  GZ **bmat,**tmat,*bmi,*tmi,*ri;
  GZ q,m1,m2,m3,s,u;
  int *wmi,*colstat,*wcolstat,*rind,*cind;
  int row,col,ind,md,i,j,k,l,t,t1,rank,rank0,inv,h;
  MAT r,crmat;
  int ret,initialized,done;

  initialized = 0;
  bmat = (GZ **)mat->body;
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
        wmi[j] = remgq((GQ)bmi[j],md);
    rank = generic_gauss_elim_mod(wmat,row,col,md,wcolstat);
    if ( rank < row ) continue;
    if ( !initialized ) {
      m1 = utogz(md);
      bcopy(wcolstat,colstat,col*sizeof(int));
      MKMAT(crmat,row,col-row);
      MKMAT(r,row,col-row); *nm = r;
      tmat = (GZ **)crmat->body;
      for ( i = 0; i < row; i++ )
        for ( j = k = 0, tmi = tmat[i], wmi = wmat[i]; j < col; j++ )
          if ( !colstat[j] ) tmi[k++] = utogz(wmi[j]);
      initialized = 1;
    } else {
      for ( j = 0; (j<col) && (colstat[j]==wcolstat[j]); j++ );
      if ( j < col ) continue;

      inv = invm(remgq((GQ)m1,md),md);
      m2 = utogz(md); mulgz(m1,m2,&m3);
      for ( i = 0; i < row; i++ )
        switch ( stat[i] ) {
          case 1: 
            /* consistency check */
            ri = (GZ *)BDY(r)[i]; wmi = wmat[i];
            for ( j = 0; j < col; j++ ) if ( colstat[j] ) break;
            h = md-remgq((GQ)dn[i],md);
            for ( j++, k = 0; j < col; j++ )
              if ( !colstat[j] ) {
                t = remgq((GQ)ri[k],md);
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
                  t = remgq((GQ)tmi[k],md);
                  if ( wmi[j] >= t ) t = wmi[j]-t;
                  else t = md-(t-wmi[j]);
                  DMAR(t,inv,0,md,t1) 
                  u = utogz(t1); mulgz(m1,u,&s); 
                  addgz(tmi[k],s,&u); tmi[k] = u;
                } else if ( wmi[j] ) {
                /* f3 = m1*(m1 mod m2)^(-1)*f2 */
                  DMAR(wmi[j],inv,0,md,t)
                  u = utogz(t); mulgz(m1,u,&s); tmi[k] = s;
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
        ret = gz_intmtoratm2(crmat,m1,*nm,dn,stat);
      if ( ret ) {
        *rindp = rind = (int *)MALLOC_ATOMIC(row*sizeof(int));
        *cindp = cind = (int *)MALLOC_ATOMIC((col-row)*sizeof(int));
        for ( j = k = l = 0; j < col; j++ )
          if ( colstat[j] ) rind[k++] = j;  
          else cind[l++] = j;
        return gz_gensolve_check2(mat,*nm,dn,rind,cind);
      }
    }
  }
}

int gz_generic_gauss_elim_direct(MAT mat,MAT *nm,GZ *dn,int **rindp,int **cindp){
  GZ **bmat,*s;
  GZ u,v,w,x,d,t,y;
  int row,col,i,j,k,l,m,rank;
  int *colstat,*colpos,*cind;
  MAT r,in;

  row = mat->row; col = mat->col;
  MKMAT(in,row,col);
  for ( i = 0; i < row; i++ )
    for ( j = 0; j < col; j++ ) in->body[i][j] = mat->body[i][j];
  bmat = (GZ **)in->body;
  colstat = (int *)MALLOC_ATOMIC(col*sizeof(int));
  *rindp = colpos = (int *)MALLOC_ATOMIC(row*sizeof(int));
  for ( j = 0, rank = 0, d = ONEGZ; j < col; j++ ) {
    for ( i = rank; i < row && !bmat[i][j]; i++  );
    if ( i == row ) { colstat[j] = 0; continue; }
    else { colstat[j] = 1; colpos[rank] = j; }
    if ( i != rank )
      for ( k = j; k < col; k++ ) {
        t = bmat[i][k]; bmat[i][k] = bmat[rank][k]; bmat[rank][k] = t;
      }
    for ( i = rank+1, v = bmat[rank][j]; i < row; i++ )
      for ( k = j, u = bmat[i][j]; k < col; k++ ) {
        mulgz(bmat[i][k],v,&w); mulgz(bmat[rank][k],u,&x);
        subgz(w,x,&y); divsgz(y,d,&bmat[i][k]);
      }
    d = v; rank++;
  }
  *dn = d;
  s = (GZ *)MALLOC(col*sizeof(GZ));
  for ( i = rank-1; i >= 0; i-- ) {
    for ( k = colpos[i]; k < col; k++ ) mulgz(bmat[i][k],d,&s[k]); 
    for ( m = rank-1; m > i; m-- ) {
      for ( k = colpos[m], u = bmat[i][k]; k < col; k++ ) {
        mulgz(bmat[m][k],u,&w); subgz(s[k],w,&x); s[k] = x;
      }
    }
    for ( k = colpos[i], u = bmat[i][k]; k < col; k++ )
      divgz(s[k],u,&bmat[i][k]);
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

int gz_intmtoratm(MAT mat,GZ md,MAT nm,GZ *dn)
{
  GZ t,s,b,dn0,dn1,nm1,q,u,unm,udn,dmy;
  int i,j,k,l,row,col,sgn,ret;
  GZ **rmat,**tmat,*tmi,*nmk;

  if ( UNIGZ(md) )
    return 0;
  row = mat->row; col = mat->col;
  bshiftgz(md,1,&t);
  isqrtgz(t,&s);
  bshiftgz(s,64,&b);
  if ( !b ) b = ONEGZ;
  dn0 = ONEGZ;
  tmat = (GZ **)mat->body;
  rmat = (GZ **)nm->body;
  for ( i = 0; i < row; i++ )
    for ( j = 0, tmi = tmat[i]; j < col; j++ )
      if ( tmi[j] ) {
        mulgz(tmi[j],dn0,&s);
        divqrgz(s,md,&dmy,&u);
        ret = gz_inttorat(u,md,b,&sgn,&unm,&udn);
        if ( !ret ) return 0;
        else {
          if ( sgn < 0 ) chsgngz(unm,&nm1);
          else nm1 = unm;
          dn1 = udn;
          if ( !UNIGZ(dn1) ) {
            for ( k = 0; k < i; k++ )
              for ( l = 0, nmk = rmat[k]; l < col; l++ ) {
                mulgz(nmk[l],dn1,&q); nmk[l] = q;
              }
            for ( l = 0, nmk = rmat[i]; l < j; l++ ) {
              mulgz(nmk[l],dn1,&q); nmk[l] = q;
            }
          }
          rmat[i][j] = nm1;
          mulgz(dn0,dn1,&q); dn0 = q;
        }
      }
  *dn = dn0;
  return 1;
}

int gz_intmtoratm2(MAT mat,GZ md,MAT nm,GZ *dn,int *stat)
{
  int row,col,i,j,ret;
  GZ dn0,dn1,t,s,b;
  GZ *w,*tmi;
  GZ **tmat;

  bshiftgz(md,1,&t);
  isqrtgz(t,&s);
  bshiftgz(s,64,&b);
  tmat = (GZ **)mat->body;
  if ( UNIGZ(md) ) return 0;
  row = mat->row; col = mat->col;
  dn0 = ONEGZ;
  for ( i = 0; i < row; i++ )
    if ( cmpgz(dn[i],dn0) > 0 ) dn0 = dn[i];
  w = (GZ *)MALLOC(col*sizeof(GZ));
  for ( i = 0; i < row; i++ )
    if ( stat[i] == 0 ) {
      for ( j = 0, tmi = tmat[i]; j < col; j++ )
          mulgz(tmi[j],dn0,&w[j]);
      ret = gz_intvtoratv(w,col,md,b,BDY(nm)[i],&dn[i]);
      if ( ret ) {
        stat[i] = 1;
        mulgz(dn0,dn[i],&t); dn[i] = t; dn0 = t;
      }
    }
  for ( i = 0; i < row; i++ ) if ( !stat[i] ) break;
  if ( i == row ) return 1;
  else return 0;
}

int gz_intvtoratv(GZ *v,int n,GZ md,GZ b,GZ *nm,GZ *dn)
{
  GZ dn0,dn1,q,s,u,nm1,unm,udn,dmy;
  GZ *nmk;
  int j,l,col,ret,sgn;

  for ( j = 0; j < n; j++ ) nm[j] = 0;
  dn0 = ONEGZ;
  for ( j = 0; j < n; j++ ) {
    if ( !v[j] ) continue;
    mulgz(v[j],dn0,&s);
    divqrgz(s,md,&dmy,&u);
    ret = gz_inttorat(u,md,b,&sgn,&unm,&udn);
    if ( !ret ) return 0;
    if ( sgn < 0 ) chsgngz(unm,&nm1);
    else nm1 = unm;
    dn1 = udn;
    if ( !UNIGZ(dn1) )
      for ( l = 0; l < j; l++ ) {
        mulgz(nm[l],dn1,&q); nm[l] = q;
      }
    nm[j] = nm1;
    mulgz(dn0,dn1,&q); dn0 = q;
  }
  *dn = dn0;
  return 1;
}

/* assuming 0 < c < m */

int gz_inttorat(GZ c,GZ m,GZ b,int *sgnp,GZ *nmp,GZ *dnp)
{
  GZ qq,t,u1,v1,r1;
  GZ q,u2,v2,r2;

  u1 = 0; v1 = ONEGZ; u2 = m; v2 = c;
  while ( cmpgz(v2,b) >= 0 ) {
    divqrgz(u2,v2,&q,&r2); u2 = v2; v2 = r2;
    mulgz(q,v1,&t); subgz(u1,t,&r1); u1 = v1; v1 = r1; 
  }
  if ( cmpgz(v1,b) >= 0 ) return 0;
  else {
    *nmp = v2;
    if ( mpz_sgn(BDY(v1))<0  ) {
      *sgnp = -1; chsgngz(v1,dnp);
    } else {
      *sgnp = 1; *dnp = v1;
    }
    return 1;
  }
}

extern int f4_nocheck;

int gz_gensolve_check(MAT mat,MAT nm,GZ dn,int *rind,int *cind)
{
  int row,col,rank,clen,i,j,k,l;
  GZ s,t;
  GZ *w;
  GZ *mati,*nmk;

  if ( f4_nocheck ) return 1;
  row = mat->row; col = mat->col; rank = nm->row; clen = nm->col;  
  w = (GZ *)MALLOC(clen*sizeof(GZ));
  for ( i = 0; i < row; i++ ) {
    mati = (GZ *)mat->body[i];
    bzero(w,clen*sizeof(GZ));
    for ( k = 0; k < rank; k++ )
      for ( l = 0, nmk = (GZ *)nm->body[k]; l < clen; l++ ) {
        mulgz(mati[rind[k]],nmk[l],&t); addgz(w[l],t,&s); w[l] = s;
      }
    for ( j = 0; j < clen; j++ ) {
      mulgz(dn,mati[cind[j]],&t);
      if ( cmpgz(w[j],t) ) break;
    }
    if ( j != clen ) break;
  }
  if ( i != row ) return 0;
  else return 1;
}

int gz_gensolve_check2(MAT mat,MAT nm,GZ *dn,int *rind,int *cind)
{
  int row,col,rank,clen,i,j,k,l;
  GZ s,t,u,d;
  GZ *w,*m;
  GZ *mati,*nmk;

  if ( f4_nocheck ) return 1;
  row = mat->row; col = mat->col; rank = nm->row; clen = nm->col;  
  w = (GZ *)MALLOC(clen*sizeof(GZ));
  m = (GZ *)MALLOC(clen*sizeof(GZ));
  for ( d = dn[0], i = 1; i < rank; i++ ) {
    lcmgz(d,dn[i],&t); d = t;
  }
  for ( i = 0; i < rank; i++ ) divsgz(d,dn[i],&m[i]);
  for ( i = 0; i < row; i++ ) {
    mati = (GZ *)mat->body[i];
    bzero(w,clen*sizeof(GZ));
    for ( k = 0; k < rank; k++ ) {
      mulgz(mati[rind[k]],m[k],&u);
      for ( l = 0, nmk = (GZ *)nm->body[k]; l < clen; l++ ) {
        mulgz(u,nmk[l],&t); addgz(w[l],t,&s); w[l] = s;
      }
    }
    for ( j = 0; j < clen; j++ ) {
      mulgz(d,mati[cind[j]],&t);
      if ( cmpgz(w[j],t) ) break;
    }
    if ( j != clen ) break;
  }
  if ( i != row ) return 0;
  else return 1;
}

void isqrtgz(GZ a,GZ *r)
{
  int k;
  GZ x,t,x2,xh,quo,rem;
  Q two;

  if ( !a ) *r = 0;
  else if ( UNIGZ(a) ) *r = ONEGZ;
  else {
    k = gz_bits((GQ)a); /* a <= 2^k-1 */
    bshiftgz(ONEGZ,-((k>>1)+(k&1)),&x); /* a <= x^2 */
    STOQ(2,two);
    while ( 1 ) {
      pwrgz(x,two,&t);
      if ( cmpgz(t,a) <= 0 ) {
        *r = x; return;
      } else {
        if ( mpz_tstbit(BDY(x),0) ) addgz(x,a,&t);
        else t = a;
        bshiftgz(x,-1,&x2); divqrgz(t,x2,&quo,&rem);
        bshiftgz(x,1,&xh); addgz(quo,xh,&x);
      }
    }
  }
}

void bshiftgz(GZ a,int n,GZ *r)
{
  mpz_t t;

  if ( !a ) *r = 0;
  else if ( n == 0 ) *r = a;
  else if ( n < 0 ) {
    mpz_init(t); mpz_mul_2exp(t,BDY(a),-n); MPZTOGZ(t,*r);
  } else { 
    mpz_init(t); mpz_fdiv_q_2exp(t,BDY(a),n);
    if ( !mpz_sgn(t) ) *r = 0;
    else MPZTOGZ(t,*r);
  }
}

void addlf(GZ a,GZ b,GZ *c)
{
  GZ t;

  addgz(a,b,c);
  if ( !lf_lazy ) {
//    remgz(*c,current_mod_lf,&t); *c = t;
    if ( cmpgz(*c,current_mod_lf) >= 0 ) {
      subgz(*c,current_mod_lf,&t); *c = t;
    }
  }
}

void sublf(GZ a,GZ b,GZ *c)
{
  GZ t;

  subgz(a,b,c);
  if ( !lf_lazy ) {
    remgz(*c,current_mod_lf,&t); *c = t;
  }
}

void mullf(GZ a,GZ b,GZ *c)
{
  GZ t;

  mulgz(a,b,c);
  if ( !lf_lazy ) {
    remgz(*c,current_mod_lf,&t); *c = t;
  }
}

void divlf(GZ a,GZ b,GZ *c)
{
  GZ t,inv;

  invgz(b,&inv);
  mulgz(a,inv,c);
  if ( !lf_lazy ) {
    remgz(*c,current_mod_lf,&t); *c = t;
  }
}

void chsgnlf(GZ a,GZ *c)
{
  GZ t;

  chsgngz(a,c);
  if ( !lf_lazy ) {
    remgz(*c,current_mod_lf,&t); *c = t;
  }
}

void lmtolf(LM a,GZ *b)
{
  Q q;

  if ( !a ) *b = 0;
  else {
    NTOQ(BDY(a),1,q); *b = ztogz(q);
  }
}

void setmod_lf(N p)
{
    Q q;

    NTOQ(p,1,q); current_mod_lf = ztogz(q);
    current_mod_lf_size = mpz_size(BDY(current_mod_lf))+1;
}

void simplf_force(GZ a,GZ *b)
{
    GZ t;

    remgz(a,current_mod_lf,&t); *b = t;
}
