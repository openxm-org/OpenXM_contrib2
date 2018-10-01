/* $OpenXM$ */
#include "gmp.h"

mp_limb_t mul64(mp_limb_t a,mp_limb_t b,mp_limb_t *c)
{
  mp_limb_t u;
  u = (mp_limb_t)mpn_mul_1((mp_limb_t *)c,(mp_limb_t *)&a,1,(mp_limb_t)b);
  c[1] = u;
  return u;
}

mp_limb_t mod64(mp_limb_t u,mp_limb_t l,mp_limb_t b)
{
  mp_limb_t q[2],d[2];
  mp_limb_t r;

  d[0] = l; d[1] = u;
  r = (mp_limb_t)mpn_divmod_1((mp_limb_t *)q,(mp_limb_t *)d,2,(mp_limb_t)b);
  return r;
}

mp_limb_t mulmod64(mp_limb_t a,mp_limb_t b,mp_limb_t m)
{
  mp_limb_t d[2],q[2];
  mp_limb_t r;

  d[1] = (mp_limb_t)mpn_mul_1(d,(mp_limb_t *)&a,1,(mp_limb_t)b);
  r = (mp_limb_t)mpn_divmod_1((mp_limb_t *)q,(mp_limb_t *)d,2,(mp_limb_t)m);
  return r;
}

mp_limb_t muladdmod64(mp_limb_t a,mp_limb_t b,mp_limb_t c,mp_limb_t m)
{
  mp_limb_t d[2],q[2];
  mp_limb_t r;

  d[0] = c;
  d[1] = (mp_limb_t)mpn_addmul_1(d,(mp_limb_t *)&a,1,(mp_limb_t)b);
  r = (mp_limb_t)mpn_divmod_1((mp_limb_t *)q,(mp_limb_t *)d,2,(mp_limb_t)m);
  return r;
}

mp_limb_t invmod64(mp_limb_t a,mp_limb_t m)
{
  mpz_t x,y,z;

  mpz_init_set_ui(x,a);
  mpz_init_set_ui(y,m);
  mpz_init(z);
  mpz_invert(z,x,y);
  return mpz_get_ui(z);
}
