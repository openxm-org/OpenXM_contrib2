/* $OpenXM: OpenXM/src/asir99/builtin/ftab.c,v 1.1.1.1 1999/11/10 08:12:25 noro Exp $ */
#include "ca.h"
#include "parse.h"

#if defined(THINK_C)
#define DO_PLOT 1
#endif

#if 0
struct ftab sysftab[] = {
	{"modfctr",Pmodfctr,2}, 
	{"modsqfr",Pmodsqfr,2},
	{"ddd",Pddd,2},
/*	{"sacinit",Psacinit,-1},
	{"sacp",Psacp,1},
	{"sacprod",Psacprod,1},
	{"sacfctr",Psacfctr,1}, */
	{0,0,0}
};
#endif

void Ptstart(), Ptstop(), Pquit(), Pdebug();

struct ftab nasysftab[] = {
	{"tstart",Ptstart,0},
	{"tstop",Ptstop,0},
	{"end",Pquit,0},
	{"quit",Pquit,0},
	{"debug",Pdebug,0},
	{0,0,0},
};

#if !defined(THINK_C)
typedef struct ftab FTAB[];

extern FTAB
alg_tab, array_tab, cplx_tab, ctrl_tab, dp_tab, dp_supp_tab, fctr_tab, file_tab,
help_tab, int_tab, list_tab, misc_tab, pdiv_tab, poly_tab,
print_tab, puref_tab, rat_tab, reduct_tab, result_tab,
str_tab, subst_tab, time_tab, type_tab, var_tab,
print2d_tab, tcp_tab, plot_tab, pari_tab, comp_tab, gf_tab, math_tab,
numerical_tab, ec_tab, al_tab;
#else
extern struct ftab
alg_tab[], array_tab[], cplx_tab[], ctrl_tab[], dp_tab[], dp_supp_tab[], fctr_tab[], file_tab[],
help_tab[], int_tab[], list_tab[], misc_tab[], pdiv_tab[], poly_tab[],
print_tab[], puref_tab[], rat_tab[], reduct_tab[], result_tab[],
str_tab[], subst_tab[], time_tab[], type_tab[], var_tab[],
plot_tab[], pari_tab[], comp_tab[], gf_tab[], math_tab[],
numerical_tab[], ec_tab[], al_tab[];
#endif

struct ftab *ftabs[] = {
	alg_tab, array_tab, cplx_tab, ctrl_tab, dp_tab, dp_supp_tab, fctr_tab, file_tab,
	help_tab, int_tab, list_tab, misc_tab, pdiv_tab, poly_tab,
	print_tab, puref_tab, rat_tab, reduct_tab, result_tab,
	str_tab, subst_tab, time_tab, type_tab, var_tab, pari_tab, comp_tab, gf_tab,	math_tab,
#if DO_PLOT
	plot_tab,
#endif
#if INET
	tcp_tab,
#endif
#if 0
	print2d_tab,
#endif
#if LAPACK
	numerical_tab,
#endif
	ec_tab, al_tab,
	0,
};

void sysf_init() {
	int i,j;
	struct ftab *tab;

	for ( j = 0, sysf = 0; tab = ftabs[j]; j++ )
		for ( i = 0; tab[i].name; i++ )
			appendbinf(&sysf,tab[i].name,tab[i].f,tab[i].argc);
	for ( i = 0, noargsysf = 0; nasysftab[i].name; i++ )
		appendbinf(&noargsysf,nasysftab[i].name, nasysftab[i].f,nasysftab[i].argc);
	parif_init();
}
