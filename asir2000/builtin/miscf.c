/* $OpenXM: OpenXM_contrib2/asir2000/builtin/miscf.c,v 1.1.1.1 1999/12/03 07:39:07 noro Exp $ */
#include "ca.h"
#include "parse.h"
#if INET && !defined(VISUAL)
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#endif

void Pquit(), Pdebug(), Pnmono(), Pnez(), Popt(), Pshell(), Pheap();
void Perror(), Perror3(), Pversion(), Pflist(), Pdelete_history(), Ppause(), Pxpause();
void Pr2g(), Pread_cmo(), Pwrite_cmo();
void Pgc(),Pbatch(),Psend_progress();
void Pnull_command();
void Pgetenv();
void Plib_ox_push_cmo(),Plib_ox_pop_cmo(),Plib_ox_push_cmd();
void Plib_ox_execute_string();

void delete_history(int,int);

struct ftab misc_tab[] = {
	{"null_command",Pnull_command,-99999},
	{"getenv",Pgetenv,1},
	{"end",Pquit,0},
	{"quit",Pquit,0},
	{"debug",Pdebug,0},
	{"shell",Pshell,-2},
	{"heap",Pheap,-1},
	{"version",Pversion,0},
	{"nmono",Pnmono,1},
	{"error",Perror,1},
	{"error3",Perror3,3},
	{"nez",Pnez,1},
	{"flist",Pflist,0},
	{"delete_history",Pdelete_history,-2},
	{"pause",Ppause,0},
	{"gc",Pgc,0},
	{"batch",Pbatch,2},
	{"send_progress",Psend_progress,-2},
#if INET && !defined(VISUAL)
	{"xpause",Pxpause,0},
#endif
#if 0
	{"opt",Popt,1},
#endif
/* test functions for library mode ox operations */
	{"lib_ox_push_cmo",Plib_ox_push_cmo,1},
	{"lib_ox_pop_cmo",Plib_ox_pop_cmo,0},
	{"lib_ox_push_cmd",Plib_ox_push_cmd,1},
	{"lib_ox_execute_string",Plib_ox_execute_string,1},
	{0,0,0},
};

extern int little_endian;

int lib_ox_initialized;

void Plib_ox_push_cmo(arg,rp)
NODE arg;
Q *rp;
{
	void *cmo;

	if ( !lib_ox_initialized ) asir_ox_io_init(little_endian);
	risa_to_cmo(ARG0(arg),&cmo);
	asir_ox_push_cmo(cmo);
	*rp = ONE;
}

void Plib_ox_pop_cmo(rp)
Obj *rp;
{
	void *buf;
	int ret,len;

	if ( !lib_ox_initialized ) asir_ox_io_init(little_endian);
	len = asir_ox_peek_cmo_size();
	buf = (void *)MALLOC_ATOMIC(len);
	ret = asir_ox_pop_cmo(buf,len);
	if ( ret < 0 )
		error("lib_ox_pop_cmo : buffer too small (cannot happen!)");
	cmo_to_risa(buf,rp);
}

void Plib_ox_push_cmd(arg,rp)
NODE arg;
Q *rp;
{
	if ( !lib_ox_initialized ) asir_ox_io_init(little_endian);
	asir_ox_push_cmd((unsigned int)QTOS((Q)ARG0(arg)));
	*rp = ONE;
}

void Plib_ox_execute_string(arg,rp)
NODE arg;
Q *rp;
{
	if ( !lib_ox_initialized ) asir_ox_io_init(little_endian);
	asir_ox_execute_string(BDY((STRING)ARG0(arg)));
	*rp = ONE;
}

void Pgetenv(arg,rp)
NODE arg;
STRING *rp;
{
	char *e,*f;
	int len;

	e = (char *)getenv(BDY((STRING)ARG0(arg)));
	if ( e ) {
		len = strlen(e);
		f = (char *)MALLOC_ATOMIC(len+1);
		strcpy(f,e);
		MKSTR(*rp,f);
	} else
		*rp = 0;
}

void Pnull_command(arg,rp)
NODE arg;
Q *rp;
{
	*rp = 0;
}

void Pquit(rp)
pointer *rp;
{
	if ( !NEXT(asir_infile) )
		asir_terminate(2); 
	else {
		closecurrentinput();
		if ( !asir_infile->fp )
			asir_terminate(2);
	}
	*rp = 0;
}

void Pdebug(rp)
pointer *rp;
{
	debug(0); *rp = 0;
}

void Pshell(arg,rp)
NODE arg;
Q *rp;
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
	STOQ(status,*rp);
}

void Pnmono(arg,rp)
NODE arg;
Q *rp;
{
	Obj obj;
	int n;

	obj = (Obj)ARG0(arg);
	if ( !obj || OID(obj) > O_R )
		*rp = 0;
	else
		switch (OID(obj)) {
			case O_N: case O_P:
				n = nmonop((P)obj); STOQ(n,*rp); break;
			case O_R:
				n = nmonop(NM((R)obj)) + nmonop(DN((R)obj));
				STOQ(n,*rp); break;
		}
}

void Pheap(arg,rp)
NODE arg;
Q *rp;
{
	int h0,h;
	void GC_expand_hp(int);
	
	h0 = get_heapsize();
	if ( arg ) {
		h = QTOS((Q)ARG0(arg));
		if ( h > h0 )
			GC_expand_hp(h-h0);
	}
	h = get_heapsize();
	STOQ(h,*rp);
}

unsigned int get_asir_version();

void Pversion(rp)
Q *rp;
{
	unsigned int version;

	version = get_asir_version();
	STOQ(version,*rp);
}

extern int nez;

void Pnez(arg,rp)
NODE arg;
pointer *rp;
{
	nez = ARG0(arg) ? 1 : 0; *rp = 0;
}

void Perror(arg,rp)
NODE arg;
Q *rp;
{
	char *s;

	if ( !arg || !ARG0(arg) || (OID((Obj)ARG0(arg)) != O_STR) )
		s = "";
	else
		s = BDY((STRING)ARG0(arg));
	error(s);
	*rp = 0;
}

void Perror3(arg,rp)
NODE arg;
Q *rp;
{
	char s[BUFSIZ];
	int code;
	char *reason,*action;

	asir_assert(ARG0(arg),O_N,"error3");
	asir_assert(ARG1(arg),O_STR,"error3");
	asir_assert(ARG2(arg),O_STR,"error3");
	code = QTOS((Q)ARG0(arg));
	reason = BDY((STRING)ARG1(arg));
	action = BDY((STRING)ARG2(arg));
	set_error(code,reason,action);
	error("");
	*rp = 0;
}

void Pflist(rp)
LIST *rp;
{
	char *n;
	STRING name;
	NODE t,r,r0;
	LIST l;

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
	MKLIST(l,r0); *rp = l;
}

void Pdelete_history(arg,rp)
NODE arg;
Q *rp;
{
	switch ( argc(arg) ) {
		case 0: default:
			delete_history(0,(int)APVS->n);
			break;
		case 1:
			delete_history(QTOS((Q)ARG0(arg)),1);
			break;
	}
	*rp = 0;
}

void delete_history(start,n)
int start,n;
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

void Ppause(rp)
LIST *rp;
{
	char buf[BUFSIZ];

	fgets(buf,BUFSIZ,stdin);
	*rp = 0;
}

void Pgc(rp)
LIST *rp;
{
	GC_gcollect();
	*rp = 0;
}

int exec_file(char *,char *);

void Pbatch(arg,rp)
NODE arg;
Q *rp;
{
	int ret;

	ret = exec_file(BDY((STRING)ARG0(arg)),BDY((STRING)ARG1(arg)));
	STOQ(ret,*rp);	
}

#if INET && !defined(VISUAL)
void Pxpause(rp)
Q *rp;
{
	if ( !init_display() )
		*rp = 0;
	else {
		grab_pointer(); *rp = ONE;
	}
}

static Display *display;
static Window rootwin;

init_display()
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
}

grab_pointer()
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

void Psend_progress(NODE arg,Q *rp)
{
#if defined(VISUAL)
	short per;
	char *msg;

	per = (short)QTOS((Q)BDY(arg)); arg = NEXT(arg);
	if ( arg )
		msg = BDY((STRING)BDY(arg));
	else
		msg = "";
	send_progress(per,msg);
#endif
	*rp = 0;
}

#if 0
static int optimize;
static struct oN oPSN[1000];
static struct oQ oPSZ[1000],oMSZ[1000];
static szinit = 0;

void Popt(arg,rp)
NODE arg;
pointer *rp;
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

optobj(p)
Obj *p;
{
	Obj t;
	int n;
	DCP dc;

	if ( t = *p )
		switch ( OID(t) ) {
			case O_N:
				if ( (NID(t)==N_Q) && INT(t) && (PL(NM((Q)t))==1) ) {
					n = QTOS((Q)t);
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
