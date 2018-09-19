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
 * $OpenXM$
*/
#include "ca.h"

void addnum(VL vl,Num a,Num b,Num *c)
{
  (*addnumt[MAX(a?NID(a):0,b?NID(b):0)])(a,b,c);
}

void subnum(VL vl,Num a,Num b,Num *c)
{
  (*subnumt[MAX(a?NID(a):0,b?NID(b):0)])(a,b,c);
}

void mulnum(VL vl,Num a,Num b,Num *c)
{
  (*mulnumt[MAX(a?NID(a):0,b?NID(b):0)])(a,b,c);
}

void divnum(VL vl,Num a,Num b,Num *c)
{
  (*divnumt[MAX(a?NID(a):0,b?NID(b):0)])(a,b,c);
}

void pwrnum(VL vl,Num a,Num b,Num *c)
{
  (*pwrnumt[MAX(a?NID(a):0,b?NID(b):0)])(a,b,c);
}

void chsgnnum(Num a,Num *c)
{
  (*chsgnnumt[a?NID(a):0])(a,c);
}

int compnum(VL vl,Num a,Num b)
{
  return (*cmpnumt[MAX(a?NID(a):0,b?NID(b):0)])(a,b);
}

extern int current_ff;

void one_ff(Num *c)
{
  LM lm;
  GFS fs;
  GFSN fspn;

  switch ( current_ff ) {
    case FF_GFP:
    case FF_GFPN:
      *c = (Num)ONELM;
      break;
    case FF_GFS:
      mqtogfs(ONEM,&fs);
      *c = (Num)fs;
      break;
    case FF_GFSN:
      ntogfsn((Obj)ONE,&fspn);
      *c = (Num)fspn;
      break;
    default:
      error("one_ff : not implemented yet");
  }
}

int negative_number(Num c)
{
  if ( !c )
    return 0;
  else {
    switch ( NID(c) ) {
      case N_Q:
        return sgnq((Q)c)<0;
      case N_R:
        return BDY((Real)c)<0;
      case N_B:
        return 0; /* XXX */
      default:
        return 0;
    }
  }
}

/* general simplifier for finite field elements */
/* XXX : we don't have appropriate file */

void simp_ff(Obj p,Obj *rp)
{
  LM s;
  DCP dc,dcr0,dcr;
  GF2N rg,sg;
  GFPN rpn,spn;
  GFS rs;
  GFSN rspn,sspn;
  P t;
  Obj obj;

  lm_lazy = 0;
  if ( !p )
    *rp = 0;
  else if ( OID(p) == O_N ) {
    switch ( current_ff ) {
      case FF_GFP:
        ptolmp((P)p,&t); simplm((LM)t,&s); *rp = (Obj)s;
        break;
      case FF_GF2N:
        ptogf2n((Obj)p,&rg); simpgf2n((GF2N)rg,&sg); *rp = (Obj)sg;
        break;
      case FF_GFPN:
        ntogfpn((Obj)p,&rpn); simpgfpn((GFPN)rpn,&spn); *rp = (Obj)spn;
        break;
      case FF_GFS:
        if ( NID((Num)p) == N_GFS )
          *rp = p;
        else {
          ptomp(current_gfs_p,(P)p,&t); mqtogfs((MQ)t,&rs);
          *rp = (Obj)rs;
        } 
        break;
      case FF_GFSN:
        ntogfsn((Obj)p,&rspn); simpgfsn((GFSN)rspn,&sspn);
        *rp = (Obj)sspn;
        break;
      default:
        *rp = (Obj)p;
        break;
    }
  } else if ( OID(p) == O_P ) {
    for ( dc = DC((P)p), dcr0 = 0; dc; dc = NEXT(dc) ) {
      simp_ff((Obj)COEF(dc),&obj);
      if ( obj ) {
        NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = (P)obj;
      }
    }
    if ( !dcr0 ) 
      *rp = 0;
    else {
      NEXT(dcr) = 0; MKP(VR((P)p),dcr0,t); *rp = (Obj)t;
    }
  } else
    error("simp_ff : not implemented yet");
}


void field_order_ff(Z *order)
{
  UP2 up2;
  UP up;
  UM dp;
  Z m,e;
  mpz_t z;
  int d,w;

  switch ( current_ff ) {
    case FF_GFP:
      getmod_lm(order); break;
    case FF_GF2N:
      getmod_gf2n(&up2); d = degup2(up2);
      mpz_init(z);
      mpz_mul_2exp(z,BDY(ONE),d);
      MPZTOZ(z,*order);
      break;
    case FF_GFPN:
      getmod_lm(&m);
      getmod_gfpn(&up);
      STOQ(up->d,e);
      pwrz(m,e,order);
      break;
    case FF_GFS:
      STOQ(current_gfs_q,*order); break;
    case FF_GFSN:
      STOQ(current_gfs_q,m);
      getmod_gfsn(&dp);
      STOQ(DEG(dp),e);
      pwrz(m,e,order);
      break;
    default:
      error("field_order_ff : current_ff is not set");
  }
}
