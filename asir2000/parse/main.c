/* $OpenXM: OpenXM_contrib2/asir2000/parse/main.c,v 1.1.1.1 1999/12/03 07:39:12 noro Exp $ */
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
void ExitAsir() {
	exit(0);
}

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
	} else if ( !strcmp(binname,"ox_plot") ) {
		/* never return */
		ox_plot_main(argc,argv);
		exit(0);
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
#if !defined(VISUAL) && !defined(THINK_C)
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
