/* $OpenXM: OpenXM_contrib2/asir2000/io/sio.c,v 1.2 1999/12/24 06:57:22 noro Exp $ */
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
	struct sockaddr_in peer;
	struct hostent *hp;
	int peerlen;

	peerlen = sizeof(peer);
	getpeername(getremotesocket(s),(struct sockaddr *)&peer,&peerlen);
	hp = gethostbyaddr((char *)&peer.sin_addr,sizeof(struct in_addr),AF_INET);
	if ( hp )
		strcpy(name,hp->h_name);
	else
		strcpy(name,(char *)inet_ntoa(peer.sin_addr));
}

int generate_port(use_unix,port_str)
int use_unix;
char *port_str;
{
	double get_current_time();
	unsigned long mt_genrand();
	unsigned int port;
	static int count=0;

	if ( use_unix ) {
		sprintf(port_str,"/tmp/ox%02x.XXXXXX",count);
		count++;
		mktemp(port_str);
	} else {
		port = ((unsigned int)mt_genrand()+(unsigned int)get_current_time())
			%(65536-1024)+1024;
		sprintf(port_str,"%d",port);
	}
}

int try_bind_listen(use_unix,port_str)
int use_unix;
char *port_str;
{
	struct sockaddr_in sin;
	struct sockaddr *saddr;
	int len;
	int service;
#if !defined(VISUAL)
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
		sin.sin_port = htons(atoi(port_str));
		len = sizeof(sin);
		saddr = (struct sockaddr *)&sin;
	}
	if (bind(service, saddr, len) < 0) {
		perror("in bind");
		close(service);
		return -1;
	}
	if (getsockname(service,saddr, &len) < 0) {
	    perror("in getsockname");
	    close(service);
	    return -1;
	}
	if (listen(service, SOCKQUEUELENGTH) < 0) {
		perror("in listen");
		close(service);
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

int try_accept(af_unix,s)
int af_unix,s;
{
	int len,c,i;
	struct sockaddr_in sin;

#if !defined(VISUAL)
	struct sockaddr_un s_un;
	if ( af_unix ) {
		len = sizeof(s_un);
		for ( c = -1, i = 0; (c < 0)&&(i = 10) ; i++ )
			c = accept(s, (struct sockaddr *) &s_un, &len);
	} else 
#endif
	{

		len = sizeof(sin);
		for ( c = -1, i = 0; (c < 0)&&(i = 10) ; i++ )
			c = accept(s, (struct sockaddr *) &sin, &len);
	}
	if ( i == 10 )
		c = -1;
	close(s);
	return c;
}

int try_connect(use_unix,host,port_str)
int use_unix;
char *host,*port_str;
{
	struct sockaddr_in sin;
	struct sockaddr *saddr;
	struct hostent *hp;
	int len,s,i;
#if !defined(VISUAL)
	struct sockaddr_un s_un;
#endif

	for ( i = 0; i < 10; i++ ) {
#if !defined(VISUAL)
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
			if ( (s = socket(AF_INET,SOCK_STREAM,0)) < 0 ) {
				perror("socket");
				return -1;
			}
			bzero(&sin,sizeof(sin));
			sin.sin_port = htons(atoi(port_str));
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
			close(s);
#if defined(VISUAL)
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

	for ( i = 0; i < MAXIOFP; i++ )
		if ( !iofp[i].in )
			break;
	iofp[i].s = s1;
#if defined(VISUAL) || MPI 
	iofp[i].in = WSIO_open(s1,"r");
	iofp[i].out = WSIO_open(s1,"w");
#else
	iofp[i].in = fdopen(s1,"r");
	iofp[i].out = fdopen(s1,"w");
	setbuffer(iofp[i].in,(char *)malloc(LBUFSIZ),LBUFSIZ);
	setbuffer(iofp[i].out,(char *)malloc(LBUFSIZ),LBUFSIZ);
#endif
#if MPI
	iofp[i].conv = 0;
	iofp[i].socket = 0;
#else
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
		write_char(iofp[i].out,&c); ox_flush_stream_force(i);
	}
	iofp[i].conv = c == rc ? 0 : 1;
	if ( af_sock && af_sock[0] ) {
		iofp[i].socket = (char *)malloc(strlen(af_sock)+1);
		strcpy(iofp[i].socket,af_sock);
	} else
		iofp[i].socket = 0;
#endif
	return i;
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
