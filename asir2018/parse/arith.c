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
 * $OpenXM: OpenXM_contrib2/asir2018/parse/arith.c,v 1.1 2018/09/19 05:45:08 noro Exp $
*/
#include "ca.h"
#include "parse.h"

typedef void (*AF)(VL,Obj,Obj,Obj *);
typedef void (*AF2)(Obj,Obj *);
typedef int (*AFI)(VL,Obj,Obj);

int compvoid(VL vl,Obj a,Obj b);

struct oAFUNC {
  AF add, sub,mul,div,pwr;
  AF2 chsgn;
  AFI comp;
};

struct oARF arf[6];
ARF addfs, subfs, mulfs, divfs, remfs, pwrfs;


struct oAFUNC afunc0[] = {
/* O_VOID=-1 */ {(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF2)notdef2,(AFI)compvoid},
/* ???=0 */  {(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF2)notdef2,(AFI)notdefi},
/* O_N=1 */  {(AF)addnum,(AF)subnum,(AF)mulnum,(AF)divnum,(AF)pwrnum,(AF2)chsgnnum,(AFI)compnum},
/* O_P=2 */  {(AF)addp,(AF)subp,(AF)mulp,(AF)divr,(AF)pwrp,(AF2)chsgnp,(AFI)compp},
/* O_R=3 */  {(AF)addr,(AF)subr,(AF)mulr,(AF)divr,(AF)pwrr,(AF2)chsgnr,(AFI)compr},
/* O_LIST=4 */  {(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF2)notdef2,(AFI)complist},
/* O_VECT=5 */  {(AF)addvect,(AF)subvect,(AF)mulvect,(AF)divvect,(AF)notdef,(AF2)chsgnvect,(AFI)compvect},
/* O_MAT=6 */  {(AF)addmat,(AF)submat,(AF)mulmat,(AF)divmat,(AF)pwrmat,(AF2)chsgnmat,(AFI)compmat},
/* O_STR=7 */  {(AF)addstr,(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF2)notdef2,(AFI)compstr},
/* O_COMP=8 */  {(AF)addcomp,(AF)subcomp,(AF)mulcomp,(AF)divcomp,(AF)pwrcomp,(AF2)chsgncomp,(AFI)compcomp},
/* O_DP=9 */   {(AF)addd,(AF)subd,(AF)muld,(AF)divdc,(AF)notdef,(AF2)chsgnd,(AFI)compd},
/* O_USINT=10 */  {(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF2)notdef2,(AFI)compui},
/* O_ERR=11 */  {(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF2)notdef2,(AFI)notdefi},
/* O_GF2MAT=12 */  {(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF2)notdef2,(AFI)notdefi},
/* O_MATHCAP=13 */  {(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF2)notdef2,(AFI)notdefi},
/* O_F=14 */  {(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF2)notdef2,(AFI)notdefi},
/* O_GFMMAT=15 */  {(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF2)notdef2,(AFI)notdefi},
/* O_BYTEARRAY=16 */  {(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF2)notdef2,(AFI)compbytearray},
/* O_QUOTE=17 */  {(AF)addquote,(AF)subquote,(AF)mulquote,(AF)divquote,(AF)pwrquote,(AF2)chsgnquote,(AFI)compquote},
/* O_OPTLIST=18 */  {(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF2)notdef2,(AFI)notdefi},
/* O_SYMBOL=19 */  {(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF2)notdef2,(AFI)notdefi},
/* O_RANGE=20 */  {(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF2)notdef2,(AFI)notdefi},
/* O_TB=21 */  {(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF2)notdef2,(AFI)notdefi},
/* O_DPV=22 */  {(AF)adddv,(AF)subdv,(AF)muldv,(AF)notdef,(AF)notdef,(AF2)chsgndv,(AFI)compdv},
/* O_QUOTEARG=23 */  {(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF)notdef,(AF2)notdef2,(AFI)compqa},
/* O_MAT=24 */  {(AF)AddMatI,(AF)SubMatI,(AF)MulMatG,(AF)notdef,(AF)notdef,(AF2)ChsgnI,(AFI)notdefi},
/* O_NBP=25 */  {(AF)addnbp,(AF)subnbp,(AF)mulnbp,(AF)notdef,(AF)pwrnbp,(AF2)chsgnnbp,(AFI)compnbp},
/* O_DPM=26 */  {(AF)adddpm,(AF)subdpm,(AF)mulobjdpm,(AF)notdef,(AF)notdef,(AF2)chsgndpm,(AFI)compdpm},
};

struct oAFUNC *afunc = &afunc0[1];

void arf_init() {
  addfs = &arf[0]; addfs->name = "+"; addfs->fp = arf_add;
  subfs = &arf[1]; subfs->name = "-"; subfs->fp = arf_sub;
  mulfs = &arf[2]; mulfs->name = "*"; mulfs->fp = arf_mul;
  divfs = &arf[3]; divfs->name = "/"; divfs->fp = arf_div;
  remfs = &arf[4]; remfs->name = "%"; remfs->fp = arf_remain;
  pwrfs = &arf[5]; pwrfs->name = "^"; pwrfs->fp = arf_pwr;
}

void arf_add(VL vl,Obj a,Obj b,Obj *r)
{
  int mid;

  if ( !a )
    *r = b;
  else if ( !b )
    *r = a;
  else if ( OID(a) == OID(b) )
    (*afunc[OID(a)].add)(vl,a,b,r);
  else if ( (mid = MAX(OID(a),OID(b))) <= O_R || mid == O_QUOTE )
    (*afunc[mid].add)(vl,a,b,r);
  else if ( (mid = MAX(OID(a),OID(b))) == O_DP && MIN(OID(a),OID(b)) <= O_R )
    (*afunc[mid].add)(vl,a,b,r);
  else
    notdef(vl,a,b,r);
}

void arf_sub(VL vl,Obj a,Obj b,Obj *r)
{
  int mid;

  if ( !a )
    if ( !b )
      *r = 0;
    else
      (*afunc[OID(b)].chsgn)(b,r);
  else if ( !b )
    *r = a;
  else if ( OID(a) == OID(b) )
    (*afunc[OID(a)].sub)(vl,a,b,r);
  else if ( (mid = MAX(OID(a),OID(b))) <= O_R || mid == O_QUOTE )
    (*afunc[mid].sub)(vl,a,b,r);
  else if ( (mid = MAX(OID(a),OID(b))) == O_DP && MIN(OID(a),OID(b)) <= O_R )
    (*afunc[mid].sub)(vl,a,b,r);
  else
    notdef(vl,a,b,r);
}

void arf_mul(VL vl,Obj a,Obj b,Obj *r)
{
  int mid,aid,bid;

  if ( !a && !b )
    *r = 0;
  else if ( !a || !b ) {
    if ( !a ) a = b;
    /* compute a*0 */
    if ( OID(a) == O_MAT || OID(a) == O_VECT )
      (*(afunc[O_MAT].mul))(vl,a,0,r);
    else if ( OID(a) == O_IMAT )
      (*(afunc[O_IMAT].mul))(vl,0,a,r);
    else
      *r = 0;
  } else if ( (aid = OID(a)) == (bid = OID(b)) )
    (*(afunc[aid].mul))(vl,a,b,r);
  else if ( (mid = MAX(aid,bid)) <= O_R || mid == O_QUOTE )
    (*afunc[mid].mul)(vl,a,b,r);
  else {
    switch ( aid ) {
      case O_N: case O_P: case O_NBP:
        (*afunc[mid].mul)(vl,a,b,r);
        break;
      case O_R:
        /* rat * something; bid > O_R */
        if ( bid == O_VECT || bid == O_MAT || bid == O_DP || bid == O_DPM )
          (*afunc[mid].mul)(vl,a,b,r);
        else 
          notdef(vl,a,b,r);
        break;
      case O_MAT:
        if ( bid <= O_R || bid == O_VECT || bid == O_DP )
          (*afunc[O_MAT].mul)(vl,a,b,r);
        else
          notdef(vl,a,b,r);
        break;
      case O_VECT:
        if ( bid <= O_R || bid == O_DP )
          (*afunc[O_VECT].mul)(vl,a,b,r);
        else if ( bid == O_MAT )
          (*afunc[O_MAT].mul)(vl,a,b,r);
        else
          notdef(vl,a,b,r);
        break;
      case O_DP:
        if ( bid <= O_R )
          (*afunc[O_DP].mul)(vl,a,b,r);
        else if ( bid == O_MAT || bid == O_VECT || bid == O_DPV || bid == O_DPM )
          (*afunc[bid].mul)(vl,a,b,r);
        else
          notdef(vl,a,b,r);
        break;
      case O_DPM:
        if ( bid <= O_R || bid == O_DP )
          (*afunc[O_DPM].mul)(vl,b,a,r);
        else
          notdef(vl,a,b,r);
        break;
      case O_IMAT:
        (*afunc[O_IMAT].mul)(vl,a,b,r);
        break;
      default:
          notdef(vl,a,b,r);
        break;
    }
  }
}

void arf_div(VL vl,Obj a,Obj b,Obj *r)
{
  int mid;

  if ( !b )
    error("div : division by 0");
  if ( !a )
    *r = 0;
  else if ( OID(a) == OID(b) )
    (*(afunc[OID(a)].div))(vl,a,b,r);
  else if ( (mid = MAX(OID(a),OID(b))) <= O_R || 
    (mid == O_MAT) || (mid == O_VECT) || (mid == O_DP) || mid == O_QUOTE )
    (*afunc[mid].div)(vl,a,b,r);
  else
    notdef(vl,a,b,r);
}

void arf_remain(VL vl,Obj a,Obj b,Obj *r)
{
  if ( !b )
    error("rem : division by 0");
  else if ( !a )
    *r = 0;
  else if ( MAX(OID(a),OID(b)) <= O_P && INT(b) )
    cmp((Z)b,(P)a,(P *)r);
  else
    notdef(vl,a,b,r);
}

int allow_laurent;

void arf_pwr(VL vl,Obj a,Obj e,Obj *r)
{
  R t;

  if ( !a ) {
    if ( !e )
      *r = (pointer)ONE;
    else if ( RATN(e) && sgnq((Q)e)>0 )
      *r = 0;
    else
      mkpow(vl,a,e,r);
  } else if ( !e ) {
    if ( OID(a) == O_MAT )
      (*(afunc[O_MAT].pwr))(vl,a,e,r);
    else  
      *r = (pointer)ONE;
  } else if ( OID(a) == O_QUOTE || OID(e) == O_QUOTE )
    (*(afunc[O_QUOTE].pwr))(vl,a,e,r);
  else if ( (OID(e) <= O_N) && INT(e) ) {
    if ( (OID(a) == O_P) && (sgnq((Q)e) < 0) ) {
      if ( allow_laurent )
        (*(afunc[O_P].pwr))(vl,a,e,r);
      else {
        MKRAT((P)a,(P)ONE,1,t);
        (*(afunc[O_R].pwr))(vl,(Obj)t,e,r);
      }
    } else
      (*(afunc[OID(a)].pwr))(vl,a,e,r);
  } else if ( (OID(e) <= O_N) && RATN(e) ) {
    if ( (OID(a) == O_P) && allow_laurent )
      (*(afunc[O_P].pwr))(vl,a,e,r);
    else if ( OID(a) <= O_R )
      mkpow(vl,a,e,r);
    else
      notdef(vl,a,e,r);
  } else if ( OID(a) <= O_R )
    mkpow(vl,a,e,r);
  else
    notdef(vl,a,e,r);
}

void arf_chsgn(Obj a,Obj *r)
{
  if ( !a )
    *r = 0;
  else
    (*(afunc[OID(a)].chsgn))(a,r);
}

int arf_comp(VL vl,Obj a,Obj b)
{
  if ( !a )
    if ( !b )
      return 0;
    else
      return (*afunc[OID(b)].comp)(vl,a,b);
  else if ( !b )
      return (*afunc[OID(a)].comp)(vl,a,b);
  else if ( OID(a) != OID(b) )
    return OID(a)>OID(b) ? 1 : -1;
  else
    return (*afunc[OID(a)].comp)(vl,a,b);
}

int complist(VL vl,LIST a,LIST b)
{
  int t;
  NODE an,bn;

  if ( !a )
    if ( !b )
      return 0;
    else
      return -1;
  else if ( !b )
    return 1;
  for ( an = BDY(a), bn = BDY(b); an && bn; an = NEXT(an), bn = NEXT(bn) );
  if ( an && !bn )
    return 1;
  else if ( !an && bn )
    return -1;
  for ( an = BDY(a), bn = BDY(b); an; an = NEXT(an), bn = NEXT(bn) )
    if ( ( t = arf_comp(vl,BDY(an),BDY(bn)) ) != 0 )
      return t;
  return 0;
}

int compvoid(VL vl,Obj a,Obj b)
{
  if ( !a )
    if ( !b ) return 0;
    else return -1;
  else if ( !b )
    return 1;
  else if ( OID(a) > OID(b) ) return 1;
  else if ( OID(b) < OID(a) ) return -1;
  else return 0;
}
