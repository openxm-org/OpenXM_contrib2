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
 * e-mail at risa-admin@flab.fujitsu.co.jp of the detailed specification
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
 * $OpenXM: OpenXM_contrib2/asir2000/asm/ddN.c,v 1.1.1.1 1999/12/03 07:39:06 noro Exp $ 
*/
#ifndef FBASE
#define FBASE
#endif

#include "ca.h"
#include "base.h"
#include "inline.h"

void bxprintn(N),bprintn(N);
void divnmain_special(int,int,unsigned int *,unsigned int *,unsigned int *);
void dupn(N,N);

void divn(n1,n2,nq,nr)
N n1,n2,*nq,*nr;
{
	int tmp,b;
	int i,j,d1,d2,dd;
	unsigned int *m1,*m2;
	unsigned int uq,ur,msw;
	N q,r,t1,t2;

	if ( !n2 )
		error("divn: divsion by 0");
	else if ( !n1 ) {
		*nq = 0; *nr = 0;
	} else if ( UNIN(n2) ) {
		COPY(n1,*nq); *nr = 0;
	} else if ( (d1 = PL(n1)) < (d2 = PL(n2)) ) {
		COPY(n1,*nr); *nq = 0;
	} else if ( d2 == 1 ) {
		if ( d1 == 1 ) {
			DQR(BD(n1)[0],BD(n2)[0],uq,ur)
			STON(uq,*nq); STON(ur,*nr);
		} else {
			ur = divin(n1,BD(n2)[0],nq); STON(ur,*nr);
		}
		return;
	} else if ( (d1 == d2) && ((tmp = cmpn(n1,n2)) <= 0) )
		if ( !tmp ) {
			*nr = 0; COPY(ONEN,*nq);
		} else {
			COPY(n1,*nr); *nq = 0;
		}
	else {
		msw = BD(n2)[d2-1];
		for ( i = BSH-1; !(msw&(((unsigned int)1)<<i)); i-- ); i++;
		b = BSH-i;
		if ( b ) {
			bshiftn(n1,-b,&t1); bshiftn(n2,-b,&t2);
		} else {
			COPY(n1,t1); COPY(n2,t2);
		}
		m1 = (unsigned int *)ALLOCA((PL(t1)+1)*sizeof(unsigned int));	
		bcopy(BD(t1),m1,PL(t1)*sizeof(unsigned int)); m1[PL(t1)] = 0;
		m2 = BD(t2); dd = d1-d2;
		*nq = q = NALLOC(dd+1); INITRC(q);
		divnmain(d1,d2,m1,m2,BD(q)); FREEN(t1); FREEN(t2);
		PL(q) = (BD(q)[dd]?dd+1:dd);
		if ( !PL(q) ) {
			FREEN(q);
		}
		for ( j = d2-1; (j >= 0) && (m1[j] == 0); j--);
		if ( j == -1 ) 
			*nr = 0;
		else {
			r = NALLOC(j+1); INITRC(r); PL(r) = j+1;
			bcopy(m1,BD(r),(j+1)*sizeof(unsigned int));
			if ( b ) {
				bshiftn(r,b,nr); FREEN(r);
			} else
				*nr = r;
		}
	}
}

void divsn(n1,n2,nq)
N n1,n2,*nq;
{
	int d1,d2,dd,i,b;
	unsigned int *m1,*m2;
	unsigned int uq,msw;
	N q,t1,t2;

	if ( !n1 )
		*nq = 0;
	else if ( UNIN(n2) ) {
		COPY(n1,*nq);
	} else if ( (d1 = PL(n1)) < (d2 = PL(n2)) ) 
		error("divsn: cannot happen");
	else if ( d2 == 1 ) {
		if ( d1 == 1 ) {
			uq = BD(n1)[0] / BD(n2)[0]; STON(uq,*nq);
		} else 
			divin(n1,BD(n2)[0],nq);
	} else {
		msw = BD(n2)[d2-1];
		for ( i = BSH-1; !(msw&(((unsigned int)1)<<i)); i-- ); i++;
		b = BSH-i;
		if ( b ) {
			bshiftn(n1,-b,&t1); bshiftn(n2,-b,&t2);
		} else {
			COPY(n1,t1); COPY(n2,t2);
		}
		m1 = (unsigned int *)ALLOCA((PL(t1)+1)*sizeof(unsigned int));	
		bcopy(BD(t1),m1,PL(t1)*sizeof(unsigned int)); m1[PL(t1)] = 0;
		m2 = BD(t2); dd = d1-d2;
		*nq = q = NALLOC(dd+1); INITRC(q);
		divnmain(d1,d2,m1,m2,BD(q));
		FREEN(t1); FREEN(t2);
		PL(q) = (BD(q)[dd]?dd+1:dd);
		if ( !PL(q) )
			FREEN(q);
	}
}

void remn(n1,n2,nr)
N n1,n2,*nr;
{
	int d1,d2,tmp;
	unsigned int uq,ur;
	N q;

	if ( !n2 )
		error("remn: divsion by 0");
	else if ( !n1 || UNIN(n2) )
		*nr = 0;
	else if ( (d1 = PL(n1)) < (d2 = PL(n2)) ) {
		COPY(n1,*nr);
	} else if ( d2 == 1 ) {
		if ( d1 == 1 ) {
			DQR(BD(n1)[0],BD(n2)[0],uq,ur)
			STON(ur,*nr);
		} else {
			ur = rem(n1,BD(n2)[0]); UTON(ur,*nr);
		}
		return;
	} else if ( (d1 == d2) && ((tmp = cmpn(n1,n2)) <= 0) )
		if ( !tmp ) {
			*nr = 0; 
		} else {
			COPY(n1,*nr);
		}
	else
		divn(n1,n2,&q,nr);
}

/* d = 2^(32*words)-lower */

void remn_special(a,d,bits,lower,b)
N a,d;
int bits;
unsigned int lower;
N *b;
{
	int words;
	N r;
	int rl,i;
	unsigned int c,tmp;
	unsigned int *rb,*src,*dst;

	if ( cmpn(a,d) < 0 ) {
		*b = a;
		return;
	}
	words = bits>>5;
	r = NALLOC(PL(a)); dupn(a,r);
	rl = PL(r); rb = BD(r);
	while ( rl > words ) {
		src = rb+words;
		dst = rb;
		i = rl-words;
		for ( c = 0; i > 0; i--, src++, dst++ ) {
			DMA2(*src,lower,c,*dst,c,*dst)
			*src = 0;
		}
		for ( ; c; dst++ ) {
			tmp = *dst + c;
			c = tmp < c ? 1 : 0;
			*dst = tmp;
		}
		rl = dst-rb;
	}
	for ( i = words-1; i >= 0 && !rb[i]; i-- );
	PL(r) = i + 1;
	if ( cmpn(r,d) >= 0 ) {
		tmp = rb[0]-BD(d)[0];
		UTON(tmp,*b);
	} else
		*b = r;
}
void mulin(n,d,p)
N n;
unsigned int d;
unsigned int *p;
{
	unsigned int carry;
	unsigned int *m,*me;

	bzero((char *)p,(int)((PL(n)+1)*sizeof(int)));
	for ( carry = 0, m = BD(n), me = m+PL(n); m < me; p++, m++ ) {
		DMA2(*m,d,*p,carry,carry,*p)
	}
	*p = carry;
}

unsigned int divin(n,dvr,q)
N n;
unsigned int dvr;
N *q;
{
	int d;
	unsigned int up;
	unsigned int *m,*mq,*md;
	N nq;

	d = PL(n); m = BD(n);
	if ( ( d == 1 ) && ( dvr > *m ) ) {
		*q = 0;
		return *m;
	}
	*q = nq = NALLOC(d); INITRC(nq);
	for ( md = m+d-1, mq = BD(nq)+d-1, up = 0; md >= m; md--, mq-- ) {
		DSAB(dvr,up,*md,*mq,up)
	}
	PL(nq) = (BD(nq)[d-1]?d:d-1);
	if ( !PL(nq) )
		FREEN(nq);
	return ( up );
}

void bprintn(n)
N n;
{
	int l,i;
	unsigned int *b;

	if ( !n )
		printf("0");
	else {
		l = PL(n); b = BD(n);
		for ( i = l-1; i >= 0; i-- )
			printf("%010u|",b[i]);
	}
}

void bxprintn(n)
N n;
{
	int l,i;
	unsigned int *b;

	if ( !n )
		printf("0");
	else {
		l = PL(n); b = BD(n);
		for ( i = l-1; i >= 0; i-- )
			printf("%08x|",b[i]);
	}
}

#if defined(VISUAL) || defined(i386)
void muln(n1,n2,nr)
N n1,n2,*nr;
{
	unsigned int tmp,carry,mul;
	unsigned int *p1,*m1,*m2;
	int i,d1,d2,d;
	N r;

	if ( !n1 || !n2 )
		*nr = 0;
	else if ( UNIN(n1) ) 
		COPY(n2,*nr);
	else if ( UNIN(n2) )
		COPY(n1,*nr);
	else if ( (PL(n1) == 1) && (PL(n2) == 1) ) {
		DM(*BD(n1),*BD(n2),carry,tmp)
		if ( carry ) {
			*nr = r = NALLOC(2); INITRC(r);
			PL(r) = 2; p1 = BD(r); *p1++ = tmp; *p1 = carry;
		} else
			STON(tmp,*nr);
	} else {
		d1 = PL(n1); d2 = PL(n2);
		d = d1+d2;
		*nr = r = NALLOC(d); INITRC(r);
		bzero((char *)BD(r),(int)((d1+d2)*sizeof(int)));
		for ( i = 0, m1 = BD(n1), m2 = BD(n2); i < d2; i++, m2++ )
			if ( mul = *m2 )
				muln_1(m1,d1,mul,BD(r)+i);
		PL(r) = (BD(r)[d-1]?d:d-1);
	}
}

void _muln(n1,n2,nr)
N n1,n2,nr;
{
	unsigned int mul;
	unsigned int *m1,*m2;
	int i,d1,d2,d;

	if ( !n1 || !PL(n1) || !n2 || !PL(n2) )
		PL(nr) = 0;
	else if ( UNIN(n1) ) 
		dupn(n2,nr);
	else if ( UNIN(n2) )
		dupn(n1,nr);
	else {
		d1 = PL(n1); d2 = PL(n2);
		d = d1+d2;
		bzero((char *)BD(nr),(int)((d1+d2)*sizeof(int)));
		for ( i = 0, m1 = BD(n1), m2 = BD(n2); i < d2; i++, m2++ )
			if ( mul = *m2 )
				muln_1(m1,d1,mul,BD(nr)+i);
		PL(nr) = (BD(nr)[d-1]?d:d-1);
	}
}

void muln_1(p,s,d,r)
unsigned int *p;
int s;
unsigned int d;
unsigned int *r;
{
	/* esi : p, edi : r, carry : ebx, s : ecx */
#if defined(VISUAL)
	__asm {
	push esi
	push edi
	mov esi,p
	mov edi,r
	mov ecx,s
	xor ebx,ebx
	Lstart_muln:
	mov eax,DWORD PTR [esi]
	mul d
	add eax,DWORD PTR [edi]
	adc edx,0
	add eax,ebx
	adc edx,0
	mov DWORD PTR[edi],eax
	mov ebx,edx
	lea	esi,DWORD PTR [esi+4]
	lea	edi,DWORD PTR [edi+4]
	dec ecx
	jnz Lstart_muln
	mov DWORD PTR [edi],ebx
	pop edi
	pop esi
	}
#else
	asm volatile("\
	movl	%0,%%esi;\
	movl	%1,%%edi;\
	movl	$0,%%ebx;\
	Lstart_muln:;\
	movl	(%%esi),%%eax;\
	mull	%2;\
	addl	(%%edi),%%eax;\
	adcl	$0,%%edx;\
	addl	%%ebx,%%eax;\
	adcl	$0,%%edx;\
	movl	%%eax,(%%edi);\
	movl	%%edx,%%ebx;\
	leal	4(%%esi),%%esi;\
	leal	4(%%edi),%%edi;\
	decl	%3;\
	jnz		Lstart_muln;\
	movl	%%ebx,(%%edi)"\
	:\
	:"m"(p),"m"(r),"m"(d),"m"(s)\
	:"eax","ebx","edx","esi","edi");
#endif
}

void divnmain(d1,d2,m1,m2,q)
int d1,d2;
unsigned int *m1,*m2,*q;
{
	int i,j;
	UL r,ltmp;
	unsigned int l,ur;
	unsigned int *n1,*n2;
	unsigned int u,qhat;
	unsigned int v1,v2;

	v1 = m2[d2-1]; v2 = m2[d2-2];
#if 0
	if ( v1 == (1<<31) && !v2 ) {
		divnmain_special(d1,d2,m1,m2,q);
		return;
	}
#endif
	for ( j = d1-d2, m1 += j, q += j; j >= 0; j--, q--, m1-- ) {
		n1 = m1+d2; n2 = m1+d2-1;
   		if ( *n1 == v1 ) {
   	   		qhat = ULBASE - 1;
			r = (UL)*n1 + (UL)*n2;
   		} else {
			DSAB(v1,*n1,*n2,qhat,ur)
			r = (UL)ur;
		}
		DM(v2,qhat,u,l)
   		while ( 1 ) {
   	  		if ((r > (UL)u) || ((r == (UL)u) && (*(n1-2) >= l))) 
				break;
			if ( l >= v2 )
				l -= v2;
			else {
				l = (unsigned int)((UL)l+(ULBASE-(UL)v2)); u--;
			}
      		r += v1; qhat--;
   		}
		if ( qhat ) {
			u = divn_1(m2,d2,qhat,m1);
			if ( *(m1+d2) < u ) {
				for ( i = d2, qhat--, ur = 0, n1 = m1, n2 = m2; 
					i > 0; i--, n1++, n2++ ) {
					ltmp = (UL)*n1 + (UL)*n2 + (UL)ur;
					*n1 = (unsigned int)(ltmp & BMASK);
					ur = (unsigned int)(ltmp >> BSH);
				}
			}
			*n1 = 0; 
		}
		*q = qhat;
	}
}

void divnmain_special(d1,d2,m1,m2,q)
int d1,d2;
unsigned int *m1,*m2,*q;
{
	int i,j;
	UL ltmp;
	unsigned int ur;
	unsigned int *n1,*n2;
	unsigned int u,qhat;
	unsigned int v1,v2;

	v1 = m2[d2-1]; v2 = 0;
	for ( j = d1-d2, m1 += j, q += j; j >= 0; j--, q--, m1-- ) {
		n1 = m1+d2; n2 = m1+d2-1;
		qhat = ((*n1)<<1)|((*n2)>>31);
		if ( qhat ) {
			u = divn_1(m2,d2,qhat,m1);
			if ( *(m1+d2) < u ) {
				for ( i = d2, qhat--, ur = 0, n1 = m1, n2 = m2; 
					i > 0; i--, n1++, n2++ ) {
					ltmp = (UL)*n1 + (UL)*n2 + (UL)ur;
					*n1 = (unsigned int)(ltmp & BMASK);
					ur = (unsigned int)(ltmp >> BSH);
				}
			}
			*n1 = 0; 
		}
		*q = qhat;
	}
}

unsigned int divn_1(p,s,d,r)
unsigned int *p;
int s;
unsigned int d;
unsigned int *r;
{
/*
	unsigned int borrow,l;

	for ( borrow = 0; s--; p++, r++ ) {
		DMA(*p,d,borrow,borrow,l)
		if ( *r >= l )
			*r -= l;
		else {
			*r = (unsigned int)((UL)*r+(ULBASE-(UL)l)); borrow++;
		}
	}
	return borrow;
*/
	/* esi : p, edi : r, borrow : ebx, s : ecx */
#if defined(VISUAL)
	__asm {
	push esi
	push edi
	mov esi,p
	mov edi,r
	mov ecx,s
	xor ebx,ebx
	Lstart_divn:
	mov eax,DWORD PTR [esi]
	mul d
	add eax,ebx
	adc edx,0
	sub DWORD PTR [edi],eax
	adc edx,0
	mov ebx,edx
	lea	esi,DWORD PTR [esi+4]
	lea	edi,DWORD PTR [edi+4]
	dec ecx
	jnz Lstart_divn
	mov eax,ebx
	pop edi
	pop esi
	}
	/* return value is in eax. */
#else
	unsigned int borrow;

	asm volatile("\
	movl	%1,%%esi;\
	movl	%2,%%edi;\
	movl	$0,%%ebx;\
	Lstart_divn:;\
	movl	(%%esi),%%eax;\
	mull	%3;\
	addl	%%ebx,%%eax;\
	adcl	$0,%%edx;\
	subl	%%eax,(%%edi);\
	adcl	$0,%%edx;\
	movl	%%edx,%%ebx;\
	leal	4(%%esi),%%esi;\
	leal	4(%%edi),%%edi;\
	decl	%4;\
	jnz		Lstart_divn;\
	movl	%%ebx,%0"\
	:"=m"(borrow)\
	:"m"(p),"m"(r),"m"(d),"m"(s)\
	:"eax","ebx","edx","esi","edi");

	return borrow;
#endif
}

#else

void muln(n1,n2,nr)
N n1,n2,*nr;
{
	unsigned int tmp,carry,mul;
	unsigned int *p1,*pp,*m1,*m2;
	int i,j,d1,d2;
	N r;

	if ( !n1 || !n2 )
		*nr = 0;
	else if ( UNIN(n1) ) 
		COPY(n2,*nr);
	else if ( UNIN(n2) )
		COPY(n1,*nr);
	else if ( (PL(n1) == 1) && (PL(n2) == 1) ) {
		DM(*BD(n1),*BD(n2),carry,tmp)
		if ( carry ) {
			*nr = r = NALLOC(2); INITRC(r);
			PL(r) = 2; p1 = BD(r); *p1++ = tmp; *p1 = carry;
		} else
			STON(tmp,*nr);
	} else {
		d1 = PL(n1); d2 = PL(n2);
		*nr = r = NALLOC(d1+d2); INITRC(r);
		bzero((char *)BD(r),(int)((d1+d2)*sizeof(int)));
		for ( i = 0, m1 = BD(n1), m2 = BD(n2); i < d2; i++, m2++ )
			if ( mul = *m2 ) {
				for ( j = d1, carry = 0, p1 = m1, pp = BD(r)+i;
					j--; pp++ ) {
					DMA2(*p1++,mul,*pp,carry,carry,*pp)
				}
				*pp = carry;
			}
		PL(r) = (carry?d1+d2:d1+d2-1);
	}
}

void _muln(n1,n2,nr)
N n1,n2,nr;
{
	unsigned int tmp,carry,mul;
	unsigned int *p1,*pp,*m1,*m2;
	int i,j,d1,d2;

	if ( !n1 || !PL(n1) || !n2 || !PL(n2) )
		PL(nr) = 0;
	else if ( UNIN(n1) ) 
		dupn(n2,nr);
	else if ( UNIN(n2) )
		dupn(n1,nr);
	else {
		d1 = PL(n1); d2 = PL(n2);
		bzero((char *)BD(nr),(int)((d1+d2)*sizeof(int)));
		for ( i = 0, m1 = BD(n1), m2 = BD(n2); i < d2; i++, m2++ )
			if ( mul = *m2 ) {
				for ( j = d1, carry = 0, p1 = m1, pp = BD(nr)+i;
					j--; pp++ ) {
					DMA2(*p1++,mul,*pp,carry,carry,*pp)
				}
				*pp = carry;
			}
		PL(nr) = (carry?d1+d2:d1+d2-1);
	}
}

/* r[0...s] = p[0...s-1]*d */

void muln_1(p,s,d,r)
unsigned int *p;
int s;
unsigned int d;
unsigned int *r;
{
	unsigned int carry;

	for ( carry = 0; s--; p++, r++ ) {
		DMA2(*p,d,carry,*r,carry,*r)
	}
	*r = carry;
}

void divnmain(d1,d2,m1,m2,q)
int d1,d2;
unsigned int *m1,*m2,*q;
{
	int i,j;
	UL r,ltmp;
	unsigned int l,ur,tmp;
	unsigned int *n1,*n2;
	unsigned int u,qhat;
	unsigned int v1,v2;

	v1 = m2[d2-1]; v2 = m2[d2-2];
	for ( j = d1-d2, m1 += j, q += j; j >= 0; j--, q--, m1-- ) {
		n1 = m1+d2; n2 = m1+d2-1;
   		if ( *n1 == v1 ) {
   	   		qhat = ULBASE - 1;
			r = (UL)*n1 + (UL)*n2;
   		} else {
			DSAB(v1,*n1,*n2,qhat,ur)
			r = (UL)ur;
		}
		DM(v2,qhat,u,l)
   		while ( 1 ) {
   	  		if ((r > (UL)u) || ((r == (UL)u) && (*(n1-2) >= l))) 
				break;
			if ( l >= v2 )
				l -= v2;
			else {
				l = (unsigned int)((UL)l+(ULBASE-(UL)v2)); u--;
			}
      		r += v1; qhat--;
   		}
		if ( qhat ) {
			u = divn_1(m2,d2,qhat,m1);
			if ( *(m1+d2) < u ) {
				for ( i = d2, qhat--, ur = 0, n1 = m1, n2 = m2; 
					i > 0; i--, n1++, n2++ ) {
					ltmp = (UL)*n1 + (UL)*n2 + (UL)ur;
					*n1 = (unsigned int)(ltmp & BMASK);
					ur = (unsigned int)(ltmp >> BSH);
				}
			}
			*n1 = 0; 
		}
		*q = qhat;
	}
}

unsigned int divn_1(p,s,d,r)
unsigned int *p;
int s;
unsigned int d;
unsigned int *r;
{
	unsigned int borrow,l;

	for ( borrow = 0; s--; p++, r++ ) {
		DMA(*p,d,borrow,borrow,l)
		if ( *r >= l )
			*r -= l;
		else {
			*r = (unsigned int)((UL)*r+(ULBASE-(UL)l)); borrow++;
		}
	}
	return borrow;
}
#endif
