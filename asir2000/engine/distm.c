/* $OpenXM: OpenXM/src/asir99/engine/distm.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"
#include "inline.h"

#define NV(p) ((p)->nv)
#define C(p) ((p)->c)
#if 0
#define ITOS(p) (((unsigned int)(p))>>1)
#define STOI(i) ((P)((((unsigned int)(i))<<1)|1))
#else
#define ITOS(p) (((unsigned int)(p))&0x7fffffff)
#define STOI(i) ((P)((unsigned int)(i)|0x80000000))
#endif

extern int (*cmpdl)();

void ptomd(vl,mod,dvl,p,pr)
VL vl,dvl;
int mod;
P p;
DP *pr;
{
	P t;

	ptomp(mod,p,&t);
	mptomd(vl,mod,dvl,t,pr);
}

void mptomd(vl,mod,dvl,p,pr)
VL vl,dvl;
int mod;
P p;
DP *pr;
{
	int n,i;
	VL tvl;
	V v;
	DL d;
	MP m;
	DCP dc;
	DP r,s,t,u;
	P x,c;

	if ( !p )
		*pr = 0;
	else {
		for ( n = 0, tvl = dvl; tvl; tvl = NEXT(tvl), n++ );
		if ( NUM(p) ) {
			NEWDL(d,n);
			NEWMP(m); m->dl = d; C(m) = p; NEXT(m) = 0; MKDP(n,m,*pr);
		} else {
			for ( i = 0, tvl = dvl, v = VR(p);
				tvl && tvl->v != v; tvl = NEXT(tvl), i++ );
			if ( !tvl ) {
				for ( dc = DC(p), s = 0, MKV(v,x); dc; dc = NEXT(dc) ) {
					mptomd(vl,mod,dvl,COEF(dc),&t); pwrmp(vl,mod,x,DEG(dc),&c);
					mulmdc(vl,mod,t,c,&r); addmd(vl,mod,r,s,&t); s = t;
				}
				*pr = s;
			} else {
				for ( dc = DC(p), s = 0; dc; dc = NEXT(dc) ) {
					mptomd(vl,mod,dvl,COEF(dc),&t);
					NEWDL(d,n); d->td = QTOS(DEG(dc)); d->d[i] = d->td;
					NEWMP(m); m->dl = d; C(m) = (P)ONEM; NEXT(m) = 0; MKDP(n,m,u);
					mulmd(vl,mod,t,u,&r); addmd(vl,mod,r,s,&t); s = t;
				}
				*pr = s;
			}
		}
	}
}

void mdtop(vl,mod,dvl,p,pr)
VL vl,dvl;
int mod;
DP p;
P *pr;
{
	int n,i;
	DL d;
	MP m;
	P r,s,t,u,w;
	Q q;
	VL tvl;

	if ( !p )
		*pr = 0;
	else {
		for ( n = p->nv, m = BDY(p), s = 0; m; m = NEXT(m) ) {
			for ( i = 0, t = C(m), d = m->dl, tvl = dvl;
				i < n; tvl = NEXT(tvl), i++ ) {
				MKMV(tvl->v,r); STOQ(d->d[i],q); pwrmp(vl,mod,r,q,&u);
				mulmp(vl,mod,t,u,&w); t = w;
			}
			addmp(vl,mod,s,t,&u); s = u;
		}
		mptop(s,pr);
	}
}

void addmd(vl,mod,p1,p2,pr)
VL vl;
int mod;
DP p1,p2,*pr;
{
	int n;
	MP m1,m2,mr,mr0;
	P t;
	int tmp;
	MQ q;

	if ( !p1 )
		*pr = p2;
	else if ( !p2 )
		*pr = p1;
	else {
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; )
			switch ( (*cmpdl)(n,m1->dl,m2->dl) ) {
				case 0:
					if ( NUM(C(m1)) && NUM(C(m2)) ) {
						tmp = (CONT((MQ)C(m1))+CONT((MQ)C(m2))) - mod;
						if ( tmp < 0 )
							tmp += mod;
						if ( tmp ) {
							STOMQ(tmp,q); t = (P)q;
						} else
							t = 0;
					} else
						addmp(vl,mod,C(m1),C(m2),&t);
					if ( t ) {
						NEXTMP(mr0,mr); mr->dl = m1->dl; C(mr) = t;
					}
					m1 = NEXT(m1); m2 = NEXT(m2); break;
				case 1:
					NEXTMP(mr0,mr); mr->dl = m1->dl; C(mr) = C(m1);
					m1 = NEXT(m1); break;
				case -1:
					NEXTMP(mr0,mr); mr->dl = m2->dl; C(mr) = C(m2);
					m2 = NEXT(m2); break;
			}
		if ( !mr0 )
			if ( m1 )
				mr0 = m1;
			else if ( m2 )
				mr0 = m2;
			else {
				*pr = 0;
				return;
			}
		else if ( m1 )
			NEXT(mr) = m1;
		else if ( m2 )
			NEXT(mr) = m2;
		else
			NEXT(mr) = 0;
		MKDP(NV(p1),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = MAX(p1->sugar,p2->sugar);
	}
}

void submd(vl,mod,p1,p2,pr)
VL vl;
int mod;
DP p1,p2,*pr;
{
	DP t;

	if ( !p2 )
		*pr = p1;
	else {
		chsgnmd(mod,p2,&t); addmd(vl,mod,p1,t,pr);
	}
}

void chsgnmd(mod,p,pr)
int mod;
DP p,*pr;
{
	MP m,mr,mr0;

	if ( !p )
		*pr = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTMP(mr0,mr); chsgnmp(mod,C(m),&C(mr)); mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar;
	}
}

void mulmd(vl,mod,p1,p2,pr)
VL vl;
int mod;
DP p1,p2,*pr;
{
	MP m;
	DP s,t,u;

	if ( !p1 || !p2 )
		*pr = 0;
	else if ( OID(p1) <= O_P )
		mulmdc(vl,mod,p2,(P)p1,pr);
	else if ( OID(p2) <= O_P )
		mulmdc(vl,mod,p1,(P)p2,pr);
	else {
		for ( m = BDY(p2), s = 0; m; m = NEXT(m) ) {
			mulmdm(vl,mod,p1,m,&t); addmd(vl,mod,s,t,&u); s = u;
		}
		*pr = s;
	}
}

void mulmdm(vl,mod,p,m0,pr)
VL vl;
int mod;
DP p;
MP m0;
DP *pr;
{
	MP m,mr,mr0;
	P c;
	DL d;
	int n,t;
	MQ q;

	if ( !p )
		*pr = 0;
	else {
		for ( mr0 = 0, m = BDY(p), c = C(m0), d = m0->dl, n = NV(p); 
			m; m = NEXT(m) ) {
			NEXTMP(mr0,mr);
			if ( NUM(C(m)) && NUM(c) ) {
				t = dmar(((MQ)(C(m)))->cont,((MQ)c)->cont,0,mod);
				STOMQ(t,q); C(mr) = (P)q;
			} else
				mulmp(vl,mod,C(m),c,&C(mr));
			adddl(n,m->dl,d,&mr->dl);
		}
		NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar + m0->dl->td;
	}
}

void mulmdc(vl,mod,p,c,pr)
VL vl;
int mod;
DP p;
P c;
DP *pr;
{
	MP m,mr,mr0;
	int t;
	MQ q;

	if ( !p || !c )
		*pr = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTMP(mr0,mr);
			if ( NUM(C(m)) && NUM(c) ) {
				t = dmar(((MQ)(C(m)))->cont,((MQ)c)->cont,0,mod);
				STOMQ(t,q); C(mr) = (P)q;
			} else
				mulmp(vl,mod,C(m),c,&C(mr));
			mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar;
	}
}

void divsmdc(vl,mod,p,c,pr)
VL vl;
int mod;
DP p;
P c;
DP *pr;
{
	MP m,mr,mr0;

	if ( !c )
		error("disvsdc : division by 0");
	else if ( !p )
		*pr = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTMP(mr0,mr); divsmp(vl,mod,C(m),c,&C(mr)); mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar;
	}
}

#define MKDPM(n,m,d) (NEWDP(d),(d)->nv=(n),BDY(d)=(m))

void _mdtop(vl,mod,dvl,p,pr)
VL vl,dvl;
int mod;
DP p;
P *pr;
{
	int n,i;
	DL d;
	MP m;
	P r,s,t,u,w;
	Q q;
	MQ tq;
	VL tvl;

	if ( !p )
		*pr = 0;
	else {
		for ( n = p->nv, m = BDY(p), s = 0; m; m = NEXT(m) ) {
			STOMQ(ITOS(C(m)),tq); t = (P)tq;
			for ( i = 0, d = m->dl, tvl = dvl;
				i < n; tvl = NEXT(tvl), i++ ) {
				MKV(tvl->v,r); STOQ(d->d[i],q); pwrmp(vl,mod,r,q,&u);
				mulmp(vl,mod,t,u,&w); t = w;
			}
			addmp(vl,mod,s,t,&u); s = u;
		}
		mptop(s,pr);
	}
}

void _addmd(vl,mod,p1,p2,pr)
VL vl;
int mod;
DP p1,p2,*pr;
{
	int n;
	MP m1,m2,mr,mr0;
	int t;

	if ( !p1 )
		*pr = p2;
	else if ( !p2 )
		*pr = p1;
	else {
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; )
			switch ( (*cmpdl)(n,m1->dl,m2->dl) ) {
				case 0:
					t = (ITOS(C(m1))+ITOS(C(m2))) - mod;
					if ( t < 0 )
						t += mod;
					if ( t ) {
						NEXTMP(mr0,mr); mr->dl = m1->dl; C(mr) = STOI(t);
					}
					m1 = NEXT(m1); m2 = NEXT(m2); break;
				case 1:
					NEXTMP(mr0,mr); mr->dl = m1->dl; C(mr) = C(m1);
					m1 = NEXT(m1); break;
				case -1:
					NEXTMP(mr0,mr); mr->dl = m2->dl; C(mr) = C(m2);
					m2 = NEXT(m2); break;
			}
		if ( !mr0 )
			if ( m1 )
				mr0 = m1;
			else if ( m2 )
				mr0 = m2;
			else {
				*pr = 0;
				return;
			}
		else if ( m1 )
			NEXT(mr) = m1;
		else if ( m2 )
			NEXT(mr) = m2;
		else
			NEXT(mr) = 0;
		MKDPM(NV(p1),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = MAX(p1->sugar,p2->sugar);
	}
}

void _submd(vl,mod,p1,p2,pr)
VL vl;
int mod;
DP p1,p2,*pr;
{
	DP t;

	if ( !p2 )
		*pr = p1;
	else {
		_chsgnmd(mod,p2,&t); _addmd(vl,mod,p1,t,pr);
	}
}

void _chsgnmd(mod,p,pr)
int mod;
DP p,*pr;
{
	MP m,mr,mr0;

	if ( !p )
		*pr = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTMP(mr0,mr); C(mr) = STOI(mod - ITOS(C(m))); mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDPM(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar;
	}
}

void _mulmd(vl,mod,p1,p2,pr)
VL vl;
int mod;
DP p1,p2,*pr;
{
	MP m;
	DP s,t,u;

	if ( !p1 || !p2 )
		*pr = 0;
	else {
		for ( m = BDY(p2), s = 0; m; m = NEXT(m) ) {
			_mulmdm(vl,mod,p1,m,&t); _addmd(vl,mod,s,t,&u); s = u;
		}
		*pr = s;
	}
}

void _mulmdm(vl,mod,p,m0,pr)
VL vl;
int mod;
DP p;
MP m0;
DP *pr;
{
	MP m,mr,mr0;
	DL d;
	int c,n,r;

	if ( !p )
		*pr = 0;
	else {
		for ( mr0 = 0, m = BDY(p), c = ITOS(C(m0)), d = m0->dl, n = NV(p); 
			m; m = NEXT(m) ) {
			NEXTMP(mr0,mr);
			C(mr) = STOI(dmar(ITOS(C(m)),c,0,mod));
			adddl(n,m->dl,d,&mr->dl);
		}
		NEXT(mr) = 0; MKDPM(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar + m0->dl->td;
	}
}

void _dtop_mod(vl,dvl,p,pr)
VL vl,dvl;
DP p;
P *pr;
{
	int n,i;
	DL d;
	MP m;
	P r,s,t,u,w;
	Q q;
	VL tvl;

	if ( !p )
		*pr = 0;
	else {
		for ( n = p->nv, m = BDY(p), s = 0; m; m = NEXT(m) ) {
			i = ITOS(m->c); STOQ(i,q); t = (P)q;
			for ( i = 0, d = m->dl, tvl = dvl;
				i < n; tvl = NEXT(tvl), i++ ) {
				MKV(tvl->v,r); STOQ(d->d[i],q); pwrp(vl,r,q,&u);
				mulp(vl,t,u,&w); t = w;
			}
			addp(vl,s,t,&u); s = u;
		}
		*pr = s;
	}
}

void _dp_red_mod(p1,p2,mod,rp)
DP p1,p2;
int mod;
DP *rp;
{
	int i,n;
	DL d1,d2,d;
	MP m;
	DP t,s;
	int c,c1;

	n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
	NEWDL(d,n); d->td = d1->td - d2->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = d1->d[i]-d2->d[i];
	c = invm(ITOS(BDY(p2)->c),mod); c1 = dmar(c,ITOS(BDY(p1)->c),0,mod);
	NEWMP(m); m->dl = d; m->c = STOI(mod-c1); NEXT(m) = 0;
	MKDP(n,m,s); s->sugar = d->td;
	_mulmd(CO,mod,p2,s,&t); _addmd(CO,mod,p1,t,rp);
}

void _dp_mod(p,mod,subst,rp)
DP p;
int mod;
NODE subst;
DP *rp;
{
	MP m,mr,mr0;
	P t,s;
	NODE tn;

	if ( !p )
		*rp = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			for ( tn = subst, s = m->c; tn; tn = NEXT(NEXT(tn)) ) {
				substp(CO,s,BDY(tn),BDY(NEXT(tn)),&t);
				s = t;
			}
			ptomp(mod,s,&t);
			if ( t ) {
				NEXTMP(mr0,mr); mr->c = STOI(CONT((MQ)t)); mr->dl = m->dl;
			}
		}
		if ( mr0 ) {
			NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
		} else
			*rp = 0;
	}
}

void _dp_sp_mod(p1,p2,mod,rp)
DP p1,p2;
int mod;
DP *rp;
{
	int i,n,td;
	int *w;
	DL d1,d2,d;
	MP m;
	DP t,s,u;

	n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
	w = (int *)ALLOCA(n*sizeof(int));
	for ( i = 0, td = 0; i < n; i++ ) {
		w[i] = MAX(d1->d[i],d2->d[i]); td += w[i];
	}
	NEWDL(d,n); d->td = td - d1->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = w[i] - d1->d[i];
	NEWMP(m); m->dl = d; m->c = BDY(p2)->c; NEXT(m) = 0;
	MKDP(n,m,s); s->sugar = d->td; _mulmd(CO,mod,p1,s,&t);
	NEWDL(d,n); d->td = td - d2->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = w[i] - d2->d[i];
	NEWMP(m); m->dl = d; m->c = STOI(mod - ITOS(BDY(p1)->c)); NEXT(m) = 0;
	MKDP(n,m,s); s->sugar = d->td; _mulmd(CO,mod,p2,s,&u);
	_addmd(CO,mod,t,u,rp);
}

void _dp_sp_component_mod(p1,p2,mod,f1,f2)
DP p1,p2;
int mod;
DP *f1,*f2;
{
	int i,n,td;
	int *w;
	DL d1,d2,d;
	MP m;
	DP t,s,u;

	n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
	w = (int *)ALLOCA(n*sizeof(int));
	for ( i = 0, td = 0; i < n; i++ ) {
		w[i] = MAX(d1->d[i],d2->d[i]); td += w[i];
	}
	NEWDL(d,n); d->td = td - d1->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = w[i] - d1->d[i];
	NEWMP(m); m->dl = d; m->c = BDY(p2)->c; NEXT(m) = 0;
	MKDP(n,m,s); s->sugar = d->td; _mulmd(CO,mod,p1,s,f1);
	NEWDL(d,n); d->td = td - d2->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = w[i] - d2->d[i];
	NEWMP(m); m->dl = d; m->c = BDY(p1)->c; NEXT(m) = 0;
	MKDP(n,m,s); s->sugar = d->td; _mulmd(CO,mod,p2,s,f2);
}

void _printdp(d)
DP d;
{
	int n,i;
	MP m;
	DL dl;

	if ( !d ) {
		printf("0"); return;
	}
	for ( n = d->nv, m = BDY(d); m; m = NEXT(m) ) {
		printf("%d*<<",ITOS(m->c));
		for ( i = 0, dl = m->dl; i < n-1; i++ )
			printf("%d,",dl->d[i]);
		printf("%d",dl->d[i]);
		printf(">>");
		if ( NEXT(m) )
			printf("+");
	}
}
