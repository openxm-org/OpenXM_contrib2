/* $OpenXM: OpenXM_contrib2/asir2000/builtin/parif.c,v 1.26 2015/08/18 02:26:05 noro Exp $ */
#include "ca.h"
#include "parse.h"
#include "ox.h"

Q ox_pari_stream;
int ox_pari_stream_initialized = 0;

typedef void (*mpfr_func)(NODE,Obj *);

void Pmpfr_ai();
void Pmpfr_eint(), Pmpfr_erf(),Pmpfr_li2();
void Pmpfr_zeta();
void Pmpfr_j0(), Pmpfr_j1();
void Pmpfr_y0(), Pmpfr_y1();
void Pmpfr_gamma(), Pmpfr_lngamma(), Pmpfr_digamma();
void Pmpfr_floor(), Pmpfr_round(), Pmpfr_ceil();

struct mpfr_tab_rec {
  char *name;
  mpfr_func func;
} mpfr_tab[] = {
	{"ai",Pmpfr_ai},
	{"zeta",Pmpfr_zeta},
	{"j0",Pmpfr_j0},
	{"j1",Pmpfr_j1},
	{"y0",Pmpfr_y0},
	{"y1",Pmpfr_y1},
	{"eint",Pmpfr_eint},
	{"erf",Pmpfr_erf},
	{"li2",Pmpfr_li2},
	{"gamma",Pmpfr_gamma},
	{"lngamma",Pmpfr_gamma},
	{"digamma",Pmpfr_gamma},
	{"floor",Pmpfr_floor},
	{"ceil",Pmpfr_ceil},
	{"round",Pmpfr_round},
};

mpfr_func mpfr_search(char *name)
{
  int i,n;

  n = sizeof(mpfr_tab)/sizeof(struct mpfr_tab_rec);
  for ( i = 0; i < n; i++ )
    if ( !strcmp(name,mpfr_tab[i].name) )
      return mpfr_tab[i].func;
  return 0;
}

pointer evalparif(FUNC f,NODE arg)
{
  int ac,intarg,opt,prec;
  Q q,r,narg;
  NODE nd,oxarg,t,t1,n;
  STRING name;
  USINT ui;
  Obj ret,dmy;
  mpfr_func mpfr_function;

  if ( mpfr_function = mpfr_search(f->name) ) {
     (*mpfr_function)(arg,&ret);
     return (pointer) ret;
  }

  if ( !ox_pari_stream_initialized ) {
	  MKSTR(name,"ox_pari");
	  nd = mknode(2,NULL,name);
	  Pox_launch_nox(nd,&r);
	  ox_pari_stream = r;
    ox_pari_stream_initialized = 1;
  }

	ac = argc(arg);
  /* reverse the arg list */
  for ( n = arg, t = 0; n; n = NEXT(n) ) {
    MKNODE(t1,BDY(n),t); t = t1;
  }
  /* push the reversed arg list */
  for ( ; t; t = NEXT(t) ) {
    oxarg = mknode(2,ox_pari_stream,BDY(t));
    Pox_push_cmo(oxarg,&dmy);
  }
  MKSTR(name,f->name);
  STOQ(ac,narg);
  oxarg = mknode(3,ox_pari_stream,name,narg);
  Pox_execute_function(oxarg,&dmy);
  oxarg = mknode(1,ox_pari_stream);
  Pox_pop_cmo(oxarg,&r);
  return r;
}

struct pariftab {
	char *name;
  int dmy;
	int type;
};

/*
 * type = 1 => argc = 1, second arg = precision
 * type = 2 => argc = 1, second arg = optional (long int)
 *
 */
/*
{"abs",0,1},
{"adj",0,1},
*/

struct pariftab pariftab[] = {
{"arg",0,1},
{"bigomega",0,1},
{"binary",0,1},
{"ceil",0,1},
{"centerlift",0,1},
{"cf",0,1},
{"classno",0,1},
{"classno2",0,1},
{"conj",0,1},
{"content",0,1},
{"denom",0,1},
{"det",0,1},
{"det2",0,1},
{"dilog",0,1},
{"disc",0,1},
{"discf",0,1},
{"divisors",0,1},
{"eigen",0,1},
{"eintg1",0,1},
{"erfc",0,1},
{"eta",0,1},
{"floor",0,1},
{"frac",0,1},
{"galois",0,1},
{"galoisconj",0,1},
{"gamh",0,1},
{"gamma",0,1},
{"hclassno",0,1},
{"hermite",0,1},
{"hess",0,1},
{"imag",0,1},
{"image",0,1},
{"image2",0,1},
{"indexrank",0,1},
{"indsort",0,1},
{"initalg",0,1},
{"isfund",0,1},
{"ispsp",0,1},
{"isqrt",0,1},
{"issqfree",0,1},
{"issquare",0,1},
{"jacobi",0,1},
{"jell",0,1},
{"ker",0,1},
{"keri",0,1},
{"kerint",0,1},
{"kerintg1",0,1},
{"length",0,1},
{"lexsort",0,1},
{"lift",0,1},
{"lindep",0,1},
{"lll",0,1},
{"lllgen",0,1},
{"lllgram",0,1},
{"lllgramgen",0,1},
{"lllgramint",0,1},
{"lllgramkerim",0,1},
{"lllgramkerimgen",0,1},
{"lllint",0,1},
{"lllkerim",0,1},
{"lllkerimgen",0,1},
{"lngamma",0,1},
{"logagm",0,1},
{"mat",0,1},
{"matrixqz2",0,1},
{"matrixqz3",0,1},
{"matsize",0,1},
{"modreverse",0,1},
{"mu",0,1},
{"nextprime",0,1},
{"norm",0,1},
{"norml2",0,1},
{"numdiv",0,1},
{"numer",0,1},
{"omega",0,1},
{"order",0,1},
{"ordred",0,1},
{"phi",0,1},
{"pnqn",0,1},
{"polred",0,1},
{"polred2",0,1},
{"primroot",0,1},
{"psi",0,1},
{"quadgen",0,1},
{"quadpoly",0,1},
{"real",0,1},
{"recip",0,1},
{"redreal",0,1},
{"regula",0,1},
{"reorder",0,1},
{"reverse",0,1},
{"rhoreal",0,1},
{"roots",0,1},
{"round",0,1},
{"sigma",0,1},
{"signat",0,1},
{"simplify",0,1},
{"smalldiscf",0,1},
{"smallfact",0,1},
{"smallpolred",0,1},
{"smallpolred2",0,1},
{"smith",0,1},
{"smith2",0,1},
{"sort",0,1},
{"sqr",0,1},
{"sqred",0,1},
{"sqrt",0,1},
{"supplement",0,1},
{"trace",0,1},
{"trans",0,1},
{"trunc",0,1},
{"unit",0,1},
{"vec",0,1},
{"wf",0,1},
{"wf2",0,1},
{"zeta",0,1},
{"factor",0,1},

{"allocatemem",0,0},

{"isprime",0,2},
{"factorint",0,2},
{0,0,0},
};

void parif_init() {
	int i;

	for ( i = 0, parif = 0; pariftab[i].name; i++ )
		 appendparif(&parif,pariftab[i].name, 0,pariftab[i].type);
}
