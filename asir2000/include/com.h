/* $OpenXM: OpenXM/src/asir99/include/com.h,v 1.1.1.1 1999/11/10 08:12:30 noro Exp $ */
#ifndef _COM_H_
#define _COM_H_
#if defined(VISUAL) || MPI
#include "wsio.h"
#endif

#if !defined(VISUAL)
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <rpc/rpc.h>
#endif

#define N32 0x80

#define ISIZ sizeof(int)
#define MAXIOFP 1024

#define C_ZERO 0
#define C_OBJ 1
#define C_VL 2
#define C_INT 3
#define C_CLOSE 4
#define C_ENDARG 5
#define C_PRIV C_ENDARG+1
#define C_SHUTDOWN 6

#if defined(VISUAL) || MPI
struct IOFP {
	STREAM *in,*out;
	int s,conv;
	char *socket;
};

void ws_loadv(STREAM *,V *);
void ws_savev(STREAM *,V);
void ws_loadvl(STREAM *);
void ws_savevl(STREAM *,VL);
V ws_load_convv(int);
int ws_save_convv(V);
#else
struct IOFP {
	FILE *in,*out;
	int s,conv;
	char *socket;
};
#endif
#endif /* _COM_H_ */
