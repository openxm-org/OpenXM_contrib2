/* $OpenXM: OpenXM_contrib2/asir2000/engine/Hgfs.c,v 1.10 2001/06/26 08:52:59 noro Exp $ */

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
void sfbmtop(int,BM,V,V,P *);
void pp_sfp(VL,P,P *);
void sfcsump(VL,P,P *);
void mulsfbmarray(int,BM,ML,int,int *,V,V,P *);
void const_term(P,UM);

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

/* Hensel related functions */

int sfberle(VL,P,int,GFS *,DCP *);
void sfgcdgen(P,ML,ML *);
void sfhenmain(LUM,ML,ML,ML *);
void ptosflum(int,P,LUM);
void sfhenmain2(BM,UM,UM,int,BM *);
void ptosfbm(int,P,BM);

/* f = f(x,y) */

void sfhensel(count,f,x,evp,sfp,listp)
int count;
P f;
V x;
GFS *evp;
P *sfp;
ML *listp;
{
	int i,j;
	int fn,n,bound;
	ML rlist;
	BM fl;
	VL vl,nvl;
	V y;
	int dx,dy,mev;
	GFS ev;
	P f1,t,yev,c,sf;
	DCP dc;
	UM w,w1,q,fm,hm;
	UM *gm;

	clctv(CO,f,&vl);
	if ( vl->v != x ) {
		reordvar(vl,x,&nvl); reorderp(nvl,vl,f,&f1);
		vl = nvl; f = f1;
	}
	y = vl->next->v;
	dx = getdeg(x,f);
	dy = getdeg(y,f);
	if ( dx == 1 ) {
		*listp = rlist = MLALLOC(1); rlist->n = 1; rlist->c[0] = 0;
		return;
	}
	fn = sfberle(vl,f,count,&ev,&dc);
	if ( fn <= 1 ) {
		/* fn == 0 => short of evaluation points */
		*listp = rlist = MLALLOC(1); rlist->n = fn; rlist->c[0] = 0;
		return;
	}
	/* pass the the leading coeff. to the first element */
	c = dc->c; dc = NEXT(dc);
	mulp(vl,dc->c,c,&t); dc->c = t;

	/* convert mod y-a factors into UM */
	gm = (UM *)ALLOCA(fn*sizeof(UM));
	for ( i = 0; i < fn; i++, dc = NEXT(dc) ) {
		gm[i] = W_UMALLOC(UDEG(dc->c));
		ptosfum(dc->c,gm[i]);
	}

	bound = dy+1;
	/* f(x,y) -> f(x,y+ev) */
	fl = BMALLOC(dx,bound);
	ptosfbm(bound,f,fl);
	shiftsfbm(bound,fl,FTOIF(CONT(ev)));

	/* sf = f(x+ev) */
	sfbmtop(bound,fl,x,y,&sf);

	/* fm = fl mod y */
	fm = W_UMALLOC(dx);
	cpyum(COEF(fl)[0],fm);
	hm = W_UMALLOC(dx);

	q = W_UMALLOC(dx);
	rlist = MLALLOC(fn); rlist->n = fn; rlist->bound = bound;
	for ( i = 0; i < fn-1; i++ ) {
		fprintf(stderr,"%d\n",i);
		/* fl = gm[i]*hm mod y */
		divsfum(fm,gm[i],hm);
		/* fl is replaced by the cofactor of gk mod y^bound */
		/* rlist->c[i] = gk */
		sfhenmain2(fl,gm[i],hm,bound,(BM *)&rlist->c[i]);
		cpyum(hm,fm);
	}
	/* finally, fl must be the lift of gm[fn-1] */
	rlist->c[i] = fl;

#if 0
	/* y -> y-a */
	mev = _chsgnsf(FTOIF(CONT(ev)));
	for ( i = 0; i < fn; i++ )
		shiftsfbm(bound,(BM)(rlist->c[i]),mev);
#endif
	*evp = ev;
	*sfp = sf;
	*listp = rlist;
}

/* main variable of f = x */

int sfberle(vl,f,count,ev,dcp)
VL vl;
P f;
int count;
GFS *ev;
DCP *dcp;
{
	UM wf,wf1,wf2,wfs,gcd;
	ML flist;
	int fn,fn1,n;
	GFS m,fm;
	DCP dc,dct,dc0;
	VL nvl;
	V x,y;
	P g,lc,lc0,f0;
	int j,q1,index,i;

	clctv(vl,f,&nvl); vl = nvl;
	x = vl->v; y = vl->next->v;
	simp_ff(f,&g); g = f;
	n = QTOS(DEG(DC(f)));
	wf = W_UMALLOC(n); wf1 = W_UMALLOC(n); wf2 = W_UMALLOC(n);
	wfs = W_UMALLOC(n); gcd = W_UMALLOC(n);
	q1 = field_order_sf()-1;
	lc = DC(f)->c;
	for ( j = 0, fn = n + 1, index = 0; 
		index < q1 && j < count && fn > 1; index++ ) {
		MKGFS(index,m);
		substp(vl,lc,y,(P)m,&lc0);
		if ( lc0 ) {
			substp(vl,f,y,(P)m,&f0);
			ptosfum(f0,wf); cpyum(wf,wf1); 
			diffsfum(wf1,wf2); gcdsfum(wf1,wf2,gcd);
			if ( DEG(gcd) == 0 ) {
				fctrsf(f0,&dc);
				for ( dct = NEXT(dc), i = 0; dct; dct = NEXT(dct), i++ );
				if ( i < fn ) {
					dc0 = dc; fn = i; fm = m;
				}
				j++;
			}
		}
	}
	if ( index == q1 )
		return 0;
	else if ( fn == 1 )
		return 1;
	else {
		*dcp = dc0;
		*ev = fm;
		return fn;
	}
}

void sfgcdgen(f,blist,clistp)
P f;
ML blist,*clistp;
{
	int i;
	int n,d,np;
	UM wf,wm,wx,wy,wu,wv,wa,wb,wg,q,tum;
	UM *in,*out;
	ML clist;

	n = UDEG(f); np = blist->n;
	d = 2*n;
	q = W_UMALLOC(d); wf = W_UMALLOC(d);
	wm = W_UMALLOC(d); wx = W_UMALLOC(d);
	wy = W_UMALLOC(d); wu = W_UMALLOC(d);
	wv = W_UMALLOC(d); wg = W_UMALLOC(d);
	wa = W_UMALLOC(d); wb = W_UMALLOC(d);
	ptosfum(f,wf); DEG(wg) = 0; COEF(wg)[0] = _onesf();
	*clistp = clist = MLALLOC(np); clist->n = np;
	for ( i = 0, in = (UM *)blist->c, out = (UM *)clist->c; i < np; i++ ) {
		divsfum(wf,in[i],q); tum = wf; wf = q; q = tum;
		cpyum(wf,wx); cpyum(in[i],wy);
		eucsfum(wx,wy,wa,wb); mulsfum(wa,wg,wm);
		DEG(wm) = divsfum(wm,in[i],q); out[i] = UMALLOC(DEG(wm));
		cpyum(wm,out[i]); mulsfum(q,wf,wu);
		mulsfum(wg,wb,wv); addsfum(wu,wv,wg);
	}
}

/*
	sfhenmain(fl,bqlist,cqlist,listp)
*/

void sfhenmain(f,bqlist,cqlist,listp)
LUM f;
ML bqlist,cqlist,*listp;
{
	int i,j,k;
	int *px,*py;
	int **pp,**pp1;
	int n,np,bound,dr,tmp;
	UM wt,wq0,wq,wr,wm,wm0,wa,q;
	LUM wb0,wb1,tlum;
	UM *b,*c;
	LUM *l;
	ML list;

	n = DEG(f); np = bqlist->n; bound = bqlist->bound;
	*listp = list = MLALLOC(n);
	list->n = np; list->bound = bound;
	W_LUMALLOC(n,bound,wb0); W_LUMALLOC(n,bound,wb1);
	wt = W_UMALLOC(n); wq0 = W_UMALLOC(n); wq = W_UMALLOC(n);
	wr = W_UMALLOC(n); wm = W_UMALLOC(2*n); wm0 = W_UMALLOC(2*n);
	wa = W_UMALLOC(2*n); q = W_UMALLOC(2*n);
	b = (UM *)bqlist->c; c = (UM *)cqlist->c; l = (LUM *)list->c; 
	for ( i = 0; i < np; i++ ) {
		l[i] = LUMALLOC(DEG(b[i]),bound);
		for ( j = DEG(b[i]), pp = COEF(l[i]), px = COEF(b[i]); j >= 0; j-- ) 
			pp[j][0] = px[j];
	}
	for ( i = 1; i < bound; i++ ) {
		fprintf(stderr,".");
		/* at this point, f = l[0]*l[1]*...*l[np-1] mod y^i */
		mulsflum(i+1,l[0],l[1],wb0);
		for ( j = 2; j < np; j++ ) {
			mulsflum(i+1,l[j],wb0,wb1);
			tlum = wb0; wb0 = wb1; wb1 = tlum;
		}
#if 0
		/* check */
		for ( j = 0, pp = COEF(f), pp1 = COEF(wb0); j <= n; j++ )
			for ( k = 0; k < i; k++ )
				if ( pp[j][k] != pp1[j][k] )
					error("henmain : cannot happen");
#endif
		for ( j = n, px = COEF(wt); j >= 0; j-- )
			px[j] = 0;
		for ( j = n, pp = COEF(f), pp1 = COEF(wb0); j >= 0; j-- )
			COEF(wt)[j] = _subsf(pp[j][i],pp1[j][i]);
		degum(wt,n);
		for ( j = n, px = COEF(wq0); j >= 0; j-- )
			px[j] = 0;
		for ( j = 1; j < np; j++ ) {
			mulsfum(wt,c[j],wm); dr = divsfum(wm,b[j],q);
			for ( k = DEG(q), px = COEF(wq0), py = COEF(q); k >= 0; k-- ) 
				px[k] = _addsf(px[k],py[k]);
			for ( k = dr, pp = COEF(l[j]), px = COEF(wm); k >= 0; k-- ) 
				pp[k][i] = px[k];
		}
		degum(wq0,n); mulsfum(wq0,b[0],wm);
		mulsfum(wt,c[0],wm0); addsfum(wm,wm0,wa);
		for ( j = DEG(wa), pp = COEF(l[0]), px = COEF(wa); j >= 0; j-- ) 
			pp[j][i] = px[j];
		for ( j = n, px = COEF(wq0); j >= 0; j-- ) 
			px[j] = 0;
	}
	fprintf(stderr,"\n");
}

/* f = g0*h0 mod y -> f = gk*hk mod y^bound, f is replaced by hk */

void sfhenmain2(f,g0,h0,bound,gp)
BM f;
UM g0,h0;
int bound;
BM *gp;
{
	int i,j,k,l;
	int *px,*py;
	int **pp,**pp1;
	int n,np,dr,tmp;
	UM wt,wa,wb,wq,wm,q,w1,w2,wh1,wg1,ws;
	UM wc,wd,we,wz;
	BM wb0,wb1;
	int ng,nh;
	BM fk,gk,hk;
	
	n = degsfbm(bound,f);
	ng = g0->d;
	nh = h0->d;

	W_BMALLOC(n,bound,wb0); W_BMALLOC(n,bound,wb1);
	wt = W_UMALLOC(n); ws = W_UMALLOC(n);
	wq = W_UMALLOC(n); q = W_UMALLOC(2*n);
	wg1 = W_UMALLOC(2*n); wh1 = W_UMALLOC(2*n);

	/* fk = gk*hk mod y^k */
	W_BMALLOC(n,bound,fk);
	cpyum(COEF(f)[0],COEF(fk)[0]);
	gk = BMALLOC(ng,bound);
	cpyum(g0,COEF(gk)[0]);
	W_BMALLOC(nh,bound,hk);
	cpyum(h0,COEF(hk)[0]);

	wc = W_UMALLOC(2*n); wd = W_UMALLOC(2*n);
	we = W_UMALLOC(2*n); wz = W_UMALLOC(2*n);

	/* compute wa,wb s.t. wa*g0+wb*h0 = 1 mod y */
	w1 = W_UMALLOC(ng); cpyum(g0,w1);
	w2 = W_UMALLOC(nh); cpyum(h0,w2);
	wa = W_UMALLOC(2*n); wb = W_UMALLOC(2*n);  /* XXX */
	eucsfum(w1,w2,wa,wb);

#if 0
	mulsfum(wa,g0,wc); mulsfum(wb,h0,wd); addsfum(wc,wd,we);
	if ( DEG(we) != 0 || COEF(we)[0] != _onesf() )
			error("henmain2 : cannot happen(euc)");
#endif

	fprintf(stderr,"bound=%d\n",bound);
	for ( k = 1; k < bound; k++ ) {
		fprintf(stderr,".");

		/* at this point, f = gk*hk mod y^k */
#if 0
		for ( j = 0; j < k; j++ )
			if ( !isequalum(COEF(f)[j],COEF(fk)[j]) )
					error("henmain2 : cannot happen(history)");
#endif

		/* clear wt */
		bzero(COEF(wt),(n+1)*sizeof(int));

		/* wt = (f-gk*hk)/y^k */
		subsfum(COEF(f)[k],COEF(fk)[k],wt);
		
		/* clear wq */
		bzero(COEF(wq),(n+1)*sizeof(int));

		/* compute wf1,wg1 s.t. wh1*g0+wg1*h0 = wt */
		mulsfum(wa,wt,wh1); DEG(wh1) = divsfum(wh1,h0,q);
		mulsfum(wh1,g0,wc); subsfum(wt,wc,wd); DEG(wd) = divsfum(wd,h0,wg1);

		/* check */
#if 0
		if ( DEG(wd) >= 0 || DEG(wg1) > ng )
			error("henmain2 : cannot happen(adj)");

		mulsfum(wg1,h0,wc); mulsfum(wh1,g0,wd); addsfum(wc,wd,we);
		subsfum(we,wt,wz);
		if ( DEG(wz) >= 0 )
			error("henmain2 : cannot happen");
#endif

		/* fk += ((wg1*hk+wh1*gk)*y^k+wg1*wh1*y^(2*k) mod y^bound */
		/* wb0 = wh1*y^k */
		clearsfbm(bound,n,wb0);
		DEG(wb0) = bound;
		cpyum(wh1,COEF(wb0)[k]);

		/* wb1 = gk*wb0 mod y^bound */
		clearsfbm(bound,n,wb1);
		mulsfbm(bound,gk,wb0,wb1);
		/* fk += wb1 */
		addtosfbm(bound,wb1,fk);

		/* wb0 = wg1*y^k */
		clearsfbm(bound,n,wb0);
		DEG(wb0) = bound;
		cpyum(wg1,COEF(wb0)[k]);

		/* wb1 = hk*wb0 mod y^bound */
		clearsfbm(bound,n,wb1);
		mulsfbm(bound,hk,wb0,wb1);
		/* fk += wb1 */
		addtosfbm(bound,wb1,fk);

		/* fk += wg1*wh1*y^(2*k) mod y^bound */
		if ( 2*k < bound ) {
			mulsfum(wg1,wh1,wt); addsfum(COEF(fk)[2*k],wt,ws);
			cpyum(ws,COEF(fk)[2*k]);
		}

		/* gk += wg1*y^k, hk += wh1*y^k */
		cpyum(wg1,COEF(gk)[k]);
		cpyum(wh1,COEF(hk)[k]);
	}
	fprintf(stderr,"\n");
	*gp = gk;
	DEG(f) = bound;
	for ( i = 0; i < bound; i++ )
		cpyum(COEF(hk)[i],COEF(f)[i]);
}

void ptosflum(bound,f,fl)
int bound;
P f;
LUM fl;
{
	DCP dc;
	int **pp;
	int d;
	UM t;

	t = UMALLOC(bound);
	for ( dc = DC(f), pp = COEF(fl); dc; dc = NEXT(dc) ) {
		d = QTOS(DEG(dc));
		ptosfum(COEF(dc),t);
		bcopy(t->c,pp[d],(t->d+1)*sizeof(int));
	}
}

/* fl->c[i] = coef_y(f,i) */

void ptosfbm(bound,f,fl)
int bound;
P f;
BM fl;
{
	DCP dc;
	int d,i,n;
	UM t;

	n = QTOS(DEG(DC(f)));
	clearsfbm(bound,n,fl);
	DEG(fl) = bound;
	t = UMALLOC(bound);
	for ( dc = DC(f); dc; dc = NEXT(dc) ) {
		d = QTOS(DEG(dc));
		ptosfum(COEF(dc),t);
		for ( i = 0; i <= DEG(t); i++ )
			COEF(COEF(fl)[i])[d] = COEF(t)[i];
	}
	for ( i = 0; i < bound; i++ )
		degum(COEF(fl)[i],n);
}

/* x : main variable */

void sflumtop(bound,fl,x,y,fp)
int bound;
LUM fl;
V x,y;
P *fp;
{
	int i,j,n;
	int **c;
	UM w;
	int *coef;
	DCP dc,dct;

	n = fl->d;
	c = fl->c;
	w = W_UMALLOC(bound);
	for ( i = 0, dc = 0; i <= n; i++ ) {
		coef = c[i];
		for ( j = bound-1; j >= 0 && coef[j] == 0; j-- );
		if ( j < 0 )
			continue;
		DEG(w) = j; bcopy(coef,COEF(w),(j+1)*sizeof(int));
		NEWDC(dct); STOQ(i,DEG(dct));
		sfumtop(y,w,&COEF(dct)); NEXT(dct) = dc; dc = dct;
	}
	MKP(x,dc,*fp);
}

/* x : main variable */

void sfbmtop(bound,f,x,y,fp)
int bound;
BM f;
V x,y;
P *fp;
{
	UM *c;
	int i,j,d,a;
	GFS b;
	DCP dc0,dc,dct;

	c = COEF(f);
	d = DEG(c[0]);
	for ( i = 1; i < bound; i++ )
		d = MAX(DEG(c[i]),d);

	dc0 = 0;
	for ( i = 0; i <= d; i++ ) {
		dc = 0;
		for ( j = 0; j < bound; j++ ) {
			if ( DEG(c[j]) >= i && (a = COEF(c[j])[i]) ) {
				NEWDC(dct);
				STOQ(j,DEG(dct));
				MKGFS(IFTOF(a),b);
				COEF(dct) = (P)b;
				NEXT(dct) = dc;
				dc = dct;
			}
		}
		if ( dc ) {
			NEWDC(dct);
			STOQ(i,DEG(dct));
			MKP(y,dc,COEF(dct));
			NEXT(dct) = dc0;
			dc0 = dct;
		}
	}
	if ( dc0 )
		MKP(x,dc0,*fp);
	else
		*fp = 0;
}

void sfdtest(P,ML,V,V,DCP *);

void sfbfctr(f,x,y,dcp)
P f;
V x,y;
DCP *dcp;
{
	ML list;
	P sf;
	GFS ev;
	DCP dc,dct;
	BM fl;
	int n,bound;

	/* sf(x) = f(x+ev) = list->c[0]*list->c[1]*... */
	sfhensel(5,f,x,&ev,&sf,&list);
	sfdtest(sf,list,x,y,&dc);
	n = getdeg(x,sf);
	bound = list->bound;
	W_BMALLOC(n,bound,fl);
	for ( dct = dc; dct; dct = NEXT(dct) ) {
		ptosfbm(bound,COEF(dct),fl);
		shiftsfbm(bound,fl,_chsgnsf(FTOIF(CONT(ev))));
		sfbmtop(bound,fl,x,y,&COEF(dct));
	}
	*dcp = dc;
}

/* f = f(x,y) = list->c[0]*list->c[1]*... mod y^list->bound */

void sfdtest(f,list,x,y,dcp)
P f;
ML list;
V x,y;
DCP *dcp;
{
	int n,np,bound;
	int i,j,k;
	int *win;
	P g,lcg,factor,cofactor,lcyx;
	P t,csum;
	DCP dcf,dcf0,dc;
	BM *c;
	BM lcy;
	UM lcg0;
	ML wlist;
	struct oVL vl1,vl0;
	VL vl;
	int z;

	/* vl = [x,y] */
	vl0.v = x; vl0.next = &vl1; vl1.v = y; vl1.next = 0; vl = &vl0;

	/* setup various structures and arrays */
	n = UDEG(f); np = list->n; bound = list->bound; win = W_ALLOC(np+1);
	wlist = W_MLALLOC(np); wlist->n = list->n;
	wlist->bound = list->bound;
	c = (BM *)COEF(wlist);
	bcopy((char *)COEF(list),(char *)c,(int)(sizeof(BM)*np));

	lcg0 = W_UMALLOC(2*bound);

	/* initialize g by f */
	g = f;

	/* initialize lcg */
	mulp(vl,g,COEF(DC(g)),&lcg);

	/* initialize lcg0 */
	const_term(lcg,lcg0);

	/* initialize csum = lcg(1) */
	sfcsump(vl,lcg,&csum);

	/* initialize lcy by LC(f) */
	W_BMALLOC(0,bound,lcy);
	NEWDC(dc); COEF(dc) = COEF(DC(g)); DEG(dc) = 0; 
	NEWP(lcyx); VR(lcyx) = x; DC(lcyx) = dc;
	ptosfbm(bound,lcyx,lcy);

	fprintf(stderr,"np = %d\n",np);
	for ( g = f, k = 1, dcf = dcf0 = 0, win[0] = 1, --np, z = 0; ; z++ ) {
		if ( !(z % 1000) ) fprintf(stderr,".");
		if ( sfdtestmain(vl,lcg,lcg0,lcy,csum,wlist,k,win,&factor,&cofactor) ) {
			NEXTDC(dcf0,dcf); DEG(dcf) = ONE; COEF(dcf) = factor;
			g = cofactor;

			/* update lcg */
			mulp(vl,g,COEF(DC(g)),&lcg);

			/* update lcg0 */
			const_term(lcg,lcg0);

			/* update csum */
			sfcsump(vl,lcg,&csum);

			/* update lcy */
			clearsfbm(bound,0,lcy);
			COEF(dc) = COEF(DC(g)); ptosfbm(bound,lcyx,lcy);

			for ( i = 0; i < k - 1; i++ ) 
				for ( j = win[i] + 1; j < win[i + 1]; j++ ) 
					c[j-i-1] = c[j];
			for ( j = win[k-1] + 1; j <= np; j++ ) 	
					c[j-k] = c[j];
			if ( ( np -= k ) < k ) 
				break;
			if ( np - win[0] + 1 < k ) 
				if ( ++k > np )
					break;
				else
					for ( i = 0; i < k; i++ ) 
						win[i] = i + 1;
			else 
				for ( i = 1; i < k; i++ ) 
					win[i] = win[0] + i;
		} else if ( !ncombi(1,np,k,win) ) 
			if ( k == np ) 
				break;
			else
				for ( i = 0, ++k; i < k; i++ ) 
					win[i] = i + 1;
	}
	fprintf(stderr,"\n");
	NEXTDC(dcf0,dcf); COEF(dcf) = g;
	DEG(dcf) = ONE; NEXT(dcf) = 0; *dcp = dcf0;
}

/* lcy = LC(g), lcg = lcy*g, lcg0 = const part of lcg */
int sfdtestmain(vl,lcg,lcg0,lcy,csum,list,k,in,fp,cofp)
VL vl;
P lcg;
UM lcg0;
BM lcy;
P csum;
ML list;
int k;
int *in;
P *fp,*cofp;
{
	P fmul,csumg,q;
	V x,y;

	x = vl->v;
	y = vl->next->v;
	if (!sfctest(lcg0,lcy,list,k,in))
		return 0;
	mulsfbmarray(UDEG(lcg),lcy,list,k,in,x,y,&fmul);
	if ( csum ) {
		sfcsump(vl,fmul,&csumg);
		if ( csumg ) {
			if ( !divtp(vl,csum,csumg,&q) )
				return 0;
		} 
	}
	if ( divtp_by_sfbm(vl,lcg,fmul,&q) ) {
		pp_sfp(vl,fmul,fp);
		pp_sfp(vl,q,cofp);
		return 1;
	} else
		return 0;
}

void const_term(f,c)
P f;
UM c;
{
	DCP dc;

	for ( dc = DC(f); dc && DEG(dc); dc = NEXT(dc) );
	if ( dc ) 
		ptosfum(COEF(dc),c);
	else 
		DEG(c) = -1;
}

void const_term_sfbm(f,bound,c)
BM f;
int bound;
UM c;
{
	int i;

	for ( i = 0; i < bound; i++ )
		if ( DEG(COEF(f)[i]) >= 0 )
			COEF(c)[i] = COEF(COEF(f)[i])[0];
		else
			COEF(c)[i] = 0;
	degum(c,bound-1);
}

/* lcy*(product of const part) | lcg0 ? */

int sfctest(lcg0,lcy,list,k,in)
UM lcg0;
BM lcy;
ML list;
int k;
int *in;
{
	DCP dc;
	int bound,i,dr;
	UM t,s,u,w;
	BM *l;

	bound = list->bound;
	t = W_UMALLOC(2*bound);
	s = W_UMALLOC(2*bound);
	u = W_UMALLOC(2*bound);
	const_term_sfbm(lcy,bound,t);
	if ( DEG(t) < 0 )
		return 1;

	l = (BM *)list->c;
	for ( i = 0; i < k; i++ ) {
		const_term_sfbm(l[in[i]],bound,s);
		mulsfum(t,s,u);
		if ( DEG(u) >= bound )
			degum(u,bound-1);
		w = t; t = u; u = w;
	}
	cpyum(lcg0,s);
	dr = divsfum(s,t,u);
	if ( dr >= 0 )
		return 0;
	else
		return 1;
}

/* main var of f is x */

void mulsfbmarray(n,lcy,list,k,in,x,y,g)
int n;
BM lcy;
ML list;
int k;
int *in;
V x,y;
P *g;
{
	int bound,i;
	BM wb0,wb1,t,lcbm;
	BM *l;

	bound = list->bound;
	W_BMALLOC(n,bound,wb0); W_BMALLOC(n,bound,wb1);
	l = (BM *)list->c;
	clearsfbm(bound,n,wb0);
	mulsfbm(bound,lcy,l[in[0]],wb0);
	for ( i = 1; i < k; i++ ) {
		clearsfbm(bound,n,wb1);
		mulsfbm(bound,l[in[i]],wb0,wb1);
		t = wb0; wb0 = wb1; wb1 = t;
	}
	sfbmtop(bound,wb0,x,y,g);
}

void sfcsump(vl,f,s)
VL vl;
P f;
P *s;
{
	P t,u;
	DCP dc;

	for ( dc = DC(f), t = 0; dc; dc = NEXT(dc) ) {
		addp(vl,COEF(dc),t,&u); t = u;
	}
	*s = t;
}

/* *fp = primitive part of f w.r.t. x */

void pp_sfp(vl,f,fp)
VL vl;
P f;
P *fp;
{
	V x,y;
	int d;
	UM t,s,gcd;
	DCP dc;
	P dvr;

	x = vl->v;
	y = vl->next->v;
	d = getdeg(y,f);
	if ( d == 0 )
		*fp = f;  /* XXX */
	else {
		t = W_UMALLOC(2*d);
		s = W_UMALLOC(2*d);
		gcd = W_UMALLOC(2*d);
		dc = DC(f);
		ptosfum(COEF(dc),gcd);
		for ( dc = NEXT(dc); dc; dc = NEXT(dc) ) {
			ptosfum(COEF(dc),t);
			gcdsfum(gcd,t,s);
			cpyum(s,gcd);
		}
		sfumtop(y,gcd,&dvr);
		divsp(vl,f,dvr,fp);
	}
}

int divtp_by_sfbm(vl,f,g,qp)
VL vl;
P f,g;
P *qp;
{
	V x,y;
	int fx,fy,gx,gy;
	BM fl,gl,ql;
	UM *cf,*cg,*cq;
	UM hg,q,t,s;
	int i,j,dr;

	x = vl->v; y = vl->next->v;
	fx = getdeg(x,f); fy = getdeg(y,f);
	gx = getdeg(x,g); gy = getdeg(y,g);

	if ( fx < gx || fy < gy )
		return 0;
	W_BMALLOC(fx,fy+1,fl); ptosfbm(fy+1,f,fl); cf = COEF(fl);
	W_BMALLOC(gx,gy+1,gl); ptosfbm(gy+1,g,gl); cg = COEF(gl);
	W_BMALLOC(fx-gx,fy-gy+1,ql); cq = COEF(ql);

	hg = cg[gy];
	q = W_UMALLOC(fx); t = W_UMALLOC(fx); s = W_UMALLOC(fx);

	for ( i = fy; i >= gy; i-- ) {
		if ( DEG(cf[i]) < 0 )
			continue;
		dr = divsfum(cf[i],hg,q);
		if ( dr >= 0 )
			return 0;
		if ( DEG(q) > fx-gx )
			return 0;
		cpyum(q,cq[i-gy]);
		for ( j = 0; j <= gy; j++ ) {
			mulsfum(cg[j],q,t);
			subsfum(cf[j+i-gy],t,s);
			cpyum(s,cf[j+i-gy]);
		}
	}
	for ( j = gy-1; j >= 0 && DEG(cf[j]) < 0; j-- );
	if ( j >= 0 )
		return 0;
	sfbmtop(DEG(ql),ql,x,y,qp);
}
