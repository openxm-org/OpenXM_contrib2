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
 * $OpenXM: OpenXM_contrib2/asir2018/builtin/miscf.c,v 1.4 2020/10/06 06:31:19 noro Exp $
*/
#include "ca.h"
#include "parse.h"
#include <string.h>
#if !defined(VISUAL) && defined(DO_PLOT)
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#endif

#if defined(VISUAL) || defined(__MINGW32__)
#include <direct.h>
#include <stdlib.h>
#include <windows.h>
#else
#include <unistd.h>
#include <sys/param.h>
#endif


void Pgetcwd(STRING *rp);
void Pchdir(NODE arg, Z *rp);
void Pset_secure_mode(NODE arg,Z *rp);
void Pset_secure_flag(NODE arg,Z *rp);
void Ptest(NODE arg,Z *rp);
void Psleep(NODE arg,Z *rp);
void Pmodule_list(LIST *rp);
void Premove_module(NODE arg,Z *rp);
void Pmodule_definedp(NODE arg,Z *rp);
void Pgetenv(NODE arg,STRING *rp);
void Pnull_command(NODE arg,Z *rp);
void Pquit(pointer *rp);
void Pdebug(pointer *rp);
void Pshell(NODE arg,Z *rp);
void Pnmono(NODE arg,Z *rp);
void Pheap(NODE arg,Z *rp);
void Pversion(NODE arg,Obj *rp);
void Pcopyright(STRING *rp);
void Pnez(NODE arg,Z *rp);
void Perror(NODE arg,Z *rp);
void Ptoplevel(NODE arg,Z *rp);
void Perror3(NODE arg,Z *rp);
void Pflist(NODE arg,LIST *rp);
void Pdelete_history(NODE arg,Z *rp);
void Ppause(LIST *rp);
void Pgc(LIST *rp);
void Pbatch(NODE arg,Z *rp);
void Pxpause(Z *rp);
void Psend_progress(NODE arg,Z *rp);
void Pget_addr(NODE arg,Z *rp);
void Phex_dump(NODE arg,Z *rp);
void Ppeek(NODE arg,Z *rp);
void Ppoke(NODE arg,Z *rp);
void Popt(NODE arg,Z *rp);
void delete_history(int,int);
void grab_pointer();

struct ftab misc_tab[] = {
  {"pwd",Pgetcwd,0},
  {"chdir",Pchdir,1},
  {"set_secure_mode",Pset_secure_mode,-1},
  {"set_secure_flag",Pset_secure_flag,-2},
  {"module_list",Pmodule_list,0},
  {"remove_module",Premove_module,1},
  {"module_definedp",Pmodule_definedp,1},
  {"sleep",Psleep,1},
  {"null_command",Pnull_command,-99999},
  {"getenv",Pgetenv,1},
  {"end",Pquit,0},
  {"quit",Pquit,0},
  {"debug",Pdebug,0},
  {"shell",Pshell,-2},
  {"heap",Pheap,-1},
  {"version",Pversion,-99999},
  {"copyright",Pcopyright,0},
  {"nmono",Pnmono,1},
  {"toplevel",Ptoplevel,-1},
  {"error",Perror,1},
  {"error3",Perror3,3},
  {"nez",Pnez,1},
  {"flist",Pflist,-1},
  {"delete_history",Pdelete_history,-2},
  {"pause",Ppause,0},
  {"gc",Pgc,0},
  {"batch",Pbatch,2},
  {"send_progress",Psend_progress,-2},
  {"get_addr",Pget_addr,1},
  {"hex_dump",Phex_dump,2},
  {"peek",Ppeek,1},
  {"poke",Ppoke,2},
#if !defined(VISUAL) && defined(DO_PLOT)
  {"xpause",Pxpause,0},
#endif
#if 0
  {"opt",Popt,1},
#endif
  {0,0,0},
};

void Pgetcwd(STRING *rp)
{
  char *r;
#if defined(VISUAL) || defined(__MINGW32__)
  char buf[_MAX_PATH];
  _getcwd(buf, _MAX_PATH);
#else
  char buf[MAXPATHLEN];
  getcwd(buf, MAXPATHLEN);
#endif
  r = (char *)MALLOC_ATOMIC(strlen(buf)+1);
  strcpy(r,buf);
  MKSTR(*rp,r);
}

void Pchdir(NODE arg, Z *rp)
{
  char *dir = BDY((STRING)ARG0(arg));
#if defined(VISUAL) || defined(__MINGW32__)
  int status = _chdir(dir);
#else
  int status = chdir(dir);
#endif
  STOZ(status,*rp);
}

void Pset_secure_mode(NODE arg,Z *rp)
{
  int s;
  if ( argc(arg) )
    setsecuremode(ZTOS((Q)ARG0(arg)));
  s = getsecuremode();
  STOZ(s,*rp);
}

void Pset_secure_flag(NODE arg,Z *rp)
{
  int ac,s,status;
  Obj f;
  char *fname;

  ac = argc(arg);
  if ( !ac )
    error("set_secure_flag : a function name must be specified");
  if ( ac == 2 )
    s = ZTOS((Q)ARG1(arg));
  else
    s = 1;
  f = ARG0(arg);
  if ( !f )
    error("set_secure_flag : invalid argument");
  switch ( OID(f) ) {
    case O_STR:
      fname = BDY((STRING)f); break;
    case O_P:
      fname = NAME(VR((P)f)); break;
    default:
      error("set_secure_flag : invalid argument"); break;
  }
  status = setsecureflag(fname,s);
  if ( status < 0 )
    error("set_secure_flag : function not found");
  STOZ(s,*rp);
}

void Ptest(NODE arg,Z *rp)
{
  int r;

  r = equalr(CO,ARG0(arg),ARG1(arg));
  STOZ(r,*rp);
}

void Psleep(NODE arg,Z *rp)
{
  int ms;

  ms = ZTOS((Q)ARG0(arg));
#if defined(VISUAL) || defined(__MINGW32__)
  Sleep(ms);
#else
  usleep(ms*1000);
#endif
  *rp = ONE;
}

void Pmodule_list(LIST *rp)
{
  char *name;
  NODE r,r1,m;
  STRING s;

  r = 0;
  for ( m = MODULE_LIST; m; m = NEXT(m) ) {
    MKSTR(s,((MODULE)BDY(m))->name);
    MKNODE(r1,s,r); r = r1;
  }
  MKLIST(*rp,r);
}

void Premove_module(NODE arg,Z *rp)
{
  NODE pm,m;
  char *name;

  asir_assert(ARG0(arg),O_STR,"remove_module");
  name = BDY((STRING)ARG0(arg));
  for ( pm = 0, m = MODULE_LIST; m; pm = m, m = NEXT(m) )
    if ( !strcmp(name,((MODULE)BDY(m))->name) ) {
      if ( !pm )
        MODULE_LIST = NEXT(MODULE_LIST);
      else
        NEXT(pm) = NEXT(m);
      *rp = ONE;
      return;
    }
  *rp = 0;
}

void Pmodule_definedp(NODE arg,Z *rp)
{
  NODE m;
  char *name;

  asir_assert(ARG0(arg),O_STR,"module_definedp");
  name = BDY((STRING)ARG0(arg));
    /* bug: the linear search is used here. The list of module shoud be sorted
       and cashed, and binary search should be used. */
  for (m = MODULE_LIST; m; m = NEXT(m) )
    if ( !strcmp(name,((MODULE)BDY(m))->name) ) {
      *rp = ONE;
      return ;
    }
  *rp = 0;
}

void Pgetenv(NODE arg,STRING *rp)
{
  char *e,*f;
  size_t len;

  e = (char *)getenv(BDY((STRING)ARG0(arg)));
  if ( e ) {
    len = strlen(e);
    f = (char *)MALLOC_ATOMIC(len+1);
    strcpy(f,e);
    MKSTR(*rp,f);
  } else
    *rp = 0;
}

void Pnull_command(NODE arg,Z *rp)
{
  *rp = 0;
}

void Pquit(pointer *rp)
{
  if ( asir_infile && NEXT(asir_infile) && asir_infile->ready_for_longjmp )
    LONGJMP(asir_infile->jmpbuf,1);
  else {
    closecurrentinput();
    asir_terminate(2);
  }
  *rp = 0;
}

void Pdebug(pointer *rp)
{
  debug(0); *rp = 0;
}

void Pshell(NODE arg,Z *rp)
{
  char *com = 0;
  char *pstr = 0;
  int status;
    
  if ( arg ) {
    asir_assert(ARG0(arg),O_STR,"shell");
    com = BDY((STRING)ARG0(arg));
    if ( NEXT(arg) )
      pstr = BDY((STRING)ARG1(arg));
  }
  status = system(com);
  STOZ(status,*rp);
}

void Pnmono(NODE arg,Z *rp)
{
  Obj obj;
  int n;

  obj = (Obj)ARG0(arg);
  if ( !obj || OID(obj) > O_R )
    *rp = 0;
  else
    switch (OID(obj)) {
      case O_N: case O_P:
        n = nmonop((P)obj); STOZ(n,*rp); break;
      case O_R:
        n = nmonop(NM((R)obj)) + nmonop(DN((R)obj));
        STOZ(n,*rp); break;
    }
}

void Pheap(NODE arg,Z *rp)
{
  size_t h0,h;
  
  h0 = get_heapsize();
  if ( arg ) {
    h = ZTOS((Q)ARG0(arg));
    if ( h > h0 )
      GC_expand_hp(h-h0);
  }
  h = get_heapsize();
  UTOZ(h,*rp);
}

unsigned int get_asir_version();
char *get_asir_distribution();

void Pversion(NODE arg,Obj *rp)
{
  unsigned int version;
  char *distribution;
  Z q;
  STRING str;
  NODE n;
  LIST l;

  version = get_asir_version();
  distribution = get_asir_distribution();
  UTOZ(version,q);
  if ( !argc(arg) )
    *rp = (Obj)q;
  else {
    MKSTR(str,distribution);  
    /* ONE means the new version */
    n = mknode(3,q,str,ONE);
    MKLIST(l,n);
    *rp = (Obj)l;
  }
}

char *scopyright();

void Pcopyright(STRING *rp)
{
  MKSTR(*rp,scopyright());
}

extern int nez;

void Pnez(NODE arg,Z *rp)
{
  nez = ARG0(arg) ? 1 : 0; *rp = 0;
}

void Perror(NODE arg,Z *rp)
{
  char *s;

  if ( !arg || !ARG0(arg) || (OID((Obj)ARG0(arg)) != O_STR) )
    s = "";
  else
    s = BDY((STRING)ARG0(arg));
  error(s);
  *rp = 0;
}

void Ptoplevel(NODE arg,Z *rp)
{
  char *s;

  if ( !arg || !ARG0(arg) || (OID((Obj)ARG0(arg)) != O_STR) )
    s = "";
  else
    s = BDY((STRING)ARG0(arg));
  goto_toplevel(s);
  *rp = 0;
}

void Perror3(NODE arg,Z *rp)
{
  int code;
  char *reason,*action;

  asir_assert(ARG0(arg),O_N,"error3");
  asir_assert(ARG1(arg),O_STR,"error3");
  asir_assert(ARG2(arg),O_STR,"error3");
  code = ZTOS((Q)ARG0(arg));
  reason = BDY((STRING)ARG1(arg));
  action = BDY((STRING)ARG2(arg));
#if defined(VISUAL) || defined(__MINGW32__)
  set_error(code,reason,action);
#endif
  error("");
  *rp = 0;
}

void Pflist(NODE arg,LIST *rp)
{
  char *n;
  STRING name;
  char *mname;
  NODE t,r,r0,m;
  LIST l;

  if ( argc(arg) ) {
    /* module name is specified */
    asir_assert(ARG0(arg),O_STR,"flist");
    mname = BDY((STRING)ARG0(arg));
    r0 = 0;
    for ( m = MODULE_LIST; m; m = NEXT(m) ) {
      if ( !strcmp(mname,((MODULE)BDY(m))->name) ) {
        t = ((MODULE)BDY(m))->usrf_list;
        for ( r0 = 0; t; t = NEXT(t) )
          if ( ((FUNC)BDY(t))->id != A_UNDEF ) {
            n = NAME((FUNC)BDY(t)); MKSTR(name,n);
            MKNODE(r,name,r0); r0 = r;
        }
      }
    }
  } else {
    for ( t = usrf, r0 = 0; t; t = NEXT(t) )
      if ( ((FUNC)BDY(t))->id != A_UNDEF ) {
        n = NAME((FUNC)BDY(t)); MKSTR(name,n);
        MKNODE(r,name,r0); r0 = r;
      }
    for ( t = ubinf; t; t = NEXT(t) )
      if ( ((FUNC)BDY(t))->id != A_UNDEF ) {
        n = NAME((FUNC)BDY(t)); MKSTR(name,n);
        MKNODE(r,name,r0); r0 = r;
      }
    for ( t = sysf; t; t = NEXT(t) )
      if ( ((FUNC)BDY(t))->id != A_UNDEF ) {
        n = NAME((FUNC)BDY(t)); MKSTR(name,n);
        MKNODE(r,name,r0); r0 = r;
      }
  }
  MKLIST(l,r0); *rp = l;
}

void Pdelete_history(NODE arg,Z *rp)
{
  switch ( argc(arg) ) {
    case 0: default:
      delete_history(0,(int)APVS->n);
      break;
    case 1:
      delete_history(ZTOS((Q)ARG0(arg)),1);
      break;
  }
  *rp = 0;
}

void delete_history(int start,int n)
{
  int i,max;

  max = APVS->n;
  if ( start < 0 || start >= max )
    return;
  if ( start + n > max )
    n = max - start;
  for ( i = 0; i < n; i++ )
    APVS->va[start+i].priv = 0;
}

void Ppause(LIST *rp)
{
  char buf[BUFSIZ];

  fgets(buf,BUFSIZ,stdin);
  *rp = 0;
}

void Pgc(LIST *rp)
{
  GC_gcollect();
  *rp = 0;
}

int exec_file(char *,char *);

void Pbatch(NODE arg,Z *rp)
{
  int ret;

  ret = exec_file(BDY((STRING)ARG0(arg)),BDY((STRING)ARG1(arg)));
  STOZ(ret,*rp);  
}

#if !defined(VISUAL) && defined(DO_PLOT)
int init_display();

void Pxpause(Z *rp)
{
  if ( !init_display() )
    *rp = 0;
  else {
    grab_pointer(); *rp = ONE;
  }
}

static Display *display;
static Window rootwin;

int init_display()
{
  char *dname;
  unsigned int tmp;
  static int initialized;
  int argc;
  char *argv[1];

  if ( initialized )
    return 1;
  else
    initialized = 1;
  dname = (char *)getenv("DISPLAY");

  display = XOpenDisplay(dname);
  if ( !display ) {
    fprintf(stderr,"Can't open display\n");
    return 0;
  }
  rootwin = RootWindow(display,DefaultScreen(display));
  return 1;
}

void grab_pointer()
{
  XEvent ev;
  static Cursor cursor;

  if ( !cursor )
    cursor = XCreateFontCursor(display,XC_leftbutton);
  XGrabPointer(display,rootwin,True,ButtonPressMask,GrabModeAsync,GrabModeAsync,None,cursor,CurrentTime);
  while ( 1 ) {
    XNextEvent(display,&ev);
    if ( ev.xany.type == ButtonPress )
      break;
  }
  XUngrabPointer(display,CurrentTime);
  XSync(display,False);
  return;
}
#endif

void Psend_progress(NODE arg,Z *rp)
{
#if defined(VISUAL)
  short per;
  char *msg;

  per = (short)ZTOS((Q)BDY(arg)); arg = NEXT(arg);
  if ( arg )
    msg = BDY((STRING)BDY(arg));
  else
    msg = "";
  send_progress(per,msg);
#endif
  *rp = 0;
}

void Pget_addr(NODE arg,Z *rp)
{
  pointer obj;

  obj = ARG0(arg);
  UTOZ((unsigned long)obj,*rp);
}

unsigned char *qtoaddr(Z q)
{
  if ( !q )
    return 0;
  else
    return (unsigned char *)ZTOS(q);
}

void Phex_dump(NODE arg,Z *rp)
{
  unsigned char *start;
  int len,i;

  *rp = 0;
  start = qtoaddr((Z)ARG0(arg));
  len = ZTOS((Q)ARG1(arg));
  for ( i = 0; i < len; i++ ) {
    if ( !(i%16) )
      fprintf(asir_out,"%016lx: ",(unsigned long)(start+i));
    fprintf(asir_out,"%02x",start[i]);
    if ( !((i+1)%16) )
      fprintf(asir_out,"\n");
    else if ( !((i+1)%4) )
      fprintf(asir_out," ");
  }
  if ( i%16 )
    fprintf(asir_out,"\n");
}

void Ppeek(NODE arg,Z *rp)
{
  unsigned long b;
  unsigned char *a;

  a = qtoaddr((Z)ARG0(arg));
  b = (unsigned long) (*a);
  UTOZ(b,*rp);
}

void Ppoke(NODE arg,Z *rp)
{
  unsigned char *addr;

  addr = qtoaddr((Z)ARG0(arg));
  *addr = (unsigned char)ZTOS((Q)ARG1(arg));
  *rp = 0;
}

#if 0
static int optimize;
static struct oN oPSN[1000];
static struct oQ oPSZ[1000],oMSZ[1000];
static szinit = 0;

void Popt(NODE arg,Z *rp)
{
  optimize = ARG0(arg) ? 1 : 0; *rp = 0;
}


void sz_init() {
  int i;
  Q t;

  for ( i = 1; i < 1000; i++ ) {
    oPSN[i].p = 1; oPSN[i].b[0] = i;
    t = &oPSZ[i];
    OID(t) = O_N; NID(t) = N_Q; SGN(t) = 1; NM(t) = &oPSN[i]; DN(t) = 0;
    t = &oMSZ[i];
    OID(t) = O_N; NID(t) = N_Q; SGN(t) = -1; NM(t) = &oPSN[i]; DN(t) = 0;
  }
  szinit = 1;
}

optobj(Obj *p)
{
  Obj t;
  int n;
  DCP dc;

  if ( t = *p )
    switch ( OID(t) ) {
      case O_N:
        if ( (NID(t)==N_Q) && INT(t) && (PL(NM((Q)t))==1) ) {
          n = ZTOS((Q)t);
          if ( !szinit )
            sz_init();
          if ( n < 1000 )
            *p = (Obj)(SGN((Q)t)>0?&oPSZ[n]:&oMSZ[n]);
        }
        break;
      case O_P:
        for ( dc = DC((P)t); dc; dc = NEXT(dc) ) {
          optobj(&DEG(dc)); optobj(&COEF(dc));
        }
        break;
      case O_R:
        optobj(&NM((R)t)); optobj(&DN((R)t)); break;
      default:
        break;
    }
}
#endif
