/* $OpenXM: OpenXM_contrib2/asir2000/engine/Fgfs.c,v 1.3 2002/09/27 08:40:48 noro Exp $ */

#include "ca.h"

void cont_pp_mv_sf(VL vl,VL rvl,P p,P *c,P *pp);
void gcdsf_main(VL vl,P *pa,int m,P *r);
void ugcdsf(P *pa,int m,P *r);
void head_monomial(V v,P p,P *coef,P *term);
void sqfrsfmain(VL vl,P f,DCP *dcp);
void pthrootsf(P f,Q m,P *r);
void partial_sqfrsf(VL vl,V v,P f,P *r,DCP *dcp);
void gcdsf(VL vl,P *pa,int k,P *r);

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
			head_monomial(vmin,ge,&ce,&he);
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

void head_monomial(V v,P p,P *coef,P *term)
{
	P t,s,u;
	DCP dc;
	GFS one;
	VL vl;

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
	ugcdsf(ps,m,c);
	divsp(vl,p,*c,pp);
}