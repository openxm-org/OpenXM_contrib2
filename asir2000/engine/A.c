/* $OpenXM: OpenXM/src/asir99/engine/A.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"

void pdiva(vl,p0,p1,p2,pr)
VL vl;
P p1,p2,p0;
P *pr;
{
	Q m,d;
	P t,q,r;

	if ( VR(p2) == VR(p0) ) {
		*pr = p1;
		return;
	}
	if ( VR(p1) == VR(p0) ) {
		error("pdiva : ???");
		return;
	}
	subq(DEG(DC(p1)),DEG(DC(p2)),&m); addq(m,ONE,&d);	
	pwrq((Q)COEF(DC(p2)),d,&m); mulp(vl,p1,(P)m,&t);
	divsrp(vl,t,p2,&q,&r); remsdcp(vl,q,p0,pr);
}

void rema(vl,p0,p1,p2,pr)
VL vl;
P p1,p2,p0,*pr;
{
	P m,m1,m2,g,lc,x,t;
	Q q;
	V v;
	int d1,d2;

	if ( !p1 || !p2 || NUM(p2) || ( VR(p2) == VR(p0) ) ) 
		*pr = 0;
	else if ( NUM(p1) ) 
		*pr = p1;
	else if ( ( v = VR(p1) ) == VR(p0) ) 
		remsdcp(vl,p1,p0,pr);
	else if ( ( d1 = deg(v,p1) ) < ( d2 = deg(v,p2) ) ) 
		*pr = p1;
	else if ( d1 == d2 ) {
		mulp(vl,p1,LC(p2),&m); mulp(vl,p2,LC(p1),&m1);
		subp(vl,m,m1,&m2); 
		remsdcp(vl,m2,p0,pr); 
	} else {
		g = p1; lc = LC(p2); MKV(v,x);
		while ( ( d1 = deg(v,g) ) >= d2 ) {
			mulp(vl,g,lc,&m); mulp(vl,p2,LC(g),&m1);
			STOQ(d1-d2,q); pwrp(vl,x,q,&t);
			mulp(vl,m1,t,&m2); subp(vl,m,m2,&m1); 
			remsdcp(vl,m1,p0,&g); 
		}
		*pr = g;
	}
}

void gcda(vl,p0,p1,p2,pr)
VL vl;
P p1,p2,p0,*pr;
{
	P g1,g2,r,t,c;

	remsdcp(vl,p1,p0,&t); pcp(vl,t,&g1,&c); remsdcp(vl,p2,p0,&g2);
	while ( 1 ) {
		if ( NUM(g2) || (VR(g2) == VR(p0)) ) {
			*pr = (P)ONE;
			return;
		}
		pcp(vl,g2,&t,&c); pmonic(vl,p0,t,&g2); rema(vl,p0,g1,g2,&r); 
		if ( !r ) {
			pmonic(vl,p0,g2,pr); 
			return;
		}
		g1 = g2; g2 = r;
	}
}

void sqa(vl,p0,p,dcp)
VL vl;
P p,p0;
DCP *dcp;
{
	V v;
	int i,j,n;
	P f,f1,f2,gcd,flat,q,r,t;
	P *a;
	int *b;
	DCP dc,dc0;

	if ( ( v = VR(p) ) == VR(p0) ) {
		NEWDC(dc); *dcp = dc; COEF(dc) = (P)ONE; DEG(dc) = ONE;
		NEXT(dc) = 0;
	} else {
		n = deg(v,p); W_CALLOC(n,P,a); W_CALLOC(n,int,b);
		for ( i = 0, f = p; ;i++ ) {
			if ( VR(f) != v ) 
				break;
			remsdcp(vl,f,p0,&f1); diffp(vl,f1,VR(f1),&f2);
			while ( 1 ) {
				pmonic(vl,p0,f2,&t); f2 = t; rema(vl,p0,f1,f2,&r); 
				if ( !r ) {
					pmonic(vl,p0,f2,&gcd); pqra(vl,p0,f,gcd,&flat,&r); 
					break;
				}
				f1 = f2; f2 = r;
			}
			if ( VR(gcd) != v ) {
				a[i] = f; b[i] = 1; 
				break;
			}
			for ( j = 1, f = gcd; ; j++ ) {
				pqra(vl,p0,f,flat,&q,&r);
				if ( r )
					break;
				else
					f = q;
			}
			a[i] = flat; b[i] = j;
		}
		for ( i = 0, j = 0, dc0 = 0; a[i]; i++ ) {
			NEXTDC(dc0,dc); j += b[i]; STOQ(j,DEG(dc));
			if ( a[i+1] ) 
				pqra(vl,p0,a[i],a[i+1],&COEF(dc),&t);
			else
				COEF(dc) = a[i];
		}
		NEXT(dc) = 0; *dcp = dc0;
	}
}

void pqra(vl, p0, p1, p2, p, pr)
VL vl;
P p0,p1,p2;
P *p,*pr;
{
	V v,v1;
	P  m,m1,q,r;
	Q tq;
	int n;

	if ( ( v = VR(p1) ) != ( v1 = VR(p2) ) ) {
		while ( ( VR(vl) != v ) && ( VR(vl) != v1 ) ) 
			vl = NEXT(vl);
		if ( VR(vl) == v ) {
			*p = p1; *pr = 0;
		} else {
			*p = 0; *pr = p1;
		}
	} else if ( (n = deg(v,p1) - deg(v,p2)) < 0 ) {
		*pr = p1; *p = 0;
	} else {			
		n++; STOQ(n,tq);
		pwrp(vl,LC(p2),tq,&m); mulp(vl,m,p1,&m1); divsrp(vl,m1,p2,&q,&r); 
		remsdcp(vl,q,p0,p); remsdcp(vl,r,p0,pr); 
	}
}

void pmonic(vl, p0, p, pr)
VL vl;
P p,p0,*pr;
{
	P d,m,pp,c,t;

	if ( NUM(p) || ( VR(p) == VR(p0) ) )
		*pr = (P)ONE;
	else if ( NUM(COEF(DC(p))) )
		*pr = p;
	else {
		ptozp(COEF(DC(p)),1,(Q *)&c,&pp); pinva(p0,pp,&d); 
		mulp(vl,p,d,&m); remsdcp(vl,m,p0,&t); ptozp(t,1,(Q *)&c,pr); 
	}
}

void remsdcp(vl, p, p0, pr)
VL vl;
P p,p0,*pr;
{
	P q,r,c;
	DCP dc,dcr,dcr0;

	if ( !p ) 
		*pr = 0;
	else if ( NUM(p) ) 
		*pr = (P)ONE;
	else if ( VR(p) == VR(p0) ) {
		divsrp(vl,p,p0,&q,&r); ptozp(r,1,(Q *)&c,pr); 
	} else {
		for ( dcr = dcr0 = 0, dc = DC(p); dc; dc = NEXT(dc) ) {
			divsrp(vl,COEF(dc),p0,&q,&r); 
			if ( r ) {
				NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = r;
			}
		}
		if ( !dcr0 ) 
			*pr = 0;
		else {
			NEXT(dcr) = 0; MKP(VR(p),dcr0,r); ptozp(r,1,(Q *)&c,pr); 
		}
	}
}

void pinva(p0, p, pr)
P p,p0,*pr;
{
	V v;
	Q f,q,q1;
	Q u,t,a,b,s;
	P c,c1;
	P tg,th,tr;
	P z,zz,zzz;
	UM wg,wh,ws,wt,wm;
	int n,m,modres,mod,index,i;

	v = VR(p); n=deg(v,p); m=deg(v,p0);
	norm(p,&a); norm(p0,&b);
	STOQ(m,u); pwrq(a,u,&t); STOQ(n,u); pwrq(b,u,&s); 
	mulq(t,s,&u); 
	factorial(n+m,&t); mulq(u,t,&s); addq(s,s,&f); 
	wg = W_UMALLOC(m+n); wh = W_UMALLOC(m+n);
	wt = W_UMALLOC(m+n); ws = W_UMALLOC(m+n);
	wm = W_UMALLOC(m+n); 
	for ( q = ONE, t = 0, c = 0, index = 0; ; ) {
		mod = lprime[index++];
		if ( !mod )
			error("pinva : lprime[] exhausted.");
		if ( !rem(NM((Q)LC(p)),mod) || !rem(NM((Q)LC(p0)),mod) ) 
			continue;
		ptomp(mod,p,&tg); ptomp(mod,p0,&th); srchump(mod,tg,th,&tr);
		if ( !tr )
			continue;
		else
			modres = CONT((MQ)tr);
		mptoum(tg,wg); mptoum(th,wh); 
		eucum(mod,wg,wh,ws,wt); /* wg * ws + wh * wt = 1 */
		DEG(wm) = 0; COEF(wm)[0] = modres;
		mulum(mod,ws,wm,wt);
		for ( i = DEG(wt); i >= 0; i-- ) 
			if ( ( COEF(wt)[i] * 2 ) > mod ) 
				COEF(wt)[i] -= mod;
		chnrem(mod,v,c,q,wt,&c1,&q1); 
		if ( !ucmpp(c,c1) ) {
			mulp(CO,c,p,&z); divsrp(CO,z,p0,&zz,&zzz);
			if ( NUM(zzz) ) {
				q = q1; c = c1; break;
			}
		}
		q = q1; c = c1;
		if ( cmpq(f,q) < 0 ) 
			break;
	}
	ptozp(c,1,&s,pr); 
}
