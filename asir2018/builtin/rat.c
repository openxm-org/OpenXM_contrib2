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
#include "parse.h"

void Pnm(NODE arg,Obj *rp);
void Pdn(NODE arg,Obj *rp);
void Pderiv(NODE arg,Obj *rp);
void Prderiv(NODE arg,Obj *rp);
void Pederiv(NODE arg,Obj *rp);
void Plaurent_to_rat(NODE arg,Obj *rp);

struct ftab rat_tab[] = {
  {"nm",Pnm,1},
  {"dn",Pdn,1},
  {"diff",Pderiv,-99999999},
  {"ediff",Pederiv,-99999999},
  {"rdiff",Prderiv,-99999999},
  {"laurent_to_rat",Plaurent_to_rat,1},
  {0,0,0},
};

void Pnm(NODE arg,Obj *rp)
{
  Obj t;

  asir_assert(ARG0(arg),O_R,"nm");
  if ( !(t = (Obj)ARG0(arg)) )
    *rp = 0;
  else
    switch ( OID(t) ) {
      case O_N: 
        switch ( NID(t) ) {
          case N_Q:
            nmq((Q)t,(Z *)rp);
            break;
          default:
            *rp = t; break;
        }
        break;
      case O_P:
        *rp = t; break;
      case O_R:
        *rp = (Obj)NM((R)t); break;
      default:
        *rp = 0;
    }
}

void Pdn(NODE arg,Obj *rp)
{
  Obj t;

  asir_assert(ARG0(arg),O_R,"dn");
  if ( !(t = (Obj)ARG0(arg)) )
    *rp = (Obj)ONE;
  else
    switch ( OID(t) ) {
      case O_N: 
        switch ( NID(t) ) {
          case N_Q:
            dnq((Q)t,(Z *)rp);
            break;
          default:
            *rp = (Obj)ONE; break;
        }
        break;
      case O_P:
        *rp = (Obj)ONE; break;
      case O_R:
        *rp = (Obj)DN((R)t); break;
      default:
        *rp = 0;
    }
}

void Pderiv(NODE arg,Obj *rp)
{
  Obj a,t;
  LIST l;
  P v;
  NODE n;

  if ( !arg ) {
    *rp = 0; return;
  }
  asir_assert(ARG0(arg),O_R,"diff");
  reductr(CO,(Obj)ARG0(arg),&a);
  n = NEXT(arg);
  if ( n && (l = (LIST)ARG0(n)) && OID(l) == O_LIST )
    n = BDY(l);
  for ( ; n; n = NEXT(n) ) {
    if ( !(v = (P)BDY(n)) || OID(v) != O_P )
      error("diff : invalid argument");
    derivr(CO,a,VR(v),&t); a = t;
  }
  *rp = a;
}

/* simple derivation with reduction */
void Prderiv(NODE arg,Obj *rp)
{
  Obj a,t;
  LIST l;
  P v;
  NODE n;

  if ( !arg ) {
    *rp = 0; return;
  }
  asir_assert(ARG0(arg),O_R,"rdiff");
  a = (Obj)ARG0(arg);
  n = NEXT(arg);
  if ( n && (l = (LIST)ARG0(n)) && OID(l) == O_LIST )
    n = BDY(l);
  for ( ; n; n = NEXT(n) ) {
    if ( !(v = (P)BDY(n)) || OID(v) != O_P )
      error("rdiff : invalid argument");
    simple_derivr(CO,a,VR(v),&t); a = t;
  }
  *rp = a;
}

/* Euler derivation */
void Pederiv(NODE arg,Obj *rp)
{
  Obj a,t;
  LIST l;
  P v;
  NODE n;

  if ( !arg ) {
    *rp = 0; return;
  }
  asir_assert(ARG0(arg),O_P,"ediff");
  reductr(CO,(Obj)ARG0(arg),&a);
  n = NEXT(arg);
  if ( n && (l = (LIST)ARG0(n)) && OID(l) == O_LIST )
    n = BDY(l);
  for ( ; n; n = NEXT(n) ) {
    if ( !(v = (P)BDY(n)) || OID(v) != O_P )
      error("diff : invalid argument");
    ediffp(CO,(P)a,VR(v),(P *)&t); a = t;
  }
  *rp = a;
}

Obj laurent_to_rat(P p)
{
  VL vl,tvl;
  P dn,pdn,v,vm,t;
  R r;
  Z m,d;

  if ( p == 0 || NUM(p) ) return (Obj)p;
  clctv(CO,p,&vl);
  dn = (P)ONE;
  for ( tvl = vl; tvl; tvl = NEXT(tvl) ) {
    getmindeg(tvl->v,p,&d);
    if ( cmpz(d,0) < 0 ) {
      MKV(tvl->v,v);
      chsgnz(d,&m);
      pwrp(CO,v,m,&vm);
      mulp(CO,dn,vm,&t); dn = t;
    }
  }
  mulp(CO,p,dn,&pdn);
  MKRAT(pdn,dn,1,r);
  return (Obj)r;
}

void Plaurent_to_rat(NODE arg,Obj *rp)
{
  R a;
  Obj nm,dn,r;

  asir_assert(ARG0(arg),O_R,"laurent_to_rat");
  a = (R)ARG0(arg);
  if ( !a || NUM(a) ) *rp = (Obj)a;
  else if ( POLY(a) ) {
    *rp = laurent_to_rat((P)a);
  } else {
    nm = laurent_to_rat(a->nm);
    dn = laurent_to_rat(a->dn);
    divr(CO,nm,dn,&r);
    reductr(CO,r,rp);
  }
}
