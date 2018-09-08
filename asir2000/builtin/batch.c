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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/batch.c,v 1.6 2015/08/14 13:51:54 fujimoto Exp $ 
*/
#if !defined(VISUAL) && !defined(__MINGW32__)

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
	char *av[BUFSIZ];
	char AsirExe[BUFSIZ];
	DWORD mypid;
	HANDLE hR0,hW0,hR1,hW1;
	SECURITY_ATTRIBUTES SecurityAttributes;
	char remread[10],remwrite[10];
	char notify[100],notify_ack[100],intr[100],intr_ack[100],kill[100];
	void _setargv();

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
#if _MSC_VER < 1900
	_setargv();
#endif
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
