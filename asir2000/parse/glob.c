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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/glob.c,v 1.26 2001/12/25 02:39:06 noro Exp $ 
*/
#include "ca.h"
#include "al.h"
#include "parse.h"
#include "ox.h"
#if PARI
#include "genpari.h"
#endif
#if !defined(VISUAL) && !defined(_PA_RISC1_1) && !defined(linux) && !defined(SYSV) && !defined(__CYGWIN__)
#include <sgtty.h>
#endif

#if defined(VISUAL)
#include <io.h>
#include <direct.h>
#endif

#if defined(SYSV) && !defined(_IBMR2)
#include <sys/ttold.h>
#endif

#if defined(VISUAL)
#define HISTORY asir_history
#endif

#define MAXHIST 100

extern int GC_free_space_divisor;
extern int GC_free_space_numerator;
extern FILE *asir_out;

INFILE asir_infile;
JMP_BUF main_env,debug_env,timer_env,exec_env;
int little_endian,debug_mode;
char *asir_libdir;
char *asir_pager;

NODE usrf,sysf,noargsysf,ubinf,parif;
NODE ONENODE;
int main_parser, ox_do_copy, ox_do_count, ox_count_length;
int ox_file_io, ox_need_conv;
char *ox_copy_bptr;
char *parse_strp;
SNODE parse_snode;
FUNC parse_targetf;
FILE *ox_istream,*ox_ostream;
int do_server_in_X11;
Obj LastVal;
char LastError[BUFSIZ];
int timer_is_set;


struct oV oVAR[] = {
	{"x",0,0}, {"y",0,0}, {"z",0,0}, {"u",0,0},
	{"v",0,0}, {"w",0,0}, {"p",0,0}, {"q",0,0},
	{"r",0,0}, {"s",0,0}, {"t",0,0}, {"a",0,0},
	{"b",0,0}, {"c",0,0}, {"d",0,0}, {"e",0,0},
	{"f",0,0}, {"g",0,0}, {"h",0,0}, {"i",0,0},
	{"j",0,0}, {"k",0,0}, {"l",0,0}, {"m",0,0},
	{"n",0,0}, {"o",0,0},
	{"_x",0,0}, {"_y",0,0}, {"_z",0,0}, {"_u",0,0},
	{"_v",0,0}, {"_w",0,0}, {"_p",0,0}, {"_q",0,0},
	{"_r",0,0}, {"_s",0,0}, {"_t",0,0}, {"_a",0,0},
	{"_b",0,0}, {"_c",0,0}, {"_d",0,0}, {"_e",0,0},
	{"_f",0,0}, {"_g",0,0}, {"_h",0,0}, {"_i",0,0},
	{"_j",0,0}, {"_k",0,0}, {"_l",0,0}, {"_m",0,0},
	{"_n",0,0}, {"_o",0,0}
};

struct oVL oVLIST[52];

VL CO = oVLIST;
VL ALG;

struct oVS oGPVS,oAPVS,oEPVS;
VS GPVS = &oGPVS;
VS APVS = &oAPVS;
VS EPVS = &oEPVS;
VS CPVS;

struct oF oF_TRUE,oF_FALSE;
F F_TRUE = &oF_TRUE;  
F F_FALSE = &oF_FALSE;

#if defined(__SVR4) && defined(sun)
char cppname[BUFSIZ] = "/usr/ccs/lib/cpp ";
#else
#if defined(__FreeBSD__) || defined(__NetBSD__) || (defined(__MACH__) && defined(__ppc__)) || defined(__CYGWIN__)
char cppname[BUFSIZ] = "/usr/bin/cpp ";
#else
#if defined(VISUAL)
char cppname[BUFSIZ] = "c:\\asir\\stdlib\\cpp ";
#else
char cppname[BUFSIZ] = "/lib/cpp ";
#endif
#endif
#endif
char asirname[BUFSIZ];
char displayname[BUFSIZ];

int Verbose;

void glob_init() {
	int i;
	
	for ( i = 0; i < 51; i++ ) {
		VR(&oVLIST[i]) = &oVAR[i]; NEXT(&oVLIST[i]) = &oVLIST[i+1];
	}
	VR(&oVLIST[i]) = &oVAR[i]; NEXT(&oVLIST[i]) = 0;
	reallocarray((char **)&GPVS->va,(int *)&GPVS->asize,(int *)&GPVS->n,(int)sizeof(struct oPV));
	reallocarray((char **)&APVS->va,(int *)&APVS->asize,(int *)&APVS->n,(int)sizeof(struct oPV));
	CPVS = GPVS;
	MKNODE(ONENODE,mkfnode(1,I_FORMULA,ONE),0);
	OID(F_TRUE)=O_F; FOP(F_TRUE)=AL_TRUE; F_TRUE->arg.dummy = 0;
	OID(F_FALSE)=O_F; FOP(F_FALSE)=AL_FALSE; F_FALSE->arg.dummy = 0;
	sprintf(asirname,"%s/asir_symtab",asir_libdir);
}

void input_init(FILE *fp,char *name)
{
	asir_infile = (INFILE)CALLOC(sizeof(struct oINFILE),1);
	asir_infile->name = name; asir_infile->fp = fp;
	asir_infile->ln = 1; NEXT(asir_infile) = 0;
}

void notdef(VL vl,Obj a,Obj b,Obj *c)
{
	error("undefined arithmetic operation.");
}

int do_asirrc;
int do_file;
int do_message;
int do_fep;
int no_prompt;
int read_exec_file;
static int buserr_sav;
static char asir_history[BUFSIZ];

extern int mpi_myid;

#if !defined(VISUAL_LIB)
void ExitAsir() {
	exit(0);
}
#endif

/*
 * status = 1 abnormal termination (error() etc.)
 * status = 2 normal termination (end(), quit() etc.)
 */

void asir_terminate(int status)
{
	int t;

	if ( read_exec_file ) {
		t = read_exec_file;
		read_exec_file = 0;
		if ( t == 1 )
			LONGJMP(main_env,status);
		else
			LONGJMP(exec_env,status);
	} else {
		tty_reset();
#if MPI
		if ( !mpi_myid )
			close_allconnections();
		mpi_finalize();
#else
#if defined(SIGPIPE)
		signal(SIGPIPE,SIG_IGN);
#endif
		close_allconnections();
#endif
		if ( asir_out )
			fflush(asir_out);
#if FEP
		if ( do_fep ) {
			stifle_history(MAXHIST);
			write_history(asir_history);
		}
#endif
		ExitAsir();
	}
}

void param_init() {
	unsigned int et = 0xff;
	extern int paristack;
	if ( *((char *)&et) )
		little_endian = 1;
	else
		little_endian = 0;
}
	
void prompt() {
	if ( !no_prompt && !do_fep && asir_infile->fp == stdin )
		fprintf(asir_out,"[%d] ",APVS->n);
	fflush(asir_out);
}

void sprompt(char *ptr)
{
	sprintf(ptr,"[%d] ",APVS->n);
}

FILE *in_fp;

void process_args(int ac,char **av)
{
	do_asirrc = 1;
#if !MPI
	do_message = 1;
#endif
	while ( ac > 0 ) {
		if ( !strcmp(*av,"-heap") && (ac >= 2) ) {
			void GC_expand_hp(int);

			GC_expand_hp(atoi(*(av+1))); av += 2; ac -= 2;
		} else if ( !strcmp(*av,"-adj") && (ac >= 2) ) {
			char *slash;

			slash = strrchr(*(av+1),'/');
			if ( slash ) {
				*slash = 0;
				GC_free_space_numerator = atoi(slash+1);
			}
			GC_free_space_divisor = atoi(*(av+1));
			av += 2; ac -= 2;
		} else if ( !strcmp(*av,"-cpp") && (ac >= 2) ) {
			strcpy(cppname,*(av+1)); av += 2; ac -= 2;
		} else if ( !strcmp(*av,"-f") && (ac >= 2) ) {
			in_fp = fopen(*(av+1),"r");
			if ( !in_fp ) {
				fprintf(stderr,"%s does not exist!",*(av+1));
				asir_terminate(1);
			}
			do_file = 1;
			av += 2; ac -= 2;
		} else if ( !strcmp(*av,"-norc") ) {
			do_asirrc = 0; av++; ac--;
		} else if ( !strcmp(*av,"-nomessage") ) {
			do_message = 0; av++; ac--;
		} else if ( !strcmp(*av,"-terse") ) {
			no_prompt = 0; av++; ac--;
		} else if ( !strcmp(*av,"-rootdir") && (ac >= 2) ) {
			set_rootdir(*(av+1)); av += 2; ac -= 2;
		} else if ( !strcmp(*av,"-maxheap") && (ac >= 2) ) {
			void GC_set_max_heap_size(int);

			GC_set_max_heap_size(atoi(*(av+1))); av += 2; ac -= 2;
#if !defined(VISUAL)
		} else if ( !strcmp(*av,"-display") && (ac >= 2) ) {
			strcpy(displayname,*(av+1)); av += 2; ac -= 2;
#endif
#if FEP
		} else if ( !strcmp(*av,"-fep") ) {
			do_fep = 1; av++; ac--;
#endif
#if PARI
		} else if ( !strcmp(*av,"-paristack") ) {
			extern int paristack;

			paristack = atoi(*(av+1)); av += 2; ac -= 2;
#endif
		} else {
			fprintf(stderr,"%s : unknown option.\n",*av);
			asir_terminate(1);
		}
	}
#if FEP
	if ( do_fep ) {
		char *home;
		home = (char *)getenv("HOME");
		if (!home)
			home = ".";
		sprintf (asir_history, "%s/.asir_history",home);
		read_history(asir_history);
		using_history();
	}
#endif
}

#include <signal.h>

void sig_init() {
#if !defined(VISUAL)
	signal(SIGINT,int_handler);
#else
	void register_ctrlc_handler();

	register_ctrlc_handler();
#endif
	signal(SIGSEGV,segv_handler);

#if defined(SIGFPE)
	signal(SIGFPE,fpe_handler);
#endif

#if defined(SIGPIPE)
	signal(SIGPIPE,pipe_handler);
#endif

#if defined(SIGILL)
	signal(SIGILL,ill_handler);
#endif

#if !defined(VISUAL)
	signal(SIGBUS,bus_handler);
#endif
}

static void (*old_int)(int);

void asir_save_handler() {
	old_int = signal(SIGINT,SIG_IGN);
	signal(SIGINT,old_int);
}

void asir_set_handler() {
	signal(SIGINT,int_handler);
}

void asir_reset_handler() {
	signal(SIGINT,old_int);
}

void resetenv(char *s)
{
	extern FILE *outfile;

	fprintf(stderr,"%s\n",s);
	while ( NEXT(asir_infile) )
		closecurrentinput();
	resetpvs();
#if !defined(VISUAL)
	if ( do_server_in_X11 )
#endif
		show_debug_window(0);
#if defined(VISUAL_LIB)
	w_noflush_stderr(0);
#endif
	asir_out = stdout;
#if PARI
	pari_outfile = stdout;
#endif
	/* restore states */
	reset_engine();
	reset_io();
#if !defined(VISUAL)
	reset_timer();
#endif
	LONGJMP(main_env,1);
}

void fatal(int n)
{
	resetenv("return to toplevel");
}

FUNC registered_handler;
extern int ox_int_received, critical_when_signal;

void int_handler(int sig)
{
	extern NODE PVSS;

	if ( do_file ) {
		ExitAsir();
	}
	if ( critical_when_signal ) {
		ox_int_received = 1;
		return;
	}
#if defined(VISUAL)
	suspend_timer();
#endif
	signal(SIGINT,SIG_IGN);
#if !defined(VISUAL) 
    if ( do_server_in_X11 ) {
		debug(PVSS?((VS)BDY(PVSS))->usrf->f.usrf->body:0);
 		restore_handler();
		return;
	}
#endif
#if defined(linux)
#if 1
	while ( stdin->_IO_read_ptr < stdin->_IO_read_end )
#else
	while ( stdin->_gptr < stdin->_egptr )
#endif
		getchar();
#endif
	while ( 1 ) {
		char buf[BUFSIZ];
		char c;

		fprintf(stderr,"interrupt ?(q/t/c/d/u/w/?) "); fflush(stderr);
		buf[0] = '\n';
		while ( buf[0] == '\n' )
			fgets(buf,BUFSIZ,stdin);
		switch ( c = buf[0] ) {
			case 'q':
				while ( 1 ) {
					fprintf(stderr,"Abort this session? (y or n) "); fflush(stderr);
					fgets(buf,BUFSIZ,stdin);
					if ( !strncmp(buf,"y",1) ) {
						read_exec_file = 0;
						fprintf(stderr,"Bye\n"); asir_terminate(1);
					} else if ( !strncmp(buf,"n",1) ) {
						restore_handler();
						return;
					}
				}
				break;
			case 't':
			case 'u':
				while ( 1 ) {
					fprintf(stderr,"Abort this computation? (y or n) "); fflush(stderr);
					fgets(buf,BUFSIZ,stdin);
					if ( !strncmp(buf,"y",1) )
						break;
					else if ( !strncmp(buf,"n",1) ) {
						restore_handler();
						return;
					}
				}
				if ( debug_mode )
					debug_mode = 0;
				restore_handler();
				if ( c == 'u' ) {
					if ( registered_handler ) {
						fprintf(stderr,
							"Calling the registered exception handler...");
						bevalf(registered_handler,0);
						fprintf(stderr, "done.\n");
					}
				}
				if ( read_exec_file ) {
					read_exec_file = 0;
					resetenv("initialization aborted; return to toplevel");
				} else
					resetenv("return to toplevel");
				break;
			case 'd':
#if 0
				nextbp = 1; nextbplevel = -1;
#endif
				debug(PVSS?((VS)BDY(PVSS))->usrf->f.usrf->body:0);
 				restore_handler();
 				return;
			case 'c': 
				restore_handler();
				return; break;
			case 'w':
				showpos(); break;
			case '?': 
				fprintf(stderr, "q:quit t:toplevel c:continue d:debug u:call registered handler w:where\n");
				break;
			default:
				break;
		}
	}
}

void restore_handler() {
#if defined(VISUAL)
	resume_timer();
#endif
#if defined(SIGINT)
	signal(SIGINT,int_handler);
#endif
}

void segv_handler(int sig)
{
#if defined(SIGSEGV)
	signal(SIGSEGV,segv_handler);
	error("internal error (SEGV)");
#endif
}

void ill_handler(int sig)
{
#if defined(SIGILL)
	signal(SIGILL,ill_handler);
	error("illegal instruction (ILL)");
#endif
}

void alrm_handler(int sig)
{
	fprintf(stderr,"interval timer expired (VTALRM)\n");
	LONGJMP(timer_env,1);
}

void bus_handler(int sig)
{
#if defined(SIGBUS)
	signal(SIGBUS,bus_handler);
	error("internal error (BUS ERROR)");
#endif
}

void fpe_handler(int sig)
{
#if defined(SIGFPE)
	signal(SIGFPE,fpe_handler);
	error("internal error (FPE)");
#endif
}

void pipe_handler(int sig)
{
#if defined(SIGPIPE)
	signal(SIGPIPE,pipe_handler);
	end_critical();
	error("internal error (BROKEN PIPE)");
#endif
}

void resize_buffer()
{
}

void tty_init() {
}

void tty_reset() {
}

extern int evalstatline;

void set_lasterror(char *s)
{
	strncpy(LastError,s,BUFSIZ);
	LastError[BUFSIZ-1] = 0;
}

SNODE error_snode;

void error(char *s)
{
	SNODE *snp;

#if !defined(VISUAL)
	if ( timer_is_set )
		alrm_handler(SIGVTALRM);
#endif
	fprintf(stderr,"%s\n",s);
	set_lasterror(s);
	if ( CPVS != GPVS ) {
		if ( CPVS->usrf && CPVS->usrf && CPVS->usrf->f.usrf )
			searchsn(&BDY(CPVS->usrf->f.usrf),evalstatline,&snp);
		if ( snp )
			error_snode = *snp;
		else
			error_snode = 0;
	} else
		error_snode = 0;
	if ( do_file ) {
		char errbuf[BUFSIZ*5]; /* sufficient to store stack information ? */

		sprintf(errbuf,"%s\n",s);
		showpos_to_string(errbuf+strlen(errbuf));
		set_lasterror(errbuf);
		ExitAsir();
	}
	if ( debug_mode )
		LONGJMP(debug_env,1);
	if ( CPVS != GPVS )
		if ( do_server_in_X11 || isatty(0) )
			bp(error_snode);
	if ( read_exec_file )
		read_exec_file = 0;
	resetenv("return to toplevel");
}

#if !defined(VISUAL)
#include <sys/time.h>

void set_timer(int interval)
{
	struct itimerval it;

	it.it_interval.tv_sec = interval;
	it.it_interval.tv_usec = 0;
	it.it_value.tv_sec = interval;
	it.it_value.tv_usec = 0;
	setitimer(ITIMER_VIRTUAL,&it,0);
	signal(SIGVTALRM,alrm_handler);
	timer_is_set = 1;
}

void reset_timer()
{
	struct itimerval it;

	it.it_interval.tv_sec = 0;
	it.it_interval.tv_usec = 0;
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 0;
	setitimer(ITIMER_VIRTUAL,&it,0);
	signal(SIGVTALRM,SIG_IGN);
	timer_is_set = 0;
}
#endif

unsigned int get_asir_version();
char *get_asir_distribution();

void copyright() {
#if defined(INTERVAL)
	printf("This is Risa/Asir + Interval Arithmetic, Version %d (%s Distribution).\n",
		get_asir_version(), get_asir_distribution());
#else
	printf("This is Risa/Asir, Version %d (%s Distribution).\n",
		get_asir_version(), get_asir_distribution());
#endif
	printf("Copyright (C) 1994-2000, all rights reserved, FUJITSU LABORATORIES LIMITED.\n");
	printf("Copyright 2000,2001, Risa/Asir committers, http://www.openxm.org/.\n");
	printf("GC 6.1(alpha5) copyright 2001, H-J. Boehm, A. J. Demers, Xerox, SGI, HP.\n");
	printf("PARI 2.2.1(alpha), copyright (C) 2000,\n");
	printf("     C. Batut, K. Belabas, D. Bernardi, H. Cohen and M. Olivier.\n");
}


