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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/xdeb.c,v 1.12 2004/02/29 08:30:31 noro Exp $ 
*/
#if defined(VISUAL)
#if defined(VISUAL_LIB)
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <signal.h>
#include <process.h>

#define FROMASIR_EXIT 0
#define FROMASIR_TEXT 1
#define FROMASIR_HEAPSIZE 2
#define FROMASIR_SHOW 3
#define FROMASIR_HIDE 4

void show_debug_window(int);
int create_debug_gui();
void set_debug_handles(int);
void get_rootdir(char *,int);

void show_debug_window(int on)
{
	static int debug_gui_invoked = 0;

	/* debug_gui_invoked < 0 : failed to invoke debug GUI */
	if ( debug_gui_invoked < 0 )
		return;
	if ( !debug_gui_invoked ) {
		if ( !on )
			return;
		else if ( create_debug_gui() < 0 ) {
			debug_gui_invoked = -1;
			return;
		}
		debug_gui_invoked = 1;
	}
	set_debug_handles(on);
}

#else /* VISUAL_LIB */
void show_debug_window(int on)
{}
#endif
#else /* VISUAL */
#if defined(DO_PLOT)
/*
 *	xdeb.c --- Asir Debug interface.
 *
 *		akashi@sy.ssl.fujitsu.co.jp
 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Dialog.h>
#include	<stdio.h>

static XrmOptionDescRec options[] = {
	{"-reverse","*reverse",XrmoptionNoArg,"on"},
	{"-fg","*foreground",XrmoptionSepArg,NULL},
	{"-bg","*background",XrmoptionSepArg,NULL},
};

static String fallback[] = {
	"*Dialog*baseTranslations:#override <Key>Return: done()",
	NULL
};

static void Done();
static void Quit();
static void SetSelected();
static void SetDismiss();

static XtActionsRec actions_table[] = {
	{"done",Done},
	{"quit",Quit},
};

char debug_cmd[BUFSIZ];

static Widget		toplevel, mainwin, cmdwin;
static XtAppContext	app_con;
static int done;
static Display *display;

void get_cmd();

static void Done(Widget w,XEvent *e,String *p,Cardinal *n)
{
	SetSelected(w,0,0);
}

static void SetSelected(Widget w,XtPointer cld,XtPointer cad)
{
	Arg		arg[5];
	char *cmd;

	done = 1;
	cmd = XawDialogGetValueString(cmdwin);
	strcpy(debug_cmd,cmd);
	XtSetArg(arg[0], XtNvalue, "");
	XtSetValues(cmdwin, arg, 1);
}

static void SetDismiss(Widget w,XtPointer cld,XtPointer cad)
{
	Arg		arg[5];
	char *cmd;

	done = 1;
	cmd = XawDialogGetValueString(cmdwin);
	strcpy(debug_cmd,"quit");
	XtSetArg(arg[0], XtNvalue, "");
	XtSetValues(cmdwin, arg, 1);
}

static Atom wm_delete_window;
static void Quit(Widget w, XEvent *ev, String *params,Cardinal *nparams)
{
	XBell(XtDisplay(w),0);
}

void init_cmdwin()
{
	Arg		arg[5];
	int argc=0;
	char *argv[1];
	int n;
	char *d;
	char hostname[BUFSIZ],title[BUFSIZ];
	extern char displayname[];
	extern int do_server_in_X11;
	static int is_init = 0;

	if ( is_init )
		return;
	if ( !displayname[0] ) {
		d = (char *)getenv("DISPLAY");
		if ( d )
			strcpy(displayname,d);
		else
			do_server_in_X11 = 0;
	}
	gethostname(hostname, BUFSIZ);
	sprintf(title,"Asir debugger:%s",hostname);
	if ( displayname[0] ) {
		argv[0] = 0;
		XtToolkitInitialize();
		app_con = XtCreateApplicationContext();
		XtAppAddActions(app_con,actions_table, XtNumber(actions_table));
		XtAppSetFallbackResources(app_con,fallback);
		display = XtOpenDisplay(app_con,displayname,title,title,
			options,XtNumber(options),&argc,argv);
		toplevel = XtAppCreateShell(0,title,applicationShellWidgetClass,
			display,0,0);
		n = 0;
		wm_delete_window = XInternAtom(XtDisplay(toplevel), "WM_DELETE_WINDOW",
			False);

		XtSetArg(arg[n],XtNiconName,title); n++;
		XtSetArg(arg[n], XtNwidth, 360); n++;
		mainwin = XtCreatePopupShell("shell",topLevelShellWidgetClass,
			toplevel,arg,n);
		XtOverrideTranslations(mainwin, 
					XtParseTranslationTable("<Message>WM_PROTOCOLS: quit()"));
	/*
	 *	Command line
	 */
		cmdwin = XtCreateManagedWidget("cmdwin", dialogWidgetClass,
			mainwin, NULL, 0);
		XawDialogAddButton(cmdwin,"Execute",SetSelected,"Execute");
		XawDialogAddButton(cmdwin,"Dismiss",SetDismiss,"Dismiss");
		n = 0;
		XtSetArg(arg[n], XtNlabel, "Command : "); n++;
		XtSetArg(arg[n], XtNvalue, ""); n++;
		XtSetValues(cmdwin, arg, n);
		XtRealizeWidget(mainwin);
		XSetWMProtocols(XtDisplay(mainwin),XtWindow(mainwin),
			&wm_delete_window,1);
	}
	is_init = 1;
}

void show_debug_window(int on)
{
	XEvent ev;

	init_cmdwin();
	if ( mainwin ) {
		if ( on )
			XtPopup(mainwin,XtGrabNone);
		else
			XtPopdown(mainwin);
		XFlush(display);
		while ( XtAppPending(app_con) ) {
			XtAppNextEvent(app_con,&ev);
			XtDispatchEvent(&ev);
		}
	}
}

void get_line(char *buf)
{
	XEvent ev;

	done = 0;
	while ( !done ) {
		XtAppNextEvent(app_con,&ev);
		XtDispatchEvent(&ev);
	}
	strcpy(buf,debug_cmd);
}
#else /* DO_PLOT */
void init_cmdwin() {}

void show_debug_window(int on)
{}

void get_line(buf)
char *buf;
{}
#endif /* DO_PLOT */
#endif /* VISUAL */
