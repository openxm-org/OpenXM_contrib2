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
 * $OpenXM: OpenXM_contrib2/asir2000/io/sio.c,v 1.9.2.3 2000/11/10 14:57:24 maekawa Exp $ 
*/
#if INET
#include "ca.h"
#include "setjmp.h"
#include "ox.h"
#if defined(VISUAL)
#include <winsock.h>
#else
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <sys/un.h>
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

int getremotesocket(s)
int s;
{
	return iofp[s].s;
}

void getremotename(s,name)
int s;
char *name;
{
	struct sockaddr_storage ss;
	struct sockaddr *sa;
	socklen_t len;
	char host[NI_MAXHOST];
	int rs;

	rs = getremotesocket(s);
	len = sizeof(ss);
	getpeername(rs, (struct sockaddr *)&ss, &len);
	sa = (struct sockaddr *)&ss;
	getnameinfo(sa, sa->sa_len, host, sizeof(host), NULL, 0, 0);
	strcpy(name, host);
}

int generate_port(use_unix,port_str)
int use_unix;
char *port_str;
{
	double get_current_time();
	unsigned long mt_genrand();
	unsigned int port;
	static int count=0;

#if !defined(VISUAL)
	if ( use_unix ) {
		sprintf(port_str,"/tmp/ox%02x.XXXXXX",count);
		count++;
		mktemp(port_str);
	} else 
#endif
	{
		port = ((unsigned int)mt_genrand()+(unsigned int)get_current_time())
			%(65536-1024)+1024;
		sprintf(port_str,"%d",port);
	}
}

int try_bind_listen(use_unix,port_str)
int use_unix;
char *port_str;
{
	struct addrinfo hints, *res, *ai;
	int s, error;
	char *errstr;

	memset(&hints, 0, sizeof(hints));
#if defined(VISUAL)
	hints.ai_family = PF_UNSPEC;
#else
	if (use_unix)
		hints.ai_family = PF_UNIX;
	else
		hints.ai_family = PF_UNSPEC;
#endif /* VISUAL */
	hints.ai_socktype = SOCK_STREAM;

	error = getaddrinfo(NULL, port_str, &hints, &res);
	if (error) {
		warnx("try_bind_listen(): %s", gai_strerror(error));
		return (-1);
	}

	s = -1;
	for (ai = res ; ai != NULL ; ai = ai->ai_next) {
		if ((s = socket(ai->ai_family, ai->ai_socktype,
				ai->ai_protocol)) < 0 ) {
			errstr = "in socket";
			continue;
		}

		if (bind(s, ai->ai_addr, ai->ai_addrlen) < 0) {
			errstr = "in bind";
			close(s);
			s = -1;
			continue;
		}

		if (listen(s, SOCKQUEUELENGTH) < 0) {
			errstr = "in listen";
			close(s);
			s = -1;
			continue;
		}

		/* established connection */
		break;
	}
	freeaddrinfo(res);

	if (s < 0)
		perror(errstr);
	return (s);
}

/*
  try to accept a connection request

  Input
    s: socket

  Output
    c: an accepted socket which is newly created
    -1: if failed to accept

  the original socket is always closed.
*/

int try_accept(s)
int s;
{
	struct sockaddr_storage ss;
	socklen_t len;
	int c, i;

	len = sizeof(ss);
	if (getsockname(s, (struct sockaddr *)&ss, &len) < 0) {
		close(s);
		return (-1);
	}

	for (i = 0 ; i < 10 ; i++) {
		c = accept(s, (struct sockaddr *)&ss, &len);
		if (c >= 0) {
			close(s);
			return (c);
		}
	}

	close(s);
	return (-1);
}

int try_connect(use_unix,host,port_str)
int use_unix;
char *host,*port_str;
{
	struct addrinfo hints, *res, *ai;
	int s, error, i;
	char *errstr;

	memset(&hints, 0, sizeof(hints));
#if defined(VISUAL)
	hints.ai_family = PF_UNSPEC;
#else
	if (use_unix)
		hints.ai_family = PF_UNIX;
	else
		hints.ai_family = PF_UNSPEC;
#endif /* VISUAL */
	hints.ai_socktype = SOCK_STREAM;

	error = getaddrinfo(host, port_str, &hints, &res);
	if (error) {
		warnx("try_connect: %s", gai_strerror(error));
		return (-1);
	}
	for (i = 0 ; i < 10 ; i++) {
		s = -1;
		for (ai = res ; ai != NULL ; ai = ai->ai_next) {
			if ((s = socket(ai->ai_family, ai->ai_socktype,
					ai->ai_protocol)) < 0 ) {
				errstr = "socket";
				continue;
			}
			if (connect(s, ai->ai_addr, ai->ai_addrlen) < 0) {
				errstr = "connect";
				close(s);
				s = -1;
				continue;
			}

			/* established a connection */
			break;
		}

		if (s >= 0) {
			freeaddrinfo(res);
			return (s);
		}

#if defined(VISUAL)
		Sleep(100);
#else
		usleep(100000);
#endif
	}
	freeaddrinfo(res);

	perror(errstr);
	return (-1);
}

#if 0
close_allconnections()
{
	int s;

#if defined(SIGPIPE)
	signal(SIGPIPE,SIG_IGN);
#endif
	for ( s = 0; s < MAXIOFP; s++ )
		close_connection(s);
}

close_connection(s)
int s;
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
close_allconnections()
{
	shutdown_all();
}
#endif

free_iofp(s)
int s;
{
	struct IOFP *r;

	r = &iofp[s];
	r->in = r->out = 0; r->s = 0;
#if !defined(VISUAL)
	if ( r->socket )
		unlink(r->socket);
#endif
}

#define LBUFSIZ BUFSIZ*10

int get_iofp(s1,af_sock,is_server)
int s1;
char *af_sock;
int is_server;
{
	int i;
	unsigned char c,rc;
	extern int mpi_myid;

#if MPI
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
#if defined(VISUAL)
	iofp[i].in = WSIO_open(s1,"r");
	iofp[i].out = WSIO_open(s1,"w");
#else
	iofp[i].in = fdopen(s1,"r");
	iofp[i].out = fdopen(s1,"w");
	setbuffer(iofp[i].in,(char *)malloc(LBUFSIZ),LBUFSIZ);
	setbuffer(iofp[i].out,(char *)malloc(LBUFSIZ),LBUFSIZ);
#endif
	if ( little_endian )
		c = 1;
	else
		c = 0xff;
	if ( is_server ) {
		/* server : write -> read */
		write_char(iofp[i].out,&c); ox_flush_stream_force(i);
		read_char(iofp[i].in,&rc);
	} else {
		/* client : read -> write */
		read_char(iofp[i].in,&rc);
		/* special care for a failure of spawing a server */
		if ( rc !=0 && rc != 1 && rc != 0xff )
			return -1;	
		write_char(iofp[i].out,&c); ox_flush_stream_force(i);
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

#if defined(VISUAL)
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

get_fd(index)
int index;
{
	return iofp[index].s;
}

get_index(fd)
int fd;
{
	int i;

	for ( i = 0; i < MAXIOFP; i++ )
		if ( iofp[i].s == fd )
			return i;
	return -1;
}

#endif /* INET */

