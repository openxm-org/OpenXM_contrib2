/* $OpenXM: OpenXM/src/asir99/engine/PUM.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"

void gcdcmp(), sprsm();

void detmp(vl,mod,rmat,n,dp)
VL vl;
int mod;
P **rmat;
int n;
P *dp;
{
	int i,j,k,sgn;
	P mjj,mij,t,s,u,d;
	P **mat;
	P *mi,*mj;

	mat = (P **)almat_pointer(n,n);
	for ( i = 0; i < n; i++ )
		for ( j = 0; j < n; j++ )
			mat[i][j] = rmat[i][j];
	for ( j = 0, d = (P)ONEM, sgn = 1; j < n; j++ ) {
		for ( i = j; (i < n) && !mat[i][j]; i++ );
		if ( i == n ) {
			*dp = 0; return;
		}
		for ( k = i; k < n; k++ )
			if ( mat[k][j] && (nmonop(mat[k][j]) < nmonop(mat[i][j]) ) )
				i = k;
		if ( j != i ) {
			mj = mat[j]; mat[j] = mat[i]; mat[i] = mj; sgn = -sgn;
		}
		for ( i = j + 1, mj = mat[j], mjj = mj[j]; i < n; i++ )
			for ( k = j + 1, mi = mat[i], mij = mi[j]; k < n; k++ ) {
				mulmp(vl,mod,mi[k],mjj,&t); mulmp(vl,mod,mj[k],mij,&s);
				submp(vl,mod,t,s,&u); divsmp(vl,mod,u,d,&mi[k]);
			}
		d = mjj;
	}
	if ( sgn < 0 )
		chsgnmp(mod,d,dp);
	else
		*dp = d;
}

void resultmp(vl,mod,v,p1,p2,pr)
VL vl;
int mod;
V v;
P p1,p2,*pr;
{
	P q1,q2,m,m1,m2,lc,q,r,t,g1,g2,adj;
	int d,d1,d2,j,k;
	VL nvl;
	Q dq;
	MQ mq;

	if ( !p1 || !p2 ) {
		*pr = 0; return;
	}
	reordvar(vl,v,&nvl);
	reordermp(nvl,mod,vl,p1,&q1); reordermp(nvl,mod,vl,p2,&q2);

	if ( VR(q1) != v )
		if ( VR(q2) != v ) {
			*pr = 0;
			return;
		} else {
			d = deg(v,q2); STOQ(d,dq);
			pwrmp(vl,mod,q1,dq,pr); 
			return;
		}
	else if ( VR(q2) != v ) {
		d = deg(v,q1); STOQ(d,dq);
		pwrmp(vl,mod,q2,dq,pr); 
		return;
	} 
		
	if ( ( VR(q1) != v ) || ( VR(q2) != v ) ) {
		*pr = 0;
		return;
	} 

	d1 = deg(v,q1); d2 = deg(v,q2);
	if ( d1 > d2 ) {
		g1 = q1; g2 = q2; adj = (P)ONEM;
	} else if ( d1 < d2 ) {
		g2 = q1; g1 = q2;
		if ( (d1 % 2) && (d2 % 2) ) {
			STOMQ(-1,mq); adj = (P)mq;
		} else
			adj = (P)ONEM;
	} else {
		premmp(nvl,mod,q1,q2,&t);
		d = deg(v,t); STOQ(d,dq); pwrmp(nvl,mod,LC(q2),dq,&adj); 
		g1 = q2; g2 = t;
		if ( d1 % 2 ) {
			chsgnmp(mod,adj,&t); adj = t;
		}
	}
	d1 = deg(v,g1); j = d1 - 1;

	for ( lc = (P)ONEM; ; ) {
		if ( ( k = deg(v,g2) ) < 0 ) {
			*pr = 0;
			return;
		}

		if ( k == j )
			if ( !k ) {
				divsmp(nvl,mod,g2,adj,pr); 
				return;
			} else {
				premmp(nvl,mod,g1,g2,&r); mulmp(nvl,mod,lc,lc,&m);
				divsmp(nvl,mod,r,m,&q); 
				g1 = g2; g2 = q;
				lc = LC(g1); /* g1 is not const */
				j = k - 1;
			}
		else {
			d = j - k; STOQ(d,dq);
			pwrmp(nvl,mod,(VR(g2)==v?LC(g2):g2),dq,&m);
			mulmp(nvl,mod,g2,m,&m1); 
			pwrmp(nvl,mod,lc,dq,&m); divsmp(nvl,mod,m1,m,&t); 
			if ( k == 0 ) {
				divsmp(nvl,mod,t,adj,pr); 
				return;
			} else {
				premmp(nvl,mod,g1,g2,&r);
				mulmp(nvl,mod,lc,lc,&m1); mulmp(nvl,mod,m,m1,&m2); 
				divsmp(nvl,mod,r,m2,&q);
				g1 = t; g2 = q;
				if ( d % 2 ) {
					chsgnmp(mod,g2,&t); g2 = t;
				}
				lc = LC(g1); /* g1 is not const */
				j = k - 1;
			}
		}
	}
}

void premmp(vl,mod,p1,p2,pr)
VL vl;
int mod;
P p1,p2,*pr;
{
	P m,m1,m2;
	P *pw;
	DCP dc;
	V v1,v2;
	register int i,j;
	int n1,n2,d;

	if ( NUM(p1) ) 
		if ( NUM(p2) ) 
			*pr = 0;
		else 
			*pr = p1;
	else if ( NUM(p2) ) 
		*pr = 0;
	else if ( ( v1 = VR(p1) ) == ( v2 = VR(p2) ) ) {
		n1 = deg(v1,p1); n2 = deg(v1,p2);
		pw = (P *)ALLOCA((n1+1)*sizeof(P));
		bzero((char *)pw,(int)((n1+1)*sizeof(P)));

		for ( dc = DC(p1); dc; dc = NEXT(dc) )
			pw[QTOS(DEG(dc))] = COEF(dc);

		for ( i = n1; i >= n2; i-- ) {
			if ( pw[i] ) {
				m = pw[i];
				for ( j = i; j >= 0; j-- ) {
					mulmp(vl,mod,pw[j],LC(p2),&m1); pw[j] = m1;
				}

				for ( dc = DC(p2), d = i - n2; dc; dc = NEXT(dc) ) {
					mulmp(vl,mod,COEF(dc),m,&m1);
					submp(vl,mod,pw[QTOS(DEG(dc))+d],m1,&m2); 
					pw[QTOS(DEG(dc))+d] = m2;
				}
			} else 
				for ( j = i; j >= 0; j-- ) 
					if ( pw[j] ) {
						mulmp(vl,mod,pw[j],LC(p2),&m1); pw[j] = m1;
					} 
		}
		plisttop(pw,v1,n2-1,pr);
	} else {
		while ( v1 != vl->v && v2 != vl->v ) 
			vl = NEXT(vl);
		if ( v1 == vl->v ) 
			*pr = 0;
		else 
			*pr = p1;
	}
}

void srchmp(vl,mod,v,p1,p2,pr)
VL vl;
int mod;
V v;
P p1,p2,*pr;
{
	P a,b,q1,q2,x,t,s,d,bg,c,c0,db;
	int i,m,k;
	V v0;
	VL nvl,tvl,nvl1,nvl2;
	Q dq;
	MQ q;

	if ( vl->v != v ) {
		reordvar(vl,v,&tvl);
		reordermp(tvl,mod,vl,p1,&q1); reordermp(tvl,mod,vl,p2,&q2);
	} else {
		q1 = p1; q2 = p2; tvl = vl;
	}
	clctv(tvl,q1,&nvl1); clctv(tvl,q2,&nvl2); mergev(tvl,nvl1,nvl2,&nvl); 
	if ( VR(q1) != v )
		if ( VR(q2) != v )
			*pr = 0;
		else {
			m = getdeg(v,q2); STOQ(m,dq); pwrmp(vl,mod,q1,dq,pr);
		}
	else if ( VR(q2) != v ) {
		m = getdeg(v,q1); STOQ(m,dq); pwrmp(vl,mod,q2,dq,pr);
	} else if ( !NEXT(nvl) )
		srchump(mod,p1,p2,pr);
	else {
		v0 = NEXT(nvl)->v;
		k = getdeg(v,q1)*getdeg(v0,q2)+getdeg(v,q2)*getdeg(v0,q1)+1; 
		for ( i = 0, c = 0, d = (P)ONEM, MKMV(v0,x);
			( i < mod ) && (getdeg(v0,d) < k) ; i++ ) {
			STOMQ(i,q),bg = (P)q; substmp(nvl,mod,LC(q1),v0,bg,&t);
			if ( !t ) 
				continue;
			substmp(nvl,mod,LC(q2),v0,bg,&t);
			if ( !t ) 
				continue;
			substmp(nvl,mod,q1,v0,bg,&a); substmp(nvl,mod,q2,v0,bg,&b);
			srchmp(nvl,mod,v,a,b,&c0); substmp(nvl,mod,c,v0,bg,&t); 
			submp(nvl,mod,c0,t,&s); mulmp(nvl,mod,s,d,&t); 
			substmp(nvl,mod,d,v0,bg,&db); 
			divsmp(nvl,mod,t,db,&s); addmp(nvl,mod,s,c,&t); c = t;
			submp(nvl,mod,x,bg,&t); mulmp(nvl,mod,d,t,&s); d = s;
		}
		if ( i == mod ) 
			error("srchmp : ???");
		*pr = c; 
	}
}

int ucmpp(p,q)
P p,q;
{
	DCP dcp,dcq;

	if ( !p )
		if ( !q ) 
			return ( 0 );
		else 
			return ( 1 );
	else if ( !q )
		return ( 1 );
	else if ( NUM(p) )
		if ( !NUM(q) )
			return ( 1 );
		else 
			return ( cmpq((Q)p,(Q)q) );
	else if ( NUM(q) )
			return ( 1 );
	else {
		for ( dcp = DC(p), dcq = DC(q); dcp && dcq;
			dcp = NEXT(dcp), dcq = NEXT(dcq) )
			if ( cmpq(DEG(dcp),DEG(dcq) ) )
				return ( 1 );
			else if ( cmpq((Q)COEF(dcp),(Q)COEF(dcq) ) )
				return ( 1 );
		if ( dcp || dcq ) 
			return ( 1 );
		else 
			return ( 0 );
	}
}

#if 0
srchump(mod,p1,p2,pr)
int mod;
P p1,p2,*pr;
{
	int r;
	MQ mq;

	r = eucresum(mod,p1,p2);
	STOMQ(r,mq); *pr = (P)mq;
}
#endif

void srchump(mod,p1,p2,pr)
int mod;
P p1,p2,*pr;
{
	UM m,m1,q,r,t,g1,g2;
	int lc,d,d1,d2,i,j,k,l,l1,l2,tmp,adj;
	V v;

	v = VR(p1); d = MAX(UDEG(p1),UDEG(p2));
	g1 = W_UMALLOC(d); g2 = W_UMALLOC(d);
	bzero((char *)g1,(int)((d+2)*sizeof(int))); bzero((char *)g2,(int)((d+2)*sizeof(int)));
	if ( d == (int)UDEG(p1) ) {
		mptoum(p1,g1); mptoum(p2,g2);
	} else {
		mptoum(p2,g1); mptoum(p1,g2);
	}
	if ( ( d1 = DEG(g1) ) > ( d2 = DEG(g2) ) ) {
		j = d1 - 1; adj = 1;
	} else 
		j = d2;
	lc = 1;
	r = W_UMALLOC(d1+d2); q = W_UMALLOC(d1+d2); 
	m1 = W_UMALLOC(d1+d2); t = W_UMALLOC(d1+d2);
	bzero((char *)r,(int)((d1+d2+2)*sizeof(int))); bzero((char *)q,(int)((d1+d2+2)*sizeof(int))); 
	bzero((char *)m1,(int)((d1+d2+2)*sizeof(int))); bzero((char *)t,(int)((d1+d2+2)*sizeof(int))); 
	m = W_UMALLOC(0); bzero((char *)m,(int)(2*sizeof(int)));
	adj = pwrm(mod,COEF(g2)[DEG(g2)],DEG(g1));
	DEG(m) = 0; COEF(m)[0] = invm(COEF(g2)[DEG(g2)],mod);
	mulum(mod,g2,m,r); cpyum(r,g2);
	while ( 1 ) {
		if ( ( k = DEG(g2) ) < 0 ) {
			*pr = 0;
			return;
		}
		if ( k == j ) {
			if ( k == 0 ) {
				DEG(m) = 0; COEF(m)[0] = adj;
				mulum(mod,g2,m,r); umtomp(v,r,pr);
				return;
			} else {
				DEG(m) = 0; 
				COEF(m)[0] = pwrm(mod,COEF(g2)[k],DEG(g1)-k+1);
				mulum(mod,g1,m,r); DEG(r) = divum(mod,r,g2,t);
				DEG(m) = 0; COEF(m)[0] = dmb(mod,lc,lc,&tmp);
				divum(mod,r,m,q); cpyum(g2,g1); cpyum(q,g2);
				lc = COEF(g1)[DEG(g1)]; j = k - 1;
			}
		} else {
			d = j - k;
			DEG(m) = 0; COEF(m)[0] = pwrm(mod,COEF(g2)[DEG(g2)],d);
			mulum(mod,g2,m,m1); l = pwrm(mod,lc,d);
			DEG(m) = 0; COEF(m)[0] = l; divum(mod,m1,m,t);
			if ( k == 0 ) {
				DEG(m) = 0; COEF(m)[0] = adj;
				mulum(mod,t,m,r); umtomp(v,r,pr);
				return;
			} else {
				DEG(m) = 0; 
				COEF(m)[0] = pwrm(mod,COEF(g2)[k],DEG(g1)-k+1);
				mulum(mod,g1,m,r); DEG(r) = divum(mod,r,g2,q);
				l1 = dmb(mod,lc,lc,&tmp); l2 = dmb(mod,l,l1,&tmp);
				DEG(m) = 0; COEF(m)[0] = l2;
				divum(mod,r,m,q); cpyum(t,g1); cpyum(q,g2);
				if ( d % 2 ) 
					for ( i = DEG(g2); i >= 0; i-- ) 
						COEF(g2)[i] = ( mod - COEF(g2)[i] ) % mod;
				lc = COEF(g1)[DEG(g1)]; j = k - 1;
			}
		}
	}
}

void substmp(vl,mod,p,v0,p0,pr)
VL vl;
int mod;
V v0;
P p,p0;
P *pr;
{
	P x,t,m,c,s,a;
	DCP dc;
	Q d;

	if ( !p ) 
		*pr = 0;
	else if ( NUM(p) ) 
		*pr = p;
	else if ( VR(p) != v0 ) {
		MKMV(VR(p),x);
		for ( c = 0, dc = DC(p); dc; dc = NEXT(dc) ) {
			substmp(vl,mod,COEF(dc),v0,p0,&t);
			if ( DEG(dc) ) {
				pwrmp(vl,mod,x,DEG(dc),&s); mulmp(vl,mod,s,t,&m); 
				addmp(vl,mod,m,c,&a); 
				c = a;
			} else {
				addmp(vl,mod,t,c,&a); 
				c = a;
			}
		}
		*pr = c;
	} else {
		dc = DC(p);
		c = COEF(dc);
		for ( d = DEG(dc), dc = NEXT(dc); 
			dc; d = DEG(dc), dc = NEXT(dc) ) {
				subq(d,DEG(dc),(Q *)&t); pwrmp(vl,mod,p0,(Q)t,&s); 
				mulmp(vl,mod,s,c,&m); 
				addmp(vl,mod,m,COEF(dc),&c); 
		}
		if ( d ) {
			pwrmp(vl,mod,p0,d,&t); mulmp(vl,mod,t,c,&m); 
			c = m;
		}
		*pr = c;
	}
}

void reordermp(nvl,mod,ovl,p,pr)
VL nvl,ovl;
int mod;
P p;
P *pr;
{
	DCP dc;
	P x,m,s,t,c;
	
	if ( !p ) 
		*pr = 0;
	else if ( NUM(p) ) 
		*pr = p;
	else {
		MKMV(VR(p),x);
		for ( s = 0, dc = DC(p); dc; dc = NEXT(dc) ) {
			reordermp(nvl,mod,ovl,COEF(dc),&c);
			if ( DEG(dc) ) {
				pwrmp(nvl,mod,x,DEG(dc),&t); mulmp(nvl,mod,c,t,&m); 
				addmp(nvl,mod,m,s,&t); 
			} else 
				addmp(nvl,mod,s,c,&t);
			s = t;
		}
		*pr = s;
	}
}
			
void chnremp(vl,mod,p,q,c,r)
VL vl;
int mod;
P p;
Q q;
P c;
P *r;
{
	P tg,sg,ug;
	P t,u;
	MQ mq;

	ptomp(mod,p,&tg); submp(vl,mod,c,tg,&sg);
	UTOMQ(rem(NM(q),mod),mq),tg = (P)mq; divsmp(vl,mod,sg,tg,&ug);
	normalizemp(mod,ug);
	mptop(ug,&u); mulp(vl,u,(P)q,&t); addp(vl,t,p,r);
}

/* XXX  strange behavior of invm() on SPARC */

void chnrem(mod,v,c,q,t,cr,qr)
int mod;
V v;
UM t;
Q q,*qr;
P c,*cr;
{
	int n,m,i,d,a,sd,tmp;
	Q b,s,z;
	Q *pc,*pcr;
	DCP dc;

	if ( !c || NUM(c) )
		n = 0;
	else 
		n = UDEG(c); 
	m = DEG(t); d = MAX(n,m); W_CALLOC(n,Q,pc); W_CALLOC(d,Q,pcr);
	if ( !c ) 
		pc[0] = 0;
	else if ( NUM(c) )
		pc[0] = (Q)c;
	else
		for ( dc = DC(c); dc; dc = NEXT(dc) )
			pc[QTOS(DEG(dc))] = (Q)COEF(dc);
	for ( i = 0; i <= d; i++ ) {
		b = (i>n?0:pc[i]); a = (i>m?0:COEF(t)[i]);
		if ( b ) 
			a = (a-SGN(pc[i])*((int)rem(NM(pc[i]),mod)))%mod;
		sd = dmb(mod,(a>=0?a:a+mod),invm(rem(NM(q),mod),mod),&tmp);
		if ( ( 2 * sd ) > mod ) 
			sd -= mod;
		STOQ(sd,z); mulq(z,q,&s); addq(s,b,&pcr[i]); 
	}
	STOQ(mod,z); mulq(q,z,qr); plisttop((P *)pcr,v,d,cr);
}

void normalizemp(mod,g)
int mod;
P g;
{
	DCP dc;

	if ( !g ) 
		return;
	else if ( NUM(g) ) {
		if ( 2 * CONT((MQ)g) > mod ) 
			CONT((MQ)g) -= mod;
		return;
	} else
		for ( dc = DC(g); dc; dc = NEXT(dc) )
			normalizemp(mod,COEF(dc));
}

void norm(p,r)
P p;
Q *r;
{
	N t;
	DCP dc;

	for ( dc = DC(p), t = ONEN; dc; dc = NEXT(dc) )
		if ( cmpn(NM((Q)COEF(dc)),t) > 0 ) {
			t = NM((Q)COEF(dc));
		}
	NTOQ(t,1,*r);
}

void norm1(p,r)
P p,*r;
{
	DCP dc;
	P t,s,u;
	Q q;

	if ( NUM(p) )
		NTOQ(NM((Q)p),1,q),*r = (P)q;
	else {
		for ( t = 0, dc = DC(p); dc; dc = NEXT(dc) ) {
			norm1(COEF(dc),&s); addq((Q)t,(Q)s,(Q *)&u); t = u;
		}
		*r = t;
	}
}

void norm1c(p,r)
P p;
Q *r;
{
	N t;
	Q s;
	DCP dc;

	if ( NUM(p) ) 
		norm1(p,(P *)r);
	else {
		for ( dc = DC(p), t = ONEN; dc; dc = NEXT(dc) ) {
			norm1(COEF(dc),(P *)&s); 
			if ( cmpn(NM(s),t) > 0 )
				t = NM(s);
		}
		NTOQ(t,1,*r);
	}
}

void gcdprsmp(vl,mod,p1,p2,pr)
VL vl;
int mod;
P p1,p2,*pr;
{
	P g1,g2,gc1,gc2,gp1,gp2,g,gc,gp,gcr;
	V v1,v2;

	if ( !p1 ) 
		*pr = p2;
	else if ( !p2 ) 
		*pr = p1;
	else if ( NUM(p1) || NUM(p2) ) 
		*pr = (P)ONEM;
	else {
		g1 = p1; g2 = p2;
		if ( ( v1 = VR(g1) ) == ( v2 = VR(g2) ) ) {
			gcdcmp(vl,mod,g1,&gc1); divsmp(vl,mod,g1,gc1,&gp1); 
			gcdcmp(vl,mod,g2,&gc2); divsmp(vl,mod,g2,gc2,&gp2); 
			gcdprsmp(vl,mod,gc1,gc2,&gcr); 
			sprsm(vl,mod,v1,gp1,gp2,&g); 

			if ( VR(g) == v1 ) {
				gp = g;
				gcdcmp(vl,mod,gp,&gc); divsmp(vl,mod,gp,gc,&gp1);
				mulmp(vl,mod,gp1,gcr,pr); 
			} else
				*pr = gcr;
		} else {
			while ( v1 != vl->v && v2 != vl->v ) 
				vl = NEXT(vl);
			if ( v1 == vl->v ) {
				gcdcmp(vl,mod,g1,&gc1); gcdprsmp(vl,mod,gc1,g2,pr);
			} else {
				gcdcmp(vl,mod,g2,&gc2); gcdprsmp(vl,mod,gc2,g1,pr);
			}
		}
	}
}

void gcdcmp(vl,mod,p,pr)
VL vl;
int mod;
P p,*pr;
{
	P g,g1;
	DCP dc;

	if ( NUM(p) ) 
		*pr = (P)ONEM;
	else {
		dc = DC(p);
		g = COEF(dc);
		for ( dc = NEXT(dc); dc; dc = NEXT(dc) ) {
			gcdprsmp(vl,mod,g,COEF(dc),&g1); 
			g = g1;
		}
		*pr = g;
	}
}

void sprsm(vl,mod,v,p1,p2,pr)
VL vl;
int mod;
V v;
P p1,p2,*pr;
{
	P q1,q2,m,m1,m2,x,h,r,g1,g2;
	int d;
	Q dq;
	VL nvl;

	reordvar(vl,v,&nvl);
	reordermp(nvl,mod,vl,p1,&q1); reordermp(nvl,mod,vl,p2,&q2);

	if ( ( VR(q1) != v ) || ( VR(q2) != v ) ) {
		*pr = 0;
		return;
	}

	if ( deg(v,q1) >= deg(v,q2) ) {
		g1 = q1; g2 = q2;
	} else {
		g2 = q1; g1 = q2;
	}
	
	for ( h = (P)ONEM, x = (P)ONEM; ; ) {
		if ( !deg(v,g2) ) 
			break;

		premmp(nvl,mod,g1,g2,&r);
		if ( !r ) 
			break;

		d = deg(v,g1) - deg(v,g2); STOQ(d,dq);
		pwrmp(nvl,mod,h,dq,&m); mulmp(nvl,mod,m,x,&m1); g1 = g2;
		divsmp(nvl,mod,r,m1,&g2); x = LC(g1); /* g1 is not const w.r.t v */
		pwrmp(nvl,mod,x,dq,&m1); mulmp(nvl,mod,m1,h,&m2); 
		divsmp(nvl,mod,m2,m,&h); 
	}
	*pr = g2;
}
