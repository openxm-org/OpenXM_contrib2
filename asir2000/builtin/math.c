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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/math.c,v 1.12 2015/08/14 13:51:54 fujimoto Exp $ 
*/
#include "ca.h"
#include <math.h>
#include "parse.h"
#if defined(VISUAL) || defined(__MINGW32__)
#include <float.h>
#endif

void Pdsqrt(),Pdsin(),Pdcos(),Pdtan(),Pdasin(),Pdacos(),Pdatan(),Pdlog(),Pdexp();
void Pabs(),Pdfloor(),Pdceil(),Pdrint(),Pdisnan();

struct ftab math_tab[] = {
  {"dsqrt",Pdsqrt,1},
  {"dabs",Pabs,1},
  {"dsin",Pdsin,1},
  {"dcos",Pdcos,1},
  {"dtan",Pdtan,1},
  {"dlog",Pdlog,1},
  {"dexp",Pdexp,1},
  {"dasin",Pdasin,1},
  {"dacos",Pdacos,1},
  {"datan",Pdatan,1},
  {"floor",Pdfloor,1},
  {"dfloor",Pdfloor,1},
  {"ceil",Pdceil,1},
  {"dceil",Pdceil,1},
  {"rint",Pdrint,1},
  {"drint",Pdrint,1},
  {"disnan",Pdisnan,1},
  {0,0,0},
};

void get_ri(Num z,double *r,double *i)
{
  if ( !z ) {
    *r = 0; *i = 0; return;
  }
  if ( OID(z) != O_N )
    error("get_ri : invalid argument");
  switch ( NID(z) ) {
    case N_Q: case N_R: case N_B:
      *r = ToReal(z); *i = 0;
      break;
    case N_C:
      *r = ToReal(((C)z)->r);
      *i = ToReal(((C)z)->i);
      break;
    default:
      error("get_ri : invalid argument");
      break;
  }
}

void Pabs(arg,rp)
NODE arg;
Real *rp;
{
  double s,r,i;

  if ( !ARG0(arg) ) {
    *rp = 0; return;
  }
  get_ri((Num)ARG0(arg),&r,&i);
  if ( i == 0 )
    s = fabs(r);
  else if ( r == 0 )
    s = fabs(i);
  else
    s = sqrt(r*r+i*i);
  MKReal(s,*rp);
}

void Pdsqrt(arg,rp)
NODE arg;
Num *rp;
{
  double s,r,i,a;
  C z;
  Real real;

  if ( !ARG0(arg) ) {
    *rp = 0; return;
  }
  get_ri((Num)ARG0(arg),&r,&i);
  if ( i == 0 )
    if ( r > 0 ) {
      s = sqrt(r);
      MKReal(s,real);
      *rp = (Num)real;
    } else {
      NEWC(z);
      z->r = 0;
      s = sqrt(-r); MKReal(s,real); z->i = (Num)real;
      *rp = (Num)z;
    }
  else {
    a = sqrt(r*r+i*i);
    NEWC(z);
    s = sqrt((r+a)/2); MKReal(s,real); z->r = (Num)real;
    s = i>0?sqrt((-r+a)/2):-sqrt((-r+a)/2);
    MKReal(s,real); z->i = (Num)real;
    *rp = (Num)z;
  }
}

void Pdsin(arg,rp)
NODE arg;
Real *rp;
{
  double s;

  s = sin(ToReal(ARG0(arg)));
  MKReal(s,*rp);
}

void Pdcos(arg,rp)
NODE arg;
Real *rp;
{
  double s;

  s = cos(ToReal(ARG0(arg)));
  MKReal(s,*rp);
}

void Pdtan(arg,rp)
NODE arg;
Real *rp;
{
  double s;

  s = tan(ToReal(ARG0(arg)));
  MKReal(s,*rp);
}

void Pdasin(arg,rp)
NODE arg;
Real *rp;
{
  double s;

  s = asin(ToReal(ARG0(arg)));
  MKReal(s,*rp);
}

void Pdacos(arg,rp)
NODE arg;
Real *rp;
{
  double s;

  s = acos(ToReal(ARG0(arg)));
  MKReal(s,*rp);
}

void Pdatan(arg,rp)
NODE arg;
Real *rp;
{
  double s;

  s = atan(ToReal(ARG0(arg)));
  MKReal(s,*rp);
}

void Pdlog(arg,rp)
NODE arg;
Real *rp;
{
  double s;

  s = log(ToReal(ARG0(arg)));
  MKReal(s,*rp);
}

void Pdexp(arg,rp)
NODE arg;
Real *rp;
{
  double s;

  s = exp(ToReal(ARG0(arg)));
  MKReal(s,*rp);
}

void Pdfloor(arg,rp)
NODE arg;
Q *rp;
{
  L a;
  unsigned int au,al;
  int sgn;
  Q q;
  double d;

  if ( !ARG0(arg) ) {
    *rp = 0;
    return;
  }
  d = floor(ToReal(ARG0(arg)));
  if ( d < -9.223372036854775808e18 || d >= 9.223372036854775808e18 )
    error("dfloor : OverFlow");
  if ( !d ) {
    *rp = 0;
    return;
  }
  a = (L)d;
  if ( a < 0 ) {
    sgn = -1;
    a = -a;
  } else
    sgn = 1;
#if defined(i386) || defined(__alpha) || defined(VISUAL) || defined(__MINGW32__) || defined(__x86_64)
  au = ((unsigned int *)&a)[1];
  al = ((unsigned int *)&a)[0];
#else
  al = ((unsigned int *)&a)[1];
  au = ((unsigned int *)&a)[0];
#endif
  if ( au ) {
    NEWQ(q); SGN(q) = sgn; NM(q)=NALLOC(2); DN(q)=0;
    PL(NM(q))=2; BD(NM(q))[0]=al; BD(NM(q))[1] = au;
  } else {
    UTOQ(al,q); SGN(q) = sgn;
  }
  *rp = q;
}

void Pdceil(arg,rp)
NODE arg;
Q *rp;
{
  L a;
  unsigned int au,al;
  int sgn;
  Q q;
  double d;

  if ( !ARG0(arg) ) {
    *rp = 0;
    return;
  }
  d = ceil(ToReal(ARG0(arg)));
  if ( d < -9.223372036854775808e18 || d >= 9.223372036854775808e18 )
    error("dceil : OverFlow");
  if ( !d ) {
    *rp = 0;
    return;
  }
  a = (L)d;
  if ( a < 0 ) {
    sgn = -1;
    a = -a;
  } else
    sgn = 1;
#if defined(i386) || defined(__alpha) || defined(VISUAL) || defined(__MINGW32__) || defined(__x86_64)
  au = ((unsigned int *)&a)[1];
  al = ((unsigned int *)&a)[0];
#else
  al = ((unsigned int *)&a)[1];
  au = ((unsigned int *)&a)[0];
#endif
  if ( au ) {
    NEWQ(q); SGN(q) = sgn; NM(q)=NALLOC(2); DN(q)=0;
    PL(NM(q))=2; BD(NM(q))[0]=al; BD(NM(q))[1] = au;
  } else {
    UTOQ(al,q); SGN(q) = sgn;
  }
  *rp = q;
}

void Pdrint(arg,rp)
NODE arg;
Q *rp;
{
  L a;
  unsigned int au,al;
  int sgn;
  Q q;
  double d;

  if ( !ARG0(arg) ) {
    *rp = 0;
    return;
  }
#if defined(VISUAL) || defined(__MINGW32__)
  d = ToReal(ARG0(arg));
  if ( d > 0 )
    d = floor(d+0.5);
  else
    d = ceil(d-0.5);
#else
  d = rint(ToReal(ARG0(arg)));
#endif
  if ( d < -9.223372036854775808e18 || d >= 9.223372036854775808e18 )
    error("drint : OverFlow");
  a = (L)d;
  if ( a < 0 ) {
    sgn = -1;
    a = -a;
  } else
    sgn = 1;
#if defined(i386) || defined(__alpha) || defined(VISUAL) || defined(__MINGW32__) || defined(__x86_64)
  au = ((unsigned int *)&a)[1];
  al = ((unsigned int *)&a)[0];
#else
  al = ((unsigned int *)&a)[1];
  au = ((unsigned int *)&a)[0];
#endif
  if ( au ) {
    NEWQ(q); SGN(q) = sgn; NM(q)=NALLOC(2); DN(q)=0;
    PL(NM(q))=2; BD(NM(q))[0]=al; BD(NM(q))[1] = au;
  } else if ( al ) {
    UTOQ(al,q); SGN(q) = sgn;
  } else
    q = 0;
  *rp = q;
}

void Pdisnan(NODE arg,Q *rp)
{
  Real r;
  double d;
#if defined(VISUAL) || defined(__MINGW32__)
    int c;
#endif

  r = (Real)ARG0(arg);
  if ( !r || !NUM(r) || !REAL(r) ) {
    *rp = 0;
    return;
  }
  d = ToReal(r);
#if defined(VISUAL) || defined(__MINGW32__)
  c = _fpclass(d);
  if ( c == _FPCLASS_SNAN || c == _FPCLASS_QNAN ) *rp = ONE;
  else if ( c == _FPCLASS_PINF || c == _FPCLASS_NINF ) STOQ(2,*rp);
#else
  if ( isnan(d) ) *rp = ONE;
  else if ( isinf(d) ) STOQ(2,*rp);
#endif
  else *rp = 0;
}
