/* $OpenXM$ */

#include "ca.h"
#include "parse.h"

void addquote(vl,a,b,c)
VL vl;
QUOTE a,b;
QUOTE *c;
{
	FNODE fn;

	fn = mkfnode(3,I_BOP,addfs,BDY(a),BDY(b));
	MKQUOTE(*c,fn);
}

void subquote(vl,a,b,c)
VL vl;
QUOTE a,b;
QUOTE *c;
{
	FNODE fn;

	fn = mkfnode(3,I_BOP,subfs,BDY(a),BDY(b));
	MKQUOTE(*c,fn);
}

void mulquote(vl,a,b,c)
VL vl;
QUOTE a,b;
QUOTE *c;
{
	FNODE fn;

	fn = mkfnode(3,I_BOP,mulfs,BDY(a),BDY(b));
	MKQUOTE(*c,fn);
}

void divquote(vl,a,b,c)
VL vl;
QUOTE a,b;
QUOTE *c;
{
	FNODE fn;

	fn = mkfnode(3,I_BOP,divfs,BDY(a),BDY(b));
	MKQUOTE(*c,fn);
}

void pwrquote(vl,a,b,c)
VL vl;
QUOTE a,b;
QUOTE *c;
{
	FNODE fn;

	if ( !b || OID(b) != O_QUOTE )
		error("pwrquote : invalid argument");
	fn = mkfnode(3,I_BOP,pwrfs,BDY(a),BDY(b));
	MKQUOTE(*c,fn);
}

void chsgnquote(a,c)
QUOTE a;
QUOTE *c;
{
	FNODE fn;

	fn = mkfnode(3,I_BOP,subfs,0,BDY(a));
	MKQUOTE(*c,fn);
}

void polytoquote(), dctoquote(), vartoquote();

void objtoquote(a,c)
Obj a;
QUOTE *c;
{
	QUOTE nm,dn;

	if ( !a ) {
		MKQUOTE(*c,mkfnode(1,I_FORMULA,(pointer)a));
		return;
	}
	switch ( OID(a) ) {
		case O_N:
			MKQUOTE(*c,mkfnode(1,I_FORMULA,(pointer)a));
			break;
		case O_P:
			polytoquote((P)a,c);
			break;
		case O_R:
			polytoquote(NM((R)a),&nm);
			polytoquote(DN((R)a),&dn);
			divquote(CO,nm,dn,c);
			break;
		case O_QUOTE:
			*c = (QUOTE)a;
			break;
		default:
			error("objtoquote : not implemented");
	}
}

void polytoquote(a,c)
P a;
QUOTE *c;
{
	DCP dc,t;
	DCP *dca;
	int n,i;
	QUOTE v,r,s,u;

	if ( !a || (OID(a) == O_N) ) {
		MKQUOTE(*c,mkfnode(1,I_FORMULA,(pointer)a));
		return;
	}
	dc = DC((P)a);
	vartoquote(VR((P)a),&v);
	for ( t = dc, n = 0; t; t = NEXT(t), n++ );
	dca = (DCP *)ALLOCA(n*sizeof(DCP));
	for ( t = dc, i = 0; t; t = NEXT(t), i++ )
		dca[i] = t;
	dctoquote(dca[n-1],v,&r);
	for ( i = n-2; i >= 0; i-- ) {
		dctoquote(dca[i],v,&s);
		addquote(CO,s,r,&u);
		r = u;
	}
	*c = r;
}

void dctoquote(dc,v,c)
DCP dc;
QUOTE v;
QUOTE *c;
{
	QUOTE r,d,s,u;

	objtoquote(COEF(dc),&r);
	if ( DEG(dc) ) {
		objtoquote(DEG(dc),&d);
		pwrquote(CO,v,d,&s);
		mulquote(CO,r,s,&u);
		r = u;
	}
	*c = r;
}

void vartoquote(v,c)
V v;
QUOTE *c;
{
	P x;
	PF pf;
	PFAD ad;
	QUOTE a,b;
	int i;
	FUNC f;
	NODE t,t1;

	if ( NAME(v) ) {
		MKV(v,x);
		MKQUOTE(*c,mkfnode(1,I_FORMULA,(pointer)x));
	} else if ( (vid)v->attr == V_PF ) {
		/* pure function */
		pf = ((PFINS)v->priv)->pf;
		ad = ((PFINS)v->priv)->ad;
		if ( !strcmp(NAME(pf),"pow") ) {
			/* pow(a,b) = a^b */
			objtoquote(ad[0].arg,&a); objtoquote(ad[1].arg,&b);
			pwrquote(CO,a,b,c);
		} else {
			for ( i = 0; i < pf->argc; i++ )
				if ( ad[i].d )
					break;
			if ( i < pf->argc )
				error("vartoquote : not implemented");
			gen_searchf(NAME(pf),&f);
			t = 0;
			for ( i = pf->argc-1; i >= 0; i-- ) {
				objtoquote(ad[i].arg,&a);
				MKNODE(t1,BDY(a),t);
				t = t1;
			}
			MKQUOTE(*c,mkfnode(2,I_FUNC,f,mkfnode(1,I_LIST,t)));
		}
	}
}
