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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/miscf.c,v 1.17 2003/04/02 06:48:19 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#if !defined(VISUAL) && defined(DO_PLOT)
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#endif

#if defined(VISUAL)
#include <stdlib.h>
#include <windows.h>
#endif

void Pquit(), Pdebug(), Pnmono(), Pnez(), Popt(), Pshell(), Pheap();
void Perror(), Perror3(), Pversion(), Pflist(), Pdelete_history(), Ppause(), Pxpause();
void Pr2g(), Pread_cmo(), Pwrite_cmo();
void Pgc(),Pbatch(),Psend_progress();
void Pnull_command();
void Pgetenv();
void Pget_addr(),Phex_dump();
void Ppeek(),Ppoke();
void Psleep();
void Premove_module();
void Pmodule_list();

void delete_history(int,int);

struct ftab misc_tab[] = {
	{"module_list",Pmodule_list,0},
	{"remove_module",Premove_module,1},
	{"sleep",Psleep,1},
	{"null_command",Pnull_command,-99999},
	{"getenv",Pgetenv,1},
	{"end",Pquit,0},
	{"quit",Pquit,0},
	{"debug",Pdebug,0},
	{"shell",Pshell,-2},
	{"heap",Pheap,-1},
	{"version",Pversion,-99999},
	{"nmono",Pnmono,1},
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

void Psleep(arg,rp)
NODE arg;
Q *rp;
{
	int ms;

	ms = QTOS((Q)ARG0(arg));
#if defined(VISUAL)
	Sleep(ms);
#else
	usleep(ms*1000);
#endif
	*rp = ONE;
}

void Pmodule_list(rp)
LIST *rp;
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

void Premove_module(arg,rp)
NODE arg;
Q *rp;
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
		if ( !asir_infile->fp && strcmp(asir_infile->name,"string") )
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
char *get_asir_distribution();

void Pversion(arg,rp)
NODE arg;
Obj *rp;
{
	unsigned int version;
	char *distribution;
	Q q;
	STRING str;
	NODE n;
	LIST l;

	version = get_asir_version();
	distribution = get_asir_distribution();
	UTOQ(version,q);
	if ( !argc(arg) )
		*rp = (Obj)q;
	else {
		MKSTR(str,distribution);	
		n = mknode(2,q,str);
		MKLIST(l,n);
		*rp = (Obj)l;
	}
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
	int code;
	char *reason,*action;

	asir_assert(ARG0(arg),O_N,"error3");
	asir_assert(ARG1(arg),O_STR,"error3");
	asir_assert(ARG2(arg),O_STR,"error3");
	code = QTOS((Q)ARG0(arg));
	reason = BDY((STRING)ARG1(arg));
	action = BDY((STRING)ARG2(arg));
#if defined(VISUAL)
	set_error(code,reason,action);
#endif
	error("");
	*rp = 0;
}

void Pflist(arg,rp)
NODE arg;
LIST *rp;
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

#if !defined(VISUAL) && defined(DO_PLOT)
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

void Pget_addr(arg,rp)
NODE arg;
Q *rp;
{
	pointer obj;
	unsigned int u,l;
	N n;

	obj = ARG0(arg);
	if ( sizeof(pointer) == sizeof(unsigned int) ) {
		UTOQ((unsigned int)obj,*rp);
	} else {
		/* a pointer must fit in long */
		u = ((unsigned long)obj)>>32;
		l = ((unsigned long)obj)&(unsigned long)0xffffffff;
		if ( u ) {
			n = NALLOC(2); PL(n) = 2; BD(n)[0] = l; BD(n)[1] = u;
			NTOQ(n,1,*rp);
		} else {
			UTOQ(l,*rp);
		}
	}
}

unsigned char *qtoaddr(q)
Q q;
{
	unsigned char *addr;
	N n;

	if ( !q )
		return 0;
	n = NM(q);
	if ( (sizeof(pointer) == sizeof(unsigned int)) || (PL(n) == 1) )
		addr = (char *)BD(n)[0];
	else {
		/* a pointer must fit in long */
		addr = (char *)((((unsigned long)BD(n)[1])<<32) 
			| ((unsigned long)BD(n)[0]));
	}
	return addr;
}

void Phex_dump(arg,rp)
NODE arg;
Q *rp;
{
	unsigned char *start;
	int len,i;

	*rp = 0;
	start = qtoaddr((Q)ARG0(arg));
	len = QTOS((Q)ARG1(arg));
	for ( i = 0; i < len; i++ ) {
		if ( !(i%16) )
			fprintf(asir_out,"%08x: ",start+i);
		fprintf(asir_out,"%02x",start[i]);
		if ( !((i+1)%16) )
			fprintf(asir_out,"\n");
		else if ( !((i+1)%4) )
			fprintf(asir_out," ");
	}
	if ( i%16 )
		fprintf(asir_out,"\n");
}

void Ppeek(arg,rp)
NODE arg;
Q *rp;
{
	unsigned int b;
	unsigned char *a;

	a = qtoaddr((Q)ARG0(arg));
	b = (unsigned int) (*a);
	UTOQ(b,*rp);
}

void Ppoke(arg,rp)
NODE arg;
Q *rp;
{
	unsigned char *addr;

	addr = qtoaddr((Q)ARG0(arg));
	*addr = (unsigned char)QTOS((Q)ARG1(arg));
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
