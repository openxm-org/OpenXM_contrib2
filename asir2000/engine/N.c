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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/N.c,v 1.2 2000/08/21 08:31:25 noro Exp $ 
*/
#include "ca.h"
#include "base.h"

void addn();
int subn();
void _bshiftn();
void dupn();

#if defined(VISUAL) || defined(i386)
void addn(n1,n2,nr)
N n1,n2,*nr;
{
	unsigned int *m1,*m2,*mr;
	unsigned int c;
	N r;
	int i,d1,d2;
	unsigned int tmp;

	if ( !n1 )
		COPY(n2,*nr);
	else if ( !n2 )
		COPY(n1,*nr);
	else {
		if ( PL(n1) > PL(n2) ) {	
			d1 = PL(n1); d2 = PL(n2); m1 = BD(n1); m2 = BD(n2);
		} else {
			d1 = PL(n2); d2 = PL(n1); m1 = BD(n2); m2 = BD(n1);
		}
		*nr = r = NALLOC(d1 + 1); INITRC(r); mr = BD(r);

#if defined(VISUAL)
		__asm {
		push	esi
		push	edi
		mov esi,m1
		mov edi,m2
		mov ebx,mr
		mov ecx,d2
		xor	eax,eax
		Lstart_addn:
		mov eax,DWORD PTR [esi]
		mov edx,DWORD PTR [edi]
		adc eax,edx
		mov DWORD PTR [ebx],eax
		lea esi,DWORD PTR [esi+4]
		lea edi,DWORD PTR [edi+4]
		lea ebx,DWORD PTR [ebx+4]
		dec ecx
		jnz Lstart_addn
		pop	edi
		pop	esi
		mov eax,0
		adc eax,eax
		mov c,eax
		}
#else
		asm volatile("\
		movl	%1,%%esi;\
		movl	%2,%%edi;\
		movl	%3,%%ebx;\
		movl	%4,%%ecx;\
		testl	%%eax,%%eax;\
		Lstart_addn:;\
		movl	(%%esi),%%eax;\
		movl	(%%edi),%%edx;\
		adcl	%%edx,%%eax;\
		movl	%%eax,(%%ebx);\
		leal	4(%%esi),%%esi;\
		leal	4(%%edi),%%edi;\
		leal	4(%%ebx),%%ebx;\
		decl	%%ecx;\
		jnz Lstart_addn;\
		movl	$0,%%eax;\
		adcl	%%eax,%%eax;\
		movl	%%eax,%0"\
		:"=m"(c)\
		:"m"(m1),"m"(m2),"m"(mr),"m"(d2)\
		:"eax","ebx","ecx","edx","esi","edi");
#endif
		for ( i = d2, m1 += d2, mr += d2; (i < d1) && c ; i++ ) {
			tmp = *m1++ + c;
			c = tmp < c ? 1 : 0;
			*mr++ = tmp;
		}
		for ( ; i < d1; i++ ) 
			*mr++ = *m1++;
		*mr = c;
		PL(r) = (c?d1+1:d1);
	}
}

int subn(n1,n2,nr)
N n1,n2,*nr;
{
	N r;
	unsigned int *m1,*m2,*mr,br;
	unsigned int tmp,t;
	int d1,d2,sgn,i;

	if ( !n1 ) {
		if ( n2 ) {
			COPY(n2,*nr);
			return -1;
		} else {
			*nr = 0;
			return 0;
		}
	} else if ( !n2 ) {
		COPY(n1,*nr);
		return 1;
	} else {
		d1 = PL(n1); d2 = PL(n2);
		m1 = BD(n1); m2 = BD(n2);
		if ( (d1 = PL(n1)) > (d2 = PL(n2)) )
			sgn = 1;
		else if ( d1 < d2 ) {
			d1 = PL(n2); d2 = PL(n1);
			m1 = BD(n2); m2 = BD(n1);
			sgn = -1;
		} else {
			for ( i = d1-1; i >= 0 && m1[i] == m2[i]; i-- );
			if ( i < 0 ) {
				*nr = 0;
				return 0;
			}
			d1 = d2 = i+1;
			if ( m1[i] > m2[i] )
				sgn = 1;
			else {
				m1 = BD(n2); m2 = BD(n1);
				sgn = -1;
			}
		}
		*nr = r = NALLOC(d1); INITRC(r); mr = BD(r);

#if defined(VISUAL)
		__asm {
		push	esi
		push	edi
		mov esi,m1
		mov edi,m2
		mov ebx,mr
		mov ecx,d2
		xor	eax,eax
		Lstart_subn:
		mov eax,DWORD PTR [esi]
		mov edx,DWORD PTR [edi]
		sbb eax,edx
		mov DWORD PTR [ebx],eax
		lea esi,DWORD PTR [esi+4]
		lea edi,DWORD PTR [edi+4]
		lea ebx,DWORD PTR [ebx+4]
		dec ecx
		jnz Lstart_subn
		pop	edi
		pop	esi
		mov eax,0
		adc eax,eax
		mov br,eax
		}
#else
		asm volatile("\
		movl	%1,%%esi;\
		movl	%2,%%edi;\
		movl	%3,%%ebx;\
		movl	%4,%%ecx;\
		testl	%%eax,%%eax;\
		Lstart_subn:;\
		movl	(%%esi),%%eax;\
		movl	(%%edi),%%edx;\
		sbbl	%%edx,%%eax;\
		movl	%%eax,(%%ebx);\
		leal	4(%%esi),%%esi;\
		leal	4(%%edi),%%edi;\
		leal	4(%%ebx),%%ebx;\
		decl	%%ecx;\
		jnz Lstart_subn;\
		movl	$0,%%eax;\
		adcl	%%eax,%%eax;\
		movl	%%eax,%0"\
		:"=m"(br)\
		:"m"(m1),"m"(m2),"m"(mr),"m"(d2)\
		:"eax","ebx","ecx","edx","esi","edi");
#endif
		for ( i = d2, m1 += d2, mr += d2; (i < d1) && br; i++ ) {
			t = *m1++;
			tmp = t - br;
			br = tmp > t ? 1 : 0;
			*mr++ = tmp;
		}
		for ( ; i < d1; i++ )
			*mr++ = *m1++;
		for ( i = d1-1, mr--; i >= 0 && !*mr--; i-- );
		PL(r) = i + 1;
		return sgn;
	}
}

void _addn(n1,n2,nr)
N n1,n2,nr;
{
	unsigned int *m1,*m2,*mr;
	unsigned int c;
	int i,d1,d2;
	unsigned int tmp;

	if ( !n1 || !PL(n1) )
		dupn(n2,nr);
	else if ( !n2 || !PL(n2) )
		dupn(n1,nr);
	else {
		if ( PL(n1) > PL(n2) ) {	
			d1 = PL(n1); d2 = PL(n2); m1 = BD(n1); m2 = BD(n2);
		} else {
			d1 = PL(n2); d2 = PL(n1); m1 = BD(n2); m2 = BD(n1);
		}
		mr = BD(nr);

#if defined(VISUAL)
		__asm {
		push	esi
		push	edi
		mov esi,m1
		mov edi,m2
		mov ebx,mr
		mov ecx,d2
		xor	eax,eax
		Lstart__addn:
		mov eax,DWORD PTR [esi]
		mov edx,DWORD PTR [edi]
		adc eax,edx
		mov DWORD PTR [ebx],eax
		lea esi,DWORD PTR [esi+4]
		lea edi,DWORD PTR [edi+4]
		lea ebx,DWORD PTR [ebx+4]
		dec ecx
		jnz Lstart__addn
		pop	edi
		pop	esi
		mov eax,0
		adc eax,eax
		mov c,eax
		}
#else
		asm volatile("\
		movl	%1,%%esi;\
		movl	%2,%%edi;\
		movl	%3,%%ebx;\
		movl	%4,%%ecx;\
		testl	%%eax,%%eax;\
		Lstart__addn:;\
		movl	(%%esi),%%eax;\
		movl	(%%edi),%%edx;\
		adcl	%%edx,%%eax;\
		movl	%%eax,(%%ebx);\
		leal	4(%%esi),%%esi;\
		leal	4(%%edi),%%edi;\
		leal	4(%%ebx),%%ebx;\
		decl	%%ecx;\
		jnz Lstart__addn;\
		movl	$0,%%eax;\
		adcl	%%eax,%%eax;\
		movl	%%eax,%0"\
		:"=m"(c)\
		:"m"(m1),"m"(m2),"m"(mr),"m"(d2)\
		:"eax","ebx","ecx","edx","esi","edi");
#endif
		for ( i = d2, m1 += d2, mr += d2; (i < d1) && c ; i++ ) {
			tmp = *m1++ + c;
			c = tmp < c ? 1 : 0;
			*mr++ = tmp;
		}
		for ( ; i < d1; i++ ) 
			*mr++ = *m1++;
		*mr = c;
		PL(nr) = (c?d1+1:d1);
	}
}

int _subn(n1,n2,nr)
N n1,n2,nr;
{
	unsigned int *m1,*m2,*mr,br;
	unsigned int tmp,t;
	int d1,d2,sgn,i;

	if ( !n1 || !PL(n1) ) {
		if ( n2 && PL(n2) ) {
			dupn(n2,nr);
			return -1;
		} else {
			PL(nr) = 0;
			return 0;
		}
	} else if ( !n2 || !PL(n2) ) {
		dupn(n1,nr);
		return 1;
	} else {
		d1 = PL(n1); d2 = PL(n2);
		m1 = BD(n1); m2 = BD(n2);
		if ( (d1 = PL(n1)) > (d2 = PL(n2)) )
			sgn = 1;
		else if ( d1 < d2 ) {
			d1 = PL(n2); d2 = PL(n1);
			m1 = BD(n2); m2 = BD(n1);
			sgn = -1;
		} else {
			for ( i = d1-1; i >= 0 && m1[i] == m2[i]; i-- );
			if ( i < 0 ) {
				PL(nr) = 0;
				return 0;
			}
			d1 = d2 = i+1;
			if ( m1[i] > m2[i] )
				sgn = 1;
			else {
				m1 = BD(n2); m2 = BD(n1);
				sgn = -1;
			}
		}
		mr = BD(nr);

#if defined(VISUAL)
		__asm {
		push	esi
		push	edi
		mov esi,m1
		mov edi,m2
		mov ebx,mr
		mov ecx,d2
		xor	eax,eax
		Lstart__subn:
		mov eax,DWORD PTR [esi]
		mov edx,DWORD PTR [edi]
		sbb eax,edx
		mov DWORD PTR [ebx],eax
		lea esi,DWORD PTR [esi+4]
		lea edi,DWORD PTR [edi+4]
		lea ebx,DWORD PTR [ebx+4]
		dec ecx
		jnz Lstart__subn
		pop	edi
		pop	esi
		mov eax,0
		adc eax,eax
		mov br,eax
		}
#else
		asm volatile("\
		movl	%1,%%esi;\
		movl	%2,%%edi;\
		movl	%3,%%ebx;\
		movl	%4,%%ecx;\
		testl	%%eax,%%eax;\
		Lstart__subn:;\
		movl	(%%esi),%%eax;\
		movl	(%%edi),%%edx;\
		sbbl	%%edx,%%eax;\
		movl	%%eax,(%%ebx);\
		leal	4(%%esi),%%esi;\
		leal	4(%%edi),%%edi;\
		leal	4(%%ebx),%%ebx;\
		decl	%%ecx;\
		jnz Lstart__subn;\
		movl	$0,%%eax;\
		adcl	%%eax,%%eax;\
		movl	%%eax,%0"\
		:"=m"(br)\
		:"m"(m1),"m"(m2),"m"(mr),"m"(d2)\
		:"eax","ebx","ecx","edx","esi","edi");
#endif
		for ( i = d2, m1 += d2, mr += d2; (i < d1) && br; i++ ) {
			t = *m1++;
			tmp = t - br;
			br = tmp > t ? 1 : 0;
			*mr++ = tmp;
		}
		for ( ; i < d1; i++ )
			*mr++ = *m1++;
		for ( i = d1-1, mr--; i >= 0 && !*mr--; i-- );
		PL(nr) = i + 1;
		return sgn;
	}
}
#else

void addn(n1,n2,nr)
N n1,n2,*nr;
{
	unsigned int *m1,*m2,*mr,i,c;
	N r;
	int d1,d2;
	unsigned int tmp;

	if ( !n1 )
		COPY(n2,*nr);
	else if ( !n2 )
		COPY(n1,*nr);
	else {
		if ( PL(n1) > PL(n2) ) {	
			d1 = PL(n1); d2 = PL(n2); m1 = BD(n1); m2 = BD(n2);
		} else {
			d1 = PL(n2); d2 = PL(n1); m1 = BD(n2); m2 = BD(n1);
		}
		*nr = r = NALLOC(d1 + 1); INITRC(r);
		for ( i = 0, c = 0, mr = BD(r); i < d2; i++, m1++, m2++, mr++ ) {
			tmp = *m1 + *m2;
			if ( tmp < *m1 ) {
				tmp += c;
				c = 1;
			} else {
				tmp += c;
				c = tmp < c ? 1 : 0;
			}
			*mr = tmp;
		}
		for ( ; (i < d1) && c ; i++, m1++, mr++ ) {
			tmp = *m1 + c;
			c = tmp < c ? 1 : 0;
			*mr = tmp;
		}
		for ( ; i < d1; i++ ) 
			*mr++ = *m1++;
		*mr = c;
		PL(r) = (c?d1+1:d1);
	}
}

int subn(n1,n2,nr)
N n1,n2,*nr;
{
	N r;
	unsigned int *m1,*m2,*mr,i,br;
	L tmp;
	int d1,d2,nz,sgn;

	if ( !n1 ) {
		if ( n2 ) {
			COPY(n2,*nr);
			return -1;
		} else {
			*nr = 0;
			return 0;
		}
	} else if ( !n2 ) {
		COPY(n1,*nr);
		return 1;
	} else {
		switch ( cmpn(n1,n2) ) {
			case 1:
				d1 = PL(n1); d2 = PL(n2); m1 = BD(n1); m2 = BD(n2);
				sgn = 1; break;
			case -1:
				d1 = PL(n2); d2 = PL(n1); m1 = BD(n2); m2 = BD(n1);
				sgn = -1; break;
			case 0:
			default:
				*nr = 0; return ( 0 ); break;
		}
		*nr = r = NALLOC(d1); INITRC(r);
		for ( i = 0, br = 0, nz = -1, mr = BD(r); 
			i < d2; i++ ) {
			if ( (tmp = (L)*m1++ - (L)*m2++ - (L)br) && ( tmp > -LBASE ) ) 
				nz = i;
			if ( tmp < 0 ) {
				br = 1; *mr++ = (unsigned int)(tmp + LBASE);
			} else {
				br = 0; *mr++ = (unsigned int)tmp;
			}
		}
		for ( ; (i < d1) && br; i++ ) {
			if ( (tmp = (L)*m1++ - (L)br) && ( tmp > -LBASE ) ) 
				nz = i;
			if ( tmp < 0 ) {
				br = 1; *mr++ = (unsigned int)(tmp + LBASE);
			} else {
				br = 0; *mr++ = (unsigned int)tmp;
			}
		}
		for ( ; i < d1; i++ )
			if ( *mr++ = *m1++ )
				nz = i;
		PL(r) = nz + 1;
		return sgn;
	}
}

void _addn(n1,n2,nr)
N n1,n2,nr;
{
	unsigned int *m1,*m2,*mr,i,c;
	int d1,d2;
	unsigned int tmp;

	if ( !n1 || !PL(n1) )
		dupn(n2,nr);
	else if ( !n2 || !PL(n2) )
		dupn(n1,nr);
	else {
		if ( PL(n1) > PL(n2) ) {	
			d1 = PL(n1); d2 = PL(n2); m1 = BD(n1); m2 = BD(n2);
		} else {
			d1 = PL(n2); d2 = PL(n1); m1 = BD(n2); m2 = BD(n1);
		}
		for ( i = 0, c = 0, mr = BD(nr); i < d2; i++, m1++, m2++, mr++ ) {
			tmp = *m1 + *m2;
			if ( tmp < *m1 ) {
				tmp += c;
				c = 1;
			} else {
				tmp += c;
				c = tmp < c ? 1 : 0;
			}
			*mr = tmp;
		}
		for ( ; (i < d1) && c ; i++, m1++, mr++ ) {
			tmp = *m1 + c;
			c = tmp < c ? 1 : 0;
			*mr = tmp;
		}
		for ( ; i < d1; i++ ) 
			*mr++ = *m1++;
		*mr = c;
		PL(nr) = (c?d1+1:d1);
	}
}

int _subn(n1,n2,nr)
N n1,n2,nr;
{
	N r;
	unsigned int *m1,*m2,*mr,i,br;
	L tmp;
	int d1,d2,nz,sgn;

	if ( !n1 || !PL(n1) ) {
		if ( n2 && PL(n2) ) {
			dupn(n2,nr);
			return -1;
		} else {
			PL(nr) = 0;
			return 0;
		}
	} else if ( !n2 || !PL(n2) ) {
		dupn(n1,nr);
		return 1;
	} else {
		switch ( cmpn(n1,n2) ) {
			case 1:
				d1 = PL(n1); d2 = PL(n2); m1 = BD(n1); m2 = BD(n2);
				sgn = 1; break;
			case -1:
				d1 = PL(n2); d2 = PL(n1); m1 = BD(n2); m2 = BD(n1);
				sgn = -1; break;
			case 0:
			default:
				PL(nr) = 0; return ( 0 ); break;
		}
		for ( i = 0, br = 0, nz = -1, mr = BD(nr); 
			i < d2; i++ ) {
			if ( (tmp = (L)*m1++ - (L)*m2++ - (L)br) && ( tmp > -LBASE ) ) 
				nz = i;
			if ( tmp < 0 ) {
				br = 1; *mr++ = (unsigned int)(tmp + LBASE);
			} else {
				br = 0; *mr++ = (unsigned int)tmp;
			}
		}
		for ( ; (i < d1) && br; i++ ) {
			if ( (tmp = (L)*m1++ - (L)br) && ( tmp > -LBASE ) ) 
				nz = i;
			if ( tmp < 0 ) {
				br = 1; *mr++ = (unsigned int)(tmp + LBASE);
			} else {
				br = 0; *mr++ = (unsigned int)tmp;
			}
		}
		for ( ; i < d1; i++ )
			if ( *mr++ = *m1++ )
				nz = i;
		PL(nr) = nz + 1;
		return sgn;
	}
}
#endif

/* a2 += a1; n2 >= n1 */

void addarray_to(a1,n1,a2,n2)
unsigned int *a1;
int n1;
unsigned int *a2;
int n2;
{
	int i;
	unsigned int c,tmp;

	for ( i = 0, c = 0; i < n2; i++, a1++, a2++ ) {
		tmp = *a1 + *a2;
		if ( tmp < *a1 ) {
			tmp += c;
			c = 1;
		} else {
			tmp += c;
			c = tmp < c ? 1 : 0;
		}
		*a2 = tmp;
	}
	for ( ; (i < n2) && c ; i++, a2++ ) {
		tmp = *a2 + c;
		c = tmp < c ? 1 : 0;
		*a2 = tmp;
	}
	if ( i == n2 && c )
		*a2 = c;
}

void pwrn(n,e,nr)
N n,*nr;
int e;
{
	N nw,nw1;

	if ( e == 1 ) {
		COPY(n,*nr);
		return;
	}
	pwrn(n,e / 2,&nw);
	if ( e % 2 == 0 ) 
		kmuln(nw,nw,nr);
	else {
		kmuln(nw,nw,&nw1); kmuln(nw1,n,nr); FREEN(nw1);
	}
	FREEN(nw);
}

extern int igcd_algorithm;

void gcdEuclidn();

void gcdn(n1,n2,nr)
N n1,n2,*nr;
{
	N m1,m2,g;

	if ( !igcd_algorithm )
		gcdEuclidn(n1,n2,nr);
	else {
		if ( !n1 )
			*nr = n2;
		else if ( !n2 )
			*nr = n1;
		else {
			n32ton27(n1,&m1);
			n32ton27(n2,&m2);
			gcdBinary_27n(m1,m2,&g);
			n27ton32(g,nr);
		}
	}
}

void gcdEuclidn(n1,n2,nr)
N n1,n2,*nr;
{
	N m1,m2,q,r;
	unsigned int i1,i2,ir;

	if ( !n1 )
		COPY(n2,*nr);
	else if ( !n2 )
		COPY(n1,*nr);
	else {
		if ( PL(n1) > PL(n2) ) {
			COPY(n1,m1); COPY(n2,m2);
		} else {
			COPY(n1,m2); COPY(n2,m1);
		}
		while ( PL(m1) > 1 ) {
			divn(m1,m2,&q,&r); FREEN(m1); FREEN(q);
			if ( !r ) {
				*nr = m2;
				return;
			} else {
				m1 = m2; m2 = r;
			}
		}
		for ( i1 = BD(m1)[0], i2 = BD(m2)[0]; ir = i1 % i2; i1 = i2, i2 = ir );
		if ( i2 == 1 )
			COPY(ONEN,*nr);
		else {
			*nr = r = NALLOC(1); INITRC(r); PL(r) = 1; BD(r)[0] = i2;
		}
	}
}

int cmpn(n1,n2)
N n1,n2;
{
	int i;
	unsigned int *m1,*m2;

	if ( !n1 ) 
		if ( !n2 ) 
			return 0;
		else 
			return -1;
	else if ( !n2 ) 
		return 1;
	else if ( PL(n1) > PL(n2) )
		return 1;
	else if ( PL(n1) < PL(n2) ) 
		return -1;
	else {	
		for ( i = PL(n1)-1, m1 = BD(n1)+i, m2 = BD(n2)+i;
			i >= 0; i--, m1--, m2-- ) 
			if ( *m1 > *m2 ) 
				return 1;
			else if ( *m1 < *m2 )
				return -1;
		return 0;
	}
}

void bshiftn(n,b,r)
N n;
int b;
N *r;
{
	int w,l,nl,i,j;
	N z;
	unsigned int msw;
	unsigned int *p,*pz;

	if ( b == 0 ) {
		COPY(n,*r); return;
	}
	if ( b > 0 ) { /* >> */
		w = b / BSH; l = PL(n)-w;
		if ( l <= 0 ) {
			*r = 0; return;
		}
		b %= BSH; p = BD(n)+w;
		if ( !b ) {
			*r = z = NALLOC(l); INITRC(z); PL(z) = l;
			bcopy(p,BD(z),l*sizeof(unsigned int));
			return;
		}
		msw = p[l-1];
		for ( i = BSH-1; !(msw&(((unsigned int)1)<<i)); i-- ); i++;
		if ( b >= i ) {
			l--;
			if ( !l ) {
				*r = 0; return;
			}
			*r = z = NALLOC(l); INITRC(z); PL(z) = l; pz = BD(z);
			for ( j = 0; j < l; j++, p++ )
				*pz++ = (*(p+1)<<(BSH-b))|(*p>>b);
		} else {
			*r = z = NALLOC(l); INITRC(z); PL(z) = l; pz = BD(z);
			for ( j = 1; j < l; j++, p++ )
				*pz++ = (*(p+1)<<(BSH-b))|(*p>>b);
			*pz = *p>>b;
		}
	} else { /* << */
		b = -b;
		w = b / BSH; b %= BSH; l = PL(n); p = BD(n);
		if ( !b ) {
			nl = l+w; *r = z = NALLOC(nl); INITRC(z); PL(z) = nl;
			bzero((char *)BD(z),w*sizeof(unsigned int));
			bcopy(p,BD(z)+w,l*sizeof(unsigned int));
			return;
		}
		msw = p[l-1];
		for ( i = BSH-1; !(msw&(((unsigned int)1)<<i)); i-- ); i++;
		if ( b + i > BSH ) {
			nl = l+w+1;
			*r = z = NALLOC(nl); INITRC(z); PL(z) = nl; pz = BD(z)+w;
			bzero((char *)BD(z),w*sizeof(unsigned int));
			*pz++ = *p++<<b;
			for ( j = 1; j < l; j++, p++ )
				*pz++ = (*p<<b)|(*(p-1)>>(BSH-b));
			*pz = *(p-1)>>(BSH-b);
		} else {
			nl = l+w;
			*r = z = NALLOC(nl); INITRC(z); PL(z) = nl; pz = BD(z)+w;
			bzero((char *)BD(z),w*sizeof(unsigned int));
			*pz++ = *p++<<b;
			for ( j = 1; j < l; j++, p++ )
				*pz++ = (*p<<b)|(*(p-1)>>(BSH-b));
		}
	}
}

#if 0
void _bshiftn(n,b,z)
N n;
int b;
N z;
{
	int w,l,nl,i,j;
	unsigned int msw;
	unsigned int *p,*pz;

	if ( b == 0 ) {
		copyn(n,PL(n),BD(z)); PL(z) = PL(n); return;
	}
	if ( b > 0 ) { /* >> */
		w = b / BSH; l = PL(n)-w;
		if ( l <= 0 ) {
			PL(z) = 0; return;
		}
		b %= BSH; p = BD(n)+w;
		if ( !b ) {
			PL(z) = l;
			bcopy(p,BD(z),l*sizeof(unsigned int));
			return;
		}
		msw = p[l-1];
		for ( i = BSH-1; !(msw&(((unsigned int)1)<<i)); i-- ); i++;
		if ( b >= i ) {
			l--;
			if ( !l ) {
				PL(z) = 0; return;
			}
			PL(z) = l; pz = BD(z);
			for ( j = 0; j < l; j++, p++ )
				*pz++ = (*(p+1)<<(BSH-b))|(*p>>b);
		} else {
			PL(z) = l; pz = BD(z);
			for ( j = 1; j < l; j++, p++ )
				*pz++ = (*(p+1)<<(BSH-b))|(*p>>b);
			*pz = *p>>b;
		}
	} else { /* << */
		b = -b;
		w = b / BSH; b %= BSH; l = PL(n); p = BD(n);
		if ( !b ) {
			nl = l+w; PL(z) = nl;
			bzero((char *)BD(z),w*sizeof(unsigned int));
			bcopy(p,BD(z)+w,l*sizeof(unsigned int));
			return;
		}
		msw = p[l-1];
		for ( i = BSH-1; !(msw&(((unsigned int)1)<<i)); i-- ); i++;
		if ( b + i > BSH ) {
			nl = l+w+1;
			PL(z) = nl; pz = BD(z)+w;
			bzero((char *)BD(z),w*sizeof(unsigned int));
			*pz++ = *p++<<b;
			for ( j = 1; j < l; j++, p++ )
				*pz++ = (*p<<b)|(*(p-1)>>(BSH-b));
			*pz = *(p-1)>>(BSH-b);
		} else {
			nl = l+w;
			PL(z) = nl; pz = BD(z)+w;
			bzero((char *)BD(z),w*sizeof(unsigned int));
			*pz++ = *p++<<b;
			for ( j = 1; j < l; j++, p++ )
				*pz++ = (*p<<b)|(*(p-1)>>(BSH-b));
		}
	}
}
#endif

void shiftn(n,w,r)
N n;
int w;
N *r;
{
	int l,nl;
	N z;

	if ( w == 0 )
		COPY(n,*r);
	else if ( w > 0 ) { /* >> */
		l = PL(n)-w;
		if ( l <= 0 )
			*r = 0;
		else {
			*r = z = NALLOC(l); INITRC(z); PL(z) = l;
			bcopy(BD(n)+w,BD(z),l*sizeof(unsigned int));
		}
	} else { /* << */
		w = -w;
		l = PL(n); nl = l+w;
		*r = z = NALLOC(nl); INITRC(z); PL(z) = nl;
		bzero((char *)BD(z),w*sizeof(unsigned int));
		bcopy(BD(n),BD(z)+w,l*sizeof(unsigned int));
	}
}

void randomn(bits,r)
int bits;
N *r;
{
	int l,i;
	unsigned int *tb;
	N t;

	l = (bits+31)>>5; /* word length */
	*r = t = NALLOC(l);
	tb = BD(t);
	for ( i = 0; i < l; i++ )
		tb[i] = mt_genrand();
	if ( bits&31 )
		tb[l-1] &= (1<<(bits&31))-1;
	for ( i = l-1; i >= 0 && !tb[i]; i-- );
	if ( i < 0 )
		*r = 0;
	else
		PL(t) = i+1;
}

void freen(n)
N n;
{
	if ( n && (n != ONEN) )
		free(n);
}

int n_bits(n)
N n;
{
	unsigned int l,i,t;

	if ( !n )
		return 0;
	l = PL(n); t = BD(n)[l-1];
	for ( i = 0; t; t>>=1, i++);
	return i + (l-1)*BSH;
}
