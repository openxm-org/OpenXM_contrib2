/* $OpenXM: OpenXM_contrib2/asir2000/parse/quote.c,v 1.4 2001/09/04 05:14:04 noro Exp $ */

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
void dptoquote(), mptoquote();

void objtoquote(a,c)
Obj a;
QUOTE *c;
{
	QUOTE nm,dn;
	NODE arg,t0,t,t1,t2,t3;
	FNODE fn;
	Obj obj;
	Obj *b;
	Obj **m;
	int i,j,len,row,col;
	Q q,qrow,qcol;
	FUNC f;

	if ( !a ) {
		MKQUOTE(*c,mkfnode(1,I_FORMULA,(pointer)a));
		return;
	}
	switch ( OID(a) ) {
		case O_N:
			if ( negative_number((Num)a) ) {
				arf_chsgn(a,&obj);
				MKQUOTE(*c,mkfnode(1,I_MINUS,
					mkfnode(1,I_FORMULA,(pointer)obj)));
			} else {
				MKQUOTE(*c,mkfnode(1,I_FORMULA,(pointer)a));
			}
			break;
		case O_STR:
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
		case O_LIST:
			t0 = 0;
			for ( arg = BDY((LIST)a); arg; arg = NEXT(arg) ) {
				NEXTNODE(t0,t);
				objtoquote(BDY(arg),&nm);
				BDY(t) = BDY(nm);
			}
			if ( t0 )
				NEXT(t) = 0;
			MKQUOTE(*c,mkfnode(1,I_LIST,t0));
			break;
		case O_VECT:
			len = ((VECT)a)->len;
			b = (Obj *)BDY(((VECT)a));
			t = 0;
			for ( i = len-1; i >= 0; i-- ) {
				objtoquote(b[i],&nm);
				MKNODE(t1,BDY(nm),t);
				t = t1;
			}
			STOQ(len,q); 
			t = mknode(2,mkfnode(1,I_FORMULA,q),mkfnode(1,I_LIST,t));
			gen_searchf("vector",&f);
			MKQUOTE(*c,mkfnode(2,I_FUNC,f,mkfnode(1,I_LIST,t)));
			break;
		case O_MAT:
			row = ((MAT)a)->row;
			col = ((MAT)a)->row;
			m = (Obj **)BDY(((MAT)a));
			t2 = 0;
			for ( i = row-1; i >= 0; i-- ) {
				t = 0;
				for ( j = col-1; j >= 0; j-- ) {
					objtoquote(m[i][j],&nm);
					MKNODE(t1,BDY(nm),t);
					t = t1;
				}
				fn = mkfnode(1,I_LIST,t);
				MKNODE(t3,fn,t2);
				t2 = t3;
			}
			fn = mkfnode(1,I_LIST,t2);

			STOQ(row,qrow); 
			STOQ(col,qcol); 
			t = mknode(3,
				mkfnode(1,I_FORMULA,qrow),mkfnode(1,I_FORMULA,qcol),fn);
			gen_searchf("matrix",&f);
			MKQUOTE(*c,mkfnode(2,I_FUNC,f,mkfnode(1,I_LIST,t)));
			break;
		case O_DP:
			dptoquote((DP)a,c);
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

void dptoquote(a,c)
DP a;
QUOTE *c;
{
	MP t;
	MP *m;
	int i,n,nv;
	QUOTE s,r,u;

	if ( !a ) {
		MKQUOTE(*c,mkfnode(1,I_FORMULA,(pointer)a));
		return;
	}
	for ( t = BDY(a), n = 0; t; t = NEXT(t), n++ );
	m = (MP *)ALLOCA(n*sizeof(MP));
	for ( t = BDY(a), i = 0; t; t = NEXT(t), i++ )
		m[i] = t;
	nv = NV(a);
	mptoquote(m[n-1],nv,&r);
	for ( i = n-2; i >= 0; i-- ) {
		mptoquote(m[i],nv,&s);
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

	if ( UNIQ(COEF(dc)) ) {
		if ( DEG(dc) ) {
			if ( UNIQ(DEG(dc)) )
				*c = v;
			else {
				objtoquote(DEG(dc),&d);
				pwrquote(CO,v,d,c);
			}
		} else
			objtoquote(ONE,c);	
	} else {
		objtoquote(COEF(dc),&u);
		if ( DEG(dc) ) {
			if ( UNIQ(DEG(dc)) )
				s = v;
			else {
				objtoquote(DEG(dc),&d);
				pwrquote(CO,v,d,&s);
			}
			mulquote(CO,u,s,c);
		} else
			*c = u;
	}
}

void mptoquote(m,n,c)
MP m;
int n;
QUOTE *c;
{
	QUOTE s,u;
	NODE t,t1;
	FNODE f;
	Q q;
	DL dl;
	int i;

	objtoquote(C(m),&s);
	dl = m->dl;
	for ( i = n-1; i >= 0; i-- ) {
		STOQ(dl->d[i],q);
		f = mkfnode(1,I_FORMULA,q);
		MKNODE(t1,f,t);
		t = t1;
	}
	MKQUOTE(u,mkfnode(1,I_EV,t));
	mulquote(CO,s,u,c);
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
