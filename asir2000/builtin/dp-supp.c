/*
 * Copyright (c) 1994-2000 FUJITSU LABORATORIES LIMITED 
 * All rights reserved.
 * 
 * FUJITSU LABORATORIES LIMITED ("FLL") hereby grants you a limited,
 * non-exclusive and royalty-free license to use, copy, modify and
 * redistribute, solely for non-commercial and non-profit purposes, the
 * computer program, "Risa/Asir" ("SOFTWARE"), subject to the terms and
 * conditions of this Agreement. For the avoidance of doubt, you acquire
 * only a limited right to use the SOFTWARE hereunder, and FLL or any
 * third party developer retains all rights, including but not limited to
 * copyrights, in and to the SOFTWARE.
 * 
 * (1) FLL does not grant you a license in any way for commercial
 * purposes. You may use the SOFTWARE only for non-commercial and
 * non-profit purposes only, such as academic, research and internal
 * business use.
 * (2) The SOFTWARE is protected by the Copyright Law of Japan and
 * international copyright treaties. If you make copies of the SOFTWARE,
 * with or without modification, as permitted hereunder, you shall affix
 * to all such copies of the SOFTWARE the above copyright notice.
 * (3) An explicit reference to this SOFTWARE and its copyright owner
 * shall be made on your publication or presentation in any form of the
 * results obtained by use of the SOFTWARE.
 * (4) In the event that you modify the SOFTWARE, you shall notify FLL by
 * e-mail at risa-admin@sec.flab.fujitsu.co.jp of the detailed specification
 * for such modification or the source code of the modified part of the
 * SOFTWARE.
 * 
 * THE SOFTWARE IS PROVIDED AS IS WITHOUT ANY WARRANTY OF ANY KIND. FLL
 * MAKES ABSOLUTELY NO WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY, AND
 * EXPRESSLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT OF THIRD PARTIES'
 * RIGHTS. NO FLL DEALER, AGENT, EMPLOYEES IS AUTHORIZED TO MAKE ANY
 * MODIFICATIONS, EXTENSIONS, OR ADDITIONS TO THIS WARRANTY.
 * UNDER NO CIRCUMSTANCES AND UNDER NO LEGAL THEORY, TORT, CONTRACT,
 * OR OTHERWISE, SHALL FLL BE LIABLE TO YOU OR ANY OTHER PERSON FOR ANY
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, PUNITIVE OR CONSEQUENTIAL
 * DAMAGES OF ANY CHARACTER, INCLUDING, WITHOUT LIMITATION, DAMAGES
 * ARISING OUT OF OR RELATING TO THE SOFTWARE OR THIS AGREEMENT, DAMAGES
 * FOR LOSS OF GOODWILL, WORK STOPPAGE, OR LOSS OF DATA, OR FOR ANY
 * DAMAGES, EVEN IF FLL SHALL HAVE BEEN INFORMED OF THE POSSIBILITY OF
 * SUCH DAMAGES, OR FOR ANY CLAIM BY ANY OTHER PARTY. EVEN IF A PART
 * OF THE SOFTWARE HAS BEEN DEVELOPED BY A THIRD PARTY, THE THIRD PARTY
 * DEVELOPER SHALL HAVE NO LIABILITY IN CONNECTION WITH THE USE,
 * PERFORMANCE OR NON-PERFORMANCE OF THE SOFTWARE.
 *
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/dp-supp.c,v 1.15 2001/09/11 08:56:47 noro Exp $ 
*/
#include "ca.h"
#include "base.h"
#include "inline.h"
#include "parse.h"
#include "ox.h"

#define HMAG(p) (p_mag(BDY(p)->c))

extern int (*cmpdl)();
extern double pz_t_e,pz_t_d,pz_t_d1,pz_t_c;
extern int dp_nelim,dp_fcoeffs;
extern int NoGCD;
extern int GenTrace;
extern NODE TraceList;

/* 
 * content reduction
 *
 */

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

extern int mpi_mag;
extern int PCoeffs;

void dp_ptozp_d(p,rp)
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
	NODE dist;
	int ndist;
	double t0;
	double t_e,t_d,t_d1,t_c;
	extern int DP_NFStat;
	extern LIST Dist;

	if ( !p )
		*rp = 0;
	else {
		if ( PCoeffs ) {
			dp_ptozp(p,rp); return;
		}	
		if ( !Dist || p_mag(BDY(p)->c) <= mpi_mag ) {
			dist = 0; ndist = 0;
			if ( DP_NFStat ) fprintf(asir_out,"L");
		} else {
			dist = BDY(Dist); ndist = length(dist);
			if ( DP_NFStat ) fprintf(asir_out,"D");
		}
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

void dp_ptozp2_d(p0,p1,hp,rp)
DP p0,p1;
DP *hp,*rp;
{
	DP t,s,h,r;
	MP m,mr,mr0,m0;

	addd(CO,p0,p1,&t); dp_ptozp_d(t,&s);
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

void dp_prim(p,rp)
DP p,*rp;
{
	P t,g;
	DP p1;
	MP m,mr,mr0;
	int i,n;
	P *w;
	Q *c;
	Q dvr;

	if ( !p )
		*rp = 0;
	else if ( dp_fcoeffs )
		*rp = p;
	else if ( NoGCD )
		dp_ptozp(p,rp);
	else {
		dp_ptozp(p,&p1); p = p1;
		for ( m = BDY(p), n = 0; m; m = NEXT(m), n++ );
		if ( n == 1 ) {
			m = BDY(p);
			NEWMP(mr); mr->dl = m->dl; mr->c = (P)ONE; NEXT(mr) = 0;
			MKDP(p->nv,mr,*rp); (*rp)->sugar = p->sugar;
			return;
		}
		w = (P *)ALLOCA(n*sizeof(P));
		c = (Q *)ALLOCA(n*sizeof(Q));
		for ( m =BDY(p), i = 0; i < n; m = NEXT(m), i++ )
			if ( NUM(m->c) ) {
				c[i] = (Q)m->c; w[i] = (P)ONE;
			} else
				ptozp(m->c,1,&c[i],&w[i]);
		qltozl(c,n,&dvr); heu_nezgcdnpz(CO,w,n,&t); mulp(CO,t,(P)dvr,&g);
		if ( NUM(g) )
			*rp = p;
		else {
			for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
				NEXTMP(mr0,mr); divsp(CO,m->c,g,&mr->c); mr->dl = m->dl;
			}
			NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
		}
	}
}

void heu_nezgcdnpz(vl,pl,m,pr)
VL vl;
P *pl,*pr;
int m;
{
	int i,r;
	P gcd,t,s1,s2,u;
	Q rq;
	
	while ( 1 ) {
		for ( i = 0, s1 = 0; i < m; i++ ) {
			r = random(); UTOQ(r,rq);
			mulp(vl,pl[i],(P)rq,&t); addp(vl,s1,t,&u); s1 = u;
		}
		for ( i = 0, s2 = 0; i < m; i++ ) {
			r = random(); UTOQ(r,rq);
			mulp(vl,pl[i],(P)rq,&t); addp(vl,s2,t,&u); s2 = u;
		}
		ezgcdp(vl,s1,s2,&gcd);
		for ( i = 0; i < m; i++ ) {
			if ( !divtpz(vl,pl[i],gcd,&t) )
				break;
		}
		if ( i == m )
			break;
	}
	*pr = gcd;
}

void dp_prim_mod(p,mod,rp)
int mod;
DP p,*rp;
{
	P t,g;
	MP m,mr,mr0;

	if ( !p )
		*rp = 0;
	else if ( NoGCD )
		*rp = p;
	else {
		for ( m = BDY(p), g = m->c, m = NEXT(m); m; m = NEXT(m) ) {
			gcdprsmp(CO,mod,g,m->c,&t); g = t;
		}
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTMP(mr0,mr); divsmp(CO,mod,m->c,g,&mr->c); mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
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

/*
 * s-poly computation
 *
 */

void dp_sp(p1,p2,rp)
DP p1,p2;
DP *rp;
{
	int i,n,td;
	int *w;
	DL d1,d2,d;
	MP m;
	DP t,s1,s2,u;
	Q c,c1,c2;
	N gn,tn;

	n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
	w = (int *)ALLOCA(n*sizeof(int));
	for ( i = 0, td = 0; i < n; i++ ) {
		w[i] = MAX(d1->d[i],d2->d[i]); td += w[i];
	}

	NEWDL(d,n); d->td = td - d1->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = w[i] - d1->d[i];
	c1 = (Q)BDY(p1)->c; c2 = (Q)BDY(p2)->c;
	if ( INT(c1) && INT(c2) ) {
		gcdn(NM(c1),NM(c2),&gn);
		if ( !UNIN(gn) ) {
			divsn(NM(c1),gn,&tn); NTOQ(tn,SGN(c1),c); c1 = c;
			divsn(NM(c2),gn,&tn); NTOQ(tn,SGN(c2),c); c2 = c;
		}
	}

	NEWMP(m); m->dl = d; m->c = (P)c2; NEXT(m) = 0;
	MKDP(n,m,s1); s1->sugar = d->td; muld(CO,s1,p1,&t);

	NEWDL(d,n); d->td = td - d2->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = w[i] - d2->d[i];
	NEWMP(m); m->dl = d; m->c = (P)c1; NEXT(m) = 0;
	MKDP(n,m,s2); s2->sugar = d->td; muld(CO,s2,p2,&u);

	subd(CO,t,u,rp);
	if ( GenTrace ) {
		LIST hist;
		NODE node;

		node = mknode(4,ONE,0,s1,ONE);
		MKLIST(hist,node);
		MKNODE(TraceList,hist,0);

		node = mknode(4,ONE,0,0,ONE);
		chsgnd(s2,(DP *)&ARG2(node));
		MKLIST(hist,node);
		MKNODE(node,hist,TraceList); TraceList = node;
	}
}

void _dp_sp_dup(p1,p2,rp)
DP p1,p2;
DP *rp;
{
	int i,n,td;
	int *w;
	DL d1,d2,d;
	MP m;
	DP t,s1,s2,u;
	Q c,c1,c2;
	N gn,tn;

	n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
	w = (int *)ALLOCA(n*sizeof(int));
	for ( i = 0, td = 0; i < n; i++ ) {
		w[i] = MAX(d1->d[i],d2->d[i]); td += w[i];
	}

	_NEWDL(d,n); d->td = td - d1->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = w[i] - d1->d[i];
	c1 = (Q)BDY(p1)->c; c2 = (Q)BDY(p2)->c;
	if ( INT(c1) && INT(c2) ) {
		gcdn(NM(c1),NM(c2),&gn);
		if ( !UNIN(gn) ) {
			divsn(NM(c1),gn,&tn); NTOQ(tn,SGN(c1),c); c1 = c;
			divsn(NM(c2),gn,&tn); NTOQ(tn,SGN(c2),c); c2 = c;
		}
	}

	_NEWMP(m); m->dl = d; m->c = (P)c2; NEXT(m) = 0;
	_MKDP(n,m,s1); s1->sugar = d->td; _muld_dup(CO,s1,p1,&t); _free_dp(s1);

	_NEWDL(d,n); d->td = td - d2->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = w[i] - d2->d[i];
	_NEWMP(m); m->dl = d; chsgnp((P)c1,&m->c); NEXT(m) = 0;
	_MKDP(n,m,s2); s2->sugar = d->td; _muld_dup(CO,s2,p2,&u); _free_dp(s2);

	_addd_destructive(CO,t,u,rp);
	if ( GenTrace ) {
		LIST hist;
		NODE node;

		node = mknode(4,ONE,0,s1,ONE);
		MKLIST(hist,node);
		MKNODE(TraceList,hist,0);

		node = mknode(4,ONE,0,0,ONE);
		chsgnd(s2,(DP *)&ARG2(node));
		MKLIST(hist,node);
		MKNODE(node,hist,TraceList); TraceList = node;
	}
}

void dp_sp_mod(p1,p2,mod,rp)
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
	NEWMP(m); m->dl = d; m->c = (P)BDY(p2)->c; NEXT(m) = 0;
	MKDP(n,m,s); s->sugar = d->td; mulmd(CO,mod,p1,s,&t);
	NEWDL(d,n); d->td = td - d2->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = w[i] - d2->d[i];
	NEWMP(m); m->dl = d; m->c = (P)BDY(p1)->c; NEXT(m) = 0;
	MKDP(n,m,s); s->sugar = d->td; mulmd(CO,mod,p2,s,&u);
	submd(CO,mod,t,u,rp);
}

void _dp_sp_mod_dup(p1,p2,mod,rp)
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
	_NEWDL(d,n); d->td = td - d1->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = w[i] - d1->d[i];
	_NEWMP(m); m->dl = d; m->c = BDY(p2)->c; NEXT(m) = 0;
	_MKDP(n,m,s); s->sugar = d->td; _mulmd_dup(mod,s,p1,&t); _free_dp(s);
	_NEWDL(d,n); d->td = td - d2->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = w[i] - d2->d[i];
	_NEWMP(m); m->dl = d; m->c = STOI(mod - ITOS(BDY(p1)->c)); NEXT(m) = 0;
	_MKDP(n,m,s); s->sugar = d->td; _mulmd_dup(mod,s,p2,&u); _free_dp(s);
	_addmd_destructive(mod,t,u,rp);
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
	MKDP(n,m,s); s->sugar = d->td; mulmd_dup(mod,s,p1,&t);
	NEWDL(d,n); d->td = td - d2->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = w[i] - d2->d[i];
	NEWMP(m); m->dl = d; m->c = STOI(mod - ITOS(BDY(p1)->c)); NEXT(m) = 0;
	MKDP(n,m,s); s->sugar = d->td; mulmd_dup(mod,s,p2,&u);
	addmd_destructive(mod,t,u,rp);
}

/*
 * m-reduction
 * do content reduction over Z or Q(x,...)
 * do nothing over finite fields
 *
 */

void dp_red(p0,p1,p2,head,rest,dnp,multp)
DP p0,p1,p2;
DP *head,*rest;
P *dnp;
DP *multp;
{
	int i,n;
	DL d1,d2,d;
	MP m;
	DP t,s,r,h;
	Q c,c1,c2;
	N gn,tn;
	P g,a;

	n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
	NEWDL(d,n); d->td = d1->td - d2->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = d1->d[i]-d2->d[i];
	c1 = (Q)BDY(p1)->c; c2 = (Q)BDY(p2)->c;
	if ( dp_fcoeffs ) {
		/* do nothing */
	} else if ( INT(c1) && INT(c2) ) {
		gcdn(NM(c1),NM(c2),&gn);
		if ( !UNIN(gn) ) {
			divsn(NM(c1),gn,&tn); NTOQ(tn,SGN(c1),c); c1 = c;
			divsn(NM(c2),gn,&tn); NTOQ(tn,SGN(c2),c); c2 = c;
		}
	} else {
		ezgcdpz(CO,(P)c1,(P)c2,&g);
		divsp(CO,(P)c1,g,&a); c1 = (Q)a; divsp(CO,(P)c2,g,&a); c2 = (Q)a;
	}
	NEWMP(m); m->dl = d; chsgnp((P)c1,&m->c); NEXT(m) = 0; MKDP(n,m,s); s->sugar = d->td;
	*multp = s;
	muld(CO,s,p2,&t); muldc(CO,p1,(P)c2,&s); addd(CO,s,t,&r);
	muldc(CO,p0,(P)c2,&h);
	*head = h; *rest = r; *dnp = (P)c2;
}

/* m-reduction over a field */

void dp_red_f(p1,p2,rest)
DP p1,p2;
DP *rest;
{
	int i,n;
	DL d1,d2,d;
	MP m;
	DP t,s,r,h;
	Obj a,b;

	n = p1->nv;
	d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;

	NEWDL(d,n); d->td = d1->td - d2->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = d1->d[i]-d2->d[i];

	NEWMP(m); m->dl = d;
	divr(CO,(Obj)BDY(p1)->c,(Obj)BDY(p2)->c,&a); chsgnr(a,&b);
	C(m) = (P)b;
	NEXT(m) = 0; MKDP(n,m,s); s->sugar = d->td;

	muld(CO,s,p2,&t); addd(CO,p1,t,rest);
}

void dp_red_mod(p0,p1,p2,mod,head,rest,dnp)
DP p0,p1,p2;
int mod;
DP *head,*rest;
P *dnp;
{
	int i,n;
	DL d1,d2,d;
	MP m;
	DP t,s,r,h;
	P c1,c2,g,u;

	n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
	NEWDL(d,n); d->td = d1->td - d2->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = d1->d[i]-d2->d[i];
	c1 = (P)BDY(p1)->c; c2 = (P)BDY(p2)->c;
	gcdprsmp(CO,mod,c1,c2,&g);
	divsmp(CO,mod,c1,g,&u); c1 = u; divsmp(CO,mod,c2,g,&u); c2 = u;
	if ( NUM(c2) ) {
		divsmp(CO,mod,c1,c2,&u); c1 = u; c2 = (P)ONEM;
	}
	NEWMP(m); m->dl = d; chsgnmp(mod,(P)c1,&m->c); NEXT(m) = 0;
	MKDP(n,m,s); s->sugar = d->td; mulmd(CO,mod,s,p2,&t);
	if ( NUM(c2) ) {
		addmd(CO,mod,p1,t,&r); h = p0;
	} else {
		mulmdc(CO,mod,p1,c2,&s); addmd(CO,mod,s,t,&r); mulmdc(CO,mod,p0,c2,&h);
	}
	*head = h; *rest = r; *dnp = c2;
}

struct oEGT eg_red_mod;

void _dp_red_mod_destructive(p1,p2,mod,rp)
DP p1,p2;
int mod;
DP *rp;
{
	int i,n;
	DL d1,d2,d;
	MP m;
	DP t,s;
	int c,c1,c2;
	struct oEGT t0,t1;
	extern int do_weyl;

	n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
	_NEWDL(d,n); d->td = d1->td - d2->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = d1->d[i]-d2->d[i];
	c = invm(ITOS(BDY(p2)->c),mod); 
	c2 = ITOS(BDY(p1)->c);
	DMAR(c,c2,0,mod,c1);
	_NEWMP(m); m->dl = d; m->c = STOI(mod-c1); NEXT(m) = 0;
#if 0
	_MKDP(n,m,s); s->sugar = d->td;
	_mulmd_dup(mod,s,p2,&t); _free_dp(s);
#else
	if ( do_weyl ) {
		_weyl_mulmdm_dup(mod,p2,m,&t); _FREEMP(m);
	} else {
		_mulmdm_dup(mod,p2,m,&t); _FREEMP(m);
	}
#endif
/* get_eg(&t0); */
	_addmd_destructive(mod,p1,t,rp);
/* get_eg(&t1); add_eg(&eg_red_mod,&t0,&t1); */
	_print_mp(NV(*rp),BDY(*rp));
}

/*
 * normal form computation
 *
 */

void dp_true_nf(b,g,ps,full,rp,dnp)
NODE b;
DP g;
DP *ps;
int full;
DP *rp;
P *dnp;
{
	DP u,p,d,s,t,dmy;
	NODE l;
	MP m,mr;
	int i,n;
	int *wb;
	int sugar,psugar;
	P dn,tdn,tdn1;

	dn = (P)ONE;
	if ( !g ) {
		*rp = 0; *dnp = dn; return;
	}
	for ( n = 0, l = b; l; l = NEXT(l), n++ );
	wb = (int *)ALLOCA(n*sizeof(int));
	for ( i = 0, l = b; i < n; l = NEXT(l), i++ )
		wb[i] = QTOS((Q)BDY(l));
	sugar = g->sugar;
	for ( d = 0; g; ) {
		for ( u = 0, i = 0; i < n; i++ ) {
			if ( dp_redble(g,p = ps[wb[i]]) ) {
				dp_red(d,g,p,&t,&u,&tdn,&dmy);
				psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
				sugar = MAX(sugar,psugar);
				if ( !u ) {
					if ( d )
						d->sugar = sugar;
					*rp = d; *dnp = dn; return;
				} else {
					d = t;
					mulp(CO,dn,tdn,&tdn1); dn = tdn1;
				}
				break;
			}
		}
		if ( u )
			g = u;
		else if ( !full ) {
			if ( g ) {
				MKDP(g->nv,BDY(g),t); t->sugar = sugar; g = t;
			}
			*rp = g; *dnp = dn; return;
		} else {
			m = BDY(g); NEWMP(mr); mr->dl = m->dl; mr->c = m->c;
			NEXT(mr) = 0; MKDP(g->nv,mr,t); t->sugar = mr->dl->td;
			addd(CO,d,t,&s); d = s;
			dp_rest(g,&t); g = t;
		}
	}
	if ( d )
		d->sugar = sugar;
	*rp = d; *dnp = dn;
}

/* nf computation over Z */

void dp_nf_z(b,g,ps,full,multiple,rp)
NODE b;
DP g;
DP *ps;
int full,multiple;
DP *rp;
{
	DP u,p,d,s,t,dmy1;
	P dmy;
	NODE l;
	MP m,mr;
	int i,n;
	int *wb;
	int hmag;
	int sugar,psugar;

	if ( !g ) {
		*rp = 0; return;
	}
	for ( n = 0, l = b; l; l = NEXT(l), n++ );
	wb = (int *)ALLOCA(n*sizeof(int));
	for ( i = 0, l = b; i < n; l = NEXT(l), i++ )
		wb[i] = QTOS((Q)BDY(l));

	hmag = multiple*HMAG(g);
	sugar = g->sugar;

	for ( d = 0; g; ) {
		for ( u = 0, i = 0; i < n; i++ ) {
			if ( dp_redble(g,p = ps[wb[i]]) ) {
				dp_red(d,g,p,&t,&u,&dmy,&dmy1);
				psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
				sugar = MAX(sugar,psugar);
				if ( !u ) {
					if ( d )
						d->sugar = sugar;
					*rp = d; return;
				}
				d = t;
				break;
			}
		}
		if ( u ) {
			g = u;
			if ( d ) {
				if ( multiple && HMAG(d) > hmag ) {
					dp_ptozp2(d,g,&t,&u); d = t; g = u;
					hmag = multiple*HMAG(d);
				}
			} else {
				if ( multiple && HMAG(g) > hmag ) {
					dp_ptozp(g,&t); g = t;
					hmag = multiple*HMAG(g);
				}
			}
		}
		else if ( !full ) {
			if ( g ) {
				MKDP(g->nv,BDY(g),t); t->sugar = sugar; g = t;
			}
			*rp = g; return;
		} else {
			m = BDY(g); NEWMP(mr); mr->dl = m->dl; mr->c = m->c;
			NEXT(mr) = 0; MKDP(g->nv,mr,t); t->sugar = mr->dl->td;
			addd(CO,d,t,&s); d = s;
			dp_rest(g,&t); g = t;
				
		}
	}
	if ( d )
		d->sugar = sugar;
	*rp = d;
}

/* nf computation over a field */

void dp_nf_f(b,g,ps,full,rp)
NODE b;
DP g;
DP *ps;
int full;
DP *rp;
{
	DP u,p,d,s,t;
	P dmy;
	NODE l;
	MP m,mr;
	int i,n;
	int *wb;
	int sugar,psugar;

	if ( !g ) {
		*rp = 0; return;
	}
	for ( n = 0, l = b; l; l = NEXT(l), n++ );
	wb = (int *)ALLOCA(n*sizeof(int));
	for ( i = 0, l = b; i < n; l = NEXT(l), i++ )
		wb[i] = QTOS((Q)BDY(l));

	sugar = g->sugar;
	for ( d = 0; g; ) {
		for ( u = 0, i = 0; i < n; i++ ) {
			if ( dp_redble(g,p = ps[wb[i]]) ) {
				dp_red_f(g,p,&u);
				psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
				sugar = MAX(sugar,psugar);
				if ( !u ) {
					if ( d )
						d->sugar = sugar;
					*rp = d; return;
				}
				break;
			}
		}
		if ( u )
			g = u;
		else if ( !full ) {
			if ( g ) {
				MKDP(g->nv,BDY(g),t); t->sugar = sugar; g = t;
			}
			*rp = g; return;
		} else {
			m = BDY(g); NEWMP(mr); mr->dl = m->dl; mr->c = m->c;
			NEXT(mr) = 0; MKDP(g->nv,mr,t); t->sugar = mr->dl->td;
			addd(CO,d,t,&s); d = s;
			dp_rest(g,&t); g = t;
		}
	}
	if ( d )
		d->sugar = sugar;
	*rp = d;
}

/* nf computation over GF(mod) (only for internal use) */

void dp_nf_mod(b,g,ps,mod,full,rp)
NODE b;
DP g;
DP *ps;
int mod,full;
DP *rp;
{
	DP u,p,d,s,t;
	P dmy;
	NODE l;
	MP m,mr;
	int sugar,psugar;

	if ( !g ) {
		*rp = 0; return;
	}
	sugar = g->sugar;
	for ( d = 0; g; ) {
		for ( u = 0, l = b; l; l = NEXT(l) ) {
			if ( dp_redble(g,p = ps[(int)BDY(l)]) ) {
				dp_red_mod(d,g,p,mod,&t,&u,&dmy);
				psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
				sugar = MAX(sugar,psugar);
				if ( !u ) {
					if ( d )
						d->sugar = sugar;
					*rp = d; return;
				}
				d = t;
				break;
			}
		}
		if ( u )
			g = u;
		else if ( !full ) {
			if ( g ) {
				MKDP(g->nv,BDY(g),t); t->sugar = sugar; g = t;
			}
			*rp = g; return;
		} else {
			m = BDY(g); NEWMP(mr); mr->dl = m->dl; mr->c = m->c;
			NEXT(mr) = 0; MKDP(g->nv,mr,t); t->sugar = mr->dl->td;
			addmd(CO,mod,d,t,&s); d = s;
			dp_rest(g,&t); g = t;
		}
	}
	if ( d )
		d->sugar = sugar;
	*rp = d;
}

void dp_true_nf_mod(b,g,ps,mod,full,rp,dnp)
NODE b;
DP g;
DP *ps;
int mod,full;
DP *rp;
P *dnp;
{
	DP u,p,d,s,t;
	NODE l;
	MP m,mr;
	int i,n;
	int *wb;
	int sugar,psugar;
	P dn,tdn,tdn1;

	dn = (P)ONEM;
	if ( !g ) {
		*rp = 0; *dnp = dn; return;
	}
	for ( n = 0, l = b; l; l = NEXT(l), n++ );
		wb = (int *)ALLOCA(n*sizeof(int));
	for ( i = 0, l = b; i < n; l = NEXT(l), i++ )
		wb[i] = QTOS((Q)BDY(l));
	sugar = g->sugar;
	for ( d = 0; g; ) {
		for ( u = 0, i = 0; i < n; i++ ) {
			if ( dp_redble(g,p = ps[wb[i]]) ) {
				dp_red_mod(d,g,p,mod,&t,&u,&tdn);	
				psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
				sugar = MAX(sugar,psugar);
				if ( !u ) {
					if ( d )
						d->sugar = sugar;
					*rp = d; *dnp = dn; return;
				} else {
					d = t;
					mulmp(CO,mod,dn,tdn,&tdn1); dn = tdn1;
				}
				break;
			}
		}
		if ( u )
			g = u;
		else if ( !full ) {
			if ( g ) {
				MKDP(g->nv,BDY(g),t); t->sugar = sugar; g = t;
			}
			*rp = g; *dnp = dn; return;
		} else {
			m = BDY(g); NEWMP(mr); mr->dl = m->dl; mr->c = m->c;
			NEXT(mr) = 0; MKDP(g->nv,mr,t); t->sugar = mr->dl->td;
			addmd(CO,mod,d,t,&s); d = s;
			dp_rest(g,&t); g = t;
		}
	}
	if ( d )
		d->sugar = sugar;
	*rp = d; *dnp = dn;
}

void _dp_nf_mod_destructive(b,g,ps,mod,full,rp)
NODE b;
DP g;
DP *ps;
int mod,full;
DP *rp;
{
	DP u,p,d,s,t;
	NODE l;
	MP m,mr,mrd;
	int sugar,psugar,n,h_reducible,i;

	if ( !g ) {
		*rp = 0; return;
	}
	sugar = g->sugar;
	n = g->nv;
	for ( d = 0; g; ) {
		for ( h_reducible = 0, l = b; l; l = NEXT(l) ) {
			if ( dp_redble(g,p = ps[(int)BDY(l)]) ) {
				h_reducible = 1;
				psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
				_dp_red_mod_destructive(g,p,mod,&u); g = u;
				sugar = MAX(sugar,psugar);
				if ( !g ) {
					if ( d )
						d->sugar = sugar;
					_dptodp(d,rp); _free_dp(d); return;
				}
				break;
			}
		}
		if ( !h_reducible ) {
			/* head term is not reducible */
			if ( !full ) {
				if ( g )
					g->sugar = sugar;
				_dptodp(g,rp); _free_dp(g); return;
			} else {
				m = BDY(g); 
				if ( NEXT(m) ) {
					BDY(g) = NEXT(m); NEXT(m) = 0;
				} else {
					_FREEDP(g); g = 0;
				}
				if ( d ) {
					for ( mrd = BDY(d); NEXT(mrd); mrd = NEXT(mrd) );
					NEXT(mrd) = m;
				} else {
					_MKDP(n,m,d);
				}
			}
		}
	}
	if ( d )
		d->sugar = sugar;
	_dptodp(d,rp); _free_dp(d);
}

/* reduction by linear base over a field */

void dp_lnf_f(p1,p2,g,r1p,r2p)
DP p1,p2;
NODE g;
DP *r1p,*r2p;
{
	DP r1,r2,b1,b2,t,s;
	Obj c,c1,c2;
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
				divr(CO,(Obj)ONE,(Obj)BDY(b1)->c,&c1);
				mulr(CO,c1,(Obj)BDY(r1)->c,&c2); chsgnr(c2,&c);
				muldc(CO,b1,(P)c,&t); addd(CO,r1,t,&s); r1 = s;
				muldc(CO,b2,(P)c,&t); addd(CO,r2,t,&s); r2 = s;
			}
	}
	*r1p = r1; *r2p = r2;
}

/* reduction by linear base over GF(mod) */

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

/*
 * setting flags
 *
 */

int create_order_spec(obj,spec)
Obj obj;
struct order_spec *spec;
{
	int i,j,n,s,row,col;
	struct order_pair *l;
	NODE node,t,tn;
	MAT m;
	pointer **b;
	int **w;

	if ( !obj || NUM(obj) ) {
		spec->id = 0; spec->obj = obj;
		spec->ord.simple = QTOS((Q)obj);
		return 1;
	} else if ( OID(obj) == O_LIST ) {
		node = BDY((LIST)obj); 
		for ( n = 0, t = node; t; t = NEXT(t), n++ );
		l = (struct order_pair *)MALLOC_ATOMIC(n*sizeof(struct order_pair));
		for ( i = 0, t = node, s = 0; i < n; t = NEXT(t), i++ ) {
			tn = BDY((LIST)BDY(t)); l[i].order = QTOS((Q)BDY(tn));
			tn = NEXT(tn); l[i].length = QTOS((Q)BDY(tn));
			s += l[i].length;
		}
		spec->id = 1; spec->obj = obj;
		spec->ord.block.order_pair = l;
		spec->ord.block.length = n; spec->nv = s;
		return 1;
	} else if ( OID(obj) == O_MAT ) {
		m = (MAT)obj; row = m->row; col = m->col; b = BDY(m);
		w = almat(row,col);
		for ( i = 0; i < row; i++ )
			for ( j = 0; j < col; j++ )
				w[i][j] = QTOS((Q)b[i][j]);
		spec->id = 2; spec->obj = obj;
		spec->nv = col; spec->ord.matrix.row = row;
		spec->ord.matrix.matrix = w;
		return 1;
	} else
		return 0;
}

/*
 * converters
 *
 */

void dp_homo(p,rp)
DP p;
DP *rp;
{
	MP m,mr,mr0;
	int i,n,nv,td;
	DL dl,dlh;

	if ( !p )
		*rp = 0;
	else {
		n = p->nv; nv = n + 1;
		m = BDY(p); td = sugard(m);
		for ( mr0 = 0; m; m = NEXT(m) ) {
			NEXTMP(mr0,mr); mr->c = m->c;
			dl = m->dl;
			mr->dl = dlh = (DL)MALLOC_ATOMIC((nv+1)*sizeof(int));
			dlh->td = td;
			for ( i = 0; i < n; i++ )
				dlh->d[i] = dl->d[i];
			dlh->d[n] = td - dl->td;
		}
		NEXT(mr) = 0; MKDP(nv,mr0,*rp); (*rp)->sugar = p->sugar;
	}
}

void dp_dehomo(p,rp)
DP p;
DP *rp;
{
	MP m,mr,mr0;
	int i,n,nv;
	DL dl,dlh;

	if ( !p )
		*rp = 0;
	else {
		n = p->nv; nv = n - 1;
		m = BDY(p);
		for ( mr0 = 0; m; m = NEXT(m) ) {
			NEXTMP(mr0,mr); mr->c = m->c;
			dlh = m->dl;
			mr->dl = dl = (DL)MALLOC_ATOMIC((nv+1)*sizeof(int));
			dl->td = dlh->td - dlh->d[nv];
			for ( i = 0; i < nv; i++ )
				dl->d[i] = dlh->d[i];
		}
		NEXT(mr) = 0; MKDP(nv,mr0,*rp); (*rp)->sugar = p->sugar;
	}
}

void dp_mod(p,mod,subst,rp)
DP p;
int mod;
NODE subst;
DP *rp;
{
	MP m,mr,mr0;
	P t,s,s1;
	V v;
	NODE tn;

	if ( !p )
		*rp = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			for ( tn = subst, s = m->c; tn; tn = NEXT(tn) ) {
				v = VR((P)BDY(tn)); tn = NEXT(tn);
				substp(CO,s,v,(P)BDY(tn),&s1); s = s1;
			}
			ptomp(mod,s,&t);
			if ( t ) {
				NEXTMP(mr0,mr); mr->c = t; mr->dl = m->dl;
			}
		}
		if ( mr0 ) {
			NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
		} else
			*rp = 0;
	}
}

void dp_rat(p,rp)
DP p;
DP *rp;
{
	MP m,mr,mr0;

	if ( !p )
		*rp = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTMP(mr0,mr); mptop(m->c,&mr->c); mr->dl = m->dl;
		}
		if ( mr0 ) {
			NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
		} else
			*rp = 0;
	}
}


void homogenize_order(old,n,new)
struct order_spec *old,*new;
int n;
{
	struct order_pair *l;
	int length,nv,row,i,j;
	int **newm,**oldm;

	switch ( old->id ) {
		case 0:
			switch ( old->ord.simple ) {
				case 0:
					new->id = 0; new->ord.simple = 0; break;
				case 1:
					l = (struct order_pair *)
						MALLOC_ATOMIC(2*sizeof(struct order_pair));
					l[0].length = n; l[0].order = 1;
					l[1].length = 1; l[1].order = 2;
					new->id = 1;
					new->ord.block.order_pair = l;
					new->ord.block.length = 2; new->nv = n+1;
					break;
				case 2:
					new->id = 0; new->ord.simple = 1; break;
				case 3: case 4: case 5:
					new->id = 0; new->ord.simple = old->ord.simple+3;
					dp_nelim = n-1; break;
				case 6: case 7: case 8: case 9:
					new->id = 0; new->ord.simple = old->ord.simple; break;
				default:
					error("homogenize_order : invalid input");
			}
			break;
		case 1:
			length = old->ord.block.length;
			l = (struct order_pair *)
				MALLOC_ATOMIC((length+1)*sizeof(struct order_pair));
			bcopy((char *)old->ord.block.order_pair,(char *)l,length*sizeof(struct order_pair));
			l[length].order = 2; l[length].length = 1;
			new->id = 1; new->nv = n+1;
			new->ord.block.order_pair = l;
			new->ord.block.length = length+1;
			break;
		case 2:
			nv = old->nv; row = old->ord.matrix.row;
			oldm = old->ord.matrix.matrix; newm = almat(row+1,nv+1);
			for ( i = 0; i <= nv; i++ )
				newm[0][i] = 1;
			for ( i = 0; i < row; i++ ) {
				for ( j = 0; j < nv; j++ )
					newm[i+1][j] = oldm[i][j];
				newm[i+1][j] = 0;
			}
			new->id = 2; new->nv = nv+1;
			new->ord.matrix.row = row+1; new->ord.matrix.matrix = newm;
			break;
		default:
			error("homogenize_order : invalid input");
	}
}

void qltozl(w,n,dvr)
Q *w,*dvr;
int n;
{
	N nm,dn;
	N g,l1,l2,l3;
	Q c,d;
	int i;
	struct oVECT v;

	for ( i = 0; i < n; i++ )
		if ( w[i] && !INT(w[i]) )
			break;
	if ( i == n ) {
		v.id = O_VECT; v.len = n; v.body = (pointer *)w;
		igcdv(&v,dvr); return;
	}
	c = w[0]; nm = NM(c); dn = INT(c) ? ONEN : DN(c);
	for ( i = 1; i < n; i++ ) {
		c = w[i]; l1 = INT(c) ? ONEN : DN(c);
		gcdn(nm,NM(c),&g); nm = g;
		gcdn(dn,l1,&l2); muln(dn,l1,&l3); divsn(l3,l2,&dn);
	}
	if ( UNIN(dn) )
		NTOQ(nm,1,d);
	else 
		NDTOQ(nm,dn,1,d);
	*dvr = d;
}

int comp_nm(a,b)
Q *a,*b;
{
	return cmpn((*a)?NM(*a):0,(*b)?NM(*b):0);
}

void sortbynm(w,n)
Q *w;
int n;
{
	qsort(w,n,sizeof(Q),(int (*)(const void *,const void *))comp_nm);
}


/*
 * simple operations
 *
 */

int dp_redble(p1,p2)
DP p1,p2;
{
	int i,n;
	DL d1,d2;

	d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
	if ( d1->td < d2->td )
		return 0;
	else {
		for ( i = 0, n = p1->nv; i < n; i++ )
			if ( d1->d[i] < d2->d[i] )
				return 0;
		return 1;
	}
}

void dp_subd(p1,p2,rp)
DP p1,p2;
DP *rp;
{
	int i,n;
	DL d1,d2,d;
	MP m;
	DP s;

	n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
	NEWDL(d,n); d->td = d1->td - d2->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = d1->d[i]-d2->d[i];
	NEWMP(m); m->dl = d; m->c = (P)ONE; NEXT(m) = 0;
	MKDP(n,m,s); s->sugar = d->td;
	*rp = s;
}

void dltod(d,n,rp)
DL d;
int n;
DP *rp;
{
	MP m;
	DP s;

	NEWMP(m); m->dl = d; m->c = (P)ONE; NEXT(m) = 0;
	MKDP(n,m,s); s->sugar = d->td;
	*rp = s;
}

void dp_hm(p,rp)
DP p;
DP *rp;
{
	MP m,mr;

	if ( !p )
		*rp = 0;
	else {
		m = BDY(p);
		NEWMP(mr); mr->dl = m->dl; mr->c = m->c; NEXT(mr) = 0;
		MKDP(p->nv,mr,*rp); (*rp)->sugar = mr->dl->td; 	/* XXX */
	}
}

void dp_rest(p,rp)
DP p,*rp;
{
	MP m;

	m = BDY(p);
	if ( !NEXT(m) )
		*rp = 0;
	else {
		MKDP(p->nv,NEXT(m),*rp);
		if ( *rp )
			(*rp)->sugar = p->sugar;
	}
}

DL lcm_of_DL(nv,dl1,dl2,dl)
int nv;
DL dl1,dl2;
register DL dl;
{
	register int n, *d1, *d2, *d, td;

	if ( !dl ) NEWDL(dl,nv);
	d = dl->d,  d1 = dl1->d,  d2 = dl2->d;
	for ( td = 0, n = nv; --n >= 0; d1++, d2++, d++ )
		td += (*d = *d1 > *d2 ? *d1 : *d2 );
	dl->td = td;
	return dl;
}

int dl_equal(nv,dl1,dl2)
int nv;
DL dl1, dl2;
{
    register int *d1, *d2, n;

    if ( dl1->td != dl2->td ) return 0;
    for ( d1 = dl1->d, d2 = dl2->d, n = nv; --n >= 0; d1++, d2++ )
        if ( *d1 != *d2 ) return 0;
    return 1;
}

int dp_nt(p)
DP p;
{
	int i;
	MP m;

	if ( !p )
		return 0;
	else {
		for ( i = 0, m = BDY(p); m; m = NEXT(m), i++ );
		return i;
	}
}

int dp_homogeneous(p)
DP p;
{
	MP m;
	int d;

	if ( !p )
		return 1;
	else {
		m = BDY(p);
		d = m->dl->td;
		m = NEXT(m);
		for ( ; m; m = NEXT(m) ) {
			if ( m->dl->td != d )
				return 0;
		}
		return 1;
	}
}

_print_mp(nv,m)
int nv;
MP m;
{
	int i;

	if ( m )
		return;
	for ( ; m; m = NEXT(m) ) {
		fprintf(stderr,"%d<",ITOS(C(m)));
		for ( i = 0; i < nv; i++ ) {
			fprintf(stderr,"%d",m->dl->d[i]);
			if ( i != nv-1 )
				fprintf(stderr," ");
		}
		fprintf(stderr,">",C(m));
	}
	fprintf(stderr,"\n");
}
