/* $OpenXM: OpenXM_contrib2/asir2000/engine/Hgfs.c,v 1.1 2001/06/20 09:32:13 noro Exp $ */

#include "ca.h"

struct p_pair {
	UM p0;
	UM p1;
	struct p_pair *next;
};

void canzassf(UM,int,UM *);
void lnfsf(int,UM,UM,struct p_pair *,UM,UM);
void minipolysf(UM,UM,UM);
void czsfum(UM,UM *);
void gensqfrsfum(UM,DUM);

void fctrsf(p,dcp)
P p;
DCP *dcp;
{
	int n,i,j,k;
	DCP dc,dc0;
	P lc;
	P zp;
	UM mp;
	UM *tl;
	struct oDUM *udc,*udc1;

	simp_ff(p,&zp); p = zp;
	if ( !p ) {
		*dcp = 0; return;
	}
	mp = W_UMALLOC(UDEG(p));
	ptosfum(p,mp);
	if ( (n = DEG(mp)) < 0 ) {
		*dcp = 0; return;
	} else if ( n == 0 ) {
		NEWDC(dc); COEF(dc) = p; DEG(dc) = ONE; 
		NEXT(dc) = 0; *dcp = dc;
		return;
	}
	lc = COEF(DC(p));
	if ( !_isonesf(COEF(mp)[n]) ) {
		monicsfum(mp);
	}

	W_CALLOC(n+1,struct oDUM,udc);
	gensqfrsfum(mp,udc);

	tl = (UM *)ALLOCA((n+1)*sizeof(UM));
	W_CALLOC(DEG(mp)+1,struct oDUM,udc1);

	for ( i = 0,j = 0; udc[i].f; i++ )
		if ( DEG(udc[i].f) == 1 ) {
			udc1[j].f = udc[i].f; udc1[j].n = udc[i].n; j++;
		} else {
			bzero((char *)tl,(n+1)*sizeof(UM));
			czsfum(udc[i].f,tl);
			for ( k = 0; tl[k]; k++, j++ ) {
				udc1[j].f = tl[k]; udc1[j].n = udc[i].n;
			}
		}
	udc = udc1;
	NEWDC(dc0); COEF(dc0) = lc; DEG(dc0) = ONE; dc = dc0;
	for ( n = 0; udc[n].f; n++ ) {
		NEWDC(NEXT(dc)); dc = NEXT(dc);
		STOQ(udc[n].n,DEG(dc)); sfumtop(VR(p),udc[n].f,&COEF(dc));
	}
	NEXT(dc) = 0; *dcp = dc0;
}

void gensqfrsfum(p,dc)
UM p;
struct oDUM *dc;
{
	int n,i,j,d,mod;
	UM t,s,g,f,f1,b;

	if ( (n = DEG(p)) == 1 ) {
		dc[0].f = UMALLOC(DEG(p)); cpyum(p,dc[0].f); dc[0].n = 1;
		return;
	}
	t = W_UMALLOC(n); s = W_UMALLOC(n); g = W_UMALLOC(n);
	f = W_UMALLOC(n); f1 = W_UMALLOC(n); b = W_UMALLOC(n);
	diffsfum(p,t); cpyum(p,s); gcdsfum(t,s,g); 
	if ( !DEG(g) ) {
		dc[0].f = UMALLOC(DEG(p)); cpyum(p,dc[0].f); dc[0].n = 1;
		return;
	}
	cpyum(p,b); cpyum(p,t); divsfum(t,g,f);
	for ( i = 0, d = 0; DEG(f); i++ ) {
		while ( 1 ) {
			cpyum(b,t);
			if ( divsfum(t,f,s) >= 0 )
				break;
			else {
				cpyum(s,b); d++;
			}
		}
		cpyum(b,t); cpyum(f,s); gcdsfum(t,s,f1);
		divsfum(f,f1,s); cpyum(f1,f);
		dc[i].f = UMALLOC(DEG(s)); cpyum(s,dc[i].f); dc[i].n = d;
	}
	mod = characteristic_sf();
	if ( DEG(b) > 0 ) {
		d = 1;
		while ( 1 ) {
			cpyum(b,t);
			for ( j = DEG(t); j >= 0; j-- )
				if ( COEF(t)[j] && (j % mod) )
					break;
			if ( j >= 0 )
				break;
			else {
				DEG(s) = DEG(t)/mod;
				for ( j = 0; j <= DEG(t); j++ )
					COEF(s)[j] = COEF(t)[j*mod];	
				cpyum(s,b); d *= mod;
			}
		}
		gensqfrsfum(b,dc+i);
		for ( j = i; dc[j].f; j++ )
			dc[j].n *= d;
	}
}

void randsfum(d,p)
int d;
UM p;
{
	int i;

	for ( i = 0; i < d; i++ )
		COEF(p)[i] = _randomsf();
	for ( i = d-1; i >= 0 && !COEF(p)[i]; i-- );
	p->d = i;
}

void pwrmodsfum(p,e,f,pr)
int e;
UM p,f,pr;
{
	UM wt,ws,q;

	if ( e == 0 ) {
		DEG(pr) = 0; COEF(pr)[0] = _onesf();
	} else if ( DEG(p) < 0 ) 
		DEG(pr) = -1;
	else if ( e == 1 ) {
		q = W_UMALLOC(DEG(p)); cpyum(p,pr);
		DEG(pr) = divsfum(pr,f,q);
	} else if ( DEG(p) == 0 ) {
		DEG(pr) = 0; COEF(pr)[0] = _pwrsf(COEF(p)[0],e);
	} else {
		wt = W_UMALLOC(2*DEG(f)); ws = W_UMALLOC(2*DEG(f));
		q = W_UMALLOC(2*DEG(f));
		pwrmodsfum(p,e/2,f,wt);
		if ( !(e%2) )  {
			mulsfum(wt,wt,pr); DEG(pr) = divsfum(pr,f,q);
		} else {
			mulsfum(wt,wt,ws);
			DEG(ws) = divsfum(ws,f,q);
			mulsfum(ws,p,pr);
			DEG(pr) = divsfum(pr,f,q);
		}
	}
}

void spwrsfum(m,f,e,r)
UM f,m,r;
N e;
{
	UM t,s,q;
	N e1;
	int a;

	if ( !e ) {
		DEG(r) = 0; COEF(r)[0] = _onesf();
	} else if ( UNIN(e) )
		cpyum(f,r);
	else {
		a = divin(e,2,&e1);
		t = W_UMALLOC(2*DEG(m)); spwrsfum(m,f,e1,t);
		s = W_UMALLOC(2*DEG(m)); q = W_UMALLOC(2*DEG(m));
		mulsfum(t,t,s); DEG(s) = divsfum(s,m,q);
		if ( a ) {
			mulsfum(s,f,t); DEG(t) = divsfum(t,m,q); cpyum(t,r);
        } else
			cpyum(s,r);
	}
}

void tracemodsfum(m,f,e,r)
UM f,m,r;
int e;
{
	UM t,s,q,u;
	int i;

	q = W_UMALLOC(2*DEG(m)+DEG(f)); /* XXX */
	t = W_UMALLOC(2*DEG(m));
	s = W_UMALLOC(2*DEG(m));
	u = W_UMALLOC(2*DEG(m));
	DEG(f) = divsfum(f,m,q);
	cpyum(f,s);
	cpyum(f,t);
	for ( i = 1; i < e; i++ ) {
		mulsfum(t,t,u);
		DEG(u) = divsfum(u,m,q); cpyum(u,t);
		addsfum(t,s,u); cpyum(u,s);
	}
	cpyum(s,r);
}

void make_qmatsf(p,tab,mp)
UM p;
UM *tab;
int ***mp;
{
	int n,i,j;
	int *c;
	UM q,r;
	int **mat;
	int one;

	n = DEG(p);
	*mp = mat = almat(n,n);
	for ( j = 0; j < n; j++ ) {
		r = W_UMALLOC(DEG(tab[j])); q = W_UMALLOC(DEG(tab[j]));	
		cpyum(tab[j],r); DEG(r) = divsfum(r,p,q);
		for ( i = 0, c = COEF(r); i <= DEG(r); i++ )
			mat[i][j] = c[i];
	}
	one = _onesf();
	for ( i = 0; i < n; i++ )
		mat[i][i] = _subsf(mat[i][i],one);
}

void nullsf(mat,n,ind)
int **mat;
int *ind;
int n;
{
	int i,j,l,s,h,inv;
	int *t,*u;

	bzero((char *)ind,n*sizeof(int));
	ind[0] = 0;
	for ( i = j = 0; j < n; i++, j++ ) {
		for ( ; j < n; j++ ) {
			for ( l = i; l < n; l++ )
				if ( mat[l][j] )
					break;
			if ( l < n ) {
				t = mat[i]; mat[i] = mat[l]; mat[l] = t; break;
			} else
				ind[j] = 1;
		}
		if ( j == n )
			break;
		inv = _invsf(mat[i][j]);
		for ( s = j, t = mat[i]; s < n; s++ )
			t[s] = _mulsf(t[s],inv);
		for ( l = 0; l < n; l++ ) {
			if ( l == i )
				continue;
			u = mat[l]; h = _chsgnsf(u[j]);
			for ( s = j; s < n; s++ )
				u[s] = _addsf(_mulsf(h,t[s]),u[s]);
		}
	}
}

void null_to_solsf(mat,ind,n,r)
int **mat;
int *ind;
int n;
UM *r;
{
	int i,j,k,l;
	int *c;
	UM w;

	for ( i = 0, l = 0; i < n; i++ ) {
		if ( !ind[i] )
			continue;
		w = UMALLOC(n);
		for ( j = k = 0, c = COEF(w); j < n; j++ )
			if ( ind[j] )
				c[j] = 0;
			else
				c[j] = mat[k++][i];
		c[i] = _chsgnsf(_onesf());
		for ( j = n; j >= 0; j-- )
			if ( c[j] )
				break;
		DEG(w) = j;
		r[l++] = w;
	}
}
/*
make_qmatsf(p,tab,mp)
nullsf(mat,n,ind)
null_to_solsf(ind,n,r)
*/

void czsfum(f,r)
UM f,*r;
{
	int i,j;
	int d,n,ord;
	UM s,t,u,v,w,g,x,m,q;
	UM *base;

	n = DEG(f); base = (UM *)ALLOCA(n*sizeof(UM));
	bzero((char *)base,n*sizeof(UM));

	w = W_UMALLOC(2*n); q = W_UMALLOC(2*n); m = W_UMALLOC(2*n);

	base[0] = W_UMALLOC(0); DEG(base[0]) = 0; COEF(base[0])[0] = _onesf();

	t = W_UMALLOC(1); DEG(t) = 1; COEF(t)[0] = 0; COEF(t)[1] = _onesf();

	ord = field_order_sf();
	pwrmodsfum(t,ord,f,w);
	base[1] = W_UMALLOC(DEG(w));
	cpyum(w,base[1]);

	for ( i = 2; i < n; i++ ) {
		mulsfum(base[i-1],base[1],m);
		DEG(m) = divsfum(m,f,q);
		base[i] = W_UMALLOC(DEG(m)); cpyum(m,base[i]);
	}

	v = W_UMALLOC(n); cpyum(f,v);
	DEG(w) = 1; COEF(w)[0] = 0; COEF(w)[1] = _onesf();
	x = W_UMALLOC(1); DEG(x) = 1; COEF(x)[0] = 0; COEF(x)[1] = _onesf();
	t = W_UMALLOC(n); s = W_UMALLOC(n); u = W_UMALLOC(n); g = W_UMALLOC(n);

	for ( j = 0, d = 1; 2*d <= DEG(v); d++ ) {
		for ( DEG(t) = -1, i = 0; i <= DEG(w); i++ )
			if ( COEF(w)[i] ) {
				mulssfum(base[i],COEF(w)[i],s); 
				addsfum(s,t,u); cpyum(u,t);
			}
		cpyum(t,w); cpyum(v,s); subsfum(w,x,t);
		gcdsfum(s,t,g);
		if ( DEG(g) >= 1 ) {
			berlekampsf(g,d,base,r+j); j += DEG(g)/d;
			divsfum(v,g,q); cpyum(q,v);
			DEG(w) = divsfum(w,v,q);
			for ( i = 0; i < DEG(v); i++ )
				DEG(base[i]) = divsfum(base[i],v,q);
		}
	}
	if ( DEG(v) ) {
		r[j] = UMALLOC(DEG(v)); cpyum(v,r[j]); j++;
	}
	r[j] = 0;
}

int berlekampsf(p,df,tab,r)
UM p;
int df;
UM *tab,*r;
{
	int n,i,j,k,nf,d,nr;
	int **mat;
	int *ind;
	UM mp,w,q,gcd,w1,w2;
	UM *u;
	int *root;

	n = DEG(p);
	ind = ALLOCA(n*sizeof(int));
	make_qmatsf(p,tab,&mat);
	nullsf(mat,n,ind);
	for ( i = 0, d = 0; i < n; i++ )
		if ( ind[i] )
			d++;
	if ( d == 1 ) {
		r[0] = UMALLOC(n); cpyum(p,r[0]); return 1;
	}
	u = ALLOCA(d*sizeof(UM *));
	r[0] = UMALLOC(n); cpyum(p,r[0]);
	null_to_solsf(mat,ind,n,u);
	root = ALLOCA(d*sizeof(int));
	w = W_UMALLOC(n); mp = W_UMALLOC(d);
	w1 = W_UMALLOC(n); w2 = W_UMALLOC(n);
	for ( i = 1, nf = 1; i < d; i++ ) {
		minipolysf(u[i],p,mp);
		nr = find_rootsf(mp,root);
		for ( j = 0; j < nf; j++ ) {
			if ( DEG(r[j]) == df )
				continue;
			for ( k = 0; k < nr; k++ ) {
				cpyum(u[i],w1); cpyum(r[j],w2);
				COEF(w1)[0] = _chsgnsf(root[k]);
				gcdsfum(w1,w2,w);
				if ( DEG(w) > 0 && DEG(w) < DEG(r[j]) ) {
					gcd = UMALLOC(DEG(w));
					q = UMALLOC(DEG(r[j])-DEG(w));
					cpyum(w,gcd); divsfum(r[j],w,q);
					r[j] = q; r[nf++] = gcd;
				}
				if ( nf == d )
					return d;
			}
		}
	}
}

void minipolysf(f,p,mp)
UM f,p,mp;
{
	struct p_pair *list,*l,*l1,*lprev;	
	int n,d;
	UM u,p0,p1,np0,np1,q,w;

	list = (struct p_pair *)MALLOC(sizeof(struct p_pair));
	list->p0 = u = W_UMALLOC(0); DEG(u) = 0; COEF(u)[0] = _onesf();
	list->p1 = W_UMALLOC(0); cpyum(list->p0,list->p1);
	list->next = 0;
	n = DEG(p); w = UMALLOC(2*n);
	p0 = UMALLOC(2*n); cpyum(list->p0,p0);
	p1 = UMALLOC(2*n); cpyum(list->p1,p1);
	q = W_UMALLOC(2*n);
	while ( 1 ) {
		COEF(p0)[DEG(p0)] = 0; DEG(p0)++; COEF(p0)[DEG(p0)] = _onesf();
		mulsfum(f,p1,w); DEG(w) = divsfum(w,p,q); cpyum(w,p1);
		np0 = UMALLOC(n); np1 = UMALLOC(n);
		lnfsf(n,p0,p1,list,np0,np1);
		if ( DEG(np1) < 0 ) {
			cpyum(np0,mp); return;
		} else {
			l1 = (struct p_pair *)MALLOC(sizeof(struct p_pair));
			l1->p0 = np0; l1->p1 = np1;
			for ( l = list, lprev = 0, d = DEG(np1);
				l && (DEG(l->p1) > d); lprev = l, l = l->next );
			if ( lprev ) {
				lprev->next = l1; l1->next = l;
			} else {
				l1->next = list; list = l1;
			}
		}
	}
}

void lnfsf(n,p0,p1,list,np0,np1)
int n;
UM p0,p1;
struct p_pair *list;
UM np0,np1;
{
	int inv,h,d1;
	UM t0,t1,s0,s1;
	struct p_pair *l;

	cpyum(p0,np0); cpyum(p1,np1);
	t0 = W_UMALLOC(n); t1 = W_UMALLOC(n);
	s0 = W_UMALLOC(n); s1 = W_UMALLOC(n);
	for ( l = list; l; l = l->next ) {
		d1 = DEG(np1);
		if ( d1 == DEG(l->p1) ) {
			h = _divsf(COEF(np1)[d1],_chsgnsf(COEF(l->p1)[d1]));
			mulssfum(l->p0,h,t0); addsfum(np0,t0,s0); cpyum(s0,np0);
			mulssfum(l->p1,h,t1); addsfum(np1,t1,s1); cpyum(s1,np1);
		}
	}
}

int find_rootsf(p,root)
UM p;
int *root;
{
	UM *r;
	int i,j,n;

	n = DEG(p);
	r = ALLOCA((DEG(p))*sizeof(UM));
	canzassf(p,1,r);
	for ( i = 0; i < n; i++ )
		root[i] = _chsgnsf(COEF(r[i])[0]);
	return n;
}

void canzassf(f,d,r)
UM f,*r;
int d;
{
	UM t,s,u,w,g,o;
	N n1,n2,n3,n4,n5;
	UM *b;
	int n,m,i,q,ed;

	if ( DEG(f) == d ) {
		r[0] = UMALLOC(d); cpyum(f,r[0]);
		return;
	} else {
		n = DEG(f); b = (UM *)ALLOCA(n*sizeof(UM));
		bzero((char *)b,n*sizeof(UM));

		t = W_UMALLOC(2*d);
		s = W_UMALLOC(DEG(f)); u = W_UMALLOC(DEG(f)); 
		w = W_UMALLOC(DEG(f)); g = W_UMALLOC(DEG(f));
		o = W_UMALLOC(0); DEG(o) = 0; COEF(o)[0] = _onesf();
		q = field_order_sf();
		if ( q % 2 ) {
			STON(q,n1); pwrn(n1,d,&n2); subn(n2,ONEN,&n3);
			STON(2,n4); divsn(n3,n4,&n5);
		} else
			ed = d*extdeg_sf();
		while ( 1 ) {
			randsfum(2*d,t);
			if ( q % 2 ) {
				spwrsfum(f,t,n5,s); subsfum(s,o,u);
			} else
				tracemodsfum(f,t,ed,u);
			cpyum(f,w);
			gcdsfum(w,u,g);
			if ( (DEG(g) >= 1) && (DEG(g) < DEG(f)) ) {
				canzassf(g,d,r); 
				cpyum(f,w); divsfum(w,g,s);
				canzassf(s,d,r+DEG(g)/d); 
				return;
			}
		}
	}
}

