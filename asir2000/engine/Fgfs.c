/* $OpenXM: OpenXM_contrib2/asir2000/engine/Fgfs.c,v 1.10 2002/11/22 07:32:10 noro Exp $ */

#include "ca.h"

void cont_pp_mv_sf(VL vl,VL rvl,P p,P *c,P *pp);
void gcdsf_main(VL vl,P *pa,int m,P *r);
void ugcdsf(P *pa,int m,P *r);
void head_monomial(VL vl,V v,P p,P *coef,P *term);
void sqfrsfmain(VL vl,P f,DCP *dcp);
void pthrootsf(P f,Q m,P *r);
void partial_sqfrsf(VL vl,V v,P f,P *r,DCP *dcp);
void gcdsf(VL vl,P *pa,int k,P *r);
void mfctrsfmain(VL vl, P f, DCP *dcp);
void next_evaluation_point(int *mev,int n);
void estimatelc_sf(VL vl,VL rvl,P c,DCP dc,P *lcp);
void mfctrsf_hensel(VL vl,VL rvl,P f,P pp0,P u0,P v0,P lcu,P lcv,P *up);
void substvp_sf(VL vl,VL rvl,P f,int *mev,P *r);
void shift_sf(VL vl, VL rvl, P f, int *mev, int sgn, P *r);
void adjust_coef_sf(VL vl,VL rvl,P lcu,P u0,P *r);
void extended_gcd_modyk(P u0,P v0,V x,V y,int dy,P *cu,P *cv);
void poly_to_gfsn_poly(VL vl,P f,V v,P *r);
void gfsn_poly_to_poly(VL vl,P f,V v,P *r);
void poly_to_gfsn_poly_main(P f,V v,P *r);
void gfsn_poly_to_poly_main(P f,V v,P *r);
void gfsn_univariate_to_sfbm(P f,int dy,BM *r);
void sfbm_to_gfsn_univariate(BM f,V x,V y,P *r);

void lex_lc(P f,P *c)
{
	if ( !f || NUM(f) )
		*c = f;
	else
		lex_lc(COEF(DC(f)),c);
}

DCP append_dc(DCP dc,DCP dct)
{
	DCP dcs;

	if ( !dc )
		return dct;
	else {
		for ( dcs = dc; NEXT(dcs); dcs = NEXT(dcs) );
		NEXT (dcs) = dct;
		return dc;
	}
}

void sqfrsf(VL vl, P f, DCP *dcp)
{
	DCP dc,dct;
	Obj obj;
	P t,s,c;
	VL tvl,nvl;

	simp_ff((Obj)f,&obj); f = (P)obj;
	lex_lc(f,&c); divsp(vl,f,c,&t); f = t;
	monomialfctr(vl,f,&t,&dc); f = t;
	clctv(vl,f,&tvl); vl = tvl;
	if ( !vl )
		;
	else if ( !NEXT(vl) ) {
		sfusqfr(f,&dct);
		dc = append_dc(dc,NEXT(dct));
	} else {
		t = f;
		for ( tvl = vl; tvl; tvl = NEXT(tvl) ) {
			reordvar(vl,tvl->v,&nvl);
			cont_pp_mv_sf(vl,NEXT(nvl),t,&c,&s); t = s;
			sqfrsf(vl,c,&dct);
			dc = append_dc(dc,NEXT(dct));
		}
		sqfrsfmain(vl,t,&dct);
		dc = append_dc(dc,dct);
	}
	NEWDC(dct); DEG(dct) = ONE; COEF(dct) = (P)c; NEXT(dct) = dc;
	*dcp = dct;
}

void sqfrsfmain(VL vl,P f,DCP *dcp)
{
	VL tvl;
	DCP dc,dct,dcs;
	P t,s;
	Q m,m1;
	V v;

	clctv(vl,f,&tvl); vl = tvl;
	dc = 0;
	t = f;
	for ( tvl = vl; tvl; tvl = NEXT(tvl) ) {
		v = tvl->v;
		partial_sqfrsf(vl,v,t,&s,&dct); t = s;
		dc = append_dc(dc,dct);
	}
	if ( !NUM(t) ) {
		STOQ(characteristic_sf(),m);
		pthrootsf(t,m,&s);
		sqfrsfmain(vl,s,&dct);
		for ( dcs = dct; dcs; dcs = NEXT(dcs) ) {
			mulq(DEG(dcs),m,&m1); DEG(dcs) = m1;
		}
		dc = append_dc(dc,dct);
	}
	*dcp = dc;
}

void pthrootsf(P f,Q m,P *r)
{
	DCP dc,dc0,dct;
	N qn,rn;

	if ( NUM(f) )
		pthrootgfs(f,r);
	else {
		dc = DC(f);
		dc0 = 0;
		for ( dc0 = 0; dc; dc = NEXT(dc) ) {
			NEXTDC(dc0,dct);
			pthrootsf(COEF(dc),m,&COEF(dct));
			if ( DEG(dc) ) {
				divn(NM(DEG(dc)),NM(m),&qn,&rn);
				if ( rn )
					error("pthrootsf : cannot happen");
				NTOQ(qn,1,DEG(dct));
			} else
				DEG(dct) = 0;
		}
		NEXT(dct) = 0;
		MKP(VR(f),dc0,*r);
	}
}

void partial_sqfrsf(VL vl,V v,P f,P *r,DCP *dcp)
{
	P ps[2];
	DCP dc0,dc;
	int m;
	P t,flat,flat1,g,df,q;

	diffp(vl,f,v,&df);
	if ( !df ) {
		*dcp = 0;
		*r = f;
		return;
	}
	ps[0] = f; ps[1] = df;
	gcdsf(vl,ps,2,&g);	
	divsp(vl,f,g,&flat);
	m = 0;
	t = f;
	dc0 = 0;
	while ( !NUM(flat) ) {
		while ( divtp(vl,t,flat,&q) ) {
			t = q; m++;
		}
		ps[0] = t; ps[1] = flat;
		gcdsf(vl,ps,2,&flat1);
		divsp(vl,flat,flat1,&g);
		flat = flat1;
		NEXTDC(dc0,dc);
		COEF(dc) = g;
		STOQ(m,DEG(dc));
	}
	NEXT(dc) = 0;
	*dcp = dc0;
	*r = t;
}

void gcdsf(VL vl,P *pa,int k,P *r)
{
	P *ps,*pl,*pm;
	P **cp;
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
			ps[m++] = (P)obj;
	}
	if ( !m ) {
		*r = 0;
		return;
	}
	if ( m == 1 ) {
		*r = ps[0];
		return;
	}
	pl = (P *)ALLOCA(m*sizeof(P));
	ml = (DCP *)ALLOCA(m*sizeof(DCP));
	for ( i = 0; i < m; i++ )
		monomialfctr(vl,ps[i],&pl[i],&ml[i]);
	gcdmonomial(vl,ml,m,&mg); simp_ff((Obj)mg,&obj); mgsf = (P)obj;
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
	P *ps;
	int i;
	UM w1,w2,w3,w;
	int d;
	V v;

	if ( m == 1 ) {
		*r = pa[0];
		return;
	}
	for ( i = 0; i < m; i++ )
		if ( NUM(pa[i]) ) {
			itogfs(1,r);
			return;
		}
	ps = (P *)ALLOCA(m*sizeof(P));
	sort_by_deg(m,pa,ps);
	v = VR(ps[m-1]);
	d = getdeg(v,ps[m-1]);
	w1 = W_UMALLOC(d);
	w2 = W_UMALLOC(d);
	w3 = W_UMALLOC(d);
	ptosfum(ps[0],w1);
	for ( i = 1; i < m; i++ ) {
		ptosfum(ps[i],w2);
		gcdsfum(w1,w2,w3); 
		w = w1; w1 = w3; w3 = w;
		if ( !DEG(w1) ) {
			itogfs(1,r);
			return;
		}
	}
	sfumtop(v,w1,r);
}

/* deg(HT(p),v), where p is considered as distributed poly over F[v] */
int gethdeg(VL vl,V v,P p)
{
	DCP dc;
	Q dmax;
	P cmax;

	if ( !p )
		return -1;
	else if ( NUM(p) )
		return 0;
	else if ( VR(p) != v )
		/* HT(p) = HT(lc(p))*x^D */
		return gethdeg(vl,v,COEF(DC(p)));
	else {
		/* VR(p) = v */
		dc = DC(p); dmax = DEG(dc); cmax = COEF(dc);
		for ( dc = NEXT(dc); dc; dc = NEXT(dc) )
			if ( compp(vl,COEF(dc),cmax) > 0 ) {
				dmax = DEG(dc); cmax = COEF(dc);
			}
		return QTOS(dmax);
	}
}

/* all the pa[i]'s have the same variables (=vl) */

void gcdsf_main(VL vl,P *pa,int m,P *r)
{
	int nv,i,i0,imin,d,d0,d1,d2,dmin,index;
	V v,v0,vmin;
	VL tvl,nvl,rvl,nvl0,rvl0;
	P *pc, *ps, *ph,*lps;
	P x,t,cont,hg,g,hm,mod,s;
	P hge,ge,ce,he,u,cof1e,mode,mod1,adj,cof1,coadj,q;
	GFS sf;

	for ( nv = 0, tvl = vl; tvl; tvl = NEXT(tvl), nv++);
	if ( nv == 1 ) {
		ugcdsf(pa,m,r);
		return;
	}
	/* find v s.t. min(deg(pa[i],v)+gethdeg(pa[i],v)) is minimal */
	tvl = vl;
	do {
		v = tvl->v;
		i = 0;
		do {
			d = getdeg(v,pa[i])+gethdeg(vl,v,pa[i]);
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
	NEXT(rvl) = 0; rvl = rvl0;
	/* nvl = ...,vmin */
	NEXTVL(nvl0,nvl); nvl->v = vmin; NEXT(nvl) = 0; nvl = nvl0;
	MKV(vmin,x);

	/* for content and primitive part */
	pc = (P *)ALLOCA(m*sizeof(P));
	ps = (P *)ALLOCA(m*sizeof(P));
	ph = (P *)ALLOCA(m*sizeof(P));
	/* separate the contents */
	for ( i = 0; i < m; i++ ) {
		reorderp(nvl,vl,pa[i],&t);
		cont_pp_mv_sf(nvl,rvl,t,&pc[i],&ps[i]);
		head_monomial(nvl,vmin,ps[i],&ph[i],&t);
	}
	ugcdsf(pc,m,&cont);
	ugcdsf(ph,m,&hg);

	/* for hg*pp (used in check phase) */
	lps = (P *)ALLOCA(m*sizeof(P));
	for ( i = 0; i < m; i++ )
		mulp(nvl,hg,ps[i],&lps[i]);

	while ( 1 ) {
		g = 0;
		cof1 = 0;
		hm = 0;
		itogfs(1,&mod);
		index = 0;
		for ( index = 0; getdeg(vmin,mod) <= d+1; index++ ) {
			/* evaluation pt */
			indextogfs(index,&s);
			substp(nvl,hg,vmin,s,&hge);
			if ( !hge )
				continue;
			for ( i = 0; i < m; i++ )
				substp(nvl,ps[i],vmin,s,&ph[i]);
			/* ge = GCD(ps[0]|x=s,...,ps[m-1]|x=s) */
			gcdsf(nvl,ph,m,&ge);
			head_monomial(nvl,vmin,ge,&ce,&he);
			if ( NUM(he) ) {
				*r = cont;
				return;
			}
			divgfs((GFS)hge,(GFS)ce,&sf); t = (P)sf;
			mulp(nvl,t,ge,&u); ge = u;
			divsp(nvl,ph[imin],ge,&t); mulp(nvl,hge,t,&cof1e);
			/* hm=0 : reset; he==hm : lucky */
			if ( !hm || !compp(nvl,he,hm) ) {
				substp(nvl,mod,vmin,s,&mode); divsp(nvl,mod,mode,&mod1); 
				/* adj = mod/(mod|x=s)*(ge-g|x=s) */
				substp(nvl,g,vmin,s,&t);
				subp(nvl,ge,t,&u); mulp(nvl,mod1,u,&adj);
				/* coadj = mod/(mod|vmin=s)*(cof1e-cof1e|vmin=s) */
				substp(nvl,cof1,vmin,s,&t);
				subp(nvl,cof1e,t,&u); mulp(nvl,mod1,u,&coadj);
				if ( !adj ) {
					/* adj == gcd ? */
					for ( i = 0; i < m; i++ )
						if ( !divtp(nvl,lps[i],g,&t) )
							break;
					if ( i == m ) {
						cont_pp_mv_sf(nvl,rvl,g,&t,&u);
						mulp(nvl,cont,u,&t);
						reorderp(vl,nvl,t,r);
						return;
					}
				} else if ( !coadj ) {
					/* ps[imin]/coadj == gcd ? */
					if ( divtp(nvl,lps[imin],cof1,&q) ) {
						for ( i = 0; i < m; i++ )
							if ( !divtp(nvl,lps[i],q,&t) )
								break;
						if ( i == m ) {
							cont_pp_mv_sf(nvl,rvl,q,&t,&u);
							mulp(nvl,cont,u,&t);
							reorderp(vl,nvl,t,r);
							return;
						}
					}
				}
				addp(nvl,g,adj,&t); g = t;
				addp(nvl,cof1,coadj,&t); cof1 = t;
				subp(nvl,x,s,&t); mulp(nvl,mod,t,&u); mod = u;
				hm = he;
			} else {
				d1 = homdeg(hm); d2 = homdeg(he);
				if ( d1 < d2 ) /* we use current hm */
					continue;
				else if ( d1 > d2 ) {
					/* use he */
					g = ge;
					cof1 = cof1e;
					hm = he;
					subp(nvl,x,s,&mod);
				} else { 
					/* d1==d2, but hm!=he => both are unlucky */
					g = 0;
					cof1 = 0;
					itogfs(1,&mod);
				}
			}
		}
	}
}

void head_monomial(VL vl,V v,P p,P *coef,P *term)
{
	P t,s,u;
	DCP dc;
	GFS one;

	itogfs(1,&one);
	t = (P)one;
	while ( 1 ) {
		if ( NUM(p) || VR(p) == v ) {
			*coef = p;
			*term = t;
			return;
		} else {
			NEWDC(dc); 
			COEF(dc) = (P)one; DEG(dc) = DEG(DC(p));
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
	for ( t = BDY(dp), i = 0; t; t = NEXT(t), i++ )
		ps[i] = C(t);
	gcdsf(vl,ps,m,c);
	divsp(vl,p,*c,pp);
}

void mfctrsf(VL vl, P f, DCP *dcp)
{
	DCP dc0,dc,dct,dcs,dcr;
	Obj obj;

	simp_ff((Obj)f,&obj); f = (P)obj;
	sqfrsf(vl,f,&dct);
	dc = dc0 = dct; dct = NEXT(dct); NEXT(dc) = 0;
	for ( ; dct; dct = NEXT(dct) ) {
		mfctrsfmain(vl,COEF(dct),&dcs);
		for ( dcr = dcs; dcr; dcr = NEXT(dcr) )
			DEG(dcr) = DEG(dct);
		for ( ; NEXT(dc); dc = NEXT(dc) );
		NEXT(dc) = dcs;
	}
	*dcp = dc0;
}

/* f : sqfr, non const */

void mfctrsfmain(VL vl, P f, DCP *dcp)
{
	VL tvl,nvl,rvl;
	DCP dc,dc0,dc1,dc2,dct,lcfdc,dcs;
	int imin,inext,i,j,n,k,np;
	int *da;
	V vx,vy;
	V *va;
	P *l,*tl;
	P gcd,g,df,dfmin;
	P pa[2];
	P f0,pp0,spp0,c,c0,x,y,u,v,lcf,lcu,lcv,u0,v0,t,s;
	P ype,yme;
	GFS ev,evy;
	P *fp0;
	int *mev,*win;

	clctv(vl,f,&tvl); vl = tvl;
	if ( !vl )
		error("mfctrsfmain : cannot happen");
	if ( !NEXT(vl) ) {
		/* univariate */
		ufctrsf(f,&dc);
		/* remove lc */
		*dcp = NEXT(dc);
		return;
	}
	for ( n = 0, tvl = vl; tvl; tvl = NEXT(tvl), n++ );
	va = (V *)ALLOCA(n*sizeof(int));
	da = (int *)ALLOCA(n*sizeof(int));
	/* find v s.t. diff(f,v) is nonzero and deg(f,v) is minimal */
	imin = -1;
	for ( i = 0, tvl = vl; i < n; tvl = NEXT(tvl), i++ ) {
		va[i] = tvl->v;	
		da[i] = getdeg(va[i],f);
		diffp(vl,f,va[i],&df);
		if ( !df )
			continue;
		if ( imin < 0 || da[i] < da[imin] ) {
			dfmin = df;
			imin = i;
		}
	}
	/* find v1 neq v s.t. deg(f,v) is minimal */
	inext = -1;
	for ( i = 0; i < n; i++ ) {
		if ( i == imin )
			continue;
		if ( inext < 0 || da[i] < da[inext] )
			inext = i;
	}
	pa[0] = f;
	pa[1] = dfmin;
	gcdsf(vl,pa,2,&gcd);
	if ( !NUM(gcd) ) {
		/* f = gcd * f/gcd */	
		mfctrsfmain(vl,gcd,&dc1);
		divsp(vl,f,gcd,&g);
		mfctrsfmain(vl,g,&dc2);
		for ( dct = dc1; NEXT(dct); dct = NEXT(dct) );
		NEXT(dct) = dc2;
		*dcp = dc1;
		return;
	}
	/* create vl s.t. vl[0] = va[imin], vl[1] = va[inext] */
	nvl = 0;
	NEXTVL(nvl,tvl); tvl->v = va[imin];
	NEXTVL(nvl,tvl); tvl->v = va[inext];
	for ( i = 0; i < n; i++ ) {
		if ( i == imin || i == inext )
			continue;
		NEXTVL(nvl,tvl); tvl->v = va[i];
	}
	NEXT(tvl) = 0;

	reorderp(nvl,vl,f,&g); f = g;
	vx = nvl->v;
	vy = NEXT(nvl)->v;
	MKV(vx,x);
	MKV(vy,y);
	/* remaining variables */
	rvl = NEXT(NEXT(nvl));
	if ( !rvl ) {
		/* bivariate */
		sfbfctr(f,vx,vy,getdeg(vx,f),&dc1);
		for ( dc0 = 0; dc1; dc1 = NEXT(dc1) ) {
			NEXTDC(dc0,dc);
			DEG(dc) = ONE;
			reorderp(vl,nvl,COEF(dc1),&COEF(dc));
		}
		NEXT(dc) = 0;
		*dcp = dc0;
		return;
	}
	/* n >= 3;  nvl = (vx,vy,X) */
	/* find good evaluation pt for X */
	mev = (int *)CALLOC(n-2,sizeof(int));
	while ( 1 ) {
		/* lcf(mev)=0 => invalid */
		substvp_sf(nvl,rvl,COEF(DC(f)),mev,&t);
		if ( t ) {
			substvp_sf(nvl,rvl,f,mev,&f0);
			pa[0] = f0;
			diffp(nvl,f0,vx,&pa[1]);
			if ( pa[1] ) {
				gcdsf(nvl,pa,2,&gcd);
			/* XXX maybe we have to accept the case where gcd is a poly of y */
				if ( NUM(gcd) )
					break;
			}
		}
		/* XXX if generated indices exceed q of GF(q) => error in indextogfs */
		next_evaluation_point(mev,n-2);
	}
	/* f0 = f(x,y,mev) */
	/* separate content; f0 may have the content wrt x */
	cont_pp_sfp(nvl,f0,&c0,&pp0);

	/* factorize pp0; pp0 = pp0(x,y+evy) = prod dc */
	sfbfctr_shift(pp0,vx,vy,getdeg(vx,pp0),&evy,&spp0,&dc); pp0 = spp0;

	if ( !NEXT(dc) ) {
		/* f is irreducible */
		NEWDC(dc); DEG(dc) = ONE; COEF(dc) = f; NEXT(dc) = 0;
		*dcp = dc;
		return;
	}
	/* ype = y+evy, yme = y-evy */
	addp(nvl,y,(P)evy,&ype); subp(nvl,y,(P)evy,&yme);

	/* shift c0; c0 <- c0(y+evy) */
	substp(nvl,c0,vy,ype,&s); c0 = s;

	/* shift f; f <- f(y+evy) */
	substp(nvl,f,vy,ype,&s); f = s;

	/* now f(x,0,mev) = c0 * prod dc */

	/* factorize lc_x(f) */
	lcf = COEF(DC(f));
	mfctrsf(nvl,lcf,&dct); 
	/* skip the first element (= a number) */
	dct = NEXT(dct);

	/* shift lcfdc; c <- c(X+mev) */
	for ( lcfdc = 0; dct; dct = NEXT(dct) ) {
		NEXTDC(lcfdc,dcs);
		DEG(dcs) = DEG(dct);
		shift_sf(nvl,rvl,COEF(dct),mev,1,&COEF(dcs));
	}
	NEXT(dcs) = 0;

	/* np = number of bivariate factors */
	for ( np = 0, dct = dc; dct; dct = NEXT(dct), np++ );
	fp0 = (P *)ALLOCA((np+1)*sizeof(P));
	for ( i = 0, dct = dc; i < np; dct = NEXT(dct), i++ )
		fp0[i] = COEF(dct);
	fp0[np] = 0;
	l = tl = (P *)ALLOCA((np+1)*sizeof(P));
	win = W_ALLOC(np+1);

	/* f <- f(X+mev) */
	shift_sf(nvl,rvl,f,mev,1,&s); f = s;

	for ( k = 1, win[0] = 1, --np; ; ) {
		itogfs(1,&u0);
		/* u0 = product of selected factors */
		for ( i = 0; i < k; i++ ) {
			mulp(nvl,u0,fp0[win[i]],&t); u0 = t;
		}
		/* we have to consider the content */
		/* f0 = c0*u0*v0 */
		mulp(nvl,LC(u0),c0,&c); estimatelc_sf(nvl,rvl,c,lcfdc,&lcu);
		divsp(nvl,pp0,u0,&v0);
		mulp(nvl,LC(v0),c0,&c); estimatelc_sf(nvl,rvl,c,lcfdc,&lcv);
		mfctrsf_hensel(nvl,rvl,f,pp0,u0,v0,lcu,lcv,&u);
		if ( u ) {
			/* save the factor */
			reorderp(vl,nvl,u,&t);
			/* x -> x-mev, y -> y-evy */
			shift_sf(vl,rvl,t,mev,-1,&s); substp(vl,s,vy,yme,tl++);

			/* update f,pp0 */
			divsp(nvl,f,u,&t); f = t;
			divsp(nvl,pp0,u0,&t); pp0 = t;
			/* update win, fp0 */
			for ( i = 0; i < k-1; i++ )
			for ( j = win[i]+1; j < win[i+1]; j++ )
				fp0[j-i-1] = fp0[j];
			for ( j = win[k-1]+1; j <= np; j++ )
					fp0[j-k] = fp0[j];
			if ( ( np -= k ) < k ) break;
			if ( np-win[0]+1 < k )
				if ( ++k <= np ) {
					for ( i = 0; i < k; i++ ) 
						win[i] = i + 1;
					continue;
				} else
					break;
			else 
				for ( i = 1; i < k; i++ ) 
					win[i] = win[0] + i;
		} else {
			if ( ncombi(1,np,k,win) == 0 )
				if ( k == np ) break;
				else 
					for ( i = 0, ++k; i < k; i++ ) 
						win[i] = i + 1;
		}
	}
	reorderp(vl,nvl,f,&t);
	/* x -> x-mev, y -> y-evy */
	shift_sf(vl,rvl,t,mev,-1,&s); substp(vl,s,vy,yme,tl++);
	*tl = 0;
	for ( dc0 = 0, i = 0; l[i]; i++ ) {
		NEXTDC(dc0,dc); DEG(dc) = ONE; COEF(dc) = l[i];
	}
	NEXT(dc) = 0; *dcp = dc0;
}

void next_evaluation_point(int *e,int n)
{
	int i,t,j;

	for ( i = n-1; i >= 0; i-- )
		if ( e[i] ) break;
	if ( i < 0 ) e[n-1] = 1;
	else if ( i == 0 ) {
		t = e[0]; e[0] = 0; e[n-1] = t+1;
	} else {
		e[i-1]++; t = e[i];
		for ( j = i; j < n-1; j++ )
			e[j] = 0;
		e[n-1] = t-1;
	}
}

/* 
 * dc : f1^E1*...*fk^Ek
 * find e1,...,ek s.t. fi(0)^ei | c 
 * and return f1^e1*...*fk^ek
 * vl = (vx,vy,rvl)
 */

void estimatelc_sf(VL vl,VL rvl,P c,DCP dc,P *lcp)
{
	DCP dct;
	P r,c1,c2,t,s,f;
	int i,d;
	Q q;

	for ( dct = dc, r = (P)ONE; dct; dct = NEXT(dct) ) {
		if ( NUM(COEF(dct)) )
			continue;
		/* constant part */
		substvp_sf(vl,rvl,COEF(dct),0,&f);
		d = QTOS(DEG(dct));
		for ( i = 0, c1 = c; i < d; i++ )
			if ( !divtp(vl,c1,f,&c2) )
				break;
			else
				c1 = c2;
		if ( i ) {
			STOQ(i,q);
			pwrp(vl,COEF(dct),q,&s); mulp(vl,r,s,&t); r = t;
		}
	}
	*lcp = r;
}

void substvp_sf(VL vl,VL rvl,P f,int *mev,P *r)
{
	int i;
	VL tvl;
	P g,t;
	GFS ev;

	for ( g = f, i = 0, tvl = rvl; tvl; tvl = NEXT(tvl), i++ ) {
		if ( !mev )
			ev = 0;
		else
			indextogfs(mev[i],&ev);
		substp(vl,g,tvl->v,(P)ev,&t); g = t;
	}
	*r = g;
}

/*
 * f <- f(X+sgn*mev)
 */

void shift_sf(VL vl, VL rvl, P f, int *mev, int sgn, P *r)
{
	VL tvl;
	int i;
	P x,g,t,s;
	GFS ev;

	for ( g = f, tvl = rvl, i = 0; tvl; tvl = NEXT(tvl), i++ ) {
		if ( !mev[i] )
			continue;
		indextogfs(mev[i],&ev);
		MKV(tvl->v,x);
		if ( sgn > 0 )
			addp(vl,x,(P)ev,&t);
		else
			subp(vl,x,(P)ev,&t);
		substp(vl,g,tvl->v,t,&s); g = s;
	}
	*r = g;
}

/*
 * pp(f(0)) = u0*v0
 */

void mfctrsf_hensel(VL vl,VL rvl,P f,P pp0,P u0,P v0,P lcu,P lcv,P *up)
{
	VL tvl,onevl;
	P t,s,w,u,v,ff,si,wu,wv,fj,cont;
	UM ydy;
	V vx,vy;
	int dy,n,i,dbd,nv,j;
	int *md;
	P *uh,*vh;
	P x,du0,dv0,m,q,r;
	P *cu,*cv;
	GFSN inv;

	/* adjust coeffs */
	/* u0 = am x^m+ ... -> lcu*x^m + a(m-1)*(lcu(0)/am)*x^(m-1)+... */
	/* v0 = bm x^l+ ... -> lcv*x^l + b(l-1)*(lcv(0)/bl)*x^(l-1)+... */
	/* f                -> lcu*lcv*x^(m+l)+... */
	adjust_coef_sf(vl,rvl,lcu,u0,&u);
	adjust_coef_sf(vl,rvl,lcv,v0,&v);
	mulp(vl,lcu,lcv,&t); divsp(vl,t,LC(f),&m); mulp(vl,m,f,&t); f = t;

	vx = vl->v; vy = NEXT(vl)->v;
	n = getdeg(vx,f);
	dy = getdeg(vy,f)+1;
	MKV(vx,x);
	cu = (P *)ALLOCA((n+1)*sizeof(P));
	cv = (P *)ALLOCA((n+1)*sizeof(P));

	/* ydy = y^dy */
	ydy = C_UMALLOC(dy); DEG(ydy) = dy; COEF(ydy)[dy] = _onesf();
	setmod_gfsn(ydy);

	/* (R[y]/(y^dy))[x,X] */
	poly_to_gfsn_poly(vl,f,vy,&ff);
	poly_to_gfsn_poly(vl,u,vy,&t); u = t;
	poly_to_gfsn_poly(vl,v,vy,&t); v = t;
	substvp_sf(vl,rvl,u,0,&u0);
	substvp_sf(vl,rvl,v,0,&v0);

	/* compute a(x,y), b(x,y) s.t. a*u0+b*v0 = 1 mod y^dy */
	extended_gcd_modyk(u0,v0,vx,vy,dy,&cu[0],&cv[0]);

	/* dv0 = LC(v0)^(-1)*v0 mod y^dy */
	invgfsn((GFSN)LC(v0),&inv); mulp(vl,v0,(P)inv,&dv0);

	/* cu[i]*u0+cv[i]*v0 = x^i mod y^dy */
	/* (x*cu[i])*u0+(x*cv[i])*v0 = x^(i+1) */
	/* x*cu[i] = q*dv0+r => cu[i+1] = r */
	/* cv[i+1]*v0 = x*cv[i]*v0+q*u0*dv0 = (x*cv[i]+q*u0*inv)*v0 */
	for ( i = 1; i <= n; i++ ) {
		mulp(vl,x,cu[i-1],&m); divsrp(vl,m,dv0,&q,&cu[i]);
		mulp(vl,x,cv[i-1],&m); mulp(vl,q,(P)inv,&t);
		mulp(vl,t,u0,&s);
		addp(vl,m,s,&cv[i]);
	}

#if 0
	/* XXX : check */
	for ( i = 0; i <= n; i++ ) {
		mulp(vl,cu[i],u0,&m); mulp(vl,cv[i],v0,&s);
		addp(vl,m,s,&w);
		printexpr(vl,w);
		fprintf(asir_out,"\n");
	}
#endif

	dbd = dbound(vx,f)+1;	

	/* extract homogeneous parts */
	W_CALLOC(dbd,P,uh); W_CALLOC(dbd,P,vh);
	for ( i = 0; i <= dbd; i++ ) {
		exthpc(vl,vx,u,i,&uh[i]); exthpc(vl,vx,v,i,&vh[i]);
	}

	/* register degrees in each variables */
	for ( nv = 0, tvl = rvl; tvl; tvl = NEXT(tvl), nv++ );
	md = (int *)ALLOCA(nv*sizeof(int));
	for ( i = 0, tvl = rvl; i < nv; tvl = NEXT(tvl), i++ )
		md[i] = getdeg(tvl->v,f);

	/* XXX for removing content of factor wrt vx */
	NEWVL(onevl); onevl->v = vx; NEXT(onevl) = 0;

	for ( j = 1; j <= dbd; j++ ) {
		for ( i = 0, tvl = rvl; i < nv; tvl = NEXT(tvl), i++ )
			if ( getdeg(tvl->v,u)+getdeg(tvl->v,v) > md[i] ) {
				*up = 0;
				return;
			}
		for ( i = 0, t = 0; i <= j; i++ ) {
			mulp(vl,uh[i],vh[j-i],&s); addp(vl,s,t,&w); t = w;
		}

		/* s = degree j part of (f-uv) */
		exthpc(vl,vx,ff,j,&fj); subp(vl,fj,t,&s);
		for ( i = 0, wu = 0, wv = 0; i <= n; i++ ) {
			if ( !s )
				si = 0;
			else if ( VR(s) == vx )
				coefp(s,i,&si);
			else if ( i == 0 )
				si = s;
			else
				si = 0;
			if ( si ) {
				mulp(vl,si,cv[i],&m); addp(vl,wu,m,&t); wu = t;
				mulp(vl,si,cu[i],&m); addp(vl,wv,m,&t); wv = t;
			}
		}
		if ( !wu ) {
			gfsn_poly_to_poly(vl,u,vy,&t);
			if ( divtp(vl,f,t,&q) ) {
				cont_pp_mv_sf(vl,onevl,t,&cont,up);
				return;
			}
		}
		if ( !wv ) {
			gfsn_poly_to_poly(vl,v,vy,&t);
			if ( divtp(vl,f,t,&q) ) {
				cont_pp_mv_sf(vl,onevl,q,&cont,up);
				return;
			}
		}
		addp(vl,u,wu,&t); u = t;
		addp(vl,uh[j],wu,&t); uh[j] = t;
		addp(vl,v,wv,&t); v = t;
		addp(vl,vh[j],wv,&t); vh[j] = t;
	}
}

void adjust_coef_sf(VL vl,VL rvl,P lcu,P u0,P *r)
{
	P lcu0,cu;
	DCP dc0,dcu,dc;

	substvp_sf(vl,rvl,lcu,0,&lcu0);
	divsp(vl,lcu0,LC(u0),&cu);
	for ( dc0 = 0, dcu = DC(u0); dcu; dcu = NEXT(dcu) ) {
		if ( !dc0 ) {
			NEXTDC(dc0,dc);
			COEF(dc) = lcu;
		} else {
			NEXTDC(dc0,dc);
			mulp(vl,cu,COEF(dcu),&COEF(dc));
		}
		DEG(dc) = DEG(dcu);
	}
	NEXT(dc) = 0;
	MKP(VR(u0),dc0,*r);
}

void extended_gcd_modyk(P u0,P v0,V x,V y,int dy,P *cu,P *cv)
{
	BM g,h,a,b;

	gfsn_univariate_to_sfbm(u0,dy,&g);	
	gfsn_univariate_to_sfbm(v0,dy,&h);	
	sfexgcd_by_hensel(g,h,dy,&a,&b);
	sfbm_to_gfsn_univariate(a,x,y,cu);
	sfbm_to_gfsn_univariate(b,x,y,cv);
}

/* (F[y])[x] -> F[x][y] */

void gfsn_univariate_to_sfbm(P f,int dy,BM *r)
{
	int dx,d,i;
	BM b;
	UM cy;
	DCP dc;

	dx = getdeg(VR(f),f);	
	b = BMALLOC(dx,dy);
	DEG(b) = dy;
	for ( dc = DC(f); dc; dc = NEXT(dc) ) {
		/* d : degree in x, cy : poly in y */
		d = QTOS(DEG(dc));
		cy = BDY((GFSN)COEF(dc));
		for ( i = DEG(cy); i >= 0; i-- )
			COEF(COEF(b)[i])[d] = COEF(cy)[i];
	}
	for ( i = 0; i <= dy; i++ )
		degum(COEF(b)[i],dx);
	*r = b;
}

void sfbm_to_gfsn_univariate(BM f,V x,V y,P *r)
{
	P g;
	VL vl;

	sfbmtop(f,x,y,&g);
	NEWVL(vl); vl->v = x;
	NEWVL(NEXT(vl)); NEXT(vl)->v = y;
	NEXT(NEXT(vl)) = 0;
	poly_to_gfsn_poly(vl,g,y,r);
}

void poly_to_gfsn_poly(VL vl,P f,V v,P *r)
{
	VL tvl,nvl0,nvl;
	P g;

	/* (x,y,...,v,...) -> (x,y,...,v) */
	for ( nvl0 = 0, tvl = vl; tvl; tvl = NEXT(tvl) ) {
		if ( tvl->v != v ) {
			NEXTVL(nvl0,nvl);
			nvl->v = tvl->v;
		}
	}
	NEXTVL(nvl0,nvl);
	nvl->v = v;
	NEXT(nvl) = 0;
	reorderp(nvl0,vl,f,&g);
	poly_to_gfsn_poly_main(g,v,r);
}

void poly_to_gfsn_poly_main(P f,V v,P *r)
{
	int d;
	UM u;
	GFSN g;
	DCP dc,dct,dc0;

	if ( !f )
		*r = f;
	else if ( NUM(f) || VR(f) == v ) {
		d = getdeg(v,f);
		u = UMALLOC(d);
		ptosfum(f,u);		
		MKGFSN(u,g);
		*r = (P)g;
	} else {
		for ( dc0 = 0, dct = DC(f); dct; dct = NEXT(dct) ) {
			NEXTDC(dc0,dc);
			DEG(dc) = DEG(dct);
			poly_to_gfsn_poly_main(COEF(dct),v,&COEF(dc));
		}
		NEXT(dc) = 0;
		MKP(VR(f),dc0,*r);
	}
}

void gfsn_poly_to_poly(VL vl,P f,V v,P *r)
{
	VL tvl,nvl0,nvl;
	P g;

	gfsn_poly_to_poly_main(f,v,&g);
	/* (x,y,...,v,...) -> (x,y,...,v) */
	for ( nvl0 = 0, tvl = vl; tvl; tvl = NEXT(tvl) ) {
		if ( tvl->v != v ) {
			NEXTVL(nvl0,nvl);
			nvl->v = tvl->v;
		}
	}
	NEXTVL(nvl0,nvl);
	nvl->v = v;
	NEXT(nvl) = 0;
	reorderp(vl,nvl0,g,r);
}

void gfsn_poly_to_poly_main(P f,V v,P *r)
{
	DCP dc,dc0,dct;

	if ( !f )
		*r = f;
	else if ( NUM(f) ) {
		if ( NID((Num)f) == N_GFSN )
			sfumtop(v,BDY((GFSN)f),r);
		else
			*r = f;
	} else {
		for ( dc0 = 0, dct = DC(f); dct; dct = NEXT(dct) ) {
			NEXTDC(dc0,dc);
			DEG(dc) = DEG(dct);
			gfsn_poly_to_poly_main(COEF(dct),v,&COEF(dc));
		}
		NEXT(dc) = 0;
		MKP(VR(f),dc0,*r);
	}
}

void printsfum(UM f)
{
	int i;

	for ( i = DEG(f); i >= 0; i-- ) {
		printf("+(");
		printf("%d",IFTOF(COEF(f)[i])); 
		printf(")*y^%d",i);
	}
}

void printsfbm(BM f)
{
	int i;

	for ( i = DEG(f); i >= 0; i-- ) {
		printf("+(");
		printsfum(COEF(f)[i]); 
		printf(")*y^%d",i);
	}
}

