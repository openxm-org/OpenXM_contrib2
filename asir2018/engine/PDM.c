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
 * $OpenXM: OpenXM_contrib2/asir2018/engine/PDM.c,v 1.1 2018/09/19 05:45:07 noro Exp $
*/
#ifndef MODULAR
#define MODULAR
#endif

#include "b.h"
#include "ca.h"

#include "polydiv.c"

int divtmp(VL vl,int mod,P p1,P p2,P *q)
{
  register int i,j;
  register DCP dc1,dc2,dc;
  P m,m1,s,dvr,t;
  P *pq,*pr,*pd;
  V v1,v2;
  Z deg1,deg2;
  int d1,d2,sgn;

  if ( !p1 ) {
    *q = 0;
    return 1;
  } else if ( NUM(p2) ) {
    divsmp(vl,mod,p1,p2,q);
    return 1;
  } else if ( NUM(p1) ) {
    *q = 0;
    return 0;
  } else if ( ( v1 = VR(p1) ) == ( v2 = VR(p2) ) ) {
    dc1 = DC(p1); dc2 = DC(p2);
    deg1 = DEG(dc1); deg2 = DEG(dc2);
    sgn = cmpz(deg1,deg2);
    if ( sgn == 0 )
      if ( !divtmp(vl,mod,COEF(dc1),COEF(dc2),&m) ) {
        *q = 0;
        return 0;
      } else {
        mulmp(vl,mod,p2,m,&m1); submp(vl,mod,p1,m1,&s); 
        if ( !s ) {
          *q = m;
          return 1;
        } else {
          *q = 0;
          return 0;
        }
      }  
    else if ( sgn < 0 ) {
      *q = 0;
      return 0;
    } else {
      if ( !smallz(deg1) ) {
        error("divtmp : invalid input");
        *q = 0;
        return ( 0 );
      }
      d1 = ZTOS(deg1); d2 = ZTOS(deg2); 
      W_CALLOC(d1-d2,P,pq); W_CALLOC(d1,P,pr); W_CALLOC(d2,P,pd);
      for ( dc = dc1; dc; dc = NEXT(dc) ) 
        pr[ZTOS(DEG(dc))] = COEF(dc);
      for ( dc = dc2; dc; dc = NEXT(dc) ) 
        pd[ZTOS(DEG(dc))] = COEF(dc);
      for ( dvr = COEF(dc2), i = d1 - d2; i >= 0; i-- ) 
        if ( !pr[i+d2] ) 
          continue;
        else if ( !divtmp(vl,mod,pr[i+d2],dvr,&m) ) {
          *q = 0;
          return 0;
        } else {
          pq[i] = m;
          for ( j = d2; j >= 0; j-- ) {
            mulmp(vl,mod,pq[i],pd[j],&m);
            submp(vl,mod,pr[i + j],m,&s); pr[i + j] = s;
          }
        }
      plisttop(pq,v1,d1 - d2,&m); plisttop(pr,v1,d1 - 1,&t);
      if ( t ) {
        *q = 0;
        return 0;
      } else {
        *q = m;
        return 1;
      }
    }
  } else {
    for ( ; (v1 != vl->v) && (v2 != vl->v); vl = NEXT(vl) );
    if ( v2 == vl->v ) {
      *q = 0;
      return 0;
    } else 
      return divtdcmp(vl,mod,p1,p2,q);
  }
}

int divtdcmp(VL vl,int mod,P p1,P p2,P *q)
{

  P m;
  register DCP dc,dcr,dcr0;

  for ( dc = DC(p1), dcr0 = 0; dc; dc = NEXT(dc) ) 
    if ( !divtmp(vl,mod,COEF(dc),p2,&m) ) {
      *q = 0;
      return 0;
    } else {
      NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = m; NEXT(dcr) = 0;
    }
  MKP(VR(p1),dcr0,*q);
  return 1;
}
