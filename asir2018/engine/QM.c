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
 * $OpenXM: OpenXM_contrib2/asir2018/engine/QM.c,v 1.1 2018/09/19 05:45:07 noro Exp $
*/
#include "ca.h"

void addmq(m,n1,n2,nr)
int m;
MQ n1,n2,*nr;
{
  int a;

  if ( !n1 ) 
    *nr = n2;
  else if ( !n2 ) 
    *nr = n1;
  else {
    a = (CONT(n1) + CONT(n2)) % m; STOMQ(a,*nr);
  }
}

void submq(m,n1,n2,nr)
int m;
MQ n1,n2,*nr;
{
  int a;

  if ( !n1 ) {
    a = (m - CONT(n2)) % m; STOMQ(a,*nr);
  } else if ( !n2 ) 
    *nr = n1;
  else {
    a = (CONT(n1) - CONT(n2) + m) % m; STOMQ(a,*nr);
  }
}

void mulmq(m,n1,n2,nr)
int m;
MQ n1,n2,*nr;
{
  int a,b;

  if ( !n1 || !n2 ) 
    *nr = 0;
  else {
    a = dmb(m,CONT(n1),CONT(n2),&b); STOMQ(a,*nr);
  }
}

void divmq(m,n1,n2,nq)
int m;
MQ n1,n2,*nq;
{
  int a,b;

  if ( !n2 )
    error("divmq : division by 0");
  else if ( !n1 ) 
    *nq = 0;
  else if ( n1 == n2 ) 
    *nq = ONEM;
  else {
    a = dmb(m,CONT(n1),invm(CONT(n2),m),&b); STOMQ(a,*nq);
  }
}

void invmq(m,n1,nq)
int m;
MQ n1,*nq;
{
  int a;

  if ( !n1 ) 
    error("invmq : division by 0");
  else {
    a = invm(CONT(n1),m); STOMQ(a,*nq);
  }
}

void pwrmq(m,n1,n,nr)
int m;
MQ n1;
Q n;
MQ *nr;
{
  int a;

  if ( n == 0 ) 
    *nr = ONEM;
  else if ( !n1 )
    *nr = 0;
  else {
    a = pwrm(m,CONT(n1),ZTOS(n)); STOMQ(a,*nr);
  }
}

void mkbcm(m,n,t)
int m,n;
MQ *t;
{
  int i,j;
  int *a,*b,*c;

  W_CALLOC(n,int,a); W_CALLOC(n,int,b);
  for ( i = 1, a[0] = 1; i <= n; i++ ) {
    for ( j = 1, b[0] = b[i] = 1; j < i; j++ )
      b[j] = (a[j-1] + a[j]) % m;
    c = a; a = b; b = c;
  }
  for ( i = 0; i <= n; i++ )
    STOMQ(a[i],t[i]);
}
