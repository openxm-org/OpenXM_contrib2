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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/pf.c,v 1.8 2004/06/27 03:15:57 noro Exp $ 
*/
#include "ca.h"
#include "math.h"
#include "parse.h"
#if 0
#include <alloca.h>
#endif

double const_pi(),const_e();

void make_ihyp(void);
void make_hyp(void);
void make_itri(void);
void make_tri(void);
void make_exp(void);
void simplify_pow(PFINS,Obj *);

void Pfunctor(),Pargs(),Pfunargs(),Pvtype(),Pcall(),Pdeval();
void Pregister_handler();
void Peval_quote();
void Pmapat();
void Padd_handler();
void Plist_handler();
void Pclear_handler();

struct ftab puref_tab[] = {
	{"mapat",Pmapat,-99999999},
	{"functor",Pfunctor,1},
	{"args",Pargs,1},
	{"funargs",Pfunargs,1},
	{"register_handler",Pregister_handler,1},
	{"add_handler",Padd_handler,2},
	{"list_handler",Plist_handler,1},
	{"clear_handler",Pclear_handler,1},
	{"call",Pcall,2},
	{"vtype",Pvtype,1},
	{"deval",Pdeval,1},
	{"eval_quote",Peval_quote,1},
	{0,0,0},
};

#if defined(PARI)
int p_pi(),p_e();
int p_log(),p_exp(),p_pow();
int p_sin(),p_cos(),p_tan(),p_asin(),p_acos(),p_atan();
int p_sinh(),p_cosh(),p_tanh(),p_asinh(),p_acosh(),p_atanh();
#else
int p_pi,p_e;
int p_log,p_exp,p_pow;
int p_sin,p_cos,p_tan,p_asin,p_acos,p_atan;
int p_sinh,p_cosh,p_tanh,p_asinh,p_acosh,p_atanh;
#endif

static V *uarg,*darg;
static P x,y;
static PF pidef,edef;
static PF logdef,expdef,powdef;
static PF sindef,cosdef,tandef;
static PF asindef,acosdef,atandef;
static PF sinhdef,coshdef,tanhdef;
static PF asinhdef,acoshdef,atanhdef;

#define OALLOC(p,n) ((p)=(Obj *)CALLOC((n),sizeof(Obj)))

double const_pi() { return 3.14159265358979323846264338327950288; }
double const_e() { return 2.718281828459045235360287471352662497; }

void pf_init() {
	uarg = (V *)CALLOC(1,sizeof(V));
	uarg[0] = &oVAR[26]; MKV(uarg[0],x);

	darg = (V *)CALLOC(2,sizeof(V));
	darg[0] = &oVAR[26];
	darg[1] = &oVAR[27]; MKV(darg[1],y);

	mkpf("@pi",0,0,0,(int (*)())p_pi,const_pi,0,&pidef);
	mkpf("@e",0,0,0,(int (*)())p_e,const_e,0,&edef);

	mkpf("log",0,1,uarg,(int (*)())p_log,log,0,&logdef);
	mkpf("exp",0,1,uarg,(int (*)())p_exp,exp,0,&expdef);
	mkpf("pow",0,2,darg,(int (*)())p_pow,pow,(int (*)())simplify_pow,&powdef);

	mkpf("sin",0,1,uarg,(int (*)())p_sin,sin,0,&sindef);
	mkpf("cos",0,1,uarg,(int (*)())p_cos,cos,0,&cosdef);
	mkpf("tan",0,1,uarg,(int (*)())p_tan,tan,0,&tandef);
	mkpf("asin",0,1,uarg,(int (*)())p_asin,asin,0,&asindef);
	mkpf("acos",0,1,uarg,(int (*)())p_acos,acos,0,&acosdef);
	mkpf("atan",0,1,uarg,(int (*)())p_atan,atan,0,&atandef);

	mkpf("sinh",0,1,uarg,(int (*)())p_sinh,sinh,0,&sinhdef);
	mkpf("cosh",0,1,uarg,(int (*)())p_cosh,cosh,0,&coshdef);
	mkpf("tanh",0,1,uarg,(int (*)())p_tanh,tanh,0,&tanhdef);
#if !defined(VISUAL)
	mkpf("asinh",0,1,uarg,(int (*)())p_asinh,asinh,0,&asinhdef);
	mkpf("acosh",0,1,uarg,(int (*)())p_acosh,acosh,0,&acoshdef);
	mkpf("atanh",0,1,uarg,(int (*)())p_atanh,atanh,0,&atanhdef);
#endif
	make_exp();
	make_tri();
	make_itri();
	make_hyp();
#if !defined(VISUAL)
	make_ihyp();
#endif
}

void make_exp() {
	V v;
	P u,vexp,vlog,vpow;
	Obj *args;

	mkpfins(expdef,uarg,&v); MKV(v,vexp);
	mkpfins(powdef,darg,&v); MKV(v,vpow);
	mkpfins(logdef,uarg,&v); MKV(v,vlog);

	/* d/dx(log(x)) = 1/x */
	OALLOC(logdef->deriv,1); divr(CO,(Obj)ONE,(Obj)x,&logdef->deriv[0]);

	/* d/dx(exp(x)) = exp(x) */
	OALLOC(expdef->deriv,1); expdef->deriv[0] = (Obj)vexp;

	/* d/dy(x^y) = log(x)*x^y */
	OALLOC(powdef->deriv,2); mulp(CO,vpow,vlog,(P *)&powdef->deriv[1]);

	/* d/dx(x^y) = y*x^(y-1) */
	args = (Obj *)ALLOCA(2*sizeof(Obj));
	args[0] = (Obj)x; subp(CO,y,(P)ONE,(P *)&args[1]);
	_mkpfins(powdef,args,&v); MKV(v,u);
	mulr(CO,(Obj)u,(Obj)y,&powdef->deriv[0]);
}

void make_tri() {
	V v;
	P vcos,vsin,vtan,t;

	mkpfins(cosdef,uarg,&v); MKV(v,vcos);
	mkpfins(sindef,uarg,&v); MKV(v,vsin);
	mkpfins(tandef,uarg,&v); MKV(v,vtan);

	/* d/dx(sin(x)) = cos(x) */
	OALLOC(sindef->deriv,1); sindef->deriv[0] = (Obj)vcos;

	/* d/dx(cos(x)) = -sin(x) */
	OALLOC(cosdef->deriv,1); chsgnp(vsin,(P *)&cosdef->deriv[0]);

	/* d/dx(tan(x)) = 1+tan(x)^2 */
	OALLOC(tandef->deriv,1);
	mulr(CO,(Obj)vtan,(Obj)vtan,(Obj *)&t); addp(CO,(P)ONE,t,(P *)&tandef->deriv[0]);
}

void make_itri() {
	P t,xx;
	Q mtwo;
	V v;
	Obj *args;

	/* d/dx(asin(x)) = (1-x^2)^(-1/2) */
	OALLOC(asindef->deriv,1);
	args = (Obj *)ALLOCA(2*sizeof(Obj));
	mulp(CO,x,x,&xx); subp(CO,(P)ONE,xx,(P *)&args[0]);
	STOQ(-2,mtwo); divq(ONE,mtwo,(Q *)&args[1]);
	_mkpfins(powdef,args,&v); MKV(v,t);
	asindef->deriv[0] = (Obj)t;

	/* d/dx(acos(x)) = -(1-x^2)^(-1/2) */
	OALLOC(acosdef->deriv,1); chsgnp((P)asindef->deriv[0],(P *)&acosdef->deriv[0]);

	/* d/dx(atan(x)) = 1/(x^2+1) */
	OALLOC(atandef->deriv,1);
	addp(CO,(P)ONE,xx,&t); divr(CO,(Obj)ONE,(Obj)t,&atandef->deriv[0]);
}

void make_hyp() {
	V v;
	P vcosh,vsinh,vtanh,t;

	mkpfins(coshdef,uarg,&v); MKV(v,vcosh);
	mkpfins(sinhdef,uarg,&v); MKV(v,vsinh);
	mkpfins(tanhdef,uarg,&v); MKV(v,vtanh);

	/* d/dx(sinh(x)) = cosh(x) */
	OALLOC(sinhdef->deriv,1); sinhdef->deriv[0] = (Obj)vcosh;

	/* d/dx(cosh(x)) = sinh(x) */
	OALLOC(coshdef->deriv,1); coshdef->deriv[0] = (Obj)vsinh;

	/* d/dx(tanh(x)) = 1-tanh(x)^2 */
	OALLOC(tanhdef->deriv,1);
	mulr(CO,(Obj)vtanh,(Obj)vtanh,(Obj *)&t); subp(CO,(P)ONE,t,(P *)&tanhdef->deriv[0]);
}

void make_ihyp() {
	P t,xx;
	Q mtwo;
	V v;
	Obj *args;

	/* d/dx(asinh(x)) = (1+x^2)^(-1/2) */
	OALLOC(asinhdef->deriv,1);
	args = (Obj *)ALLOCA(2*sizeof(Obj));
	mulp(CO,x,x,&xx); addp(CO,(P)ONE,xx,(P *)&args[0]);
	STOQ(-2,mtwo); divq(ONE,mtwo,(Q *)&args[1]);
	_mkpfins(powdef,args,&v); MKV(v,t);
	asinhdef->deriv[0] = (Obj)t;

	/* d/dx(acosh(x)) = (x^2-1)^(-1/2) */
	OALLOC(acoshdef->deriv,1);
	subp(CO,xx,(P)ONE,(P *)&args[0]);
	_mkpfins(powdef,args,&v); MKV(v,t);
	acoshdef->deriv[0] = (Obj)t;

	/* d/dx(atanh(x)) = 1/(1-x^2) */
	OALLOC(atanhdef->deriv,1);
	subp(CO,(P)ONE,xx,&t); divr(CO,(Obj)ONE,(Obj)t,&atanhdef->deriv[0]);
}

void mkpow(vl,a,e,r)
VL vl;
Obj a;
Obj e;
Obj *r;
{
	PFINS ins;
	PFAD ad;

	ins = (PFINS)CALLOC(1,sizeof(PF)+2*sizeof(struct oPFAD));
	ins->pf = powdef; ad = ins->ad;
	ad[0].d = 0; ad[0].arg = a; ad[1].d = 0; ad[1].arg = e;
	simplify_ins(ins,r);
}

void simplify_pow(ins,rp)
PFINS ins;
Obj *rp;
{
	PF pf;
	PFAD ad;
	Obj a0,a1;
	V v;
	P t;

	pf = ins->pf; ad = ins->ad; a0 = ad[0].arg; a1 = ad[1].arg;	
	if ( !a1 )
		*rp = (Obj)ONE;
	else if ( !a0 ) {
		if ( RATN(a1) && SGN((Q)a1)>0 )
			*rp = 0;
		else if ( RATN(a1) && SGN((Q)a1) < 0 )
			error("simplify_pow : division by 0");
		else {
			instov(ins,&v); MKV(v,t); *rp = (Obj)t;
		}
	} else if ( NUM(a1) && INT(a1) )
		arf_pwr(CO,a0,a1,rp);
	else {
		instov(ins,&v); MKV(v,t); *rp = (Obj)t;
	}
}

#define ISPFINS(p)\
(p)&&(ID(p) == O_P)&&((int)VR((P)p)->attr!=V_PF)&&\
UNIQ(DEG(DC((P)p)))&&UNIQ(COEF(DC((P)p)))

void Pfunctor(arg,rp)
NODE arg;
P *rp;
{
	P p;
	FUNC t;
	PF pf;
	PFINS ins;

	p = (P)ARG0(arg);
	if ( !ISPFINS(p) )
		*rp = 0;
	else {
		ins = (PFINS)VR(p)->priv; pf = ins->pf;
		t = (FUNC)MALLOC(sizeof(struct oFUNC));
		t->name = t->fullname = pf->name; t->id = A_PURE; t->argc = pf->argc;
		t->f.puref = pf;
		makesrvar(t,rp);
	}
}

void Pargs(arg,rp)
NODE arg;
LIST *rp;
{
	P p;
	PF pf;
	PFAD ad;
	PFINS ins;
	NODE n,n0;
	int i;

	p = (P)ARG0(arg);
	if ( !ISPFINS(p) )
		*rp = 0;
	else {
		ins = (PFINS)VR(p)->priv; ad = ins->ad; pf = ins->pf;
		for ( i = 0, n0 = 0; i < pf->argc; i++ ) {
			NEXTNODE(n0,n); BDY(n) = (pointer)ad[i].arg;
		}
		if ( n0 )
			NEXT(n) = 0;
		MKLIST(*rp,n0);	
	}
}

void Pfunargs(arg,rp)
NODE arg;
LIST *rp;
{
	P p;
	P f;
	FUNC t;
	PF pf;
	PFINS ins;
	PFAD ad;
	NODE n,n0;
	int i;

	p = (P)ARG0(arg);
	if ( !ISPFINS(p) )
		*rp = 0;
	else {
		ins = (PFINS)VR(p)->priv; ad = ins->ad; pf = ins->pf;
		t = (FUNC)MALLOC(sizeof(struct oFUNC));
		t->name = t->fullname = pf->name; t->id = A_PURE; t->argc = pf->argc;
		t->f.puref = pf;
		makesrvar(t,&f);
		n = n0 = 0; NEXTNODE(n0,n); BDY(n) = (pointer)f;
		for ( i = 0; i < pf->argc; i++ ) {
			NEXTNODE(n0,n); BDY(n) = (pointer)ad[i].arg;
		}
		NEXT(n) = 0;
		MKLIST(*rp,n0);	
	}
}

void Pvtype(arg,rp)
NODE arg;
Q *rp;
{
	P p;

	p = (P)ARG0(arg);
	if ( !p || ID(p) != O_P )
		*rp = 0;
	else
		STOQ((int)VR(p)->attr,*rp);
}

extern NODE user_int_handler,user_quit_handler;

void Pregister_handler(arg,rp)
NODE arg;
Q *rp;
{
	P p;
	V v;
	NODE n;
	FUNC func;

	p = (P)ARG0(arg);
	if ( !p ) {
		user_int_handler = 0;
		*rp = 0;
		return;
	} else if ( OID(p) != 2 )
		error("register_hanlder : invalid argument");
	v = VR(p);
	if ( (int)v->attr != V_SR )	
		error("register_hanlder : no such function");
	else {
		func = (FUNC)v->priv;
		if ( func->argc )
			error("register_hanlder : the function must be with no argument");
		else {
			MKNODE(n,(pointer)func,user_int_handler);
			user_int_handler = n;
			*rp = ONE;
		}
	}
}

void Padd_handler(arg,rp)
NODE arg;
Q *rp;
{
	P p;
	V v;
	NODE n;
	FUNC func;
	char *name;
	NODE *hlistp;

	asir_assert(ARG0(arg),O_STR,"add_handler");
	name = BDY((STRING)ARG0(arg));
	p = (P)ARG1(arg);
	if ( !strcmp(name,"intr") )
		hlistp = &user_int_handler;
	else if ( !strcmp(name,"quit") )
		hlistp = &user_quit_handler;
	else
		error("add_handler : invalid keyword (must be \"intr\" or \"quit\")");
	if ( !p ) {
		*hlistp = 0; *rp = 0;
		return;
	}
	if ( OID(p) == 2 ) {
		v = VR(p);
		if ( (int)v->attr != V_SR )	
			error("add_hanlder : no such function");
		func = (FUNC)v->priv;
	} else if ( OID(p) == O_STR ) {
		gen_searchf_searchonly(BDY((STRING)p),&func);
		if ( !func )
			error("add_hanlder : no such function");
	}
	if ( func->argc )
		error("register_hanlder : the function must be with no argument");
	else {
		MKNODE(n,(pointer)func,*hlistp);
		*hlistp = n;
		*rp = ONE;
	}
}

void Plist_handler(arg,rp)
NODE arg;
LIST *rp;
{
	NODE r0,r,t;
	char *name;
	NODE hlist;
	STRING fname;

	asir_assert(ARG0(arg),O_STR,"list_handler");
	name = BDY((STRING)ARG0(arg));
	if ( !strcmp(name,"intr") )
		hlist = user_int_handler;
	else if ( !strcmp(name,"quit") )
		hlist = user_quit_handler;
	else
		error("list_handler : invalid keyword (must be \"intr\" or \"quit\")");
	for ( r0 = 0, t = hlist; t; t = NEXT(t) ) {
		NEXTNODE(r0,r);
		MKSTR(fname,((FUNC)BDY(t))->fullname);
		BDY(r) = (pointer)fname;
	}
	if ( r0 ) NEXT(r) = 0;
	MKLIST(*rp,r0);
}

void Pclear_handler(arg,rp)
NODE arg;
Q *rp;
{
	NODE r0,r,t;
	char *name;
	NODE hlist;
	STRING fname;

	asir_assert(ARG0(arg),O_STR,"clear_handler");
	name = BDY((STRING)ARG0(arg));
	if ( !strcmp(name,"intr") )
		user_int_handler = 0;
	else if ( !strcmp(name,"quit") )
		user_quit_handler = 0;
	else
		error("clear_handler : invalid keyword (must be \"intr\" or \"quit\")");
	*rp = 0;
}

void Pcall(NODE arg,Obj *rp)
{
	P p;
	V v;

	p = (P)ARG0(arg);
	if ( !p || OID(p) != 2 )
		error("call : invalid argument");
	v = VR(p);
	if ( (int)v->attr != V_SR )	
		error("call : no such function");

	else
		*rp = (Obj)bevalf((FUNC)v->priv,BDY((LIST)ARG1(arg)));
}

/* at=position of arg to be used for iteration */

void Pmapat(NODE arg,Obj *rp)
{
	LIST args;
	NODE node,rest,t0,t,n,r,r0;
	P fpoly;
	V fvar;
	FUNC f;
	VECT v,rv;
	MAT m,rm;
	LIST rl;
	int len,row,col,i,j,pos;
	Obj iter;
	pointer val;

	if ( argc(arg) < 3 )
		error("mapat : too few arguments");

	fpoly = (P)ARG0(arg);
	if ( !fpoly || OID(fpoly) != O_P )
		error("mapat : invalid function specification");
	fvar = VR(fpoly);
	if ( fvar->attr != (pointer)V_SR || !(f=(FUNC)fvar->priv) )
		error("mapat : invalid function specification");
	if ( !INT(ARG1(arg)) )
		error("mapat : invalid position");
	pos = QTOS((Q)ARG1(arg));
	node = NEXT(NEXT(arg));
	len = length(node);
	if ( pos >= len )
		error("evalmapatf : invalid position");
	r0 = 0;
	for ( i = 0, t = node; i < pos; i++, t = NEXT(t) ) {
		NEXTNODE(r0,r);
		BDY(r) = BDY(t);
	}
	NEXTNODE(r0,r);
	iter = BDY(t); rest = NEXT(t);
	if ( !iter ) {
		*rp = bevalf(f,node);
		return;
	}
	switch ( OID(iter) ) {
		case O_VECT:
			v = (VECT)iter; len = v->len; MKVECT(rv,len);
			for ( i = 0; i < len; i++ ) {
				BDY(r) = BDY(v)[i]; NEXT(r) = rest;
				BDY(rv)[i] = bevalf(f,r0);
			}
			*rp = (Obj)rv;
			break;
		case O_MAT:
			m = (MAT)iter; row = m->row; col = m->col; MKMAT(rm,row,col);
			for ( i = 0; i < row; i++ )
				for ( j = 0; j < col; j++ ) {
					BDY(r) = BDY(m)[i][j]; NEXT(r) = rest;
					BDY(rm)[i][j] = bevalf(f,r0);
				}
			*rp = (Obj)rm;
			break;
		case O_LIST:
			n = BDY((LIST)iter);
			for ( t0 = t = 0; n; n = NEXT(n) ) {
				BDY(r) = BDY(n); NEXT(r) = rest;
				NEXTNODE(t0,t); BDY(t) = bevalf(f,r0);
			}
			if ( t0 )
				NEXT(t) = 0;
			MKLIST(rl,t0);
			*rp = (Obj)rl;
			break;
		default:
			*rp = bevalf(f,node);
			break;
	}
}

void Pdeval(arg,rp)
NODE arg;
Obj *rp;
{
	asir_assert(ARG0(arg),O_R,"deval");
	devalr(CO,(Obj)ARG0(arg),rp);
}

void Peval_quote(arg,rp)
NODE arg;
Obj *rp;
{
	asir_assert(ARG0(arg),O_QUOTE,"eval_quote");
	*rp = eval((FNODE)BDY((QUOTE)ARG0(arg)));
}
