/* $OpenXM: OpenXM_contrib2/asir2000/engine/Mgfs.c,v 1.2 2001/06/22 08:51:12 noro Exp $ */

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

void addsfarray(int,int *,int *);
void mulsfarray_trunc(int,int *,int *,int *);

void mulsflum(n,f1,f2,fr)
int n;
LUM f1,f2,fr;
{
	int max;
	int i,j,**p1,**p2,*px;
	int *w,*w1,*w2;

	p1 = (int **)COEF(f1); p2 = (int **)COEF(f2);
	w = W_ALLOC(2*(n+1)); w1 = W_ALLOC(DEG(f1)); w2 = W_ALLOC(DEG(f2));
	for ( i = DEG(f1); i >= 0; i-- ) {
		for ( j = n - 1, px = p1[i]; ( j >= 0 ) && ( px[j] == 0 ); j-- );
		w1[i] = ( j == -1 ? 0 : 1 );
	}
	for ( i = DEG(f2); i >= 0; i-- ) {
		for ( j = n - 1, px = p2[i]; ( j >= 0 ) && ( px[j] == 0 ); j-- );
		w2[i] = ( j == -1 ? 0 : 1 );
	}
	for ( j = DEG(fr) = DEG(f1) + DEG(f2); j >= 0; j-- ) {
		for ( i = n - 1, px = COEF(fr)[j]; i >= 0; i-- ) 
			px[i] = 0;
		for ( max = MIN(DEG(f1),j), i = MAX(0,j-DEG(f2)); i <= max; i++ ) 
			if ( w1[i] != 0 && w2[j - i] != 0 ) {
				mulsfarray_trunc(n,p1[i],p2[j - i],w); addsfarray(n,w,px);
			}
	}
}

void addsfarray(n,a1,a2)
int n;
int *a1,*a2;
{
	int i;

	for ( i = 0; i < n; i++, a1++, a2++ )
		if ( *a1 )
			*a2 = _addsf(*a1,*a2);
}

void mulsfarray_trunc(n,a1,a2,r)
int n;
int *a1,*a2,*r;
{
	int mul,i,j;
	int *c1,*c2,*cr;
	int *pc1,*pc2,*pcr;

	bzero(r,n*sizeof(int));
	c1 = a1; c2 = a2; cr = r;
	for ( i = 0; i < n; i++, cr++ ) {
		if ( mul = *c2++ )
		for ( j = 0, pc1 = c1, pcr = cr; j+i < n; j++, pc1++, pcr++ )
			if ( *pc1 )
				*pcr = _addsf(_mulsf(*pc1,mul),*pcr);
	}
}

void eucsfum(f1,f2,a,b)
UM f1,f2,a,b;
{
	UM g1,g2,a1,a2,a3,wm,q,tum;
	int d,dr;

	/* XXX */
	d = DEG(f1) + DEG(f2) + 10;
	g1 = W_UMALLOC(d); g2 = W_UMALLOC(d); a1 = W_UMALLOC(d);
	a2 = W_UMALLOC(d); a3 = W_UMALLOC(d); wm = W_UMALLOC(d);
	q = W_UMALLOC(d);
	DEG(a1) = 0; COEF(a1)[0] = _onesf(); DEG(a2) = -1;
	cpyum(f1,g1); cpyum(f2,g2);
	while ( 1 ) {
		dr = divsfum(g1,g2,q); tum = g1; g1 = g2; g2 = tum;
		if ( ( DEG(g2) = dr ) == -1 ) 
			break;
		mulsfum(a2,q,wm); subsfum(a1,wm,a3); dr = divsfum(a3,f2,q);
		tum = a1; a1 = a2; a2 = a3; a3 = tum; DEG(a3) = dr;
	}
	if ( COEF(g1)[0] != _onesf() )
		mulssfum(a2,_invsf(COEF(g1)[0]),a);
	else 
		cpyum(a2,a);
	mulsfum(a,f1,wm);
	if ( DEG(wm) >= 0 ) 
		COEF(wm)[0] = _subsf(COEF(wm)[0],_onesf());
	else {
		DEG(wm) = 0; COEF(wm)[0] = _chsgnsf(_onesf());
	}
	divsfum(wm,f2,q); mulssfum(q,_chsgnsf(_onesf()),b);
}

void shiftsfum(UM,int,UM);

void shiftsflum(n,f,ev)
int n;
LUM f;
int ev;
{
	int d,i,j;
	UM w,w1;
	int *coef;
	int **c;

	d = f->d;
	c = f->c;
	w = W_UMALLOC(n);
	w1 = W_UMALLOC(n);
	for ( i = 0; i <= d; i++ ) { 
		coef = c[i];
		for ( j = n-1; j >= 0 && coef[j] == 0; j-- );
		if ( j <= 0 )
			continue;	
		DEG(w) = j; bcopy(coef,COEF(w),(j+1)*sizeof(int));
		shiftsfum(w,ev,w1);
		bcopy(COEF(w1),coef,(j+1)*sizeof(int));
	}
}

/* f(x) -> g(x) = f(x+a) */

void shiftsfum(f,a,g)
UM f;
int a;
UM g;
{
	int n,i;
	UM pwr,xa,w,t;
	int *c;

	n = DEG(f);
	if ( n <= 0 )
		cpyum(f,g);
	else {
		c = COEF(f);

		/* g = 0 */
		DEG(g) = -1;

		/* pwr = 1 */
		pwr = W_UMALLOC(n); DEG(pwr) = 0; COEF(pwr)[0] = _onesf();

		/* xa = x+a */
		xa = W_UMALLOC(n); DEG(xa) = 1; 
		COEF(xa)[0] = a; COEF(xa)[1] = _onesf();

		w = W_UMALLOC(n);
		t = W_UMALLOC(n);

		/* g += pwr*c[0] */
		for ( i = 0; i <= n; i++ ) {
			mulssfum(pwr,c[i],w); addsfum(g,w,t); cpyum(t,g);
			if ( i < n ) {
				mulsfum(pwr,xa,t); cpyum(t,pwr);
			}
		}
	}
}

/* clear the body of f */

void clearsflum(bound,n,f)
int bound,n;
LUM f;
{
	int **c;
	int i;

	DEG(f) = 0;
	for ( c = COEF(f), i = 0; i <= n; i++ )
		bzero(c[i],(bound+1)*sizeof(int));
}

/* f += g */

void addtosflum(bound,g,f)
int bound;
LUM g,f;
{
	int dg,i,j;
	int **cg,**cf;

	dg = DEG(g);
	cg = COEF(g);
	cf = COEF(f);
	for ( i = 0; i <= dg; i++ )
		for ( j = 0; j <= bound; j++ )
			cf[i][j] = _addsf(cf[i][j],cg[i][j]);
}
