#include "ca.h"
#include "inline.h"

typedef struct oZ {
	int p;
	unsigned int b[1];
} *Z;

#define ZALLOC(d) ((Z)MALLOC_ATOMIC(TRUESIZE(oZ,(d)-1,int)))
#define SL(n) ((n)->p)

Z qtoz(Q n);
Q ztoq(Z n);
Z chsgnz(Z n);
Z dupz(Z n);
Z addz(Z n1,Z n2);
Z subz(Z n1,Z n2);
Z mulz(Z n1,Z n2);
Z divsz(Z n1,Z n2);
Z divz(Z n1,Z n2,Z *rem);
Z gcdz(Z n1,Z n2);
Z gcdz_cofactor(Z n1,Z n2,Z *c1,Z *c2);
Z estimate_array_gcdz(Z *a,int n);
Z array_gcdz(Z *a,int n);
int remzi(Z n,int m);
inline void _addz(Z n1,Z n2,Z nr);
inline void _subz(Z n1,Z n2,Z nr);
inline void _mulz(Z n1,Z n2,Z nr);
inline int _addz_main(unsigned int *m1,int d1,unsigned int *m2,int d2,unsigned int *mr);
inline int _subz_main(unsigned int *m1,int d1,unsigned int *m2,int d2,unsigned int *mr);

z_mag(Z n)
{
	return n_bits((N)n);
}

Z qtoz(Q n)
{
	Z r;

	if ( !n ) return 0;
	else if ( !INT(n) )
		error("qtoz : invalid input");
	else {
		r = dupz((Z)NM(n));
		if ( SGN(n) < 0 ) SL(r) = -SL(r);
		return r;
	}
}

Q ztoq(Z n)
{
	Q r;
	Z nm;
	int sgn;

	if ( !n ) return 0;
	else {
		nm = dupz(n);
		if ( SL(nm) < 0 ) {
			sgn = -1;
			SL(nm) = -SL(nm);
		} else
			sgn = 1;
		NTOQ((N)nm,sgn,r);
		return r;
	}
}

Z dupz(Z n)
{
	Z nr;
	int sd,i;

	if ( !n ) return 0;
	if ( (sd = SL(n)) < 0 ) sd = -sd;
	nr = ZALLOC(sd);
	SL(nr) = SL(n);
	for ( i = 0; i < sd; i++ ) BD(nr)[i] = BD(n)[i];
	return nr;
}

Z chsgnz(Z n)
{
	Z r;

	if ( !n ) return 0;
	else {
		r = dupz(n);
		SL(r) = -SL(r);
		return r;
	}
}

Z addz(Z n1,Z n2)
{
	unsigned int u1,u2,t;
	int sd1,sd2,d,d1,d2;
	Z nr;

	if ( !n1 ) return dupz(n2);
	else if ( !n2 ) return dupz(n1);
	else {
		d1 = ((sd1 = SL(n1))< 0)?-sd1:sd1;
		d2 = ((sd2 = SL(n2))< 0)?-sd2:sd2;
		if ( d1 == 1 && d2 == 1 ) {
			u1 = BD(n1)[0]; u2 = BD(n2)[0];
			if ( sd1 > 0 ) {
				if ( sd2 > 0 ) {
					t = u1+u2;
					if ( t < u1 ) {
						nr=ZALLOC(2); SL(nr) = 2; BD(nr)[1] = 1;
					} else {
						nr=ZALLOC(1); SL(nr) = 1;
					}
					BD(nr)[0] = t;
				} else {
					if ( u1 == u2 ) nr = 0;
					else if ( u1 > u2 ) {
						nr=ZALLOC(1); SL(nr) = 1; BD(nr)[0] = u1-u2;
					} else {
						nr=ZALLOC(1); SL(nr) = -1; BD(nr)[0] = u2-u1;
					}
				}
			} else {
				if ( sd2 > 0 ) {
					if ( u2 == u1 ) nr = 0;
					else if ( u2 > u1 ) {
						nr=ZALLOC(1); SL(nr) = 1; BD(nr)[0] = u2-u1;
					} else {
						nr=ZALLOC(1); SL(nr) = -1; BD(nr)[0] = u1-u2;
					}
				} else {
					t = u1+u2;
					if ( t < u1 ) {
						nr=ZALLOC(2); SL(nr) = -2; BD(nr)[1] = 1;
					} else {
						nr=ZALLOC(1); SL(nr) = -1;
					}
					BD(nr)[0] = t;
				}
			}
		} else {
			d = MAX(d1,d2)+1;
			nr = ZALLOC(d);
			_addz(n1,n2,nr);
			if ( !SL(nr) ) nr = 0;
		}
		return nr;
	}
}

Z subz(Z n1,Z n2)
{
	int sd1,sd2,d;
	Z nr;

	if ( !n1 )
		if ( !n2 ) return 0;
		else {
			nr = dupz(n2);
			SL(nr) = -SL(nr);
		}
	else if ( !n2 ) return dupz(n1);
	else {
		if ( (sd1 = SL(n1)) < 0 ) sd1 = -sd1;
		if ( (sd2 = SL(n2)) < 0 ) sd2 = -sd2;
		d = MAX(sd1,sd2)+1;
		nr = ZALLOC(d);
		_subz(n1,n2,nr);
		if ( !SL(nr) ) nr = 0;
		return nr;
	}
}

Z mulz(Z n1,Z n2)
{
	int sd1,sd2,d1,d2;
	unsigned int u1,u2,u,l;
	Z nr;

	if ( !n1 || !n2 ) return 0;
	else {
		d1 = ((sd1 = SL(n1))< 0)?-sd1:sd1;
		d2 = ((sd2 = SL(n2))< 0)?-sd2:sd2;
		if ( d1 == 1 && d2 == 1 ) {
			u1 = BD(n1)[0]; u2 = BD(n2)[0];
			DM(u1,u2,u,l);
			if ( u ) {
				nr = ZALLOC(2); SL(nr) = sd1*sd2*2; BD(nr)[1] = u;
			} else {
				nr = ZALLOC(1); SL(nr) = sd1*sd2;
			}
			BD(nr)[0] = l;
		} else {
			nr = ZALLOC(d1+d2);
			_mulz(n1,n2,nr);
		}
		return nr;
	}
}

/* XXX */
Z divz(Z n1,Z n2,Z *rem)
{
	int sgn,sd1,sd2;
	N q,r;

	if ( !n2 ) error("divz : division by 0");
	else if ( !n1 ) {
		*rem = 0; return 0;
	} else {
		sd2 = SL(n2);
		if ( sd2 == 1 && BD(n2)[0] == 1 ) {
			*rem = 0; return n1;
		} else if ( sd2 == -1 && BD(n2)[0] == 1 ) {
			*rem = 0; return chsgnz(n1);
		}

		sd1 = SL(n1);
		n1 = dupz(n1);
		if ( SL(n1) < 0 ) SL(n1) = -SL(n1);
		divn((N)n1,(N)n2,&q,&r);
		if ( q && ((sd1>0&&sd2<0)||(sd1<0&&sd2>0)) ) SL((Z)q) = -SL((Z)q);
		if ( r && sd1 < 0 ) SL((Z)r) = -SL((Z)r);
		*rem = (Z)r;
		return (Z)q;
	}
}

Z divsz(Z n1,Z n2)
{
	int sgn,sd1,sd2;
	N q;

	if ( !n2 ) error("divsz : division by 0");
	else if ( !n1 ) return 0;
	else {
		sd2 = SL(n2);
		if ( sd2 == 1 && BD(n2)[0] == 1 ) return n1;
		else if ( sd2 == -1 && BD(n2)[0] == 1 ) return chsgnz(n1);
			
		sd1 = SL(n1);
		n1 = dupz(n1);
		if ( SL(n1) < 0 ) SL(n1) = -SL(n1);
		divsn((N)n1,(N)n2,&q);
		if ( q && ((sd1>0&&sd2<0)||(sd1<0&&sd2>0)) ) SL((Z)q) = -SL((Z)q);
		return (Z)q;
	}
}

Z gcdz(Z n1,Z n2)
{
	int sd1,sd2;
	N gcd;

	if ( !n1 ) return n2;
	else if ( !n2 ) return n1;

	n1 = dupz(n1);
	if ( SL(n1) < 0 ) SL(n1) = -SL(n1);
	n2 = dupz(n2);
	if ( SL(n2) < 0 ) SL(n2) = -SL(n2);
	gcdn((N)n1,(N)n2,&gcd);	
	return (Z)gcd;
}

int remzi(Z n,int m)
{
	unsigned int *x;
	unsigned int t,r;
	int i;

	if ( !n ) return 0;
	i = SL(n);
	if ( i < 0 ) i = -i;
	for ( i--, x = BD(n)+i, r = 0; i >= 0; i--, x-- ) {
#if defined(sparc)
		r = dsa(m,r,*x);
#else
		DSAB(m,r,*x,t,r);
#endif
	}
	return r;
}

Z gcdz_cofactor(Z n1,Z n2,Z *c1,Z *c2)
{
	Z gcd;

	gcd = gcdz(n1,n2);
	*c1 = divsz(n1,gcd);
	*c2 = divsz(n2,gcd);
	return gcd;
}

Z estimate_array_gcdz(Z *b,int n)
{
	int m,i,j,sd;
	Z *a;
	Z s,t;

	a = (Z *)ALLOCA(n*sizeof(Z));
	for ( i = j = 0; i < n; i++ ) if ( b[i] ) a[j++] = b[i];
	n = j;
	if ( !n ) return 0;
	if ( n == 1 ) return a[0];

	m = n/2;
	for ( i = 0, s = 0; i < m; i++ ) {
		if ( !a[i] ) continue;
		else s = (SL(a[i])<0)?subz(s,a[i]):addz(s,a[i]);
	}
	for ( t = 0; i < n; i++ ) {
		if ( !a[i] ) continue;
		else t = (SL(a[i])<0)?subz(t,a[i]):addz(t,a[i]);
	}
	return gcdz(s,t);
}

Z array_gcdz(Z *b,int n)
{
	int m,i,j,sd;
	Z *a;
	Z gcd;

	a = (Z *)ALLOCA(n*sizeof(Z));
	for ( i = j = 0; i < n; i++ ) if ( b[i] ) a[j++] = b[i];
	n = j;
	if ( !n ) return 0;
	if ( n == 1 ) return a[0];
	gcd = a[0];
	for ( i = 1; i < n; i++ )
		gcd = gcdz(gcd,a[i]);
	return gcd;
}

void _copyz(Z n1,Z n2)
{
	int n,i;

	if ( !n1 || !SL(n1) ) SL(n2) = 0;
	else {
		n = SL(n2) = SL(n1);
		if ( n < 0 ) n = -n;
		for ( i = 0; i < n; i++ ) BD(n2)[i] = BD(n1)[i];
	}
}

void _addz(Z n1,Z n2,Z nr)
{
	int sd1,sd2;

	if ( !n1 || !SL(n1) ) _copyz(n2,nr);
	else if ( !n2 || !SL(n2) ) _copyz(n1,nr);
	else if ( (sd1=SL(n1)) > 0 )
		if ( (sd2=SL(n2)) > 0 )
			SL(nr) = _addz_main(BD(n1),sd1,BD(n2),sd2,BD(nr));
		else 
			SL(nr) = _subz_main(BD(n1),sd1,BD(n2),-sd2,BD(nr));
	else if ( (sd2=SL(n2)) > 0 )
		SL(nr) = _subz_main(BD(n2),sd2,BD(n1),-sd1,BD(nr));
	else
		SL(nr) = -_addz_main(BD(n1),-sd1,BD(n2),-sd2,BD(nr));
}

void _subz(Z n1,Z n2,Z nr)
{
	int sd1,sd2;

	if ( !n1 || !SL(n1) ) _copyz(n2,nr);
	else if ( !n2 || !SL(n2) ) {
		_copyz(n1,nr);
		SL(nr) = -SL(nr);
	} else if ( (sd1=SL(n1)) > 0 )
		if ( (sd2=SL(n2)) > 0 )
			SL(nr) = _subz_main(BD(n1),sd1,BD(n2),sd2,BD(nr));
		else 
			SL(nr) = _addz_main(BD(n1),sd1,BD(n2),-sd2,BD(nr));
	else if ( (sd2=SL(n2)) > 0 )
		SL(nr) = -_addz_main(BD(n1),-sd1,BD(n2),sd2,BD(nr));
	else
		SL(nr) = -_subz_main(BD(n1),-sd1,BD(n2),-sd2,BD(nr));
}

void _mulz(Z n1,Z n2,Z nr)
{
	int sd1,sd2;
	int i,d,sgn;
	unsigned int mul;
	unsigned int *m1,*m2;

	if ( !n1 || !SL(n1) || !n2 || !SL(n2) )
		SL(nr) = 0;
	else {
		sd1 = SL(n1); sd2 = SL(n2);
		sgn = 1;
		if ( sd1 < 0 ) { sgn = -sgn; sd1 = -sd1; }
		if ( sd2 < 0 ) { sgn = -sgn; sd2 = -sd2; }
		d = sd1+sd2;
		for ( i = d-1, m1 = BD(nr); i >= 0; i-- ) *m1++ = 0;
		for ( i = 0, m1 = BD(n1), m2 = BD(n2); i < sd2; i++, m2++ )
			if ( mul = *m2 ) muln_1(m1,sd1,mul,BD(nr)+i);
		SL(nr) = sgn*(BD(nr)[d-1]?d:d-1);
	}
}

int _addz_main(unsigned int *m1,int d1,unsigned int *m2,int d2,unsigned int *mr)
{
	int d,i;
	unsigned int tmp,c;
	unsigned int *t;

	if ( d2 > d1 ) {
		t = m1; m1 = m2; m2 = t;
		d = d1; d1 = d2; d2 = d;
	}
#if defined(VISUAL)
	__asm {
	push	esi
	push	edi
	mov esi,m1
	mov edi,m2
	mov ebx,mr
	mov ecx,d2
	xor	eax,eax
	Lstart__addz:
	mov eax,DWORD PTR [esi]
	mov edx,DWORD PTR [edi]
	adc eax,edx
	mov DWORD PTR [ebx],eax
	lea esi,DWORD PTR [esi+4]
	lea edi,DWORD PTR [edi+4]
	lea ebx,DWORD PTR [ebx+4]
	dec ecx
	jnz Lstart__addz
	pop	edi
	pop	esi
	mov eax,0
	adc eax,eax
	mov c,eax
	}
#elif defined(i386)
	asm volatile("\
	movl	%1,%%esi;\
	movl	%2,%%edi;\
	movl	%3,%%ebx;\
	movl	%4,%%ecx;\
	testl	%%eax,%%eax;\
	Lstart__addz:;\
	movl	(%%esi),%%eax;\
	movl	(%%edi),%%edx;\
	adcl	%%edx,%%eax;\
	movl	%%eax,(%%ebx);\
	leal	4(%%esi),%%esi;\
	leal	4(%%edi),%%edi;\
	leal	4(%%ebx),%%ebx;\
	decl	%%ecx;\
	jnz Lstart__addz;\
	movl	$0,%%eax;\
	adcl	%%eax,%%eax;\
	movl	%%eax,%0"\
	:"=m"(c)\
	:"m"(m1),"m"(m2),"m"(mr),"m"(d2)\
	:"eax","ebx","ecx","edx","esi","edi");
#else
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
#endif
	for ( i = d2, m1 += d2, mr += d2; (i < d1) && c ; i++ ) {
		tmp = *m1++ + c;
		c = tmp < c ? 1 : 0;
		*mr++ = tmp;
	}
	for ( ; i < d1; i++ ) 
			*mr++ = *m1++;
	*mr = c;
	return (c?d1+1:d1);
}

int _subz_main(unsigned int *m1,int d1,unsigned int *m2,int d2,unsigned int *mr)
{
	int d,i,sgn;
	unsigned int t,tmp,br;
	unsigned int *m;

	if ( d1 > d2 ) sgn = 1;
	else if ( d1 < d2 ) sgn = -1;
	else {
		for ( i = d1-1; i >= 0 && m1[i] == m2[i]; i-- );
		if ( i < 0 ) return 0;
		if ( m1[i] > m2[i] ) sgn = 1;
		else if ( m1[i] < m2[i] ) sgn = -1;
	}
	if ( sgn < 0 ) {
		m = m1; m1 = m2; m2 = m;
		d = d1; d1 = d2; d2 = d;
	}
#if defined(VISUAL)
	__asm {
	push	esi
	push	edi
	mov esi,m1
	mov edi,m2
	mov ebx,mr
	mov ecx,d2
	xor	eax,eax
	Lstart__subz:
	mov eax,DWORD PTR [esi]
	mov edx,DWORD PTR [edi]
	sbb eax,edx
	mov DWORD PTR [ebx],eax
	lea esi,DWORD PTR [esi+4]
	lea edi,DWORD PTR [edi+4]
	lea ebx,DWORD PTR [ebx+4]
	dec ecx
	jnz Lstart__subz
	pop	edi
	pop	esi
	mov eax,0
	adc eax,eax
	mov br,eax
	}
#elif defined(i386)
	asm volatile("\
	movl	%1,%%esi;\
	movl	%2,%%edi;\
	movl	%3,%%ebx;\
	movl	%4,%%ecx;\
	testl	%%eax,%%eax;\
	Lstart__subz:;\
	movl	(%%esi),%%eax;\
	movl	(%%edi),%%edx;\
	sbbl	%%edx,%%eax;\
	movl	%%eax,(%%ebx);\
	leal	4(%%esi),%%esi;\
	leal	4(%%edi),%%edi;\
	leal	4(%%ebx),%%ebx;\
	decl	%%ecx;\
	jnz Lstart__subz;\
	movl	$0,%%eax;\
	adcl	%%eax,%%eax;\
	movl	%%eax,%0"\
	:"=m"(br)\
	:"m"(m1),"m"(m2),"m"(mr),"m"(d2)\
	:"eax","ebx","ecx","edx","esi","edi");
#else
	for ( i = 0, br = 0, mr = BD(nr); i < d2; i++, mr++ ) {
		t = *m1++;
		tmp = *m2++ + br;
		if ( br > 0 && !tmp ) {
			/* tmp = 2^32 => br = 1 */
		}else {
			tmp = t-tmp;
			br = tmp > t ? 1 : 0;
			*mr = tmp;
		}
	}
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
	return sgn*(i+1);
}

/* XXX */

void printz(Z n)
{
	int sd;

	if ( !n )
		fprintf(asir_out,"0");
	else {
		if ( (sd = SL(n)) < 0 ) { SL(n) = -SL(n); fprintf(asir_out,"-"); }
		printn((N)n);
		if ( sd < 0 ) SL(n) = -SL(n);
	}
}
