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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/parif.c,v 1.13 2003/01/28 09:03:06 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"

#if defined(PARI)
#include "genpari.h"

extern long prec;

void patori(GEN,Obj *);
void patori_i(GEN,N *);
void ritopa(Obj,GEN *);
void ritopa_i(N,int,GEN *);

void Ptodouble();
void Peval(),Psetprec(),p_pi(),p_e(),p_mul(),p_gcd();
void asir_cgiv(GEN);

#if defined(INTERVAL) || 1
void Psetprecword();
#endif

struct ftab pari_tab[] = {
	{"eval",Peval,-2},
	{"setprec",Psetprec,-1},
	{"todouble",Ptodouble,1},
#if defined(INTERVAL) || 1
	{"setprecword",Psetprecword,-1},
#endif
	{0,0,0},
};

#define MKPREC(a,i,b) (argc(a)==(i)?mkprec(QTOS((Q)(b))):prec)

#define CALLPARI1(f,a,p,r)\
ritopa((Obj)a,&_pt1_); _pt2_ = f(_pt1_,p); patori(_pt2_,r); asir_cgiv(_pt2_); asir_cgiv(_pt1_)
#define CALLPARI2(f,a,b,p,r)\
ritopa((Obj)a,&_pt1_); ritopa((Obj)b,&_pt2_); _pt3_ = f(_pt1_,_pt2_,p); patori(_pt3_,r); asir_cgiv(_pt3_); asir_cgiv(_pt2_); asir_cgiv(_pt1_)

#define PARIF1P(f,pf)\
void f(NODE,Obj *);\
void f(ar,rp) NODE ar; Obj *rp;\
{ GEN _pt1_,_pt2_; CALLPARI1(pf,ARG0(ar),MKPREC(ar,2,ARG1(ar)),rp); }
#define PARIF2P(f,pf)\
void f(NODE,Obj *);\
void f(ar,rp) NODE ar; Obj *rp;\
{ GEN _pt1_,_pt2_,_pt3_; CALLPARI2(pf,ARG0(ar),ARG1(ar),MKPREC(ar,3,ARG2(ar)),rp); }

#if defined(INTERVAL)
#define	PREC_CONV	pariK1
#else
#if defined(LONG_IS_32BIT)
#define PREC_CONV		0.103810253
#endif
#if defined(LONG_IS_64BIT)
#define PREC_CONV		0.051905126
#endif
#endif

/* XXX : we should be more careful when we free PARI pointers. */

void asir_cgiv(ptr)
GEN ptr;
{
	if ( ptr != gzero && ptr != gun
	&& ptr != gdeux && ptr != ghalf
	&& ptr != polvar && ptr != gi )
		cgiv(ptr);
}

mkprec(p)
int p;
{
	if ( p <= 0 )
		p = 1;
	return (int)(p*PREC_CONV+3);
}

void Ptodouble(arg,rp)
NODE arg;
Num *rp;
{
	double r,i;
	Real real,imag;
	Num num;

	asir_assert(ARG0(arg),O_N,"todouble");
	num = (Num)ARG0(arg);
	if ( !num ) {
		*rp = 0;
		return;
	}
	switch ( NID(num) ) {
		case N_R: case N_Q: case N_B:
			r = ToReal(num);
			MKReal(r,real);
			*rp = (Num)real;
			break;
		case N_C:
			r = ToReal(((C)num)->r);
			i = ToReal(((C)num)->i);
			MKReal(r,real);
			MKReal(i,imag);
			reimtocplx((Num)real,(Num)imag,rp);
			break;
		default:
			*rp = num;
			break;
	}
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

#if defined(INTERVAL) || 1
	p = (int)((prec-2)/PREC_CONV); STOQ(p,q); *rp = (Obj)q;
	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"setprec");
		p = QTOS((Q)ARG0(arg));
		if ( p > 0 )
			prec = (long)(p*PREC_CONV+3);
	}
#else
	p = (int)((prec-3)/PREC_CONV); STOQ(p,q); *rp = (Obj)q;
	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"setprec");
		prec = mkprec(QTOS((Q)ARG0(arg)));
	}
#endif
}

#if defined(INTERVAL) || 1
void Psetprecword(arg,rp)
NODE arg;
Obj *rp;
{
	int p;
	Q q;

	p = (int)((prec-2)); STOQ(p,q); *rp = (Obj)q;
	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"setprecword");
		p = QTOS((Q)ARG0(arg));
		if ( p > 0 ) {
			prec = p + 2;
		}
	}
}
#endif

void p_pi(arg,rp)
NODE arg;
Obj *rp;
{
	GEN x;

	x = mppi(MKPREC(arg,1,ARG0(arg)));
	patori(x,rp); asir_cgiv(x);
}

void p_e(arg,rp)
NODE arg;
Obj *rp;
{
	GEN x;

	x = gexp(gun,MKPREC(arg,1,ARG0(arg))); patori(x,rp); asir_cgiv(x);
}

void p_mul(a,b,r)
Obj a,b,*r;
{
	GEN p1,p2,p3;

	ritopa((Obj)a,&p1); ritopa((Obj)b,&p2);
	p3 = mulii(p1,p2);
	patori(p3,r); asir_cgiv(p3); asir_cgiv(p2); asir_cgiv(p1);
}

void p_gcd(a,b,r)
N a,b,*r;
{
	GEN p1,p2,p3;

	ritopa_i(a,1,&p1); ritopa_i(b,1,&p2);
	p3 = mppgcd(p1,p2);
	patori_i(p3,r); asir_cgiv(p3); asir_cgiv(p2); asir_cgiv(p1);
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
	int ac,opt,intarg,ret;
	char buf[BUFSIZ];
	Q q;
	GEN (*dmy)();

	if ( !f->f.binf ) {
		sprintf(buf,"pari : %s undefined.",f->name);
		error(buf);
		/* NOTREACHED */
		return 0;
	}
	switch ( f->type ) {
		case 0: /* in/out : integer */
			ac = argc(arg);
			if ( ac > 2 ) {
				fprintf(stderr,"argument mismatch in %s()\n",NAME(f));
				error("");
				/* NOTREACHED */
				return 0;
			}
			intarg = !ac ? 0 : QTOS((Q)ARG0(arg));
			dmy = (GEN (*)())f->f.binf;
			ret = (int)(*dmy)(intarg);
			STOQ(ret,q);
			return (pointer)q;

		case 1:
			ac = argc(arg);
			if ( !ac || ( ac > 2 ) ) {
				fprintf(stderr,"argument mismatch in %s()\n",NAME(f));
				error("");
				/* NOTREACHED */
				return 0;
			}
			ltop = avma;
			ritopa((Obj)ARG0(arg),&a);
			dmy = (GEN (*)())f->f.binf;
			v = (*dmy)(a,MKPREC(arg,2,ARG1(arg)));
			lbot = avma;
			patori(v,(Obj *)&r); gerepile(ltop,lbot,0);
			return r;

		case 2:
			ac = argc(arg);
			if ( !ac || ( ac > 2 ) ) {
				fprintf(stderr,"argument mismatch in %s()\n",NAME(f));
				error("");
				/* NOTREACHED */
				return 0;
			}
			if ( ac == 1 )
				opt = 0;
			else
				opt = QTOS((Q)ARG1(arg));
			ltop = avma;
			ritopa((Obj)ARG0(arg),&a);
			dmy = (GEN (*)())f->f.binf;
			v = (*dmy)(a,opt);
			lbot = avma;
			patori(v,(Obj *)&r); gerepile(ltop,lbot,0);
			return r;

		default:
			error("evalparif : not implemented yet.");
			/* NOTREACHED */
			return 0;
	}
}

struct pariftab {
	char *name;
	GEN (*f)();
	int type;
};

/*
 * type = 1 => argc = 1, second arg = precision
 * type = 2 => argc = 1, second arg = optional (long int)
 *
 */

struct pariftab pariftab[] = {
{"allocatemem",(GEN(*)())allocatemoremem,0},
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
{"isprime",gisprime,2},
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
{"length",(GEN(*)())glength,1},
{"lexsort",lexsort,1},
{"lift",lift,1},
{"lindep",lindep,1},
{"lll",lll,1},
{"lllgen",lllgen,1},
{"lllgram",lllgram,1},
{"lllgramgen",lllgramgen,1},
{"lllgramint",lllgramint,1},
{"lllgramkerim",lllgramkerim,1},
{"lllgramkerimgen",lllgramkerimgen,1},
{"lllint",lllint,1},
{"lllkerim",lllkerim,1},
{"lllkerimgen",lllkerimgen,1},
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
{"factor",factor,1},
{"factorint",factorint,2},
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
