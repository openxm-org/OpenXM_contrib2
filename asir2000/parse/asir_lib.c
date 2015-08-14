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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/asir_lib.c,v 1.10 2015/08/06 10:01:53 fujimoto Exp $ 
*/
#include "ca.h"
#include "parse.h"
#if defined(THINK_C)
#include <console.h>
#endif

#if defined(PARI)
#  include "genpari.h"
#  if !(PARI_VERSION_CODE > 131588 )
extern jmp_buf environnement;
#  endif
#endif

extern jmp_buf env;
extern char *parse_strp;
extern int do_file;

void Init_Asir(int,char **);
int Call_Asir(char *,pointer *);

void Init_Asir(int argc,char **argv)
{
	FILE *ifp;
	char ifname[BUFSIZ];
	extern int GC_dont_gc;
	extern int do_asirrc;
	char *getenv();
	static asirlib_initialized=0;

	if ( asirlib_initialized )
		return;
	GC_init();
	asirlib_initialized = 1;
	do_file = 1;
#if defined(PARI)
    risa_pari_init();
#endif 
	srandom((int)get_current_time());
/*	mt_sgenrand((unsigned long)get_current_time()); */

#if defined(THINK_C)
	param_init();
#endif
	rtime_init();
	env_init();
	endian_init();
#if !defined(VISUAL) && !defined(__MINGW32__) && !defined(THINK_C)
/*	check_key(); */
#endif
	process_args(--argc,++argv);
	copyright();
	output_init();
	arf_init();
	nglob_init();
	glob_init();
	sig_init();
	tty_init();
	debug_init();
	pf_init();
	sysf_init();
	parif_init();
#if defined(UINIT)
	reg_sysf();
#endif
#if defined(THINK_C)
	sprintf(ifname,"asirrc");
#else
	sprintf(ifname,"%s/.asirrc",getenv("HOME"));
#endif
	if ( do_asirrc && (ifp = fopen(ifname,"r")) ) {
		input_init(ifp,ifname);
		if ( !setjmp(asir_infile->jmpbuf) )
			read_eval_loop();
		fclose(ifp);
	} else
		error(".asirrc not found");
	input_init(0,"string");
}

int Call_Asir(char *cmd,pointer *result)
{
	SNODE snode;
	pointer val;
#if defined(PARI)
	void recover(int);

	recover(0);
#  if !(PARI_VERSION_CODE > 131588)
	if ( setjmp(environnement) ) {
		avma = top; recover(1);
		error("PARI error");
	}
#  endif
#endif
	if ( setjmp(env) ) {
		*result = 0;
		return -1;
	}
	parse_strp = cmd;
	if ( mainparse(&snode) ) {
		*result = 0;
		return -1;
	}
	val = evalstat(snode);
	if ( NEXT(asir_infile) ) {
		while ( NEXT(asir_infile) ) {
			if ( mainparse(&snode) ) {
				*result = val;
				return -1;
			}
			nextbp = 0;
			val = evalstat(snode);
		}
	}
	*result = val;
	return 0;
}
