/* $OpenXM: OpenXM/src/asir99/engine/NEZ.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"

void nezgcdnpz(vl,ps,m,pr)
VL vl;
P *ps,*pr;
int m;
{
	P t,s,mg;
	VL tvl,svl,avl,nvl;
	int i,j,k;
	N c;
	Q cq;
	P *pl,*pm;
	DCP *ml;
	Q *cl;
	P **cp;
	int *cn;

	pl = (P *)ALLOCA(m*sizeof(P)); 
	ml = (DCP *)ALLOCA(m*sizeof(DCP)); 
	cl = (Q *)ALLOCA(m*sizeof(Q));
	for ( i = 0; i < m; i++ )
		monomialfctr(vl,ps[i],&pl[i],&ml[i]);
	gcdmonomial(vl,ml,m,&mg);
	for ( i = 0; i < m; i++ ) {
		ptozp(pl[i],1,&cl[i],&t); pl[i] = t;
	}
	for ( i = 1, cq = cl[0]; i < m; i++ ) {
		gcdn(NM(cl[i]),NM(cq),&c); NTOQ(c,1,cq);
	}
	for ( i = 0; i < m; i++ ) 
		if ( NUM(pl[i]) ) {
			mulp(vl,(P)cq,mg,pr); return;
		}
	for ( i = 0, nvl = vl, avl = 0; nvl && i < m; i++ ) {
		clctv(vl,pl[i],&tvl);
		intersectv(nvl,tvl,&svl); nvl = svl;
		mergev(vl,avl,tvl,&svl); avl = svl;
	}
	if ( !nvl ) {
		mulp(vl,(P)cq,mg,pr); return;
	}
	if ( !NEXT(avl) ) {
		nuezgcdnpzmain(vl,pl,m,&t); mulp(vl,t,(P)cq,&s); mulp(vl,s,mg,pr);
		return;
	}
	for ( tvl = nvl, i = 0; tvl; tvl = NEXT(tvl), i++ );
	for ( tvl = avl, j = 0; tvl; tvl = NEXT(tvl), j++ );
	if ( i == j ) {
		/* all the pl[i]'s have the same variables */
		sortplistbyhomdeg(pl,m); nezgcdnpzmain(nvl,pl,m,&t);
#if 0
		/* search the minimal degree poly */
		for ( i = 0; i < m; i++ ) {
        	for ( tvl = nvl; tvl; tvl = NEXT(tvl) ) {
                dt = getdeg(tvl->v,pl[i]);
                if ( tvl == nvl || dt < d1 ) {
                        v1 = tvl->v; d1 = dt;
                }
        	}
			if ( i == 0 || d1 < d ) {
				v = v1; d = d1; j = i;
			}
		}
		t = pl[0]; pl[0] = pl[j]; pl[j] = t;
		if ( v != nvl->v ) {
			reordvar(nvl,v,&mvl);
			for ( i = 0; i < m; i++ ) {
				reorderp(mvl,nvl,pl[i],&t); pl[i] = t;
			}
			nezgcdnpzmain(mvl,pl,m,&s); reorderp(nvl,mvl,s,&t);
		} else
			nezgcdnpzmain(nvl,pl,m,&t);
#endif
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
		for ( i = k = 0; i < m; i++ )
			for ( j = 0; j < cn[i]; j++ )
				pm[k++] = cp[i][j];
		nezgcdnpz(vl,pm,k,&t);						
	}
	mulp(vl,t,(P)cq,&s); mulp(vl,s,mg,pr);
}

void sortplistbyhomdeg(p,n)
P *p;
int n;
{
	int i,j,k;
	int *l;
	P t;

	l = (int *)ALLOCA(n*sizeof(int));
	for ( i = 0; i < n; i++ )
		l[i] = homdeg(p[i]);
	for ( i = 0; i < n; i++ )
		for ( j = i + 1; j < n; j++ )
			if ( l[j] < l[i] ) {
				t = p[i]; p[i] = p[j]; p[j] = t;
				k = l[i]; l[i] = l[j]; l[j] = k;
			}
}

void nuezgcdnpzmain(vl,ps,m,r)
VL vl;
P *ps;
int m;
P *r;
{
	P *tps;
	P f,t;
	int i;

	for ( i = 0; i < m; i++ )
		if ( NUM(ps[i]) ) {
			*r = (P)ONE; return;
		}
	tps = (P *) ALLOCA(m*sizeof(P));
	for ( i = 0; i < m; i++ )
		tps[i] = ps[i];
	sortplist(tps,m);	
	for ( i = 1, f = tps[0]; i < m && !NUM(f); i++ ) {
		uezgcdpz(vl,f,tps[i],&t); f = t;
	}
	*r = f;
}

void gcdmonomial(vl,dcl,m,r)
VL vl;
DCP *dcl;
int m;
P *r;
{
	int i,j,n;
	P g,x,s,t;
	Q d;
	DCP dc;
	VN vn;

	for ( i = 0; i < m; i++ ) 
		if ( !dcl[i] ) {
			*r = (P)ONE; return;
		}
	for ( n = 0, dc = dcl[0]; dc; dc = NEXT(dc), n++ );
	vn = (VN)ALLOCA(n*sizeof(struct oVN));
	for ( i = 0, dc = dcl[0]; i < n; dc = NEXT(dc), i++ ) {
		vn[i].v = VR(COEF(dc)); vn[i].n = QTOS(DEG(dc));
	}
	for ( i = 1; i < m; i++ ) {
		for ( j = 0; j < n; j++ ) {
			for ( dc = dcl[i]; dc; dc = NEXT(dc) )
				if ( VR(COEF(dc)) == vn[j].v ) {
					vn[j].n = MIN(vn[j].n,QTOS(DEG(dc))); break;
				}
			if ( !dc )
				vn[j].n = 0;
		}
		for ( j = n-1; j >= 0 && !vn[j].n; j-- );
		if ( j < 0 ) {
			*r = (P)ONE; return;
		} else
			n = j+1;
	}
	for ( j = 0, g = (P)ONE; j < n; j++ )
		if ( vn[j].n ) {
			MKV(vn[j].v,x); STOQ(vn[j].n,d); 
			pwrp(vl,x,d,&t); mulp(vl,t,g,&s); g = s;
		}
	*r = g;
}

void nezgcdnpzmain(vl,pl,m,pr)
VL vl;
P *pl,*pr;
int m;
{
	P *ppl,*pcl;
	int i;
	P cont,gcd,t,s;
	DCP dc;
	
	ppl = (P *)ALLOCA(m*sizeof(P));
	pcl = (P *)ALLOCA(m*sizeof(P));
	for ( i = 0; i < m; i++ )
		pcp(vl,pl[i],&ppl[i],&pcl[i]);
	nezgcdnpz(vl,pcl,m,&cont);						
	sqfrp(vl,ppl[0],&dc);
	for ( dc = NEXT(dc), gcd = (P)ONE; dc; dc = NEXT(dc) ) {
		if ( NUM(COEF(dc)) )
			continue;
		nezgcdnpp(vl,dc,ppl+1,m-1,&t);
		if ( NUM(t) ) 
			continue;
		mulp(vl,gcd,t,&s); gcd = s;
	}
	mulp(vl,gcd,cont,pr);
}

void nezgcdnpp(vl,dc,pl,m,r)
VL vl;
DCP dc;
P *pl;
int m;
P *r;
{
	int i,k;
	P g,t,s,gcd;
	P *pm;

	nezgcdnp_sqfr_primitive(vl,COEF(dc),pl,m,&gcd);
	if ( NUM(gcd) ) {
		*r = (P)ONE; return;
	}
	pm = (P *) ALLOCA(m*sizeof(P));
	for ( i = 0; i < m; i++ ) {
		divsp(vl,pl[i],gcd,&pm[i]);
		if ( NUM(pm[i]) ) {
			*r = gcd; return;
		}
	}
	for ( g = gcd, k = QTOS(DEG(dc))-1; k > 0; k-- ) {
		nezgcdnp_sqfr_primitive(vl,g,pm,m,&t);
		if ( NUM(t) )
			break;
		mulp(vl,gcd,t,&s); gcd = s;
		for ( i = 0; i < m; i++ ) {
			divsp(vl,pm[i],t,&s);
			if ( NUM(s) ) {
				*r = gcd; return;
			}
			pm[i] = s;
		}
	}
	*r = gcd;
}

/*
 * pr = gcd(p0,ps[0],...,ps[m-1])
 *
 * p0 is already square-free and primitive.
 * ps[i] is at least primitive.
 *
 */

void nezgcdnp_sqfr_primitive(vl,p0,ps,m,pr)
VL vl;
int m;
P p0,*ps,*pr;
{
	/* variables */
	P p00,g,h,g0,h0,a0,b0;
	P lp0,lgp0,lp00,lg,lg0,cbd,tq,t;
	P *lc;
	P *tps;
	VL nvl1,nvl2,nvl,tvl;
	V v;
	int i,j,k,d0,dg,dg0,dmin;
	VN vn0,vn1,vnt;
	int nv,flag;

	/* set-up */
	if ( NUM(p0) ) {
		*pr = (P) ONE; return;
	}
	for ( v = VR(p0), i = 0; i < m; i++ )
		if ( NUM(ps[i]) || (v != VR(ps[i])) ) {
			*pr = (P)ONE; return;
		}
	tps = (P *) ALLOCA(m*sizeof(P));
	for ( i = 0; i < m; i++ )
		tps[i] = ps[i];
	sortplist(tps,m);	
	/* deg(tps[0]) <= deg(tps[1]) <= ... */
	
	if ( !cmpq(DEG(DC(p0)),ONE) ) {
		if ( divcheck(vl,tps,m,(P)ONE,p0) )
			*pr = p0;
		else
			*pr = (P)ONE;
		return;
	}

	lp0 = LC(p0); dmin = d0 = deg(v,p0); lc = (P *)ALLOCA((m+1)*sizeof(P));
	for ( lc[0] = lp0, i = 0; i < m; i++ )
		lc[i+1] = LC(tps[i]);
	clctv(vl,p0,&nvl);
	for ( i = 0; i < m; i++ ) {
		clctv(vl,tps[i],&nvl1); mergev(vl,nvl,nvl1,&nvl2); nvl = nvl2;
	}
	nezgcdnpz(nvl,lc,m+1,&lg);

	mulp(nvl,p0,lg,&lgp0); k = dbound(v,lgp0)+1; cbound(nvl,lgp0,(Q *)&cbd);
	for ( nv = 0, tvl = nvl; tvl; tvl = NEXT(tvl), nv++ );
	W_CALLOC(nv,struct oVN,vn0); W_CALLOC(nv,struct oVN,vnt);
	W_CALLOC(nv,struct oVN,vn1);
	for ( i = 0, tvl = NEXT(nvl); tvl; tvl = NEXT(tvl), i++ )
		vn1[i].v = vn0[i].v = tvl->v;

	/* main loop */
	for ( dg = deg(v,tps[0]) + 1; ; next(vn0) )
		do {
			for ( i = 0, j = 0; vn0[i].v; i++ ) 
				if ( vn0[i].n ) vnt[j++].v = (V)i;
			vnt[j].n = 0;

			/* find b s.t. LC(p0)(b), LC(tps[i])(b) != 0 */
			mulsgn(vn0,vnt,j,vn1); substvp(nvl,p0,vn1,&p00);
			flag = (!zerovpchk(nvl,lp0,vn1) && sqfrchk(p00));
			for ( i = 0; flag && (i < m); i++ )
				flag &= (!zerovpchk(nvl,LC(tps[i]),vn1));
			if ( !flag ) 
				continue;

			/* substitute y -> b */
			substvp(nvl,lg,vn1,&lg0); lp00 = LC(p00);
			/* extended-gcd in 1-variable */
			uexgcdnp(nvl,p00,tps,m,vn1,(Q)cbd,&g0,&h0,&a0,&b0,(Q *)&tq);
			if ( NUM(g0) ) {
				*pr = (P)ONE; return;
			} else if ( dg > ( dg0 = deg(v,g0) ) ) {
				dg = dg0;
				if ( dg == d0 ) {
					if ( divcheck(nvl,tps,m,lp0,p0) ) {
						*pr = p0; return;
					}
				} else if ( dg == deg(v,tps[0]) ) {
					if ( divtpz(nvl,p0,tps[0],&t) &&
						divcheck(nvl,tps+1,m-1,LC(tps[0]),tps[0]) ) {
						*pr = tps[0]; return;
					} else
						break;
				} else {
					henmv(nvl,vn1,lgp0,g0,h0,a0,b0,lg,lp0,lg0,lp00,(Q)tq,k,&g,&h);
					if ( divtpz(nvl,lgp0,g,&t) &&
						divcheck(nvl,tps,m,lg,g) ) {
						pcp(nvl,g,pr,&t); return;
					}
				}
			}
		} while ( !nextbin(vnt,j) );
}

void intersectv(vl1,vl2,vlp)
VL vl1,vl2,*vlp;
{
	int i,n;
	VL tvl;
	VN tvn;

	if ( !vl1 || !vl2 ) {
		*vlp = 0; return;
	}
	for ( n = 0, tvl = vl1; tvl; tvl = NEXT(tvl), n++ );
	tvn = (VN) ALLOCA(n*sizeof(struct oVN));
	for ( i = 0, tvl = vl1; i < n; tvl = NEXT(tvl), i++ ) {
		tvn[i].v = tvl->v; tvn[i].n = 0;
	}
	for ( tvl = vl2; tvl; tvl = NEXT(tvl) )
		for ( i = 0; i < n; i++ )
			if ( tvn[i].v == tvl->v ) {
				tvn[i].n = 1; break;
			}
	vntovl(tvn,n,vlp);
}

int pcoef(vl,ivl,p,cp)
VL vl,ivl;
P p;
P *cp;
{
	VL nvl,tvl,svl,mvl,mvl0;
	P t;

	if ( NUM(p) ) {
		cp[0] = p; return 1;
	} else {
		clctv(vl,p,&nvl);
		for ( tvl = nvl, mvl0 = 0; tvl; tvl = NEXT(tvl) ) {
			for ( svl = ivl; svl; svl = NEXT(svl) )
				if ( tvl->v == svl->v )
					break;
			if ( !svl ) {
				if ( !mvl0 ) {
					NEWVL(mvl0); mvl = mvl0;
				} else {
					NEWVL(NEXT(mvl)); mvl = NEXT(mvl);	
				}
				mvl->v = tvl->v;
			}
		}
		if ( mvl0 )
			NEXT(mvl) = ivl; 
		else
			mvl0 = ivl;
		reorderp(mvl0,nvl,p,&t);
		return pcoef0(mvl0,ivl,t,cp);
	}
}

int pcoef0(vl,ivl,p,cp)
VL vl,ivl;
P p;
P *cp;
{
	int cn,n;
	DCP dc;
	V v;
	VL tvl;

	if ( NUM(p) ) {
		cp[0] = p; return 1;
	} else {
		for ( v = VR(p), tvl = ivl; tvl; tvl = NEXT(tvl) )
			if ( v == tvl->v )
				break;
		if ( tvl ) {
			cp[0] = p; return 1;
		} else {
			for ( dc = DC(p), n = 0; dc; dc = NEXT(dc) ) {
				cn = pcoef0(vl,ivl,COEF(dc),cp); cp += cn; n += cn;
			}
			return n;
		}
	}
}

int lengthp(p)
P p;
{
	int n;
	DCP dc;

	if ( NUM(p) )
		return 1;
	else {
		for ( dc = DC(p), n = 0; dc; dc = NEXT(dc) )
			n += lengthp(COEF(dc));
		return n;
	}
}
