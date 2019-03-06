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
 * $OpenXM: OpenXM_contrib2/asir2018/builtin/ctrl.c,v 1.2 2018/09/28 08:20:27 noro Exp $
*/
#include "ca.h"
#include "parse.h"
#include <string.h>
#if defined(VISUAL) || defined(__MINGW32__)
#include <windows.h>
#include <winnls.h>
#else
#include <locale.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#endif

static struct {
  char *type;
  char *kernel;
  char *name;
  char *arch;
  char *release;
  char *full;
  char *lang;
} sysinfo;

void Pctrl();
void Psysinfo(LIST *rp);
static void get_sysinfo();

struct ftab ctrl_tab[] = {
  {"ctrl",Pctrl,-2},
  {"asir_env",Pctrl,-2},
  {"sysinfo", Psysinfo, 0},
  {0,0,0},
};

extern int error_in_timer;
extern int prtime,nez,echoback,bigfloat,evalef;
extern int debug_up;
extern int GC_max_heap_size,Verbose,hideargs,hex_output,do_server_in_X11;
extern int do_message,do_terse;
extern int ox_batch,ox_check,ox_exchange_mathcap;
extern int f4_nocheck;
extern int StrassenSize;
extern int outputstyle;
extern int fortran_output;
extern int real_digit;
extern int real_binary;
extern int allow_laurent;
#if defined(INTERVAL)
extern int zerorewrite;
extern int Itvplot;
#endif
extern int double_output;
extern int use_new_hensel;
extern int print_quote;
extern int show_crossref;
extern Obj user_defined_prompt;
extern int asir_setenv;
extern int show_orderspec;
extern int no_debug_on_error;
extern int diag_period;
extern int weight_check;
extern char **ASIRLOADPATH;
extern int ASIRLOADPATH_LEN;
extern int No_ox_reset;
extern int plot_by_bigfloat;
extern int debug_plot;

static struct keyval {
  char *key;
  int *val;
  char *desc;
} ctrls[] = {
  {"StrassenSize",&StrassenSize,"Determines the parameter in Strassen-Winograd matrix multiplication algorithm."},
  {"allow_laurent",&allow_laurent,"If set to 1, negative exponents are allowed in monomials."},
  {"asir_setenv",&asir_setenv,"Not used."},
  {"bigfloat",&bigfloat,"If set to 1, MPFR bigfloat functions are used for floating point evaluation." },
  {"cputime",&prtime,"If set to 1, the CPU time at the toplevel is shown." },
  {"debug_up",&debug_up,"If set to 1, some debug messages are printed in special functions for univariate polynomials."},
  {"debug_window",&do_server_in_X11,"If set to 1, an input window for debugging remote server are shown."},
  {"diag_period",&diag_period,"Determines the frequency of the intermediate inter-reduction in nd_gr()."},
  {"double_output",&double_output,"If set to 1, floating point numbers are printed in the style ddd.ddd."},
  {"echo",&echoback,"If set to 1, characters read by the input function are printed."  },
  {"error_in_timer",&error_in_timer,"If set to 1, the usual error handler is executed when the timer is expired."},
  {"evalef",&evalef,"If set to 1, elementary functions are evaluated immediately."},
  {"f4_nocheck",&f4_nocheck,"If set to 1, correctness check of the result of modular computation are omitted in nd_f4()."},
  {"fake_ox_reset",&No_ox_reset,"Determines the treatment of OX reset request for a server which does not implement the reset protocol."},
  {"fortran_output",&fortran_output,"If set to 1, ** is used instead of ^ for representing the power."},
  {"hex",&hex_output,"If set to 1, integers are printed by the hexadecimal notation."},
  {"hideargs",&hideargs,"If set to 1, the arguments of a function call are not printed."},
  {"message",&do_message,"If set to 1, an opening message is printed in ox_asir and ox_plot."},
  {"new_hensel",&use_new_hensel,"If set to 1, a function hensel2() is used in the univariate factorizer over Q."},
  {"nez",&nez,"If set to 1, a new version of EZGCD implementation is used."  },
  {"no_debug_on_error",&no_debug_on_error,"If set to 1, the debug mode is not used."},
  {"no_ox_reset",&No_ox_reset,"Determines the treatment of OX reset request for a server which does not implement the reset protocol."},
  {"no_prompt",&do_quiet,"If set to 1, prompts are not shown."},
  {"outputstyle",&outputstyle,"If set to 1, structured data such as matrices and vectors are printed in the style mat(...), vect(...)."},
  {"ox_batch",&ox_batch,"If set to 1, the OX stream are not flushed at each sending of an OX data."},
  {"ox_check",&ox_check,"If set to 1, mathcap check is done for OpenXM date communication."},
  {"ox_exchange_mathcap",&ox_exchange_mathcap,"If set to 1, mathcaps are exchanged."},
  {"print_quote",&print_quote,"Determines the behavior of the printed form of a quote."},
  {"quiet_mode",&do_quiet,"If set to 1, the copyright notices are not printed at the beginning of the session."},
  {"real_binary",&real_binary,"If set to 1, a floating point number is printed by the binary notation."},
  {"real_digit",&real_digit,"Determines the number of digits to appear after the decimal point."},
  {"show_crossref",&show_crossref,"If set to 1, cross-references are shown when a program file is loaded."},
  {"show_orderspec",&show_orderspec,"If set to 1, the specification of a composite term order is printed upon its creation."},
  {"terse",&do_terse,"If set to 1, messages are not printed when user-defined callbacks are executed."},
  {"verbose",&Verbose,"If set to 1, a warning is printed if a function is redefined."},
  {"weight_check",&weight_check,"If set to 1, an overflow check for the given weight vector is done before starting the Groebner basis computation."},
#if defined(DO_PLOT)
  {"plot_by_bigfloat",&plot_by_bigfloat,"If set to 1, computation is done by using MPFR bigfloat functions in ox_plot."},
  {"debug_plot",&debug_plot,"If set to 1, ox_plot is executed with the message window."},
#endif
#if defined(INTERVAL)
  {"zerorewrite",&zerorewrite,""},
  {"itvplotsize",&Itvplot,""},
#endif
  {0,0},
};

LIST create_control_value(char *keystr,Obj value,char *descstr,int withdesc)
{
  STRING key,desc;
  NODE nd;
  LIST list;

  MKSTR(key,keystr); 
  if ( withdesc ) {
    MKSTR(desc,descstr);
    nd = mknode(3,key,value,desc);
  } else nd = mknode(2,key,value);
  MKLIST(list,nd); 
  return list;
}

extern Z ox_pari_stream;
extern int ox_pari_stream_initialized;
extern P ox_pari_starting_function;

LIST create_control_values(int withdesc)
{
  int n,i;
  NODE top,top1,nd,node,p;
  LIST list,l;
  STRING s;
  char *descstr;
  Z val;
  Q adj;
  int nm,dv;
  Z num,den;

  n = sizeof(ctrls)/sizeof(struct keyval)-1;
  top = 0;
  for ( i = n-1; i >= 0; i-- ) {
    STOZ(*(ctrls[i].val),val);
    list = create_control_value(ctrls[i].key,(Obj)val,ctrls[i].desc,withdesc);
    MKNODE(top1,list,top); top = top1;
  }
/* adj */
  Risa_GC_get_adj(&nm,&dv); UTOZ(nm,num); UTOZ(dv,den); divq((Q)num,(Q)den,&adj);
  descstr = "Determines the parameter for Boehm's GC.";
  list = create_control_value("adj",(Obj)adj,descstr,withdesc);
  MKNODE(top1,list,top); top = top1;

/* prompt */
  descstr = "Determines the user-defined prompt.";
  list = create_control_value("prompt",(Obj)user_defined_prompt,descstr,withdesc);
  MKNODE(top1,list,top); top = top1;

/* loadpath */
  node = 0;
  if( ASIRLOADPATH[0] ) {
    for(i=0; ASIRLOADPATH[i]; i++);
    for(i--,p=NULL; i>=0; i--,p=node) {
      MKSTR(s,ASIRLOADPATH[i]);
      MKNODE(node,s,p);
    }
  }
  MKLIST(l,node);
  descstr = "List of paths in ASIRLOADPATH.";
  list = create_control_value("loadpath",(Obj)l,descstr,withdesc);
  MKNODE(top1,list,top); top = top1;

/* oxpari_id */
  if(!ox_pari_stream_initialized) {
    STOZ(-1,val);
  } else val = ox_pari_stream;
  descstr = "Id of ox_pari.";
  list = create_control_value("oxpari_id",(Obj)val,descstr,withdesc);
  MKNODE(top1,list,top); top = top1;

/* oxpari_start */
  val = (Z)ox_pari_starting_function;
  descstr = "oxpari starting function.";
  list = create_control_value("oxpari_start",(Obj)val,descstr,withdesc);
  MKNODE(top1,list,top); top = top1;

  MKLIST(list,top);
  return list;
}

void Pctrl(NODE arg,Obj *rp)
{
  int t,i,n,desc=0;
  int nm,dv;
  Z num,den,z;
  Q c;
  char *key;
  char buf[BUFSIZ];
  char *str;
  STRING s;
  NODE node,p,opt;
  LIST list;
  P f;
  Obj value;

  if ( !arg ) {
    if ( current_option ) {
      for ( opt = current_option; opt; opt = NEXT(opt) ) {
        p = BDY((LIST)BDY(opt));
        key = BDY((STRING)BDY(p));
        value = (Obj)BDY(NEXT(p));
        if ( !strcmp(key,"desc") && value ) {
          desc = value ? 1 : 0;
          break;
        }
      }
    }
    *rp = (Obj)create_control_values(desc);
    return;
  }
  key = BDY((STRING)ARG0(arg));
  if ( !strcmp(key,"adj") ) {
    /* special treatment is necessary for "adj" */
    if ( argc(arg) == 1 ) {
      Risa_GC_get_adj(&nm,&dv);
      UTOZ(dv,num);
      UTOZ(nm,den);
      divq((Q)num,(Q)den,(Q *)rp);
    } else {
      absq((Q)ARG1(arg),&c);
      if ( !c )
        error("ctrl : adj : invalid argument");
      nmq(c,&num);
      dnq(c,&den);
      Risa_GC_set_adj(ZTOS(den),ZTOS(num));
      *rp = (Obj)c;
    }
    return;
  } else if ( !strcmp(key,"prompt") ) {
    /* special treatment is necessary for "prompt" */
    if ( argc(arg) == 1 ) {
      if ((!do_quiet) && (user_defined_prompt == (Obj)0)) *rp=(Obj)ONE;
          else *rp = (Obj)user_defined_prompt;
    } else {
      c = (Q)ARG1(arg);
      if ( !c ) {
        do_quiet = 1;
        user_defined_prompt = 0;
        *rp = 0;
      } else if ( OID(c) == O_STR ) {
        str = BDY((STRING)c);
         for ( i = 0, n = 0; str[i]; i++ )
          if ( str[i] == '%' )
            n++;
        if ( n >= 2 )
          error("ctrl : prompt : invalid prompt specification");  
        do_quiet = 1;
        user_defined_prompt = (Obj)c;
        *rp = (Obj)c;
      } else if ( NUM(c) && RATN(c) && UNIQ(c) ) {
        user_defined_prompt = 0;
        do_quiet = 0;
      } else {
        error("ctrl : prompt : invalid argument");  
      }
    }
    return;
  } else if ( !strcmp(key,"loadpath") ) {
    *rp = 0;
    if ( argc(arg) == 1 ) {
      if( ASIRLOADPATH[0] ) {
        for(i=0; ASIRLOADPATH[i]; i++) {
        }
        for(i--,p=NULL; i>=0; i--,p=node) {
          MKSTR(s,ASIRLOADPATH[i]);
          MKNODE(node,s,p);
        }
        MKLIST(list,node);
        *rp = (Obj)list;
      }
    } else {
      list = (LIST)ARG1(arg);
      if ( OID(list) == O_LIST ) {
        for(i=0,p=BDY(list); p; i++,p=NEXT(p)) {
          s=(STRING)BDY(p);
          if(!s || OID(s)!=O_STR) {
            error("ctrl : loadpath : invalid argument");
          }
        }
        if(i >= ASIRLOADPATH_LEN) {
          ASIRLOADPATH_LEN = i+1;
          ASIRLOADPATH = (char **)MALLOC(sizeof(char *)*ASIRLOADPATH_LEN);
        }
        for(i=0,p=BDY(list); p; i++,p=NEXT(p)) {
          ASIRLOADPATH[i] = (char *)BDY((STRING)BDY(p));
        }
        ASIRLOADPATH[i] = NULL;
      }else {
        error("ctrl : loadpath : invalid argument");
      }
    }
    return;
  } else if ( !strcmp(key,"oxpari_id") ) {
    if ( argc(arg) == 1 ) {
      if(!ox_pari_stream_initialized) {
        STOZ(-1,z);
        *rp = (Obj)z;
      }else {
        *rp = (Obj)ox_pari_stream;
      }
    }else {
      c = (Q)ARG1(arg);
      if ( !c || ( NUM(c) && INT(c) && sgnq(c)>0 ) ) {
        ox_pari_stream_initialized = 1;
        ox_pari_stream = (Z)c;
        *rp = (Obj)c;
      }else {
        STOZ(-1,z);
        *rp = (Obj)z;
      }
    }
    return;
  } else if ( !strcmp(key,"oxpari_start") ) {
    if ( argc(arg) == 1 ) {
      *rp = (Obj)ox_pari_starting_function;
    } else {
      f = (P)ARG1(arg);
      if ( !f || OID(f) == O_P) {
        ox_pari_starting_function = f;
        *rp = (Obj)f;
      }else {
        *rp = 0;
      }
    }
    return;
  }
  for ( i = 0; ctrls[i].key; i++ )
    if ( !strcmp(key,ctrls[i].key) )
      break;
  if ( ctrls[i].key ) {
    if ( argc(arg) == 1 )
      t = *ctrls[i].val;
    else
      *ctrls[i].val = t = ZTOS((Q)ARG1(arg));
    STOZ(t,z);
    *rp = (Obj)z;
  } else {
    sprintf(buf,"ctrl : %s : no such key",key);
    error(buf);
  }
}

void Psysinfo(LIST *rp)
{
    int i;
    NODE n,p;
    STRING s[7];

    get_sysinfo();
    MKSTR(s[0],sysinfo.type);  MKSTR(s[1],sysinfo.kernel);   MKSTR(s[2],sysinfo.name);
    MKSTR(s[3],sysinfo.arch);  MKSTR(s[4],sysinfo.release);  MKSTR(s[5],sysinfo.full);
    MKSTR(s[6],sysinfo.lang);
    for(i=6,p=NULL; i>=0; i--,p=n) {
        MKNODE(n,s[i],p);
    }
    MKLIST(*rp,n);
}

#if !defined(VISUAL) && !defined(__MINGW32__)

static char *get_lang()
{
    char *c, *p, *q;
    c = setlocale(LC_ALL, NULL); /* saving current locale */
    p = setlocale(LC_ALL, "");
    q = (char *)MALLOC(strlen(p)+1);
    strcpy(q,p);
    setlocale(LC_ALL, c);        /* restoring current locale */
    return q;
}

static char *myuname(char *option)
{
    char buf[BUFSIZ];
    char *s;
    int fd[2], status, pid;
    *buf = 0;
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) < 0) {
        *buf = 0; return NULL;
    }
    pid = fork();
    if (pid < 0) {
        return NULL;
    }else if (pid == 0) {
        dup2(fd[1], 1);
        close(2);
        execlp("uname", "uname", option, NULL);
    }
    waitpid(pid, &status, 0);
    if (status) { /* error */
        return NULL;
    }
    s = buf;
    if( !read(fd[0], s, BUFSIZ-1) || (s = strchr(s, '\n')) ) {
        *s = 0;
    }
    close(fd[0]);
    close(fd[1]);
    s = (char *)MALLOC(strlen(buf)+1);
    strcpy(s, buf);
    return s;
}

static void get_sysinfo()
{
    static struct utsname u;
    static int initialized = 0;
    if (initialized) {
        return;
    }
    initialized = 1;
    uname(&u);
    sysinfo.kernel = u.sysname;
#if defined(__DARWIN__)
    sysinfo.type   = "macosx";
    sysinfo.name   = sysinfo.kernel;
#else
    sysinfo.type   = "unix";
    sysinfo.name   = myuname("-o"); // not work on Darwin
    if (!sysinfo.name) {
        sysinfo.name = sysinfo.kernel;
    }
#endif
    sysinfo.arch   = u.machine;
    sysinfo.release= u.release;
    sysinfo.full   = myuname("-a");
    sysinfo.lang   = get_lang();
}

#else

/* http://msdn.microsoft.com/ja-jp/library/windows/desktop/ms724834%28v=vs.85%29.aspx */
static char *osnameNT(int major, int minor)
{
    if ((major == 3 && minor == 51) || (major == 4 && minor == 0)) {
        return "WindowsNT";
    }else if (major == 5 && minor == 0) {
        return "Windows2000";
    }else if (major == 5 && minor == 1) {
        return "WindowsXP";
    }else if (major == 5 && minor == 2) {
        return "Windows2003 Server";
    }else if (major == 6 && minor == 0) {
        return "WindowsVista";
    }else if (major == 6 && minor == 1) {
        return "Windows7";
    }else if (major == 6 && minor == 2) {
        return "Windows8";
    }else if (major == 6 && minor == 3) {
        return "Windows8.1";
    }
    return "unknown";
}

static char *osname95(int major, int minor)
{
    if (major == 4 && minor == 0) {
        return "Windows95";
    }else if (major == 4 && minor == 10) {
        return "Windows98";
    }else if (major == 4 && minor == 90) {
        return "WindowsMe";
    }
    return "unknown";
}

static char *get_lang()
{
    char lang[BUFSIZ];
    char *s;
    if(GetLocaleInfo(GetUserDefaultLCID(), LOCALE_SISO639LANGNAME, lang, BUFSIZ)) {
        s = (char *)MALLOC(strlen(lang)+1);
        strcpy(s,lang);
        return s;
    }
    return "en"; // English
}

static void get_sysinfo()
{
    int arch64 = 0;
    char buf[BUFSIZ];
    OSVERSIONINFO v;
    char *s;
    static int initialized = 0;

    if (initialized) {
        return;
    }
    initialized = 1;

    v.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&v);

    sysinfo.type = "windows";
    sysinfo.arch = "x86";
    sprintf(buf, "%d.%d.%d", v.dwMajorVersion, v.dwMinorVersion, v.dwBuildNumber);
    s = (char *)MALLOC(strlen(buf)+1);
    strcpy(s, buf);
    sysinfo.release = s;

    if (v.dwPlatformId == VER_PLATFORM_WIN32_NT) {
        sysinfo.kernel = "WindowsNT";
        sysinfo.name = osnameNT(v.dwMajorVersion, v.dwMinorVersion);

#if defined(_WIN64)
        sysinfo.arch = "x86_64";
#else
        // IsWow64Process exists on WindowsXP SP2 or later
        if(GetProcAddress(GetModuleHandle("kernel32.dll"), "IsWow64Process")) {
            IsWow64Process(GetCurrentProcess(),&arch64);
            if(arch64) {
                sysinfo.arch = "x86_64";
            }
        }
#endif
    }else { /* v.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS */
        sysinfo.kernel = "Windows95";
        sysinfo.name = osname95(v.dwMajorVersion, v.dwMinorVersion);
    }
    sprintf(buf, "%s %s %s %s %s", sysinfo.kernel, sysinfo.release, sysinfo.name, v.szCSDVersion, sysinfo.arch);
    s = (char *)MALLOC(strlen(buf)+1);
    strcpy(s, buf);
    sysinfo.full = s;
    sysinfo.lang = get_lang();
}

#endif
