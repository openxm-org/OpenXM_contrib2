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
 * $OpenXM: OpenXM_contrib2/asir2000/engine-27/ddN-27.c,v 1.2 2000/08/21 08:31:32 noro Exp $ 
*/
#include "ca-27.h"
#include "base.h"
#include "inline.h"

void divn_27(n1,n2,nq,nr)
N n1,n2,*nq,*nr;
{
	int tmp;
	int br;
	int j,d,d1,d2,dd;
	int *m1,*m1e,*m2,*mr;
	N q,r;

	if ( !n2 )
		error("divn: division by 0");
	else if ( !n1 ) {
		*nq = 0; *nr = 0;
	} else if ( UNIN(n2) ) {
		*nq = n1; *nr = 0;
	} else if ( (d1 = PL(n1)) < (d2 = PL(n2)) ) {
		*nr = n1; *nq = 0;
	} else if ( d2 == 1 ) {
		if ( d1 == 1 ) {
			DQR(BD(n1)[0],BD(n2)[0],br,tmp)
			STON(br,*nq); STON(tmp,*nr);
		} else {
			tmp = divin_27(n1,BD(n2)[0],nq); STON(tmp,*nr);
		}
		return;
	} else if ( (d1 == d2) && ((tmp = cmpn(n1,n2)) <= 0) )
		if ( !tmp ) {
			*nr = 0; *nq = ONEN;
		} else {
			*nr = n1; *nq = 0;
		}
	else {
		d = BASE27/(BD(n2)[d2-1] + 1); dd = d1-d2;
		m1 = W_ALLOC(d1+1); m2 = W_ALLOC(d2+1);
		mulin_27(n1,d,m1); mulin_27(n2,d,m2);
		*nq = q = NALLOC(dd+1);
		divnmain_27(d1,d2,m1,m2,BD(q));
		PL(q) = (BD(q)[dd]?dd+1:dd);
		for ( j = d2-1; (j >= 0) && (m1[j] == 0); j--);
		if ( j == -1 ) 
			*nr = 0;
		else {
			*nr = r = NALLOC(j+1);
			for ( br = 0, m1e = m1+j, mr = BD(r)+j; 
				m1e >= m1; m1e--, mr-- ) {
				DSAB27(d,br,*m1e,*mr,br)
			}
			PL(r) = (BD(r)[j]?j+1:j);
		}
	}
}

void divnmain_27(d1,d2,m1,m2,q)
int d1,d2;
int *m1,*m2,*q;
{
	unsigned int tmp;
	int *n1,*n2;
	int i,j;
	int l,r;
	int u,qhat;
	int v1,v2;

	v1 = m2[d2-1]; v2 = m2[d2-2];
	for ( j = d1-d2, m1 += j, q += j; j >= 0; j--, q--, m1-- ) {
		n1 = m1+d2; n2 = m1+d2-1;
   		if ( *n1 == v1 ) {
   	   		qhat = BASE27 - 1; r = *n1 + *n2;
   		} else {
			DSAB27(v1,*n1,*n2,qhat,r)
		}
		DM27(v2,qhat,u,l)
   		while ( 1 ) {
   	  		if ((r > u) || ((r == u) && (*(n1-2) >= l))) 
				break;
			if ( ( l -= v2 ) < 0 ) {
				l += BASE27; u--;
			}
      		r += v1; qhat--;
   		}
		if ( qhat ) {
			for ( i = d2, u = 0, n1 = m1, n2 = m2; i > 0; i--, n1++, n2++ ) {
				DMA27(*n2,qhat,u,u,l)
				if ( (l = *n1 - l) < 0 ) {
					u++; *n1 = l + BASE27;
				} else 
					*n1 = l;
			}
			if ( *n1 < u ) {
				for ( i = d2, qhat--, r = 0, n1 = m1, n2 = m2; 
					i > 0; i--, n1++, n2++ ) {
					l = *n1 + *n2 + r; *n1 = l % BASE27; r = l / BASE27;
				}
			}
			*n1 = 0; 
		}
		*q = qhat;
	}
}

void mulin_27(n,d,p)
N n;
int d;
int *p;
{
	int carry;
	int *m,*me;

	bzero((char *)p,(int)((PL(n)+1)*sizeof(int)));
	for ( carry = 0, m = BD(n), me = m+PL(n); m < me; p++, m++ ) {
		carry += *p;
		DMA27(*m,d,carry,carry,*p)
	}
	*p = carry;
}

int divin_27(n,dvr,q)
N n;
int dvr;
N *q;
{
	int up,d;
	int *m,*mq,*md;
	N nq;

	d = PL(n); m = BD(n);
	if ( ( d == 1 ) && ( dvr > *m ) ) {
		*q = 0;
		return( *m );
	}
	*q = nq = NALLOC(d);
	for ( md = m+d-1, mq = BD(nq)+d-1, up = 0; md >= m; md--, mq-- ) {
 		DSAB27(dvr,up,*md,*mq,up)
	}
	PL(nq) = (BD(nq)[d-1]?d:d-1);
	return ( up );
}
