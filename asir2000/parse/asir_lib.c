/* $OpenXM: OpenXM/src/asir99/parse/asir_lib.c,v 1.1.1.1 1999/11/10 08:12:34 noro Exp $ */
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
extern char *parse_strp;
extern int do_file;

void Init_Asir(int,char **);
int Call_Asir(char *,pointer *);

void Init_Asir(int argc,char **argv)
{
	int tmp;
	FILE *ifp;
	char ifname[BUFSIZ];
	extern int GC_dont_gc;
	extern int read_exec_file;
	extern int do_asirrc;
	char *getenv();
	static asirlib_initialized=0;

	if ( asirlib_initialized )
		return;
	asirlib_initialized = 1;
	do_file = 1;
#if PARI 
    risa_pari_init();
#endif 
	srandom((int)get_current_time());
/*	mt_sgenrand((unsigned long)get_current_time()); */

#if defined(THINK_C)
	param_init();
#endif
	StackBottom = &tmp + 1; /* XXX */
	rtime_init();
	env_init();
	endian_init();
#if !defined(VISUAL) && !defined(THINK_C)
/*	check_key(); */
#endif
	GC_init();
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
		if ( !setjmp(env) ) {
			read_exec_file = 1;
			read_eval_loop();
			read_exec_file = 0;
		}
		fclose(ifp);
	} else
		error(".asirrc not found");
	input_init(0,"string");
}

int Call_Asir(char *cmd,pointer *result)
{
	SNODE snode;
	pointer val;
#if PARI
	recover(0);
	if ( setjmp(environnement) ) {
		avma = top; recover(1);
		error("PARI error");
	}
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
