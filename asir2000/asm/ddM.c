/* $OpenXM: OpenXM/src/asir99/asm/ddM.c,v 1.1.1.1 1999/11/10 08:12:25 noro Exp $ */
#include "ca.h"
#include "base.h"
#include "inline.h"

void ksquareummain(int,UM,UM);
void kmulummain(int,UM,UM,UM);
void c_copyum(UM,int,int *);
void copyum(UM,UM);
void extractum(UM,int,int,UM);
void ksquareum(int,UM,UM);
void kmulum(int,UM,UM,UM);

/*
 * mod is declared as 'int', because several xxxum functions contains signed 
 * integer addition/subtraction. So mod should be less than 2^31.
 */
    
void mulum(mod,p1,p2,pr)
int mod;
UM p1,p2,pr;
{
	int *pc1,*pcr;
	int *c1,*c2,*cr;
	unsigned int mul;
	int i,j,d1,d2;

	if ( ( (d1 = DEG(p1)) < 0) || ( (d2 = DEG(p2)) < 0 ) ) {
		DEG(pr) = -1;
		return;
	}
	c1 = COEF(p1); c2 = COEF(p2); cr = COEF(pr);
	bzero((char *)cr,(int)((d1+d2+1)*sizeof(int)));
	for ( i = 0; i <= d2; i++, cr++ ) 
		if ( mul = *c2++ ) 
			for ( j = 0, pc1 = c1, pcr = cr; j <= d1; j++, pc1++, pcr++ ) {
				DMAR(*pc1,mul,*pcr,mod,*pcr)
			}
	DEG(pr) = d1 + d2;
}

void mulsum(mod,p,n,pr)
int mod,n;
UM p,pr;
{
	int *sp,*dp;
	int i;

	for ( i = DEG(pr) = DEG(p), sp = COEF(p)+i, dp = COEF(pr)+i; 
		  i >= 0; i--, dp--, sp-- ) {
		DMAR(*sp,n,0,mod,*dp)
	}
}
	
int divum(mod,p1,p2,pq)
int mod;
UM p1,p2,pq;
{
	int *pc1,*pct;
	int *c1,*c2,*ct;
	unsigned int inv,hd,tmp;
	int i,j, d1,d2,dd;

	if ( (d1 = DEG(p1)) < (d2 = DEG(p2)) ) {
		DEG(pq) = -1;
		return d1;
	}
	c1 = COEF(p1); c2 = COEF(p2); dd = d1-d2;
	if ( ( hd = c2[d2] ) != 1 ) {
		inv = invm(hd,mod);
		for ( pc1 = c2 + d2; pc1 >= c2; pc1-- ) {
			DMAR(*pc1,inv,0,mod,*pc1)
		}
	} else 
		inv = 1;
	for ( i = dd, ct = c1+d1; i >= 0; i-- ) 
		if ( tmp = *ct-- ) {
			tmp = mod - tmp;
			for ( j = d2-1, pct = ct, pc1 = c2+j; j >= 0; j--, pct--, pc1-- ) {
				DMAR(*pc1,tmp,*pct,mod,*pct)
			}
		}
	if ( inv != 1 ) {
		for ( pc1 = c1+d2, pct = c1+d1; pc1 <= pct; pc1++ ) {
			DMAR(*pc1,inv,0,mod,*pc1)
		}
		for ( pc1 = c2, pct = c2+d2, inv = hd; pc1 <= pct; pc1++ ) {
			DMAR(*pc1,inv,0,mod,*pc1)
		}
	}
	for ( i = d2-1, pc1 = c1+i; i >= 0 && !(*pc1); pc1--, i-- );
	for ( DEG(pq) = j = dd, pc1 = c1+d1, pct = COEF(pq)+j; j >= 0; j-- )
		*pct-- = *pc1--;
	return i;
}

void diffum(mod,f,fd)
int mod;
UM f,fd;
{
	int *dp,*sp;
	int i;
	UL ltmp;

	for ( i = DEG(f), dp = COEF(fd)+i-1, sp = COEF(f)+i; 
		i >= 1; i--, dp--, sp-- ) {
		DMAR(*sp,i,0,mod,*dp)
	}
	degum(fd,DEG(f) - 1);
}

unsigned int pwrm(mod,a,n)
int mod,a;
int n;
{
	unsigned int s,t;

	if ( !n ) 
		return 1;
	else if ( n == 1 ) 
		return a;
	else {
		t = pwrm(mod,a,n/2); 
		DMAR(t,t,0,mod,s)
		if ( n % 2 ) {
			DMAR(s,a,0,mod,t)
			return t;
		} else
			return s;
	}
}

unsigned int invm(s,mod)
unsigned int s;
int mod;
{
	unsigned int r,a2,q;
	unsigned int f1,f2,a1;

	for ( f1 = s, f2 = mod, a1 = 1, a2 = 0; ; ) {
		q = f1/f2; r = f1 - f2*q; f1 = f2;
		if ( !(f2 = r) ) 
			break;
		DMAR(a2,q,0,mod,r)
/*		r = ( a1 - r + mod ) % mod; */
		if ( a1 >= r )
			r = a1 - r;
		else {
			r = (mod - r) + a1;
		}
		a1 = a2; a2 = r;
	}
/*	return( ( a2 >= 0 ? a2 : a2 + mod ) ); */
	return a2;
}

unsigned int rem(n,m)
N n;
unsigned int m;
{
	unsigned int *x;
	unsigned int t,r;
	int i;

	if ( !n ) 
		return 0;
	for ( i = PL(n)-1, x = BD(n)+i, r = 0; i >= 0; i--, x-- ) {
#if defined(sparc)
		r = dsar(m,r,*x);
#else
		DSAB(m,r,*x,t,r)
#endif
	}
	return r;
}

#ifndef sparc
void addpadic(mod,n,n1,n2)
int mod;
int n;
unsigned int *n1,*n2;
{
	unsigned int carry,tmp;
	int i;

	for ( i = 0, carry = 0; i < n; i++ ) {
		tmp = *n1++ + *n2 + carry;
		DQR(tmp,mod,carry,*n2++)
/*
		carry = tmp / mod;
		*n2++ = tmp - ( carry * mod );
*/
	}
}
#endif 

void mulpadic(mod,n,n1,n2,nr)
int mod;
int n;
unsigned int *n1;
unsigned int *n2,*nr;
{
	unsigned int *pn1,*pnr;
	unsigned int carry,mul;
	int i,j;

	bzero((char *)nr,(int)(n*sizeof(int)));
	for ( j = 0; j < n; j++, n2++, nr++ ) 
		if ( mul = *n2 )
			for ( i = j, carry = 0, pn1 = n1, pnr = nr; 
				i < n; i++, pn1++, pnr++ ) {
				carry += *pnr;
				DMAB(mod,*pn1,mul,carry,carry,*pnr)
			}
}

extern up_kara_mag;

void kmulum(mod,n1,n2,nr)
UM n1,n2,nr;
{
	UM n,t,s,m,carry;
	int d,d1,d2,len,i,l;
	unsigned int *r,*r0;

	if ( !n1 || !n2 ) {
		nr->d = -1; return;
	}
	d1 = DEG(n1)+1; d2 = DEG(n2)+1;
	if ( (d1 < up_kara_mag) || (d2 < up_kara_mag) ) {
		mulum(mod,n1,n2,nr); return;
	}
	if ( d1 < d2 ) {
		n = n1; n1 = n2; n2 = n;
		d = d1; d1 = d2; d2 = d;
	}
	if ( d2 > (d1+1)/2 ) {
		kmulummain(mod,n1,n2,nr); return;
	}
	d = (d1/d2)+((d1%d2)!=0);
	len = (d+1)*d2;
	r0 = (unsigned int *)ALLOCA(len*sizeof(int));
	bzero((char *)r0,len*sizeof(int));
	m = W_UMALLOC(d2+1);
	carry = W_UMALLOC(d2+1);
	t = W_UMALLOC(d1+d2+1);
	s = W_UMALLOC(d1+d2+1);
	for ( carry = 0, i = 0, r = r0; i < d; i++, r += d2 ) {
		extractum(n1,i*d2,d2,m);
		if ( m ) {
			kmulum(mod,m,n2,t);
			addum(mod,t,carry,s);
			c_copyum(s,d2,r);
			extractum(s,d2,d2,carry);
		} else {
			c_copyum(carry,d2,r);
			carry = 0;
		}
	}
	c_copyum(carry,d2,r);
	for ( l = len - 1; !r0[l]; l-- );
	l++;
	DEG(nr) = l-1;
	bcopy((char *)r0,(char *)COEF(nr),l*sizeof(int));
}

void ksquareum(mod,n1,nr)
int mod;
UM n1,nr;
{
	int d1;

	if ( !n1 ) {
		nr->d = -1; return;
	}
	d1 = DEG(n1)+1;
	if ( (d1 < up_kara_mag) ) {
		pwrum(mod,n1,2,nr); return;
	}
	ksquareummain(mod,n1,nr);
}

void extractum(n,index,len,nr)
UM n;
int index,len;
UM nr;
{
	int *m;
	int l;

	if ( !n ) {
		nr->d = -1; return;
	}
	m = COEF(n)+index;
	if ( (l = (DEG(n)+1)-index) >= len ) {
		for ( l = len - 1; (l >= 0) && !m[l]; l-- );
		l++;
	}
	if ( l <= 0 ) {
		nr->d = -1; return;
	} else {
		DEG(nr) = l-1;
		bcopy((char *)m,(char *)COEF(nr),l*sizeof(Q));
	}
}

void copyum(n1,n2)
UM n1,n2;
{
	n2->d = n1->d;
	bcopy((char *)n1->c,(char *)n2->c,(n1->d+1)*sizeof(int));
}

void c_copyum(n,len,p)
UM n;
int len;
int *p;
{
	if ( n )
		bcopy((char *)COEF(n),(char *)p,MIN((DEG(n)+1),len)*sizeof(int));
}

void kmulummain(mod,n1,n2,nr)
int mod;
UM n1,n2,nr;
{
	int d1,d2,h,len;
	UM n1lo,n1hi,n2lo,n2hi,hi,lo,mid1,mid2,mid,s1,s2,t1,t2;

	d1 = DEG(n1)+1; d2 = DEG(n2)+1; h = (d1+1)/2;
	n1lo = W_UMALLOC(d1+1); n1hi = W_UMALLOC(d1+1);
	n2lo = W_UMALLOC(d2+1); n2hi = W_UMALLOC(d2+1);
	lo = W_UMALLOC(d1+d2+1); hi = W_UMALLOC(d1+d2+1);
	mid1 = W_UMALLOC(d1+d2+1); mid2 = W_UMALLOC(d1+d2+1);
	mid = W_UMALLOC(d1+d2+1);
	s1 = W_UMALLOC(d1+d2+1); s2 = W_UMALLOC(d1+d2+1);
	extractum(n1,0,h,n1lo); extractum(n1,h,d1-h,n1hi);
	extractum(n2,0,h,n2lo); extractum(n2,h,d2-h,n2hi);
	kmulum(mod,n1hi,n2hi,hi); kmulum(mod,n1lo,n2lo,lo);
	len = DEG(hi)+1+2*h; t1 = W_UMALLOC(len-1); DEG(t1) = len-1;
	bzero((char *)COEF(t1),len*sizeof(int));
	if ( lo )
		bcopy((char *)COEF(lo),(char *)COEF(t1),(DEG(lo)+1)*sizeof(int));
	if ( hi )
		bcopy((char *)COEF(hi),(char *)(COEF(t1)+2*h),(DEG(hi)+1)*sizeof(int));

	addum(mod,hi,lo,mid1);
	subum(mod,n1hi,n1lo,s1); subum(mod,n2lo,n2hi,s2); kmulum(mod,s1,s2,mid2);
	addum(mod,mid1,mid2,mid);
	if ( mid ) {
		len = DEG(mid)+1+h; t2 = W_UMALLOC(len-1); DEG(t2) = len-1;
		bzero((char *)COEF(t2),len*sizeof(int));
		bcopy((char *)COEF(mid),(char *)(COEF(t2)+h),(DEG(mid)+1)*sizeof(int));
		addum(mod,t1,t2,nr);
	} else
		copyum(t1,nr);
}

void ksquareummain(mod,n1,nr)
int mod;
UM n1,nr;
{
	int d1,h,len;
	UM n1lo,n1hi,hi,lo,mid1,mid2,mid,s1,t1,t2;

	d1 = DEG(n1)+1; h = (d1+1)/2;
	n1lo = W_UMALLOC(d1+1); n1hi = W_UMALLOC(d1+1);
	lo = W_UMALLOC(2*d1+1); hi = W_UMALLOC(2*d1+1);
	mid1 = W_UMALLOC(2*d1+1); mid2 = W_UMALLOC(2*d1+1);
	mid = W_UMALLOC(2*d1+1);
	s1 = W_UMALLOC(2*d1+1);
	extractum(n1,0,h,n1lo); extractum(n1,h,d1-h,n1hi);
	ksquareum(mod,n1hi,hi); ksquareum(mod,n1lo,lo);
	len = DEG(hi)+1+2*h; t1 = W_UMALLOC(len-1); DEG(t1) = len-1;
	bzero((char *)COEF(t1),len*sizeof(int));
	if ( lo )
		bcopy((char *)COEF(lo),(char *)COEF(t1),(DEG(lo)+1)*sizeof(int));
	if ( hi )
		bcopy((char *)COEF(hi),(char *)(COEF(t1)+2*h),(DEG(hi)+1)*sizeof(int));

	addum(mod,hi,lo,mid1);
	subum(mod,n1hi,n1lo,s1); ksquareum(mod,s1,mid2);
	subum(mod,mid1,mid2,mid);
	if ( mid ) {
		len = DEG(mid)+1+h; t2 = W_UMALLOC(len-1); DEG(t2) = len-1;
		bzero((char *)COEF(t2),len*sizeof(int));
		bcopy((char *)COEF(mid),(char *)(COEF(t2)+h),(DEG(mid)+1)*sizeof(int));
		addum(mod,t1,t2,nr);
	} else
		copyum(t1,nr);
}
