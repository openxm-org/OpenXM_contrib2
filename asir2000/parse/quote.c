/* $OpenXM: OpenXM_contrib2/asir2000/parse/quote.c,v 1.14 2004/07/07 07:40:19 noro Exp $ */

#include "ca.h"
#include "parse.h"

void addquote(VL vl,QUOTE a,QUOTE b,QUOTE *c)
{
	FNODE fn;

	fn = mkfnode(3,I_BOP,addfs,BDY(a),BDY(b));
	MKQUOTE(*c,fn);
}

void subquote(VL vl,QUOTE a,QUOTE b,QUOTE *c)
{
	FNODE fn;

	fn = mkfnode(3,I_BOP,subfs,BDY(a),BDY(b));
	MKQUOTE(*c,fn);
}

void mulquote(VL vl,QUOTE a,QUOTE b,QUOTE *c)
{
	FNODE fn;

	fn = mkfnode(3,I_BOP,mulfs,BDY(a),BDY(b));
	MKQUOTE(*c,fn);
}

void divquote(VL vl,QUOTE a,QUOTE b,QUOTE *c)
{
	FNODE fn;

	fn = mkfnode(3,I_BOP,divfs,BDY(a),BDY(b));
	MKQUOTE(*c,fn);
}

void pwrquote(VL vl,QUOTE a,QUOTE b,QUOTE *c)
{
	FNODE fn;

	if ( !b || OID(b) != O_QUOTE )
		error("pwrquote : invalid argument");
	fn = mkfnode(3,I_BOP,pwrfs,BDY(a),BDY(b));
	MKQUOTE(*c,fn);
}

void chsgnquote(QUOTE a,QUOTE *c)
{
	FNODE fn;

	fn = mkfnode(3,I_BOP,subfs,0,BDY(a));
	MKQUOTE(*c,fn);
}

void objtoquote(Obj a,QUOTE *c)
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

void polytoquote(P a,QUOTE *c)
{
	DCP dc,t;
	DCP *dca;
	int n,i,sgn;
	QUOTE v,r,s,u;

	if ( !a ) {
		MKQUOTE(*c,mkfnode(1,I_FORMULA,(pointer)a));
		return;
	} else if ( OID(a) == O_N ) {
		MKQUOTE(*c,mkfnode(1,I_FORMULA,(pointer)a));
		return;
	}
	vartoquote(VR((P)a),&v);
	dc = DC((P)a);
	dctoquote(dc,v,&r,&sgn);
	if ( sgn == -1 ) {
		MKQUOTE(u,mkfnode(1,I_MINUS,BDY(r)));
		r = u;
	}
	for (dc = NEXT(dc); dc; dc = NEXT(dc) ) {
		dctoquote(dc,v,&s,&sgn);
		if ( sgn == -1 )
			subquote(CO,r,s,&u);
		else
			addquote(CO,r,s,&u);
		r = u;
	}
	*c = r;
}

void dptoquote(DP a,QUOTE *c)
{
	MP t;
	MP m;
	int i,n,nv,sgn;
	QUOTE s,r,u;

	if ( !a ) {
		MKQUOTE(*c,mkfnode(1,I_FORMULA,(pointer)a));
		return;
	}
	nv = NV(a);
	m = BDY(a);
	mptoquote(m,nv,&r,&sgn);
	if ( sgn == -1 ) {
		MKQUOTE(u,mkfnode(1,I_MINUS,BDY(r)));
		r = u;
	}
	for ( m = NEXT(m); m; m = NEXT(m) ) {
		mptoquote(m,nv,&s,&sgn);
		if ( sgn < 0 )
			subquote(CO,r,s,&u);
		else
			addquote(CO,r,s,&u);
		r = u;
	}
	*c = r;
}

void dctoquote(DCP dc,QUOTE v,QUOTE *q,int *sgn)
{
	QUOTE t,s,u,r;
	P c;
	Q d;

	if ( mmono(COEF(dc)) ) {
		/* -xyz... */
		chsgnp(COEF(dc),&c);
		*sgn = -1;
	} else {
		c = COEF(dc);
		*sgn = 1;
	}
	d = DEG(dc);
	if ( UNIQ(c) ) {
		if ( d ) {
			if ( UNIQ(d) )
				r = v;
			else {
				objtoquote((Obj)d,&t);
				pwrquote(CO,v,t,&r);
			}
		} else
			objtoquote((Obj)ONE,&r);	
	} else {
		objtoquote((Obj)c,&u);
		if ( !NUM(c) && NEXT(DC(c)) && d ) {
			MKQUOTE(t,mkfnode(1,I_PAREN,BDY(u)));
			u = t;
		}
		if ( d ) {
			if ( UNIQ(d) )
				s = v;
			else {
				objtoquote((Obj)d,&t);
				pwrquote(CO,v,t,&s);
			}
			mulquote(CO,u,s,&r);
		} else
			r = u;
	}
	*q = r;
}

void mptoquote(MP m,int n,QUOTE *r,int *sgn)
{
	QUOTE s,u;
	P c;
	NODE t,t1;
	FNODE f;
	Q q;
	DL dl;
	int i;

	if ( mmono(C(m)) ) {
		chsgnp(C(m),&c);
		*sgn = -1;
	} else {
		c = C(m);
		*sgn = 1;
	}
	objtoquote((Obj)c,&s);
	if ( !NUM(c) && NEXT(DC(c)) ) {
		MKQUOTE(u,mkfnode(1,I_PAREN,BDY(s)));
		s = u;
	}
	dl = m->dl;
	for ( i = n-1, t = 0; i >= 0; i-- ) {
		STOQ(dl->d[i],q);
		f = mkfnode(1,I_FORMULA,q);
		MKNODE(t1,f,t);
		t = t1;
	}
	MKQUOTE(u,mkfnode(1,I_EV,t));
	if ( UNIQ(c) )
		*r = u;
	else
		mulquote(CO,s,u,r);
}

void vartoquote(V v,QUOTE *c)
{
	P x;
	PF pf;
	PFAD ad;
	QUOTE a,b,u;
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
			objtoquote(ad[0].arg,&a);
			x = (P)ad[0].arg;
			/* check whether x is a variable */
			if ( x && OID(x)==O_P && !NEXT(DC(x))
				&& UNIQ(DEG(DC(x))) && UNIQ(COEF(DC(x))) ) {
				/* use a as is */
				u = a;
			} else {
				/* a => (a) */
				MKQUOTE(u,mkfnode(1,I_PAREN,BDY(a)));
			}
			objtoquote(ad[1].arg,&b);
			pwrquote(CO,u,b,c);
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

typedef enum {
	A_arg,A_arf,A_int,A_str,A_internal,A_node,A_notimpl,A_func,A_end
} farg_type;

typedef struct fid_spec {
	fid id;
	farg_type type[10];
} *fid_spec_p;

struct fid_spec fid_spec_tab[] = {
	{I_BOP,A_arf,A_arg,A_arg,A_end},
	{I_COP,A_int,A_arg,A_arg,A_end},
	{I_AND,A_arg,A_arg,A_end},
	{I_OR,A_arg,A_arg,A_end},
	{I_NOT,A_arg,A_end},
	{I_CE,A_arg,A_arg,A_end},
	{I_PRESELF,A_arf,A_arg,A_end},
	{I_POSTSELF,A_arf,A_arg,A_end},
	{I_FUNC,A_func,A_arg,A_end},
	{I_FUNC_OPT,A_func,A_arg,A_arg,A_end},
	{I_IFUNC,A_arg,A_arg,A_end},
	{I_MAP,A_func,A_arg,A_end},
	{I_RECMAP,A_func,A_arg,A_end},
	{I_PFDERIV,A_notimpl,A_end},
	{I_ANS,A_int,A_end},
	{I_PVAR,A_int,A_node,A_end},
	{I_ASSPVAR,A_arg,A_arg,A_end},
	{I_FORMULA,A_internal,A_end},
	{I_LIST,A_node,A_end},
	{I_STR,A_str,A_end},
	{I_NEWCOMP,A_int,A_end},
	{I_CAR,A_arg,A_end},
	{I_CDR,A_arg,A_end},
	{I_CAST,A_notimpl,A_end},
	{I_INDEX,A_arg,A_node,A_end},
	{I_EV,A_node,A_end},
	{I_TIMER,A_arg,A_arg,A_arg,A_end},
	{I_GF2NGEN,A_end},
	{I_GFPNGEN,A_end},
	{I_GFSNGEN,A_end},
	{I_LOP,A_int,A_arg,A_arg,A_end},
	{I_OPT,A_str,A_arg,A_end},
	{I_GETOPT,A_str,A_end},
	{I_POINT,A_arg,A_str,A_end},
	{I_PAREN,A_arg,A_end},
	{I_MINUS,A_arg,A_end},
	{I_NARYOP,A_notimpl,A_end}
};

#define N_FID_SPEC (sizeof(fid_spec_tab)/sizeof(struct fid_spec))

void get_fid_spec(fid id,fid_spec_p *spec)
{
	int i;

	for ( i = 0; i < N_FID_SPEC; i++ )
		if ( fid_spec_tab[i].id == id ) {
			*spec = &fid_spec_tab[i];
			return;
		}
	*spec = 0;
}

FNODE strip_paren(FNODE f)
{
	if ( !f || f->id != I_PAREN ) return f;
	else {
		return strip_paren((FNODE)FA0(f));
	}
}

FNODE flatten_fnode(FNODE f,char *opname)
{
	fid_spec_p spec;
	farg_type *type;
	fid id;
	FNODE f1,f2,r;
	int i;

	if ( !f ) return f;
	id = f->id;
	get_fid_spec(id,&spec);
	/* unknown fid */
	if ( !spec ) return f;
	if ( id == I_BOP && !strcmp(((ARF)FA0(f))->name,opname) ) {
		f1 = (pointer)flatten_fnode(FA1(f),opname);
		f1 = strip_paren(f1);
		f2 = (pointer)flatten_fnode(FA2(f),opname);
		f2 = strip_paren(f2);
		if ( f1->id == I_BOP && !strcmp(((ARF)FA0(f1))->name,opname) ) {
			/* [+ [+ A B] C] => [+ A [+ B C]] */
			return mkfnode(3,I_BOP,(ARF)FA0(f),FA1(f1),
				mkfnode(3,I_BOP,(ARF)FA0(f),FA2(f1),f2));
		} else
			return mkfnode(3,I_BOP,(ARF)FA0(f),f1,f2);
	} else {
		type = spec->type;
		for ( i = 0; type[i] != A_end; i++ );
		NEWFNODE(r,i); ID(r) = f->id;
		for ( i = 0; type[i] != A_end; i++ ) {
			if ( type[i] == A_arg )
				r->arg[i] = (pointer)flatten_fnode(f->arg[i],opname);
			else
				r->arg[i] = f->arg[i];
		}
		return r;
	}
}
