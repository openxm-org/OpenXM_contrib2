/* $OpenXM: OpenXM_contrib2/asir2000/builtin/parif.c,v 1.1.1.1 1999/12/03 07:39:07 noro Exp $ */
#include "ca.h"
#include "parse.h"

#if PARI
#include "genpari.h"

extern long prec;

#if defined(THINK_C)
void patori(GEN,Obj *);
void patori_i(GEN,N *);
void ritopa(Obj,GEN *);
void ritopa_i(N,int,GEN *);
#else
void patori();
void patori_i();
void ritopa();
void ritopa_i();
#endif

void Peval(),Psetprec(),p_pi(),p_e(),p_mul(),p_gcd();

struct ftab pari_tab[] = {
	{"eval",Peval,-2}, {"setprec",Psetprec,-1}, {0,0,0},
};

#define MKPREC(a,i,b) (argc(a)==(i)?mkprec(QTOS((Q)(b))):prec)

#define CALLPARI1(f,a,p,r)\
ritopa((Obj)a,&_pt1_); _pt2_ = f(_pt1_,p); patori(_pt2_,r); cgiv(_pt2_); cgiv(_pt1_)
#define CALLPARI2(f,a,b,p,r)\
ritopa((Obj)a,&_pt1_); ritopa((Obj)b,&_pt2_); _pt3_ = f(_pt1_,_pt2_,p); patori(_pt3_,r); cgiv(_pt3_); cgiv(_pt2_); cgiv(_pt1_)

#define PARIF1P(f,pf)\
void f(NODE,Obj *);\
void f(ar,rp) NODE ar; Obj *rp;\
{ GEN _pt1_,_pt2_; CALLPARI1(pf,ARG0(ar),MKPREC(ar,2,ARG1(ar)),rp); }
#define PARIF2P(f,pf)\
void f(NODE,Obj *);\
void f(ar,rp) NODE ar; Obj *rp;\
{ GEN _pt1_,_pt2_,_pt3_; CALLPARI2(pf,ARG0(ar),ARG1(ar),MKPREC(ar,3,ARG2(ar)),rp); }

#if defined(LONG_IS_32BIT)
#define PREC_CONV		0.103810253
#endif
#if defined(LONG_IS_64BIT)
#define PREC_CONV		0.051905126
#endif

mkprec(p)
int p;
{
	if ( p > 0 )
		return (int)(p*PREC_CONV+3);
}

void Peval(arg,rp)
NODE arg;
Obj *rp;
{
	asir_assert(ARG0(arg),O_R,"eval");
	evalr(CO,(Obj)ARG0(arg),argc(arg)==2?QTOS((Q)ARG1(arg)):0,rp);
}

void Psetprec(arg,rp)
NODE arg;
Obj *rp;
{
	int p;
	Q q;

	p = (int)((prec-3)/PREC_CONV); STOQ(p,q); *rp = (Obj)q;
	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"setprec");
		prec = mkprec(QTOS((Q)ARG0(arg)));
	}
}

void p_pi(arg,rp)
NODE arg;
Obj *rp;
{
	GEN x;

	x = mppi(MKPREC(arg,1,ARG0(arg)));
	patori(x,rp); cgiv(x);
}

void p_e(arg,rp)
NODE arg;
Obj *rp;
{
	GEN x;

	x = gexp(gun,MKPREC(arg,1,ARG0(arg))); patori(x,rp); cgiv(x);
}

void p_mul(a,b,r)
Obj a,b,*r;
{
	GEN p1,p2,p3;

	ritopa((Obj)a,&p1); ritopa((Obj)b,&p2);
	p3 = mulii(p1,p2);
	patori(p3,r); cgiv(p3); cgiv(p2); cgiv(p1);
}

void p_gcd(a,b,r)
N a,b,*r;
{
	GEN p1,p2,p3;

	ritopa_i(a,1,&p1); ritopa_i(b,1,&p2);
	p3 = mppgcd(p1,p2);
	patori_i(p3,r); cgiv(p3); cgiv(p2); cgiv(p1);
}

PARIF1P(p_sin,gsin) PARIF1P(p_cos,gcos) PARIF1P(p_tan,gtan)
PARIF1P(p_asin,gasin) PARIF1P(p_acos,gacos) PARIF1P(p_atan,gatan)
PARIF1P(p_sinh,gsh) PARIF1P(p_cosh,gch) PARIF1P(p_tanh,gth)
PARIF1P(p_asinh,gash) PARIF1P(p_acosh,gach) PARIF1P(p_atanh,gath)
PARIF1P(p_exp,gexp) PARIF1P(p_log,glog)
PARIF1P(p_dilog,dilog) PARIF1P(p_erf,gerfc)
PARIF1P(p_eigen,eigen) PARIF1P(p_roots,roots)

PARIF2P(p_pow,gpui)

pointer evalparif(f,arg)
FUNC f;
NODE arg;
{
	GEN a,v;
	long ltop,lbot;
	pointer r;
	int ac;
	char buf[BUFSIZ];

	if ( !f->f.binf ) {
		sprintf(buf,"pari : %s undefined.",f->name);
		error(buf);
	}
	switch ( f->type ) {
		case 1:
			ac = argc(arg);
			if ( !ac || ( ac > 2 ) ) {
				fprintf(stderr,"argument mismatch in %s()\n",NAME(f));
				error("");
			}
			ltop = avma;
			ritopa((Obj)ARG0(arg),&a);
#if 1 || defined(__MWERKS__)
		{
			GEN (*dmy)();
			
			dmy = (GEN (*)())f->f.binf;
			v = (*dmy)(a,MKPREC(arg,2,ARG1(arg)));
		}
#else
			v = (GEN)(*f->f.binf)(a,MKPREC(arg,2,ARG1(arg)));
#endif
			lbot = avma;
			patori(v,(Obj *)&r); gerepile(ltop,lbot,0);
			return r;
		default:
			error("evalparif : not implemented yet.");
	}
}

struct pariftab {
	char *name;
	GEN (*f)();
	int type;
};

struct pariftab pariftab[] = {
{"abs",(GEN (*)())gabs,1},
{"adj",adj,1},
{"arg",garg,1},
{"bigomega",gbigomega,1},
{"binary",binaire,1},
{"ceil",gceil,1},
{"centerlift",centerlift,1},
{"cf",gcf,1},
{"classno",classno,1},
{"classno2",classno2,1},
{"conj",gconj,1},
{"content",content,1},
{"denom",denom,1},
{"det",det,1},
{"det2",det2,1},
{"dilog",dilog,1},
{"disc",discsr,1},
{"discf",discf,1},
{"divisors",divisors,1},
{"eigen",eigen,1},
{"eintg1",eint1,1},
{"erfc",gerfc,1},
{"eta",eta,1},
{"floor",gfloor,1},
{"frac",gfrac,1},
{"galois",galois,1},
{"galoisconj",galoisconj,1},
{"gamh",ggamd,1},
{"gamma",ggamma,1},
{"hclassno",classno3,1},
{"hermite",hnf,1},
{"hess",hess,1},
{"imag",gimag,1},
{"image",image,1},
{"image2",image2,1},
{"indexrank",indexrank,1},
{"indsort",indexsort,1},
{"initalg",initalg,1},
{"isfund",gisfundamental,1},
{"isprime",gisprime,1},
{"ispsp",gispsp,1},
{"isqrt",racine,1},
{"issqfree",gissquarefree,1},
{"issquare",gcarreparfait,1},
{"jacobi",jacobi,1},
{"jell",jell,1},
{"ker",ker,1},
{"keri",keri,1},
{"kerint",kerint,1},
{"kerintg1",kerint1,1},
{"kerint2",kerint2,1},
{"length",(GEN(*)())glength,1},
{"lexsort",lexsort,1},
{"lift",lift,1},
{"lindep",lindep,1},
{"lll",lll,1},
{"lllg1",lll1,1},
{"lllgen",lllgen,1},
{"lllgram",lllgram,1},
{"lllgramg1",lllgram1,1},
{"lllgramgen",lllgramgen,1},
{"lllgramint",lllgramint,1},
{"lllgramkerim",lllgramkerim,1},
{"lllgramkerimgen",lllgramkerimgen,1},
{"lllint",lllint,1},
{"lllkerim",lllkerim,1},
{"lllkerimgen",lllkerimgen,1},
{"lllrat",lllrat,1},
{"lngamma",glngamma,1},
{"logagm",glogagm,1},
{"mat",gtomat,1},
{"matrixqz2",matrixqz2,1},
{"matrixqz3",matrixqz3,1},
{"matsize",matsize,1},
{"modreverse",polymodrecip,1},
{"mu",gmu,1},
{"nextprime",nextprime,1},
{"norm",gnorm,1},
{"norml2",gnorml2,1},
{"numdiv",numbdiv,1},
{"numer",numer,1},
{"omega",gomega,1},
{"order",order,1},
{"ordred",ordred,1},
{"phi",phi,1},
{"pnqn",pnqn,1},
{"polred",polred,1},
{"polred2",polred2,1},
{"primroot",gener,1},
{"psi",gpsi,1},
{"quadgen",quadgen    ,1},
{"quadpoly",quadpoly    ,1},
{"real",greal,1},
{"recip",polrecip       ,1},
{"redreal",redreal       ,1},
{"regula",regula  ,1},
{"reorder",reorder  ,1},
{"reverse",recip  ,1},
{"rhoreal",rhoreal       ,1},
{"roots",roots,1},
{"round",ground,1},
{"sigma",sumdiv,1},
{"signat",signat,1},
{"simplify",simplify,1},
{"smalldiscf",smalldiscf,1},
{"smallfact",smallfact,1},
{"smallpolred",smallpolred,1},
{"smallpolred2",smallpolred2,1},
{"smith",smith,1},
{"smith2",smith2,1},
{"sort",sort,1},
{"sqr",gsqr,1},
{"sqred",sqred,1},
{"sqrt",gsqrt,1},
{"supplement",suppl,1},
{"trace",gtrace,1},
{"trans",gtrans,1},
{"trunc",gtrunc,1},
{"unit",fundunit,1},
{"vec",gtovec,1},
{"wf",wf,1},
{"wf2",wf2,1},
{"zeta",gzeta,1},
{0,0,0},
};

void parif_init() {
	int i;

	for ( i = 0, parif = 0; pariftab[i].name; i++ )
		 appendparif(&parif,pariftab[i].name, (int (*)())pariftab[i].f,pariftab[i].type);
}
#else /* PARI */

struct ftab pari_tab[] = {
	{0,0,0},
};

void parif_init() {}

pointer evalparif(f,arg)
FUNC f;
NODE arg;
{
	error("evalparif : PARI is not combined.");
}
#endif /*PARI */
