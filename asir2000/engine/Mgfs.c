/* $OpenXM$ */

#include "ca.h"

void mulssfum(UM,int,UM);

void addsfum(p1,p2,pr)
UM p1,p2,pr;
{
	int *c1,*c2,*cr,i,dmax,dmin;
		
	if ( DEG(p1) == -1 ) {
		cpyum(p2,pr);
		return;
	}
	if ( DEG(p2) == -1 ) {
		cpyum(p1,pr);
		return;
	}
	if ( DEG(p1) >= DEG(p2) ) {
		c1 = COEF(p1); c2 = COEF(p2); dmax = DEG(p1); dmin = DEG(p2);
	} else {
		c1 = COEF(p2); c2 = COEF(p1); dmax = DEG(p2); dmin = DEG(p1);
	}
	for ( i = 0, cr = COEF(pr); i <= dmin; i++ ) 
		cr[i] = _addsf(c1[i],c2[i]);
	for ( ; i <= dmax; i++ ) 
		cr[i] = c1[i];
	if ( dmax == dmin ) 
		degum(pr,dmax);
	else 
		DEG(pr) = dmax;
}

void subsfum(p1,p2,pr)
UM p1,p2,pr;
{
	int *c1,*c2,*cr,i;
	int dmax,dmin;

	if ( DEG(p1) == -1 ) {
		for ( i = DEG(pr) = DEG(p2), c2 = COEF(p2), cr = COEF(pr); 
			  i >= 0; i-- ) 
			cr[i] = _chsgnsf(c2[i]);
		return;
	}
	if ( DEG(p2) == -1 ) {
		cpyum(p1,pr);
		return;
	}
	c1 = COEF(p1); c2 = COEF(p2); cr = COEF(pr);
	if ( DEG(p1) >= DEG(p2) ) { 
		dmax = DEG(p1); dmin = DEG(p2);
		for ( i = 0; i <= dmin; i++ ) 
			cr[i] = _subsf(c1[i],c2[i]);
		for ( ; i <= dmax; i++ ) 
			cr[i] = c1[i];
	} else {
		dmax = DEG(p2); dmin = DEG(p1);
		for ( i = 0; i <= dmin; i++ ) 
			cr[i] = _subsf(c1[i],c2[i]);
		for ( ; i <= dmax; i++ ) 
			cr[i] = _chsgnsf(c2[i]);
	}
	if ( dmax == dmin ) 
		degum(pr,dmax);
	else 
		DEG(pr) = dmax;
}
		
void gcdsfum(p1,p2,pr)
UM p1,p2,pr;
{
	int inv;
	UM t1,t2,q,tum;
	int drem;

	if ( DEG(p1) < 0 )
		cpyum(p2,pr);
	else if ( DEG(p2) < 0 )
		cpyum(p1,pr);
	else {
		if ( DEG(p1) >= DEG(p2) ) {
			t1 = p1; t2 = p2;
		} else {
			t1 = p2; t2 = p1;
		}
		q = W_UMALLOC(DEG(t1));
		while ( ( drem = divsfum(t1,t2,q) ) >= 0 ) {
			tum = t1; t1 = t2; t2 = tum; DEG(t2) = drem;
		}
		inv = _invsf(COEF(t2)[DEG(t2)]);
		mulssfum(t2,inv,pr);
	}
}
void mulsfum(p1,p2,pr)
UM p1,p2,pr;
{
	int *pc1,*pcr;
	int *c1,*c2,*cr;
	int mul;
	int i,j,d1,d2;

	if ( ( (d1 = DEG(p1)) < 0) || ( (d2 = DEG(p2)) < 0 ) ) {
		DEG(pr) = -1;
		return;
	}
	c1 = COEF(p1); c2 = COEF(p2); cr = COEF(pr);
	bzero((char *)cr,(int)((d1+d2+1)*sizeof(int)));
	for ( i = 0; i <= d2; i++, cr++ ) 
		if ( mul = *c2++ ) 
			for ( j = 0, pc1 = c1, pcr = cr; j <= d1; j++, pc1++, pcr++ )
				*pcr = _addsf(_mulsf(*pc1,mul),*pcr);
	DEG(pr) = d1 + d2;
}

void mulssfum(p,n,pr)
int n;
UM p,pr;
{
	int *sp,*dp;
	int i;

	for ( i = DEG(pr) = DEG(p), sp = COEF(p)+i, dp = COEF(pr)+i; 
		  i >= 0; i--, dp--, sp-- )
		*dp = _mulsf(*sp,n);
}
	
int divsfum(p1,p2,pq)
UM p1,p2,pq;
{
	int *pc1,*pct;
	int *c1,*c2,*ct;
	int inv,hd,tmp;
	int i,j, d1,d2,dd;

	if ( (d1 = DEG(p1)) < (d2 = DEG(p2)) ) {
		DEG(pq) = -1;
		return d1;
	}
	c1 = COEF(p1); c2 = COEF(p2); dd = d1-d2;
	if ( ( hd = c2[d2] ) != _onesf() ) {
		inv = _invsf(hd);
		for ( pc1 = c2 + d2; pc1 >= c2; pc1-- )
			*pc1 = _mulsf(*pc1,inv);
	} else 
		inv = _onesf();
	for ( i = dd, ct = c1+d1; i >= 0; i-- ) 
		if ( tmp = *ct-- ) {
			tmp = _chsgnsf(tmp);
			for ( j = d2-1, pct = ct, pc1 = c2+j; j >= 0; j--, pct--, pc1-- )
				*pct = _addsf(_mulsf(*pc1,tmp),*pct);
		}
	if ( inv != _onesf() ) {
		for ( pc1 = c1+d2, pct = c1+d1; pc1 <= pct; pc1++ )
			*pc1 = _mulsf(*pc1,inv);
		for ( pc1 = c2, pct = c2+d2; pc1 <= pct; pc1++ )
			*pc1 = _mulsf(*pc1,hd);
	}
	for ( i = d2-1, pc1 = c1+i; i >= 0 && !(*pc1); pc1--, i-- );
	for ( DEG(pq) = j = dd, pc1 = c1+d1, pct = COEF(pq)+j; j >= 0; j-- )
		*pct-- = *pc1--;
	return i;
}

void diffsfum(f,fd)
UM f,fd;
{
	int *dp,*sp;
	int i;

	for ( i = DEG(f), dp = COEF(fd)+i-1, sp = COEF(f)+i; 
		i >= 1; i--, dp--, sp-- ) {
		*dp = _mulsf(*sp,_itosf(i));
	}
	degum(fd,DEG(f) - 1);
}

void monicsfum(f)
UM f;
{
	int *sp;
	int i,inv;

	i = DEG(f); sp = COEF(f)+i;
	inv = _invsf(*sp);
	for ( ; i >= 0; i--, sp-- )
		*sp = _mulsf(*sp,inv);
}
