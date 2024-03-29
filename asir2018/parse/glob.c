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
 * $OpenXM: OpenXM_contrib2/asir2018/parse/glob.c,v 1.10 2021/03/25 23:25:03 noro Exp $
*/
#include "ca.h"
#include "al.h"
#include "parse.h"
#include "ox.h"
#include <signal.h>
#if !defined(VISUAL) && !defined(__MINGW32__) && !defined(_PA_RISC1_1) && !defined(linux) && !defined(SYSV) && !defined(__CYGWIN__) && !defined(__INTERIX) && !defined(__FreeBSD__)
#include <sgtty.h>
#endif

#if defined(VISUAL) || defined(__MINGW32__)
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#endif

#if defined(SYSV) && !defined(_IBMR2)
#include <sys/ttold.h>
#endif

#if defined(VISUAL) || defined(__MINGW32__)
#define HISTORY asir_history
#endif

#if defined(FEP)
int read_history(const char *filename);
int write_history(const char *filename);
void stifle_history(int max);
void using_history(void);
extern char history_expansion_char;
extern char *history_no_expand_chars;
int asir_history_inhibit_expansion_function(const char *s,int i);
extern int (*history_inhibit_expansion_function)(const char *s,int i);
static char asir_history_no_expand_chars[] = { ' ', '\t', '\n', '=', '(', '+', '-', '*', '&', '|', ')', '^', ';', '$', '\0' };
#endif

#define MAXHIST 100

extern FILE *asir_out;

INFILE asir_infile;
JMP_BUF main_env,debug_env,timer_env,exec_env;
int little_endian,debug_mode,no_debug_on_error;
char *asir_libdir;
char *asir_contrib_dir;
char *asir_private_dir;
char *asir_pager;

NODE usrf,sysf,noargsysf,ubinf,parif;
NODE ONENODE;
int main_parser, allow_create_var, ox_do_copy, ox_do_count, ox_count_length;
int ox_file_io, ox_need_conv;
char *ox_copy_bptr;
char *parse_strp;
SNODE parse_snode;
FUNC parse_targetf;
FILE *ox_istream,*ox_ostream;
int do_server_in_X11;
Obj LastVal;
LIST LastStackTrace;
char LastError[BUFSIZ];
int timer_is_set;
NODE current_option;
NODE user_int_handler, user_quit_handler;

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
VL LASTCO;

struct oVS oGPVS,oAPVS,oEPVS,oPPVS;
VS GPVS = &oGPVS;
VS APVS = &oAPVS;
VS EPVS = &oEPVS;
VS PPVS = &oPPVS;
VS CPVS,MPVS;

NODE MODULE_LIST;
MODULE CUR_MODULE;
char *CUR_FUNC;

struct oSYMBOL oGrlex, oGlex, oLex;
SYMBOL Symbol_grlex = &oGrlex;
SYMBOL Symbol_glex = &oGlex;
SYMBOL Symbol_lex = &oLex;;

struct oF oF_TRUE,oF_FALSE;
F F_TRUE = &oF_TRUE;  
F F_FALSE = &oF_FALSE;

#if defined(VISUAL) || defined(__MINGW32__)
char cppname[BUFSIZ] = "c:\\asir\\stdlib\\cpp ";
#else
char cppname[BUFSIZ] = "/lib/cpp ";
#endif
char asirname[BUFSIZ];
char displayname[BUFSIZ];

int Verbose;
int do_quiet;

void glob_init() {
  int i;
  
  for ( i = 0; i < 51; i++ ) {
    VR(&oVLIST[i]) = &oVAR[i]; NEXT(&oVLIST[i]) = &oVLIST[i+1];
  }
  VR(&oVLIST[i]) = &oVAR[i]; NEXT(&oVLIST[i]) = 0;
  LASTCO = &oVLIST[i];
  asir_reallocarray((char **)&GPVS->va,(int *)&GPVS->asize,(int *)&GPVS->n,(int)sizeof(struct oPV));
  asir_reallocarray((char **)&APVS->va,(int *)&APVS->asize,(int *)&APVS->n,(int)sizeof(struct oPV));
  asir_reallocarray((char **)&PPVS->va,(int *)&PPVS->asize,(int *)&PPVS->n,(int)sizeof(struct oPV));
  CPVS = GPVS;
  MKNODE(ONENODE,mkfnode(1,I_FORMULA,ONE),NULLP);
  OID(F_TRUE)=O_F; FOP(F_TRUE)=AL_TRUE; F_TRUE->arg.dummy = 0;
  OID(F_FALSE)=O_F; FOP(F_FALSE)=AL_FALSE; F_FALSE->arg.dummy = 0;
  OID(Symbol_grlex) = O_SYMBOL; Symbol_grlex->name = "@grlex"; 
  Symbol_grlex->value = 0;
  OID(Symbol_glex) = O_SYMBOL; Symbol_glex->name = "@glex"; 
  Symbol_glex->value = 1;
  OID(Symbol_lex) = O_SYMBOL; Symbol_lex->name = "@lex"; 
  Symbol_lex->value = 2;
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

void notdef2(Obj b,Obj *c)
{
  error("undefined arithmetic operation.");
}

int notdefi(VL vl,Obj a,Obj b)
{
  error("undefined arithmetic operation.");
  return 0;
}

int disable_debugger;
int do_asirrc;
int do_file;
char *do_filename;
int do_message;
int do_terse;
int do_fep;
int asir_setenv;
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
 * status = 3 absolute termination
 */

void asir_terminate(int status)
{
  int t;
  NODE n;

  /* called from engine in Windows */
  if ( status == 3 ) asir_infile = 0;

  if ( asir_infile && asir_infile->ready_for_longjmp )
    LONGJMP(asir_infile->jmpbuf,status);
  else {
    if ( user_quit_handler ) {
      if ( !do_terse ) {
        fprintf(stderr,"Calling the registered quit callbacks...");
#if defined(__MINGW32__)
        fflush(stderr);
#endif
      }
      for ( n = user_quit_handler; n; n = NEXT(n) )
        bevalf((FUNC)BDY(n),0);
      if ( !do_terse ) {
        fprintf(stderr, "done.\n");
#if defined(__MINGW32__)
        fflush(stderr);
#endif
      }
    }
    tty_reset();
#if defined(MPI)
    if ( !mpi_myid )
      close_allconnections();
    mpi_finalize();
#else
#if defined(SIGPIPE)
    set_signal(SIGPIPE,SIG_IGN);
#endif
    close_allconnections();
#endif
    if ( asir_out )
      fflush(asir_out);
#if defined(FEP)
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

Obj user_defined_prompt;

void prompt() {
  if ( !do_quiet && !do_fep && asir_infile->fp == stdin ) {
    fprintf(asir_out,"[%d] ",APVS->n);
    fflush(asir_out);
  } else if ( do_quiet && user_defined_prompt 
    && OID(user_defined_prompt)==O_STR) {
    fprintf(asir_out,BDY((STRING)user_defined_prompt),APVS->n);
    fflush(asir_out);
  }
}

void sprompt(char *ptr)
{
  sprintf(ptr,"[%d] ",APVS->n);
}

#if !defined(VISUAL) && !defined(__MINGW32__)
static int which(char *prog, char *path, char *buf, size_t size)
{
    char *tok;
    char delim[] = ":";
    char *path2  = malloc(strlen(path)+1);
    char *name   = malloc(size);
    int  proglen = strlen(prog)+3; /* "/" + prog + " \0" */

    if (!name || !path2) {
        return 0;
    }
    strcpy(path2, path);
    tok = strtok(path2, delim);
    while (tok != NULL) {
        if (size >= strlen(tok)) {
            sprintf(name, "%s/%s", tok, prog);
            if (access(name, X_OK&R_OK) == 0) {
                strcpy(buf, name);
                strcat(buf, " ");
                free(path2); free(name);
                return 1;
            }
            tok = strtok(NULL, delim);
        }
    }
    free(path2); free(name);
    return 0;
}
#endif

void cppname_init()
{
#if !defined(VISUAL) && !defined(__MINGW32__)
  char *oxhome;
  char oxcpp[BUFSIZ];
#define OXCPP "/bin/ox_cpp"

  if ( ( oxhome = getenv("OpenXM_HOME") ) != 0 ) {
    if ( strlen(oxhome)+strlen(OXCPP)<BUFSIZ ) {
      sprintf(oxcpp,"%s%s",oxhome,OXCPP);
      if ( access(oxcpp,X_OK&R_OK) == 0 ) {
            strcpy(cppname,oxcpp);
        return;
      }
    }
  }
    if (access(cppname, X_OK&R_OK) != 0) {
        which("cpp", "/lib:/usr/ccs/lib:/usr/bin", cppname, BUFSIZ) ||
        which("cpp", getenv("PATH"), cppname, BUFSIZ);
    }
#endif
}

FILE *in_fp;

void process_args(int ac,char **av)
{
  int nm,dv;
  do_asirrc = 1;
#if !defined(MPI)
  do_message = 1;
#endif
#if defined(VISUAL) && defined(VISUAL_CONSOLE)
  disable_debugger=1;
#endif
#if defined(FEP)
  do_fep = 1;
  history_no_expand_chars = asir_history_no_expand_chars;
/*  history_expansion_char = 0; */
  history_inhibit_expansion_function = asir_history_inhibit_expansion_function;
#endif
  do_quiet = 0;
  while ( ac > 0 ) {
    if ( !strcmp(*av,"-heap") && (ac >= 2) ) {
      GC_expand_hp(atoi(*(av+1))); av += 2; ac -= 2;
    } else if ( !strcmp(*av,"-adj") && (ac >= 2) ) {
      char *slash;
      slash = strrchr(*(av+1),'/');
      if ( slash ) {
        *slash = 0;
        nm = atoi(slash+1);
      }else {
        nm = 1;
      }
      dv = atoi(*(av+1));
      Risa_GC_set_adj(nm,dv);
      av += 2; ac -= 2;
    } else if ( !strcmp(*av,"-cpp") && (ac >= 2) ) {
      strcpy(cppname,*(av+1)); av += 2; ac -= 2;
    } else if ( !strcmp(*av,"-d") && (ac >= 2) ) {
#if defined(VISUAL) && defined(VISUAL_CONSOLE)
      disable_debugger=0;
#endif
      av += 2; ac -= 2;
    } else if ( !strcmp(*av,"-f") && (ac >= 2) ) {
      do_quiet = 1;
      in_fp = fopen(*(av+1),"r");
      if ( !in_fp ) {
        fprintf(stderr,"%s does not exist!\n",*(av+1));
#if defined(__MINGW32__)
        fflush(stderr);
#endif
        asir_terminate(1);
      }
      do_file = 1;
      do_filename = *(av+1);
      av += 2; ac -= 2;
    } else if ( !strcmp(*av,"-E") ) {
      asir_setenv = 1; av++; ac--;
    } else if ( !strcmp(*av,"-quiet") ) {
      do_quiet = 1; av++; ac--;
    } else if ( !strcmp(*av,"-norc") ) {
      do_asirrc = 0; av++; ac--;
    } else if ( !strcmp(*av,"-nomessage") ) {
      do_message = 0; av++; ac--;
    } else if ( !strcmp(*av,"-rootdir") && (ac >= 2) ) {
      set_rootdir(*(av+1)); av += 2; ac -= 2;
    } else if ( !strcmp(*av,"-maxheap") && (ac >= 2) ) {
      GC_set_max_heap_size(atoi(*(av+1))); av += 2; ac -= 2;
#if !defined(VISUAL) && !defined(__MINGW32__)
    } else if ( !strcmp(*av,"-display") && (ac >= 2) ) {
      strcpy(displayname,*(av+1)); av += 2; ac -= 2;
#endif
#if defined(FEP)
    } else if ( !strcmp(*av,"-fep") ) {
      do_fep = 1; av++; ac--;
    } else if ( !strcmp(*av,"-nofep") ) {
      do_fep = 0; av++; ac--;
#endif
    } else {
      fprintf(stderr,"%s : unknown option.\n",*av);
#if defined(__MINGW32__)
      fflush(stderr);
#endif
      asir_terminate(1);
    }
  }
#if defined(FEP)
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

#if defined(HAVE_SIGACTION)
void (*set_signal(int sig, void (*handler)(int)))(int)
{
  struct sigaction act;
  struct sigaction oldact;
  if (handler == SIG_IGN || handler == SIG_DFL) {
    return signal(sig,handler);
  }
  act.sa_handler=handler;
  act.sa_flags=0;
  act.sa_flags |= SA_RESTART;
  sigemptyset(&act.sa_mask);
  sigaction(sig,&act,&oldact);
  return oldact.sa_handler;
}
#endif

void sig_init() {
#if !defined(VISUAL) && !defined(__MINGW32__)
  set_signal(SIGINT,int_handler);
#else
  void register_ctrlc_handler();

  register_ctrlc_handler();
#endif
  set_signal(SIGSEGV,segv_handler);

#if defined(SIGFPE)
  set_signal(SIGFPE,fpe_handler);
#endif

#if defined(SIGPIPE)
  set_signal(SIGPIPE,pipe_handler);
#endif

#if defined(SIGILL)
  set_signal(SIGILL,ill_handler);
#endif

#if !defined(VISUAL) && !defined(__MINGW32__)
  set_signal(SIGBUS,bus_handler);
#endif
}

static void (*old_int)(int);

void asir_save_handler() {
  old_int = set_signal(SIGINT,SIG_IGN);
  set_signal(SIGINT,old_int);
}

void asir_set_handler() {
  set_signal(SIGINT,int_handler);
}

void asir_reset_handler() {
  set_signal(SIGINT,old_int);
}

extern int I_am_server;

void resetenv(char *s)
{
  extern FILE *outfile;

  fprintf(stderr,"%s\n",s);
  while ( NEXT(asir_infile) )
    closecurrentinput();
  resetpvs();
#if !defined(VISUAL) && !defined(__MINGW32__)
  if ( do_server_in_X11 )
#endif
    show_debug_window(0);
#if defined(VISUAL_LIB)
  w_noflush_stderr(0);
#endif
#if defined(__MINGW32__)
  fflush(stderr);
#endif
  asir_out = stdout;
  /* restore states */
  reset_engine();
  reset_io();
#if !defined(VISUAL) && !defined(__MINGW32__)
  reset_timer();
#endif
  LONGJMP(main_env,1);
}

void fatal(int n)
{
  resetenv("return to toplevel");
}

extern int ox_int_received, critical_when_signal;
extern int in_gc, caught_intr;
extern int ox_get_pari_result;
extern int thread_working;

void int_handler(int sig)
{
  extern NODE PVSS;
  NODE t;

  if ( do_file || disable_debugger ) {
    LEAVE_SIGNAL_CS_ALL;
    ExitAsir();
  }
  if ( !ox_get_pari_result && critical_when_signal ) {
    ox_int_received = 1;
    return;
  }
  if ( in_gc ) {
    caught_intr = 1;
    return;
  }
  if ( thread_working != 0 ) {
    return;
  }
#if defined(VISUAL) || defined(__MINGW32__)
  suspend_timer();
#endif
  set_signal(SIGINT,SIG_IGN);
#if !defined(VISUAL) && !defined(__MINGW32__) 
    if ( do_server_in_X11 ) {
    debug(PVSS?((VS)BDY(PVSS))->usrf->f.usrf->body:0);
     restore_handler();
    return;
  }
#endif
#if defined(linux)
#if !defined(__GLIBC__) && !defined(ANDROID)
  if( __freadahead(stdin) > 0 ) __fpurge(stdin);
#else
  while ( FP_DATA_IS_AVAILABLE(stdin) )
    getchar();
#endif
#endif
  while ( 1 ) {
    char buf[BUFSIZ];
    char c;

    fprintf(stderr,"interrupt ?(q/t/c/d/u/w/?) "); fflush(stderr);
    buf[0] = '\n';
    while ( !feof(stdin) && buf[0] == '\n' )
      fgets(buf,BUFSIZ,stdin);
    if ( feof(stdin) ) {
      clearerr(stdin);
      continue;
    }
    switch ( c = buf[0] ) {
      case 'q':
        while ( 1 ) {
          fprintf(stderr,"Abort this session? (y or n) "); fflush(stderr);
          fgets(buf,BUFSIZ,stdin);
          if ( !strncmp(buf,"y",1) ) {
            fprintf(stderr,"Bye\n");
#if defined(__MINGW32__)
            fflush(stderr);
#endif
            /* for terminating myself */
            asir_infile = 0;
            asir_terminate(1);
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
          if ( user_int_handler ) {
            if ( !do_terse ) {
              fprintf(stderr,
                "Calling the registered exception callbacks...");
#if defined(__MINGW32__)
              fflush(stderr);
#endif
            }
            for ( t = user_int_handler; t; t = NEXT(t) )
              bevalf((FUNC)BDY(t),0);
            if ( !do_terse ) {
              fprintf(stderr, "done.\n");
#if defined(__MINGW32__)
              fflush(stderr);
#endif
            }
          }
        }
        LEAVE_SIGNAL_CS_ALL;
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
        showpos();
        break;
      case '?': 
        fprintf(stderr, "q:quit t:toplevel c:continue d:debug u:call registered handler w:where\n");
#if defined(__MINGW32__)
        fflush(stderr);
#endif
        break;
      default:
        break;
    }
  }
}

void restore_handler() {
#if defined(VISUAL) || defined(__MINGW32__)
  resume_timer();
#endif
#if defined(SIGINT)
  set_signal(SIGINT,int_handler);
#endif
}

void segv_handler(int sig)
{
#if defined(SIGSEGV)
  set_signal_for_restart(SIGSEGV,segv_handler);
  error("internal error (SEGV)");
#endif
}

void ill_handler(int sig)
{
#if defined(SIGILL)
  set_signal_for_restart(SIGILL,ill_handler);
  error("illegal instruction (ILL)");
#endif
}

#if defined(__DARWIN__)
#define SIGNAL_FOR_TIMER SIGALRM
#define ITIMER_TYPE ITIMER_REAL
#else
#define SIGNAL_FOR_TIMER SIGVTALRM
#define ITIMER_TYPE ITIMER_VIRTUAL
#endif

void alrm_handler(int sig)
{
  fprintf(stderr,"interval timer expired (VTALRM)\n");
#if defined(__MINGW32__)
  fflush(stderr);
#endif
  LONGJMP(timer_env,1);
}

void bus_handler(int sig)
{
#if defined(SIGBUS)
  set_signal_for_restart(SIGBUS,bus_handler);
  error("internal error (BUS ERROR)");
#endif
}

void fpe_handler(int sig)
{
#if defined(SIGFPE)
  set_signal_for_restart(SIGFPE,fpe_handler);
  error("internal error (FPE)");
#endif
}

void pipe_handler(int sig)
{
#if defined(SIGPIPE)
  set_signal_for_restart(SIGPIPE,pipe_handler);
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
int error_in_timer;

void error(char *s)
{
  SNODE *snp=0;

#if !defined(VISUAL) && !defined(__MINGW32__)
  if ( !error_in_timer && timer_is_set )
    alrm_handler(SIGNAL_FOR_TIMER);
#endif
  fprintf(stderr,"%s\n",s);
#if defined(__MINGW32__)
  fflush(stderr);
#endif
  set_lasterror(s);
  if ( CPVS != GPVS ) {
    if ( CPVS && CPVS->usrf && CPVS->usrf->f.usrf )
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
    if ( !no_debug_on_error && (do_server_in_X11 || isatty(0)) )
      bp(error_snode);
  if ( I_am_server )
    showpos_to_list(&LastStackTrace);  
  resetenv("return to toplevel");
}

void goto_toplevel(char *s)
{
  SNODE *snp=0;

#if !defined(VISUAL) && !defined(__MINGW32__)
  if ( timer_is_set )
    alrm_handler(SIGNAL_FOR_TIMER);
#endif
  fprintf(stderr,"%s\n",s);
#if defined(__MINGW32__)
  fflush(stderr);
#endif
  if ( do_file ) {
    char errbuf[BUFSIZ*5]; /* sufficient to store stack information ? */

    sprintf(errbuf,"%s\n",s);
    showpos_to_string(errbuf+strlen(errbuf));
    ExitAsir();
  }
  resetenv("return to toplevel");
}

#if !defined(VISUAL) && !defined(__MINGW32__)
#include <sys/time.h>

void set_timer(int interval)
{
  struct itimerval it;

  it.it_interval.tv_sec = interval;
  it.it_interval.tv_usec = 0;
  it.it_value.tv_sec = interval;
  it.it_value.tv_usec = 0;
  setitimer(ITIMER_TYPE,&it,0);
  set_signal(SIGNAL_FOR_TIMER,alrm_handler);
  timer_is_set = 1;
}

void reset_timer()
{
  struct itimerval it;

  it.it_interval.tv_sec = 0;
  it.it_interval.tv_usec = 0;
  it.it_value.tv_sec = 0;
  it.it_value.tv_usec = 0;
  setitimer(ITIMER_TYPE,&it,0);
  set_signal(SIGNAL_FOR_TIMER,SIG_IGN);
  timer_is_set = 0;
}
#endif

unsigned int get_asir_version();
char *get_asir_distribution();

char *get_gcversion() 
{
  return "GC 7.6.12 copyright 1988-2018, H-J. Boehm, A. J. Demers, Xerox, SGI, HP, I. Maidanski.\n";
}

char *get_pariversion() 
{
  return "";
}

char *get_intervalversion()
{
#if defined(INTERVAL)
  return " + Interval Arithmetic";
#else
  return "";
#endif
}

void copyright()
{
  char *scopyright();
  fputs(scopyright(), stdout);
}

char *scopyright()
{
  static char *notice;
  char *s1, *s2, *s3, *s4;
  int d, len;
  char *format = "This is Risa/Asir%s, full GMP Version %d (%s Distribution).\nCopyright (C) 1994-2000, all rights reserved, FUJITSU LABORATORIES LIMITED.\nCopyright 2000-2021, Risa/Asir committers, http://www.openxm.org/.\n%s%s";
  if (!notice) {
    s1 = get_intervalversion();
    s2 = get_asir_distribution();
    s3 = get_gcversion();
    s4 = get_pariversion();
    d  = get_asir_version();
    len = (strlen(format)-8)+strlen(s1)+strlen(s2)+strlen(s3)+strlen(s4)+sizeof(int)*3;
    notice = MALLOC(len+1);
    sprintf(notice, format, s1, d, s2, s3, s4);
  }
  return notice;
}

#if defined(VISUAL) || defined(__MINGW32__)
int recv_intr;

static CRITICAL_SECTION signal_cs;
static int initialized_signal_cs;
static int signal_cs_count;

static void init_signal_cs()
{
    if (!initialized_signal_cs) {
        InitializeCriticalSection(&signal_cs);
    initialized_signal_cs=1;
        signal_cs_count=0;
    }
}

void try_enter_signal_cs()
{
  init_signal_cs();
    if(TryEnterCriticalSection(&signal_cs)) {
        signal_cs_count++;
    }
}

void enter_signal_cs()
{
    init_signal_cs();
    EnterCriticalSection(&signal_cs);
    signal_cs_count++;
}

void leave_signal_cs()
{
  init_signal_cs();
    if(signal_cs_count>0) {
        LeaveCriticalSection(&signal_cs);
        signal_cs_count--;
    }
}

void leave_signal_cs_all()
{
    if (!initialized_signal_cs) {
        init_signal_cs();
    }
    while(signal_cs_count>0) {
        LeaveCriticalSection(&signal_cs);
        signal_cs_count--;
    }
}

void check_intr()
{
  extern int recv_intr;
  enter_signal_cs();
  if ( recv_intr ) {
    if ( recv_intr == 1 ) {
      recv_intr = 0;
      int_handler(SIGINT);
    } else {
      recv_intr = 0;
      ox_usr1_handler(0);
    }
  }
  leave_signal_cs_all();
}
#endif
