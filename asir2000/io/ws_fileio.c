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
 * $OpenXM: OpenXM_contrib2/asir2000/io/ws_fileio.c,v 1.11 2015/08/14 13:51:55 fujimoto Exp $ 
*/
#if defined(VISUAL) || defined(__MINGW32__) || defined(MPI)
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
#if _MSC_VER < 1500
    _fileno(&rst->fp) = -1;
#else
    (&rst->fp)->_file = -1;
#endif
#elif defined(MPI)
#if defined(sparc) || defined(__FreeBSD__)
    (&rst->fp)->_file = -1;
#elif defined(linux)
    (&rst->fp)->_fileno = -1;
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

#if defined(VISUAL) || defined(__MINGW32__)
  size = recv(s->fildes,data,count,0);
#elif defined(MPI)
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

#if defined(VISUAL) || defined(__MINGW32__)
  size = send(s->fildes,data,count,0);
  return size;
#elif defined(MPI)
  MPI_Ssend(&count,1,MPI_INT,s->fildes,0,MPI_COMM_WORLD);
  MPI_Ssend(data,count,MPI_CHAR,s->fildes,0,MPI_COMM_WORLD);
  return count;
#endif
}

#if defined(MPI)
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
