/* $OpenXM: OpenXM/src/asir99/io/ws_fileio.c,v 1.2 1999/11/19 10:41:08 noro Exp $ */
#if defined(VISUAL) || MPI
#include<stdio.h>
#include"wsio.h"

STREAM* WSIO_open(int, char *);
int WSIO_read (char *, int, STREAM *);
int WSIO_write (char *, int, STREAM *);
int WSIO_flushbuf(STREAM *);
int WSIO_fillbuf(STREAM *);

int cread (data, element_size, count, s)
char* data;
unsigned element_size;
int count;
STREAM* s;
{
	int length,total,r;
	char *p;

	if (!s || !s->read_flag || !data 
		|| !element_size || !count)
		return 0;

	length = element_size * count;

	total = length;
	p = data;
	while ( length > (r = s->buf_size - s->p) ) {
		memcpy((void *)p,(void *)(s->buf+s->p),r);
		s->p += r;
		p += r;
		length -= r;
		if ( !WSIO_fillbuf(s) )
			return (total-length)/ element_size;
	}
	memcpy((void *)p,(void *)(s->buf+s->p),length);
	s->p += length;
	return count;
}

int cwrite (data, element_size, count, s)
char* data;
unsigned element_size;
int count;
STREAM* s;
{
	int length,total,r;
	char *p;

	if (!s || s->read_flag || !data 
		|| !element_size || !count)
		return 0;

	length = element_size * count;

	total = length;
	p = data;
	while ( length > (r = s->buf_size - s->p) ) {
		memcpy((void *)(s->buf+s->p),(void *)p,r);
		s->p += r;
		p += r;
		length -= r;
		if ( !WSIO_flushbuf(s) )
			return (total-length)/ element_size;
	}
	memcpy((void *)(s->buf+s->p),(void *)p,length);
	s->p += length;
	return count;
}

int cflush(s)
STREAM* s;
{
	if (!s || s->read_flag)
		return EOF;

	if (!WSIO_flushbuf(s))
		return EOF;
	else
		return 0;
}

int WSIO_fillbuf(s)
STREAM* s;
{
	s->buf_size = WSIO_read(s->buf, s->max_buf_size, s);
	s->p = 0;

	return s->buf_size;
}

int WSIO_flushbuf(s)
STREAM* s;
{
	int rst;


	rst = WSIO_write(s->buf, s->p, s);
	s->p = 0;

	return rst;
}

STREAM* WSIO_open(fildes, mode)
int fildes;
char* mode;
{
	STREAM* rst = 0;
	int flag = 0;

	rst = (STREAM*)malloc(sizeof(STREAM));
	if (rst) {
#if defined(VISUAL)
		_fileno(&rst->fp) = -1;
#elif MPI
#if defined(sparc)
		(&rst->fp)->_file = -1;
#else
		fileno(&rst->fp) = -1;
#endif
#endif
		rst->fildes = fildes;

		if (mode[0] == 'r') {
			rst->read_flag = 1;
			rst->p = WSIO_STRING_LENGTH;
		} else {
			rst->read_flag = 0;
			rst->p = 0;
		}
		rst->max_buf_size = rst->buf_size = WSIO_STRING_LENGTH;
		rst->buf = (char*)malloc(WSIO_STRING_LENGTH);
		if (!rst->buf) {
			free (rst);
			rst = 0;
		}
		rst->eof = 0;
		rst->error = 0;
	}
	return rst;
}

int WSIO_read (data, count, s)
char* data;
int count;
STREAM* s;
{
	int size;

	if (!s)
		return 0;

#if defined(VISUAL)
	size = recv(s->fildes,data,count,0);
#elif MPI
	{
		MPI_Status status;

		MPI_Recv(&size,1,MPI_INT,s->fildes,0,MPI_COMM_WORLD,&status);
		if ( size > count )
			size = count;
		MPI_Recv(data,size,MPI_CHAR,s->fildes,0,MPI_COMM_WORLD,&status);
	}
#endif
	return size;
}

int WSIO_write (data, count, s)
char* data;
int count;
STREAM* s;
{
	int size;

	if (!s || !count)
		return 0;

#if defined(VISUAL)
	size = send(s->fildes,data,count,0);
	return size;
#elif MPI
	MPI_Ssend(&count,1,MPI_INT,s->fildes,0,MPI_COMM_WORLD);
	MPI_Ssend(data,count,MPI_CHAR,s->fildes,0,MPI_COMM_WORLD);
	return count;
#endif
}

#if MPI
int mpi_nprocs,mpi_myid;
 
void mpi_init()
{
	int argc;
	char *argv[1];
 
	argc = 0; argv[0] = 0;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&mpi_nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&mpi_myid);
}
 
void mpi_finalize()
{
	MPI_Finalize();
}
#endif
#endif
