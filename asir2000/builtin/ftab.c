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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/ftab.c,v 1.13 2004/12/18 16:50:10 saito Exp $ 
*/
#include "ca.h"
#include "parse.h"

void Ptstart(), Ptstop(), Pquit(), Pdebug();

struct ftab nasysftab[] = {
	{"tstart",Ptstart,0},
	{"tstop",Ptstop,0},
	{"end",Pquit,0},
	{"quit",Pquit,0},
	{"debug",Pdebug,0},
	{0,0,0},
};

typedef struct ftab FTAB[];

extern FTAB
alg_tab, array_tab, cplx_tab, ctrl_tab, dp_tab, dp_supp_tab, fctr_tab, file_tab,
help_tab, int_tab, list_tab, misc_tab, pdiv_tab, poly_tab,
print_tab, puref_tab, rat_tab, reduct_tab, result_tab,
str_tab, subst_tab, time_tab, type_tab, var_tab,
#if defined(INTERVAL)
	interval_tab, isolv_tab,
#endif
print2d_tab, tcp_tab, plot_tab, pari_tab, comp_tab, gf_tab, imat_tab, 
math_tab, mat_tab, numerical_tab, ec_tab, al_tab, round_tab, user_tab;

struct ftab *ftabs[] = {
	alg_tab, array_tab, cplx_tab, ctrl_tab, dp_tab, dp_supp_tab, fctr_tab, file_tab,
	help_tab, int_tab, list_tab, misc_tab, pdiv_tab, poly_tab,
	print_tab, puref_tab, rat_tab, reduct_tab, result_tab,
	str_tab, subst_tab, time_tab, type_tab, var_tab,
#if defined(INTERVAL)
	interval_tab, isolv_tab,
#endif
	pari_tab, comp_tab, gf_tab, imat_tab, math_tab, mat_tab, tcp_tab,
#if defined(DO_PLOT)
	plot_tab,
#endif
#if defined(LAPACK)
	numerical_tab,
#endif
	ec_tab, al_tab, round_tab,
	user_tab,
	0,
};

void sysf_init() {
	int i,j;
	struct ftab *tab;

	for ( j = 0, sysf = 0; tab = ftabs[j]; j++ )
		for ( i = 0; tab[i].name; i++ )
			appendbinf(&sysf,tab[i].name,tab[i].f,tab[i].argc,tab[i].quote);
	for ( i = 0, noargsysf = 0; nasysftab[i].name; i++ )
		appendbinf(&noargsysf,nasysftab[i].name, nasysftab[i].f,nasysftab[i].argc,0);
	parif_init();
}
