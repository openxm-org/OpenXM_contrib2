/* $OpenXM: OpenXM/src/asir99/builtin/dp-supp.c,v 1.2 1999/11/18 05:42:01 noro Exp $ */
#include "ca.h"
#include "base.h"
#include "parse.h"
#include "ox.h"

extern int (*cmpdl)();
double pz_t_e,pz_t_d,pz_t_d1,pz_t_c;

void Pdp_mbase(),Pdp_lnf_mod(),Pdp_nf_tab_mod(),Pdp_mdtod();
void Pdp_vtoe(), Pdp_etov(), Pdp_dtov(), Pdp_idiv(), Pdp_sep();
void Pdp_cont(), Pdp_igcdv_hist(), Pdp_idivv_hist();

struct ftab dp_supp_tab[] = {
  {"dp_mbase",Pdp_mbase,1},
  {"dp_lnf_mod",Pdp_lnf_mod,3},
  {"dp_nf_tab_mod",Pdp_nf_tab_mod,3},
  {"dp_etov",Pdp_etov,1},
  {"dp_vtoe",Pdp_vtoe,1},
  {"dp_dtov",Pdp_dtov,1},
  {"dp_idiv",Pdp_idiv,2},
  {"dp_cont",Pdp_cont,1},
  {"dp_sep",Pdp_sep,2},
  {"dp_mdtod",Pdp_mdtod,1},
  {"dp_igcdv_hist",Pdp_igcdv_hist,1},
  {"dp_idivv_hist",Pdp_idivv_hist,1},
  {0,0,0}
};

void Pdp_mdtod(arg,rp)
NODE arg;
DP *rp;
{
	MP m,mr,mr0;
	DP p;
	P t;

	p = (DP)ARG0(arg);
	if ( !p )
		*rp = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			mptop(m->c,&t); NEXTMP(mr0,mr); mr->c = t; mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
	}
}

void Pdp_sep(arg,rp)
NODE arg;
VECT *rp;
{
	DP p,r;
	MP m,t;
	MP *w0,*w;
	int i,n,d,nv,sugar;
	VECT v;
	pointer *pv;

	p = (DP)ARG0(arg); m = BDY(p);
	d = QTOS((Q)ARG1(arg));
	for ( t = m, n = 0; t; t = NEXT(t), n++ );
	if ( d > n )
		d = n;
	MKVECT(v,d); *rp = v;
	pv = BDY(v); nv = p->nv; sugar = p->sugar;
	w0 = (MP *)MALLOC(d*sizeof(MP)); bzero(w0,d*sizeof(MP));
	w = (MP *)MALLOC(d*sizeof(MP)); bzero(w,d*sizeof(MP));
	for ( t = BDY(p), i = 0; t; t = NEXT(t), i++, i %= d  ) {
		NEXTMP(w0[i],w[i]); w[i]->c = t->c; w[i]->dl = t->dl;
	}
	for ( i = 0; i < d; i++ ) {
		NEXT(w[i]) = 0; MKDP(nv,w0[i],r); r->sugar = sugar;
		pv[i] = (pointer)r;
	}
}

void Pdp_idiv(arg,rp)
NODE arg;
DP *rp;
{
	dp_idiv((DP)ARG0(arg),(Q)ARG1(arg),rp);
}

void Pdp_cont(arg,rp)
NODE arg;
Q *rp;
{
	dp_cont((DP)ARG0(arg),rp);
}

void Pdp_dtov(arg,rp)
NODE arg;
VECT *rp;
{
	dp_dtov((DP)ARG0(arg),rp);
}

void Pdp_mbase(arg,rp)
NODE arg;
LIST *rp;
{
	NODE mb;

	asir_assert(ARG0(arg),O_LIST,"dp_mbase");
	dp_mbase(BDY((LIST)ARG0(arg)),&mb);
	MKLIST(*rp,mb);
}

void Pdp_etov(arg,rp)
NODE arg;
VECT *rp;
{
	DP dp;
	int n,i;
	int *d;
	VECT v;
	Q t;

	dp = (DP)ARG0(arg);
	asir_assert(dp,O_DP,"dp_etov");
	n = dp->nv; d = BDY(dp)->dl->d;
	MKVECT(v,n);
	for ( i = 0; i < n; i++ ) {
		STOQ(d[i],t); v->body[i] = (pointer)t;
	}
	*rp = v;
}

void Pdp_vtoe(arg,rp)
NODE arg;
DP *rp;
{
	DP dp;
	DL dl;
	MP m;
	int n,i,td;
	int *d;
	VECT v;

	v = (VECT)ARG0(arg);
	asir_assert(v,O_VECT,"dp_vtoe");
	n = v->len;
	NEWDL(dl,n); d = dl->d;
	for ( i = 0, td = 0; i < n; i++ ) {
		d[i] = QTOS((Q)(v->body[i])); td += d[i];
	}
	dl->td = td;
	NEWMP(m); m->dl = dl; m->c = (P)ONE; NEXT(m) = 0;
	MKDP(n,m,dp); dp->sugar = td;
	*rp = dp;
}

void Pdp_lnf_mod(arg,rp)
NODE arg;
LIST *rp;
{
	DP r1,r2;
	NODE b,g,n;
	int mod;

	asir_assert(ARG0(arg),O_LIST,"dp_lnf_mod");
	asir_assert(ARG1(arg),O_LIST,"dp_lnf_mod");
	asir_assert(ARG2(arg),O_N,"dp_lnf_mod");
	b = BDY((LIST)ARG0(arg)); g = BDY((LIST)ARG1(arg));
	mod = QTOS((Q)ARG2(arg));
	dp_lnf_mod((DP)BDY(b),(DP)BDY(NEXT(b)),g,mod,&r1,&r2);
	NEWNODE(n); BDY(n) = (pointer)r1;
	NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)r2;
	NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

void Pdp_nf_tab_mod(arg,rp)
NODE arg;
DP *rp;
{
	asir_assert(ARG0(arg),O_DP,"dp_nf_tab_mod");
	asir_assert(ARG1(arg),O_VECT,"dp_nf_tab_mod");
	asir_assert(ARG2(arg),O_N,"dp_nf_tab_mod");
	dp_nf_tab_mod((DP)ARG0(arg),(LIST *)BDY((VECT)ARG1(arg)),
		QTOS((Q)ARG2(arg)),rp);
}

void Pdp_igcdv_hist(arg,rp)
NODE arg;
Q *rp;
{
	dp_igcdv_hist((DP)ARG0(arg),rp);
}

void Pdp_idivv_hist(arg,rp)
NODE arg;
DP *rp;
{
	dp_idivv_hist((Q)ARG0(arg),rp);
}

void dp_idiv(p,c,rp)
DP p;
Q c;
DP *rp;
{
	Q t;
	N nm,q;
	int sgn,s;
	MP mr0,m,mr;

	if ( !p )
		*rp = 0;
	else if ( MUNIQ((Q)c) )
		*rp = p;
	else if ( MUNIQ((Q)c) )
		chsgnd(p,rp);
	else {
		nm = NM(c); sgn = SGN(c);
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTMP(mr0,mr);

			divsn(NM((Q)(m->c)),nm,&q);
			s = sgn*SGN((Q)(m->c));
			NTOQ(q,s,t);
			mr->c = (P)t;
			mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(p->nv,mr0,*rp);
		if ( *rp )
			(*rp)->sugar = p->sugar;
	}
}

void dp_cont(p,rp)
DP p;
Q *rp;
{
	VECT v;

	dp_dtov(p,&v); igcdv(v,rp);
}

void dp_dtov(dp,rp)
DP dp;
VECT *rp;
{
	MP m,t;
	int i,n;
	VECT v;
	pointer *p;

	m = BDY(dp);
	for ( t = m, n = 0; t; t = NEXT(t), n++ );
	MKVECT(v,n);
	for ( i = 0, p = BDY(v), t = m; i < n; t = NEXT(t), i++ )
		p[i] = (pointer)(t->c);
	*rp = v;
}

void dp_mbase(hlist,mbase)
NODE hlist;
NODE *mbase;
{
	DL *dl;
	DL d;
	int i,j,n,nvar,td;

	n = length(hlist); nvar = ((DP)BDY(hlist))->nv;
	dl = (DL *)MALLOC(n*sizeof(DL));
	for ( i = 0; i < n; i++, hlist = NEXT(hlist) )
		dl[i] = BDY((DP)BDY(hlist))->dl;
	NEWDL(d,nvar); *mbase = 0;
	while ( 1 ) {
		insert_to_node(d,mbase,nvar);
		for ( i = nvar-1; i >= 0; ) {
			d->d[i]++; d->td++;
			for ( j = 0; j < n; j++ ) {
				if ( _dl_redble(dl[j],d,nvar) )
					break;
			}
			if ( j < n ) {
				for ( j = nvar-1; j >= i; j-- )
					d->d[j] = 0;
				for ( j = 0, td = 0; j < i; j++ )
					td += d->d[j];
				d->td = td;
				i--;
			} else
				break;
		}
		if ( i < 0 )
			break;
	}
}

int _dl_redble(d1,d2,nvar)
DL d1,d2;
int nvar;
{
	int i;

	if ( d1->td > d2->td )
		return 0;
	for ( i = 0; i < nvar; i++ )
		if ( d1->d[i] > d2->d[i] )
			break;
	if ( i < nvar )
		return 0;
	else
		return 1;
}

void insert_to_node(d,n,nvar)
DL d;
NODE *n;
int nvar;
{
	DL d1;
	MP m;
	DP dp;
	NODE n0,n1,n2;

	NEWDL(d1,nvar); d1->td = d->td;
	bcopy((char *)d->d,(char *)d1->d,nvar*sizeof(int));
	NEWMP(m); m->dl = d1; m->c = (P)ONE; NEXT(m) = 0;
	MKDP(nvar,m,dp); dp->sugar = d->td;
	if ( !(*n) ) {
		MKNODE(n1,dp,0); *n = n1;
	} else {
		for ( n1 = *n, n0 = 0; n1; n0 = n1, n1 = NEXT(n1) )
			if ( (*cmpdl)(nvar,d,BDY((DP)BDY(n1))->dl) > 0 ) {
				MKNODE(n2,dp,n1);
				if ( !n0 )
					*n = n2;
				else
					NEXT(n0) = n2;
				break;
			}
		if ( !n1 ) {
			MKNODE(n2,dp,0); NEXT(n0) = n2;
		}
	}
}

void dp_lnf_mod(p1,p2,g,mod,r1p,r2p)
DP p1,p2;
NODE g;
int mod;
DP *r1p,*r2p;
{
	DP r1,r2,b1,b2,t,s;
	P c;
	MQ c1,c2;
	NODE l,b;
	int n;

	if ( !p1 ) {
		*r1p = p1; *r2p = p2; return;
	}
	n = p1->nv;
	for ( l = g, r1 = p1, r2 = p2; l; l = NEXT(l) ) {
			if ( !r1 ) {
				*r1p = r1; *r2p = r2; return;
			}
			b = BDY((LIST)BDY(l)); b1 = (DP)BDY(b);
			if ( dl_equal(n,BDY(r1)->dl,BDY(b1)->dl) ) {
				b2 = (DP)BDY(NEXT(b));
				invmq(mod,(MQ)BDY(b1)->c,&c1);
				mulmq(mod,c1,(MQ)BDY(r1)->c,&c2); chsgnmp(mod,(P)c2,&c);
				mulmdc(CO,mod,b1,c,&t); addmd(CO,mod,r1,t,&s); r1 = s;
				mulmdc(CO,mod,b2,c,&t); addmd(CO,mod,r2,t,&s); r2 = s;
			}
	}
	*r1p = r1; *r2p = r2;
}

void dp_nf_tab_mod(p,tab,mod,rp)
DP p;
LIST *tab;
int mod;
DP *rp;
{
	DP s,t,u;
	MP m;
	DL h;
	int i,n;

	if ( !p ) {
		*rp = p; return;
	}
	n = p->nv;
	for ( s = 0, i = 0, m = BDY(p); m; m = NEXT(m) ) {
		h = m->dl;
		while ( !dl_equal(n,h,BDY((DP)BDY(BDY(tab[i])))->dl ) )
			i++;
		mulmdc(CO,mod,(DP)BDY(NEXT(BDY(tab[i]))),m->c,&t);
		addmd(CO,mod,s,t,&u); s = u;
	}
	*rp = s;
}

void dp_ptozp(p,rp)
DP p,*rp;
{
	MP m,mr,mr0;
	int i,n;
	Q *w;
	Q dvr;
	P t;

	if ( !p )
		*rp = 0;
	else {
		for ( m =BDY(p), n = 0; m; m = NEXT(m), n++ );
		w = (Q *)ALLOCA(n*sizeof(Q));
		for ( m =BDY(p), i = 0; i < n; m = NEXT(m), i++ )
			if ( NUM(m->c) )
				w[i] = (Q)m->c;
			else
				ptozp(m->c,1,&w[i],&t);
		sortbynm(w,n);	
		qltozl(w,n,&dvr);
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTMP(mr0,mr); divsp(CO,m->c,(P)dvr,&mr->c); mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
	}
}

void dp_ptozp2(p0,p1,hp,rp)
DP p0,p1;
DP *hp,*rp;
{
	DP t,s,h,r;
	MP m,mr,mr0,m0;

	addd(CO,p0,p1,&t); dp_ptozp(t,&s);
	if ( !p0 ) {
		h = 0; r = s;
	} else if ( !p1 ) {
		h = s; r = 0;
	} else {
		for ( mr0 = 0, m = BDY(s), m0 = BDY(p0); m0;
			m = NEXT(m), m0 = NEXT(m0) ) {
			NEXTMP(mr0,mr); mr->c = m->c; mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(p0->nv,mr0,h); MKDP(p0->nv,m,r);
	}
	if ( h )
		h->sugar = p0->sugar;
	if ( r )
		r->sugar = p1->sugar;
	*hp = h; *rp = r;
}

struct igcdv_hist {
	int len;
	DP dp;
	Q d,d1;
	Q *q,*q1;
};

static struct igcdv_hist current_igcdv_hist;

void dp_igcdv_hist(p,rp)
DP p;
Q *rp;
{
	VECT c;
	Q *a,*q,*r,*q1,*r1;
	int i,n,nz;
	Q d,d1;
	N qn,rn,gn;
	struct oVECT v;

	dp_dtov(p,&c); a = (Q *)c->body;
	n = c->len;
	q = (Q *)MALLOC(n*sizeof(Q)); r = (Q *)MALLOC(n*sizeof(Q));
	igcdv_estimate(c,&d);
	for ( i = 0, nz = 0; i < n; i++ ) {
		divn(NM(a[i]),NM(d),&qn,&rn);
		NTOQ(qn,SGN(a[i]),q[i]); NTOQ(rn,SGN(a[i]),r[i]);
		if ( r[i] )
			nz = 1;
	}
	current_igcdv_hist.len = n;
	current_igcdv_hist.dp = p;
	current_igcdv_hist.d = d;
	current_igcdv_hist.q = q;
	if ( nz ) {
		v.id = O_VECT; v.len = n; v.body = (pointer *)r;
		igcdv(&v,&d1);
		q1 = (Q *)MALLOC(n*sizeof(Q)); r1 = (Q *)MALLOC(n*sizeof(Q));
		for ( i = 0; i < n; i++ ) {
			if ( r[i] )
				divn(NM(r[i]),NM(d1),&qn,&rn);
			else {
				qn = rn = 0;
			}
			NTOQ(qn,SGN(r[i]),q1[i]); NTOQ(rn,SGN(r[i]),r1[i]);
		}
		gcdn(NM(d),NM(d1),&gn); NTOQ(gn,1,*rp);
	} else {
		d1 = 0; q1 = 0; *rp = d;
	}
	current_igcdv_hist.d1 = d1;
	current_igcdv_hist.q1 = q1;
}

void dp_idivv_hist(g,rp)
Q g;
DP *rp;
{
	int i,n;
	Q *q,*q1,*s;
	Q t,u,m;
	N qn;

	n = current_igcdv_hist.len;
	q = current_igcdv_hist.q; q1 = current_igcdv_hist.q1;
	divsn(NM(current_igcdv_hist.d),NM(g),&qn); NTOQ(qn,1,m);
	s = (Q *)MALLOC(n*sizeof(Q));
	for ( i = 0; i < n; i++ )
		mulq(m,q[i],&s[i]);
	if ( q1 ) {
		divsn(NM(current_igcdv_hist.d1),NM(g),&qn); NTOQ(qn,1,m);
		for ( i = 0; i < n; i++ ) {
			mulq(m,q1[i],&t); addq(t,s[i],&u); s[i] = u;
		}
	}
	dp_vtod(s,current_igcdv_hist.dp,rp);
}

void dp_vtod(c,p,rp)
Q *c;
DP p;
DP *rp;
{
	MP mr0,m,mr;
	int i;

	if ( !p )
		*rp = 0;
	else {
		for ( mr0 = 0, m = BDY(p), i = 0; m; m = NEXT(m), i++ ) {
			NEXTMP(mr0,mr); mr->c = (P)c[i]; mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(p->nv,mr0,*rp);
		(*rp)->sugar = p->sugar;
	}
}

#if INET
void dp_ptozp_d_old(dist,ndist,p,rp)
NODE dist;
int ndist;
DP p,*rp;
{
	DP d,s,u,su;
	MP m,mr,mr0;
	MP *w0,*w;
	Q *g;
	int i,n,nv,nsep;
	P t;
	NODE tn,n0,n1,n2;
	struct oVECT v;
	int id,sindex;
	Obj dmy;
	Q gcd;
	STRING cont,div;

	if ( !p )
		*rp = 0;
	else {
		for ( m = BDY(p), n = 0; m; m = NEXT(m), n++ );
		nsep = ndist + 1;
		if ( n <= nsep ) {
			dp_ptozp(p,rp); return;
		}
		nv = p->nv;
		w0 = (MP *)MALLOC(nsep*sizeof(MP)); bzero(w0,nsep*sizeof(MP));
		w = (MP *)MALLOC(nsep*sizeof(MP)); bzero(w,nsep*sizeof(MP));
		g = (Q *)MALLOC(nsep*sizeof(Q)); bzero(g,nsep*sizeof(Q));
		for ( m = BDY(p), i = 0; m; m = NEXT(m), i++, i %= nsep  ) {
			NEXTMP(w0[i],w[i]); w[i]->c = m->c; w[i]->dl = m->dl;
		}
		MKSTR(cont,"reg_dp_cont"); MKSTR(div,"reg_dp_idiv");

		for ( i = 0, tn = dist; i < ndist; i++, tn = NEXT(tn) ) {
			NEXT(w[i]) = 0; MKDP(nv,w0[i],d); d->sugar = p->sugar;
			MKNODE(n2,d,0); MKNODE(n1,cont,n2); MKNODE(n0,BDY(tn),n1);
			Pox_rpc(n0,&dmy);
		}
		NEXT(w[i]) = 0; MKDP(nv,w0[i],d); d->sugar = p->sugar;
		dp_cont(d,&g[i]);

		for ( i = 0, tn = dist; i < ndist; i++, tn = NEXT(tn) )
			Pox_pop_local(tn,&g[i]);
		v.id = O_VECT; v.len = nsep; v.body = (pointer *)g; igcdv(&v,&gcd);

		for ( i = 0, tn = dist; i < ndist; i++, tn = NEXT(tn) ) {
			MKNODE(n2,gcd,0); MKNODE(n1,div,n2); MKNODE(n0,BDY(tn),n1);
			Pox_rpc(n0,&dmy);
		}
		dp_idiv(d,gcd,&s);

		for ( i = 0, tn = dist; i < ndist; i++, tn = NEXT(tn) ) {
			Pox_pop_local(tn,&u);
			addd(CO,s,u,&su); s = su;
		}
		*rp = s;
	}
}

void dp_ptozp_d(dist,ndist,p,rp)
NODE dist;
int ndist;
DP p,*rp;
{
	int i,j,k,l,n,nsep;
	MP m;
	NODE tn,n0,n1,n2,n3;
	struct oVECT v;
	VECT c,cs;
	VECT qi,ri;
	LIST *qr;
	int s,id;
	Obj dmy;
	Q d0,d1,gcd,a,u,u1;
	Q *q,*r;
	STRING iqr_v;
	pointer *b;
	N qn,gn;
	double get_rtime();
	int blen;
	double t0;
	double t_e,t_d,t_d1,t_c;

	if ( !p )
		*rp = 0;
	else {
		for ( m = BDY(p), n = 0; m; m = NEXT(m), n++ );
		nsep = ndist + 1;
		if ( n <= nsep ) {
			dp_ptozp(p,rp); return;
		}
		t0 = get_rtime();
		dp_dtov(p,&c);
		igcdv_estimate(c,&d0);
		t_e = get_rtime()-t0;
		t0 = get_rtime();
		dp_dtov(p,&c);
		sepvect(c,nsep,&cs);
		MKSTR(iqr_v,"iqr");
		qr = (LIST *)CALLOC(nsep,sizeof(LIST));
		q = (Q *)CALLOC(n,sizeof(Q));
		r = (Q *)CALLOC(n,sizeof(Q));
		for ( i = 0, tn = dist, b = BDY(cs); i < ndist; i++, tn = NEXT(tn) ) {
			MKNODE(n3,d0,0); MKNODE(n2,b[i],n3);
			MKNODE(n1,iqr_v,n2); MKNODE(n0,BDY(tn),n1);
			Pox_rpc(n0,&dmy);
		}
		iqrv(b[i],d0,&qr[i]);
		dp_dtov(p,&c);
		for ( i = 0, tn = dist; i < ndist; i++, tn = NEXT(tn) ) {
			Pox_pop_local(tn,&qr[i]);
			if ( OID(qr[i]) == O_ERR ) {
				printexpr(CO,(Obj)qr[i]);
				error("dp_ptozp_d : aborted");
			}
		}
		t_d = get_rtime()-t0;
		t_d1 = t_d/n;
		t0 = get_rtime();
		for ( i = j = 0; i < nsep; i++ ) {
			tn = BDY(qr[i]); qi = (VECT)BDY(tn); ri = (VECT)BDY(NEXT(tn));
			for ( k = 0, l = qi->len; k < l; k++, j++ ) {
				q[j] = (Q)BDY(qi)[k]; r[j] = (Q)BDY(ri)[k];
			}
		}
		v.id = O_VECT; v.len = n; v.body = (pointer *)r; igcdv(&v,&d1);
		if ( d1 ) {
			gcdn(NM(d0),NM(d1),&gn); NTOQ(gn,1,gcd);
			divsn(NM(d0),gn,&qn); NTOQ(qn,1,a);
			for ( i = 0; i < n; i++ ) {
				mulq(a,q[i],&u);
				if ( r[i] ) {
					divsn(NM(r[i]),gn,&qn); NTOQ(qn,SGN(r[i]),u1);
					addq(u,u1,&q[i]);
				} else
					q[i] = u;
			}
		} else
			gcd = d0;
		dp_vtod(q,p,rp);
		t_c = get_rtime()-t0;
		blen=p_mag((P)gcd);
		pz_t_e += t_e; pz_t_d += t_d; pz_t_d1 += t_d1; pz_t_c += t_c;
		if ( 0 )
			fprintf(stderr,"(%d,%d)",p_mag((P)d0)-blen,blen);
	}
}

void dp_ptozp2_d(dist,ndist,p0,p1,hp,rp)
NODE dist;
int ndist;
DP p0,p1;
DP *hp,*rp;
{
	DP t,s,h,r;
	MP m,mr,mr0,m0;

	addd(CO,p0,p1,&t); dp_ptozp_d(dist,ndist,t,&s);
	if ( !p0 ) {
		h = 0; r = s;
	} else if ( !p1 ) {
		h = s; r = 0;
	} else {
		for ( mr0 = 0, m = BDY(s), m0 = BDY(p0); m0;
			m = NEXT(m), m0 = NEXT(m0) ) {
			NEXTMP(mr0,mr); mr->c = m->c; mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(p0->nv,mr0,h); MKDP(p0->nv,m,r);
	}
	if ( h )
		h->sugar = p0->sugar;
	if ( r )
		r->sugar = p1->sugar;
	*hp = h; *rp = r;
}
#endif

/*
 * Old codes
 */

#if 0
void Pdp_sep(arg,rp)
NODE arg;
VECT *rp;
{
	DP p,s;
	MP m,t,mr,mr0;
	int i,j,n,d,r,q,q1,nv;
	VECT w;
	pointer *pw;

	p = (DP)ARG0(arg); m = BDY(p);
	d = QTOS((Q)ARG1(arg));
	for ( t = m, n = 0; t; t = NEXT(t), n++ );
	if ( d > n )
		d = n;
	q = n/d; r = n%d; q1 = q+1;
	MKVECT(w,d); *rp = w;
	t = m; pw = BDY(w); nv = p->nv;
	for ( i = 0; i < r; i++ ) {
		for ( mr0 = 0, j = 0; j < q1; j++, t = NEXT(t) ) {
			NEXTMP(mr0,mr);
			mr->c = t->c;
			mr->dl = t->dl;
		}
		NEXT(mr) = 0; MKDP(nv,mr0,s); s->sugar = p->sugar;
		pw[i] = (pointer)s;	
	}
	for ( ; i < d; i++ ) {
		for ( mr0 = 0, j = 0; j < q; j++, t = NEXT(t) ) {
			NEXTMP(mr0,mr);
			mr->c = t->c;
			mr->dl = t->dl;
		}
		NEXT(mr) = 0; MKDP(nv,mr0,s); s->sugar = p->sugar;
		pw[i] = (pointer)s;	
	}
}

void dp_ptozp_mpi(dist,ndist,p,rp)
NODE dist;
int ndist;
DP p,*rp;
{
	int i,j,k,l,n,nsep;
	MP m;
	NODE tn,n0,n1,n2,n3;
	struct oVECT v;
	VECT c,cs;
	VECT qi,ri;
	LIST *qr;
	int id;
	Obj dmy;
	Q d0,d1,gcd,a,u,u1;
	Q *q,*r;
	STRING iqr_v;
	pointer *b;
	N qn,gn;
	double get_rtime();
	int blen;
	double t0;
	double t_e,t_d,t_d1,t_c;

	if ( !p )
		*rp = 0;
	else {
		for ( m = BDY(p), n = 0; m; m = NEXT(m), n++ );
		nsep = ndist + 1;
		if ( n <= nsep ) {
			dp_ptozp(p,rp); return;
		}
		t0 = get_rtime();
		dp_dtov(p,&c);
		igcdv_estimate(c,&d0);
		t_e = get_rtime()-t0;
		t0 = get_rtime();
		dp_dtov(p,&c);
		sepvect(c,nsep,&cs);
		MKSTR(iqr_v,"iqr");
		qr = (LIST *)CALLOC(nsep,sizeof(LIST));
		q = (Q *)CALLOC(n,sizeof(Q));
		r = (Q *)CALLOC(n,sizeof(Q));
		for ( i = 0, tn = dist, b = BDY(cs); i < ndist;
			 i++, tn = NEXT(tn) ) {
			div_vect_mpi(QTOS((Q)BDY(tn)),b[i],d0);
		}
		iqrv(b[i],d0,&qr[i]);
		dp_dtov(p,&c);
		for ( i = j = 0, tn = dist; i < ndist; i++, tn = NEXT(tn) ) {
			get_div_vect_mpi(QTOS((Q)BDY(tn)),&qr[i]);
		}
		t_d = get_rtime()-t0;
		t_d1 = t_d/n;
		t0 = get_rtime();
		for ( i = j = 0; i < nsep; i++ ) {
			tn = BDY(qr[i]); qi = (VECT)BDY(tn); ri = (VECT)BDY(NEXT(tn));
			for ( k = 0, l = qi->len; k < l; k++, j++ ) {
				q[j] = (Q)BDY(qi)[k]; r[j] = (Q)BDY(ri)[k];
			}
		}
		v.id = O_VECT; v.len = n; v.body = (pointer *)r; igcdv(&v,&d1);
		if ( d1 ) {
			gcdn(NM(d0),NM(d1),&gn); NTOQ(gn,1,gcd);
			divsn(NM(d0),gn,&qn); NTOQ(qn,1,a);
			for ( i = 0; i < n; i++ ) {
				mulq(a,q[i],&u);
				if ( r[i] ) {
					divsn(NM(r[i]),gn,&qn); NTOQ(qn,SGN(r[i]),u1);
					addq(u,u1,&q[i]);
				} else
					q[i] = u;
			}
		} else
			gcd = d0;
		dp_vtod(q,p,rp);
		t_c = get_rtime()-t0;
		blen=p_mag(gcd);
		pz_t_e += t_e; pz_t_d += t_d; pz_t_d1 += t_d1; pz_t_c += t_c;
		if ( 0 )
			fprintf(stderr,"(%d,%d)",p_mag(d0)-blen,blen);
	}
}

void dp_ptozp_d(dist,ndist,p,rp)
NODE dist;
int ndist;
DP p,*rp;
{
	DP d,s,u,su;
	MP m,mr,mr0;
	MP *w0,*w;
	Q *g;
	int i,n,nv,nsep;
	P t;
	NODE tn,n0,n1,n2;
	struct oVECT v;
	int sindex,id;
	Obj dmy;
	Q gcd;
	STRING gcd_h,div_h;

	if ( !p )
		*rp = 0;
	else {
		for ( m = BDY(p), n = 0; m; m = NEXT(m), n++ );
		nsep = ndist + 1;
		if ( n <= nsep ) {
			dp_ptozp(p,rp); return;
		}
		nv = p->nv;
		w0 = (MP *)MALLOC(nsep*sizeof(MP)); bzero(w0,nsep*sizeof(MP));
		w = (MP *)MALLOC(nsep*sizeof(MP)); bzero(w,nsep*sizeof(MP));
		g = (Q *)MALLOC(nsep*sizeof(Q)); bzero(g,nsep*sizeof(Q));
		for ( m = BDY(p), i = 0; m; m = NEXT(m), i++, i %= nsep  ) {
			NEXTMP(w0[i],w[i]); w[i]->c = m->c; w[i]->dl = m->dl;
		}
		MKSTR(gcd_h,"dp_igcdv_hist"); MKSTR(div_h,"dp_idivv_hist");

		for ( i = 0, tn = dist; i < ndist; i++, tn = NEXT(tn) ) {
			NEXT(w[i]) = 0; MKDP(nv,w0[i],d); d->sugar = p->sugar;
			MKNODE(n2,d,0); MKNODE(n1,gcd_h,n2); MKNODE(n0,BDY(tn),n1);
			Pox_rpc(n0,&dmy);
		}
		NEXT(w[i]) = 0; MKDP(nv,w0[i],d); d->sugar = p->sugar;
		dp_igcdv_hist(d,&g[i]);

		for ( i = 0, tn = dist; i < ndist; i++, tn = NEXT(tn) )
			Pox_pop_local(tn,&g[i]);
		v.id = O_VECT; v.len = nsep; v.body = (pointer *)g; igcdv(&v,&gcd);

		for ( i = 0, tn = dist; i < ndist; i++, tn = NEXT(tn) ) {
			MKNODE(n2,gcd,0); MKNODE(n1,div_h,n2); MKNODE(n0,BDY(tn),n1);
			Pox_rpc(n0,&dmy);
		}
		dp_idivv_hist(gcd,&s);

		for ( i = 0, tn = dist; i < ndist; i++, tn = NEXT(tn) ) {
			Pox_pop_local(tn,&u);
			addd(CO,s,u,&su); s = su;
		}
		*rp = s;
	}
}
#endif
