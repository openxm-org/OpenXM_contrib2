/* $OpenXM: OpenXM_contrib2/asir2000/parse/xdeb.c,v 1.1.1.1 1999/12/03 07:39:12 noro Exp $ */
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
#if DO_PLOT
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
static void SetSelected();

static XtActionsRec actions_table[] = {
	{"done",Done},
};

char debug_cmd[BUFSIZ];

static Widget		toplevel, mainwin, cmdwin;
static XtAppContext	app_con;
static int done;
static Display *display;

void get_cmd();

static void Done(w,e,p,n)
Widget w;
XEvent *e;
String *p;
Cardinal *n;
{
	SetSelected(w,0,0);
}

static void SetSelected(w,cld,cad)
Widget w;
XtPointer cld,cad;
{
	Arg		arg[5];
	char *cmd;

	done = 1;
	cmd = XawDialogGetValueString(cmdwin);
	strcpy(debug_cmd,cmd);
	XtSetArg(arg[0], XtNvalue, "");
	XtSetValues(cmdwin, arg, 1);
}

init_cmdwin()
{
	Arg		arg[5];
	int argc=0;
	char *argv[1];
	int n;
	char *d;
	extern char displayname[];
	extern int do_server_in_X11;

	if ( !displayname[0] ) {
		d = (char *)getenv("DISPLAY");
		if ( d )
			strcpy(displayname,d);
		else
			do_server_in_X11 = 0;
	}
	if ( displayname[0] ) {
		argv[0] = 0;
		XtToolkitInitialize();
		app_con = XtCreateApplicationContext();
		XtAppAddActions(app_con,actions_table, XtNumber(actions_table));
		XtAppSetFallbackResources(app_con,fallback);
		display = XtOpenDisplay(app_con,displayname,"ox_asir_debug_window","ox_asir_debug_window",
			options,XtNumber(options),&argc,argv);
		toplevel = XtAppCreateShell(0,"ox_asir_debug_window",applicationShellWidgetClass,
			display,0,0);
		XtSetArg(arg[0],XtNiconName,"ox_asir_debug_window");
		mainwin = XtCreatePopupShell("shell",topLevelShellWidgetClass,
			toplevel,arg,1);
	/*
	 *	Command line
	 */
		cmdwin = XtCreateManagedWidget("cmdwin", dialogWidgetClass,
			mainwin, NULL, 0);
		XawDialogAddButton(cmdwin,"OK",SetSelected,"OK");
		n = 0;
		XtSetArg(arg[n], XtNlabel, "Command : "); n++;
		XtSetArg(arg[n], XtNvalue, ""); n++;
		XtSetValues(cmdwin, arg, n);
		XtRealizeWidget(mainwin);
	}
}

show_debug_window(on)
int on;
{
	XEvent ev;

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

get_line(buf)
char *buf;
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
init_cmdwin() {}

show_debug_window(on)
int on;
{}

get_line(buf)
char *buf;
{}
#endif /* DO_PLOT */
#endif /* VISUAL */
