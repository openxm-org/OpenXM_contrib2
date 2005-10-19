/*
  main.c

  Windows toplevel and functions for communication, error-handling.
  WinMain() is the common toplevel function for several asir variants.
  'Main()' (the toplevel of the usual asir session) is called from WinMain().  
  Furthermore, a special GUI for debugging is created by create_debug_gui().

  There are two sets of events: 
  	{Notify0, Notify0_Ack, Intr0, Intr0_Ack, Kill}
   and
  	{DebugNotify, DebugNotify_Ack, DebugIntr, DebugIntr_Ack}.
  These are used for the communication between Asir GUI and engine,
  or debug GUI and engine respectively.

   Asir GUI <-- Notify0, Notify0_Ack, Kill --> engine
   debug GUI <-- DebugNotify, DebugNotify_Ack --> engine

  Revision History:

  99/04/27   noro   the first official version
  99/07/27   noro   
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <signal.h>
#include <io.h>
#include <fcntl.h>
#include <process.h>
#include "ca.h"

/* error message structure */

typedef struct ErrMsg {
	int code;
	char reason[BUFSIZ*5]; /* XXX : the size should coincide with that in GUI */
	char action[BUFSIZ*5];
} ErrMsg;

/* main thread id */
DWORD MainThread;

/* pipe handles */
static HANDLE hRead,hWrite;
static HANDLE hRead0,hWrite0;
static HANDLE hDebugRead,hDebugWrite;

/* event handles */
static HANDLE hOxIntr,hOxReset,hOxKill;
static HANDLE hNotify,hNotify_Ack,hIntr,hIntr_Ack,hKill;
static HANDLE hNotify0,hNotify_Ack0,hIntr0;
static HANDLE hDebugNotify,hDebugNotify_Ack,hDebugIntr,hDebugIntr_Ack,hDebugKill;

HANDLE hResizeNotify,hResizeNotify_Ack; /* should be visible from another file */
HANDLE hMainThreadReady,hCanvasCreated; /* notification for ox_plot */

/* XXX */
extern HANDLE hStreamNotify,hStreamNotify_Ack; /* declared in io/ox.c */

/* process handle */
static HANDLE hDebugProc,hMessageProc;
static HANDLE hThread,hWatchIntrThread,hWatchStreamThread,hComputingThread;

static struct ErrMsg Errmsg;

/*
 * recv_intr : flag for Asir
 * interrupt_state : flag to distinguish an Asir error and a cancellation
 */

int emergency;
static int interrupt_state;
int debuggui_is_present;
int messagegui_is_present;

extern int recv_intr,doing_batch;
extern int ox_sock_id, do_message;

void get_rootdir(char *,int);
void ox_watch_stream();
void ox_plot_main();

void send_intr();
void SendCmdSize(unsigned int,unsigned int);
void SendBody(void *buf,unsigned int);
void SendHeapSize();
void Main(int,char **);
void AsirMain(int,char **);
void OxAsirMain(int,char **);
void OxPlotMain(int,char **);

void Init_Asir(int,char **);
int Call_Asir(char *,void *);
void int_handler(int);
void set_debug_handles(int);
int create_debug_gui();
void terminate_debug_gui();

void _setargv();

#define FROMASIR_EXIT 0
#define FROMASIR_TEXT 1
#define FROMASIR_HEAPSIZE 2
#define FROMASIR_SHOW 3
#define FROMASIR_HIDE 4

static char Asir_Cmd[BUFSIZ*8];

extern char LastError[];

/* the watch threads */

void watch_intr() {
	HANDLE handle[2];
	DWORD ret;

	handle[0] = hIntr0; handle[1] = hKill;
	while ( 1 ) {
		ret = WaitForMultipleObjects(2,(CONST HANDLE *)handle,FALSE,(DWORD)-1);
		switch ( ret ) {
			case WAIT_OBJECT_0: /* hIntr0 */
				if ( doing_batch )
					send_intr();
				/* for Asir; recv_intr is reset to 0 in Asir */
				recv_intr = 1;
				break;
			case WAIT_OBJECT_0+1: /* hKill */
			default:
				terminate_debug_gui();
				emergency = 1; /* XXX */
				asir_terminate(2);
				exit(0);
				/* NOTREACHED */
				break;
		}
	}
}

void ox_watch_intr() {
	HANDLE handle[3];
	DWORD ret;

	handle[0] = hOxIntr; handle[1] = hOxReset; handle[2] = hOxKill;
	while ( 1 ) {
		ret = WaitForMultipleObjects(3,(CONST HANDLE *)handle,FALSE,(DWORD)-1);
		switch ( ret ) {
			case WAIT_OBJECT_0: /* hOxIntr */
				ResetEvent(hOxIntr);
				if ( doing_batch )
					send_intr();
				recv_intr = 1;
				break;
			case WAIT_OBJECT_0+1: /* hOxReset */
				ResetEvent(hOxReset);
				if ( doing_batch )
					send_intr();
				recv_intr = 2;
				break;
			case WAIT_OBJECT_0+2: /* hOxKill */
				ResetEvent(hOxKill);
				terminate_debug_gui();
				emergency = 1; /* XXX */
				asir_terminate(2);
				/* NOTREACHED */
				break;
		}
	}
}

/*
   setup the necessary events for communication between debug GUI and engine.
   Then the debug GUI is invoked.
*/

int create_message_gui()
{
	DWORD mypid,len;
	HANDLE hR0,hW0,hR1,hW1;
    SECURITY_ATTRIBUTES SecurityAttributes;
	char remread[10],remwrite[10];
	char notify[100],notify_ack[100];
	char name[100];
	char DebugGUI[100];
	char *av[100];

	mypid = GetCurrentProcessId();
	sprintf(notify,"message_notify_%d",mypid);
	sprintf(notify_ack,"message_notify_ack_%d",mypid);
	hNotify = hNotify0 = CreateEvent(NULL,TRUE,FALSE,notify);
	hNotify_Ack = hNotify_Ack0 = CreateEvent(NULL,TRUE,FALSE,notify_ack);
	
    SecurityAttributes.nLength = sizeof(SecurityAttributes);
    SecurityAttributes.lpSecurityDescriptor = NULL;
    SecurityAttributes.bInheritHandle = TRUE;
    CreatePipe(&hR0, &hW0, &SecurityAttributes, 65536);
    CreatePipe(&hR1, &hW1, &SecurityAttributes, 65536);

	hRead = hRead0 = hR0;
	hWrite = hWrite0 = hW1;
	sprintf(remread,"%d",(DWORD)hR1);
	sprintf(remwrite,"%d",(DWORD)hW0);
	len = sizeof(name);
	get_rootdir(name,len);
	sprintf(DebugGUI,"%s\\bin\\asirgui.exe",name);
	av[0] = "messagegui";
	av[1] = remread;
	av[2] = remwrite;
	av[3] = notify;
	av[4] = notify_ack;
	av[5] = NULL;
	hMessageProc = (HANDLE)_spawnv(_P_NOWAIT,DebugGUI,av);
	if ( hMessageProc == (HANDLE)-1 ) {
		fprintf(stderr,"%s not found",DebugGUI);
		messagegui_is_present = 0;
		return -1;
	} else
		messagegui_is_present = 1;
	return 0;
}

/*
   setup the necessary events for communication between debug GUI and engine.
   Then the debug GUI is invoked.
*/

int create_debug_gui()
{
	DWORD mypid,len;
	HANDLE hR0,hW0,hR1,hW1;
    SECURITY_ATTRIBUTES SecurityAttributes;
	char remread[10],remwrite[10];
	char notify[100],notify_ack[100],intr[100],intr_ack[100],kill[100];
	char name[100];
	char DebugGUI[100];
	char *av[100];

	mypid = GetCurrentProcessId();
	sprintf(notify,"debug_notify_%d",mypid);
	sprintf(notify_ack,"debug_notify_ack_%d",mypid);
	sprintf(intr,"debug_intr_%d",mypid);
	sprintf(intr_ack,"debug_intr_ack_%d",mypid);
	sprintf(kill,"debug_kill_%d",mypid);
	hDebugNotify = CreateEvent(NULL,TRUE,FALSE,notify);
	hDebugNotify_Ack = CreateEvent(NULL,TRUE,FALSE,notify_ack);
	hDebugIntr = CreateEvent(NULL,TRUE,FALSE,intr);
	hDebugIntr_Ack = CreateEvent(NULL,TRUE,FALSE,intr_ack);
	hDebugKill = CreateEvent(NULL,TRUE,FALSE,kill);
	
    SecurityAttributes.nLength = sizeof(SecurityAttributes);
    SecurityAttributes.lpSecurityDescriptor = NULL;
    SecurityAttributes.bInheritHandle = TRUE;
    CreatePipe(&hR0, &hW0, &SecurityAttributes, 65536);
    CreatePipe(&hR1, &hW1, &SecurityAttributes, 65536);

	hDebugRead = hR0;
	hDebugWrite = hW1;
	sprintf(remread,"%d",(DWORD)hR1);
	sprintf(remwrite,"%d",(DWORD)hW0);
	len = sizeof(name);
	get_rootdir(name,len);
	sprintf(DebugGUI,"%s\\bin\\asirgui.exe",name);
	av[0] = "debuggui";
	av[1] = remread;
	av[2] = remwrite;
	av[3] = notify;
	av[4] = notify_ack;
	av[5] = intr;
	av[6] = intr_ack;
	av[7] = kill;
	av[8] = NULL;
	hDebugProc = (HANDLE)_spawnv(_P_NOWAIT,DebugGUI,av);
	if ( hDebugProc == (HANDLE)-1 ) {
		fprintf(stderr,"%s not found",DebugGUI);
		return -1;
	}
	return 0;
}

/* if debug GUI is present, we have to terminate the process */

void terminate_debug_gui()
{
	if ( hDebugProc ) {
		TerminateProcess(hDebugProc,0);
//		hRead = hDebugRead; hWrite = hDebugWrite;
//		hNotify = hDebugNotify; hNotify_Ack = hDebugNotify_Ack;
//		hIntr = hDebugIntr;
//		SendCmdSize(FROMASIR_EXIT,0);
		hDebugProc = 0;
	}
	if ( hMessageProc ) {
		TerminateProcess(hMessageProc,0);
//		hRead = hRead0; hWrite = hWrite0;
//		hNotify = hNotify0; hNotify_Ack = hNotify_Ack0;
//		hIntr = hIntr0;
//		SendCmdSize(FROMASIR_EXIT,0);
		hMessageProc = 0;
	}
}

/* set the current active set of events */

void set_debug_handles(int on)
{
	if ( on ) {
		if ( !debuggui_is_present ) {
			hRead = hDebugRead; hWrite = hDebugWrite;
			hNotify = hDebugNotify; hNotify_Ack = hDebugNotify_Ack;
			hIntr = hDebugIntr;
			SendCmdSize(FROMASIR_SHOW,0);
			debuggui_is_present = 1;
		}
	} else {
		if ( debuggui_is_present ) {
			SendCmdSize(FROMASIR_HIDE,0);
			hRead = hRead0; hWrite = hWrite0;
			hNotify = hNotify0; hNotify_Ack = hNotify_Ack0;
			hIntr = hIntr0;
			debuggui_is_present = 0;
		}
	}
}

void Init_IO()
{
	_setargv();
	if ( !strcmp(__argv[0],"ox_asir") ) {
		OxAsirMain(__argc,__argv);
		exit(0);
	} else if ( !strcmp(__argv[0],"ox_plot") ) 
		OxPlotMain(__argc,__argv);
	else if ( !strcmp(__argv[0],"ox_launch") ) {
		launch_main(__argc,__argv);
		exit(0);
	} else {
		AsirMain(__argc,__argv);
		exit(0);
	}
}

void AsirMain(int argc, char **argv)
{
	DWORD tid;

	hRead = (void *)atoi(__argv[1]);
	hWrite = (void *)atoi(__argv[2]);
	hNotify = OpenEvent(EVENT_ALL_ACCESS|EVENT_MODIFY_STATE,TRUE,__argv[3]);
	hNotify_Ack = OpenEvent(EVENT_ALL_ACCESS|EVENT_MODIFY_STATE,TRUE,__argv[4]);
	hIntr = OpenEvent(EVENT_ALL_ACCESS|EVENT_MODIFY_STATE,TRUE,__argv[5]);
	hIntr_Ack = OpenEvent(EVENT_ALL_ACCESS|EVENT_MODIFY_STATE,TRUE,__argv[6]);
	hKill = OpenEvent(EVENT_ALL_ACCESS|EVENT_MODIFY_STATE,TRUE,__argv[7]);
	if ( !hRead || !hWrite || !hNotify || !hNotify_Ack || !hIntr || !hIntr_Ack || !hKill )
		exit(0);
	/* save the above handles */
	hRead0 = hRead; hWrite0 = hWrite;
	hNotify0 = hNotify; hNotify_Ack0 = hNotify_Ack;
	hIntr0 = hIntr;

	hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)watch_intr,NULL,0,&tid); 
	if ( !hThread )
		exit(0);
//	ret = SetThreadPriority(hThread,THREAD_PRIORITY_BELOW_NORMAL);
//	if ( !ret )
//		exit(0);
	/* messagegui is the asirgui main window. */
	messagegui_is_present = 1;
	/* The rest of the args are passed to Main(). */
	/* XXX : process_args() increments argv. */
//	ret = SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);
//	if ( !ret )
//		exit(0);
	Main(__argc-7,__argv+7);
}

void OxAsirMain(int argc, char **argv)
{
	int create_message;
	int tid;

	ox_sock_id = atoi(__argv[1]);
	create_message = atoi(__argv[2]);
	hOxIntr = OpenEvent(EVENT_ALL_ACCESS|EVENT_MODIFY_STATE,TRUE,__argv[3]);
	hOxReset = OpenEvent(EVENT_ALL_ACCESS|EVENT_MODIFY_STATE,TRUE,__argv[4]);
	hOxKill = OpenEvent(EVENT_ALL_ACCESS|EVENT_MODIFY_STATE,TRUE,__argv[5]);
	if ( create_message )
		create_message_gui();
	else
		messagegui_is_present = 0;
	if ( messagegui_is_present )
		SendCmdSize(FROMASIR_SHOW,0);
	hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ox_watch_intr,NULL,0,&tid); 
//	ret = SetThreadPriority(hThread,THREAD_PRIORITY_BELOW_NORMAL);
//	if ( !ret )
//		exit(0);
//	ret = SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);
//	if ( !ret )
//		exit(0);
	/* XXX : process_args() increments argv. */
	ox_main(__argc-5,__argv+5);
}

int plot_argc;
char **plot_argv;

void OxPlotMain(int argc, char **argv)
{
	DWORD tid;
	DWORD mypid;
	char eventname[BUFSIZ];

	ox_sock_id = atoi(argv[1]);
	do_message = atoi(argv[2]);
	hOxIntr = OpenEvent(EVENT_ALL_ACCESS|EVENT_MODIFY_STATE,TRUE,argv[3]);
	hOxReset = OpenEvent(EVENT_ALL_ACCESS|EVENT_MODIFY_STATE,TRUE,argv[4]);
	hOxKill = OpenEvent(EVENT_ALL_ACCESS|EVENT_MODIFY_STATE,TRUE,argv[5]);

	if ( do_message )
		create_message_gui();
	else
		messagegui_is_present = 0;
	if ( messagegui_is_present )
		SendCmdSize(FROMASIR_SHOW,0);

	mypid = GetCurrentProcessId();
	sprintf(eventname,"stream_notify_%d",mypid);
	hStreamNotify = CreateEvent(NULL,TRUE,FALSE,eventname);
	sprintf(eventname,"stream_notify_ack_%d",mypid);
	hStreamNotify_Ack = CreateEvent(NULL,TRUE,FALSE,eventname);
	sprintf(eventname,"resize_notify_%d",mypid);
	hResizeNotify = CreateEvent(NULL,TRUE,FALSE,eventname);
	sprintf(eventname,"resize_notify_ack_%d",mypid);
	hResizeNotify_Ack = CreateEvent(NULL,TRUE,FALSE,eventname);
	sprintf(eventname,"mainthreadready_%d",mypid);
	hMainThreadReady = CreateEvent(NULL,TRUE,FALSE,eventname);
	sprintf(eventname,"canvascreated_%d",mypid);
	hCanvasCreated = CreateEvent(NULL,TRUE,FALSE,eventname);

	hWatchStreamThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ox_watch_stream,NULL,0,&tid); 
//	ret = SetThreadPriority(hWatchStreamThread,THREAD_PRIORITY_BELOW_NORMAL);
//	if ( !ret )
//		exit(0);
	hWatchIntrThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ox_watch_intr,NULL,0,&tid); 
//	ret = SetThreadPriority(hWatchIntrThread,THREAD_PRIORITY_BELOW_NORMAL);
//	if ( !ret )
//		exit(0);

	/* process_args() increments argv */
	plot_argc = argc-5;
	plot_argv = argv+5;
	hComputingThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ox_plot_main,NULL,0,&tid);
//	ret = SetThreadPriority(hComputingThread,THREAD_PRIORITY_ABOVE_NORMAL);
//	if ( !ret )
//		exit(0);
}

void ox_watch_stream() {
	fd_set r,w,e;

	/* wait for the completion of initalization in the computing thread */
	WaitForSingleObject(hStreamNotify_Ack,(DWORD)-1);
	ResetEvent(hStreamNotify_Ack);
	while ( 1 ) {
		FD_ZERO(&r);
		FD_ZERO(&w);
		FD_ZERO(&e);
		FD_SET(ox_sock_id,&r);
		select(FD_SETSIZE,&r,&w,&e,NULL);
		SetEvent(hStreamNotify);
		WaitForSingleObject(hStreamNotify_Ack,(DWORD)-1);
		ResetEvent(hStreamNotify_Ack);
	}
}

/* get_line : used in Asir mode */

int get_line(char *buf) {
	DWORD len;
	int size;

	ReadFile(hRead,&size,sizeof(int),&len,NULL);	
	ReadFile(hRead,buf,size,&len,NULL);	
	buf[size] = 0;
	return 0;
}

/* put_line : used in Asir mode */

void put_line(char *buf) {
	if ( debuggui_is_present || messagegui_is_present ) {
		int size = strlen(buf);

		if ( size ) {
			SendCmdSize(FROMASIR_TEXT,size);
			SendBody(buf,size);
		}
	}
}

/* common function for error exit */

void ExitAsir() {
	terminate_debug_gui();
	/* if emergency != 0, asirgui may not exist */
	if ( !emergency )
		SendCmdSize(FROMASIR_EXIT,0);
	ExitProcess(0);
}

/* SendHeapSize : used in Asir mode */

int get_heapsize();

void SendHeapSize()
{
	if ( messagegui_is_present || debuggui_is_present ) {
		int h = get_heapsize();
		SendCmdSize(FROMASIR_HEAPSIZE,sizeof(int));
		SendBody(&h,sizeof(int));
	}
}

/* SendCmdSize : used in Asir mode */

void SendCmdSize(unsigned int cmd,unsigned int size)
{
	DWORD len;
	unsigned int cmdsize;

	if ( hNotify ) {
		SetEvent(hNotify);
		WaitForSingleObject(hNotify_Ack,(DWORD)-1);
		ResetEvent(hNotify_Ack);
		cmdsize = (cmd<<16)|size;
		WriteFile(hWrite,&cmdsize,sizeof(unsigned int),&len,NULL);
	}
}

/* SendBody : used in Asir mode */

void SendBody(void *buf,unsigned int size)
{

	DWORD len;

	WriteFile(hWrite,buf,size,&len,NULL);
}

/* send header + send progress data */

void send_progress(short percentage,char *message)
{
}

/* set error code, message, action in Errmsg. */

void set_error(int id,char *reason,char *action)
{
}

/* dummy functions */
void reset_current_computation(){}
void set_selection(){}
void reset_selection(){}
void set_busy(){}
void reset_busy(){}
