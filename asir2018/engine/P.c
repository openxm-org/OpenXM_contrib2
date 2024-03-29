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
 * $OpenXM: OpenXM_contrib2/asir2018/engine/P.c,v 1.1 2018/09/19 05:45:07 noro Exp $
*/
#ifndef FBASE
#define FBASE
#endif

#include "b.h"
#include "ca.h"

#include "poly.c"

void divcp(P f,Q q,P *rp)
{
  DCP dc,dcr,dcr0;

  if ( !f ) 
    *rp = 0;
  else if ( NUM(f) )
    DIVNUM((Num)f,(Num)q,(Num *)rp);
  else {
    for ( dc = DC(f), dcr0 = 0; dc; dc = NEXT(dc) ) {
      NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc);
      divcp(COEF(dc),q,&COEF(dcr));
    }
    NEXT(dcr) = 0; MKP(VR(f),dcr0,*rp);
  }
}

void diffp(VL vl,P p,V v,P *r)
{
  P t;
  DCP dc,dcr,dcr0;

  if ( !p || NUM(p) ) 
    *r = 0;
  else {
    if ( v == VR(p) ) {
      for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
        if ( !DEG(dc) ) continue;
        MULPQ(COEF(dc),(P)DEG(dc),&t);
        if ( t ) {
          NEXTDC(dcr0,dcr); subz(DEG(dc),ONE,&DEG(dcr));
          COEF(dcr) = t;
        }
      }
      if ( !dcr0 )
        *r = 0;
      else {
        NEXT(dcr) = 0; MKP(v,dcr0,*r);
      }
    } else {
      for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
        diffp(vl,COEF(dc),v,&t);
        if ( t ) {
          NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
        }
      }
      if ( !dcr0 )
        *r = 0;
      else {
        NEXT(dcr) = 0; MKP(VR(p),dcr0,*r);
      }
    }
  }
}

/* Euler derivation */
void ediffp(VL vl,P p,V v,P *r)
{
  P t;
  DCP dc,dcr,dcr0;

  if ( !p || NUM(p) ) 
    *r = 0;
  else {
    if ( v == VR(p) ) {
      for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
        if ( !DEG(dc) ) continue;
        MULPQ(COEF(dc),(P)DEG(dc),&t);
        if ( t ) {
          NEXTDC(dcr0,dcr);
          DEG(dcr) = DEG(dc);
          COEF(dcr) = t;
        }
      }
      if ( !dcr0 )
        *r = 0;
      else {
        NEXT(dcr) = 0; MKP(v,dcr0,*r);
      }
    } else {
      for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
        ediffp(vl,COEF(dc),v,&t);
        if ( t ) {
          NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
        }
      }
      if ( !dcr0 )
        *r = 0;
      else {
        NEXT(dcr) = 0; MKP(VR(p),dcr0,*r);
      }
    }
  }
}

void coefp(P p,int d,P *pr)
{
  DCP dc;
  int sgn;
  Z dq;

  if ( NUM(p) )
    if ( d == 0 ) 
      *pr = p;
    else 
      *pr = 0;
  else {
    for ( STOZ(d,dq), dc = DC(p); dc; dc = NEXT(dc) ) 
      if ( (sgn = cmpz(DEG(dc),dq)) > 0 )
        continue;
      else if ( sgn == 0 ) {
        *pr = COEF(dc);
        return;
      } else {
        *pr = 0;
        break;  
      }
    *pr = 0;
  }
}

int compp(VL vl,P p1,P p2)
{
  DCP dc1,dc2;
  V v1,v2;

  if ( !p1 )
    return p2 ? -1 : 0;
  else if ( !p2 )
    return 1;
  else if ( NUM(p1) )
    return NUM(p2) ? (*cmpnumt[MAX(NID(p1),NID(p2))])((Num)p1,(Num)p2) : -1;
  else if ( NUM(p2) )
    return 1;
  if ( (v1 = VR(p1)) == (v2 = VR(p2)) ) {
    for ( dc1 = DC(p1), dc2 = DC(p2); 
      dc1 && dc2; dc1 = NEXT(dc1), dc2 = NEXT(dc2) )
      switch ( cmpz(DEG(dc1),DEG(dc2)) ) {
        case 1:
          return 1; break;
        case -1:
          return -1; break;
        default:
          switch ( compp(vl,COEF(dc1),COEF(dc2)) ) {
            case 1:
              return 1; break;
            case -1:
              return -1; break;
            default:
              break;
          }
          break;
      }
    return dc1 ? 1 : (dc2 ? -1 : 0);
  } else {
    for ( ; v1 != VR(vl) && v2 != VR(vl); vl = NEXT(vl) );
    return v1 == VR(vl) ? 1 : -1;
  }
}

int equalp(VL vl,P p1,P p2)
{
  DCP dc1,dc2;
  V v1,v2;

  if ( !p1 ) {
    if ( !p2 ) return 1;
    else return 0;
  }
  /* p1 != 0 */
  if ( !p2 ) return 0;

  /* p1 != 0, p2 != 0 */
  if ( NUM(p1) ) {
    if ( !NUM(p2) ) return 0;
    /* p1 and p2 are numbers */
    if ( NID((Num)p1) != NID((Num)p2) ) return 0;
    if ( !(*cmpnumt[NID(p1),NID(p1)])((Num)p1,(Num)p2) ) return 1;
    return 0;
  }
  if ( VR(p1) != VR(p2) ) return 0;
  for ( dc1 = DC(p1), dc2 = DC(p2); 
    dc1 && dc2; dc1 = NEXT(dc1), dc2 = NEXT(dc2) ) {
    if ( cmpz(DEG(dc1),DEG(dc2)) ) return 0;
    else if ( !equalp(vl,COEF(dc1),COEF(dc2)) ) return 0;
  }
  if ( dc1 || dc2 ) return 0;
  else return 1;
}

void csump(VL vl,P p,Q *c)
{
  DCP dc;
  Q s,s1,s2;

  if ( !p || NUM(p) )
    *c = (Q)p;
  else {
    for ( dc = DC(p), s = 0; dc; dc = NEXT(dc) ) {
      csump(vl,COEF(dc),&s1); addq(s,s1,&s2); s = s2;
    }
    *c = s;
  }
}

void degp(V v,P p,Z *d)
{
  DCP dc;
  Z m,m1;
  
  if ( !p || NUM(p) ) 
    *d = 0;
  else if ( v == VR(p) ) 
    *d = DEG(DC(p));
  else {
    for ( dc = DC(p), m = 0; dc; dc = NEXT(dc) ) {
      degp(v,COEF(dc),&m1);
      if ( cmpz(m,m1) < 0 )
        m = m1;
    }
    *d = m;
  }
}

void mulpc_trunc(VL vl,P p,P c,VN vn,P *pr);
void mulpq_trunc(P p,Q q,VN vn,P *pr);
void mulp_trunc(VL vl,P p1,P p2,VN vn,P *pr);

void mulp_trunc(VL vl,P p1,P p2,VN vn,P *pr)
{
  register DCP dc,dct,dcr,dcr0;
  V v1,v2;
  P t,s,u;
  int n1,n2,i,d,d1;

  if ( !p1 || !p2 ) *pr = 0;
  else if ( NUM(p1) ) 
    mulpq_trunc(p2,(Q)p1,vn,pr);
  else if ( NUM(p2) )
    mulpq_trunc(p1,(Q)p2,vn,pr);
  else if ( ( v1 = VR(p1) ) ==  ( v2 = VR(p2) ) ) {
    for ( ; vn->v && vn->v != v1; vn++ )
      if ( vn->n ) {
        /* p1,p2 do not contain vn->v */
        *pr = 0;
        return;
      }
    if ( !vn->v )
      error("mulp_trunc : invalid vn");
    d = vn->n;
    for ( dc = DC(p2), s = 0; dc; dc = NEXT(dc) ) {
      for ( dcr0 = 0, dct = DC(p1); dct; dct = NEXT(dct) ) {
        d1 = ZTOS(DEG(dct))+ZTOS(DEG(dc));
        if ( d1 >= d ) {
          mulp_trunc(vl,COEF(dct),COEF(dc),vn+1,&t);
          if ( t ) {
            NEXTDC(dcr0,dcr);
            STOZ(d1,DEG(dcr));
            COEF(dcr) = t;
          }
        }
      }
      if ( dcr0 ) {
        NEXT(dcr) = 0; MKP(v1,dcr0,t);
        addp(vl,s,t,&u); s = u; t = u = 0;
      }
    }
    *pr = s;
  } else {
    while ( v1 != VR(vl) && v2 != VR(vl) ) 
      vl = NEXT(vl);
    if ( v1 == VR(vl) ) 
      mulpc_trunc(vl,p1,p2,vn,pr);
    else 
      mulpc_trunc(vl,p2,p1,vn,pr);
  }
}

void mulpq_trunc(P p,Q q,VN vn,P *pr)
{
  DCP dc,dcr,dcr0;
  P t;
  int i,d;
  V v;

  if (!p || !q)
    *pr = 0;
  else if ( NUM(p) ) {
    for ( ; vn->v; vn++ )
      if ( vn->n ) {
        *pr = 0;
        return;
      }
    MULNUM((Num)p,(Num)q,(Num *)pr);
  } else {
    v = VR(p);
    for ( ; vn->v && vn->v != v; vn++ ) {
      if ( vn->n ) {
        /* p does not contain vn->v */
        *pr = 0;
        return;
      }
    }
    if ( !vn->v )
      error("mulpq_trunc : invalid vn");
    d = vn->n;
    for ( dcr0 = 0, dc = DC(p); dc && ZTOS(DEG(dc)) >= d; dc = NEXT(dc) ) {
      mulpq_trunc(COEF(dc),q,vn+1,&t);
      if ( t ) {
        NEXTDC(dcr0,dcr); COEF(dcr) = t; DEG(dcr) = DEG(dc);
      }
    }
    if ( dcr0 ) {
      NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
    } else
      *pr = 0;
  }
}

void mulpc_trunc(VL vl,P p,P c,VN vn,P *pr)
{
  DCP dc,dcr,dcr0;
  P t;
  V v;
  int i,d;

  if ( NUM(c) ) 
    mulpq_trunc(p,(Q)c,vn,pr);
  else {
    v = VR(p);
    for ( ; vn->v && vn->v != v; vn++ )
      if ( vn->n ) {
        /* p,c do not contain vn->v */
        *pr = 0;
        return;
      }
    if ( !vn->v )
      error("mulpc_trunc : invalid vn");
    d = vn->n;
    for ( dcr0 = 0, dc = DC(p); dc && ZTOS(DEG(dc)) >= d; dc = NEXT(dc) ) {
      mulp_trunc(vl,COEF(dc),c,vn+1,&t);
      if ( t ) {
        NEXTDC(dcr0,dcr); COEF(dcr) = t; DEG(dcr) = DEG(dc);
      }
    }
    if ( dcr0 ) {
      NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
    } else
      *pr = 0;
  }
}

void quop_trunc(VL vl,P p1,P p2,VN vn,P *pr)
{
  DCP dc,dcq0,dcq;
  P t,s,m,lc2,qt;
  V v1,v2;
  Z d2;
  VN vn1;

  if ( !p1 )
    *pr = 0;
  else if ( NUM(p2) )
    divsp(vl,p1,p2,pr);
  else if ( (v1 = VR(p1)) != (v2 = VR(p2)) ) {
    for ( dcq0 = 0, dc = DC(p1); dc; dc = NEXT(dc) ) {
      NEXTDC(dcq0,dcq);
      DEG(dcq) = DEG(dc);
      quop_trunc(vl,COEF(dc),p2,vn,&COEF(dcq));
    }
    NEXT(dcq) = 0;
    MKP(v1,dcq0,*pr);
  } else {
    d2 = DEG(DC(p2));
    lc2 = COEF(DC(p2));
    t = p1;
    dcq0 = 0;
    /* vn1 = degree list of LC(p2) */
    for ( vn1 = vn; vn1->v != v1; vn1++ );
    vn1++;
    while ( t ) {
      dc = DC(t);
      NEXTDC(dcq0,dcq);
      subz(DEG(dc),d2,&DEG(dcq));
      quop_trunc(vl,COEF(dc),lc2,vn1,&COEF(dcq));
      NEXT(dcq) = 0;
      MKP(v1,dcq,qt);
      mulp_trunc(vl,p2,qt,vn,&m);
      subp(vl,t,m,&s); t = s;
    }
    NEXT(dcq) = 0;
    MKP(v1,dcq0,*pr);
  }
}
