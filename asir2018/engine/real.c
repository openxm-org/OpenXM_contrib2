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
#include "base.h"
#include <math.h>

double RatnToReal(Q a)
{
  if ( INT(a) )
    return mpz_get_d(BDY((Z)a));
  else
    return mpq_get_d(BDY((Q)a));
}

void addreal(Num a,Num b,Real *c)
{
  double t;

  t = ToReal(a)+ToReal(b); MKReal(t,*c);
}

void subreal(Num a,Num b,Real *c)
{
  double t;

  t = ToReal(a)-ToReal(b); MKReal(t,*c);
}

void mulreal(Num a,Num b,Real *c)
{
  double t;

  if ( !a || !b )
    *c = 0;
  else {
    t = ToReal(a)*ToReal(b);
#if 0
    if ( !t )
      error("mulreal : Underflow"); /* XXX */
    else
#endif
      MKReal(t,*c);
  }
}

void divreal(Num a,Num b,Real *c)
{
  double t;

  if ( !a )
    *c = 0;
  else {
    t = ToReal(a)/ToReal(b);
#if 0
    if ( !t )
      error("divreal : Underflow"); /* XXX */
    else  
#endif
      MKReal(t,*c);
  }
}

void chsgnreal(Num a,Num *c)
{
  double t;
  Real s;

  if ( !a )
    *c = 0;
  else if ( NID(a) == N_Q )
    chsgnq((Q)a,(Q *)c);
  else {
    t = -ToReal(a); MKReal(t,s); *c = (Num)s;
  }
}

double pwrreal0(double n,int e);

void pwrreal(Num a,Num b,Real *c)
{
  int e,ea;
  double t;

  if ( !b ) 
    MKReal(1.0,*c);
  else if ( !a ) 
    *c = 0;
  else if ( !RATN(b) || !INT(b) || !smallz((Z)b) ) {
    t = (double)pow((double)ToReal(a),(double)ToReal(b));
    MKReal(t,*c);
  } else {
    e = mpz_get_si(BDY((Z)b));
    ea = e>0?e:-e;
    t = pwrreal0(BDY((Real)a),ea);
    t = e>0?t:1/t;
    MKReal(t,*c);
  }
}

double pwrreal0(double n,int e)
{
  double t;

  if ( e == 1 )
    return n;
  else {
    t = pwrreal0(n,e / 2);
    return e%2 ? t*t*n : t*t;
  }
}

int cmpreal(Real a,Real b)
{
  double t;

  t = ToReal(a)-ToReal(b);
  return t>0.0 ? 1 : t<0.0?-1 : 0;
}
