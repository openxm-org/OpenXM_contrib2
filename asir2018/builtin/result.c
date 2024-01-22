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
 * $OpenXM: OpenXM_contrib2/asir2018/builtin/result.c,v 1.1 2018/09/19 05:45:06 noro Exp $
*/
#include "ca.h"
#include "parse.h"

void Pnd_res(NODE arg,P *rp);
void Presult(NODE arg,P *rp);
void Psrcr(NODE arg,P *rp);

struct ftab result_tab[] = {
  {"res",Presult,-4},
  {"nd_res",Pnd_res,-4},
  {"srcr",Psrcr,3},
  {0,0,0},
};

void Pnd_res(NODE arg,P *rp)
{
  int mod,d1,d2,i,j;
  Z q1,q2;
  P *a1,*a2;
  P p1,p2,r1,r2,r;
  V v;
  MAT m;
  DCP dc;
  VL vl;

  asir_assert(ARG0(arg),O_P,"nd_res");
  asir_assert(ARG1(arg),O_P,"nd_res");
  asir_assert(ARG2(arg),O_P,"nd_res");
  v = VR((P)ARG0(arg)); p1 = ARG1(arg); p2 = ARG2(arg);
  d1 = getdeg(v,p1);
  d2 = getdeg(v,p2);
  if ( d1 == 0 ) {
    if ( d2 == 0 )
      *rp = (P)ONE;
    else {
      STOZ(d2,q2);
      pwrp(CO,p1,q2,rp);
    }
    return;
  } else if ( d2 == 0 ) {
    STOZ(d1,q1);
    pwrp(CO,p2,q1,rp);
    return;
  } 
  MKMAT(m,d1+d2,d1+d2);
  reordvar(CO,v,&vl);
  reorderp(vl,CO,p1,&r1);
  reorderp(vl,CO,p2,&r2);
  a1 = ALLOCA((d1+1)*sizeof(P));
  a2 = ALLOCA((d2+1)*sizeof(P));
  for ( i = 0; i <= d1; i++ ) a1[i] = 0;
  for ( i = 0; i <= d2; i++ ) a2[i] = 0;
  for ( dc = DC(r1); dc; dc = NEXT(dc) ) a1[d1-ZTOS(DEG(dc))] = COEF(dc);
  for ( dc = DC(r2); dc; dc = NEXT(dc) ) a2[d2-ZTOS(DEG(dc))] = COEF(dc);
  for ( i = 0; i < d2; i++ )
    for ( j = 0; j <= d1; j++ )
      m->body[i][i+j] = a1[j];
  for ( i = 0; i < d1; i++ )
    for ( j = 0; j <= d2; j++ )
      m->body[i+d2][i+j] = a2[j];
  nd_det(argc(arg)==3?0:ZTOS((Q)ARG3(arg)),m,rp);
}

void Presult(NODE arg,P *rp)
{
  int mod;
  P p1,p2,r;

  asir_assert(ARG0(arg),O_P,"res");
  asir_assert(ARG1(arg),O_P,"res");
  asir_assert(ARG2(arg),O_P,"res");
  if ( argc(arg) == 3 )
    resultp(CO,VR((P)ARG0(arg)),(P)ARG1(arg),(P)ARG2(arg),rp);
  else {
    mod = ZTOS((Q)ARG3(arg));
    ptomp(mod,(P)ARG1(arg),&p1); ptomp(mod,(P)ARG2(arg),&p2);
    resultmp(CO,mod,VR((P)ARG0(arg)),p1,p2,&r);
    mptop(r,rp);
  }
}

void Psrcr(NODE arg,P *rp)
{
  asir_assert(ARG0(arg),O_P,"srcr");
  asir_assert(ARG1(arg),O_P,"srcr");
  asir_assert(ARG2(arg),O_P,"srcr");
  res_ch_det(CO,VR((P)ARG0(arg)),(P)ARG1(arg),(P)ARG2(arg),rp);
}
