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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/main.c,v 1.27 2004/06/15 00:56:52 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"

#if defined(PARI)
#include "genpari.h"

/* XXX : environment is defined in libpari.a */
#  if !(PARI_VERSION_CODE > 131588)
extern jmp_buf environnement;
#  endif
#endif

extern JMP_BUF main_env;

#if defined(INTERVAL)
#ifndef ASIRRCNAME
#define ASIRRCNAME      "int_asirrc"
#endif
#else
#ifndef ASIRRCNAME
#define ASIRRCNAME      "asirrc"
#endif
#endif


double get_current_time();
void init_socket();
void recover();

extern int mpi_nprocs,mpi_myid;

#if defined(VISUAL_LIB)
void Main(int argc,char *argv[])
#else
#if defined(VISUAL)
void
#endif
main(int argc,char *argv[])
#endif
{
	int tmp;
	FILE *ifp;
	char ifname[BUFSIZ];
	extern int GC_dont_gc;
	extern int do_asirrc;
	extern int do_file;
	extern char *do_filename;
	extern int asir_setenv;
	extern FILE *in_fp;
	extern int *StackBottom;
	char *getenv();
	char *homedir;
	char *ptr;
#if !defined(VISUAL)
	char *slash,*bslash,*binname,*p;
#endif

	StackBottom = &tmp;
	GC_init();
#if defined(MPI)
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
	bslash = (char *)rindex(argv[0],'\\');
	if ( slash )
		binname = slash+1;
	else if ( bslash )
		binname = bslash+1;
	else
		binname = argv[0];
	for ( p = binname; *p; p++ )
		*p = tolower(*p);
	if ( !strncmp(binname,"ox_asir",strlen("ox_asir")) ) {
		/* never return */
		ox_main(argc,argv);
		exit(0);
#if defined(DO_PLOT)
	} else if ( !strncmp(binname,"ox_plot",strlen("ox_plot")) ) {
		/* never return */
		ox_plot_main(argc,argv);
		exit(0);
#endif
	} else if ( !strncmp(binname,"ox_launch",strlen("ox_launch")) ) {
		/* never return */
		launch_main(argc,argv);
		exit(0);
	}
#endif

	srandom((int)get_current_time());
/*	mt_sgenrand((unsigned long)get_current_time()); */

	rtime_init();
	env_init();
	endian_init();
	cppname_init();
	process_args(--argc,++argv);
#if defined(PARI)
    risa_pari_init();
#endif 
	if (!do_quiet) {
		copyright();
	}
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
	order_init();
	/* XXX set the default ordering */
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
		homedir = getenv("HOME");
		if ( !homedir ) {
			char rootname[BUFSIZ];

			get_rootdir(rootname,sizeof(rootname));
			homedir = rootname;
		}
		sprintf(ifname,"%s/." ASIRRCNAME,homedir);
	}

	if ( do_file ) {
		asir_infile=NULL;
		loadasirfile(do_filename);
	} else {
		/* the bottom of the input stack */
		input_init(stdin,"stdin");
	}

	if ( do_asirrc && (ifp = fopen(ifname,"r")) ) {
		fclose(ifp);
		if ( !SETJMP(main_env) )
			execasirfile(ifname);
	}

	prompt();
	while ( 1 ) {
#if defined(PARI)
		recover(0);
		/* XXX environement is defined in libpari.a */
#  if !(PARI_VERSION_CODE > 131588)
		if ( setjmp(environnement) ) {
			avma = top; recover(1);
			resetenv("");
		}
#  endif
#endif
		if ( SETJMP(main_env) )
			prompt();
		if ( SETJMP(asir_infile->jmpbuf) )
			prompt();
		else
			asir_infile->ready_for_longjmp = 1;
		restore_handler();
		read_eval_loop();
	}
}

#if !defined(VISUAL_LIB)
/* a dummy function */

void set_error(int code,char *reason,char *action)
{}
#endif
