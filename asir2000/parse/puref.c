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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/puref.c,v 1.6 2004/03/11 09:52:56 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"

NODE pflist;

void searchpf(char *name,FUNC *fp)
{
	NODE node;
	PF pf;
	FUNC t;

	for ( node = pflist; node; node = NEXT(node) )
		if ( !strcmp(name,((PF)node->body)->name) ) {
			pf = (PF)node->body;
			*fp = t = (FUNC)MALLOC(sizeof(struct oFUNC));
			t->name = name; t->id = A_PURE; t->argc = pf->argc;
			t->f.puref = pf; t->fullname = name;
			return;
		}
	*fp = 0;
}

void searchc(char *name,FUNC *fp)
{
	NODE node;
	PF pf;
	FUNC t;

	for ( node = pflist; node; node = NEXT(node) )
		if ( !strcmp(name,((PF)node->body)->name) 
			&& !((PF)node->body)->argc ) {
			pf = (PF)node->body;
			*fp = t = (FUNC)MALLOC(sizeof(struct oFUNC));
			t->name = name; t->id = A_PURE; t->argc = pf->argc;
			t->f.puref = pf; t->fullname = name;
			return;
		}
	*fp = 0;
}

void mkpf(char *name,Obj body,int argc,V *args,
	int (*parif)(),double (*libmf)(), int (*simp)(),PF *pfp)
{
	PF pf;
	NODE node;

	NEWPF(pf); pf->name = name; pf->body = body; 
	pf->argc = argc; pf->args = args; pf->pari = parif; pf->simplify = simp;
	pf->libm = libmf;
	for ( node = pflist; node; node = NEXT(node) )
		if ( !strcmp(((PF)BDY(node))->name,name) )
			break;
	if ( !node ) {
		NEWNODE(node); NEXT(node) = pflist; pflist = node;
/*		fprintf(stderr,"%s() defined.\n",name); */
	} else
		fprintf(stderr,"%s() redefined.\n",name);
	BDY(node) = (pointer)pf; *pfp = pf;
}

/*
   create an instance of a pure function. args are given
   as an array of V. So we have to create a P object for 
   each arg.
 */

void mkpfins(PF pf,V *args,V *vp)
{
	V v;
	PFINS ins;
	PFAD ad;
	int i;
	P t;

	NEWV(v); NAME(v) = 0; v->attr = (pointer)V_PF;
	ins = (PFINS)MALLOC(sizeof(PF)+pf->argc*sizeof(struct oPFAD));
	bzero((char *)ins,(int)(sizeof(PF)+pf->argc*sizeof(struct oPFAD)));
	ins->pf = pf;
	v->priv = (pointer)ins;
	for ( i = 0, ad = ins->ad; i < pf->argc; i++ ) {
		ad[i].d = 0; MKV(args[i],t); ad[i].arg = (Obj)t;
	}
	appendpfins(v,vp);
}

/* the same as above. Argements are given as an array of Obj */

void _mkpfins(PF pf,Obj *args,V *vp)
{
	V v;
	PFINS ins;
	PFAD ad;
	int i;

	NEWV(v); NAME(v) = 0; v->attr = (pointer)V_PF;
	ins = (PFINS)MALLOC(sizeof(PF)+pf->argc*sizeof(struct oPFAD));
	bzero((char *)ins,(int)(sizeof(PF)+pf->argc*sizeof(struct oPFAD)));
	ins->pf = pf;
	v->priv = (pointer)ins;
	for ( i = 0, ad = ins->ad; i < pf->argc; i++ ) {
		ad[i].d = 0; ad[i].arg = args[i];
	}
	appendpfins(v,vp);
}

/* the same as above. darray is also given */

void _mkpfins_with_darray(PF pf,Obj *args,int *darray,V *vp)
{
	V v;
	PFINS ins;
	PFAD ad;
	int i;

	NEWV(v); NAME(v) = 0; v->attr = (pointer)V_PF;
	ins = (PFINS)MALLOC(sizeof(PF)+pf->argc*sizeof(struct oPFAD));
	bzero((char *)ins,(int)(sizeof(PF)+pf->argc*sizeof(struct oPFAD)));
	ins->pf = pf;
	v->priv = (pointer)ins;
	for ( i = 0, ad = ins->ad; i < pf->argc; i++ ) {
		ad[i].d = darray[i]; ad[i].arg = args[i];
	}
	appendpfins(v,vp);
}

void appendpfins(V v,V *vp)
{
	PF fdef;
	PFAD ad,tad;
	NODE node;
	int i;

	fdef = ((PFINS)v->priv)->pf; ad = ((PFINS)v->priv)->ad;
	for ( node = fdef->ins; node; node = NEXT(node) ) {
		for ( i = 0, tad = ((PFINS)((V)node->body)->priv)->ad; 
			i < fdef->argc; i++ )
			if ( (ad[i].d != tad[i].d) || !equalr(CO,ad[i].arg,tad[i].arg) )
				break;
		if ( i == fdef->argc ) {
			*vp = (V)node->body;
			return;
		}
	}
	NEWNODE(node); node->body = (pointer)v; NEXT(node) = fdef->ins;
	fdef->ins = node; appendvar(CO,v); *vp = v;
}

void duppfins(V v,V *vp)
{
	V tv;
	PFINS tins;
	int size;

	NEWV(tv); tv->name = v->name; tv->attr = v->attr;
	size = sizeof(PF)+((PFINS)v->priv)->pf->argc*sizeof(struct oPFAD);
	tins = (PFINS)MALLOC(size); bcopy((char *)v->priv,(char *)tins,size);
	tv->priv = (pointer)tins;
	*vp = tv;
}

void derivvar(VL vl,V pf,V v,Obj *a)
{
	Obj t,s,u,w,u1;
	P p;
	V tv,sv;
	PF fdef;
	PFAD ad;
	int i,j;
	
	fdef = ((PFINS)pf->priv)->pf; ad = ((PFINS)pf->priv)->ad;
	if ( fdef->deriv ) {
		for ( t = 0, i = 0; i < fdef->argc; i++ ) {
			derivr(vl,ad[i].arg,v,&s);
			for ( j = 0, u = fdef->deriv[i]; j < fdef->argc; j++ ) {
				substr(vl,0,u,fdef->args[j],ad[j].arg,&u1); u = u1;
			}
			mulr(vl,s,u,&w); addr(vl,t,w,&s); t = s;
		}
		*a = t;
	} else {
		for ( t = 0, i = 0; i < fdef->argc; i++ ) {
			derivr(vl,ad[i].arg,v,&s);
			duppfins(pf,&tv); (((PFINS)tv->priv)->ad)[i].d++;
			appendpfins(tv,&sv);
			MKV(sv,p); mulr(vl,s,(Obj)p,&w); addr(vl,t,w,&s); t = s;
		}
		*a = t;
	}
}

void derivr(VL vl,Obj a,V v,Obj *b)
{
	VL tvl,svl;
	Obj r,s,t,u,nm,dn,dnm,ddn,m;

	if ( !a ) 
		*b = 0;
	else
		switch ( OID(a) ) {
			case O_N:
				*b = 0; break;
			case O_P:
				clctvr(vl,a,&tvl);
				for ( dnm = 0, svl = tvl; svl; svl = NEXT(svl) ) {
					if ( svl->v == v ) {
						pderivr(vl,a,v,&s); addr(vl,s,dnm,&u); dnm = u;
					} else if ( (vid)svl->v->attr == V_PF ) {
						pderivr(vl,a,svl->v,&s); derivvar(vl,svl->v,v,&r);
						mulr(vl,s,r,&u); addr(vl,u,dnm,&s); dnm = s;
					}
				}
				*b = (Obj)dnm; break;
			case O_R:
				clctvr(vl,a,&tvl);
				nm = (Obj)NM((R)a); dn = (Obj)DN((R)a);
				for ( dnm = ddn = 0, svl = tvl; svl; svl = NEXT(svl) ) {
					if ( svl->v == v ) {
						pderivr(vl,nm,v,&s); addr(vl,s,dnm,&u); dnm = u;
						pderivr(vl,dn,v,&s); addr(vl,s,ddn,&u); ddn = u;
					} else if ( (vid)svl->v->attr == V_PF ) {
						pderivr(vl,nm,svl->v,&s); derivvar(vl,svl->v,v,&r);
						mulr(vl,s,r,&u); addr(vl,u,dnm,&s); dnm = s;
						pderivr(vl,dn,svl->v,&s); derivvar(vl,svl->v,v,&r);
						mulr(vl,s,r,&u); addr(vl,u,ddn,&s); ddn = s;
					} 
				}
				mulr(vl,dnm,dn,&t); mulr(vl,ddn,nm,&s);
				subr(vl,t,s,&u); reductr(vl,u,&t);
				if ( !t )
					*b = 0;
				else {
					mulp(vl,(P)dn,(P)dn,(P *)&m); divr(vl,t,m,b);
				}
				break;
	}
}

void gen_pwrr(VL vl,Obj a,Obj d,Obj *r)
{
	if ( INT(d) )
		pwrr(vl,a,d,r);
	else
		mkpow(vl,a,d,r);
}

void substr(VL vl,int partial,Obj a,V v,Obj b,Obj *c)
{
	Obj nm,dn,t;

	if ( !a )
		*c = 0;
	else {
		switch ( OID(a) ) {
			case O_N:
				*c = a; break;
			case O_P:
				substpr(vl,partial,a,v,b,c); break;
			case O_R:	
				substpr(vl,partial,(Obj)NM((R)a),v,b,&nm); 
				substpr(vl,partial,(Obj)DN((R)a),v,b,&dn);
				divr(vl,nm,dn,&t); reductr(vl,t,c);
				break;
			default:
				*c = 0; break;
		}
	}
}

void substpr(VL vl,int partial,Obj p,V v0,Obj p0,Obj *pr)
{
	P x;
	Obj t,m,c,s,a;
	DCP dc;
	Q d;
	V v;
	PF pf;
	PFAD ad,tad;
	PFINS tins;
	int i;

	if ( !p ) 
		*pr = 0;
	else if ( NUM(p) ) 
		*pr = (Obj)p;
	else if ( (v = VR((P)p)) != v0 ) {
		if ( !partial && ((vid)v->attr == V_PF) ) {
			ad = ((PFINS)v->priv)->ad; pf = ((PFINS)v->priv)->pf;
			tins = (PFINS)CALLOC(1,sizeof(PF)+pf->argc*sizeof(struct oPFAD));
			tins->pf = pf;
			for ( i = 0, tad = tins->ad; i < pf->argc; i++ ) {
				tad[i].d = ad[i].d;
				substr(vl,partial,ad[i].arg,v0,p0,&tad[i].arg);
			}
			simplify_ins(tins,(Obj *)&x);
		} else
			MKV(VR((P)p),x);
		for ( c = 0, dc = DC((P)p); dc; dc = NEXT(dc) ) {
			substpr(vl,partial,(Obj)COEF(dc),v0,p0,&t);
			if ( DEG(dc) ) {
				gen_pwrr(vl,(Obj)x,(Obj)DEG(dc),&s);
				mulr(vl,s,t,&m); 
				addr(vl,m,c,&a); c = a;
			} else {
				addr(vl,t,c,&a); c = a;
			}
		}
		*pr = c;
	} else {
		dc = DC((P)p);
		if ( !partial )
			substpr(vl,partial,(Obj)COEF(dc),v0,p0,&c); 
		else
			c = (Obj)COEF(dc);
		for ( d = DEG(dc), dc = NEXT(dc); dc; d = DEG(dc), dc = NEXT(dc) ) {
				subq(d,DEG(dc),(Q *)&t);
				gen_pwrr(vl,p0,t,&s); mulr(vl,s,c,&m); 
				if ( !partial )
					substpr(vl,partial,(Obj)COEF(dc),v0,p0,&t); 
				else
					t = (Obj)COEF(dc);
				addr(vl,m,t,&c); 
		}
		if ( d ) {
			gen_pwrr(vl,p0,(Obj)d,&t);
			mulr(vl,t,c,&m); 
			c = m;
		}
		*pr = c;
	}
}

void evalr(VL vl,Obj a,int prec,Obj *c)
{
	Obj nm,dn;

	if ( !a )
		*c = 0;
	else {
		switch ( OID(a) ) {
			case O_N:
				*c = a; break;
			case O_P:
				evalp(vl,(P)a,prec,(P *)c); break;
			case O_R:	
				evalp(vl,NM((R)a),prec,(P *)&nm); evalp(vl,DN((R)a),prec,(P *)&dn);
				divr(vl,nm,dn,c);
				break;
			default:
				error("evalr : not implemented"); break;
		}
	}
}

void evalp(VL vl,P p,int prec,P *pr)
{
	P t;
	DCP dc,dcr0,dcr;
	Obj u;

	if ( !p || NUM(p) ) 
		*pr = p;
	else {
		for ( dcr0 = 0, dc = DC((P)p); dc; dc = NEXT(dc) ) {
			evalp(vl,COEF(dc),prec,&t);
			if ( t ) {
				NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
			}
		}
		if ( !dcr0 ) {
			*pr = 0; return;
		} else {
			NEXT(dcr) = 0; MKP(VR(p),dcr0,t);
		}
		if ( NUM(t) || (VR(t) != VR(p)) || ((vid)VR(p)->attr != V_PF) ) {
			*pr = t; return;
		} else {
			evalv(vl,VR(p),prec,&u); substr(vl,1,(Obj)t,VR(p),u,(Obj *)pr);
		}
	}
}

void evalv(VL vl,V v,int prec,Obj *rp)
{
	PFINS ins,tins;
	PFAD ad,tad;
	PF pf;
	P t;
	int i;

	if ( (vid)v->attr != V_PF ) {
		MKV(v,t); *rp = (Obj)t;
	} else {
		ins = (PFINS)v->priv; ad = ins->ad; pf = ins->pf;
		tins = (PFINS)CALLOC(1,sizeof(PF)+pf->argc*sizeof(struct oPFAD));
		tins->pf = pf;
		for ( i = 0, tad = tins->ad; i < pf->argc; i++ ) {
			tad[i].d = ad[i].d; evalr(vl,ad[i].arg,prec,&tad[i].arg);
		}
		evalins(tins,prec,rp);
	}
}

void evalins(PFINS ins,int prec,Obj *rp)
{
	PF pf;
	PFAD ad;
	int i;
	Q q;
	V v;
	P x;
	NODE n0,n;

	pf = ins->pf; ad = ins->ad;
	for ( i = 0; i < pf->argc; i++ )
		if ( ad[i].d || (ad[i].arg && !NUM(ad[i].arg)) )
			break;
	if ( (i != pf->argc) || !pf->pari ) {
		instov(ins,&v); MKV(v,x); *rp = (Obj)x;
	} else {
		for ( n0 = 0, i = 0; i < pf->argc; i++ ) {
			NEXTNODE(n0,n); BDY(n) = (pointer)ad[i].arg;
		}
		if ( prec ) {
			NEXTNODE(n0,n); STOQ(prec,q); BDY(n) = (pointer)q;
		}
		if ( n0 )
			NEXT(n) = 0;
		(*pf->pari)(n0,rp);
	}
}

void devalr(VL vl,Obj a,Obj *c)
{
	Obj nm,dn;
	double d;
	Real r;

	if ( !a )
		*c = 0;
	else {
		switch ( OID(a) ) {
			case O_N:
				d = ToReal(a);
				MKReal(d,r);
				*c = (Obj)r;
				break;
			case O_P:
				devalp(vl,(P)a,(P *)c); break;
			case O_R:	
				devalp(vl,NM((R)a),(P *)&nm);
				devalp(vl,DN((R)a),(P *)&dn);
				divr(vl,nm,dn,c);
				break;
			default:
				error("devalr : not implemented"); break;
		}
	}
}

void devalp(VL vl,P p,P *pr)
{
	P t;
	DCP dc,dcr0,dcr;
	Obj u,s;
	double d;
	Real r;

	if ( !p || NUM(p) ) {
		d = ToReal(p);
		MKReal(d,r);
		*pr = (P)r;
	} else {
		for ( dcr0 = 0, dc = DC((P)p); dc; dc = NEXT(dc) ) {
			devalp(vl,COEF(dc),&t);
			if ( t ) {
				NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
			}
		}
		if ( !dcr0 )
			*pr = 0;
		else {
			NEXT(dcr) = 0; MKP(VR(p),dcr0,t);
			if ( NUM(t) ) {
				d = ToReal((Num)t);
				MKReal(d,r);
				*pr = (P)r;
			} else if ( (VR(t) != VR(p)) || (VR(p)->attr != (pointer)V_PF) )
				*pr = t;
			else {
				devalv(vl,VR(p),&u);
				substr(vl,1,(Obj)t,VR(p),u,&s);
				if ( s && NUM(s) ) {
					d = ToReal((Num)s);
					MKReal(d,r);
					*pr = (P)r;
				} else
					*pr = (P)s;
			}
		}
	}
}

void devalv(VL vl,V v,Obj *rp)
{
	PFINS ins,tins;
	PFAD ad,tad;
	PF pf;
	P t;
	int i;

	if ( (vid)v->attr != V_PF ) {
		MKV(v,t); *rp = (Obj)t;
	} else {
		ins = (PFINS)v->priv; ad = ins->ad; pf = ins->pf;
		tins = (PFINS)CALLOC(1,sizeof(PF)+pf->argc*sizeof(struct oPFAD));
		tins->pf = pf;
		for ( i = 0, tad = tins->ad; i < pf->argc; i++ ) {
			tad[i].d = ad[i].d; devalr(vl,ad[i].arg,&tad[i].arg);
		}
		devalins(tins,rp);
	}
}

void devalins(PFINS ins,Obj *rp)
{
	PF pf;
	PFAD ad;
	int i;
	Real r;
	double d;
	V v;
	P x;

	pf = ins->pf; ad = ins->ad;
	for ( i = 0; i < pf->argc; i++ )
		if ( ad[i].d || (ad[i].arg && !NUM(ad[i].arg)) )
			break;
	if ( (i != pf->argc) || !pf->libm ) {
		instov(ins,&v); MKV(v,x); *rp = (Obj)x;
	} else {
		switch ( pf->argc ) {
			case 0:
				d = (*pf->libm)(); break;
			case 1:
				d = (*pf->libm)(ToReal(ad[0].arg)); break;
			case 2:
				d = (*pf->libm)(ToReal(ad[0].arg),ToReal(ad[1].arg)); break;
			case 3:
				d = (*pf->libm)(ToReal(ad[0].arg),ToReal(ad[1].arg),
					ToReal(ad[2].arg)); break;
			case 4:
				d = (*pf->libm)(ToReal(ad[0].arg),ToReal(ad[1].arg),
					ToReal(ad[2].arg),ToReal(ad[3].arg)); break;
			default:
				error("devalv : not supported");
		}
		MKReal(d,r); *rp = (Obj)r;
	}
}

void simplify_ins(PFINS ins,Obj *rp)
{
	V v;
	P t;

	if ( ins->pf->simplify )
		(*ins->pf->simplify)(ins,rp);
	else {
		instov(ins,&v); MKV(v,t); *rp = (Obj)t;
	}
}

void instov(PFINS ins,V *vp)
{
	V v;

	NEWV(v); NAME(v) = 0; 
	v->attr = (pointer)V_PF; v->priv = (pointer)ins;
	appendpfins(v,vp);
}

void substfr(VL vl,Obj a,PF u,PF f,Obj *c)
{
	Obj nm,dn;

	if ( !a )
		*c = 0;
	else {
		switch ( OID(a) ) {
			case O_N:
				*c = a; break;
			case O_P:
				substfp(vl,a,u,f,c); break;
			case O_R:	
				substfp(vl,(Obj)NM((R)a),u,f,&nm); substfp(vl,(Obj)DN((R)a),u,f,&dn);
				divr(vl,nm,dn,c);
				break;
			default:
				error("substfr : not implemented"); break;
		}
	}
}

void substfp(VL vl,Obj p,PF u,PF f,Obj *pr)
{
	V v;
	DCP dc;
	Obj a,c,m,s,t,p0;
	Q d;
	P x;

	if ( !p ) 
		*pr = 0;
	else if ( NUM(p) ) 
		*pr = (Obj)p;
	else {
		v = VR((P)p); dc = DC((P)p);
		if ( (int)v->attr != V_PF ) {
			MKV(VR((P)p),x);
			for ( c = 0; dc; dc = NEXT(dc) ) {
				substfp(vl,(Obj)COEF(dc),u,f,&t);
				if ( DEG(dc) ) {
					gen_pwrr(vl,(Obj)x,(Obj)DEG(dc),&s);
					mulr(vl,s,t,&m); 
					addr(vl,m,c,&a); c = a;
				} else {
					addr(vl,t,c,&a); c = a;
				}
			}
		} else {
			substfv(vl,v,u,f,&p0);
			substfp(vl,(Obj)COEF(dc),u,f,&c); 
			for ( d = DEG(dc), dc = NEXT(dc); dc; d = DEG(dc), dc = NEXT(dc) ) {
					subq(d,DEG(dc),(Q *)&t); 
					gen_pwrr(vl,p0,t,&s); mulr(vl,s,c,&m); 
					substfp(vl,(Obj)COEF(dc),u,f,&t); addr(vl,m,t,&c); 
			}
			if ( d ) {
				gen_pwrr(vl,p0,(Obj)d,&t); mulr(vl,t,c,&m); 
				c = m;
			}
		}
		*pr = c;
	}
}

void substfv(VL vl,V v,PF u,PF f,Obj *c)
{
	P t;
	Obj r,s,w;
	int i,j;
	PFINS ins,tins;
	PFAD ad,tad;

	ins = (PFINS)v->priv; ad = ins->ad;
	if ( ins->pf == u ) {
		if ( u->argc != f->argc )
			error("substfv : argument mismatch");
		if ( !f->body ) {
			mkpfins(f,f->args,&v); MKV(v,t); r = (Obj)t;
		} else
			r = f->body;
		for ( i = 0; i < f->argc; i++ )
			for ( j = 0; j < ad[i].d; j++ ) {
				derivr(vl,r,f->args[i],&s); r = s;
			}
		for ( i = 0; i < f->argc; i++ ) {
			substfr(vl,ad[i].arg,u,f,&w);
			substr(vl,0,r,f->args[i],w,&s); r = s;
		}
		*c = r;
	} else {
		tins = (PFINS)MALLOC(sizeof(PF)+f->argc*sizeof(struct oPFAD));
		tins->pf = ins->pf; tad = tins->ad;
		for ( i = 0; i < f->argc; i++ ) {
			tad[i].d = ad[i].d; substfr(vl,ad[i].arg,u,f,&tad[i].arg);
		}
		instov(tins,&v); MKV(v,t); *c = (Obj)t;
	}
}
