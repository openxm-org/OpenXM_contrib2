/* $OpenXM$ */

#include "ca.h"


void gcdsf(VL vl,P *pa,int k,P *r)
{
	P *ps,*pl,*pm,*cp;
	int *cn;
	DCP *ml;
	Obj obj;
	int i,j,l,m;
	P mg,mgsf,t;
	VL avl,nvl,tvl,svl;

	ps = (P *)ALLOCA(k*sizeof(P));
	for ( i = 0, m = 0; i < k; i++ ) {
		simp_ff((Obj)pa[i],&obj);
		if ( obj )
			ps[m++] = obj;
	}
	if ( !m ) {
		*r = 0;
		return;
	}
	if ( m == 1 ) {
		*r = BDY(n0);
		return;
	}
	pl = (P *)ALLOCA(m*sizeof(P));
	ml = (DCP *)ALLOCA(m*sizeof(DCP));
	for ( i = 0; i < m; i++ )
		monomialfctr(vl,ps[i],&pl[i],&ml[i]);
	gcdmonomial(vl,ml,m,&mg); simp_ff(mg,&mgsf);
	for ( i = 0, nvl = vl, avl = 0; nvl && i < m; i++ ) {
		clctv(vl,pl[i],&tvl);
		intersectv(nvl,tvl,&svl); nvl = svl;
		mergev(vl,avl,tvl,&svl); avl = svl;
	}
	if ( !nvl ) {
		*r = mgsf;	
		return;
	}
	if ( !NEXT(avl) ) {
		ugcdsf(pl,m,&t);
		mulp(vl,mgsf,t,r);	
		return;
	}
	for ( tvl = nvl, i = 0; tvl; tvl = NEXT(tvl), i++ );
	for ( tvl = avl, j = 0; tvl; tvl = NEXT(tvl), j++ );
	if ( i == j ) {
		/* all the pl[i]'s have the same variables */
		gcdsf_main(avl,pl,m,&t);
	} else {
		cp = (P **)ALLOCA(m*sizeof(P *));
		cn = (int *)ALLOCA(m*sizeof(int));
		for ( i = 0; i < m; i++ ) {
			cp[i] = (P *)ALLOCA(lengthp(pl[i])*sizeof(P));
			cn[i] = pcoef(vl,nvl,pl[i],cp[i]);
		}
		for ( i = j = 0; i < m; i++ )
			j += cn[i];
		pm = (P *)ALLOCA(j*sizeof(P));
		for ( i = l = 0; i < m; i++ )
			for ( j = 0; j < cn[i]; j++ )
				pm[l++] = cp[i][j];
		gcdsf(vl,pm,l,&t);
	}
	mulp(vl,mgsf,t,r);	
}

/* univariate gcd */

void ugcdsf(P *pa,int m,P *r)
{
	P *pa;
	int i;
	UM w1,w2,w3,w;
	int d;
	V v;

	if ( m == 1 ) {
		*r = pa[0];
		return;
	}
	ps = (P *)ALLOCA(m*sizeof(P));
	v = VR(pa[0]);
	sort_by_deg(m,pa,ps);
	d = getdeg(ps[m-1],v);
	w1 = W_UMALLOC(d);
	w2 = W_UMALLOC(d);
	w3 = W_UMALLOC(d);
	ptosfum(ps[0],w1);
	for ( i = 1; i < m; i++ ) {
		ptosfum(ps[i],w2);
		gcdsfum(w1,w2,w3); 
		w = w1; w1 = w3; w3 = w;
		if ( !DEG(w1) ) {
			MKGFS(0,*r);
			return;
		}
	}
	sfumtop(v,w1,r);
}


/* all the pa[i]'s have the same variables (=vl) */

void gcdsf_main(VL vl,P *pa,int m,P *r)
{
	for ( nv = 0, tvl = vl; tvl; tvl = NEXT(tvl), nv++);
	if ( nv == 1 ) {
		ugcdsf(pa,m,r);
		return;
	}
	/* find v s.t. min(deg(pa[i],v)) is minimal */
	tvl = vl;
	do {
		v = tvl->v;
		i = 0;
		do {
			d = getdeg(pa[i],v);
			if ( i == 0 || (d < d0) ) {
				d0 = d; i0 = i; v0 = v;
			}
		} while ( ++i < m );
		if ( tvl == vl || (d0 < dmin) ) {
			dmin = d0; imin = i0; vmin = v0;
		}
	} while ( tvl = NEXT(tvl) );

	/* reorder variables so that vmin is the last variable */
	for ( nvl0 = 0, rvl0 = 0, tvl = vl; tvl; tvl = NEXT(tvl) )
		if ( tvl->v != vmin ) {
			NEXTVL(nvl0,nvl); nvl->v = tvl->v;
			NEXTVL(rvl0,rvl); rvl->v = tvl->v;
		}
	/* rvl = remaining variables */
	NEXT(rvl) = 0;
	/* nvl = ...,vmin */
	NEXTVL(nvl0,nvl); nvl->v = vmin; NEXT(nvl) = 0;

	/* for content and primitive part */
	pc = (P *)ALLOCA(m*sizeof(P));
	ps = (P *)ALLOCA(m*sizeof(P));
	ph = (P *)ALLOCA(m*sizeof(P));
	/* separate the contents */
	for ( i = 0; i < m; i++ ) {
		reorderp(vl,nvl,pa[i],&t);
		cont_pp_mv_sf(nvl,rvl,t,&pc[i],&ps[i]);
		head_monomial(vmin,ps[i],&ph[i],&t);
	}
	ugcdsf(pc,m,&cont);
	ugcdsf(ph,m,&hg);

	/* for hg*pp (used in check phase) */
	lps = (P *)ALLOCA(m*sizeof(P));
	for ( i = 0; i < m; i++ )
		mulp(nvl,hg,ps[i],&lps[i]);

	while ( 1 ) {
		g = 0;
		cofmin = 0;
		hm = 0;
		MKGFS(0,mod);
		index = 0;
		while ( getdeg(mod,vmin) <= d+1 ) {
			/* evaluation pt */
			MKGFS(index,s);
			substp(nvl,hg,vmin,s,&hge);
			if ( !hge )
				continue;
			for ( i = 0; i < m; i++ )
				substp(nvl,ps[i],vmin,s,&ph[i]);
			/* ge = GCD(ps[0]|x=s,...,ps[m-1]|x=s) */
			gcdsf(nvl,ph,m,&ge);
			head_monomial(vmin,ge,&ce,&he);
			if ( NUM(hge) ) {
				*r = cont;
				return;
			}
			divgfs(hge,ce,&t); mulp(nvl,t,ge,&u); ge = u;
			divsp(nvl,ph[imin],ge,&t); mulp(nvl,hge,t,&cof1e);
			if ( !hm || !cmpp(he,hm) ) {
			}
		}
	}
}

void head_monomial(V v,P p,P *coef,P *term)
{
	P t,s,u;
	DCP dc;
	GFS one;

	MKGFS(0,one); t = one;
	while ( 1 ) {
		if ( NUM(p) || VR(p) == v ) {
			*coef = p;
			*term = t;
			return;
		} else {
			NEWDC(dc); MKGFS(0,one); 
			COEF(dc) = one; DEG(dc) = DEG(DC(p));
			MKP(VR(p),dc,s);
			mulp(vl,t,s,&u); t = u;
			p = COEF(DC(p));
		}
	}
}

void cont_pp_mv_sf(VL vl,VL rvl,P p,P *c,P *pp)
{
	DP dp;
	MP t;
	int i,m;
	P *ps;

	ptod(vl,rvl,p,&dp);
	for ( t = BDY(dp), m = 0; t; t = NEXT(t), m++ );
	ps = (P *)ALLOCA(m*sizeof(P));
	for ( t = BDY(dp), i = 0; t; t = NEXT(t), i++ );
		ps[i] = C(t);
	ugcdsf(ps,m,c);
	divsp(vl,p,c,pp);
}
