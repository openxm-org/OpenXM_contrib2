/* $OpenXM: OpenXM_contrib2/windows/post-msg-asirgui/cmdasir.c,v 1.7 2013/11/27 15:47:18 ohara Exp $ */
// cl test.c user32.lib

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <process.h>
#include <fcntl.h>

char *winname2uxname(char winname[]);
FILE *findAsirHandler();
int sendFileName(HWND hnd,char uname[]);
int loadFile(HWND hnd, char *uname);
int terminateAsir(HWND hnd);
int hasTemp(char *s);
int pasteFile(HWND hnd, char *uname);
int containEnd(char s[]);
int abortAsir(HWND hnd);
int main(int argc, char *argv[]);

FILE *open_stdio(DWORD type) {
    HANDLE hnd;
    int fd;
    if((hnd = GetStdHandle(type)) != INVALID_HANDLE_VALUE && (fd = _open_osfhandle((intptr_t)hnd, _O_TEXT)) >= 0) {
        return _fdopen(fd, "w");
    }
    return NULL;
}

int WINAPI WinMain(HINSTANCE hins, HINSTANCE prev, LPSTR arg, int show) {
    FILE *fp;
#if (_WIN32_WINNT > 0x0500)
    if(AttachConsole(ATTACH_PARENT_PROCESS)) {
        if(fp = open_stdio(STD_OUTPUT_HANDLE)) {
            *stdout = *fp;
            setvbuf(stdout, NULL, _IONBF, 0);
        }
    }
#endif
    main(__argc, __argv);
    return 0;
}

int main(int argc, char *argv[])
{
  HWND hnd;
  int c,ii;
  char *uname=NULL;
  FILE *fp=NULL;
  FILE *fp2=NULL;
  char snameWin[1024];
  char *snameUx;
  char msg[1024];
  char cmd[1024];
  int paste_contents=0;
  int abort=0;

  /* MessageBox(NULL,TEXT("test"),TEXT("Error in cmdasir.c"),MB_OK); */
  sprintf(snameWin,"%s\\cmdasir-%d.txt",getenv("TEMP"),_getpid());
  snameUx = winname2uxname(snameWin);
  if ((argc > 1) && (strcmp(argv[1],"--delete-tmp")==0)) {
    sprintf(cmd,"del %s\\cmdasir-*",getenv("TEMP"));
    system(cmd);
    return(0);
  }

  fp = findAsirHandler();
  if (fp == NULL) {
    printf("handler file is not found.\n"); return(-1);
  }
  fscanf(fp,"%d",&hnd);
  fclose(fp);
  if ((argc > 1) && (strcmp(argv[1],"--quit")==0)) {
    return terminateAsir(hnd);
  }
  if ((argc > 1) && (strcmp(argv[1],"--abort")==0)) {
    return abortAsir(hnd);
  }
  printf("Handler is %d\n",hnd);
  paste_contents=0;
  for (ii=1; ii<argc; ii++) {
    if (strcmp(argv[ii],"--paste-contents")==0) {
      paste_contents=1; continue;
    }
    uname = NULL;
    if (hasTemp(argv[ii])) {
      fp = fopen(winname2uxname(argv[ii]),"r");
      fp2 = fopen(snameUx,"w");
      if (fp2 == NULL) {
        sprintf(msg,"File %s to load is not found.",argv[ii]);
        MessageBox(NULL,TEXT(msg),TEXT("Error in cmdasir.c"), MB_OK);
        return(-1);
      }
      while ((c=fgetc(fp)) >= 0) fputc(c,fp2);
    	if (!paste_contents) fprintf(fp2,"\nend$\n");
      fclose(fp2);
      uname=snameUx;
    }
    printf("filename=%s\n",argv[ii]);
    if(_access(argv[ii],0)==0) {
    	if (paste_contents) return pasteFile(hnd, (uname != NULL)? uname: winname2uxname(argv[ii]));
    	else return loadFile(hnd, (uname != NULL)? uname: winname2uxname(argv[ii]));
    }
  }
  /* work file is kept. Prepare a commnd to delete all the work files under temp
   */
  while ((c=getchar()) != EOF) {
    if (!PostMessage(hnd,WM_CHAR,c,1)) {
      MessageBox(NULL,TEXT("asirgui is not running."), TEXT("ERROR in cmdasir.c"),MB_OK);
      return(-1);
    }
  }
  return 0;
}

int loadFile(HWND hnd, char *uname) {
    char *s="load(@);\n";
    int i;
    for (i=0; i<strlen(s); i++) {
        if (s[i] != '@') PostMessage(hnd,WM_CHAR,s[i],1);
        else {
            PostMessage(hnd,WM_CHAR,'"',1);
            sendFileName(hnd,uname);
            if (!PostMessage(hnd,WM_CHAR,'"',1)) {
                MessageBox(NULL,TEXT("asirgui is not running."),TEXT("ERROR in cmdasir.c"),MB_OK);
                return -1;
            }
        }
    }
    return 0;
}

int terminateAsir(HWND hnd) {
    int i;
    char *s="quit;\n";
    for (i=0; i<strlen(s); i++) {
        if (!PostMessage(hnd,WM_CHAR,s[i],1)) {
            MessageBox(NULL,TEXT("asirgui is not running."),TEXT("ERROR in cmdasir.c"),MB_OK);
            return -1;
        }
    }
    return 0;
}

char *winname2uxname(char wname[]) {
  int i;
  char *uname;
  uname = (char *) malloc(strlen(wname)+1);
  for (i=0; i<strlen(wname); i++) {
    if (wname[i] == '\\') uname[i] = '/';
    else uname[i] = wname[i];
    uname[i+1] = 0;
  }
  return(uname);
}

#if 0
int hasTemp(char *s) {
  int i;
  size_t n=strlen(s);
  for (i=0; i<n-3; i++) {
    if (strncmp(&(s[i]),"Temp",4)==0) return(1);
  }
  return(0);
}
#endif

int hasTemp(char *s) {
	return strstr(s, "Temp")!=NULL;
}

FILE *findAsirHandler() {
  int i;
  FILE *hnd = NULL;
  char msg[1024];
  char path1[1024];
  char path0[1024];
  char *path[] = { NULL, NULL, "c:/Program files/asir/bin/asirgui.hnd", "c:/Program files (x86)/asir/bin/asirgui.hnd", "c:/asir/bin/asirgui.hnd" }; 
  sprintf(path0,"%s/asirgui.hnd",(char *)getenv("TEMP"));
  path[0]=winname2uxname(path0);
  if (getenv("ASIR_ROOTDIR") != NULL) {
    sprintf(path1,"%s/bin/asirgui.hnd",(char *)getenv("ASIR_ROOTDIR"));
    path[1]=winname2uxname(path1);
  }else{
    sprintf(path1,"%s/Desktop/asir/bin/asirgui.hnd",(char *)getenv("HOMEPATH"));
    path[1]=winname2uxname(path1);
  }
  for(i=0; i<sizeof(path); i++) {
    if(path[i]!=NULL) {
      hnd = fopen(path[i],"r");
      if (hnd != NULL) return(hnd);
    }
  }
  sprintf(msg,"asir handler is not found.");
  MessageBox(NULL,TEXT(msg),TEXT("ERROR in cmdasir.c"),MB_OK);
  return NULL;
}
#if 0
int sendFileName_ascii(HWND hnd,char uname[]) {
    int j;
    for (j=0; j<strlen(uname);j++) PostMessage(hnd,WM_CHAR,uname[j],1);
}
#endif
int sendFileName(HWND hnd,char uname[]) {
    size_t len=strlen(uname)+1;
    HGLOBAL hMem;
    LPTSTR pMem;
    if (!OpenClipboard(NULL) ) return 1;
    hMem = GlobalAlloc(GMEM_FIXED,len);
    pMem = (LPTSTR)hMem;
    lstrcpy(pMem,(LPCTSTR)uname);
    EmptyClipboard();
    SetClipboardData(CF_TEXT,hMem);
    CloseClipboard();
	if (!PostMessage(hnd,WM_CHAR,0x19,1)) { /* 0x19 ctrl-Y, ctrl-V 0x16 */
      MessageBox(NULL,TEXT("asirgui is not running."),TEXT("ERROR in cmdasir.c"),MB_OK);
	  return -1;
	}
	return 0;
}

int pasteFile(HWND hnd, char *uname) {
    FILE *fp;
	char *s=NULL;
	char *sold=NULL;
	int size=1024;
	int c,i;
	fp = fopen(uname,"r");
	if (fp == NULL) {
		MessageBox(NULL,TEXT("File is not found: "),TEXT(uname),MB_OK);
		return -1;
	}
	s = (char *)malloc(size+1);
	i = 0;s[0]=0;
	while ((c=fgetc(fp)) != EOF) {
		s[i] = c; s[i+1]=0;
		if (i >= size-1) {
			sold = s;
			size = size*2;
			s = (char*)malloc(size+1);
			strcpy(s,sold);
			free(sold);
		}
		i++;
	}
	if (containEnd(s)) {
		MessageBox(NULL,TEXT("The string contains end$ or end;"),TEXT(uname),MB_OK);
		return(0);
	}
	if (strlen(s)>0) sendFileName(hnd,s); else {
		MessageBox(NULL,TEXT("Empty string: "),TEXT(uname),MB_OK);
		return 0;
	}
	free(s);
	PostMessage(hnd,WM_CHAR,0xa,1);
	return 0;
}

int containEnd(char s[]) {
	int i;
	for (i=0; i<strlen(s)-4; i++) {
		if (strncmp(&(s[i]),"end$",4)==0) return 1;
		if (strncmp(&(s[i]),"end;",4)==0) return 1;
	}
	return(0);
}

int abortAsir(HWND hnd) {
  int c;
  int result;
  result=MessageBox(NULL,TEXT("Do you abort this computation?"),TEXT("Abort"),MB_OKCANCEL);
  if (result == IDCANCEL) return(0);
  c=0x3;
  if (!PostMessage(hnd,WM_CHAR,c,1)) {
    MessageBox(NULL,TEXT("asirgui is not running."), TEXT("ERROR in cmdasir.c"),MB_OK);
    return(-1);
  }
        
  printf("Sending ctrl-C\n");
  /* Sleep(3*1000); */
  result=MessageBox(NULL,TEXT("Interrupt is displayed in asirgui?"),TEXT("Sent ctrl-C."),MB_OKCANCEL);
  if (result == IDCANCEL) return(0);
  c='t'; PostMessage(hnd,WM_CHAR,c,1);
  c=0xd; PostMessage(hnd,WM_CHAR,c,1); 
  c='y'; PostMessage(hnd,WM_CHAR,c,1);
  c=0xd; PostMessage(hnd,WM_CHAR,c,1);
  return(0);
}
