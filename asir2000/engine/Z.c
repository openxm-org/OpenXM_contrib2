#include "ca.h"
#include "base.h"
#include "inline.h"

#if defined(__GNUC__)
#define INLINE inline
#elif defined(VISUAL)
#define INLINE __inline
#else
#define INLINE
#endif

INLINE void _addz(Z n1,Z n2,Z nr);
INLINE void _subz(Z n1,Z n2,Z nr);
INLINE void _mulz(Z n1,Z n2,Z nr);
int _addz_main(unsigned int *m1,int d1,unsigned int *m2,int d2,unsigned int *mr);
int _subz_main(unsigned int *m1,int d1,unsigned int *m2,int d2,unsigned int *mr);

/* immediate int -> Z */
#define UTOZ(c,n) (n)=(!((unsigned int)(c))?0:(((unsigned int)(c))<=IMM_MAX?((Z)((((unsigned int)(c))<<1)|1)):utoz((unsigned int)(c))))
#define STOZ(c,n) (n)=(!((int)(c))?0:(((int)(c))>=IMM_MIN&&((int)(c))<=IMM_MAX?((Z)((((int)(c))<<1)|1)):stoz((int)(c))))
/* immediate Z ? */
#define IS_IMM(c) (((unsigned int)c)&1)
/* Z can be conver to immediate ? */
#define IS_SZ(n) (((SL(n) == 1)||(SL(n)==-1))&&BD(n)[0]<=IMM_MAX)
/* Z -> immediate Z */
#define SZTOZ(n,z) (z)=(Z)(SL(n)<0?(((-BD(n)[0])<<1)|1):(((BD(n)[0])<<1)|1))
/* Z -> immediate int */
#define ZTOS(c) (((int)(c))>>1)

int uniz(Z a)
{
	if ( IS_IMM(a) && ZTOS(a) == 1 ) return 1;
	else return 0;
}

int cmpz(Z a,Z b)
{
	int *ma,*mb;
	int sa,sb,da,db,ca,cb,i;

	if ( !a )
		return -sgnz(b);
	else if ( !b )
		return sgnz(a);
	else {
		sa = sgnz(a); sb = sgnz(b);
		if ( sa > sb ) return 1;
		else if ( sa < sb ) return -1;
		else if ( IS_IMM(a) ) 
			if ( IS_IMM(b) ) {
				ca = ZTOS(a); cb = ZTOS(b);
				if ( ca > cb ) return sa;
				else if ( ca < cb ) return -sa;
				else return 0;
			} else
				return -sa;
		else if ( IS_IMM(b) )
			return sa;
		else {
			da = SL(a)*sa; db = SL(b)*sa;
			if ( da > db ) return sa;
			else if ( da < db ) return -sa;
			else {
				for ( i = da-1, ma = BD(a)+i, mb = BD(b)+i; i >= 0; i-- )
					if ( *ma > *mb ) return sa;
					else if ( *ma < *mb ) return -sa;
				return 0;
			}
		}
	}
}

Z stoz(int c)
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

Z utoz(unsigned int c)
{
	Z z;

	z = ZALLOC(1);
	SL(z) = 1; BD(z)[0] = c;
	return z;
}

Z simpz(Z n)
{
	Z n1;

	if ( !n ) return 0;
	else if ( IS_IMM(n) ) return n;
	else if ( IS_SZ(n) ) {
		SZTOZ(n,n1); return n1;
	} else
		return n;
}

int sgnz(Z n)
{
	if ( !n ) return 0;
	else if ( IS_IMM(n) ) return ZTOS(n)>0?1:-1;
	else if ( SL(n) < 0 ) return -1;
	else return 1;
}

z_mag(Z n)
{
	int c,i;

	if ( !n ) return 0;
	else if ( IS_IMM(n) ) {
		c = ZTOS(n);
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
		if ( IS_SZ(t) ) {
			c = SGN(n) < 0 ? -BD(t)[0] : BD(t)[0];
			STOZ(c,r);
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
		c = ZTOS(n);
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
	Z r;
	int sd,i;

	if ( !n ) return 0;
	else if ( IS_IMM(n) ) return n;
	else {
		if ( (sd = SL(n)) < 0 ) sd = -sd;
		r = ZALLOC(sd);
		SL(r) = SL(n);
		for ( i = 0; i < sd; i++ ) BD(r)[i] = BD(n)[i];
		return r;
	}
}

Z chsgnz(Z n)
{
	Z r;
	int c;

	if ( !n ) return 0;
	else if ( IS_IMM(n) ) {
		c = -ZTOS(n);
		STOZ(c,r);
		return r;
	} else {
		r = dupz(n);
		SL(r) = -SL(r);
		return r;
	}
}

Z absz(Z n)
{
	Z r;
	int c;

	if ( !n ) return 0;
	else if ( sgnz(n) > 0 )
		return n;
	else 
		return chsgnz(n);
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
			c = ZTOS(n1)+ZTOS(n2);
			STOZ(c,r);
		} else {
			c = ZTOS(n1);
			if ( c < 0 ) {
				t.p = -1; t.b[0] = -c;
			} else {
				t.p = 1; t.b[0] = c;
			}
			if ( (d2 = SL(n2)) < 0 ) d2 = -d2;
			r = ZALLOC(d2+1);
			_addz(&t,n2,r);
			if ( !SL(r) ) r = 0;
		}
	} else if ( IS_IMM(n2) ) {
		c = ZTOS(n2);
		if ( c < 0 ) {
			t.p = -1; t.b[0] = -c;
		} else {
			t.p = 1; t.b[0] = c;
		}
		if ( (d1 = SL(n1)) < 0 ) d1 = -d1;
		r = ZALLOC(d1+1);
		_addz(n1,&t,r);
		if ( !SL(r) ) r = 0;
	} else {
		if ( (d1 = SL(n1)) < 0 ) d1 = -d1;
		if ( (d2 = SL(n2)) < 0 ) d2 = -d2;
		d = MAX(d1,d2)+1;
		r = ZALLOC(d);
		_addz(n1,n2,r);
		if ( !SL(r) ) r = 0;
	}
	if ( r && !((int)r&1) && IS_SZ(r) ) {
		SZTOZ(r,r1); r = r1;	
	}
	return r;
}

Z subz(Z n1,Z n2)
{
	int d1,d2,d,c;
	Z r,r1;
	struct oZ t;

	if ( !n1 )
		return chsgnz(n2);
	else if ( !n2 ) return n1;
	else if ( IS_IMM(n1) ) {
		if ( IS_IMM(n2) ) {
			c = ZTOS(n1)-ZTOS(n2);
			STOZ(c,r);
		} else {
			c = ZTOS(n1);
			if ( c < 0 ) {
				t.p = -1; t.b[0] = -c;
			} else {
				t.p = 1; t.b[0] = c;
			}
			if ( (d2 = SL(n2)) < 0 ) d2 = -d2;
			r = ZALLOC(d2+1);
			_subz(&t,n2,r);
			if ( !SL(r) ) r = 0;
		}
	} else if ( IS_IMM(n2) ) {
		c = ZTOS(n2);
		if ( c < 0 ) {
			t.p = -1; t.b[0] = -c;
		} else {
			t.p = 1; t.b[0] = c;
		}
		if ( (d1 = SL(n1)) < 0 ) d1 = -d1;
		r = ZALLOC(d1+1);
		_subz(n1,&t,r);
		if ( !SL(r) ) r = 0;
	} else {
		if ( (d1 = SL(n1)) < 0 ) d1 = -d1;
		if ( (d2 = SL(n2)) < 0 ) d2 = -d2;
		d = MAX(d1,d2)+1;
		r = ZALLOC(d);
		_subz(n1,n2,r);
		if ( !SL(r) ) r = 0;
	}
	if ( r && !((int)r&1) && IS_SZ(r) ) {
		SZTOZ(r,r1); r = r1;	
	}
	return r;
}

Z mulz(Z n1,Z n2)
{
	int d1,d2,sgn,i;
	int c1,c2;
	unsigned int u1,u2,u,l;
	Z r;

	if ( !n1 || !n2 ) return 0;

	if ( IS_IMM(n1) ) {
		c1 = ZTOS(n1);
		if ( IS_IMM(n2) ) {
			c2 = ZTOS(n2);
			if ( c1 == 1 )
				return n2;
			else if ( c1 == -1 )
				return chsgnz(n2);
			else if ( c2 == 1 )
				return n1;
			else if ( c2 == -1 )
				return chsgnz(n1);
			else {
				sgn = 1;
				if ( c1 < 0 ) { c1 = -c1; sgn = -sgn; }
				if ( c2 < 0 ) { c2 = -c2; sgn = -sgn; }
				u1 = (unsigned int)c1; u2 = (unsigned int)c2;
				DM(u1,u2,u,l);
				if ( !u ) {
					UTOZ(l,r);
				} else {
					r = ZALLOC(2); SL(r) = 2; BD(r)[1] = u; BD(r)[0] = l;
				}
			}
		} else {
			sgn = 1;
			if ( c1 < 0 ) { c1 = -c1; sgn = -sgn; }
			u1 = (unsigned int)c1;
			if ( (d2 = SL(n2)) < 0 ) { sgn = -sgn; d2 = -d2; }
			r = ZALLOC(d2+1);
			for ( i = d2; i >= 0; i-- ) BD(r)[i] = 0;
			muln_1(BD(n2),d2,u1,BD(r));
			SL(r) = BD(r)[d2]?d2+1:d2;
		}
	} else if ( IS_IMM(n2) ) {
		c2 = ZTOS(n2);
		if ( c2 == 1 )
			return n1;
		else if ( c2 == -1 )
			return chsgnz(n1);

		sgn = 1;
		if ( c2 < 0 ) { sgn = -sgn; c2 = -c2; }
		u2 = (unsigned int)c2;
		if ( (d1 = SL(n1)) < 0 ) { sgn = -sgn; d1 = -d1; }
		r = ZALLOC(d1+1);
		for ( i = d1; i >= 0; i-- ) BD(r)[i] = 0;
		muln_1(BD(n1),d1,u2,BD(r));
		SL(r) = BD(r)[d1]?d1+1:d1;
	} else {
		sgn = 1;
		if ( (d1 = SL(n1)) < 0 ) d1 = -d1;
		if ( (d2 = SL(n2)) < 0 ) d2 = -d2;
		r = ZALLOC(d1+d2);
		_mulz(n1,n2,r);
	}
	if ( sgn < 0 ) r = chsgnz(r);
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
		c = ZTOS(n1)/ZTOS(n2);
		STOZ(c,q);
		return q;
	}
	if ( IS_IMM(n2) ) {
		sgn = 1;
		u2 = ZTOS(n2); if ( u2 < 0 ) { sgn = -sgn; u2 = -u2; }
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
#endif

/* XXX */
Z divz(Z n1,Z n2,Z *r)
{
	int s1,s2;
	Q t1,t2,qq,rq;
	N qn,rn;

	if ( !n1 ) {
		*r = 0; return 0;
	}
	if ( !n2 )
		error("divz : division by 0");
	t1 = ztoq(n1); t2 = ztoq(n2);
	s1 = sgnz(n1); s2 = sgnz(n2);
	/* n1 = qn*SGN(n1)*SGN(n2)*n2+SGN(n1)*rn */
	divn(NM(t1),NM(t2),&qn,&rn);
	NTOQ(qn,s1*s2,qq);
	NTOQ(rn,s1,rq);
	*r = qtoz(rq);
	return qtoz(qq);
}

Z divsz(Z n1,Z n2)
{
	int s1,s2;
	Q t1,t2,qq;
	N qn;

	if ( !n1 )
		return 0;
	if ( !n2 )
		error("divsz : division by 0");
	t1 = ztoq(n1); t2 = ztoq(n2);
	s1 = sgnz(n1); s2 = sgnz(n2);
	/* n1 = qn*SGN(n1)*SGN(n2)*n2 */
	divsn(NM(t1),NM(t2),&qn);
	NTOQ(qn,s1*s2,qq);
	return qtoz(qq);
}

int gcdimm(int c1,int c2)
{
	int r;

	if ( !c1 ) return c2;
	else if ( !c2 ) return c1;
	while ( 1 ) {
		r = c1%c2;
		if ( !r ) return c2;
		c1 = c2; c2 = r;
	}
}

Z gcdz(Z n1,Z n2)
{
	int c1,c2,g;
	Z gcd,r;
	N gn;

	if ( !n1 ) return n2;
	else if ( !n2 ) return n1;

	if ( IS_IMM(n1) ) {
		c1 = ZTOS(n1);
		if ( c1 < 0 ) c1 = -c1;
		if ( IS_IMM(n2) )
			c2 = ZTOS(n2);
		else
			c2 = remzi(n2,c1>0?c1:-c1);
		if ( c2 < 0 ) c2 = -c2;
		g = gcdimm(c1,c2);
		STOZ(g,gcd);
		return gcd;
	} else if ( IS_IMM(n2) ) {
		c2 = ZTOS(n2);
		if ( c2 < 0 ) c2 = -c2;
		c1 = remzi(n1,c2>0?c2:-c2);
		if ( c1 < 0 ) c1 = -c1;
		g = gcdimm(c1,c2);
		STOZ(g,gcd);
		return gcd;
	} else {
		n1 = dupz(n1);
		if ( SL(n1) < 0 ) SL(n1) = -SL(n1);
		n2 = dupz(n2);
		if ( SL(n2) < 0 ) SL(n2) = -SL(n2);
		gcdn((N)n1,(N)n2,&gn);	
		gcd = (Z)gn;
		if ( IS_SZ(gcd) ) {
			SZTOZ(gcd,r); gcd = r;
		}
		return gcd;
	}
}

int remzi(Z n,int m)
{
	unsigned int *x;
	unsigned int t,r;
	int i,c;

	if ( !n ) return 0;
	if ( IS_IMM(n) ) {
		c = ZTOS(n)%m;
		if ( c < 0 ) c += m;
		return c;
	}

	i = SL(n);
	if ( i < 0 ) i = -i;
	for ( i--, x = BD(n)+i, r = 0; i >= 0; i--, x-- ) {
#if defined(sparc)
		r = dsa(m,r,*x);
#else
		DSAB(m,r,*x,t,r);
#endif
	}
	if ( r && SL(n) < 0 )
		r = m-r;
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

#if 0
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
	pushl	%%ebx;\
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
	movl	%%eax,%0;\
	popl	%%ebx"\
	:"=m"(c)\
	:"m"(m1),"m"(m2),"m"(mr),"m"(d2)\
	:"eax","ecx","edx","esi","edi");
#else
	for ( i = 0, c = 0; i < d2; i++, m1++, m2++, mr++ ) {
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
	pushl	%%ebx;\
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
	movl	%%eax,%0;\
	popl	%%ebx"\
	:"=m"(br)\
	:"m"(m1),"m"(m2),"m"(mr),"m"(d2)\
	:"eax","ecx","edx","esi","edi");
#else
	for ( i = 0, br = 0; i < d2; i++, mr++ ) {
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
		u = ZTOS(n);
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
