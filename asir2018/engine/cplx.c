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
 * $OpenXM: OpenXM_contrib2/asir2018/engine/cplx.c,v 1.2 2019/04/26 00:46:49 noro Exp $
*/
#include "ca.h"
#include "base.h"

void toreim(Num a,Num *rp,Num *ip)
{
  if ( !a )
    *rp = *ip = 0;
#if defined(INTERVAL)
  else if ( NID(a) < N_C ) {
#else
  else if ( NID(a) <= N_B ) {
#endif
    *rp = a; *ip = 0;
  } else {
    *rp = ((C)a)->r; *ip = ((C)a)->i;
  }
}

void reimtocplx(Num r,Num i,Num *cp)
{
  C c;

  if ( !i )
    *cp = r;
  else {
    NEWC(c); c->r = r; c->i = i; *cp = (Num)c;
  }
}

void addcplx(Num a,Num b,Num *c)
{
  Num ar,ai,br,bi,cr,ci;

  if ( !a )
    *c = b;
  else if ( !b )
    *c = a;
#if defined(INTERVAL)
  else if ( (NID(a) < N_C) && (NID(b) < N_C ) )
#else
  else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
#endif
    addnum(0,a,b,c);
  else {
    toreim(a,&ar,&ai); toreim(b,&br,&bi);
    addnum(0,ar,br,&cr); addnum(0,ai,bi,&ci);
    reimtocplx(cr,ci,c);
  }
}

void subcplx(Num a,Num b,Num *c)
{
  Num ar,ai,br,bi,cr,ci;

  if ( !a )
    chsgnnum(b,c);
  else if ( !b )
    *c = a;
#if defined(INTERVAL)
  else if ( (NID(a) < N_C) && (NID(b) <= N_C ) )
#else
  else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
#endif
    subnum(0,a,b,c);
  else {
    toreim(a,&ar,&ai); toreim(b,&br,&bi);
    subnum(0,ar,br,&cr); subnum(0,ai,bi,&ci);
    reimtocplx(cr,ci,c);
  }
}

void mulcplx(Num a,Num b,Num *c)
{
  Num ar,ai,br,bi,cr,ci,t,s;

  if ( !a || !b )
    *c = 0;
#if defined(INTERVAL)
  else if ( (NID(a) < N_C) && (NID(b) < N_C ) )
#else
  else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
#endif
    mulnum(0,a,b,c);
  else {
    toreim(a,&ar,&ai); toreim(b,&br,&bi);
    mulnum(0,ar,br,&t); mulnum(0,ai,bi,&s); subnum(0,t,s,&cr);
    mulnum(0,ar,bi,&t); mulnum(0,ai,br,&s); addnum(0,t,s,&ci);
    reimtocplx(cr,ci,c);
  }
}

void divcplx(Num a,Num b,Num *c)
{
  Num ar,ai,br,bi,cr,ci,t,s,u,w;

  if ( !b )
    error("divcplx : division by 0");
  else if ( !a )
    *c = 0;
#if defined(INTERVAL)
  else if ( (NID(a) < N_C) && (NID(b) < N_C ) )
#else
  else if ( (NID(a) <= N_B) && (NID(b) <= N_B ) )
#endif
    divnum(0,a,b,c);
  else {
    toreim(a,&ar,&ai); toreim(b,&br,&bi);
    mulnum(0,br,br,&t); mulnum(0,bi,bi,&s); addnum(0,t,s,&u);
    mulnum(0,ar,br,&t); mulnum(0,ai,bi,&s);
    addnum(0,t,s,&w); divnum(0,w,u,&cr);
    mulnum(0,ar,bi,&t); mulnum(0,ai,br,&s);
    subnum(0,s,t,&w); divnum(0,w,u,&ci);
    reimtocplx(cr,ci,c);
  }
}

void pwrcplx(Num a,Num e,Num *c)
{
  int ei;
  Num t;

  if ( !e )
    *c = (Num)ONE;
  else if ( !a )
    *c = 0;
  else if ( !INT(e) )
    error("pwrcplx : not implemented (use eval()).");
  else {
    ei = sgnq((Q)e)*ZTOS((Q)e);
    pwrcplx0(a,ei,&t);
    if ( sgnq((Q)e) < 0 )
      divnum(0,(Num)ONE,t,c);
    else
      *c = t;
  }
}

void pwrcplx0(Num a,int e,Num *c)
{
  Num t,s;

  if ( e == 1 )
    *c = a;
  else {
    pwrcplx0(a,e/2,&t);
    if ( !(e%2) )
      mulnum(0,t,t,c);
    else {
      mulnum(0,t,t,&s); mulnum(0,s,a,c);
    }
  }
}

void chsgncplx(Num a,Num *c)
{
  Num r,i;

  if ( !a )
    *c = 0;
#if defined(INTERVAL)
  else if ( NID(a) < N_C )
#else
  else if ( NID(a) <= N_B )
#endif
    chsgnnum(a,c);
  else {
    chsgnnum(((C)a)->r,&r); chsgnnum(((C)a)->i,&i);
    reimtocplx(r,i,c);
  }
}

int cmpcplx(Num a,Num b)
{
  Num ar,ai,br,bi;
  int s;

  if ( !a ) {
#if defined(INTERVAL)
    if ( !b || (NID(b)<N_C) )
#else
    if ( !b || (NID(b)<=N_B) )
#endif
      return compnum(0,a,b);
    else
      return -1;
  } else if ( !b ) {
#if defined(INTERVAL)
    if ( !a || (NID(a)<N_C) )
#else
    if ( !a || (NID(a)<=N_B) )
#endif
      return compnum(0,a,b);
    else
      return 1;
  } else {
    toreim(a,&ar,&ai); toreim(b,&br,&bi);
    s = compnum(0,ar,br);
    if ( s )
      return s;
    else
      return compnum(0,ai,bi);
  }
}
