#include "gmp.h"

unsigned long mul64(unsigned long a,unsigned long b,unsigned long *c)
{
  unsigned long u;
  u = (unsigned long)mpn_mul_1((mp_limb_t *)c,(mp_limb_t *)&a,1,(mp_limb_t)b);
  c[1] = u;
  return u;
}

unsigned long mod64(unsigned long u,unsigned long l,unsigned long b)
{
  unsigned long q[2],d[2];
  unsigned long r;

  d[0] = l; d[1] = u;
  r = (unsigned long)mpn_divmod_1((mp_limb_t *)q,(mp_limb_t *)d,2,(mp_limb_t)b);
  return r;
}

unsigned long mulmod64(unsigned long a,unsigned long b,unsigned long m)
{
  unsigned long d[2],q[2];
  unsigned long r;

  d[1] = (unsigned long)mpn_mul_1(d,(mp_limb_t *)&a,1,(mp_limb_t)b);
  r = (unsigned long)mpn_divmod_1((mp_limb_t *)q,(mp_limb_t *)d,2,(mp_limb_t)m);
  return r;
}

unsigned long muladdmod64(unsigned long a,unsigned long b,unsigned long c,unsigned long m)
{
  unsigned long d[2],q[2];
  unsigned long r;

  d[0] = c;
  d[1] = (unsigned long)mpn_addmul_1(d,(mp_limb_t *)&a,1,(mp_limb_t)b);
  r = (unsigned long)mpn_divmod_1((mp_limb_t *)q,(mp_limb_t *)d,2,(mp_limb_t)m);
  return r;
}

unsigned long invmod64(unsigned long a,unsigned long m)
{
  mpz_t x,y,z;

  mpz_init_set_ui(x,a);
  mpz_init_set_ui(y,m);
  mpz_invert(z,x,y);
  return mpz_get_ui(z);
}
