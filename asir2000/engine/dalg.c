/*
 * $OpenXM$
*/

#include "ca.h"
#include "base.h"

typedef struct oNumberField {
	int n;
	int dim;
	VL vl;
	P *defpoly;
	DP *mb;
	DP *ps;
	NODE ind;
	struct order_spec *spec;
} *NumberField;

typedef struct oDAlg {
	short id;
	char nid;
	char pad;
	DP nm;
	Q dn;
} *DAlg;

#define N_DA 11
#define NEWDAlg(r) ((r)=(DAlg)MALLOC(sizeof(struct oDAlg)),OID(r)=O_N,NID(r)=N_DA)
#define MKDAlg(dp,dn,r) (NEWDAlg(r),(r)->nm = (dp),(r)->dn=(dn))

static NumberField current_numberfield;
extern struct order_spec *dp_current_spec;

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
		MKNODE(b2,(pointer)ps[i],&hlist); hlist = b2;
	}
	initd(current_spec);	
	nf->ind = b;
	dp_base(hlist,&mblist);
	nf->dim = dim = length(mblist);
	nf->mb = mb = (DP *)MALLOC(dim*sizeof(DP));
	for ( i = 0, t = mblist; t; t = NEXT(t), i++ )
		mb[i] = (DP)BDY(mblist);	
}

void algtodalg(Alg a,DAlg *r)
{
	P ap,p,p1;
	Q c,dn,nm;
	DP dp;
	DAlg da;
	NumberField nf;
	struct order_spec *current_spec;

	if ( !(nf=current_numberfield) )
		error("algtodalg : current_numberfield is not set");
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
	current_spec = dp_current_spec;
	initd(nf->spec);
	ptod(ALG,nf->vl,p,&dp);
	MKDAlg(dp,dn,da);
	*r = da;
}

void dalgtoalg(DAlg da,Num *a)
{
	if ( !current_numberfield )
		error("algtodalg : current_numberfield is not set");
}

void simpdalg(DAlg da,DAlg *r)
{
	if ( !current_numberfield )
		error("algtodalg : current_numberfield is not set");
}

void adddalg(DAlg a,DAlg b,DAlg *c)
{
	if ( !current_numberfield )
		error("algtodalg : current_numberfield is not set");
}

void subdalg(DAlg a,DAlg b,DAlg *c)
{
	if ( !current_numberfield )
		error("algtodalg : current_numberfield is not set");
}

void divdalg(DAlg a,DAlg b,DAlg *c)
{
	if ( !current_numberfield )
		error("algtodalg : current_numberfield is not set");
}

void invdalg(DAlg a,DAlg *c)
{
	if ( !current_numberfield )
		error("algtodalg : current_numberfield is not set");
}

void chsgndalg(DAlg a,DAlg *c)
{
}

void pwrdalg(DAlg a,Q b,DAlg *c)
{
	if ( !current_numberfield )
		error("algtodalg : current_numberfield is not set");
}

int cmpgdalg(DAlg a,DAlg b)
{
	if ( !current_numberfield )
		error("algtodalg : current_numberfield is not set");
}
