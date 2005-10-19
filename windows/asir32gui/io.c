#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <process.h>

#include "asir32gui_io.h"

#define ECGEN_KEYNAME "SoftWare\\Fujitsu\\FSEcParamGen\\V1.0L10"
#define ASIR_KEYNAME "SoftWare\\Fujitsu\\Asir\\1999.03.31"

#define MAXHIST 256
#define HISTORY "history"

static int use_current_dir;

int asirgui_kind;
HANDLE hProc;
static HANDLE hRead,hWrite;
HANDLE hNotify,hNotify_Ack;
static HANDLE hIntr,hIntr_Ack,hKill;

void _setargv(void);
void init_input_history();
void write_input_history();
void read_input_history();
void init_hist(int);
void read_hist(char *);
void write_hist(char *);
void show_window(int);

BOOL get_rootdir(name,len,errmsg)
char *name;
int len;
char *errmsg;
{
	LONG	ret;
	HKEY	hOpenKey;
	DWORD	Type,dw;
	char	dir[BUFSIZ],message[BUFSIZ];
	char	*slash;
	int		ldir;	
	static  char rootdir[BUFSIZ];
	static  int rootdir_is_initialized;

	if ( rootdir_is_initialized ) {
		strcpy(name,rootdir);
		return TRUE;
	}

	GetCurrentDirectory(BUFSIZ,dir);
	slash = strrchr(dir,'\\');
	if ( slash )
		*slash = 0;
	ldir = strlen(dir)+1;
	if ( 1 || access("UseCurrentDir",0) >= 0 ) {
		use_current_dir = 1;
		strcpy(rootdir,dir);
		strcpy(name,dir);
		rootdir_is_initialized = 1;
		return TRUE;
	}
	ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, ECGEN_KEYNAME, 0,
		KEY_QUERY_VALUE, &hOpenKey);
	if ( ret != ERROR_SUCCESS )
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, ASIR_KEYNAME, 0,
			KEY_QUERY_VALUE, &hOpenKey);
	if( ret != ERROR_SUCCESS ) {
		sprintf(message,"May I register '%s' as the ASIR main directory?",dir);
		if( IDNO == MessageBox(NULL,message,"Asir",
			MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON2) ) {
			sprintf(errmsg,"The ASIR main directory was not set.");
			return FALSE;
		}
		if ( ret = RegCreateKeyEx(HKEY_LOCAL_MACHINE,ASIR_KEYNAME,0,
			REG_NONE,REG_OPTION_NON_VOLATILE,KEY_READ|KEY_WRITE,NULL,&hOpenKey,&dw) ) {
			FormatMessage(
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				ret,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPSTR)errmsg,
				BUFSIZ,
				NULL);
			return FALSE;
		}
		if ( ret = RegSetValueEx(hOpenKey,"Directory",NULL,REG_SZ,dir,ldir) ) {
			FormatMessage(
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				ret,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPSTR)errmsg,
				BUFSIZ,
				NULL);
			return FALSE;
		}
	}
	RegQueryValueEx(hOpenKey, "Directory", NULL, &Type, name, &len);
	RegCloseKey(hOpenKey);
	strcpy(rootdir,name);
	rootdir_is_initialized = 1;
	return TRUE;
}

BOOL Init_IO(char *errmsg) {
	int i;
	char *av[BUFSIZ];
	char AsirExe[BUFSIZ];
	DWORD mypid,len;
	HANDLE hR0,hW0,hR1,hW1;
    SECURITY_ATTRIBUTES SecurityAttributes;
	char remread[10],remwrite[10];
	char notify[100],notify_ack[100],intr[100],intr_ack[100],kill[100];
	char name[BUFSIZ],qname[BUFSIZ],home[BUFSIZ];

	_setargv();
	if ( !strcmp(__argv[0],"debuggui") ) {
		asirgui_kind = ASIRGUI_DEBUG;
		hRead = atoi(__argv[1]);
		hWrite = atoi(__argv[2]);
		hNotify = OpenEvent(EVENT_ALL_ACCESS|EVENT_MODIFY_STATE,TRUE,__argv[3]);
		hNotify_Ack = OpenEvent(EVENT_ALL_ACCESS|EVENT_MODIFY_STATE,TRUE,__argv[4]);
		hIntr = OpenEvent(EVENT_ALL_ACCESS|EVENT_MODIFY_STATE,TRUE,__argv[5]);
		hIntr_Ack = OpenEvent(EVENT_ALL_ACCESS|EVENT_MODIFY_STATE,TRUE,__argv[6]);
		hKill = OpenEvent(EVENT_ALL_ACCESS|EVENT_MODIFY_STATE,TRUE,__argv[7]);
		return TRUE;
	} else if ( !strcmp(__argv[0],"messagegui") ) {
		asirgui_kind = ASIRGUI_MESSAGE;
		hRead = atoi(__argv[1]);
		hWrite = atoi(__argv[2]);
		hNotify = OpenEvent(EVENT_ALL_ACCESS|EVENT_MODIFY_STATE,TRUE,__argv[3]);
		hNotify_Ack = OpenEvent(EVENT_ALL_ACCESS|EVENT_MODIFY_STATE,TRUE,__argv[4]);
		return TRUE;
	} else {
		asirgui_kind = ASIRGUI_MAIN;
		mypid = GetCurrentProcessId();
		sprintf(notify,"asir_notify_%d",mypid);
		sprintf(notify_ack,"asir_notify_ack_%d",mypid);
		sprintf(intr,"asir_intr_%d",mypid);
		sprintf(intr_ack,"asir_intr_ack_%d",mypid);
		sprintf(kill,"asir_kill_%d",mypid);
		hNotify = CreateEvent(NULL,TRUE,FALSE,notify);
		hNotify_Ack = CreateEvent(NULL,TRUE,FALSE,notify_ack);
		hIntr = CreateEvent(NULL,TRUE,FALSE,intr);
		hIntr_Ack = CreateEvent(NULL,TRUE,FALSE,intr_ack);
		hKill = CreateEvent(NULL,TRUE,FALSE,kill);
		
    	SecurityAttributes.nLength = sizeof(SecurityAttributes);
    	SecurityAttributes.lpSecurityDescriptor = NULL;
    	SecurityAttributes.bInheritHandle = TRUE;
    	CreatePipe(&hR0, &hW0, &SecurityAttributes, 65536);
    	CreatePipe(&hR1, &hW1, &SecurityAttributes, 65536);
	
		hRead = hR0;
		hWrite = hW1;
		sprintf(remread,"%d",(DWORD)hR1);
		sprintf(remwrite,"%d",(DWORD)hW0);
	
		len = sizeof(name);
		if ( get_rootdir(name,len,errmsg) == FALSE ) {
			return FALSE;
		}
//		sprintf(AsirExe,"%s\\bin\\engine.exe",name);
//		sprintf(AsirExe,"c:\\home\\noro\\engine2000\\debug\\engine.exe");
		sprintf(AsirExe,"d:\\cygwin\\home\\noro\\OpenXM_contrib2\\\\windows\\engine2000\\debug\\engine.exe");
		if ( !getenv("HOME") ) {
			sprintf(home,"HOME=%s",name);
			_putenv(home);
		}
		av[0] = "asir";
		av[1] = remread;
		av[2] = remwrite;
		av[3] = notify;
		av[4] = notify_ack;
		av[5] = intr;
		av[6] = intr_ack;
		av[7] = kill;
		for ( i = 1; i < __argc; i++ )
			av[i+7] = __argv[i];
		if ( use_current_dir )  {
			av[i+7] = "-rootdir"; i++;
			sprintf(qname,"\"%s\"",name);
			av[i+7] = qname; i++;
		}
		av[i+7] = NULL;
		hProc = (HANDLE)_spawnv(_P_NOWAIT,AsirExe,av);
		if ( hProc == (HANDLE)-1 ) {
			sprintf(errmsg,"%s is not found.",AsirExe);
			return FALSE;
		}
		return TRUE;
	}
}

void put_line(char *buf) {
	DWORD len;
	int size = strlen(buf);
	buf[size] = '\n';
	buf[size+1] = 0;
	size++;
	WriteFile(hWrite,&size,sizeof(int),&len,NULL);
	WriteFile(hWrite,buf,size,&len,NULL);
}

#define FROMASIR_EXIT 0
#define FROMASIR_TEXT 1
#define FROMASIR_HEAPSIZE 2
#define FROMASIR_SHOW 3
#define FROMASIR_HIDE 4
#define FROMASIR_DEBUGPROC 5

static char insert_buf[BUFSIZ*10];
static int insert_buf_len = 0;

int AsirHeapSize;

void insert_to_theView(char *);
void flush_log();

void read_and_insert()
{
	DWORD len;
	unsigned int cmdsize,cmd,size;

	ReadFile(hRead,&cmdsize,sizeof(unsigned int),&len,NULL);	
	cmd = cmdsize>>16;
	size = cmdsize&0xffff;

	switch ( cmd ) {
		case FROMASIR_EXIT:
			write_input_history();
			flush_log();
			PostMessage(NULL,WM_QUIT,0,0);
			break;
		case FROMASIR_TEXT:
			if ( size ) {
				ReadFile(hRead,insert_buf,size,&len,NULL);	
				insert_buf[size] = 0;
				insert_to_theView(insert_buf);
			}
			break;
		case FROMASIR_HEAPSIZE:
			if ( size )
				ReadFile(hRead,&AsirHeapSize,size,&len,NULL);
			break;
		case FROMASIR_SHOW:
			show_window(1);
			break;
		case FROMASIR_HIDE:
			show_window(0);
			break;
		default:
			if ( size )
				ReadFile(hRead,insert_buf,size,&len,NULL);	
			break;
	}
}

void init_input_history()
{
	init_hist(MAXHIST);
}

void write_input_history()
{
	write_hist(HISTORY);
}

void read_input_history()
{
	read_hist(HISTORY);
}

void send_intr() {
	PulseEvent(hIntr);
}

void terminate_asir() {
	PulseEvent(hKill);
}
