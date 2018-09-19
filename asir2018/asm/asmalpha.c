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
#if defined(__alpha) || ( SIZEOF_LONG == 8 )
typedef unsigned long UL;
#elif defined(mips) || defined(hpux) || defined(powerpc) || defined(__ppc__) || defined(_IBMR2) || defined(HAVE_UNSIGNED_LONG_LONG)
typedef unsigned long long UL;
#endif

#include "base.h"

unsigned int dm(a1,a2,u)
unsigned int a1,a2,*u;
{
  UL t;

  t = (UL)a1*(UL)a2;
  *u = t>>32;
  return (unsigned int)(t&0xffffffff);
}

unsigned int dma(a1,a2,a3,u)
unsigned int a1,a2,a3,*u;
{
  UL t;

  t = ((UL)a1*(UL)a2)+(UL)a3;
  *u = t>>32;
  return (unsigned int)(t&0xffffffff);
}

unsigned int dma2(a1,a2,a3,a4,u)
unsigned int a1,a2,a3,a4,*u;
{
  UL t;

  t = (UL)a1*(UL)a2+(UL)a3+(UL)a4;
  *u = t>>32;
  return (unsigned int)(t&0xffffffff);
}

unsigned int dmb(base,a1,a2,u)
unsigned int base,a1,a2,*u;
{
  UL t;

  t = (UL)a1*(UL)a2;
  *u = t/(UL)base;
  return (unsigned int)(t-(UL)base*(UL)(*u));
}

unsigned int dmab(base,a1,a2,a3,u)
unsigned int base,a1,a2,a3,*u;
{
  UL t;

  t = (UL)a1*(UL)a2+(UL)a3;
  *u = t/(UL)base;
  return (unsigned int)(t-(UL)base*(UL)(*u));
}

unsigned int dmar(a1,a2,a3,d)
unsigned int a1,a2,a3,d;
{
  UL t;

  t = (UL)a1*(UL)a2+(UL)a3;
  return (unsigned int)(t%(UL)d);
}

unsigned int dsab(base,a1,a2,u)
unsigned int base,a1,a2,*u;
{
  UL t;

  t = (((UL)a1)<<32)+(UL)a2;
  *u = t/(UL)base;
  return (unsigned int)(t-(UL)base*(UL)(*u));
}

unsigned int dqr(a,b,qp)
unsigned int a,b,*qp;
{
  *qp = a/b;
  return a - b * (*qp);
}
