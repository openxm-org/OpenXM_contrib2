/* $OpenXM: OpenXM/src/asir99/include/wsio.h,v 1.1.1.1 1999/11/10 08:12:30 noro Exp $ */
#ifndef WSIO_H
#define WSIO_H
#if defined(VISUAL)
#include <winsock.h>
#elif MPI
#include <sys/types.h>
#include <mpi.h>
#endif

#if MPI
#define WSIO_STRING_LENGTH	(1048576)  /* Default size of buffer */
#else
#define WSIO_STRING_LENGTH	(16384)  /* Default size of buffer */
#endif

#define	WSIO_BUFSIZ	WSIO_STRING_LENGTH

typedef struct
	{
	FILE	fp;
	int		fildes;
	int		p;
	int 		read_flag;
	int		buf_size;
	char*		buf;
	int		max_buf_size;
	int		eof;
	int		error;
	}STREAM;

STREAM *WSIO_open(int,char *);
int cread (char *, unsigned int, int, STREAM *);
int cwrite (char *, unsigned int, int, STREAM *);
int cflush(STREAM *);

#endif
