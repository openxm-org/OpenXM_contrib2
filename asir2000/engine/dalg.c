/*
 * $OpenXM: OpenXM_contrib2/asir2000/engine/dalg.c,v 1.3 2004/12/02 13:48:43 noro Exp $
*/

#include "ca.h"
#include "base.h"

static NumberField current_numberfield;
extern struct order_spec *dp_current_spec;
void simpdalg(DAlg da,DAlg *r);
void invdalg(DAlg a,DAlg *c);
void rmcontdalg(DAlg a, DAlg *c);

void setfield_dalg(NODE alist)
{
	NumberField nf;
	VL vl,vl1,vl2;
	int n,i,dim;
	Alg *gen;
	P *defpoly;
	P p;
	Q c,iq,two;
	DP *ps,*mb;
	DP one;
	NODE t,b,b1,b2,hlist,mblist;
	struct order_spec *current_spec;

	nf = (NumberField)MALLOC(sizeof(struct oNumberField));
	current_numberfield = nf;
	vl = 0;
	for ( t = alist; t; t = NEXT(t) ) {
		clctalg(BDY((Alg)BDY(t)),&vl1);
		mergev(ALG,vl,vl1,&vl2); vl = vl2;
	}
	for ( n = 0, vl1 = vl; vl1; vl1 = NEXT(vl1), n++ ); 
	nf->n = n;
	nf->vl = vl;
	nf->defpoly = defpoly = (P *)MALLOC(n*sizeof(P));
	nf->ps = ps = (DP *)MALLOC(n*sizeof(DP));
	current_spec = dp_current_spec;
	STOQ(2,two);
	create_order_spec(0,(Obj)two,&nf->spec);
	initd(nf->spec);	
	for ( b = hlist = 0, i = 0, vl1 = vl; i < n; vl1 = NEXT(vl1), i++ ) {
		ptozp(vl1->v->attr,1,&c,&defpoly[i]);
		ptod(ALG,vl,defpoly[i],&ps[i]);
		STOQ(i,iq); MKNODE(b1,(pointer)iq,b); b = b1;
		MKNODE(b2,(pointer)ps[i],hlist); hlist = b2;
	}
	ptod(ALG,vl,(P)ONE,&one);
	MKDAlg(one,ONE,nf->one);
	nf->ind = b;
	dp_mbase(hlist,&mblist);
	initd(current_spec);	
	nf->dim = dim = length(mblist);
	nf->mb = mb = (DP *)MALLOC(dim*sizeof(DP));
	for ( i = 0, t = mblist; t; t = NEXT(t), i++ )
		mb[i] = (DP)BDY(t);	
}

void qtodalg(Q q,DAlg *r)
{
	NumberField nf;
	Q t;
	DP nm;

	if ( !(nf=current_numberfield) )
		error("qtodalg : current_numberfield is not set");
	if ( !q )
		*r = 0;
	else if ( NID(q) == N_DA )
		*r = (DAlg)q;
	else if ( NID(q) == N_Q ) {
		if ( INT(q) ) {
			muldc(CO,nf->one->nm,(P)q,&nm);
			MKDAlg(nm,ONE,*r);
		} else {
			NTOQ(NM(q),SGN(q),t);
			muldc(CO,nf->one->nm,(P)t,&nm);
			NTOQ(DN(q),1,t);
			MKDAlg(nm,t,*r);
		}
	} else
		error("qtodalg : invalid argument");
}

void algtodalg(Alg a,DAlg *r)
{
	P ap,p,p1;
	Q c,c1,d1,dn,nm;
	DP dp;
	DAlg da;
	NumberField nf;
	struct order_spec *current_spec;
	VL vl,tvl,svl;
	V v;

	if ( !(nf=current_numberfield) )
		error("algtodalg : current_numberfield is not set");
	if ( !a ) {
		*r = 0;
		return;
	}
	switch (NID((Num)a) ) {
		case N_Q:
			c = (Q)a;
			if ( INT(c) ) {
				muldc(CO,nf->one->nm,(P)c,&dp);
				MKDAlg(dp,ONE,*r);
			} else {
				NTOQ(NM(c),SGN(c),c1);
				NTOQ(DN(c),1,d1);
				muldc(CO,nf->one->nm,(P)c1,&dp);
				MKDAlg(dp,c1,*r);
			}
			break;
		case N_A:
			ap = (P)BDY(a);
			ptozp(ap,1,&c,&p);
			if ( INT(c) ) {
				p = ap;
				dn = ONE;
			} else {
				NTOQ(NM(c),SGN(c),nm);
				NTOQ(DN(c),1,dn);
				mulpq(p,(P)nm,&p1); p = p1;
			}
			current_spec = dp_current_spec; initd(nf->spec);
			get_vars(p,&vl);
			for ( tvl = vl; tvl; tvl = NEXT(tvl) ) {
				v = tvl->v;
				for ( svl = nf->vl; svl; svl = NEXT(svl) )
					if ( v == svl->v )
						break;
				if ( !svl )
					error("algtodalg : incompatible numberfield");
			}
			ptod(ALG,nf->vl,p,&dp);
			MKDAlg(dp,dn,da);
			simpdalg(da,r);
			break;
		default:
			error("algtodalg : invalid argument");
			break;
	}
}

void dalgtoalg(DAlg da,Alg *r)
{
	NumberField nf;
	P p,p1;
	Q inv;

	if ( !(nf=current_numberfield) )
		error("dalgtoalg : current_numberfield is not set");
	dtop(ALG,nf->vl,da->nm,&p);
	invq(da->dn,&inv);
	mulpq(p,(P)inv,&p1);
	MKAlg(p1,*r);
}

void simpdalg(DAlg da,DAlg *r)
{
	NumberField nf;
	DP nm;
	DAlg d;
	Q dn,dn1;

	if ( !(nf=current_numberfield) )
		error("simpdalg : current_numberfield is not set");
	if ( !da ) {
		*r = 0;
		return;
	}
	dp_true_nf(nf->ind,da->nm,nf->ps,1,&nm,&dn);
	mulq(da->dn,dn,&dn1);
	MKDAlg(nm,dn1,d);
	rmcontdalg(d,r);
}

void adddalg(DAlg a,DAlg b,DAlg *c)
{
	NumberField nf;
	Q dna,dnb,a1,b1,dn,g;
	N an,bn,gn;
	DAlg t;
	DP ta,tb,nm;
	struct order_spec *current_spec;

	if ( !(nf=current_numberfield) )
		error("adddalg : current_numberfield is not set");
	if ( !a )
		*c = b;
	else if ( !b )
		*c = a;
	else {
		qtodalg((Q)a,&t); a = t; qtodalg((Q)b,&t); b = t;
		dna = a->dn;
		dnb = b->dn;
		gcdn(NM(dna),NM(dnb),&gn);
		divsn(NM(dna),gn,&an); divsn(NM(dnb),gn,&bn);
		NTOQ(an,SGN(dna),a1); NTOQ(bn,SGN(dnb),b1);
		/* nma/dna+nmb/dnb = (nma*b1+nmb*a1)/(dna*b1) */
		muldc(CO,a->nm,(P)b1,&ta); muldc(CO,b->nm,(P)a1,&tb);
		current_spec = dp_current_spec; initd(nf->spec);
		addd(CO,ta,tb,&nm);
		initd(current_spec);
		if ( !nm )
			*c = 0;
		else {
			mulq(dna,b1,&dn);
			MKDAlg(nm,dn,*c);
		}
	}
}

void subdalg(DAlg a,DAlg b,DAlg *c)
{
	NumberField nf;
	Q dna,dnb,a1,b1,dn,g;
	N an,bn,gn;
	DP ta,tb,nm;
	DAlg t;
	struct order_spec *current_spec;

	if ( !(nf=current_numberfield) )
		error("subdalg : current_numberfield is not set");
	if ( !a )
		*c = b;
	else if ( !b )
		*c = a;
	else {
		qtodalg((Q)a,&t); a = t; qtodalg((Q)b,&t); b = t;
		dna = a->dn;
		dnb = b->dn;
		gcdn(NM(dna),NM(dnb),&gn);
		divsn(NM(dna),gn,&an); divsn(NM(dnb),gn,&bn);
		NTOQ(an,SGN(dna),a1); NTOQ(bn,SGN(dnb),b1);
		/* nma/dna-nmb/dnb = (nma*b1-nmb*a1)/(dna*b1) */
		muldc(CO,a->nm,(P)b1,&ta); muldc(CO,b->nm,(P)a1,&tb);
		current_spec = dp_current_spec; initd(nf->spec);
		subd(CO,ta,tb,&nm);
		initd(current_spec);
		if ( !nm )
			*c = 0;
		else {
			mulq(dna,b1,&dn);
			MKDAlg(nm,dn,*c);
		}
	}
}

void muldalg(DAlg a,DAlg b,DAlg *c)
{
	NumberField nf;
	DP nm;
	Q dn;
	DAlg t;
	struct order_spec *current_spec;

	if ( !(nf=current_numberfield) )
		error("muldalg : current_numberfield is not set");
	if ( !a || !b )
		*c = 0;
	else {
		qtodalg((Q)a,&t); a = t; qtodalg((Q)b,&t); b = t;
		current_spec = dp_current_spec; initd(nf->spec);
		muld(CO,a->nm,b->nm,&nm);
		initd(current_spec);
		mulq(a->dn,b->dn,&dn);
		MKDAlg(nm,dn,t);
		simpdalg(t,c);
	}
}


void divdalg(DAlg a,DAlg b,DAlg *c)
{
	DAlg inv,t;

	if ( !current_numberfield )
		error("divdalg : current_numberfield is not set");
	if ( !b )
		error("divdalg : division by 0");
	if ( !a )
		c = 0;
	else {
		qtodalg((Q)a,&t); a = t; qtodalg((Q)b,&t); b = t;
		invdalg(b,&inv);
		muldalg(a,inv,c);
	}
}

void rmcontdalg(DAlg a, DAlg *r)
{
	DP u,u1;
	Q cont,c,d;
	N gn,cn,dn;

	if ( !a )
		*r = a;
	else {
		dp_ptozp(a->nm,&u);
		divq((Q)BDY(a->nm)->c,(Q)BDY(u)->c,&cont);
		gcdn(NM(cont),NM(a->dn),&gn);
		divsn(NM(cont),gn,&cn); NTOQ(cn,SGN(cont),c);
		divsn(NM(a->dn),gn,&dn); NTOQ(dn,SGN(a->dn),d);
		muldc(CO,u,(P)c,&u1);
		MKDAlg(u1,d,*r);
	}
}

void invdalg(DAlg a,DAlg *c)
{
	NumberField nf;
	int dim,n,i,j;
	DP *mb;
	DP m,d,u;
	N ln,gn,qn;
	DAlg *simp;
	DAlg t,a0,r;
	Q dn,dnsol,mul;
	MAT mobj,sol;
	Q **mat,**solmat;
	MP mp0,mp;
	int *rinfo,*cinfo;
	struct order_spec *current_spec;

	if ( !(nf=current_numberfield) )
		error("invdalg : current_numberfield is not set");
	if ( !a )
		error("invdalg : division by 0");
	else if ( NID(a) == N_Q ) {
		invq((Q)a,&dn); *c = (DAlg)dn;
		return;
	}
	dim = nf->dim;
	mb = nf->mb;
	n = nf->n;
	ln = ONEN;
	MKDAlg(a->nm,ONE,a0);
	simp = (DAlg *)ALLOCA(dim*sizeof(DAlg));
	current_spec = dp_current_spec; initd(nf->spec);
	for ( i = dim-1; i >= 0; i-- ) {
		m = mb[i];
		for ( j = i+1; j < dim; j++ )
			if ( dp_redble(m,mb[j]) )
				break;
		if ( j < dim ) {
			dp_subd(m,mb[j],&d);
			muld(CO,d,simp[j]->nm,&u);
			MKDAlg(u,simp[j]->dn,t);
			simpdalg(t,&simp[i]);
		} else {
			MKDAlg(m,ONE,t);
			muldalg(t,a0,&simp[i]);
		}
		gcdn(NM(simp[i]->dn),ln,&gn); divsn(ln,gn,&qn);
		muln(NM(simp[i]->dn),qn,&ln);
	}
	initd(current_spec);
	NTOQ(ln,1,dn);
	MKMAT(mobj,dim,dim+1);
	mat = (Q **)BDY(mobj);
	mulq(dn,a->dn,&mat[dim-1][dim]);
	for ( j = 0; j < dim; j++ ) {
		divq(dn,simp[j]->dn,&mul);
		for ( i = 0, mp = BDY(simp[j]->nm); mp && i < dim; i++ )
			if ( dl_equal(n,BDY(mb[i])->dl,mp->dl) ) {
				mulq(mul,(Q)mp->c,&mat[i][j]);
				mp = NEXT(mp);
			}
	}
	generic_gauss_elim_hensel(mobj,&sol,&dnsol,&rinfo,&cinfo);
	solmat = (Q **)BDY(sol);
	for ( i = 0, mp0 = 0; i < dim; i++ )
		if ( solmat[i][0] ) {
			NEXTMP(mp0,mp);
			mp->c = (P)solmat[i][0];
			mp->dl = BDY(mb[i])->dl;
		}
	NEXT(mp) = 0; MKDP(n,mp0,u);
	MKDAlg(u,dnsol,r);
	rmcontdalg(r,c);
}

void chsgndalg(DAlg a,DAlg *c)
{
	DP nm;
	Q t;

	if ( !a ) *c = 0;
	else if ( NID(a) == N_Q ) {
		chsgnq((Q)a,&t); *c = (DAlg)t;
	} else {
		chsgnd(a->nm,&nm);
		MKDAlg(nm,a->dn,*c);
	}
}

void pwrdalg(DAlg a,Q e,DAlg *c)
{
	NumberField nf;
	DAlg t,z,y;
	Q q;
	N en,qn;
	int r;

	if ( !(nf=current_numberfield) )
		error("pwrdalg : current_numberfield is not set");
	if ( !a )
		*c = !e ? (DAlg)ONE : 0;
	else if ( NID(a) == N_Q ) {
		pwrq((Q)a,e,&q); *c = (DAlg)q;
	} else if ( !e )
		*c = nf->one;
	else if ( UNIQ(e) )
		*c = a;
	else {
		if ( SGN(e) < 0 ) {
			invdalg(a,&t); a = t;
		}
		en = NM(e);
		y = nf->one;
		z = a;
		while ( 1 ) {
			r = divin(en,2,&qn); en = qn;
			if ( r ) {
				muldalg(z,y,&t); y = t;
				if ( !en ) {
					*c = y;
					return;
				}
			}
			muldalg(z,z,&t); z = t;
		}
	}
}

int cmpdalg(DAlg a,DAlg b)
{
	DAlg c;

	subdalg(a,b,&c);
	if ( !c ) return 0;
	else
		return SGN((Q)BDY(c->nm)->c);
}
