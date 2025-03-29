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

void Pred(NODE arg,Obj *rp);
void Predc(NODE arg,P *rp);
void Pprim(NODE arg,P *rp);

struct ftab reduct_tab[] = {
  {"red",Pred,1},
  {"redc",Predc,2},
  {"prim",Pprim,-2},
  {0,0,0},
};

void Pred(NODE arg,Obj *rp)
{
  Obj a,b,t;
  LIST l;
  V v;
  int row,col,len;
  VECT vect;
  MAT mat;
  int i,j;
  NODE n0,n,nd;
  struct oNODE arg0;
  MP m,mp,mp0;
  DP d;
  DMM dm,dm0,dm1;
  DPM dd;

  a = (Obj)ARG0(arg);
  if ( !a ) {
    *rp = 0;
    return;
  }
  switch ( OID(a) ) {
    case O_N: case O_P: case O_R:
      reductr(CO,(Obj)ARG0(arg),rp);
      break;
    case O_LIST:
      n0 = 0;
      for ( nd = BDY((LIST)a); nd; nd = NEXT(nd) ) {
        NEXTNODE(n0,n);
        arg0.body = (pointer)BDY(nd);
        arg0.next = 0;
        Pred(&arg0,&b);
        BDY(n) = (pointer)b;
      }
      if ( n0 )
        NEXT(n) = 0;
      MKLIST(l,n0);
      *rp = (Obj)l;
      break;
    case O_VECT:
      len = ((VECT)a)->len;
      MKVECT(vect,len);
      for ( i = 0; i < len; i++ ) {
        arg0.body = (pointer)BDY((VECT)a)[i];
        arg0.next = 0;
        Pred(&arg0,&b);
        BDY(vect)[i] = (pointer)b;
      }
      *rp = (Obj)vect;
      break;
    case O_MAT:
      row = ((MAT)a)->row;
      col = ((MAT)a)->col;
      MKMAT(mat,row,col);
      for ( i = 0; i < row; i++ )
        for ( j = 0; j < col; j++ ) {
          arg0.body = (pointer)BDY((MAT)a)[i][j];
          arg0.next = 0;
          Pred(&arg0,&b);
          BDY(mat)[i][j] = (pointer)b;
        }
      *rp = (Obj)mat;
      break;
    case O_DP:
      mp0 = 0;
      for ( m = BDY((DP)a); m; m = NEXT(m) ) {
        arg0.body = (pointer)C(m);
        arg0.next = 0;
        Pred(&arg0,&b);
        if ( b ) {
          NEXTMP(mp0,mp);
          C(mp) = b;
          mp->dl = m->dl;
        }
      }
      if ( mp0 ) {
        MKDP(NV((DP)a),mp0,d);
        d->sugar = ((DP)a)->sugar;
        *rp = (Obj)d;
      } else
        *rp = 0;

      break;
    case O_DPM:
      dm0 = 0;
      for ( dm1 = BDY((DPM)a); dm1; dm1 = NEXT(dm1) ) {
        arg0.body = (pointer)C(dm1);
        arg0.next = 0;
        Pred(&arg0,&b);
        if ( b ) {
          NEXTDMM(dm0,dm);
          C(dm) = b;
          dm->dl = dm1->dl;
          dm->pos = dm1->pos;
        }
      }
      if ( dm0 ) {
        MKDPM(NV((DPM)a),dm0,dd);
        dd->sugar = ((DPM)a)->sugar;
        *rp = (Obj)dd;
      } else
        *rp = 0;

      break;
    default:
      error("red : invalid argument");
  }
}

void Predc(NODE arg,P *rp)
{
  asir_assert(ARG0(arg),O_P,"redc");
  asir_assert(ARG1(arg),O_P,"redc");
  remsdcp(CO,(P)ARG0(arg),(P)ARG1(arg),rp);
}

void Pprim(NODE arg,P *rp)
{
  P t,p,p1,r;
  V v;
  VL vl;

  asir_assert(ARG0(arg),O_P,"prim");
  p = (P)ARG0(arg);
  if ( NUM(p) )
    *rp = (P)ONE;
  else {
    if ( argc(arg) == 2 ) {
      v = VR((P)ARG1(arg));
      change_mvar(CO,p,v,&p1);
      if ( VR(p1) != v ) {
        *rp = (P)ONE; return;
      } else {
        reordvar(CO,v,&vl); pcp(vl,p1,&r,&t);
        restore_mvar(CO,r,v,rp);
      }
    } else
      pcp(CO,p,rp,&t);
  }
}
