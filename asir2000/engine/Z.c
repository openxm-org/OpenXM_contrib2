#if 1
#include "ca.h"
#include "inline.h"
#endif

typedef struct oZ {
	int p;
	unsigned int b[1];
} *Z;

#define IMM_MAX 1073741823
#define IMM_MIN -1073741823

/* immediate int -> Z */
#define IMMTOZ(c,n) (n)=((c)>=IMM_MIN&&(c)<=IMM_MAX?((Z)(((c)<<1)|1)):immtoz(c))
/* immediate Z ? */
#define IS_IMM(c) (((unsigned int)c)&1)
/* Z can be conver to immediate ? */
#define IS_IMMZ(n) (SL(n) == 1&&BD(n)[0]<=IMM_MAX)
/* Z -> immediate Z */
#define IMMZTOZ(n,z) (z)=(Z)(SL(n)<0?(((-BD(n)[0])<<1)|1):(((BD(n)[0])<<1)|1))
/* Z -> immediate int */
#define ZTOIMM(c) (((int)(c))>>1)
#define ZALLOC(d) ((Z)MALLOC_ATOMIC(TRUESIZE(oZ,(d)-1,int)))
#define SL(n) ((n)->p)

Z immtoz(int c);
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
void mkwcz(int k,int l,Z *t);
int remzi(Z n,int m);
inline void _addz(Z n1,Z n2,Z nr);
inline void _subz(Z n1,Z n2,Z nr);
inline void _mulz(Z n1,Z n2,Z nr);
inline int _addz_main(unsigned int *m1,int d1,unsigned int *m2,int d2,unsigned int *mr);
inline int _subz_main(unsigned int *m1,int d1,unsigned int *m2,int d2,unsigned int *mr);

Z immtoz(int c)
{
	Z z;

	z = ZALLOC(1);
	if ( c < 0 ) {
		SL(z) = -1; BD(z)[0] = -c;
	} else {
		SL(z) = 1; BD(z)[0] = c;
	}
	return z;
}

int sgnz(Z n)
{
	if ( !n ) return 0;
	else if ( IS_IMM(n) ) return ZTOIMM(n)>0?1:1;
	else if ( SL(n) < 0 ) return -1;
	else return 1;
}

z_mag(Z n)
{
	int c,i;

	if ( !n ) return 0;
	else if ( IS_IMM(n) ) {
		c = ZTOIMM(n);
		if ( c < 0 ) c = -c;
		for ( i = 0; c; c >>= 1, i++ );
		return i;
	}
	else return n_bits((N)n);
}

Z qtoz(Q n)
{
	Z r,t;
	int c;

	if ( !n ) return 0;
	else if ( !INT(n) )
		error("qtoz : invalid input");
	else {
		t = (Z)NM(n);
		if ( IS_IMMZ(t) ) {
			c = SGN(n) < 0 ? -BD(t)[0] : BD(t)[0];
			IMMTOZ(c,r);
		} else {
			r = dupz((Z)t);
			if ( SGN(n) < 0 ) SL(r) = -SL(r);
		}
		return r;
	}
}

Q ztoq(Z n)
{
	Q r;
	Z nm;
	int sgn,c;

	if ( !n ) return 0;
	else if ( IS_IMM(n) ) {
		c = ZTOIMM(n);
		STOQ(c,r);
		return r;
	} else {
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
	else if ( IS_IMM(n) ) return n;
	else {
		if ( (sd = SL(n)) < 0 ) sd = -sd;
		nr = ZALLOC(sd);
		SL(nr) = SL(n);
		for ( i = 0; i < sd; i++ ) BD(nr)[i] = BD(n)[i];
		return nr;
	}
}

Z chsgnz(Z n)
{
	Z r;
	int c;

	if ( !n ) return 0;
	else if ( IS_IMM(n) ) {
		c = -ZTOIMM(n);
		IMMTOZ(c,r);
		return r;
	} else {
		r = dupz(n);
		SL(r) = -SL(r);
		return r;
	}
}


Z addz(Z n1,Z n2)
{
	int d1,d2,d,c;
	Z r,r1;
	struct oZ t;

	if ( !n1 ) return dupz(n2);
	else if ( !n2 ) return dupz(n1);
	else if ( IS_IMM(n1) ) {
		if ( IS_IMM(n2) ) {
			c = ZTOIMM(n1)+ZTOIMM(n2);
			IMMTOZ(c,r);
			return r;
		} else {
			c = ZTOIMM(n1);
			if ( c < 0 ) {
				t.p = -1; t.b[0] = -c;
			} else {
				t.p = 1; t.b[0] = c;
			}
			if ( (d2 = SL(n2)) < 0 ) d2 = -d2;
			r = ZALLOC(d2+1);
			_addz(&t,n2,r);
			return r;
		}
	} else if ( IS_IMM(n2) ) {
		c = ZTOIMM(n2);
		if ( c < 0 ) {
			t.p = -1; t.b[0] = -c;
		} else {
			t.p = 1; t.b[0] = c;
		}
		if ( (d1 = SL(n1)) < 0 ) d1 = -d1;
		r = ZALLOC(d1+1);
		_addz(n1,&t,r);
		return r;
	} else {
		if ( (d1 = SL(n1)) < 0 ) d1 = -d1;
		if ( (d2 = SL(n2)) < 0 ) d2 = -d2;
		d = MAX(d1,d2)+1;
		r = ZALLOC(d);
		_addz(n1,n2,r);
		if ( !SL(r) ) r = 0;
		else if ( IS_IMMZ(r) ) {
			IMMZTOZ(r,r1); r = r1;	
		}
		return r;
	}
}

Z subz(Z n1,Z n2)
{
	int d1,d2,d,c;
	Z r,r1;
	struct oZ t;

	if ( !n1 ) {
		r = dupz(n2);
		SL(r) = -SL(r);
		return r;
	} else if ( !n2 ) return dupz(n1);
	else if ( IS_IMM(n1) ) {
		if ( IS_IMM(n2) ) {
			c = ZTOIMM(n1)-ZTOIMM(n2);
			IMMTOZ(c,r);
			return r;
		} else {
			c = ZTOIMM(n1);
			if ( c < 0 ) {
				t.p = -1; t.b[0] = -c;
			} else {
				t.p = 1; t.b[0] = c;
			}
			if ( (d2 = SL(n2)) < 0 ) d2 = -d2;
			r = ZALLOC(d2+1);
			_subz(&t,n2,r);
			return r;
		}
	} else if ( IS_IMM(n2) ) {
		c = ZTOIMM(n2);
		if ( c < 0 ) {
			t.p = -1; t.b[0] = -c;
		} else {
			t.p = 1; t.b[0] = c;
		}
		if ( (d1 = SL(n1)) < 0 ) d1 = -d1;
		r = ZALLOC(d1+1);
		_subz(n1,&t,r);
		return r;
	} else {
		if ( (d1 = SL(n1)) < 0 ) d1 = -d1;
		if ( (d2 = SL(n2)) < 0 ) d2 = -d2;
		d = MAX(d1,d2)+1;
		r = ZALLOC(d);
		_subz(n1,n2,r);
		if ( !SL(r) ) r = 0;
		else if ( IS_IMMZ(r) ) {
			IMMZTOZ(r,r1); r = r1;	
		}
		return r;
	}
}

Z mulz(Z n1,Z n2)
{
	int d1,d2,sgn,i;
	unsigned int u1,u2,u,l;
	Z r;

	if ( !n1 || !n2 ) return 0;

	if ( IS_IMM(n1) ) {
		sgn = 1;
		u1 = ZTOIMM(n1); if ( u1 < 0 ) { sgn = -sgn; u1 = -u1; }
		if ( IS_IMM(n2) ) {
			u2 = ZTOIMM(n2); if ( u2 < 0 ) { sgn = -sgn; u2 = -u2; }
			DM(u1,u2,u,l);
			if ( !u ) {
				IMMTOZ(l,r);
			} else {
				r = ZALLOC(2); SL(r) = 2; BD(r)[1] = u; BD(r)[0] = l;
			}
		} else {
			if ( (d2 = SL(n2)) < 0 ) { sgn = -sgn; d2 = -d2; }
			r = ZALLOC(d2+1);
			for ( i = d2; i >= 0; i-- ) BD(r)[i] = 0;
			muln_1(BD(n2),d2,u1,BD(r));
			SL(r) = BD(r)[d2]?d2+1:d2;
		}
	} else if ( IS_IMM(n2) ) {
		sgn = 1;
		u2 = ZTOIMM(n2); if ( u2 < 0 ) { sgn = -sgn; u2 = -u2; }
		if ( (d1 = SL(n1)) < 0 ) { sgn = -sgn; d1 = -d1; }
		r = ZALLOC(d1+1);
		for ( i = d1; i >= 0; i-- ) BD(r)[i] = 0;
		muln_1(BD(n1),d1,u2,BD(r));
		SL(r) = BD(r)[d1]?d1+1:d1;
	} else {
		sgn = 1;
		if ( (d1 = SL(n1)) < 0 ) { sgn = -sgn; d1 = -d1; }
		if ( (d2 = SL(n2)) < 0 ) { sgn = -sgn; d2 = -d2; }
		r = ZALLOC(d1+d2);
		_mulz(n1,n2,r);
	}
	if ( sgn < 0 ) SL(r) = -SL(r);
	return r;
}

/* kokokara */
#if 0
Z divsz(Z n1,Z n2)
{
	int sgn,d1,d2;
	Z q;

	if ( !n2 ) error("divsz : division by 0");
	if ( !n1 ) return 0;

	if ( IS_IMM(n1) ) {
		if ( !IS_IMM(n2) )
			error("divsz : cannot happen");
		c = ZTOIMM(n1)/ZTOIMM(n2);
		IMMTOZ(c,q);
		return q;
	}
	if ( IS_IMM(n2) ) {
		sgn = 1;
		u2 = ZTOIMM(n2); if ( u2 < 0 ) { sgn = -sgn; u2 = -u2; }
		diviz(n1,u2,&q);
		if ( sgn < 0 ) SL(q) = -SL(q);
		return q;
	}

	sgn = 1;
	if ( (d2 = SL(n2)) < 0 ) { sgn = -sgn; d2 = -d2; }
	if ( d2 == 1 ) {
		diviz(n1,BD(u2)[0],&q);
		if ( sgn < 0 ) SL(q) = -SL(q);
		return q;
	}
	if ( (d1 = SL(n1)) < 0 ) { sgn = -sgn; d1 = -d1; }
	if ( d1 < d2 ) error("divsz : cannot happen");
	return q;
}

Z gcdz(Z n1,Z n2)
{
	int d1,d2;
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
#endif

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
	int d1,d2;

	if ( !n1 || !SL(n1) ) _copyz(n2,nr);
	else if ( !n2 || !SL(n2) ) _copyz(n1,nr);
	else if ( (d1=SL(n1)) > 0 )
		if ( (d2=SL(n2)) > 0 )
			SL(nr) = _addz_main(BD(n1),d1,BD(n2),d2,BD(nr));
		else 
			SL(nr) = _subz_main(BD(n1),d1,BD(n2),-d2,BD(nr));
	else if ( (d2=SL(n2)) > 0 )
		SL(nr) = _subz_main(BD(n2),d2,BD(n1),-d1,BD(nr));
	else
		SL(nr) = -_addz_main(BD(n1),-d1,BD(n2),-d2,BD(nr));
}

void _subz(Z n1,Z n2,Z nr)
{
	int d1,d2;

	if ( !n1 || !SL(n1) ) _copyz(n2,nr);
	else if ( !n2 || !SL(n2) ) {
		_copyz(n1,nr);
		SL(nr) = -SL(nr);
	} else if ( (d1=SL(n1)) > 0 )
		if ( (d2=SL(n2)) > 0 )
			SL(nr) = _subz_main(BD(n1),d1,BD(n2),d2,BD(nr));
		else 
			SL(nr) = _addz_main(BD(n1),d1,BD(n2),-d2,BD(nr));
	else if ( (d2=SL(n2)) > 0 )
		SL(nr) = -_addz_main(BD(n1),-d1,BD(n2),d2,BD(nr));
	else
		SL(nr) = -_subz_main(BD(n1),-d1,BD(n2),-d2,BD(nr));
}

void _mulz(Z n1,Z n2,Z nr)
{
	int d1,d2;
	int i,d,sgn;
	unsigned int mul;
	unsigned int *m1,*m2;

	if ( !n1 || !SL(n1) || !n2 || !SL(n2) )
		SL(nr) = 0;
	else {
		d1 = SL(n1); d2 = SL(n2);
		sgn = 1;
		if ( d1 < 0 ) { sgn = -sgn; d1 = -d1; }
		if ( d2 < 0 ) { sgn = -sgn; d2 = -d2; }
		d = d1+d2;
		for ( i = d-1, m1 = BD(nr); i >= 0; i-- ) *m1++ = 0;
		for ( i = 0, m1 = BD(n1), m2 = BD(n2); i < d2; i++, m2++ )
			if ( mul = *m2 ) muln_1(m1,d1,mul,BD(nr)+i);
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
	int sd,u;

	if ( !n )
		fprintf(asir_out,"0");
	else if ( IS_IMM(n) ) {
		u = ZTOIMM(n);
		fprintf(asir_out,"%d",u);
	} else {
		if ( (sd = SL(n)) < 0 ) { SL(n) = -SL(n); fprintf(asir_out,"-"); }
		printn((N)n);
		if ( sd < 0 ) SL(n) = -SL(n);
	}
}

/*
 *  Dx^k*x^l = W(k,l,0)*x^l*Dx^k+W(k,l,1)*x^(l-1)*x^(k-1)*+...
 *
 *  t = [W(k,l,0) W(k,l,1) ... W(k,l,min(k,l)]
 *  where W(k,l,i) = i! * kCi * lCi
 */

void mkwcz(int k,int l,Z *t)
{
	int i,n,up,low;
	N nm,d,c;

	n = MIN(k,l);
	for ( t[0] = (Z)ONEN, i = 1; i <= n; i++ ) {
		DM(k-i+1,l-i+1,up,low);
		if ( up ) {
			nm = NALLOC(2); PL(nm) = 2; BD(nm)[0] = low; BD(nm)[1] = up;
		} else {
			nm = NALLOC(1); PL(nm) = 1; BD(nm)[0] = low;
		}
		kmuln((N)t[i-1],nm,&d); divin(d,i,&c); t[i] = (Z)c;
	}
}
