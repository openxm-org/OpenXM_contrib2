/* $OpenXM: OpenXM_contrib2/asir2000/engine/Mgfs.c,v 1.9 2001/07/23 05:05:41 noro Exp $ */

#include "ca.h"

extern int up_kara_mag, current_gfs_q1;
extern int *current_gfs_plus1;

void mulssfum(UM,int,UM);
void kmulsfummain(UM,UM,UM);


#if defined(__GNUC__)
#define INLINE inline
#elif defined(VISUAL)
#define INLINE __inline
#else
#define INLINE
#endif

INLINE int _ADDSF(a,b)
int a,b;
{
	if ( !a )
		return b;
	else if ( !b )
		return a;

	a = IFTOF(a); b = IFTOF(b);
	if ( a > b ) {
		/* tab[a]+tab[b] = tab[b](tab[a-b]+1) */
		a = current_gfs_plus1[a-b];
		if ( a < 0 )
			return 0;
		else {
			a += b;
			if ( a >= current_gfs_q1 )
				a -= current_gfs_q1;
			return FTOIF(a);
		}
	} else {
		/* tab[a]+tab[b] = tab[a](tab[b-a]+1) */
		b = current_gfs_plus1[b-a];
		if ( b < 0 )
			return 0;
		else {
			b += a;
			if ( b >= current_gfs_q1 )
				b -= current_gfs_q1;
			return FTOIF(b);
		}
	}
}

INLINE int _MULSF(a,b)
int a,b;
{
	if ( !a || !b )
		return 0;
	else {
		a = IFTOF(a) + IFTOF(b);
		if ( a >= current_gfs_q1 )
			a -= current_gfs_q1;
		return FTOIF(a);
	}
}

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
		cr[i] = _ADDSF(c1[i],c2[i]);
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
				if ( *pc1 )
					*pcr = _ADDSF(_MULSF(*pc1,mul),*pcr);
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
		*dp = _MULSF(*sp,n);
}
	
void kmulsfum(n1,n2,nr)
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
		mulsfum(n1,n2,nr); return;
	}
	if ( d1 < d2 ) {
		n = n1; n1 = n2; n2 = n;
		d = d1; d1 = d2; d2 = d;
	}
	if ( d2 > (d1+1)/2 ) {
		kmulsfummain(n1,n2,nr); return;
	}
	d = (d1/d2)+((d1%d2)!=0);
	len = (d+1)*d2;
	r0 = (unsigned int *)ALLOCA(len*sizeof(int));
	bzero((char *)r0,len*sizeof(int));
	m = W_UMALLOC(d2+1);
	carry = W_UMALLOC(d2+1);
	t = W_UMALLOC(d1+d2+1);
	s = W_UMALLOC(d1+d2+1);
	for ( DEG(carry) = -1, i = 0, r = r0; i < d; i++, r += d2 ) {
		extractum(n1,i*d2,d2,m);
		if ( m ) {
			kmulsfum(m,n2,t);
			addsfum(t,carry,s);
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

void kmulsfummain(n1,n2,nr)
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
	kmulsfum(n1hi,n2hi,hi); kmulsfum(n1lo,n2lo,lo);
	len = DEG(hi)+1+2*h; t1 = W_UMALLOC(len-1); DEG(t1) = len-1;
	bzero((char *)COEF(t1),len*sizeof(int));
	if ( lo )
		bcopy((char *)COEF(lo),(char *)COEF(t1),(DEG(lo)+1)*sizeof(int));
	if ( hi )
		bcopy((char *)COEF(hi),(char *)(COEF(t1)+2*h),(DEG(hi)+1)*sizeof(int));

	addsfum(hi,lo,mid1);
	subsfum(n1hi,n1lo,s1); subsfum(n2lo,n2hi,s2);
	kmulsfum(s1,s2,mid2); addsfum(mid1,mid2,mid);
	if ( mid ) {
		len = DEG(mid)+1+h; t2 = W_UMALLOC(len-1); DEG(t2) = len-1;
		bzero((char *)COEF(t2),len*sizeof(int));
		bcopy((char *)COEF(mid),(char *)(COEF(t2)+h),(DEG(mid)+1)*sizeof(int));
		addsfum(t1,t2,nr);
	} else
		copyum(t1,nr);
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
			*pc1 = _MULSF(*pc1,inv);
	} else 
		inv = _onesf();
	for ( i = dd, ct = c1+d1; i >= 0; i-- ) 
		if ( tmp = *ct-- ) {
			tmp = _chsgnsf(tmp);
			for ( j = d2-1, pct = ct, pc1 = c2+j; j >= 0; j--, pct--, pc1-- )
				*pct = _ADDSF(_MULSF(*pc1,tmp),*pct);
		}
	if ( inv != _onesf() ) {
		for ( pc1 = c1+d2, pct = c1+d1; pc1 <= pct; pc1++ )
			*pc1 = _MULSF(*pc1,inv);
		for ( pc1 = c2, pct = c2+d2; pc1 <= pct; pc1++ )
			*pc1 = _MULSF(*pc1,hd);
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
		*dp = _MULSF(*sp,_itosf(i));
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
		*sp = _MULSF(*sp,inv);
}

int compsfum(a,b)
UM a,b;
{
	int i,da,db;

	if ( !a )
		return !b?0:1;
	else if ( !b )
		return 1;
	else if ( (da = DEG(a)) > (db = DEG(b)) )
		return 1;
	else if ( da < db )
		return -1;
	else {
		for ( i = da; i >= 0 && COEF(a)[i] == COEF(b)[i]; i-- );
		if ( i < 0 )
			return 0;
		else if ( (unsigned int)COEF(a)[i] > (unsigned int)COEF(b)[i] )
			return 1;
		else
			return -1;
	}
}

void addsfarray(int,int *,int *);
void mulsfarray_trunc(int,int *,int *,int *);

/* f1 = f1->c[0]+f1->c[1]*y+...,  f2 = f2->c[0]+f2->c[1]*y+... mod y^n */

void mulsfbm(f1,f2,fr)
BM f1,f2,fr;
{
	UM mul,t,s;
	int i,j,h,d1,d2,dy;

	dy = MIN(DEG(f1),DEG(f2));

	d1 = degbm(f1);
	d2 = degbm(f2);
	t = W_UMALLOC(d1+d2);
	s = W_UMALLOC(d1+d2);

	for ( i = 0; i <= dy; i++ ) {
		mul = COEF(f2)[i];
		if ( DEG(mul) >= 0 )
		for ( j = 0; i+j <= dy; j++ ) {
			if ( COEF(f1)[j] ) {
				kmulsfum(COEF(f1)[j],mul,t);
				addsfum(t,COEF(fr)[i+j],s);
				cpyum(s,COEF(fr)[i+j]);
			}
		}
	}
	DEG(fr) = dy;
}

int degbm(f)
BM f;
{
	int d,i,dy;

	dy = DEG(f);
	d = DEG(COEF(f)[0]);
	for ( i = 1; i <= dy; i++ )
		d = MAX(DEG(COEF(f)[i]),d);
	return d;
}

/* g += f */

void addtosfbm(f,g)
BM f,g;
{
	int i,d1,d2,dy;
	UM t;

	dy = DEG(g);
	if ( DEG(f) > dy )
		error("addtosfbm : invalid input");
	dy = MIN(DEG(f),dy);
	d1 = degbm(f);
	d2 = degbm(g);
	t = W_UMALLOC(MAX(d1,d2));
	for ( i = 0; i <= dy; i++ ) {
		addsfum(COEF(f)[i],COEF(g)[i],t);
		cpyum(t,COEF(g)[i]);
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

/* f(y) -> f(y+a) */

void shiftsfbm(f,a)
BM f;
int a;
{
	int i,j,d,dy;
	UM pwr,ya,w,t,s;
	UM *c;

	dy = DEG(f);
	c = COEF(f);
	d = degbm(f);

	w = W_UMALLOC(d);
	t = W_UMALLOC(d);
	s = W_UMALLOC(dy);

	/* pwr = 1 */
	pwr = W_UMALLOC(dy); DEG(pwr) = 0; COEF(pwr)[0] = _onesf();

	/* ya = y+a */
	ya = W_UMALLOC(1); DEG(ya) = 1; 
	COEF(ya)[0] = a; COEF(ya)[1] = _onesf();

	for ( i = 0; i <= dy; i++ ) {
		/* c[i] does not change */
		for ( j = 0; j < i; j++ ) {
			mulssfum(c[i],COEF(pwr)[j],w);
			addsfum(w,c[j],t); cpyum(t,c[j]);
		}
		if ( i < dy ) {
			mulsfum(pwr,ya,s); cpyum(s,pwr);
		}
	}
}

void clearbm(n,f)
int n;
BM f;
{
	int i,dy;
	UM *c;

	dy = DEG(f);
	for ( c = COEF(f), i = 0; i <= dy; i++ )
		clearum(c[i],n);
}
