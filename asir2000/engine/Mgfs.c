/* $OpenXM: OpenXM_contrib2/asir2000/engine/Mgfs.c,v 1.15 2002/12/18 06:15:40 noro Exp $ */

#include "ca.h"
#include "inline.h"

extern int current_gfs_p;
extern int up_kara_mag, current_gfs_q1;
extern int *current_gfs_plus1;

#if defined(__GNUC__)
#define INLINE inline
#elif defined(VISUAL)
#define INLINE __inline
#else
#define INLINE
#endif

INLINE int _ADDSF(int a,int b)
{
	if ( !a )
		return b;
	else if ( !b )
		return a;

	a = IFTOF(a); b = IFTOF(b);

	if ( !current_gfs_ntoi ) {
		a = a+b-current_gfs_q;
		if ( a == 0 )
			return 0;
		else {
			if ( a < 0 )
				a += current_gfs_q;
			return FTOIF(a);
		}
	}

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

INLINE int _MULSF(int a,int b)
{
	int c;

	if ( !a || !b )
		return 0;
	else if ( !current_gfs_ntoi ) {
		a = IFTOF(a); b = IFTOF(b);
		DMAR(a,b,0,current_gfs_q,c);
		return FTOIF(c);
	} else {
		a = IFTOF(a) + IFTOF(b);
		if ( a >= current_gfs_q1 )
			a -= current_gfs_q1;
		return FTOIF(a);
	}
}

void addsfum(UM p1,UM p2,UM pr)
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

void subsfum(UM p1,UM p2,UM pr)
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
		
void gcdsfum(UM p1,UM p2,UM pr)
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

void mulsfum(UM p1,UM p2,UM pr)
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

void mulssfum(UM p,int n,UM pr)
{
	int *sp,*dp;
	int i;

	for ( i = DEG(pr) = DEG(p), sp = COEF(p)+i, dp = COEF(pr)+i; 
		  i >= 0; i--, dp--, sp-- )
		*dp = _MULSF(*sp,n);
}
	
void kmulsfum(UM n1,UM n2,UM nr)
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
	m = W_UMALLOC(d1+d2+1);
	carry = W_UMALLOC(d1+d2+1);
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

void kmulsfummain(UM n1,UM n2,UM nr)
{
	int d1,d2,h,len,d;
	UM n1lo,n1hi,n2lo,n2hi,hi,lo,mid1,mid2,mid,s1,s2,t1,t2;

	d1 = DEG(n1)+1; d2 = DEG(n2)+1; h = (d1+1)/2;
	d = d1+d2+1;
	n1lo = W_UMALLOC(d); n1hi = W_UMALLOC(d);
	n2lo = W_UMALLOC(d); n2hi = W_UMALLOC(d);
	lo = W_UMALLOC(d); hi = W_UMALLOC(d);
	mid1 = W_UMALLOC(d); mid2 = W_UMALLOC(d);
	mid = W_UMALLOC(d);
	s1 = W_UMALLOC(d); s2 = W_UMALLOC(d);
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

int divsfum(UM p1,UM p2,UM pq)
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

void diffsfum(UM f,UM fd)
{
	int *dp,*sp;
	int i;

	for ( i = DEG(f), dp = COEF(fd)+i-1, sp = COEF(f)+i; 
		i >= 1; i--, dp--, sp-- ) {
		*dp = _MULSF(*sp,_itosf(i%current_gfs_p));
	}
	degum(fd,DEG(f) - 1);
}

void monicsfum(UM f)
{
	int *sp;
	int i,inv;

	i = DEG(f); sp = COEF(f)+i;
	inv = _invsf(*sp);
	for ( ; i >= 0; i--, sp-- )
		*sp = _MULSF(*sp,inv);
}

int compsfum(UM a,UM b)
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

void mulsfbm(BM f1,BM f2,BM fr)
{
	UM mul,t,s;
	int i,j,d1,d2,dy;

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

int degbm(BM f)
{
	int d,i,dy;

	dy = DEG(f);
	d = DEG(COEF(f)[0]);
	for ( i = 1; i <= dy; i++ )
		d = MAX(DEG(COEF(f)[i]),d);
	return d;
}

/* g += f */

void addtosfbm(BM f,BM g)
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

void eucsfum(UM f1,UM f2,UM a,UM b)
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

void shiftsflum(int n,LUM f,int ev)
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

void shiftsfum(UM f,int a,UM g)
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

void shiftsfbm(BM f,int a)
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

void clearbm(int n,BM f)
{
	int i,dy;
	UM *c;

	dy = DEG(f);
	for ( c = COEF(f), i = 0; i <= dy; i++ )
		clearum(c[i],n);
}

static void create_bmono(P c,V x,int i,V y,int j,P *mono);

struct lb {
	int pos,len;
	int *r;
	int *hist;
};

static NODE insert_lb(NODE g,struct lb *a)
{
	NODE prev,cur,n;

	prev = 0; cur = g;
	while ( cur ) {
		if ( a->pos < ((struct lb *)BDY(cur))->pos ) {
			MKNODE(n,a,cur);
			if ( !prev )
				return n;
			else {
				NEXT(prev) = n;
				return g;
			}
		} else {
			prev = cur;
			cur = NEXT(cur);
		}
	}
	MKNODE(n,a,0);
	NEXT(prev) = n;
	return g;
}

static void lnf(int *r,int *h,int n,int len,NODE g)
{
	struct lb *t;
	int pos,i,j,len1,c;
	int *r1,*h1;

	for ( ; g; g = NEXT(g) ) {
		t = (struct lb *)BDY(g);
		pos = t->pos;
		if ( c = r[pos] ) {
			c = _chsgnsf(c);
			r1 = t->r;
			h1 = t->hist;
			len1 = t->len;
			for ( i = pos; i < n; i++ )
				if ( r1[i] )
					r[i] = _ADDSF(r[i],_MULSF(r1[i],c));
			for ( i = 0; i < len1; i++ )
				if ( h1[i] )
					h[i] = _ADDSF(h[i],_MULSF(h1[i],c));
		}
	}
	for ( i = 0; i < n && !r[i]; i++ );
	if ( i < n ) {
		c = _invsf(r[i]);
		for ( j = i; j < n; j++ )
			if ( r[j] )
				r[j] = _MULSF(r[j],c);	
		for ( j = 0; j < len; j++ )
			if ( h[j] ) 
				h[j] = _MULSF(h[j],c);	
	}
}

void sfmintdeg(VL vl,P fx,int dy,int c,P *fr)
{
	V x,y;
	int dx,dxdy,i,j,k,l,d,len,len0,u,dyk;
	UP *rx;
	DCP dc;
	P t,f,mono,f1;
	UP ut,h;
	int ***nf;
	int *r,*hist,*prev,*r1;
	struct lb *lb;
	GFS s;
	NODE g;

	x = vl->v;
	y = NEXT(vl)->v;
	dx = getdeg(x,fx);
	dxdy = dx*dy;
	/* rx = -(fx-x^dx) */
	rx = (UP *)CALLOC(dx,sizeof(UP));
	for ( dc = DC(fx); dc; dc = NEXT(dc)) {
		chsgnp(COEF(dc),&t);
		ptoup(t,&ut);
		rx[QTOS(DEG(dc))] = ut;
	}
	/* nf[d] = normal form table of monomials with total degree d */
	nf = (int ***)CALLOC(dx+dy+1,sizeof(int **)); /* xxx */
	nf[0] = (int **)CALLOC(1,sizeof(int *));

	/* nf[0][0] = 1 */
	r = (int *)CALLOC(dxdy,sizeof(int));
	r[0] = _onesf();
	nf[0][0] = r;

	hist = (int *)CALLOC(1,sizeof(int));
	hist[0] = _onesf();

	lb = (struct lb *)CALLOC(1,sizeof(struct lb));
	lb->pos = 0;
	lb->r = r;
	lb->hist = hist;
	lb->len = 1;

	/* g : table of normal form as linear form */
	MKNODE(g,lb,0);

	len = 1;
	h = UPALLOC(dy);
	for ( d = 1; ; d++ ) {
		if ( d > c ){		
			return;
		}
		nf[d] = (int **)CALLOC(d+1,sizeof(int *));
		len0 = len;
		len += d+1;

		for ( i = d; i >= 0; i-- ) {
			/* nf(x^(d-i)*y^i) = nf(y*nf(x^(d-i)*y^(i-1))) */
			/* nf(x^d) = nf(nf(x^(d-1))*x) */
			r = (int *)CALLOC(dxdy,sizeof(int));
			if ( i == 0 ) {
				prev = nf[d-1][0];
				bcopy(prev,r+dy,(dxdy-dy)*sizeof(int));

				/* create the head coeff */
				for ( l = 0, k = dxdy-dy; l < dy; l++, k++ ) {
					iftogfs(prev[k],&s);
					COEF(h)[l] = (Num)s;
				}
				for ( l = dy-1; l >= 0 && !COEF(h)[l]; l--);
				DEG(h) = l;

				for ( k = 0, dyk = 0; k < dx; k++, dyk += dy ) {
					tmulup(rx[k],h,dy,&ut);
					if ( ut ) 
						for ( l = 0; l < dy; l++ ) {
							s = (GFS)COEF(ut)[l];
							if ( s ) {
								u = CONT(s);
								r[dyk+l] = _ADDSF(r[dyk+l],FTOIF(u));
							}
						}
				}
			} else {
				prev = nf[d-1][i-1];
				for ( k = 0, dyk = 0; k < dx; k++, dyk += dy ) {
					for ( l = 1; l < dy; l++ )
						r[dyk+l] = prev[dyk+l-1];
				}
			}
			nf[d][i] = r;
			hist = (int *)CALLOC(len,sizeof(int));
			hist[len0+i] = _onesf();
			r1 = (int *)CALLOC(dxdy,sizeof(int));
			bcopy(r,r1,dxdy*sizeof(int));
			lnf(r1,hist,dxdy,len,g);
			for ( k = 0; k < dxdy && !r1[k]; k++ );
			if ( k == dxdy ) {
				f = 0;
				for ( k = j = 0; k <= d; k++ )
					for ( i = 0; i <= k; i++, j++ )
						if ( hist[j] ) {
							iftogfs(hist[j],&s);
							/* mono = s*x^(k-i)*y^i */
							create_bmono((P)s,x,k-i,y,i,&mono);
							addp(vl,f,mono,&f1);
							f = f1;
						}
				*fr = f;
				return;
			}	else {
				lb = (struct lb *)CALLOC(1,sizeof(struct lb));
				lb->pos = k;
				lb->r = r1;
				lb->hist = hist;
				lb->len = len;
				g = insert_lb(g,lb);
			}
		}
	}
}

static void create_bmono(P c,V x,int i,V y,int j,P *mono)
{
	P t,s;

	if ( !i )
		if ( !j )
			t = c;
		else {
			/* c*y^j */
			MKV(y,t);
			COEF(DC(t)) = c;
			STOQ(j,DEG(DC(t)));
		}
	else if ( !j ) {
		/* c*x^i */
		MKV(x,t);
		COEF(DC(t)) = c;
		STOQ(i,DEG(DC(t)));
	} else {
		MKV(y,s);
		COEF(DC(s)) = c;
		STOQ(j,DEG(DC(s)));
		MKV(x,t);
		COEF(DC(t)) = s;
		STOQ(i,DEG(DC(t)));
	}
	*mono = t;
}
