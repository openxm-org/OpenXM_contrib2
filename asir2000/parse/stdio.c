/* $OpenXM: OpenXM/src/asir99/parse/stdio.c,v 1.1.1.1 1999/11/10 08:12:34 noro Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "w_stdio.h"

static struct w_buf {
  int inlen, inpos;
  int outpos;
  char inbuf[2*BUFSIZ];
  char outbuf[2*BUFSIZ];
} W_BUF;

static int noflush;

#define INBUF_EMPTY (W_BUF.inpos >= W_BUF.inlen)

int w_filbuf(void)
{
  int status = get_line(W_BUF.inbuf);
  W_BUF.inpos = 0;
  W_BUF.inlen = strlen(W_BUF.inbuf);
  return status;
}

int w_flushbuf(void) {
  W_BUF.outbuf[W_BUF.outpos] = 0;
  W_BUF.outpos = 0;
  put_line(W_BUF.outbuf);
  return 0;
}

int w_fflush(FILE *file)
{
  if ( file == stdout || file == stderr )
    return w_flushbuf();
  else
    return fflush(file);
}

void w_noflush_stderr(int flag)
{
  noflush = flag;
}

int get_char(void)
{
  int status = 0;

  if ( INBUF_EMPTY )
    status = w_filbuf();
  if ( status )
    return EOF;
  else
    return (int)W_BUF.inbuf[W_BUF.inpos++];
}

char *get_string(char *s, int n)
{
  int i;
  char *p;
  int status = 0;

  if ( INBUF_EMPTY )
    status = w_filbuf();
  if ( status )
    return NULL;
  for ( i = 0, p = s; i < n && !INBUF_EMPTY && W_BUF.inbuf[i]; i++ )
    *p++ = W_BUF.inbuf[W_BUF.inpos++];
  if ( i < n )
   *p = 0;
  return s;
}

int w_fgetc(FILE *file)
{
  if ( file == stdin )
    return get_char();     
  else
    return fgetc(file);
}

int unget_char(int c)
{
  W_BUF.inbuf[--W_BUF.inpos] = (char )c;
  return c;
}

int put_char(int c,FILE *file)
{
  static char s[2];
  
  s[0] = (char)c;
  put_string(s,file);
  return c;
}

int put_string(char *s,FILE *file)
{
  char c;

  while ( 1 ) {
    for ( ; (c = *s) && W_BUF.outpos < BUFSIZ; s++ ) {
      W_BUF.outbuf[W_BUF.outpos++] = c;
      if ( c == '\n' ) {
        W_BUF.outbuf[W_BUF.outpos-1] = '\r';
        W_BUF.outbuf[W_BUF.outpos++] = '\n';
        w_flushbuf();
      }
    }
    if ( !c )
      break;
    else if ( W_BUF.outpos == BUFSIZ )
      w_flushbuf();
  }
  if (file == stderr && !noflush)
      w_flushbuf();
  return 0;
}

char *w_fgets(char *s,int n, FILE *file)
{
  if ( file == stdin )
    return get_string(s,n);
  else
    return fgets(s,n,file);
}

int w_ungetc(int c, FILE *file)
{
  if ( file == stdin )
    return unget_char(c);
  else
    return ungetc(c,file);
}

int w_fputc(int c, FILE *file)
{
  if ( file == stdout || file == stderr )
    return put_char(c,file);
  else
    return fputc(c,file);
}

int w_fputs(char *s, FILE *file)
{
  int status;

  if ( file == stdout || file == stderr )
    return put_string(s,file);
  else
    return fputs(s,file);
}

int w_printf(char *format, ...)
{
  va_list ap;
  int status;
  char buf[BUFSIZ];

  va_start(ap,format);
  status = vsprintf(buf,format,ap);
  put_string(buf,stdout);
  return status;
}

int w_fprintf(FILE *file, char *format, ...)
{
  va_list ap;
  int status;
  char buf[BUFSIZ];

  va_start(ap,format);
  if ( file == stdout || file == stderr ) {
    status = vsprintf(buf,format,ap);
    put_string(buf,file);
  } else {
    status = vfprintf(file,format,ap);
  }
  va_end(ap);
  return status;
}
