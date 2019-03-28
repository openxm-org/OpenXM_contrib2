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
 * $OpenXM$ 
*/
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
extern int emergency;

#define INBUF_EMPTY (W_BUF.inpos >= W_BUF.inlen)

void w_purge_stdin(void)
{
  W_BUF.inpos = 0;
  W_BUF.inlen = 0;
}

int w_filbuf(void)
{
  int status = get_line(W_BUF.inbuf);

  /* XXX */
  if ( emergency ) return 0;

  W_BUF.inpos = 0;
  W_BUF.inlen = strlen(W_BUF.inbuf);
  return status;
}

int w_flushbuf(void) {
  /* XXX */
  if ( emergency ) return 0;

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

int win_put_string(char *s,FILE *file);

int win_put_char(int c,FILE *file)
{
  static char s[2];
  
  s[0] = (char)c;
  win_put_string(s,file);
  return c;
}

int win_put_string(char *s,FILE *file)
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
    return win_put_char(c,file);
  else
    return fputc(c,file);
}

int w_fputs(char *s, FILE *file)
{
  if ( file == stdout || file == stderr )
    return win_put_string(s,file);
  else
    return fputs(s,file);
}

int w_printf(char *format, ...)
{
  va_list ap;
  int status;
  static char *buf;
  static int bufsiz=0;

  if ( !bufsiz ) {
    bufsiz = BUFSIZ*10; buf = (char *)malloc(bufsiz);
  }
  va_start(ap,format);
  status = vsnprintf(buf,0,format,ap);
  if ( status > bufsiz ) {
    bufsiz = 2*status; free(buf); buf = (char *)malloc(bufsiz);
  }
  status = vsprintf(buf,format,ap);
  win_put_string(buf,stdout);
  return status;
}

int w_fprintf(FILE *file, char *format, ...)
{
  va_list ap;
  int status;
  static char *buf;
  static int bufsiz=0;

  if ( !bufsiz ) {
    bufsiz = BUFSIZ*10; buf = (char *)malloc(bufsiz);
  }
  va_start(ap,format);
  if ( file == stdout || file == stderr ) {
    status = vsnprintf(buf,0,format,ap);
    if ( status > bufsiz ) {
      bufsiz = 2*status; free(buf); buf = (char *)malloc(bufsiz);
    }
    status = vsprintf(buf,format,ap);
    win_put_string(buf,file);
  } else {
    status = vfprintf(file,format,ap);
  }
  va_end(ap);
  return status;
}
