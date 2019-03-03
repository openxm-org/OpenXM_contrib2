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
 * $OpenXM: OpenXM_contrib2/asir2018/engine/lmi.c,v 1.1 2018/09/19 05:45:07 noro Exp $
*/
#include "ca.h"
#include "base.h"
#include "inline.h"

int lm_lazy;

static LM current_mod_lm;

void random_lm(LM *r)
{
  Z n;
  LM s;

  if ( !current_mod_lm )
    error("random_lm : current_mod_lm is not set");
  randomz(z_bits((Q)current_mod_lm)+1,&n);
  MKLM(BDY(n),s);
  simplm_force(s,r);
}

void setmod_lm(Z p)
{
  MKLM(BDY(p),current_mod_lm);
  setmod_lf(p);
}

void getmod_lm(Z *p)
{
  if ( !current_mod_lm )
    *p = 0;
  else
    MPZTOZ(BDY(current_mod_lm),*p);
}

void simplm(LM n,LM *r)
{
  mpz_t rem;

  if ( !n || NID(n) != N_LM || lm_lazy )
    *r = n;
  else {
    mpz_init(rem);
    mpz_mod(rem,BDY(n),BDY(current_mod_lm));
    if ( mpz_sgn(rem) )
      MKLM(rem,*r);
    else
      *r = 0;
  }
}

void simplm_force(LM n,LM *r)
{
  mpz_t rem;

  if ( !n || NID(n) != N_LM )
    *r = n;
  else {
    mpz_init(rem);
    mpz_mod(rem,BDY(n),BDY(current_mod_lm));
    if ( mpz_sgn(rem) )
      MKLM(rem,*r);
    else
      *r = 0;
  }
}

void qtolm(Q q,LM *l)
{
  LM nm0,nm,nm1,dn,dn1;
  mpz_t t;

  if ( !q || (OID(q)==O_N && ((NID(q) == N_LM) || (NID(q) == N_GFPN))) ) { /* XXX */
    *l = (LM)q;
  } else if ( OID(q) == O_N && NID(q) == N_Q ) {
    if ( q->z ) {
      if ( mpz_sgn(BDY((Z)q)) > 0 ) {
        MKLM(BDY((Z)q),nm); 
        simplm(nm,l);
      } else {
        mpz_init(t); mpz_neg(t,BDY((Z)q));
        MKLM(t,nm); simplm(nm,&nm1); chsgnlm(nm1,l);
      }
    } else {
      if ( mpq_sgn(BDY(q)) > 0 ) {
        MKLM(mpq_numref(BDY(q)),nm);
        simplm_force(nm,&nm1);
      } else {
        mpz_init(t); mpz_neg(t,mpq_numref(BDY(q)));
        MKLM(t,nm0); simplm(nm0,&nm); chsgnlm(nm,&nm1);
      }
      MKLM(mpq_denref(BDY(q)),dn);
      simplm_force(dn,&dn1);
      divlm(nm1,dn1,l);
    }
  } else
    error("qtolm : invalid argument");
}

#define NZLM(a) ((a)&&(NID(a)==N_LM))

void addlm(LM a,LM b,LM *c)
{
  mpz_t t;
  LM s,z;

  qtolm((Q)a,&z); a = z; qtolm((Q)b,&z); b = z;
  if ( !a )
    *c = b;
  else if ( !b )
    *c = a;
  else {
    mpz_init(t); mpz_add(t,BDY(a),BDY(b));
    MKLM(t,s); simplm(s,c);
  }
}

void sublm(LM a,LM b,LM *c)
{
  mpz_t t;
  LM s,z;
  int sgn;

  qtolm((Q)a,&z); a = z; qtolm((Q)b,&z); b = z;
  if ( !b )
    *c = a;
  else if ( !a )
    chsgnlm(b,c);
  else {
    sgn = mpz_cmp(BDY(a),BDY(b));
    if ( sgn > 0 ) {
      mpz_init(t); mpz_sub(t,BDY(a),BDY(b));
      MKLM(t,*c);
    } else if ( sgn < 0 ) {
      mpz_init(t); mpz_sub(t,BDY(b),BDY(a));
      MKLM(t,s); chsgnlm(s,c);
    } else
      *c = 0;
  }
}

void mullm(LM a,LM b,LM *c)
{
  mpz_t t;
  LM s,z;

  qtolm((Q)a,&z); a = z; qtolm((Q)b,&z); b = z;
  if ( !a || !b )
    *c = 0;
  else {
    mpz_init(t); mpz_mul(t,BDY(a),BDY(b));
    MKLM(t,s); simplm(s,c);
  }
}

void divlm(LM a,LM b,LM *c)
{
  mpz_t t,u;
  LM s,z;

  qtolm((Q)a,&z); a = z; qtolm((Q)b,&z); b = z;
  if ( !b )
    error("divlm: division by 0");
  else if ( !a )
    *c = 0;
  else {
    mpz_init(t); mpz_invert(t,BDY(b),BDY(current_mod_lm));
    mpz_init(u); mpz_mul(u,BDY(a),t);
    MKLM(u,s); simplm(s,c);
  }
}

void chsgnlm(LM a,LM *c)
{
  LM t;

  if ( !a )
    *c = a;
  else {
    qtolm((Q)a,&t); a = t;
    simplm_force(a,&t);
    sublm(current_mod_lm,t,c);
  }
}

void pwrlm(LM a,Z b,LM *c)
{
  mpz_t t;

  if ( !b ) 
    MKLM(BDY(ONE),*c);
  else if ( !a )
    *c = 0;
  else {
    mpz_init(t); mpz_powm(t,BDY(a),BDY(b),BDY(current_mod_lm));
    MKLM(t,*c);
  }
}

int cmplm(LM a,LM b)
{
  LM z;
  int sgn;

  qtolm((Q)a,&z); a = z; qtolm((Q)b,&z); b = z;
  if ( !a )
    if ( !b )
      return 0;
    else
      return -1;
  else if ( !b )
      return 1;
  else {
    sgn = mpz_cmp(BDY(a),BDY(b));
    if ( sgn > 0 ) return 1;
    else if ( sgn < 0 ) return -1;
    else return 0;
  }
}
