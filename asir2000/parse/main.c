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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/main.c,v 1.4 2000/08/21 08:31:47 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#if defined(THINK_C)
#include <console.h>
#endif

#if PARI
#include "genpari.h"

extern jmp_buf environnement;
#endif

extern jmp_buf env;
extern int *StackBottom;

double get_current_time();
extern int mpi_nprocs,mpi_myid;

#if defined(VISUAL_LIB)
void Main(argc,argv)
#else
#if defined(VISUAL)
void
#endif
main(argc,argv)
#endif
int argc;
char *argv[];
{
	int tmp;
	FILE *ifp;
	char ifname[BUFSIZ];
	extern int GC_dont_gc;
	extern int read_exec_file;
	extern int do_asirrc;
	extern int do_file;
	extern FILE *in_fp;
	char *getenv();
	char *homedir;
	char *slash,*binname;
	char *ptr;

	StackBottom = &tmp + 1; /* XXX */
#if MPI
	mpi_init();
	if ( mpi_myid ) {
		int slave_argc;
		char *slave_argv[2];
 
		ox_mpi_slave_init();
		slave_argc = 1;
		slave_argv[0] = "ox_asir"; slave_argv[1]=0;
		ox_main(argc,argv);
		exit(0);
	} else
		ox_mpi_master_init();
#elif !defined(VISUAL)
	slash = (char *)rindex(argv[0],'/');
	if ( slash )
		binname = slash+1;
	else
		binname = argv[0];
	if ( !strcmp(binname,"ox_asir") ) {
		/* never return */
		ox_main(argc,argv);
		exit(0);
#if DO_PLOT
	} else if ( !strcmp(binname,"ox_plot") ) {
		/* never return */
		ox_plot_main(argc,argv);
		exit(0);
#endif
	} else if ( !strcmp(binname,"ox_launch") ) {
		/* never return */
		launch_main(argc,argv);
		exit(0);
	}
#endif

	srandom((int)get_current_time());
/*	mt_sgenrand((unsigned long)get_current_time()); */

#if defined(THINK_C)
	param_init();
#endif
	rtime_init();
	env_init();
	endian_init();
#if 0 && !defined(VISUAL) && !defined(THINK_C)
	check_key();
#endif
	GC_init();
	process_args(--argc,++argv);
#if PARI 
    risa_pari_init();
#endif 
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
#if defined(VISUAL)
	init_socket();
#endif
#if defined(UINIT)
	reg_sysf();
#endif

/* if ASIR_CONFIG is set, execute it; else execute .asirrc */
	if ( ptr = getenv("ASIR_CONFIG") )
		strcpy(ifname,ptr);
	else {
#if defined(THINK_C)
		sprintf(ifname,"asirrc");
#else
		homedir = getenv("HOME");
		if ( !homedir ) {
			char rootname[BUFSIZ];

			get_rootdir(rootname,sizeof(rootname));
			homedir = rootname;
		}
		sprintf(ifname,"%s/.asirrc",homedir);
#endif
	}

	if ( do_asirrc && (ifp = fopen(ifname,"r")) ) {
		input_init(ifp,ifname);
		if ( !setjmp(env) ) {
			read_exec_file = 1;
			read_eval_loop();
			read_exec_file = 0;
		}
		fclose(ifp);
	}

	if ( do_file )
		input_init(in_fp,"stdin");
	else
		input_init(stdin,"stdin");
	prompt();
	while ( 1 ) {
#if PARI
		recover(0);
		if ( setjmp(environnement) ) {
			avma = top; recover(1);
			resetenv("");
		}
#endif
		if ( setjmp(env) )
			prompt();
		read_eval_loop();
	}
}

#if !defined(VISUAL_LIB)
/* a dummy function */

void set_error(code,reasion,action)
int code;
char *reasion,*action;
{}
#endif
