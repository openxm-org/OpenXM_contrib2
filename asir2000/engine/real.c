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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/real.c,v 1.11 2016/06/29 08:16:11 ohara Exp $ 
*/
#include "ca.h"
#include "base.h"
#include <math.h>

double RatnToReal(a)
Q a;
{
  double nm,dn,man;
  int enm,edn,e;

  nm = NatToReal(NM(a),&enm);
  if ( INT(a) )
    if ( enm >= 1 ) {
      error("RatnToReal : Overflow");
      /* NOTREACHED */
      return 0;
    } else
      return SGN(a)>0 ? nm : -nm;
  else {
    dn = NatToReal(DN(a),&edn);
    man = nm/dn;
    if ( SGN(a) < 0 )
      man = -man;
    if ( ((e = enm - edn) >= 1024) || (e <= -1023) ) {
      error("RatnToReal : Overflow"); /* XXX */
      /* NOTREACHED */
      return 0;
    } else if ( !e )
      return man;
    else
      return man*pow(2,e);
  }
}

double NatToReal(a,expo)
N a;
int *expo;
{
  unsigned int *p;
  int l,all,i,j,s,tb,top,tail;
  unsigned int t,m[2];  
  
  p = BD(a); l = PL(a) - 1;
  for ( top = 0, t = p[l]; t; t >>= 1, top++ );
  all = top + BSH*l; tail = (53-top)%BSH; i = l-(53-top)/BSH-1; 
  m[1] = i < 0 ? 0 : p[i]>>(BSH-tail);
  for ( j = 1, i++, tb = tail; i <= l; i++ ) {
    s = 32-tb; t = i < 0 ? 0 : p[i];
    if ( BSH > s ) {
      m[j] |= ((t&((1<<s)-1))<<tb);
      if ( !j ) 
        break;
      else {
        j--; m[j] = t>>s; tb = BSH-s;
      }
    } else {
      m[j] |= (t<<tb); tb += BSH;
    }
  }
  s = (all-1)+1023;
  m[0] = (m[0]&((1<<20)-1))|(MIN(2046,s)<<20); *expo = MAX(s-2046,0);
#ifdef vax
  t = m[0]; m[0] = m[1]; m[1] = t; itod(m);
#endif
#if defined(__i386__) || defined(MIPSEL) || defined(VISUAL) || defined(__MINGW32__) || defined(__alpha) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__x86_64) || defined(__ARM_ARCH) || defined(ANDROID)
  t = m[0]; m[0] = m[1]; m[1] = t;
#endif
  return *((double *)m);
}

void addreal(a,b,c)
Num a,b;
Real *c;
{
  double t;

  t = ToReal(a)+ToReal(b); MKReal(t,*c);
}

void subreal(a,b,c)
Num a,b;
Real *c;
{
  double t;

  t = ToReal(a)-ToReal(b); MKReal(t,*c);
}

void mulreal(a,b,c)
Num a,b;
Real *c;
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

void divreal(a,b,c)
Num a,b;
Real *c;
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

void chsgnreal(a,c)
Num a,*c;
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

void pwrreal(a,b,c)
Num a,b;
Real *c;
{
  double t;
  double pwrreal0();

  if ( !b ) 
    MKReal(1.0,*c);
  else if ( !a ) 
    *c = 0;
  else if ( !RATN(b) || !INT(b) || (PL(NM((Q)b)) > 1) ) {
    t = (double)pow((double)ToReal(a),(double)ToReal(b));
#if 0
    if ( !t )
      error("pwrreal : Underflow"); /* XXX */
    else  
#endif
      MKReal(t,*c);
  } else {
    t = pwrreal0(BDY((Real)a),BD(NM((Q)b))[0]);
    t = SGN((Q)b)>0?t:1/t;
#if 0
    if ( !t )
      error("pwrreal : Underflow"); /* XXX */
    else  
#endif
      MKReal(t,*c);
  }
}

double pwrreal0(n,e)
double n;
int e;
{
  double t;

  if ( e == 1 )
    return n;
  else {
    t = pwrreal0(n,e / 2);
    return e%2 ? t*t*n : t*t;
  }
}

int cmpreal(a,b)
Real a,b;
{
  double t;

  t = ToReal(a)-ToReal(b);
  return t>0.0 ? 1 : t<0.0?-1 : 0;
}
