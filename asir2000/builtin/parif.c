/* $OpenXM$ */
#include "ca.h"
#include "parse.h"
#include "ox.h"

Q ox_pari_stream;
int ox_pari_stream_initialized = 0;

pointer evalparif(FUNC f,NODE arg)
{
  int ac,intarg,opt,prec;
  Q q,r,narg;
  NODE nd,oxarg;
  STRING name;
  USINT ui;
  Obj ret,dmy;

  if (strcmp(f->name,"gamma")==0) {
    Pmpfr_gamma(arg,&ret);
    return((pointer) ret);
  }else if (strcmp(f->name,"floor")==0) {
    Pmpfr_floor(arg,&ret);
    return((pointer) ret);
  }
  if ( !ox_pari_stream_initialized ) {
	  MKSTR(name,"ox_pari");
	  nd = mknode(2,NULL,name);
	  Pox_launch(nd,&r);
	  ox_pari_stream = r;
    ox_pari_stream_initialized = 1;
  }
	switch ( f->type ) {
		case 0: /* in/out : integer */
			ac = argc(arg);
			if ( ac > 1 ) {
				fprintf(stderr,"argument mismatch in %s()\n",NAME(f));
				error("");
				/* NOTREACHED */
				return 0;
			}
      intarg = ac == 0 ? 0 : QTOS((Q)ARG0(arg));
      MKUSINT(ui,intarg);
      oxarg = mknode(2,ox_pari_stream,ui);
      Pox_push_cmo(oxarg,&dmy);
      MKSTR(name,f->name);
      oxarg = mknode(3,ox_pari_stream,name,ONE);
      Pox_execute_function(oxarg,&dmy);
      oxarg = mknode(1,ox_pari_stream);
      Pox_pop_cmo(oxarg,&r);
      return r;

		case 1:
			ac = argc(arg);
			if ( !ac || ( ac > 2 ) ) {
				fprintf(stderr,"argument mismatch in %s()\n",NAME(f));
				error("");
				/* NOTREACHED */
				return 0;
			}
      /* arg1 : prec */
      prec = ac == 1 ? 0 : QTOS((Q)ARG1(arg));
      MKUSINT(ui,prec);
      oxarg = mknode(2,ox_pari_stream,ui);
      Pox_push_cmo(oxarg,&dmy);

      /* arg0 : arg */
      oxarg = mknode(2,ox_pari_stream,ARG0(arg));
      Pox_push_cmo(oxarg,&dmy);

      MKSTR(name,f->name);
      STOQ(2,narg);
      oxarg = mknode(3,ox_pari_stream,name,narg);
      Pox_execute_function(oxarg,&dmy);
      oxarg = mknode(1,ox_pari_stream);
      Pox_pop_cmo(oxarg,&r);
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
			return r;

		default:
			error("evalparif : not implemented yet.");
			/* NOTREACHED */
			return 0;
	}
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
