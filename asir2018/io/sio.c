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
 * $OpenXM: OpenXM_contrib2/asir2018/io/sio.c,v 1.4 2020/10/06 06:31:20 noro Exp $
*/
#include "ca.h"
#include <setjmp.h>
#include "ox.h"
#if defined(VISUAL) || defined(__MINGW32__)
#include <winsock2.h>
#include <io.h>
#else
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include<signal.h>

#define SOCKQUEUELENGTH 5
#define ISIZ sizeof(int)

extern int little_endian;

int I_am_server;
struct IOFP iofp[MAXIOFP];

#if !defined(_PA_RISC1_1)
#define RSH "rsh"
#else
#define RSH "remsh"
#endif

void init_socket(void);

#if defined(VISUAL)
typedef int socklen_t;
#endif
#if !defined(VISUAL) && !defined(__MINGW32__)
#define closesocket(s)   (close((s)))
#endif

int getremotesocket(int s)
{
  return iofp[s].s;
}

void getremotename(int s,char *name)
{
  struct sockaddr_in peer;
  struct hostent *hp;
  int peerlen;

  peerlen = sizeof(peer);
  getpeername(getremotesocket(s),(struct sockaddr *)&peer,(socklen_t *)&peerlen);
  hp = gethostbyaddr((char *)&peer.sin_addr,sizeof(struct in_addr),AF_INET);
  if ( hp )
    strcpy(name,hp->h_name);
  else
    strcpy(name,(char *)inet_ntoa(peer.sin_addr));
}

void generate_port(int use_unix,char *port_str)
{
  double get_current_time();
  unsigned long mt_genrand();
  unsigned int port;
  static int count=0;
  int fd;

#if !defined(VISUAL) && !defined(__MINGW32__)
  if ( use_unix ) {
#if defined(ANDROID)
    sprintf(port_str,"/data/data/com.termux/files/usr/tmp/ox%02x.XXXXXX",count);
#else
    sprintf(port_str,"/tmp/ox%02x.XXXXXX",count);
#endif
    count++;
    fd = mkstemp(port_str);
    unlink(port_str);
  } else 
#endif
  {
    port = ((unsigned int)mt_genrand()+(unsigned int)get_current_time())
      %(65536-1024)+1024;
    sprintf(port_str,"%d",port);
  }
}

int try_bind_listen(int use_unix,char *port_str)
{
  struct sockaddr_in sin;
  struct sockaddr *saddr;
  int len;
  int service;
#if !defined(VISUAL) && !defined(__MINGW32__)
  struct sockaddr_un s_un;

  if ( use_unix ) {
    service = socket(AF_UNIX, SOCK_STREAM, 0);
    if (service < 0) {
      perror("in socket");
      return -1;
    }
    s_un.sun_family = AF_UNIX;
    strcpy(s_un.sun_path,port_str);
#if defined(__FreeBSD__)
    len = SUN_LEN(&s_un);
    s_un.sun_len = len+1; /* XXX */
#else
    len = strlen(s_un.sun_path)+sizeof(s_un.sun_family);
#endif
    saddr = (struct sockaddr *)&s_un;
  } else 
#endif
  {
    service = socket(AF_INET, SOCK_STREAM, 0);
    if ( service < 0 ) {
      perror("in socket");
      return -1;
    }
    sin.sin_family = AF_INET; sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons((unsigned short)atoi(port_str));
    len = sizeof(sin);
    saddr = (struct sockaddr *)&sin;
  }
  if (bind(service, saddr, len) < 0) {
    perror("in bind");
    closesocket(service);
    return -1;
  }
  if (getsockname(service,saddr, (socklen_t *)&len) < 0) {
      perror("in getsockname");
      closesocket(service);
      return -1;
  }
  if (listen(service, SOCKQUEUELENGTH) < 0) {
    perror("in listen");
    closesocket(service);
    return -1;
  }
  return service;
}

/*
  try to accept a connection request

  Input
    af_unix: s is UNIX domain socket if af_unix is nonzero
    s: socket

  Output
    c: an accepted socket which is newly created
    -1: if failed to accept

  the original socket is always closed.
*/

int try_accept(int af_unix,int s)
{
  int len,c,i;
  struct sockaddr_in sin;

#if !defined(VISUAL) && !defined(__MINGW32__)
  struct sockaddr_un s_un;
  if ( af_unix ) {
    len = sizeof(s_un);
    for ( c = -1, i = 0; (c < 0)&&(i = 10) ; i++ )
      c = accept(s, (struct sockaddr *) &s_un, (socklen_t *)&len);
  } else 
#endif
  {

    len = sizeof(sin);
    for ( c = -1, i = 0; (c < 0)&&(i = 10) ; i++ )
      c = accept(s, (struct sockaddr *) &sin, (socklen_t *)&len);
  }
  if ( i == 10 )
    c = -1;
  closesocket(s);
  return c;
}

int try_connect(int use_unix,char *host,char *port_str)
{
  struct sockaddr_in sin;
  struct sockaddr *saddr;
  struct hostent *hp;
  int len,s,i;
#if !defined(VISUAL) && !defined(__MINGW32__)
  struct sockaddr_un s_un;
#endif

  for ( i = 0; i < 10; i++ ) {
#if !defined(VISUAL) && !defined(__MINGW32__)
    if ( use_unix ) {
      if ( (s = socket(AF_UNIX,SOCK_STREAM,0)) < 0 ) {
        perror("socket");
        return -1;
      }
      bzero(&s_un,sizeof(s_un));
      s_un.sun_family = AF_UNIX;
      strcpy(s_un.sun_path,port_str);
#if defined(__FreeBSD__)
      len = SUN_LEN(&s_un);
      s_un.sun_len = len+1; /* XXX */
#else
      len = strlen(s_un.sun_path)+sizeof(s_un.sun_family);
#endif
      saddr = (struct sockaddr *)&s_un;
    } else 
#endif /* VISUAL */
    {
      if ( !host )
        host = "127.0.0.1";
      if ( (s = socket(AF_INET,SOCK_STREAM,0)) < 0 ) {
        perror("socket");
        return -1;
      }
      bzero(&sin,sizeof(sin));
      sin.sin_port = htons((unsigned short)atoi(port_str));
      sin.sin_addr.s_addr = inet_addr(host);
      if ( sin.sin_addr.s_addr != -1 ) {
        sin.sin_family = AF_INET;
      } else {
        hp = gethostbyname(host);
        bcopy(hp->h_addr,&sin.sin_addr,hp->h_length);
        sin.sin_family = hp->h_addrtype;
      }
      len = sizeof(sin);
      saddr = (struct sockaddr *)&sin;
    }
    if ( connect(s,saddr,len) >= 0 )
      break;
    else {
      closesocket(s);
#if defined(VISUAL) || defined(__MINGW32__)
      Sleep(100);
#else
      usleep(100000);
#endif
    }
  }
  if ( i == 10 ) {
    perror("connect");
    return -1;
  } else
    return s;
}

#if 0
void close_allconnections()
{
  int s;

#if defined(SIGPIPE)
  signal(SIGPIPE,SIG_IGN);
#endif
  for ( s = 0; s < MAXIOFP; s++ )
    close_connection(s);
}

void close_connection(int s)
{
  struct IOFP *r;

  r = &iofp[s];
  if ( r->in && r->out ) {
    if ( check_sm_by_mc(s,SM_shutdown) )
      ox_send_cmd(s,SM_shutdown);
    free_iofp(s);
  }
}
#else
void close_allconnections()
{
  shutdown_all();
}
#endif

void free_iofp(int s)
{
  struct IOFP *r;

  r = &iofp[s];
#if defined(VISUAL) || defined(__MINGW32__)
  if ( r->s ) closesocket(r->s);
#elif !defined(MPI)
  if ( r->in ) fclose(r->in);
  if ( r->out ) fclose(r->out);
  if ( r->socket ) unlink(r->socket);
#endif
  r->inbuf = r->outbuf = 0;
  r->in = r->out = 0; r->s = 0;
}

int get_iofp(int s1,char *af_sock,int is_server)
{
  int i;
  unsigned char c,rc;
  extern int mpi_myid;

#if defined(MPI)
  iofp[s1].s = s1;
  if ( mpi_myid == s1 ) {
    iofp[s1].in = 0;
    iofp[s1].out = 0;
  } else {
    iofp[s1].in = WSIO_open(s1,"r");
    iofp[s1].out = WSIO_open(s1,"w");
  }
  iofp[s1].conv = 0;
  iofp[s1].socket = 0;

  return s1;
#else
  for ( i = 0; i < MAXIOFP; i++ )
    if ( !iofp[i].in )
      break;
  iofp[i].s = s1;
#if defined(VISUAL) || defined(__MINGW32__)
  iofp[i].in = WSIO_open(s1,"r");
  iofp[i].out = WSIO_open(s1,"w");
#else
  iofp[i].in = fdopen(s1,"r");
  iofp[i].out = fdopen(dup(s1),"w");
#if !defined(__CYGWIN__)
  setbuffer(iofp[i].in,iofp[i].inbuf = (char *)MALLOC_ATOMIC(LBUFSIZ),LBUFSIZ);
  setbuffer(iofp[i].out,iofp[i].outbuf = (char *)MALLOC_ATOMIC(LBUFSIZ),LBUFSIZ);
#endif
#endif
  if ( little_endian )
    c = 1;
  else
    c = 0xff;
  if ( is_server ) {
    /* server : write -> read */
    write_char((FILE *)iofp[i].out,&c); ox_flush_stream_force(i);
    read_char((FILE *)iofp[i].in,&rc);
  } else {
    /* client : read -> write */
    read_char((FILE *)iofp[i].in,&rc);
    /* special care for a failure of spawing a server */
    if ( rc !=0 && rc != 1 && rc != 0xff )
      return -1;  
    write_char((FILE *)iofp[i].out,&c); ox_flush_stream_force(i);
  }
  iofp[i].conv = c == rc ? 0 : 1;
  if ( af_sock && af_sock[0] ) {
    iofp[i].socket = (char *)malloc(strlen(af_sock)+1);
    strcpy(iofp[i].socket,af_sock);
  } else
    iofp[i].socket = 0;
  return i;
#endif
}

#if defined(VISUAL) || defined(__MINGW32__)
void init_socket()
{
  static int socket_is_initialized;
  WORD wVersionRequested;
  WSADATA wsaData;
  int err;
  wVersionRequested = MAKEWORD(2,0);

  if ( socket_is_initialized )
    return;
  err = WSAStartup(wVersionRequested,&wsaData);  
  if ( err )
    return;
}
#endif

int get_fd(int index)
{
  return iofp[index].s;
}

int get_index(int fd)
{
  int i;

  for ( i = 0; i < MAXIOFP; i++ )
    if ( iofp[i].s == fd )
      return i;
  return -1;
}

