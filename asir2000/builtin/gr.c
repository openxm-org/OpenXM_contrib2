/* $OpenXM: OpenXM/src/asir99/builtin/gr.c,v 1.2 1999/11/18 05:42:01 noro Exp $ */
#include "ca.h"
#include "parse.h"
#include "base.h"
#include "ox.h"

#define ITOS(p) (((unsigned int)(p))&0x7fffffff)
#define STOI(i) ((P)((unsigned int)(i)|0x80000000))

#define NEXTVL(r,c) \
if(!(r)){NEWVL(r);(c)=(r);}else{NEWVL(NEXT(c));(c)=NEXT(c);}

#define HMAG(p) (p_mag(BDY(p)->c))

struct dp_pairs {
	int dp1, dp2;
	DL lcm;
	int sugar;
	struct dp_pairs *next;
};

typedef struct dp_pairs *DP_pairs;

#define NEWDPP(a) ((a)=(DP_pairs)MALLOC(sizeof(struct dp_pairs)))

#define NEXTDPP(r,c) \
if(!(r)){NEWDPP(r);(c)=(r);}else{NEWDPP(NEXT(c));(c)=NEXT(c);}

struct oEGT eg_up,eg_sp,eg_spm,eg_nf,eg_nfm;
struct oEGT eg_znfm,eg_pz,eg_np,eg_mp,eg_ra,eg_mc,eg_gc;
int TP,NBP,NMP,NFP,NDP,ZR,NZR;

#define NEWDP_pairs ((DP_pairs)MALLOC(sizeof(struct dp_pairs)))

extern int (*cmpdl)();

void Pdp_gr_flags(), Pdp_gr_print();
void Pdp_gr_main(),Pdp_gr_mod_main(),Pdp_f4_main(),Pdp_f4_mod_main();

void print_stat(void);
void init_stat(void);
int dp_load_t(int,DP *);
void dp_load(int,DP *);
void dp_save(int,DP);
void dp_make_flaglist(LIST *);
void dp_set_flag(Obj,Obj);
int membercheck(NODE,NODE);
int gbcheck(NODE);
int dl_redble(DL,DL);
NODE remove_reducibles(NODE,int);
NODE updbase(NODE,int);
DP_pairs criterion_F(DP_pairs);
int criterion_2(int,int);
static DP_pairs collect_pairs_of_hdlcm(DP_pairs,DP_pairs *);
DP_pairs criterion_M(DP_pairs);
DP_pairs criterion_B(DP_pairs,int);
DP_pairs newpairs(NODE,int);
DP_pairs updpairs(DP_pairs,NODE,int);
void _dp_nf_ptozp(NODE,DP,DP *,int,int,DP *);
void _dp_nf_ptozp_mpi(NODE,DP,DP *,int,int,DP *);
void _dp_nf(NODE,DP,DP *,int,DP *);
void _dp_nf_mod(NODE,DP,DP *,int,int,DP *);
void interreduce_mod(NODE,int,int);
NODE gb_mod(NODE,int);
NODE gbd(NODE,int,NODE,NODE);
NODE gb(NODE,int,NODE);
NODE gb_f4(NODE);
NODE gb_f4_mod(NODE,int);
DP_pairs minp(DP_pairs, DP_pairs *);
void minsugar(DP_pairs,DP_pairs *,DP_pairs *);
NODE append_one(NODE,int);
void reducebase_dehomo(NODE,NODE *);
int newps_mod(DP,int);
int newps_nosave(DP,int,NODE);
int newps(DP,int,NODE);
void reduceall_mod(NODE,int,NODE *);
void reduceall(NODE,NODE *);
NODE NODE_sortbi(NODE,int);
NODE NODE_sortbi_insert(int, NODE,int);
NODE NODE_sortb(NODE,int);
NODE NODE_sortb_insert(DP,NODE,int);
void prim_part(DP,int,DP *);
void setup_arrays(NODE,int,NODE *);
int validhc(P,int,NODE);
void vlminus(VL,VL,VL *);
void printsubst(NODE);
void makesubst(VL,NODE *);
void pltovl(LIST,VL *);
void printdl(DL);
int DPPlength(DP_pairs);
void dp_gr_mod_main(LIST,LIST,Num,int,struct order_spec *,LIST *);
void dp_gr_main(LIST,LIST,Num,int,struct order_spec *,LIST *);
void dp_f4_main(LIST,LIST,struct order_spec *,LIST *);
void dp_f4_mod_main(LIST,LIST,int,struct order_spec *,LIST *);
double get_rtime();
void dp_symb(DP,DP *);
void _dpmod_to_vect(DP,DL *,int *);
void dp_to_vect(DP,DL *,Q *);

extern int dp_nelim;
extern int dp_fcoeffs;
static DP *ps,*psm;
static DL *psh;
static P *psc;

static int *pss;
static int psn,pslen;
static int NVars,CNVars,PCoeffs;
static VL VC;
static int NoSugar = 0;
static int NoCriB = 0;
static int NoGC = 0;
static int NoMC = 0;
static int NoRA = 0;
int Print = 0;
static int PrintShort = 0;
static int ShowMag = 0;
static int Stat = 0;
static int Multiple = 0;
static int Denominator = 1;
static int Top = 0;
static int Reverse = 0;
static int InterReduce = 0;
static int Max_mag = 0;
static char *Demand = 0;
static int PtozpRA = 0;
LIST Dist = 0;
int NoGCD = 0;
int doing_f4;

void Pdp_gr_flags(arg,rp)
NODE arg;
LIST *rp;
{
	Obj name,value;
	NODE n;

	if ( arg ) {
		asir_assert(ARG0(arg),O_LIST,"dp_gr_flags");
		n = BDY((LIST)ARG0(arg));
		while ( n ) {
			name = (Obj)BDY(n); n = NEXT(n);
			if ( !n )
				break;
			else {
				value = (Obj)BDY(n); n = NEXT(n);
			}
			dp_set_flag(name,value);
		}
	}
	dp_make_flaglist(rp);
}

void Pdp_gr_print(arg,rp)
NODE arg;
Q *rp;
{
	Q q;

	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"dp_gr_print");
		q = (Q)ARG0(arg); Print = QTOS(q);
	} else
		STOQ(Print,q);
	*rp = q;
}

void Pdp_gr_main(arg,rp)
NODE arg;
LIST *rp;
{
	LIST f,v;
	Num homo;
	Q m;
	int modular;
	struct order_spec ord;

	asir_assert(ARG0(arg),O_LIST,"dp_gr_main");
	asir_assert(ARG1(arg),O_LIST,"dp_gr_main");
	asir_assert(ARG2(arg),O_N,"dp_gr_main");
	asir_assert(ARG3(arg),O_N,"dp_gr_main");
	f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
	homo = (Num)ARG2(arg);
	m = (Q)ARG3(arg);
	if ( !m )
		modular = 0;
	else if ( PL(NM(m))>1 || (PL(NM(m)) == 1 && BD(NM(m))[0] >= 0x80000000) )
		error("dp_gr_main : too large modulus");
	else
		modular = QTOS(m);
	create_order_spec(ARG4(arg),&ord);
	dp_gr_main(f,v,homo,modular,&ord,rp);
}

void Pdp_f4_main(arg,rp)
NODE arg;
LIST *rp;
{
	LIST f,v;
	struct order_spec ord;

	asir_assert(ARG0(arg),O_LIST,"dp_f4_main");
	asir_assert(ARG1(arg),O_LIST,"dp_f4_main");
	f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
	create_order_spec(ARG2(arg),&ord);
	dp_f4_main(f,v,&ord,rp);
}

void Pdp_f4_mod_main(arg,rp)
NODE arg;
LIST *rp;
{
	LIST f,v;
	int m;
	struct order_spec ord;

	asir_assert(ARG0(arg),O_LIST,"dp_f4_main");
	asir_assert(ARG1(arg),O_LIST,"dp_f4_main");
	asir_assert(ARG2(arg),O_N,"dp_f4_main");
	f = (LIST)ARG0(arg); v = (LIST)ARG1(arg); m = QTOS((Q)ARG2(arg));
	create_order_spec(ARG3(arg),&ord);
	dp_f4_mod_main(f,v,m,&ord,rp);
}

void Pdp_gr_mod_main(arg,rp)
NODE arg;
LIST *rp;
{
	LIST f,v;
	Num homo;
	int m;
	struct order_spec ord;

	asir_assert(ARG0(arg),O_LIST,"dp_gr_mod_main");
	asir_assert(ARG1(arg),O_LIST,"dp_gr_mod_main");
	asir_assert(ARG2(arg),O_N,"dp_gr_mod_main");
	asir_assert(ARG3(arg),O_N,"dp_gr_mod_main");
	f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
	homo = (Num)ARG2(arg); m = QTOS((Q)ARG3(arg));
	create_order_spec(ARG4(arg),&ord);
	dp_gr_mod_main(f,v,homo,m,&ord,rp);
}

int eqdl(nv,dl1,dl2)
int nv;
DL dl1,dl2;
{
	int i;
	int *b1,*b2;

	if ( dl1->td != dl2->td )
		return 0;
	for ( i = 0, b1 = dl1->d, b2 = dl2->d; i < nv; i++ )
		if ( b1[i] != b2[i] )
			break;
	if ( i == nv )
		return 1;
	else
		return 0;
}

void _dpmod_to_vect(f,at,b)
DP f;
DL *at;
int *b;
{
	int i,nv;
	MP m;

	nv = f->nv;
	for ( m = BDY(f), i = 0; m; m = NEXT(m), i++ ) {
		for ( ; !eqdl(nv,m->dl,at[i]); i++ );
		b[i] = ITOS(m->c);
	}
}

void dp_to_vect(f,at,b)
DP f;
DL *at;
Q *b;
{
	int i,nv;
	MP m;

	nv = f->nv;
	for ( m = BDY(f), i = 0; m; m = NEXT(m), i++ ) {
		for ( ; !eqdl(nv,m->dl,at[i]); i++ );
		b[i] =(Q)m->c;
	}
}

void dp_symb(f,rp)
DP f;
DP *rp;
{
	MP mp,mp0,m;
	DP r;

	if ( !f ) {
		*rp = 0; return;
	}
	mp0 = 0;
	for ( m = BDY(f); m; m = NEXT(m) ) {
		NEXTMP(mp0,mp); mp->dl = m->dl; mp->c = (P)ONE;
	}
	NEXT(mp) = 0;
	MKDP(f->nv,mp0,r); r->sugar = f->sugar;
	*rp = r;
}

void dp_gr_main(f,v,homo,modular,ord,rp)
LIST f,v;
Num homo;
int modular;
struct order_spec *ord;
LIST *rp;
{
	int i,mindex,m,nochk;
	struct order_spec ord1;
	VL fv,vv,vc;
	NODE fd,fd0,fi,fi0,r,r0,t,subst,x,s,xx;

	mindex = 0; nochk = 0; dp_fcoeffs = 0;
	get_vars((Obj)f,&fv); pltovl(v,&vv); vlminus(fv,vv,&vc);
	NVars = length((NODE)vv); PCoeffs = vc ? 1 : 0; VC = vc;
	CNVars = homo ? NVars+1 : NVars;
	if ( ord->id && NVars != ord->nv )
		error("dp_gr_main : invalid order specification");
	initd(ord);
	if ( homo ) {
		homogenize_order(ord,NVars,&ord1);
		for ( fd0 = fi0 = 0, t = BDY(f); t; t = NEXT(t) ) {
			NEXTNODE(fd0,fd); NEXTNODE(fi0,fi);
			ptod(CO,vv,(P)BDY(t),(DP *)&BDY(fi)); dp_homo((DP)BDY(fi),(DP *)&BDY(fd));
		}
		if ( fd0 ) NEXT(fd) = 0;
		if ( fi0 ) NEXT(fi) = 0;
		initd(&ord1);
	} else {
		for ( fd0 = 0, t = BDY(f); t; t = NEXT(t) ) {
			NEXTNODE(fd0,fd); ptod(CO,vv,(P)BDY(t),(DP *)&BDY(fd));
		}
		if ( fd0 ) NEXT(fd) = 0;
		fi0 = fd0;
	}
	if ( modular < 0 ) {
		modular = -modular; nochk = 1;
	}
	if ( modular )
		m = modular > 1 ? modular : lprime[mindex];
	else
		m = 0;
	makesubst(vc,&subst);
	setup_arrays(fd0,0,&s);
	init_stat();
	while ( 1 ) {
		if ( homo ) {
			initd(&ord1); CNVars = NVars+1;
		}
		if ( Print && modular ) {
			fprintf(asir_out,"mod= %d, eval = ",m); printsubst(subst);
		}
		x = gb(s,m,subst);
		if ( x ) {
			if ( homo ) {
				reducebase_dehomo(x,&xx); x = xx;
				initd(ord); CNVars = NVars;
			}
			reduceall(x,&xx); x = xx;
			if ( modular ) {
				if ( nochk || (membercheck(fi0,x) && gbcheck(x)) )
					break;
			} else
				break;
		}
		if ( modular )
			if ( modular > 1 ) {
				*rp = 0; return;
			} else
				m = lprime[++mindex];
		makesubst(vc,&subst);
		psn = length(s);
		for ( i = psn; i < pslen; i++ ) {
			pss[i] = 0; psh[i] = 0; psc[i] = 0; ps[i] = 0;
		}
	}
	for ( r0 = 0; x; x = NEXT(x) ) {
		NEXTNODE(r0,r); dp_load((int)BDY(x),&ps[(int)BDY(x)]);
		dtop(CO,vv,ps[(int)BDY(x)],(P *)&BDY(r));
	}
	if ( r0 ) NEXT(r) = 0;
	MKLIST(*rp,r0);
	print_stat();
	if ( ShowMag )
		fprintf(asir_out,"\nMax_mag=%d\n",Max_mag);
}

void dp_gr_mod_main(f,v,homo,m,ord,rp)
LIST f,v;
Num homo;
int m;
struct order_spec *ord;
LIST *rp;
{
	struct order_spec ord1;
	VL fv,vv,vc;
	NODE fd,fd0,r,r0,t,x,s,xx;
	DP a,b,c;

	get_vars((Obj)f,&fv); pltovl(v,&vv); vlminus(fv,vv,&vc);
	NVars = length((NODE)vv); PCoeffs = vc ? 1 : 0; VC = vc;
	CNVars = homo ? NVars+1 : NVars;
	if ( ord->id && NVars != ord->nv )
		error("dp_gr_mod_main : invalid order specification");
	initd(ord);
	if ( homo ) {
		for ( fd0 = 0, t = BDY(f); t; t = NEXT(t) ) {
			ptod(CO,vv,(P)BDY(t),&a); dp_homo(a,&b);
			if ( PCoeffs )
				dp_mod(b,m,0,&c);
			else
				_dp_mod(b,m,(NODE)0,&c);
			if ( c ) {
				NEXTNODE(fd0,fd); BDY(fd) = (pointer)c;
			}
		}
		homogenize_order(ord,NVars,&ord1); initd(&ord1);
	} else {
		for ( fd0 = 0, t = BDY(f); t; t = NEXT(t) ) {
			ptod(CO,vv,(P)BDY(t),&b);
			if ( PCoeffs )
				dp_mod(b,m,0,&c);
			else
				_dp_mod(b,m,0,&c);
			if ( c ) {
				NEXTNODE(fd0,fd); BDY(fd) = (pointer)c;
			}
		}
	}
	if ( fd0 ) NEXT(fd) = 0;
	setup_arrays(fd0,m,&s);
	init_stat();
	if ( homo ) {
		initd(&ord1); CNVars = NVars+1;
	}
	x = gb_mod(s,m);
	if ( homo ) {
		reducebase_dehomo(x,&xx); x = xx;
		initd(ord); CNVars = NVars;
	}
	reduceall_mod(x,m,&xx); x = xx;
	if ( PCoeffs )
		for ( r0 = 0; x; x = NEXT(x) ) {
			NEXTNODE(r0,r); mdtop(CO,m,vv,ps[(int)BDY(x)],(P *)&BDY(r));
		}
	else
		for ( r0 = 0; x; x = NEXT(x) ) {
			NEXTNODE(r0,r); _dtop_mod(CO,vv,ps[(int)BDY(x)],(P *)&BDY(r));
		}
	print_stat();
	if ( r0 ) NEXT(r) = 0;
	MKLIST(*rp,r0);
}

void dp_f4_main(f,v,ord,rp)
LIST f,v;
struct order_spec *ord;
LIST *rp;
{
	int i,mindex,m,nochk;
	struct order_spec ord1;
	VL fv,vv,vc;
	NODE fd,fd0,fi,fi0,r,r0,t,subst,x,s,xx;

	dp_fcoeffs = 0;
	get_vars((Obj)f,&fv); pltovl(v,&vv); vlminus(fv,vv,&vc);
	NVars = length((NODE)vv); PCoeffs = vc ? 1 : 0; VC = vc;
	CNVars = NVars;
	if ( ord->id && NVars != ord->nv )
		error("dp_f4_main : invalid order specification");
	initd(ord);
	for ( fd0 = 0, t = BDY(f); t; t = NEXT(t) ) {
		NEXTNODE(fd0,fd); ptod(CO,vv,(P)BDY(t),(DP *)&BDY(fd));
	}
	if ( fd0 ) NEXT(fd) = 0;
	setup_arrays(fd0,0,&s);
	x = gb_f4(s);
	reduceall(x,&xx); x = xx;
	for ( r0 = 0; x; x = NEXT(x) ) {
		NEXTNODE(r0,r); dp_load((int)BDY(x),&ps[(int)BDY(x)]);
		dtop(CO,vv,ps[(int)BDY(x)],(P *)&BDY(r));
	}
	if ( r0 ) NEXT(r) = 0;
	MKLIST(*rp,r0);
}

void dp_f4_mod_main(f,v,m,ord,rp)
LIST f,v;
int m;
struct order_spec *ord;
LIST *rp;
{
	int i;
	struct order_spec ord1;
	VL fv,vv,vc;
	DP b,c;
	NODE fd,fd0,fi,fi0,r,r0,t,subst,x,s,xx;

	dp_fcoeffs = 0;
	get_vars((Obj)f,&fv); pltovl(v,&vv); vlminus(fv,vv,&vc);
	NVars = length((NODE)vv); PCoeffs = vc ? 1 : 0; VC = vc;
	CNVars = NVars;
	if ( ord->id && NVars != ord->nv )
		error("dp_f4_mod_main : invalid order specification");
	initd(ord);
	for ( fd0 = 0, t = BDY(f); t; t = NEXT(t) ) {
		ptod(CO,vv,(P)BDY(t),&b);
		_dp_mod(b,m,0,&c);
		if ( c ) {
			NEXTNODE(fd0,fd); BDY(fd) = (pointer)c;
		}
	}
	if ( fd0 ) NEXT(fd) = 0;
	setup_arrays(fd0,m,&s);
	x = gb_f4_mod(s,m);
	reduceall_mod(x,m,&xx); x = xx;
	for ( r0 = 0; x; x = NEXT(x) ) {
		NEXTNODE(r0,r); _dtop_mod(CO,vv,ps[(int)BDY(x)],(P *)&BDY(r));
	}
	if ( r0 ) NEXT(r) = 0;
	MKLIST(*rp,r0);
}

NODE gb_f4(f)
NODE f;
{
	int i,j,k,nh,row,col,nv;
	NODE r,g,gall;
	DP_pairs d,dm,dr,t;
	DP h,nf,nf1,s,s0,s1,f1,f2,f21,f21r,sp,sp1,sd,sdm;
	MP mp,mp0;
	NODE blist,bt,nt;
	DL *ht,*at;
	MAT mat,nm;
	int *colstat;
	int *rind,*cind;
	int rank,nred;
	Q dn;
	struct oEGT tmp0,tmp1,tmp2,eg_split_symb,eg_split_elim;
	extern struct oEGT eg_mod,eg_elim,eg_chrem,eg_gschk,eg_intrat,eg_symb;

	init_eg(&eg_mod); init_eg(&eg_elim); init_eg(&eg_chrem);
	init_eg(&eg_gschk); init_eg(&eg_intrat); init_eg(&eg_symb);

	doing_f4 = 1;
	for ( gall = g = 0, d = 0, r = f; r; r = NEXT(r) ) {
		i = (int)BDY(r);
		d = updpairs(d,g,i);
		g = updbase(g,i);
		gall = append_one(gall,i);
	}
	if ( gall )
		nv = ((DP)ps[(int)BDY(gall)])->nv;
	while ( d ) {
		get_eg(&tmp0);
		minsugar(d,&dm,&dr); d = dr;
		if ( Print )
			fprintf(asir_out,"sugar=%d\n",dm->sugar);
		blist = 0; s0 = 0;
		/* asph : sum of all head terms of spoly */
		for ( t = dm; t; t = NEXT(t) ) {
			dp_sp(ps[t->dp1],ps[t->dp2],&sp);
			if ( sp ) {
				MKNODE(bt,sp,blist); blist = bt;
				dp_symb(sp,&sp1);
				symb_addd(s0,sp1,&s1); s0 = s1;
			}
		}
		/* s0 : all the terms appeared in symbolic redunction */
		s = s0;
		nred = 0;
		while ( s ) {
			for ( r = gall;	r; r = NEXT(r) )
				if ( dp_redble(s,ps[(int)BDY(r)] ) )
					break;
			if ( r ) {
				dp_subd(s,ps[(int)BDY(r)],&sd);
				muld(CO,sd,ps[(int)BDY(r)],&f2);
				MKNODE(bt,f2,blist); blist = bt;
				dp_symb(f2,&f21); symb_addd(s0,f21,&s1); s0 = s1;
				dp_rest(f21,&f21r);
				dp_rest(s,&s1); symb_addd(s1,f21r,&s);
				nred++;
			} else {
				dp_rest(s,&s1); s = s1;
			}
		}

		/* the first nred polys in blist are reducers */
		/* row = the number of all the polys */
		for ( r = blist, row = 0; r; r = NEXT(r), row++ );
		ht = (DL *)MALLOC(nred*sizeof(DL));
		for ( r = blist, i = 0; i < nred; r = NEXT(r), i++ )
			ht[i] = BDY((DP)BDY(r))->dl;
		for ( mp = BDY(s0), col = 0; mp; mp = NEXT(mp), col++ );
		at = (DL *)MALLOC(col*sizeof(DL));
		for ( mp = BDY(s0), i = 0; i < col; mp = NEXT(mp), i++ )
			at[i] = mp->dl;
		MKMAT(mat,row,col);
		for ( i = 0, r = blist; i < row; r = NEXT(r), i++ )
			dp_to_vect(BDY(r),at,(Q *)mat->body[i]);
		get_eg(&tmp1); add_eg(&eg_symb,&tmp0,&tmp1);
		init_eg(&eg_split_symb); add_eg(&eg_split_symb,&tmp0,&tmp1);
		if ( Print ) {
			print_eg("Symb",&eg_split_symb);
			fprintf(asir_out,"mat : %d x %d",row,col);
			fflush(asir_out);
		}
		rank = generic_gauss_elim(mat,&nm,&dn,&rind,&cind);
		if ( Print )
			fprintf(asir_out,"done rank = %d\n",rank,row,col);
		for ( i = 0; i < rank; i++ ) {
			for ( k = 0; k < nred; k++ )
				if ( !cmpdl(nv,at[rind[i]],ht[k]) )
					break;
			if ( k == nred ) {
				/* this is a new base */
				mp0 = 0;
				NEXTMP(mp0,mp); mp->dl = at[rind[i]]; mp->c = (P)dn;
				for ( k = 0; k < col-rank; k++ )
					if ( nm->body[i][k] ) {
						NEXTMP(mp0,mp); mp->dl = at[cind[k]];
						mp->c = (P)nm->body[i][k];
					}
				NEXT(mp) = 0;
				MKDP(nv,mp0,nf); nf->sugar = dm->sugar;
				dp_ptozp(nf,&nf1);
				nh = newps(nf1,0,0);
				d = updpairs(d,g,nh);
				g = updbase(g,nh);
				gall = append_one(gall,nh);
			}
		}
	}
	if ( Print ) {
		print_eg("Symb",&eg_symb);
		print_eg("Mod",&eg_mod); print_eg("GaussElim",&eg_elim);
		print_eg("ChRem",&eg_chrem); print_eg("IntToRat",&eg_intrat);
		print_eg("Check",&eg_gschk);
	}
	return g;
}

NODE gb_f4_mod(f,m)
NODE f;
int m;
{
	int i,j,k,nh,row,col,nv;
	NODE r,g,gall;
	DP_pairs d,dm,dr,t;
	DP h,nf,s,s0,s1,f1,f2,f21,f21r,sp,sp1,sd,sdm;
	MP mp,mp0;
	NODE blist,bt,nt;
	DL *ht,*at;
	int **mat;
	int *colstat;
	int rank,nred,nonzero;
	struct oEGT tmp0,tmp1,tmp2,eg_split_symb,eg_split_elim;
	extern struct oEGT eg_symb,eg_elim;

	init_eg(&eg_symb); init_eg(&eg_elim);
	for ( gall = g = 0, d = 0, r = f; r; r = NEXT(r) ) {
		i = (int)BDY(r);
		d = updpairs(d,g,i);
		g = updbase(g,i);
		gall = append_one(gall,i);
	}
	if ( gall )
		nv = ((DP)ps[(int)BDY(gall)])->nv;
	while ( d ) {
		get_eg(&tmp0);
		minsugar(d,&dm,&dr); d = dr;
		if ( Print )
			fprintf(asir_out,"sugar=%d\n",dm->sugar);
		blist = 0; s0 = 0;
		/* asph : sum of all head terms of spoly */
		for ( t = dm; t; t = NEXT(t) ) {
			_dp_sp_mod(ps[t->dp1],ps[t->dp2],m,&sp);
			if ( sp ) {
				MKNODE(bt,sp,blist); blist = bt;
				dp_symb(sp,&sp1);
				addd(CO,s0,sp1,&s1); s0 = s1;
			}
		}
		/* s0 : all the terms appeared in symbolic redunction */
		s = s0;
		nred = 0;
		while ( s ) {
			for ( r = gall;	r; r = NEXT(r) )
				if ( dp_redble(s,ps[(int)BDY(r)] ) )
					break;
			if ( r ) {
				dp_subd(s,ps[(int)BDY(r)],&sd);
				_dp_mod(sd,m,0,&sdm);
				_mulmd(CO,m,sdm,ps[(int)BDY(r)],&f2);
				MKNODE(bt,f2,blist); blist = bt;
				dp_symb(f2,&f21); addd(CO,s0,f21,&s1); s0 = s1;
				dp_rest(f21,&f21r);
				dp_rest(s,&s1); addd(CO,s1,f21r,&s);
				nred++;
			} else {
				dp_rest(s,&s1); s = s1;
			}
		}

		/* the first nred polys in blist are reducers */
		/* row = the number of all the polys */
		for ( r = blist, row = 0; r; r = NEXT(r), row++ );
		ht = (DL *)MALLOC(nred*sizeof(DL));
		for ( r = blist, i = 0; i < nred; r = NEXT(r), i++ )
			ht[i] = BDY((DP)BDY(r))->dl;
		for ( mp = BDY(s0), col = 0; mp; mp = NEXT(mp), col++ );
		at = (DL *)MALLOC(col*sizeof(DL));
		for ( mp = BDY(s0), i = 0; i < col; mp = NEXT(mp), i++ )
			at[i] = mp->dl;
		mat = almat(row,col);
		for ( i = 0, r = blist; i < row; r = NEXT(r), i++ )
			_dpmod_to_vect(BDY(r),at,mat[i]);
		colstat = (int *)MALLOC_ATOMIC(col*sizeof(int));
		for ( i = 0, nonzero=0; i < row; i++ )
			for ( j = 0; j < col; j++ )
				if ( mat[i][j] )
					nonzero++;
		if ( Print )
			fprintf(asir_out,"mat : %d x %d (nonzero=%f%%)...",
				row,col,((double)nonzero*100)/(row*col));
		get_eg(&tmp1); add_eg(&eg_symb,&tmp0,&tmp1);
		init_eg(&eg_split_symb); add_eg(&eg_split_symb,&tmp0,&tmp1);
		rank = generic_gauss_elim_mod(mat,row,col,m,colstat);
		get_eg(&tmp2); add_eg(&eg_elim,&tmp1,&tmp2);
		init_eg(&eg_split_elim); add_eg(&eg_split_elim,&tmp1,&tmp2);
		if ( Print ) {
			fprintf(asir_out,"done rank = %d\n",rank,row,col);
			print_eg("Symb",&eg_split_symb);
			print_eg("Elim",&eg_split_elim);
			fprintf(asir_out,"\n");
		}
		for ( j = 0, i = 0; j < col; j++ )
			if ( colstat[j] ) {
				for ( k = 0; k < nred; k++ )
					if ( !cmpdl(nv,at[j],ht[k]) )
						break;
				if ( k == nred ) {
					/* this is a new base */
					mp0 = 0;
					NEXTMP(mp0,mp); mp->dl = at[j]; mp->c = STOI(1);
					for ( k = j+1; k < col; k++ )
						if ( !colstat[k] && mat[i][k] ) {
							NEXTMP(mp0,mp); mp->dl = at[k];
							mp->c = STOI(mat[i][k]);
						}
					NEXT(mp) = 0;
					MKDP(nv,mp0,nf); nf->sugar = dm->sugar;
					nh = newps_mod(nf,m);
					d = updpairs(d,g,nh);
					g = updbase(g,nh);
					gall = append_one(gall,nh);
				}
				i++;
			}
	}
	if ( Print ) {
		print_eg("Symb",&eg_symb);
		print_eg("Elim",&eg_elim);
		fflush(asir_out);
	}
	return g;
}

int DPPlength(n)
DP_pairs n;
{
	int i;

	for ( i = 0; n; n = NEXT(n), i++ );
	return i;
}

void printdl(dl)
DL dl;
{
	int i;

	fprintf(asir_out,"<<");
	for ( i = 0; i < CNVars-1; i++ )
		fprintf(asir_out,"%d,",dl->d[i]);
	fprintf(asir_out,"%d>>",dl->d[i]);
}

void pltovl(l,vl)
LIST l;
VL *vl;
{
	NODE n;
	VL r,r0;

	n = BDY(l);
	for ( r0 = 0; n; n = NEXT(n) ) {
		NEXTVL(r0,r); r->v = VR((P)BDY(n));
	}
	if ( r0 ) NEXT(r) = 0;
	*vl = r0;
}

void makesubst(v,s)
VL v;
NODE *s;
{
	NODE r,r0;
	Q q;
	unsigned int n;

	for ( r0 = 0; v; v = NEXT(v) ) {
		NEXTNODE(r0,r); BDY(r) = (pointer)v->v;
#if defined(_PA_RISC1_1)
		n = mrand48()&BMASK; UTOQ(n,q);
#else
		n = random(); UTOQ(n,q);
#endif
		NEXTNODE(r0,r); BDY(r) = (pointer)q;
	}
	if ( r0 ) NEXT(r) = 0;
	*s = r0;
}

void printsubst(s)
NODE s;
{
	fputc('[',asir_out);
	while ( s ) {
		printv(CO,(V)BDY(s)); s = NEXT(s);
		fprintf(asir_out,"->%d",QTOS((Q)BDY(s)));
		if ( NEXT(s) ) {
			fputc(',',asir_out); s = NEXT(s);
		} else
			break;
	}
	fprintf(asir_out,"]\n"); return;
}

void vlminus(v,w,d)
VL v,w,*d;
{
	int i,j,n,m;
	V *va,*wa;
	V a;
	VL r,r0;
	VL t;

	for ( n = 0, t = v; t; t = NEXT(t), n++ );
	va = (V *)ALLOCA(n*sizeof(V));
	for ( i = 0, t = v; t; t = NEXT(t), i++ )
		va[i] = t->v;
	for ( m = 0, t = w; t; t = NEXT(t), m++ );
	wa = (V *)ALLOCA(m*sizeof(V));
	for ( i = 0, t = w; t; t = NEXT(t), i++ )
		wa[i] = t->v;
	for ( i = 0; i < n; i++ ) {
		a = va[i];
		for ( j = 0; j < m; j++ )
			if ( a == wa[j] )
				break;
		if ( j < m )
			va[i] = 0;
	}
	for ( r0 = 0, i = 0; i < n; i++ )
		if ( va[i] ) { NEXTVL(r0,r); r->v = va[i];	}
	if ( r0 ) NEXT(r) = 0;
	*d = r0;
}

int validhc(a,m,s)
P a;
int m;
NODE s;
{
	P c,c1;
	V v;

	if ( !a )
		return 0;
	for ( c = a; s; s = NEXT(s) ) {
		v = (V)BDY(s); s = NEXT(s);
		substp(CO,c,v,(P)BDY(s),&c1); c = c1;
	}
	ptomp(m,c,&c1);
	return c1 ? 1 : 0;
}

void setup_arrays(f,m,r)
NODE f,*r;
int m;
{
	int i;
	NODE s,s0;

	f = NODE_sortb(f,1);
	psn = length(f); pslen = 2*psn;
	ps = (DP *)MALLOC(pslen*sizeof(DP));
	psh = (DL *)MALLOC(pslen*sizeof(DL));
	pss = (int *)MALLOC(pslen*sizeof(int));
	psc = (P *)MALLOC(pslen*sizeof(P));
	for ( i = 0; i < psn; i++, f = NEXT(f) ) {
		prim_part((DP)BDY(f),m,&ps[i]);
		if ( Demand )
			dp_save(i,ps[i]);
		psh[i] = BDY(ps[i])->dl;
		pss[i] = ps[i]->sugar;
		psc[i] = BDY(ps[i])->c;
	}
	for ( s0 = 0, i = 0; i < psn; i++ ) {
		NEXTNODE(s0,s); BDY(s) = (pointer)i;
	}
	if ( s0 ) NEXT(s) = 0;
	*r = s0;
}

void prim_part(f,m,r)
DP f,*r;
int m;
{
	if ( m > 0 ) {
		if ( PCoeffs )
			dp_prim_mod(f,m,r);
		else
			*r = f;
	} else {
		if ( dp_fcoeffs )
			*r = f;
		else if ( PCoeffs )
			dp_prim(f,r);
		else
			dp_ptozp(f,r);
	}
}

NODE /* of DP */ NODE_sortb_insert( newdp, nd, dec )
DP newdp;
NODE /* of DP */ nd;
int dec;
{
	register NODE last, p;
	register DL newdl = BDY(newdp)->dl;
	register int (*cmpfun)() = cmpdl, nv = CNVars;
	NODE newnd;
	int sgn = dec ? 1 : -1;
	MKNODE( newnd, newdp, 0 );
	if ( !(last = nd) || sgn*(*cmpfun)( nv, newdl, BDY((DP) BDY(last))->dl ) > 0 ) {
		NEXT(newnd) = last;
		return newnd;
	}
	for ( ; p = NEXT(last); last = p )
		if ( sgn*(*cmpfun)( nv, newdl, BDY((DP) BDY(p))->dl ) > 0 ) break;
	if ( p ) NEXT(NEXT(last) = newnd) = p;
	else NEXT(last) = newnd;
	return nd;
}

NODE NODE_sortb( node, dec )
NODE node;
int dec;
{
	register NODE nd, ans;

	for ( ans = 0, nd = node; nd; nd = NEXT(nd) )
		ans = NODE_sortb_insert( (DP) BDY(nd), ans, dec );
	return ans;
}

NODE /* of index */ NODE_sortbi_insert( newdpi, nd, dec )
int newdpi;
NODE /* of index */ nd;
int dec;
{
	register NODE last, p;
	register DL newdl = psh[newdpi];
	register int (*cmpfun)() = cmpdl, nv = CNVars;
	NODE newnd;
	int sgn = dec ? 1 : -1;
	MKNODE( newnd, newdpi, 0 );
	if ( !(last = nd) || sgn*(*cmpfun)( nv, newdl, psh[(int)BDY(last)] ) > 0 ) {
		NEXT(newnd) = last;
		return newnd;
	}
	for ( ; p = NEXT(last); last = p )
		if ( sgn*(*cmpfun)( nv, newdl, psh[(int)BDY(p)] ) > 0 ) break;
	if ( p ) NEXT(NEXT(last) = newnd) = p;
	else NEXT(last) = newnd;
	return nd;
}

NODE NODE_sortbi( node, dec )
NODE node;
int dec;
{
	register NODE nd, ans;

	for ( ans = 0, nd = node; nd; nd = NEXT(nd) )
		ans = NODE_sortbi_insert( (int) BDY(nd), ans, dec );
	return ans;
}

void reduceall(in,h)
NODE in;
NODE *h;
{
	NODE r,t,top;
	int n,i,j;
	int *w;
	DP g,g1;
	struct oEGT tmp0,tmp1;

	if ( NoRA ) {
		*h = in; return;
	}
	if ( Print || PrintShort ) {
		fprintf(asir_out,"reduceall\n"); fflush(asir_out);
	}
	r = NODE_sortbi(in,0);
	n = length(r);
	w = (int *)ALLOCA(n*sizeof(int));
	for ( i = 0, t = r; i < n; i++, t = NEXT(t) )
		w[i] = (int)BDY(t);
	for ( i = 0; i < n; i++ ) {
		for ( top = 0, j = n-1; j >= 0; j-- )
			if ( j != i ) {
				MKNODE(t,(pointer)w[j],top); top = t;
			}
		get_eg(&tmp0);
		dp_load(w[i],&ps[w[i]]);
		_dp_nf(top,ps[w[i]],ps,1,&g);

		if ( !PtozpRA || !Multiple )
			_dp_nf(top,ps[w[i]],ps,1,&g);
		else
#if MPI
			_dp_nf_ptozp_mpi(top,ps[w[i]],ps,1,Multiple,&g);
#else
			_dp_nf_ptozp(top,ps[w[i]],ps,1,Multiple,&g);
#endif
		prim_part(g,0,&g1);
		get_eg(&tmp1); add_eg(&eg_ra,&tmp0,&tmp1);
		if ( Print || PrintShort ) {
			fprintf(asir_out,"."); fflush(asir_out);
		}
		w[i] = newps(g1,0,(NODE)0);
	}
	for ( top = 0, j = n-1; j >= 0; j-- ) {
		MKNODE(t,(pointer)w[j],top); top = t;
	}
	*h = top;
	if ( Print || PrintShort )
		fprintf(asir_out,"\n");
}

void reduceall_mod(in,m,h)
NODE in;
int m;
NODE *h;
{
	NODE r,t,top;
	int n,i,j;
	int *w;
	DP g;
	struct oEGT tmp0,tmp1;

	if ( NoRA ) {
		*h = in; return;
	}
	if ( Print || PrintShort ) {
		fprintf(asir_out,"reduceall\n"); fflush(asir_out);
	}
	r = NODE_sortbi(in,0);
	n = length(r);
	w = (int *)ALLOCA(n*sizeof(int));
	for ( i = 0, t = r; i < n; i++, t = NEXT(t) )
		w[i] = (int)BDY(t);
	for ( i = 0; i < n; i++ ) {
		for ( top = 0, j = n-1; j >= 0; j-- )
			if ( j != i ) {
				MKNODE(t,(pointer)w[j],top); top = t;
			}
		get_eg(&tmp0);
		if ( PCoeffs )
			dp_nf_mod(top,ps[w[i]],ps,m,1,&g);
		else
			_dp_nf_mod(top,ps[w[i]],ps,m,1,&g);
		get_eg(&tmp1); add_eg(&eg_ra,&tmp0,&tmp1);
		if ( Print || PrintShort ) {
			fprintf(asir_out,"."); fflush(asir_out);
		}
		w[i] = newps_mod(g,m);
	}
	for ( top = 0, j = n-1; j >= 0; j-- ) {
		MKNODE(t,(pointer)w[j],top); top = t;
	}
	*h = top;
	if ( Print || PrintShort )
		fprintf(asir_out,"\n");
}

int newps(a,m,subst)
DP a;
int m;
NODE subst;
{
	if ( m && !validhc(!a?0:BDY(a)->c,m,subst) )
		return -1;
	if ( psn == pslen ) {
		pslen *= 2;
		ps = (DP *)REALLOC((char *)ps,pslen*sizeof(DP));
		psh = (DL *)REALLOC((char *)psh,pslen*sizeof(DL));
		pss = (int *)REALLOC((char *)pss,pslen*sizeof(int));
		psc = (P *)REALLOC((char *)psc,pslen*sizeof(P));
		if ( m )
			psm = (DP *)REALLOC((char *)psm,pslen*sizeof(DP));
	}
	if ( Demand ) {
		if ( doing_f4 )
			ps[psn] = a;
		else
			ps[psn] = 0;
		dp_save(psn,a);
	} else
		ps[psn] = a;
	psh[psn] = BDY(a)->dl;
	pss[psn] = a->sugar;
	psc[psn] = BDY(a)->c;
	if ( m )
		_dp_mod(a,m,subst,&psm[psn]);
	return psn++;
}

int newps_nosave(a,m,subst)
DP a;
int m;
NODE subst;
{
	if ( m && !validhc(!a?0:BDY(a)->c,m,subst) )
		return -1;
	if ( psn == pslen ) {
		pslen *= 2;
		ps = (DP *)REALLOC((char *)ps,pslen*sizeof(DP));
		psh = (DL *)REALLOC((char *)psh,pslen*sizeof(DL));
		pss = (int *)REALLOC((char *)pss,pslen*sizeof(int));
		psc = (P *)REALLOC((char *)psc,pslen*sizeof(P));
		if ( m )
			psm = (DP *)REALLOC((char *)psm,pslen*sizeof(DP));
	}
	ps[psn] = 0;
	psh[psn] = BDY(a)->dl;
	pss[psn] = a->sugar;
	psc[psn] = BDY(a)->c;
	if ( m )
		_dp_mod(a,m,subst,&psm[psn]);
	return psn++;
}

int newps_mod(a,m)
DP a;
int m;
{
	if ( psn == pslen ) {
		pslen *= 2;
		ps = (DP *)REALLOC((char *)ps,pslen*sizeof(DP));
		psh = (DL *)REALLOC((char *)psh,pslen*sizeof(DL));
		pss = (int *)REALLOC((char *)pss,pslen*sizeof(int));
		psc = (P *)REALLOC((char *)psc,pslen*sizeof(P)); /* XXX */
	}
	ps[psn] = a;
	psh[psn] = BDY(ps[psn])->dl;
	pss[psn] = ps[psn]->sugar;
	return psn++;
}

void reducebase_dehomo(f,g)
NODE f,*g;
{
	int n,i,j,k;
	int *r;
	DL *w,d;
	DP u;
	NODE t,top;

	n = length(f);
	w = (DL *)ALLOCA(n*sizeof(DL));
	r = (int *)ALLOCA(n*sizeof(int));
	for ( i = 0, t = f; i < n; i++, t = NEXT(t) ) {
		r[i] = (int)BDY(t); w[i] = psh[r[i]];
	}
	for ( i = 0; i < n; i++ ) {
		for ( j = 0, d = w[i]; j < n; j++ ) {
			if ( j != i ) {
				for ( k = 0; k < NVars; k++ )
					if ( d->d[k] < w[j]->d[k] )
						break;
				if ( k == NVars )
					break;
			}
		}
		if ( j != n )
			r[i] = -1;
	}
	for ( top = 0, i = n-1; i >= 0; i-- )
		if ( r[i] >= 0 ) {
			dp_load(r[i],&ps[r[i]]); dp_dehomo(ps[r[i]],&u); j = newps(u,0,0);
			MKNODE(t,j,top); top = t;
		}
	*g = top;
}

NODE append_one(f,n)
NODE f;
int n;
{
	NODE t;

	if ( Reverse || !f ) {
		MKNODE(t,(pointer)n,f); return t;
	} else {
		for ( t = f; NEXT(t); t = NEXT(t) );
		MKNODE(NEXT(t),(pointer)n,0);
		return f;
	}
}

DP_pairs minp( d, prest )
DP_pairs d, *prest;
{
	register DP_pairs m, ml, p, l;
	register DL lcm;
	register int s, nv = CNVars;
	register int (*cmpfun)() = cmpdl;

	if ( !(p = NEXT(m = d)) ) {
		*prest = p;
		NEXT(m) = 0;
		return m;
	}
	for ( lcm = m->lcm, s = m->sugar, ml = 0, l = m; p; p = NEXT(l = p) )
		if ( NoSugar ? (*cmpfun)( nv, lcm, p->lcm ) >= 0 :
		     (s > p->sugar || s == p->sugar && (*cmpfun)( nv, lcm, p->lcm ) >= 0) )
			ml = l,  lcm = (m = p)->lcm,  s = p->sugar;
	if ( !ml ) *prest = NEXT(m);
	else {
		NEXT(ml) = NEXT(m);
		*prest = d;
	}
	NEXT(m) = 0;
	return m;
}

void minsugar(d,dm,dr)
DP_pairs d;
DP_pairs *dm,*dr;
{
	int msugar;
	DP_pairs t,dm0,dr0,dmt,drt;

	for ( msugar = d->sugar, t = NEXT(d); t; t = NEXT(t) )
		if ( t->sugar < msugar )
			msugar = t->sugar;
	dm0 = 0; dr0 = 0;
	for ( t = d; t; t = NEXT(t) ) {
		if ( t->sugar == msugar ) {
			NEXTDPP(dm0,dmt);
			dmt->dp1 = t->dp1; dmt->dp2 = t->dp2;
			dmt->lcm = t->lcm; dmt->sugar = t->sugar;
		} else {
			NEXTDPP(dr0,drt);
			drt->dp1 = t->dp1; drt->dp2 = t->dp2;
			drt->lcm = t->lcm; drt->sugar = t->sugar;
		}
	}
	if ( dm0 ) NEXT(dmt) = 0;
	if ( dr0 ) NEXT(drt) = 0;
	*dm = dm0; *dr = dr0;
}

NODE gb(f,m,subst)
NODE f;
int m;
NODE subst;
{
	int i,nh,prev,mag;
	NODE r,g,gall;
	DP_pairs d,d1;
	DP_pairs l;
	DP h,nf,nfm,dp1,dp2;
	MP mp;
	struct oEGT tnf0,tnf1,tnfm0,tnfm1,tpz0,tpz1,tsp0,tsp1,tspm0,tspm1,tnp0,tnp1,tmp0,tmp1;
	int skip_nf_flag;
	double t_0;
	static prev_sugar = -1;

	Max_mag = 0;
	prev = 1;
	doing_f4 = 0;
	if ( m ) {
		psm = (DP *)MALLOC(pslen*sizeof(DP));
		for ( i = 0; i < psn; i++ )
			if ( psh[i] && !validhc(psc[i],m,subst) )
				return 0;
			else
				_dp_mod(ps[i],m,subst,&psm[i]);
	}
	for ( gall = g = 0, d = 0, r = f; r; r = NEXT(r) ) {
		i = (int)BDY(r);
		d = updpairs(d,g,i);
		g = updbase(g,i);
		gall = append_one(gall,i);
	}
	while ( d ) {
		get_eg(&tmp0);
		l = minp(d,&d1); d = d1;
		get_eg(&tmp1); add_eg(&eg_mp,&tmp0,&tmp1);
		if ( m ) {
			get_eg(&tspm0);
			_dp_sp_mod(psm[l->dp1],psm[l->dp2],m,&h);
			get_eg(&tspm1); add_eg(&eg_spm,&tspm0,&tspm1);
			get_eg(&tnfm0);
			_dp_nf_mod(gall,h,psm,m,0,&nfm);
			get_eg(&tnfm1); add_eg(&eg_nfm,&tnfm0,&tnfm1);
		} else
			nfm = (DP)1;
		if ( nfm ) {
			get_eg(&tsp0);
			if ( Demand ) {
				if ( dp_load_t(psn,&nf) ) {
					skip_nf_flag = 1;
					get_eg(&tsp1); add_eg(&eg_nf,&tsp0,&tsp1);
					tnf0=tsp0; tnf1=tsp1;
					goto skip_nf;
				} else {
					skip_nf_flag = 0;
					dp_load(l->dp1,&dp1); dp_load(l->dp2,&dp2);
					dp_sp(dp1,dp2,&h);
				}
			} else
				dp_sp(ps[l->dp1],ps[l->dp2],&h);
			get_eg(&tsp1); add_eg(&eg_sp,&tsp0,&tsp1);
			get_eg(&tnf0);
			t_0 = get_rtime();
			if ( !Multiple )
				_dp_nf(gall,h,ps,!Top,&nf);
			else
#if MPI
				_dp_nf_ptozp_mpi(gall,h,ps,!Top,Multiple,&nf);
#else
				_dp_nf_ptozp(gall,h,ps,!Top,Multiple,&nf);
#endif
			if ( Print )
				fprintf(asir_out,"(%.3g)",get_rtime()-t_0);
			get_eg(&tnf1); add_eg(&eg_nf,&tnf0,&tnf1);
		} else
			nf = 0;
skip_nf:
		if ( nf ) {
			NZR++;
			get_eg(&tpz0);
			prim_part(nf,0,&h);
			get_eg(&tpz1); add_eg(&eg_pz,&tpz0,&tpz1);
			get_eg(&tnp0);
			if ( Demand && skip_nf_flag )
				nh = newps_nosave(h,m,subst);
			else
				nh = newps(h,m,subst);
			get_eg(&tnp1); add_eg(&eg_np,&tnp0,&tnp1);
			if ( nh < 0 )
				return 0;
			d = updpairs(d,g,nh);
			g = updbase(g,nh);
			gall = append_one(gall,nh);
			if ( !dp_fcoeffs && ShowMag ) {
				for ( mag = 0, mp = BDY(h); mp; mp = NEXT(mp) )
					mag += p_mag((P)mp->c);
				Max_mag = MAX(Max_mag,mag);
			}
			if ( Print ) {
				if ( !prev )
					fprintf(asir_out,"\n");
				print_split_e(&tnf0,&tnf1); print_split_e(&tpz0,&tpz1);
				printdl(psh[nh]);
				fprintf(asir_out,"(%d,%d),nb=%d,nab=%d,rp=%d,sugar=%d",
					l->dp1,l->dp2,length(g),length(gall),DPPlength(d),
					pss[nh]);
				if ( ShowMag )
					fprintf(asir_out,",mag=%d",mag);
				fprintf(asir_out,"\n"); fflush(asir_out);
			} else if ( PrintShort ) {
				fprintf(asir_out,"+"); fflush(asir_out);
			}
			prev = 1;
		} else {
			if ( m )
				add_eg(&eg_znfm,&tnfm0,&tnfm1);
			ZR++;
			if ( Print || PrintShort ) {
				if ( h && (h->sugar != prev_sugar) ) {
					fprintf(asir_out,"[%d]",h->sugar);	
					prev_sugar = h->sugar;
				}
				fprintf(asir_out,"."); fflush(asir_out); prev = 0;
			}
		}
	}
	if ( Print || PrintShort )
		fprintf(asir_out,"gb done\n");
	return g;
}

NODE gbd(f,m,subst,dlist)
NODE f;
int m;
NODE subst;
NODE dlist;
{
	int i,nh,prev;
	NODE r,g,gall;
	struct dp_pairs ol;
	DP_pairs l;
	NODE pair;
	DP h,nf,nfm,dp1,dp2;
	struct oEGT tnf0,tnf1,tnfm0,tnfm1,tpz0,tpz1,tsp0,tsp1,tspm0,tspm1,tnp0,tnp1;

	prev = 1;
	l = &ol;
	if ( m ) {
		psm = (DP *)MALLOC(pslen*sizeof(DP));
		for ( i = 0; i < psn; i++ )
			if ( psh[i] && !validhc(psc[i],m,subst) )
				return 0;
			else
				_dp_mod(ps[i],m,subst,&psm[i]);
	}
	for ( gall = g = 0, r = f; r; r = NEXT(r) ) {
		i = (int)BDY(r);
		g = updbase(g,i);
		gall = append_one(gall,i);
	}
	while ( dlist ) {
		pair = BDY((LIST)BDY(dlist)); dlist = NEXT(dlist);
		l->dp1 = QTOS((Q)BDY(pair)); pair = NEXT(pair);
		l->dp2 = QTOS((Q)BDY(pair));
		if ( m ) {
			get_eg(&tspm0);
			_dp_sp_mod(psm[l->dp1],psm[l->dp2],m,&h);
			get_eg(&tspm1); add_eg(&eg_spm,&tspm0,&tspm1);
			get_eg(&tnfm0);
			_dp_nf_mod(gall,h,psm,m,0,&nfm);
			get_eg(&tnfm1); add_eg(&eg_nfm,&tnfm0,&tnfm1);
		} else
			nfm = (DP)1;
		if ( nfm ) {
			get_eg(&tsp0);
			if ( Demand ) {
				dp_load(l->dp1,&dp1); dp_load(l->dp2,&dp2);
				dp_sp(dp1,dp2,&h);
			} else
				dp_sp(ps[l->dp1],ps[l->dp2],&h);
			get_eg(&tsp1); add_eg(&eg_sp,&tsp0,&tsp1);
			get_eg(&tnfm0);
			get_eg(&tnf0);
			if ( !Multiple )
				_dp_nf(gall,h,ps,!Top,&nf);
			else
				_dp_nf_ptozp(gall,h,ps,!Top,Multiple,&nf);
			get_eg(&tnf1); add_eg(&eg_nf,&tnf0,&tnf1);
		} else
			nf = 0;
		if ( nf ) {
			NZR++;
			get_eg(&tpz0);
			prim_part(nf,0,&h);
			get_eg(&tpz1); add_eg(&eg_pz,&tpz0,&tpz1);
			get_eg(&tnp0);
			nh = newps(h,m,subst);
			get_eg(&tnp1); add_eg(&eg_np,&tnp0,&tnp1);
			if ( nh < 0 )
				return 0;
			g = updbase(g,nh);
			gall = append_one(gall,nh);
			if ( Print ) {
				if ( !prev )
					fprintf(asir_out,"\n");
				print_split_eg(&tnf0,&tnf1); fflush(asir_out);
				fprintf(asir_out,"(%d,%d),nb=%d,nab=%d,rp=%d,sugar=%d",l->dp1,l->dp2,length(g),length(gall),length(dlist),pss[nh]);
				printdl(psh[nh]); fprintf(asir_out,"\n"); fflush(asir_out);
			}
			prev = 1;
		} else {
			if ( m )
				add_eg(&eg_znfm,&tnfm0,&tnfm1);
			ZR++;
			if ( Print ) {
				fprintf(asir_out,"."); fflush(asir_out); prev = 0;
			}
		}
	}
	if ( Print )
		fprintf(asir_out,"gb done\n");
	return g;
}

NODE gb_mod(f,m)
NODE f;
int m;
{
	int i,nh,prev;
	NODE r,g,gall;
	DP_pairs d,d1;
	DP_pairs l;
	DP h,nf;
	struct oEGT tnfm0,tnfm1,tspm0,tspm1,tmp0,tmp1,tpz0,tpz1;

	prev = 1;
	for ( gall = g = 0, d = 0, r = f; r; r = NEXT(r) ) {
		i = (int)BDY(r);
		d = updpairs(d,g,i);
		g = updbase(g,i);
		gall = append_one(gall,i);
	}
	while ( d ) {
		get_eg(&tmp0);
		l = minp(d,&d1); d = d1;
		get_eg(&tmp1); add_eg(&eg_mp,&tmp0,&tmp1); get_eg(&tspm0);
		if ( PCoeffs ) {
			dp_sp_mod(ps[l->dp1],ps[l->dp2],m,&h);
			get_eg(&tspm1); add_eg(&eg_spm,&tspm0,&tspm1); get_eg(&tnfm0);
			dp_nf_mod(gall,h,ps,m,!Top,&nf);
		} else {
			_dp_sp_mod(ps[l->dp1],ps[l->dp2],m,&h);
			get_eg(&tspm1); add_eg(&eg_spm,&tspm0,&tspm1); get_eg(&tnfm0);
			_dp_nf_mod(gall,h,ps,m,!Top,&nf);
		}
		get_eg(&tnfm1); add_eg(&eg_nfm,&tnfm0,&tnfm1);
		if ( nf ) {
			NZR++;
			get_eg(&tpz0);
			prim_part(nf,m,&h);
			get_eg(&tpz1); add_eg(&eg_pz,&tpz0,&tpz1);
			nh = newps_mod(h,m);
			if ( nh < 0 )
				return 0;
			d = updpairs(d,g,nh);
			g = updbase(g,nh);
			if ( InterReduce )
				interreduce_mod(g,nh,m);
			gall = append_one(gall,nh);
			if ( Print ) {
				if ( !prev )
					fprintf(asir_out,"\n");
				print_split_eg(&tnfm0,&tnfm1); fflush(asir_out);
				fprintf(asir_out,"(%d,%d),nb=%d,nab=%d,rp=%d,sugar=%d",l->dp1,l->dp2,length(g),length(gall),DPPlength(d),pss[nh]);
				printdl(psh[nh]); fprintf(asir_out,"\n"); fflush(asir_out);
			}
			prev = 1;
		} else {
			add_eg(&eg_znfm,&tnfm0,&tnfm1);
			ZR++;
			if ( Print ) {
				fprintf(asir_out,"."); fflush(asir_out); prev = 0;
			}
		}
	}
	if ( Print )
		fprintf(asir_out,"gb_mod done\n");
	return g;
}

void interreduce_mod(g,n,m)
NODE g;
int n,m;
{
	DP nf;
	NODE gn,t;
	int i;

	MKNODE(gn,(pointer)n,0);
	if ( PCoeffs )
		for ( t = g; t; t = NEXT(t) ) {
			i = (int)BDY(t);
			if ( i != n ) {
				dp_nf_mod(gn,ps[i],ps,m,1,&nf); ps[i] = nf;
			}
		}
	else
		for ( t = g; t; t = NEXT(t) ) {
			i = (int)BDY(t);
			if ( i != n ) {
				_dp_nf_mod(gn,ps[i],ps,m,1,&nf); ps[i] = nf;
			}
		}
}

void _dp_nf_mod(b,g,ps,mod,full,rp)
NODE b;
DP g;
DP *ps;
int mod,full;
DP *rp;
{
	DP u,p,d,s,t;
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
				_dp_red_mod(g,p,mod,&u);		
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
			_addmd(CO,mod,d,t,&s); d = s;
			dp_rest(g,&t); g = t;
		}
	}
	if ( d )
		d->sugar = sugar;
	*rp = d;
}

void _dp_nf(b,g,ps,full,rp)
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
	int sugar,psugar;

	if ( !g ) {
		*rp = 0; return;
	}
	sugar = g->sugar;
	for ( d = 0; g; ) {
		for ( u = 0, l = b; l; l = NEXT(l) ) {
			if ( dl_redble(BDY(g)->dl,psh[(int)BDY(l)]) ) {
				dp_load((int)BDY(l),&p);
				dp_red(d,g,p,&t,&u,&dmy);
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

#define SAFENM(q) ((q)?NM(q):0)

double pz_t_e, pz_t_d, pz_t_d1, pz_t_c, im_t_s, im_t_r;

void _dp_nf_ptozp(b,g,ps,full,multiple,r)
NODE b;
DP g;
DP *ps;
int full,multiple;
DP *r;
{
	DP u,dp,rp,t,t1,t2,red,shift;
	Q dc,rc,dcq,rcq,cont,hr,hred,cr,cred,mcred,c,gcd,cq;
	N gn,tn,cn;
	NODE l;
	MP m,mr;
	int hmag,denom;
	int sugar,psugar;
	NODE dist;
	STRING imul;
	int ndist;
	int kara_bit;
	extern int kara_mag;
	double get_rtime();	
	double t_0,t_00,tt,ttt,t_p,t_m,t_m1,t_m2,t_s,t_g,t_a;

	if ( !g ) {
		*r = 0; return;
	}
	pz_t_e = pz_t_d = pz_t_d1 = pz_t_c = 0;
	t_p = t_m = t_m1 = t_m2 = t_s = t_g = t_a = 0;

	denom = Denominator?Denominator:1;
	hmag = multiple*HMAG(g)/denom;
	kara_bit = kara_mag*27; /* XXX */
	if ( Dist ) {
		dist = BDY(Dist);
		ndist = length(dist);
	}
	sugar = g->sugar;

	dc = 0; dp = 0; rc = ONE; rp = g;
	MKSTR(imul,"dp_imul_index");

	for ( ; rp; ) {
		for ( u = 0, l = b; l; l = NEXT(l) ) {
			if ( dl_redble(BDY(rp)->dl,psh[(int)BDY(l)]) ) {
				t_0 = get_rtime();
				dp_load((int)BDY(l),&red);
				hr = (Q)BDY(rp)->c; hred = (Q)BDY(red)->c;
				gcdn(NM(hr),NM(hred),&gn);
				divsn(NM(hred),gn,&tn); NTOQ(tn,SGN(hred),cr);
				divsn(NM(hr),gn,&tn); NTOQ(tn,SGN(hr),cred);
				tt = get_rtime()-t_0; t_p += tt;

				t_0 = get_rtime();
				dp_subd(rp,red,&shift);
#if INET
				if ( Dist && ndist && HMAG(red) > kara_bit ) {
					NODE n0,n1,n2,n3;
					int i,s,id;
					Obj dmy;
					Q ind;

					if ( Print )
						fprintf(asir_out,"d");
					i = (int)BDY(l); STOQ(i,ind);
					chsgnp((P)cred,(P *)&mcred);

					MKNODE(n3,ind,0); MKNODE(n2,mcred,n3);
					MKNODE(n1,imul,n2); MKNODE(n0,BDY(dist),n1); 
					Pox_rpc(n0,&dmy);
					muldc(CO,rp,(P)cr,&t);
					NEXT(n0)=0;
					Pox_pop_local(n0,&t1);
				} else
#endif
				{
/*					
					if ( Print )
						fprintf(asir_out,"l");
*/
					t_00 = get_rtime();
					muldc(CO,rp,(P)cr,&t);
					ttt = get_rtime()-t_00; t_m1 += ttt/dp_nt(rp);
					t_00 = get_rtime();
					chsgnp((P)cred,(P *)&mcred);
					muldc(CO,red,(P)mcred,&t1);
					ttt = get_rtime()-t_00; t_m2 += ttt/dp_nt(red);
				}
				t_00 = get_rtime();
				muld(CO,shift,t1,&t2);
				addd(CO,t,t2,&u);
				tt = get_rtime(); t_m += tt-t_0;
				ttt = get_rtime(); t_s += ttt-t_00;

				psugar = (BDY(rp)->dl->td - BDY(red)->dl->td) + red->sugar;
				sugar = MAX(sugar,psugar);
				if ( !u ) {
					if ( dp )
						dp->sugar = sugar;
					*r = dp;
					goto final;
				}
				break;
			}
		}
		if ( u ) {
			if ( HMAG(u) > hmag ) {
				t_0 = get_rtime();
#if INET
				if ( Dist && HMAG(u) > kara_bit ) {
					if ( Print )
						fprintf(asir_out,"D");
					dp_ptozp_d(dist,ndist,u,&t);
				} else {
					if ( Print )
						fprintf(asir_out,"L");
					dp_ptozp_d(0,0,u,&t);
				}
#else
				dp_ptozp(u,&t); /* XXX */
#endif
				tt = get_rtime()-t_0; t_g += tt;
				t_0 = get_rtime();
				divsn(NM((Q)BDY(u)->c),NM((Q)BDY(t)->c),&cn); NTOQ(cn,1,cont);
				if ( !dp_fcoeffs && Print ) {
					fprintf(asir_out,"(%d)",p_mag((P)cont)*100/p_mag((P)BDY(u)->c));
					fflush(asir_out);
				}
				mulq(cr,dc,&dcq);
				mulq(cont,rc,&rcq);
				gcdn(SAFENM(dcq),SAFENM(rcq),&gn);
				divsn(SAFENM(dcq),gn,&tn); NTOQ(tn,SGN(dcq),dc);
				divsn(SAFENM(rcq),gn,&tn); NTOQ(tn,SGN(rcq),rc);
				tt = get_rtime()-t_0; t_a += tt;
				rp = t;
				hmag = multiple*HMAG(rp)/denom;
			} else {
				t_0 = get_rtime();
				mulq(cr,dc,&dcq); dc = dcq;
				tt = get_rtime()-t_0; t_a += tt;
				rp = u;
			}
		} else if ( !full ) {
			if ( rp ) {
				MKDP(rp->nv,BDY(rp),t); t->sugar = sugar; rp = t;
			}
			*r = rp;
			goto final;
		} else {
			t_0 = get_rtime();
			mulq((Q)BDY(rp)->c,rc,&c);
			gcdn(SAFENM(dc),SAFENM(c),&gn); NTOQ(gn,1,gcd);
			divsn(SAFENM(dc),gn,&tn); NTOQ(tn,SGN(dc),dcq);
			divsn(SAFENM(c),gn,&tn); NTOQ(tn,SGN(c),cq);
			muldc(CO,dp,(P)dcq,&t1);
			m = BDY(rp); NEWMP(mr); mr->dl = m->dl; mr->c = (P)cq;
			NEXT(mr) = 0; MKDP(rp->nv,mr,t); t->sugar = mr->dl->td;
			addd(CO,t,t1,&dp);
			dc = gcd;

			dp_rest(rp,&t); rp = t;
			tt = get_rtime()-t_0; t_a += tt;
		}
	}
	if ( dp )
		dp->sugar = sugar;
	*r = dp;
final:
	if ( Print )
		fprintf(asir_out,"(%.3g %.3g %.3g %.3g %.3g %.3g %.3g %.3g %.3g %.3g %.3g)",
			t_p,t_m,t_m1,t_m2,t_s,
			t_g,t_a,
			pz_t_e, pz_t_d, pz_t_d1, pz_t_c);
}

void dp_imul_d();
void imulv();

void _dp_nf_ptozp_mpi(b,g,ps,full,multiple,r)
NODE b;
DP g;
DP *ps;
int full,multiple;
DP *r;
{
	Obj dmy;
	DP u,dp,rp,t,t1,t2,red,shift;
	Q dc,rc,dcq,rcq,cont,hr,hred,cr,cred,mcred,c,gcd,cq;
	N gn,tn,cn;
	NODE l,n0,n1,n2,n3;
	MP m,mr;
	int i,n;
	int hmag,denom;
	int sugar,psugar;
	NODE dist;
	STRING imul;
	int ndist;
	int kara_bit;
	extern int kara_mag;
	extern int mpi_mag;
	Q ind;
	int id;
	double get_rtime();	
	double t_0,t_00,tt,ttt,t_p,t_m,t_m1,t_m2,t_s,t_g,t_a;

	if ( !g ) {
		*r = 0; return;
	}
	pz_t_e = pz_t_d = pz_t_d1 = pz_t_c = 0;
	t_p = t_m = t_m1 = t_m2 = t_s = t_g = t_a = 0;

	denom = Denominator?Denominator:1;
	hmag = multiple*HMAG(g)/denom;
	kara_bit = kara_mag*27; /* XXX */
	if ( Dist ) {
		dist = BDY(Dist);
		ndist = length(dist);
	}
	sugar = g->sugar;

	dc = 0; dp = 0; rc = ONE; rp = g;
	MKSTR(imul,"dp_imul_index");

	for ( ; rp; ) {
		for ( u = 0, l = b; l; l = NEXT(l) ) {
			if ( dl_redble(BDY(rp)->dl,psh[(int)BDY(l)]) ) {
				t_0 = get_rtime();
				dp_load((int)BDY(l),&red);
				hr = (Q)BDY(rp)->c; hred = (Q)BDY(red)->c;
				gcdn(NM(hr),NM(hred),&gn);
				divsn(NM(hred),gn,&tn); NTOQ(tn,SGN(hred),cr);
				divsn(NM(hr),gn,&tn); NTOQ(tn,SGN(hr),cred);
				tt = get_rtime()-t_0; t_p += tt;

				t_0 = get_rtime();
				dp_subd(rp,red,&shift);
				t_00 = get_rtime();
				if ( Dist && ndist 
					&& HMAG(rp) > mpi_mag && p_mag((P)cr) > mpi_mag ) {
					if ( Print ) fprintf(asir_out,"~");
					dp_imul_d(rp,cr,&t);
				} else {
					if ( Print ) fprintf(asir_out,"_");
					muldc(CO,rp,(P)cr,&t);
				}
				ttt = get_rtime()-t_00; t_m1 += ttt/dp_nt(rp);

				t_00 = get_rtime();
				chsgnp((P)cred,(P *)&mcred);
				if ( Dist && ndist 
					&& HMAG(red) > mpi_mag && p_mag((P)mcred) > mpi_mag ) {
					if ( Print ) fprintf(asir_out,"=");
					dp_imul_d(red,mcred,&t1);
				} else {
					if ( Print ) fprintf(asir_out,"_");
					muldc(CO,red,(P)mcred,&t1);
				}
				ttt = get_rtime()-t_00; t_m2 += ttt/dp_nt(red);

				t_00 = get_rtime();
				muld(CO,shift,t1,&t2);
				addd(CO,t,t2,&u);
				tt = get_rtime(); t_m += tt-t_0;
				ttt = get_rtime(); t_s += ttt-t_00;

				psugar = (BDY(rp)->dl->td - BDY(red)->dl->td) + red->sugar;
				sugar = MAX(sugar,psugar);
				if ( !u ) {
					if ( dp )
						dp->sugar = sugar;
					*r = dp;
					goto final;
				}
				break;
			}
		}
		if ( u ) {
			if ( HMAG(u) > hmag ) {
				t_0 = get_rtime();
				if ( Dist && HMAG(u) > mpi_mag ) {
					if ( Print )
						fprintf(asir_out,"D");
					dp_ptozp_d(dist,ndist,u,&t);
				} else {
					if ( Print )
						fprintf(asir_out,"L");
					dp_ptozp_d(0,0,u,&t);
				}
				tt = get_rtime()-t_0; t_g += tt;
				t_0 = get_rtime();
				divsn(NM((Q)BDY(u)->c),NM((Q)BDY(t)->c),&cn); NTOQ(cn,1,cont);
				if ( Print ) {
					fprintf(asir_out,"(%d)",p_mag((P)cont)*100/p_mag((P)BDY(u)->c));
					fflush(asir_out);
				}
				mulq(cr,dc,&dcq);
				mulq(cont,rc,&rcq);
				gcdn(SAFENM(dcq),SAFENM(rcq),&gn);
				divsn(SAFENM(dcq),gn,&tn); NTOQ(tn,SGN(dcq),dc);
				divsn(SAFENM(rcq),gn,&tn); NTOQ(tn,SGN(rcq),rc);
				tt = get_rtime()-t_0; t_a += tt;
				rp = t;
				hmag = multiple*HMAG(rp)/denom;
			} else {
				t_0 = get_rtime();
				mulq(cr,dc,&dcq); dc = dcq;
				tt = get_rtime()-t_0; t_a += tt;
				rp = u;
			}
		} else if ( !full ) {
			if ( rp ) {
				MKDP(rp->nv,BDY(rp),t); t->sugar = sugar; rp = t;
			}
			*r = rp;
			goto final;
		} else {
			t_0 = get_rtime();
			mulq((Q)BDY(rp)->c,(Q)rc,(Q *)&c);
			gcdn(SAFENM(dc),SAFENM(c),&gn); NTOQ(gn,1,gcd);
			divsn(SAFENM(dc),gn,&tn); NTOQ(tn,SGN(dc),dcq);
			divsn(SAFENM(c),gn,&tn); NTOQ(tn,SGN(c),cq);
			muldc(CO,dp,(P)dcq,&t1);
			m = BDY(rp); NEWMP(mr); mr->dl = m->dl; mr->c = (P)cq;
			NEXT(mr) = 0; MKDP(rp->nv,mr,t); t->sugar = mr->dl->td;
			addd(CO,t,t1,&dp);
			dc = gcd;

			dp_rest(rp,&t); rp = t;
			tt = get_rtime()-t_0; t_a += tt;
		}
	}
	if ( dp )
		dp->sugar = sugar;
	*r = dp;
final:
	if ( Print )
		fprintf(asir_out,"(%.3g %.3g %.3g %.3g %.3g %.3g %.3g %.3g %.3g %.3g %.3g)",
			t_p,t_m,t_m1,t_m2,t_s,
			t_g,t_a,
			pz_t_e, pz_t_d, pz_t_d1, pz_t_c);
}

void dp_imul_d(p,q,rp)
DP p;
Q q;
DP *rp;
{
	int nsep,ndist,i,j,k,l,n;
	double t0,t1,t2;
	Q *s;
	pointer *b;
	VECT c,cs,ri;
	VECT *r;
	MP m;
	NODE tn,dist,n0,n1,n2;
	Obj dmy;
	STRING imul;

	extern LIST Dist;

	if ( !p || !q ) {
		*rp = 0; return;
	}
	dist = BDY(Dist);
	for ( tn = dist, ndist = 0; tn; tn = NEXT(tn), ndist++ );
	nsep = ndist + 1;
	for ( m = BDY(p), n = 0; m; m = NEXT(m), n++ );
	if ( n <= nsep ) {
		muldc(CO,p,(P)q,rp); return;
	}
	MKSTR(imul,"imulv");
	t0 = get_rtime();
	dp_dtov(p,&c);
	sepvect(c,nsep,&cs);
	r = (VECT *)CALLOC(nsep,sizeof(VECT *));
	for ( i = 0, tn = dist, b = BDY(cs); i < ndist; i++, tn = NEXT(tn) ) {
		n0 = mknode(4,BDY(tn),imul,b[i],q);
		Pox_rpc(n0,&dmy);
	}
	t1 = get_rtime();
	im_t_s += t1 - t0;
	imulv(b[i],q,&r[i]);
	t1 = get_rtime();
	for ( i = 0, tn = dist; i < ndist; i++, tn = NEXT(tn) ) {
		MKNODE(n0,BDY(tn),0);
		Pox_pop_local(n0,&r[i]);
		if ( OID(r[i]) == O_ERR ) {
			printexpr(CO,(Obj)r[i]);
			error("dp_imul_d : aborted");
		}
	}
	t2 = get_rtime();
	im_t_r += t2 - t1;
	s = (Q *)CALLOC(n,sizeof(Q));
	for ( i = j = 0; i < nsep; i++ ) {
		for ( k = 0, ri = r[i], l = ri->len; k < l; k++, j++ ) {
			s[j] = (Q)BDY(ri)[k];
		}
	}
	dp_vtod(s,p,rp);
}

void imulv(w,c,rp)
VECT w;
Q c;
VECT *rp;
{
	int n,i;
	VECT r;

	n = w->len;
	MKVECT(r,n); *rp = r;
	for ( i = 0; i < n; i++ )
		mulq((Q)BDY(w)[i],(Q)c,(Q *)&BDY(r)[i]);
}

DP_pairs updpairs( d, g, t)
DP_pairs d;
NODE /* of index */ g;
int t;
{
	register DP_pairs d1, dd, nd;
	struct oEGT tup0,tup1;
	int dl,dl1;

	if ( !g ) return d;
	get_eg(&tup0);
	if ( !NoCriB && d ) {
		dl = DPPlength(d);
		d = criterion_B( d, t );
		dl -= DPPlength(d); NBP += dl;
	}
	d1 = newpairs( g, t );
	if ( NEXT(d1) ) {
		dl = DPPlength(d1); TP += dl;
		d1 = criterion_M( d1 );
		dl1 = DPPlength(d1); NMP += (dl-dl1); dl = dl1;
		d1 = criterion_F( d1 );
		dl1 = DPPlength(d1); NFP += (dl-dl1); dl = dl1;
	} else
		dl = 1;
	for ( dd = 0; d1; d1 = nd ) {
		nd = NEXT(d1);
		if ( !criterion_2( d1->dp1, d1->dp2 ) ) {
			NEXT(d1) = dd;
			dd = d1;
		}
	}
	dl1 = DPPlength(dd); NDP += (dl-dl1);
	get_eg(&tup1);
	add_eg(&eg_up,&tup0,&tup1);
	if ( !(nd = d) ) return dd;
	while ( nd = NEXT(d1 = nd) ) ;
	NEXT(d1) = dd;
	return d;
}

DP_pairs newpairs( g, t )
NODE /* of index */ g;
register int t;
{
	register NODE r;
	register DL tdl = psh[t];
	register int ts;
	register DP_pairs p, last;
	int dp;
	register DL dl;
	register int s;

	ts = pss[t] - tdl->td;
	for ( last = 0, r = g; r; r = NEXT(r) ) {
		NEXT(p = NEWDP_pairs) = last;
		last = p;
		dp = p->dp1 = (int)BDY(r);  p->dp2 = t;
		p->lcm = lcm_of_DL(CNVars, dl = psh[dp], tdl, (DL)0 );
		p->sugar = (ts > (s = pss[dp] - dl->td) ? ts : s) + p->lcm->td;
	}
	return last;
}

DP_pairs criterion_B( d, s )
register DP_pairs d;
int s;
{
	register DP_pairs dd, p;
	register DL tij, t = psh[s], dltmp;

	if ( !d ) return 0;
	NEWDL( dltmp, CNVars );
	for ( dd = 0; d; d = p ) {
		p = NEXT(d),
		tij = d->lcm;
		if ( tij->td != lcm_of_DL(CNVars, tij, t, dltmp )->td
			|| !dl_equal(CNVars, tij, dltmp )
			|| (tij->td == lcm_of_DL(CNVars, psh[d->dp1], t, dltmp )->td
			    && dl_equal(CNVars, dltmp, tij ))
			|| (tij->td == lcm_of_DL(CNVars, psh[d->dp2], t, dltmp )->td
			    && dl_equal(CNVars, dltmp, tij )) ) {
			NEXT(d) = dd;
			dd = d;
		}
	}
	return dd;
}

DP_pairs criterion_M( d1 )
DP_pairs d1;
{
	register DP_pairs dd, e, d3, d2, p;
	register DL itdl, jtdl;
	register int itdltd, jtdltd;

	for ( dd = 0, e = d1; e; e = d3 ) {
		if ( !(d2 = NEXT(e)) ) {
			NEXT(e) = dd;
			return e;
		}
		itdltd = (itdl = e->lcm)->td;
		for ( d3 = 0; d2; d2 = p ) {
			p = NEXT(d2),
			jtdltd = (jtdl = d2->lcm)->td;
			if ( jtdltd == itdltd  )
				if ( dl_equal(CNVars, itdl, jtdl ) ) ;
				else if ( dl_redble( jtdl, itdl ) ) continue;
				else if ( dl_redble( itdl, jtdl ) ) goto delit;
				else ;
			else if ( jtdltd > itdltd )
				if ( dl_redble( jtdl, itdl ) ) continue;
				else ;
			else if ( dl_redble( itdl, jtdl ) ) goto delit;
			NEXT(d2) = d3;
			d3 = d2;
		}
		NEXT(e) = dd;
		dd = e;
		continue;
		/**/
	delit:	NEXT(d2) = d3;
		d3 = d2;
		for ( ; p; p = d2 ) {
			d2 = NEXT(p);
			NEXT(p) = d3;
			d3 = p;
		}
	}
	return dd;
}

static DP_pairs collect_pairs_of_hdlcm( d1, prest )
DP_pairs d1, *prest;
{
	register DP_pairs w, p, r, s;
	register DL ti;
	register int td;

	td = (ti = (w = d1)->lcm)->td;
	s = NEXT(w);
	NEXT(w) = 0;
	for ( r = 0; s; s = p ) {
		p = NEXT(s);
		if ( td == s->lcm->td && dl_equal(CNVars, ti, s->lcm ) )
		{
			NEXT(s) = w;
			w = s;
		} else {
			NEXT(s) = r;
			r = s;
		}
	}
	*prest = r;
	return w;
}

int criterion_2( dp1, dp2 )
int dp1, dp2;
{
	register int i, *d1, *d2;

	d1 = psh[dp1]->d,  d2 = psh[dp2]->d;
	for ( i = CNVars; --i >= 0; d1++, d2++ )
		if ( (*d1 <= *d2 ? *d1 : *d2) > 0  ) return 0;
	return 1;
}

DP_pairs criterion_F( d1 )
DP_pairs d1;
{
	DP_pairs rest, head;
	register DP_pairs last, p, r, w;
	register int s;

	for ( head = last = 0, p = d1; NEXT(p); ) {
		s = (r = w = collect_pairs_of_hdlcm( p, &rest ))->sugar;
		while ( w = NEXT(w) )
			if ( criterion_2( w->dp1, w->dp2 ) ) {
				r = w;
				break;
			} else if ( w->sugar < s ) s = (r = w)->sugar;
		if ( last ) NEXT(last) = r;
		else head = r;
		NEXT(last = r) = 0;
		if ( !(p = rest) ) return head;
	}
	if ( !last ) return p;
	NEXT(last) = p;
	return head;
}

NODE updbase(g,t)
NODE g;
int t;
{
	g = remove_reducibles(g,t);
	g = append_one(g,t);
	return g;
}

NODE /* of index */ remove_reducibles( nd, newdp )
NODE /* of index */ nd;
int newdp;
{
	register DL dl, dln;
	register NODE last, p, head;
	register int td;

	dl = psh[newdp];
	td = dl->td;
	for ( head = last = 0, p = nd; p; ) {
		dln = psh[(int)BDY(p)];
		if ( dln->td >= td && dl_redble( dln, dl ) ) {
			p = NEXT(p);
			if ( last ) NEXT(last) = p;
		} else {
			if ( !last ) head = p;
			p = NEXT(last = p);
		}
	}
	return head;
}

int dl_redble( dl1, dl2 )
DL dl1, dl2;
{
	register int n, *d1, *d2;

	for ( d1 = dl1->d, d2 = dl2->d, n = CNVars; --n >= 0; d1++, d2++ )
		if ( *d1 < *d2 ) return 0;
	return 1;
}

int gbcheck(f)
NODE f;
{
	int i;
	NODE r,g,gall;
	DP_pairs d,l;
	DP h,nf,dp1,dp2;
	struct oEGT tmp0,tmp1;

	if ( NoGC )
		return 1;
	for ( gall = g = 0, d = 0, r = f; r; r = NEXT(r) ) {
		i = (int)BDY(r);
		d = updpairs(d,g,i);
		g = updbase(g,i);
		gall = append_one(gall,i);
	}
	if ( Print || PrintShort ) {
		fprintf(asir_out,"gbcheck total %d pairs\n",DPPlength(d)); fflush(asir_out);
	}
	while ( d ) {
		l = d; d = NEXT(d);
		get_eg(&tmp0);
		dp_load(l->dp1,&dp1); dp_load(l->dp2,&dp2); dp_sp(dp1,dp2,&h);
		_dp_nf(gall,h,ps,1,&nf);
		get_eg(&tmp1); add_eg(&eg_gc,&tmp0,&tmp1);
		if ( Print || PrintShort ) {
			fprintf(asir_out,"."); fflush(asir_out);
		}
		if ( nf )
			return 0;
	}
	if ( Print || PrintShort )
		fprintf(asir_out,"\n");
	return 1;
}

int membercheck(f,x)
NODE f,x;
{
	DP g;
	struct oEGT tmp0,tmp1;

	if ( NoMC )
		return 1;
	if ( Print || PrintShort ) {
		fprintf(asir_out,"membercheck\n"); fflush(asir_out);
	}
	for ( ; f; f = NEXT(f) ) {
		get_eg(&tmp0);
		_dp_nf(x,(DP)BDY(f),ps,1,&g);
		get_eg(&tmp1); add_eg(&eg_mc,&tmp0,&tmp1);
		if ( Print ) {
			print_split_eg(&tmp0,&tmp1); fflush(asir_out);
		} else if ( PrintShort ) {
			fprintf(asir_out,"."); fflush(asir_out);
		}
		if ( g )
			return 0;
	}
	if ( Print || PrintShort )
		fprintf(asir_out,"\n");
	return 1;
}

void dp_set_flag(name,value)
Obj name,value;
{
	char *n;
	int v;

	if ( OID(name) != O_STR )
		return;
	n = BDY((STRING)name);
	if ( !strcmp(n,"Demand") ) {
		Demand = value ? BDY((STRING)value) : 0; return;
	}
	if ( !strcmp(n,"Dist") ) {
		Dist = (LIST)value; return;
	}
	if ( value && OID(value) != O_N )
		return;
	v = QTOS((Q)value);
	if ( !strcmp(n,"NoSugar") )
		NoSugar = v;
	else if ( !strcmp(n,"NoCriB") )
		NoCriB = v;
	else if ( !strcmp(n,"NoGC") )
		NoGC = v;
	else if ( !strcmp(n,"NoMC") )
		NoMC = v;
	else if ( !strcmp(n,"NoRA") )
		NoRA = v;
	else if ( !strcmp(n,"NoGCD") )
		NoGCD = v;
	else if ( !strcmp(n,"Top") )
		Top = v;
	else if ( !strcmp(n,"ShowMag") )
		ShowMag = v;
	else if ( !strcmp(n,"PrintShort") )
		PrintShort = v;
	else if ( !strcmp(n,"Print") )
		Print = v;
	else if ( !strcmp(n,"Stat") )
		Stat = v;
	else if ( !strcmp(n,"Reverse") )
		Reverse = v;
	else if ( !strcmp(n,"InterReduce") )
		InterReduce = v;
	else if ( !strcmp(n,"Multiple") )
		Multiple = v;
	else if ( !strcmp(n,"Denominator") )
		Denominator = v;
	else if ( !strcmp(n,"PtozpRA") )
		PtozpRA = v;
}

void dp_make_flaglist(list)
LIST *list;
{
	Q v;
	STRING name,path;
	NODE n,n1;

	STOQ(Multiple,v); MKNODE(n,v,0); MKSTR(name,"Multiple"); MKNODE(n1,name,n); n = n1;
	STOQ(Denominator,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"Denominator"); MKNODE(n1,name,n); n = n1;
	MKNODE(n1,Dist,n); n = n1; MKSTR(name,"Dist"); MKNODE(n1,name,n); n = n1;
	STOQ(InterReduce,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"InterReduce"); MKNODE(n1,name,n); n = n1;
	STOQ(Reverse,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"Reverse"); MKNODE(n1,name,n); n = n1;
	STOQ(Stat,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"Stat"); MKNODE(n1,name,n); n = n1;
	STOQ(Print,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"Print"); MKNODE(n1,name,n); n = n1;
	STOQ(PtozpRA,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"PtozpRA"); MKNODE(n1,name,n); n = n1;
	STOQ(ShowMag,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"ShowMag"); MKNODE(n1,name,n); n = n1;
	STOQ(Top,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"Top"); MKNODE(n1,name,n); n = n1;
	STOQ(NoGCD,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"NoGCD"); MKNODE(n1,name,n); n = n1;
	STOQ(NoRA,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"NoRA"); MKNODE(n1,name,n); n = n1;
	STOQ(NoMC,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"NoMC"); MKNODE(n1,name,n); n = n1;
	STOQ(NoGC,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"NoGC"); MKNODE(n1,name,n); n = n1;
	STOQ(NoCriB,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"NoCriB"); MKNODE(n1,name,n); n = n1;
	STOQ(NoSugar,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"NoSugar"); MKNODE(n1,name,n); n = n1;
	if ( Demand )
		MKSTR(path,Demand);
	else
		path = 0;
	MKNODE(n1,path,n); n = n1; MKSTR(name,"Demand"); MKNODE(n1,name,n); n = n1;
	MKLIST(*list,n);
}

#define DELIM '/'

#if defined(THINK_C)
#undef DELIM
#define DELIM ':'
#endif

void dp_save(index,p)
int index;
DP p;
{
	FILE *fp;
	char path[BUFSIZ];

	sprintf(path,"%s%c%d",Demand,DELIM,index);
#if defined(VISUAL) || defined(THINK_C)
	if ( !(fp = fopen(path,"wb") ) )
#else
	if ( !(fp = fopen(path,"w") ) )
#endif
		error("dp_save : cannot open a file");
	savevl(fp,VC); saveobj(fp,(Obj)p); fclose(fp);
}

void dp_load(index,p)
int index;
DP *p;
{
	FILE *fp;
	char path[BUFSIZ];

	if ( !Demand || ps[index] )
		*p = ps[index];
	else {
		sprintf(path,"%s%c%d",Demand,DELIM,index);
#if defined(VISUAL) || defined(THINK_C)
		if ( !(fp = fopen(path,"rb") ) )
#else
		if ( !(fp = fopen(path,"r") ) )
#endif
			error("dp_load : cannot open a file");
		skipvl(fp); loadobj(fp,(Obj *)p); fclose(fp);
	}
}

int dp_load_t(index,p)
int index;
DP *p;
{
	FILE *fp;
	char path[BUFSIZ];

	sprintf(path,"%s%c%d",Demand,DELIM,index);
#if defined(VISUAL) || defined(THINK_C)
	if ( !(fp = fopen(path,"rb") ) )
#else
	if ( !(fp = fopen(path,"r") ) )
#endif
		return 0;
	else {
		skipvl(fp); loadobj(fp,(Obj *)p); fclose(fp); return 1;
	}
}

void init_stat() {
	init_eg(&eg_up); init_eg(&eg_sp); init_eg(&eg_spm);
	init_eg(&eg_nf); init_eg(&eg_nfm); init_eg(&eg_znfm);
	init_eg(&eg_pz); init_eg(&eg_np); init_eg(&eg_mp);
	init_eg(&eg_ra); init_eg(&eg_mc); init_eg(&eg_gc);
	ZR = NZR = TP = NBP = NMP = NFP = NDP = 0;
}

void print_stat() {
	if ( !Print && !Stat )
		return;
	print_eg("UP",&eg_up); print_eg("SP",&eg_sp); print_eg("SPM",&eg_spm);
	print_eg("NF",&eg_nf); print_eg("NFM",&eg_nfm); print_eg("ZNFM",&eg_znfm);
	print_eg("PZ",&eg_pz); print_eg("NP",&eg_np); print_eg("MP",&eg_mp);
	print_eg("RA",&eg_ra); print_eg("MC",&eg_mc); print_eg("GC",&eg_gc);
	fprintf(asir_out,"T=%d,B=%d M=%d F=%d D=%d ZR=%d NZR=%d\n",TP,NBP,NMP,NFP,NDP,ZR,NZR);
}

/*
 * Old codes 
 */

#if 0
void _dp_nf_ptozp(b,g,ps,full,multiple,rp)
NODE b;
DP g;
DP *ps;
int full,multiple;
DP *rp;
{
	DP u,p,d,s,t,dmy;
	NODE l;
	MP m,mr;
	int i,n;
	int hmag,denom;
	int sugar,psugar;
	NODE dist;
	int ndist;
	int kara_bit;
	extern int kara_mag;

	if ( !g ) {
		*rp = 0; return;
	}
	denom = Denominator?Denominator:1;
	hmag = multiple*HMAG(g)/denom;
	kara_bit = kara_mag*27; /* XXX */
	if ( Dist ) {
		dist = BDY(Dist);
		ndist = length(dist);
	}
	sugar = g->sugar;
	for ( d = 0; g; ) {
		for ( u = 0, l = b; l; l = NEXT(l) ) {
			if ( dl_redble(BDY(g)->dl,psh[(int)BDY(l)]) ) {
				dp_load((int)BDY(l),&p);
				dp_red(d,g,p,&t,&u,&dmy);
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
					if ( Dist && HMAG(g) > kara_bit )
						dp_ptozp2_d(dist,ndist,d,g,&t,&u);
					else
						dp_ptozp2(d,g,&t,&u);
					d = t; g = u;
					hmag = multiple*HMAG(d)/denom;
				}
			} else {
				if ( HMAG(g) > hmag ) {
#if INET
					if ( Dist && HMAG(g) > kara_bit ) {
						dp_ptozp_d(dist,ndist,g,&t);
					} else
#endif
						dp_ptozp(g,&t);
					g = t;
					hmag = multiple*HMAG(g)/denom;
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

int criterion_2(dp1,dp2)
{
	DP g1,g2,g,s1,s2;

	monomial_gcd(ps[dp1],&g1); monomial_gcd(ps[dp2],&g2);
	dp_gcd(g1,g2,&g);
	dp_subd(ps[dp1],g,&s1); dp_subd(ps[dp2],g,&s2);
	return _criterion_2(s1,s2);
}

int _criterion_2( dp1, dp2 )
DP dp1, dp2;
{
	register int i, *d1, *d2;

	d1 = BDY(dp1)->dl->d; d2 = BDY(dp2)->dl->d;
	for ( i = CNVars; --i >= 0; d1++, d2++ )
		if ( (*d1 <= *d2 ? *d1 : *d2) > 0  ) return 0;
	return 1;
}

void dp_gcd(p1,p2,rp)
DP p1,p2;
DP *rp;
{
	int i,n,td;
	DL d1,d2,d;
	MP m;
	DP s;

	n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
	NEWDL(d,n);
	for ( i = 0, td = 0; i < n; i++ ) {
		d->d[i] = MIN(d1->d[i],d2->d[i]);
		td += d->d[i];
	}
	d->td = td;
	NEWMP(m); m->dl = d; m->c = (P)ONE; NEXT(m) = 0; MKDP(n,m,s); s->sugar = d->td;
	*rp = s;
}

void monomial_gcd(p,rp)
DP p;
DP *rp;
{
	int n,i,td;
	DL d,d1;
	MP m;
	DP s;

	n = p->nv; m = BDY(p); d = m->dl;
	NEWDL(d1,n);
	for ( i = 0; i < n; i++ )
		d1->d[i] = d->d[i];
	for ( m = NEXT(m); m; m = NEXT(m) ) {
		d = m->dl;
		for ( i = 0; i < n; i++ )
			d1->d[i] = MIN(d1->d[i],d->d[i]);
	}
	for ( i = 0, td = 0; i < n; i++ )
		td += d1->d[i];
	NEWMP(m); m->dl = d1; m->c = (P)ONE; NEXT(m) = 0; MKDP(n,m,s); s->sugar = d->td;
	*rp = s;
}
#endif
