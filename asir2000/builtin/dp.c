/* $OpenXM: OpenXM_contrib2/asir2000/builtin/dp.c,v 1.3 2000/05/29 08:54:45 noro Exp $ */
#include "ca.h"
#include "base.h"
#include "parse.h"

extern int dp_fcoeffs;

void Pdp_ord(), Pdp_ptod(), Pdp_dtop();
void Pdp_ptozp(), Pdp_ptozp2(), Pdp_red(), Pdp_red2(), Pdp_lcm(), Pdp_redble();
void Pdp_sp(), Pdp_hm(), Pdp_ht(), Pdp_hc(), Pdp_rest(), Pdp_td(), Pdp_sugar();
void Pdp_cri1(),Pdp_cri2(),Pdp_subd(),Pdp_mod(),Pdp_red_mod(),Pdp_tdiv();
void Pdp_prim(),Pdp_red_coef(),Pdp_mag(),Pdp_set_kara(),Pdp_rat();
void Pdp_nf(),Pdp_true_nf(),Pdp_nf_ptozp();
void Pdp_nf_mod(),Pdp_true_nf_mod();
void Pdp_criB(),Pdp_nelim();
void Pdp_minp(),Pdp_nf_demand(),Pdp_sp_mod();
void Pdp_homo(),Pdp_dehomo();
void Pdp_gr_mod_main();
void Pdp_gr_main(),Pdp_gr_hm_main(),Pdp_gr_d_main(),Pdp_gr_flags();
void Pdp_f4_main(),Pdp_f4_mod_main();
void Pdp_gr_print();

struct ftab dp_tab[] = {
	{"dp_ord",Pdp_ord,-1},
	{"dp_ptod",Pdp_ptod,2},
	{"dp_dtop",Pdp_dtop,2},
	{"dp_ptozp",Pdp_ptozp,1},
	{"dp_ptozp2",Pdp_ptozp2,2},
	{"dp_prim",Pdp_prim,1},
	{"dp_redble",Pdp_redble,2},
	{"dp_subd",Pdp_subd,2},
	{"dp_red",Pdp_red,3},
	{"dp_red_mod",Pdp_red_mod,4},
	{"dp_sp",Pdp_sp,2},
	{"dp_sp_mod",Pdp_sp_mod,3},
	{"dp_lcm",Pdp_lcm,2},
	{"dp_hm",Pdp_hm,1},
	{"dp_ht",Pdp_ht,1},
	{"dp_hc",Pdp_hc,1},
	{"dp_rest",Pdp_rest,1},
	{"dp_td",Pdp_td,1},
	{"dp_sugar",Pdp_sugar,1},
	{"dp_cri1",Pdp_cri1,2},
	{"dp_cri2",Pdp_cri2,2},
	{"dp_criB",Pdp_criB,3},
	{"dp_minp",Pdp_minp,2},
	{"dp_mod",Pdp_mod,3},
	{"dp_rat",Pdp_rat,1},
	{"dp_tdiv",Pdp_tdiv,2},
	{"dp_red_coef",Pdp_red_coef,2},
	{"dp_nelim",Pdp_nelim,-1},
	{"dp_mag",Pdp_mag,1},
	{"dp_set_kara",Pdp_set_kara,-1},
	{"dp_nf",Pdp_nf,4},
	{"dp_true_nf",Pdp_true_nf,4},
	{"dp_nf_ptozp",Pdp_nf_ptozp,5},
	{"dp_nf_demand",Pdp_nf_demand,5},
	{"dp_nf_mod",Pdp_nf_mod,5},
	{"dp_true_nf_mod",Pdp_true_nf_mod,5},
	{"dp_homo",Pdp_homo,1},
	{"dp_dehomo",Pdp_dehomo,1},
	{"dp_gr_main",Pdp_gr_main,5},
/*	{"dp_gr_hm_main",Pdp_gr_hm_main,5}, */
/*	{"dp_gr_d_main",Pdp_gr_d_main,6}, */
	{"dp_gr_mod_main",Pdp_gr_mod_main,5},
	{"dp_f4_main",Pdp_f4_main,3},
	{"dp_f4_mod_main",Pdp_f4_mod_main,4},
	{"dp_gr_flags",Pdp_gr_flags,-1},
	{"dp_gr_print",Pdp_gr_print,-1},
	{0,0,0},
};

extern int dp_nelim;
extern int dp_order_pair_length;
extern struct order_pair *dp_order_pair;
extern struct order_spec dp_current_spec;

void Pdp_ord(arg,rp)
NODE arg;
Obj *rp;
{
	struct order_spec spec;

	if ( !arg )
		*rp = dp_current_spec.obj;
	else if ( !create_order_spec((Obj)ARG0(arg),&spec) )
		error("dp_ord : invalid order specification");
	else {
		initd(&spec); *rp = spec.obj;
	}
}

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

void Pdp_ptod(arg,rp)
NODE arg;
DP *rp;
{
	NODE n;
	VL vl,tvl;

	asir_assert(ARG0(arg),O_P,"dp_ptod");
	asir_assert(ARG1(arg),O_LIST,"dp_ptod");
	for ( vl = 0, n = BDY((LIST)ARG1(arg)); n; n = NEXT(n) ) {
		if ( !vl ) {
			NEWVL(vl); tvl = vl;
		} else {
			NEWVL(NEXT(tvl)); tvl = NEXT(tvl);
		}
		VR(tvl) = VR((P)BDY(n));
	}
	if ( vl )
		NEXT(tvl) = 0;
	ptod(CO,vl,(P)ARG0(arg),rp);
}

void Pdp_dtop(arg,rp)
NODE arg;
P *rp;
{
	NODE n;
	VL vl,tvl;

	asir_assert(ARG0(arg),O_DP,"dp_dtop");
	asir_assert(ARG1(arg),O_LIST,"dp_dtop");
	for ( vl = 0, n = BDY((LIST)ARG1(arg)); n; n = NEXT(n) ) {
		if ( !vl ) {
			NEWVL(vl); tvl = vl;
		} else {
			NEWVL(NEXT(tvl)); tvl = NEXT(tvl);
		}
		VR(tvl) = VR((P)BDY(n));
	}
	if ( vl )
		NEXT(tvl) = 0;
	dtop(CO,vl,(DP)ARG0(arg),rp);
}

extern LIST Dist;

void Pdp_ptozp(arg,rp)
NODE arg;
DP *rp;
{
	asir_assert(ARG0(arg),O_DP,"dp_ptozp");
#if INET
	if ( Dist )
		dp_ptozp_d(BDY(Dist),length(BDY(Dist)),(DP)ARG0(arg),rp);
	else
#endif
		dp_ptozp((DP)ARG0(arg),rp);
}
	
void Pdp_ptozp2(arg,rp)
NODE arg;
LIST *rp;
{
	DP p0,p1,h,r;
	NODE n0;

	p0 = (DP)ARG0(arg); p1 = (DP)ARG1(arg);
	asir_assert(p0,O_DP,"dp_ptozp2");
	asir_assert(p1,O_DP,"dp_ptozp2");
#if INET
	if ( Dist )
		dp_ptozp2_d(BDY(Dist),length(BDY(Dist)),p0,p1,&h,&r);
	else
#endif
		dp_ptozp2(p0,p1,&h,&r);
	NEWNODE(n0); BDY(n0) = (pointer)h;
	NEWNODE(NEXT(n0)); BDY(NEXT(n0)) = (pointer)r;
	NEXT(NEXT(n0)) = 0;
	MKLIST(*rp,n0);
}

void Pdp_prim(arg,rp)
NODE arg;
DP *rp;
{
	DP t;

	asir_assert(ARG0(arg),O_DP,"dp_prim");
	dp_prim((DP)ARG0(arg),&t); dp_ptozp(t,rp);
}
	
extern int NoGCD;

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

void Pdp_mod(arg,rp)
NODE arg;
DP *rp;
{
	DP p;
	int mod;
	NODE subst;

	asir_assert(ARG0(arg),O_DP,"dp_mod");
	asir_assert(ARG1(arg),O_N,"dp_mod");
	asir_assert(ARG2(arg),O_LIST,"dp_mod");
	p = (DP)ARG0(arg); mod = QTOS((Q)ARG1(arg));
	subst = BDY((LIST)ARG2(arg));
	dp_mod(p,mod,subst,rp);
}

void Pdp_rat(arg,rp)
NODE arg;
DP *rp;
{
	asir_assert(ARG0(arg),O_DP,"dp_rat");
	dp_rat((DP)ARG0(arg),rp);
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

void Pdp_nf(arg,rp)
NODE arg;
DP *rp;
{
	NODE b;
	DP *ps;
	DP g;
	int full;

	asir_assert(ARG0(arg),O_LIST,"dp_nf");
	asir_assert(ARG1(arg),O_DP,"dp_nf");
	asir_assert(ARG2(arg),O_VECT,"dp_nf");
	asir_assert(ARG3(arg),O_N,"dp_nf");
	if ( !(g = (DP)ARG1(arg)) ) {
		*rp = 0; return;
	}
	b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
	full = (Q)ARG3(arg) ? 1 : 0;
	dp_nf(b,g,ps,full,rp);
}

void Pdp_true_nf(arg,rp)
NODE arg;
LIST *rp;
{
	NODE b,n;
	DP *ps;
	DP g;
	DP nm;
	P dn;
	int full;

	asir_assert(ARG0(arg),O_LIST,"dp_true_nf");
	asir_assert(ARG1(arg),O_DP,"dp_true_nf");
	asir_assert(ARG2(arg),O_VECT,"dp_true_nf");
	asir_assert(ARG3(arg),O_N,"dp_nf");
	if ( !(g = (DP)ARG1(arg)) ) {
		nm = 0; dn = (P)ONE;
	} else {
		b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
		full = (Q)ARG3(arg) ? 1 : 0;
		dp_true_nf(b,g,ps,full,&nm,&dn);
	}
	NEWNODE(n); BDY(n) = (pointer)nm;
	NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)dn;
	NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

void dp_nf(b,g,ps,full,rp)
NODE b;
DP g;
DP *ps;
int full;
DP *rp;
{
	DP u,p,d,s,t,dmy1;
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

#define HMAG(p) (p_mag(BDY(p)->c))

void Pdp_nf_ptozp(arg,rp)
NODE arg;
DP *rp;
{
	NODE b;
	DP g;
	DP *ps;
	int full,multiple;

	asir_assert(ARG0(arg),O_LIST,"dp_nf_ptozp");
	asir_assert(ARG1(arg),O_DP,"dp_nf_ptozp");
	asir_assert(ARG2(arg),O_VECT,"dp_nf_ptozp");
	asir_assert(ARG3(arg),O_N,"dp_nf_ptozp");
	asir_assert(ARG4(arg),O_N,"dp_nf_ptozp");
	if ( !(g = (DP)ARG1(arg)) ) {
		*rp = 0; return;
	}
	b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
	full = (Q)ARG3(arg) ? 1 : 0;
	multiple = QTOS((Q)ARG4(arg));
	dp_nf_ptozp(b,g,ps,full,multiple,rp);
}

void dp_nf_ptozp(b,g,ps,full,multiple,rp)
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
				if ( HMAG(d) > hmag ) {
					dp_ptozp2(d,g,&t,&u); d = t; g = u;
					hmag = multiple*HMAG(d);
				}
			} else {
				if ( HMAG(g) > hmag ) {
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

void Pdp_nf_demand(arg,rp)
NODE arg;
DP *rp;
{
	DP g,u,p,d,s,t,dmy1;
	P dmy;
	NODE b,l;
	DP *hps;
	MP m,mr;
	int i,n;
	int *wb;
	int full;
	char *fprefix;
	int sugar,psugar;

	asir_assert(ARG0(arg),O_LIST,"dp_nf_demand");
	asir_assert(ARG1(arg),O_DP,"dp_nf_demand");
	asir_assert(ARG2(arg),O_N,"dp_nf_demand");
	asir_assert(ARG3(arg),O_VECT,"dp_nf_demand");
	asir_assert(ARG4(arg),O_STR,"dp_nf_demand");
	if ( !(g = (DP)ARG1(arg)) ) {
		*rp = 0; return;
	}
	b = BDY((LIST)ARG0(arg)); full = (Q)ARG2(arg) ? 1 : 0;
	hps = (DP *)BDY((VECT)ARG3(arg)); fprefix = BDY((STRING)ARG4(arg));
	for ( n = 0, l = b; l; l = NEXT(l), n++ );
	wb = (int *)ALLOCA(n*sizeof(int));
	for ( i = 0, l = b; i < n; l = NEXT(l), i++ )
		wb[i] = QTOS((Q)BDY(l));
	sugar = g->sugar;
	for ( d = 0; g; ) {
		for ( u = 0, i = 0; i < n; i++ ) {
			if ( dp_redble(g,hps[wb[i]]) ) {
				FILE *fp;
				char fname[BUFSIZ];

				sprintf(fname,"%s%d",fprefix,wb[i]);
				fprintf(stderr,"loading %s\n",fname);
				fp = fopen(fname,"r"); skipvl(fp);						
				loadobj(fp,(Obj *)&p); fclose(fp);
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

void Pdp_nf_mod(arg,rp)
NODE arg;
DP *rp;
{
	NODE b;
	DP g;
	DP *ps;
	int mod,full,ac;

	ac = argc(arg);
	asir_assert(ARG0(arg),O_LIST,"dp_nf_mod");
	asir_assert(ARG1(arg),O_DP,"dp_nf_mod");
	asir_assert(ARG2(arg),O_VECT,"dp_nf_mod");
	asir_assert(ARG3(arg),O_N,"dp_nf_mod");
	asir_assert(ARG4(arg),O_N,"dp_nf_mod");
	if ( !(g = (DP)ARG1(arg)) ) {
		*rp = 0; return;
	}
	b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
	full = QTOS((Q)ARG3(arg)); mod = QTOS((Q)ARG4(arg));
	dp_nf_mod_qindex(b,g,ps,mod,full,rp);
}

void Pdp_true_nf_mod(arg,rp)
NODE arg;
LIST *rp;
{
	NODE b;
	DP g,nm;
	P dn;
	DP *ps;
	int mod,full;
	NODE n;

	asir_assert(ARG0(arg),O_LIST,"dp_nf_mod");
	asir_assert(ARG1(arg),O_DP,"dp_nf_mod");
	asir_assert(ARG2(arg),O_VECT,"dp_nf_mod");
	asir_assert(ARG3(arg),O_N,"dp_nf_mod");
	asir_assert(ARG4(arg),O_N,"dp_nf_mod");
	if ( !(g = (DP)ARG1(arg)) ) {
		nm = 0; dn = (P)ONEM;
	} else {
		b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
		full = QTOS((Q)ARG3(arg)); mod = QTOS((Q)ARG4(arg));
		dp_true_nf_mod(b,g,ps,mod,full,&nm,&dn);
	}
	NEWNODE(n); BDY(n) = (pointer)nm;
	NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)dn;
	NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

void dp_nf_mod_qindex(b,g,ps,mod,full,rp)
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
			if ( dp_redble(g,p = ps[QTOS((Q)BDY(l))]) ) {
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

void Pdp_tdiv(arg,rp)
NODE arg;
DP *rp;
{
	MP m,mr,mr0;
	DP p;
	Q c;
	N d,q,r;
	int sgn;

	asir_assert(ARG0(arg),O_DP,"dp_tdiv");
	asir_assert(ARG1(arg),O_N,"dp_tdiv");
	p = (DP)ARG0(arg); d = NM((Q)ARG1(arg)); sgn = SGN((Q)ARG1(arg));
	if ( !p )
		*rp = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			divn(NM((Q)m->c),d,&q,&r);
			if ( r ) {
				*rp = 0; return;
			} else {
				NEXTMP(mr0,mr); NTOQ(q,SGN((Q)m->c)*sgn,c);
				mr->c = (P)c; mr->dl = m->dl;
			}
		}
		NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
	}
}

void Pdp_red_coef(arg,rp)
NODE arg;
DP *rp;
{
	MP m,mr,mr0;
	P q,r;
	DP p;
	P mod;

	p = (DP)ARG0(arg); mod = (P)ARG1(arg);
	asir_assert(p,O_DP,"dp_red_coef");
	asir_assert(mod,O_P,"dp_red_coef");
	if ( !p )
		*rp = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			divsrp(CO,m->c,mod,&q,&r);
			if ( r ) {
				NEXTMP(mr0,mr); mr->c = r; mr->dl = m->dl;
			}
		}
		if ( mr0 ) {
			NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
		} else
			*rp = 0;
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

void Pdp_redble(arg,rp)
NODE arg;
Q *rp;
{
	asir_assert(ARG0(arg),O_DP,"dp_redble");
	asir_assert(ARG1(arg),O_DP,"dp_redble");
	if ( dp_redble((DP)ARG0(arg),(DP)ARG1(arg)) )
		*rp = ONE;
	else
		*rp = 0;
}

void Pdp_red_mod(arg,rp)
NODE arg;
LIST *rp;
{
	DP h,r;
	P dmy;
	NODE n;

	asir_assert(ARG0(arg),O_DP,"dp_red_mod");
	asir_assert(ARG1(arg),O_DP,"dp_red_mod");
	asir_assert(ARG2(arg),O_DP,"dp_red_mod");
	asir_assert(ARG3(arg),O_N,"dp_red_mod");
	dp_red_mod((DP)ARG0(arg),(DP)ARG1(arg),(DP)ARG2(arg),QTOS((Q)ARG3(arg)),
		&h,&r,&dmy);
	NEWNODE(n); BDY(n) = (pointer)h;
	NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)r;
	NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

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
	MKDP(n,m,s); s->sugar = d->td; mulmd(CO,mod,p2,s,&t);
	if ( NUM(c2) ) {
		addmd(CO,mod,p1,t,&r); h = p0;
	} else {
		mulmdc(CO,mod,p1,c2,&s); addmd(CO,mod,s,t,&r); mulmdc(CO,mod,p0,c2,&h);
	}
	*head = h; *rest = r; *dnp = c2;
}

void Pdp_subd(arg,rp)
NODE arg;
DP *rp;
{
	DP p1,p2;

	p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
	asir_assert(p1,O_DP,"dp_subd");
	asir_assert(p2,O_DP,"dp_subd");
	dp_subd(p1,p2,rp);
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

void Pdp_red(arg,rp)
NODE arg;
LIST *rp;
{
	NODE n;
	DP head,rest,dmy1;
	P dmy;

	asir_assert(ARG0(arg),O_DP,"dp_red");
	asir_assert(ARG1(arg),O_DP,"dp_red");
	asir_assert(ARG2(arg),O_DP,"dp_red");
	dp_red((DP)ARG0(arg),(DP)ARG1(arg),(DP)ARG2(arg),&head,&rest,&dmy,&dmy1);
	NEWNODE(n); BDY(n) = (pointer)head;
	NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)rest;
	NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

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

void Pdp_sp(arg,rp)
NODE arg;
DP *rp;
{
	DP p1,p2;

	p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
	asir_assert(p1,O_DP,"dp_sp"); asir_assert(p2,O_DP,"dp_sp");
	dp_sp(p1,p2,rp);
}

extern int GenTrace;
extern NODE TraceList;

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

void Pdp_sp_mod(arg,rp)
NODE arg;
DP *rp;
{
	DP p1,p2;
	int mod;

	p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
	asir_assert(p1,O_DP,"dp_sp_mod"); asir_assert(p2,O_DP,"dp_sp_mod");
	asir_assert(ARG2(arg),O_N,"dp_sp_mod");
	mod = QTOS((Q)ARG2(arg));
	dp_sp_mod(p1,p2,mod,rp);
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

void Pdp_lcm(arg,rp)
NODE arg;
DP *rp;
{
	int i,n,td;
	DL d1,d2,d;
	MP m;
	DP p1,p2;

	p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
	asir_assert(p1,O_DP,"dp_lcm"); asir_assert(p2,O_DP,"dp_lcm");
	n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
	NEWDL(d,n);
	for ( i = 0, td = 0; i < n; i++ ) {
		d->d[i] = MAX(d1->d[i],d2->d[i]); td += d->d[i];
	}
	d->td = td;
	NEWMP(m); m->dl = d; m->c = (P)ONE; NEXT(m) = 0;
	MKDP(n,m,*rp); (*rp)->sugar = td;	/* XXX */
}

void Pdp_hm(arg,rp)
NODE arg;
DP *rp;
{
	DP p;

	p = (DP)ARG0(arg); asir_assert(p,O_DP,"dp_hm");
	dp_hm(p,rp);
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

void Pdp_ht(arg,rp)
NODE arg;
DP *rp;
{
	DP p;
	MP m,mr;

	p = (DP)ARG0(arg); asir_assert(p,O_DP,"dp_ht");
	if ( !p )
		*rp = 0;
	else {
		m = BDY(p);
		NEWMP(mr); mr->dl = m->dl; mr->c = (P)ONE; NEXT(mr) = 0;
		MKDP(p->nv,mr,*rp); (*rp)->sugar = mr->dl->td;	/* XXX */
	}
}

void Pdp_hc(arg,rp)
NODE arg;
P *rp;
{
	asir_assert(ARG0(arg),O_DP,"dp_hc");
	if ( !ARG0(arg) )
		*rp = 0;
	else
		*rp = BDY((DP)ARG0(arg))->c;
}

void Pdp_rest(arg,rp)
NODE arg;
DP *rp;
{
	asir_assert(ARG0(arg),O_DP,"dp_rest");
	if ( !ARG0(arg) )
		*rp = 0;
	else
		dp_rest((DP)ARG0(arg),rp);
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

void Pdp_td(arg,rp)
NODE arg;
Q *rp;
{
	DP p;

	p = (DP)ARG0(arg); asir_assert(p,O_DP,"dp_td");
	if ( !p )
		*rp = 0;
	else
		STOQ(BDY(p)->dl->td,*rp);
}

void Pdp_sugar(arg,rp)
NODE arg;
Q *rp;
{
	DP p;

	p = (DP)ARG0(arg); asir_assert(p,O_DP,"dp_sugar");
	if ( !p )
		*rp = 0;
	else
		STOQ(p->sugar,*rp);
}

void Pdp_cri1(arg,rp)
NODE arg;
Q *rp;
{
	DP p1,p2;
	int *d1,*d2;
	int i,n;

	p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
	asir_assert(p1,O_DP,"dp_cri1"); asir_assert(p2,O_DP,"dp_cri1");
	n = p1->nv; d1 = BDY(p1)->dl->d; d2 = BDY(p2)->dl->d;
	for ( i = 0; i < n; i++ )
		if ( d1[i] > d2[i] )
			break;
	*rp = i == n ? ONE : 0;
}

void Pdp_cri2(arg,rp)
NODE arg;
Q *rp;
{
	DP p1,p2;
	int *d1,*d2;
	int i,n;

	p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
	asir_assert(p1,O_DP,"dp_cri2"); asir_assert(p2,O_DP,"dp_cri2");
	n = p1->nv; d1 = BDY(p1)->dl->d; d2 = BDY(p2)->dl->d;
	for ( i = 0; i < n; i++ )
		if ( MIN(d1[i],d2[i]) >= 1 )
			break;
	*rp = i == n ? ONE : 0;
}

void Pdp_minp(arg,rp)
NODE arg;
LIST *rp;
{
	NODE tn,tn1,d,dd,dd0,p,tp;
	LIST l,minp;
	DP lcm,tlcm;
	int s,ts;

	asir_assert(ARG0(arg),O_LIST,"dp_minp");
	d = BDY((LIST)ARG0(arg)); minp = (LIST)BDY(d);
	p = BDY(minp); p = NEXT(NEXT(p)); lcm = (DP)BDY(p); p = NEXT(p);
	if ( !ARG1(arg) ) {
		s = QTOS((Q)BDY(p)); p = NEXT(p);
		for ( dd0 = 0, d = NEXT(d); d; d = NEXT(d) ) {
			tp = BDY((LIST)BDY(d)); tp = NEXT(NEXT(tp));
			tlcm = (DP)BDY(tp); tp = NEXT(tp);
			ts = QTOS((Q)BDY(tp)); tp = NEXT(tp);
			NEXTNODE(dd0,dd);
			if ( ts < s ) {
				BDY(dd) = (pointer)minp;
				minp = (LIST)BDY(d); lcm = tlcm; s = ts;
			} else if ( ts == s ) {
				if ( compd(CO,lcm,tlcm) > 0 ) {
					BDY(dd) = (pointer)minp;
					minp = (LIST)BDY(d); lcm = tlcm; s = ts;
				} else
					BDY(dd) = BDY(d);
			} else
				BDY(dd) = BDY(d);
		}
	} else {
		for ( dd0 = 0, d = NEXT(d); d; d = NEXT(d) ) {
			tp = BDY((LIST)BDY(d)); tp = NEXT(NEXT(tp));
			tlcm = (DP)BDY(tp);
			NEXTNODE(dd0,dd);
			if ( compd(CO,lcm,tlcm) > 0 ) {
				BDY(dd) = (pointer)minp; minp = (LIST)BDY(d); lcm = tlcm;
			} else
				BDY(dd) = BDY(d);
		}
	}
	if ( dd0 )
		NEXT(dd) = 0;
	MKLIST(l,dd0); MKNODE(tn,l,0); MKNODE(tn1,minp,tn); MKLIST(*rp,tn1);
}

void Pdp_criB(arg,rp)
NODE arg;
LIST *rp;
{
	NODE d,ij,dd,ddd;
	int i,j,s,n;
	DP *ps;
	DL ts,ti,tj,lij,tdl;

	asir_assert(ARG0(arg),O_LIST,"dp_criB"); d = BDY((LIST)ARG0(arg));
	asir_assert(ARG1(arg),O_N,"dp_criB"); s = QTOS((Q)ARG1(arg));
	asir_assert(ARG2(arg),O_VECT,"dp_criB"); ps = (DP *)BDY((VECT)ARG2(arg));
	if ( !d )
		*rp = (LIST)ARG0(arg);
	else {
		ts = BDY(ps[s])->dl;
		n = ps[s]->nv;
		NEWDL(tdl,n);
		for ( dd = 0; d; d = NEXT(d) ) {
			ij = BDY((LIST)BDY(d));
			i = QTOS((Q)BDY(ij)); ij = NEXT(ij);
			j = QTOS((Q)BDY(ij)); ij = NEXT(ij);
			lij = BDY((DP)BDY(ij))->dl;
			ti = BDY(ps[i])->dl; tj = BDY(ps[j])->dl;
			if ( lij->td != lcm_of_DL(n,lij,ts,tdl)->td
				|| !dl_equal(n,lij,tdl)
				|| (lij->td == lcm_of_DL(n,ti,ts,tdl)->td
					&& dl_equal(n,tdl,lij))
				|| (lij->td == lcm_of_DL(n,tj,ts,tdl)->td
					&& dl_equal(n,tdl,lij)) ) {
				MKNODE(ddd,BDY(d),dd);
				dd = ddd;
			}
		}
		MKLIST(*rp,dd);
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

void Pdp_nelim(arg,rp)
NODE arg;
Q *rp;
{
	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"dp_nelim");
		dp_nelim = QTOS((Q)ARG0(arg));
	}
	STOQ(dp_nelim,*rp);
}

void Pdp_mag(arg,rp)
NODE arg;
Q *rp;
{
	DP p;
	int s;
	MP m;

	p = (DP)ARG0(arg);
	asir_assert(p,O_DP,"dp_mag");
	if ( !p )
		*rp = 0;
	else {
		for ( s = 0, m = BDY(p); m; m = NEXT(m) )
			s += p_mag(m->c);
		STOQ(s,*rp);
	}
}

extern int kara_mag;

void Pdp_set_kara(arg,rp)
NODE arg;
Q *rp;
{
	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"dp_set_kara");
		kara_mag = QTOS((Q)ARG0(arg));
	}
	STOQ(kara_mag,*rp);
}

void Pdp_homo(arg,rp)
NODE arg;
DP *rp;
{
	asir_assert(ARG0(arg),O_DP,"dp_homo");
	dp_homo((DP)ARG0(arg),rp);
}

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

void Pdp_dehomo(arg,rp)
NODE arg;
DP *rp;
{
	asir_assert(ARG0(arg),O_DP,"dp_dehomo");
	dp_dehomo((DP)ARG0(arg),rp);
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
