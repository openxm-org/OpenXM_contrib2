/* $OpenXM: OpenXM/src/asir99/builtin/batch.c,v 1.1.1.1 1999/11/10 08:12:25 noro Exp $ */
#if !defined(VISUAL)

#include <stdio.h>
#include <stdlib.h>

int exec_file(char *bindir,char *fname) {
	char command[BUFSIZ*5];
	int status;

	sprintf(command,"%s/asir -f %s",bindir,fname);
	status = system(command);	
	return status;
}

#else

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <process.h>

static void read_until_eof();

static HANDLE hRead,hWrite,hNotify,hNotify_Ack,hIntr,hIntr_Ack,hKill,hProc;

int exec_file(char *bindir,char *fname) {
	int i;
	char *av[BUFSIZ];
	char AsirExe[BUFSIZ];
	DWORD tid,mypid,pid,len;
	HANDLE hR0,hW0,hR1,hW1;
    SECURITY_ATTRIBUTES SecurityAttributes;
	char remread[10],remwrite[10];
	char notify[100],notify_ack[100],intr[100],intr_ack[100],kill[100];

	mypid = GetCurrentProcessId();
	sprintf(notify,"asir_notify_%d",mypid);
	sprintf(notify_ack,"asir_ack_%d",mypid);
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

	sprintf(AsirExe,"%s\\engine.exe",bindir);
	_setargv();
	av[0] = "asir";
	av[1] = remread;
	av[2] = remwrite;
	av[3] = notify;
	av[4] = notify_ack;
	av[5] = intr;
	av[6] = intr_ack;
	av[7] = kill;
	av[8] = "-norc";
	av[9] = "-f";
	av[10] = fname;
	av[11] = NULL;
	hProc = (HANDLE)_spawnv(_P_NOWAIT,AsirExe,av);
	if ( hProc == (HANDLE)-1 ) {
		fprintf(stderr,"%s ‚ª‚ ‚è‚Ü‚¹‚ñ",AsirExe);
		return 0;
	}
	read_until_eof();	
	return 1;
}

int doing_batch = 0;
static char insert_buf[BUFSIZ*10];
static int insert_buf_len = 0;

static void read_until_eof()
{
	char *p;
	DWORD len;
	unsigned int cmdsize,cmd,size;

	doing_batch = 1;
	while ( 1 ) {
		WaitForSingleObject(hNotify,(DWORD)-1);
		ResetEvent(hNotify);
		SetEvent(hNotify_Ack);
		ReadFile(hRead,&cmdsize,sizeof(unsigned int),&len,NULL);	
		cmd = cmdsize>>16;
		size = cmdsize&0xffff;
		if ( !cmd ) {
			doing_batch = 0;
			return;
		} else if ( size )
			ReadFile(hRead,insert_buf,size,&len,NULL);	
	}
}

void send_intr() {
	PulseEvent(hIntr);
}

void terminate_batch() {
	TerminateProcess(hProc,0);
}
#endif /* VISUAL */
