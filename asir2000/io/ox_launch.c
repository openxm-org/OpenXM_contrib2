/* $OpenXM: OpenXM_contrib2/asir2000/io/ox_launch.c,v 1.1.1.1 1999/12/03 07:39:11 noro Exp $ */
#include <setjmp.h>
#include <signal.h>
#include <fcntl.h>
#include "ca.h"
#include "com.h"
#include "ox.h"
#if defined(VISUAL)
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <process.h>
#else
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#endif

#if defined(SYSV)
#include <unistd.h>
#endif

#if defined(VISUAL)
HANDLE hIntr,hReset,hKill;
#endif

static void put_log(char *);
static int which_command(char *,char *);
static int search_command(char *);
static int ox_spawn(char *,int,char *);
static void launch_error(char *);
static int ox_io_init(int);
static void push_one(Obj);
static Obj pop_one();
static void do_cmd(int);

static Obj *asir_OperandStack;
static int asir_OperandStackPtr;

static char hostname[BUFSIZ];
static int sindex;
static struct sockaddr_in peer;
static int cpid;

static void put_log(str)
char *str;
{
	static FILE *logfile;

	if ( !logfile )
		logfile = fopen("/tmp/ox_log","w");
	fprintf(logfile,"%s\n",str);
	fflush(logfile);
}

#if !defined(VISUAL)
static int which_command(com,file)
char *com,*file;
{
	char *c,*s;
	int len;
	char dir[BUFSIZ],path[BUFSIZ];

	for ( s = (char *)getenv("PATH"); s; ) {
		c = (char *)index(s,':');
		if ( c ) {
			len = c-s;
			strncpy(dir,s,len); s = c+1; dir[len] = 0;
		} else {
			strcpy(dir,s); s = 0;
		}
		sprintf(path,"%s/%s",dir,com);
		if ( search_command(path) ) {
			strcpy(file,path); return 1;
		}
	}
	file[0] = 0; return 0;
}

static int search_command(file)
char *file;
{
	struct stat buf;

	if ( stat(file,&buf) || (buf.st_mode & S_IFDIR) )
		return 0;
	if ( access(file,X_OK) )
		return 0;
	else
		return 1;
}
#endif

/*  
	argv[1] : host to connect
	argv[2] : if 1, call try_bind_listen and try_accept
			if 0, call try_connect
	argv[3] : control_port
	argv[4] : server_port 
	argv[5] : server
	argv[6] : display or "0"
*/

void launch_main(argc,argv)
int argc;
char **argv;
{
	int id;
	Obj p,obj;
	char *name;
	char buf[BUFSIZ];
	int cs,ss;
	unsigned int cmd;
	int use_unix,accept_client;
	char *control_port_str,*server_port_str;
	char *rhost,*server,*dname;

	GC_init(); nglob_init();
	gethostname(hostname,BUFSIZ);
	rhost = argv[1];
	use_unix = !strcmp(rhost,".") ? 1 : 0;
	accept_client = atoi(argv[2]) ? 1 : 0;
	control_port_str = argv[3];
	server_port_str = argv[4];
	server = argv[5];
	dname = argv[6];

#if defined(VISUAL)
		init_socket();
#endif

	signal(SIGINT,SIG_IGN);
#if defined(SIGUSR1)
	signal(SIGUSR1,SIG_IGN);
#endif

	if ( accept_client ) {
		cs = try_bind_listen(use_unix,control_port_str);
		ss = try_bind_listen(use_unix,server_port_str);
		cs = try_accept(use_unix,cs);
		ss = try_accept(use_unix,ss);
	} else {
		cs = try_connect(use_unix,rhost,control_port_str);
		ss = try_connect(use_unix,rhost,server_port_str);
	}
	ox_io_init(cs);
	if ( cs < 0 || ss < 0 )
		launch_error("cannot connect to the client");
	cpid = ox_spawn(server,ss,dname);

	while ( 1 ) {
		ox_recv(sindex,&id,&obj);
		switch ( id ) {
			case OX_COMMAND:
				cmd = ((USINT)obj)->body;
				do_cmd(cmd);
				break;
			case OX_DATA:
				push_one(obj);
				break;
			case OX_SYNC_BALL:
				break;
			default:
				break;
		}
	}
}

#if defined(VISUAL)
static void do_cmd(cmd)
int cmd;
{
	USINT t;
	int id,cindex;
	int bport,sport;
	int bs,bs0;
	STRING prog,dname;

	switch ( cmd ) {
		case SM_shutdown:
			SetEvent(hKill);
			exit(0); break;
		case SM_control_intr:
			SetEvent(hIntr);
			break;
		case SM_control_kill:
			SetEvent(hKill);
			break;
		case SM_control_reset_connection:
			MKUSINT(t,0);
			ox_send_data(sindex,t);
			SetEvent(hReset);
			break;
		default:
			break;
	}
}
#else
static void do_cmd(cmd)
int cmd;
{
	USINT t;
	int id,cindex;
	int bport,sport;
	int bs,bs0;
	int status;
	STRING prog,dname;

	switch ( cmd ) {
		case SM_shutdown:
			kill(cpid,SIGKILL);
			exit(0); break;
		case SM_control_intr:
			kill(cpid,SIGINT);
			break;
		case SM_control_kill:
			kill(cpid,SIGKILL);
			break;
		case SM_control_reset_connection:
			MKUSINT(t,0);
			ox_send_data(sindex,t);
			kill(cpid,SIGUSR1);
			break;
		default:
			break;
	}
}
#endif

static int ox_spawn(prog,bs,dname)
char *prog;
int bs;
char *dname;
{
#if defined(VISUAL)
	char *av[BUFSIZ];
	char sock_id[BUFSIZ],ox_intr[BUFSIZ],ox_reset[BUFSIZ],ox_kill[BUFSIZ];
	char AsirExe[BUFSIZ];
	HANDLE hProc;
	STRING rootdir;
	int mypid;
		
	mypid = GetCurrentProcessId();
	sprintf(ox_intr,"ox_intr_%d",mypid);
	sprintf(ox_reset,"ox_reset_%d",mypid);
	sprintf(ox_kill,"ox_kill_%d",mypid);
	hIntr = CreateEvent(NULL,TRUE,FALSE,ox_intr);
	hReset = CreateEvent(NULL,TRUE,FALSE,ox_reset);
	hKill = CreateEvent(NULL,TRUE,FALSE,ox_kill);
	sprintf(sock_id,"%d",bs);
	av[0] = prog;
	av[1] = sock_id;
	av[2] = ox_intr;
	av[3] = ox_reset;
	av[4] = ox_kill;
	av[5] = NULL;
	Pget_rootdir(&rootdir);
	sprintf(AsirExe,"%s\\bin\\engine.exe",BDY(rootdir));
	hProc = _spawnv(_P_NOWAIT,AsirExe,av);
	return (int)hProc;
#else /* VISUAL */
	int b,s,i;
	struct hostent *hp;
	int pid;
	char wname[BUFSIZ];
	char buf[BUFSIZ];

	pid = fork();
	if ( pid )
		return pid;
	else {
		setpgid(0,getpid());
		if ( dup2(bs,3) != 3 )
			exit(1);
		if ( dup2(bs,4) != 4 )
			exit(1);
		{
#if defined(linux) || defined(__NeXT__) || defined(ultrix)
#include <sys/param.h>
			close(0);
			for ( i = 5; i < NOFILE; i++ )
				close(i);
#else
#include <sys/resource.h>
			struct rlimit rl;

			getrlimit(RLIMIT_NOFILE,&rl);
			close(0);
			for ( i = 5; i < rl.rlim_cur; i++ )
				close(i);
#endif
		}
		if ( strcmp(dname,"0") )
			execl(prog,prog,"-display",dname,0);
		else {
			FILE *null;

			null = fopen("/dev/null","wb");
			dup2(fileno(null),1);
			dup2(fileno(null),2);
			putenv("DISPLAY=");
			execl(prog,prog,0);
		}
	}
#endif
}

static void launch_error(s)
char *s;
{
	exit(0);
}

static int ox_io_init(sock)
int sock;
{
	endian_init();
	/* server mode */
	sindex = get_iofp(sock,0,1);
	asir_OperandStack = (Obj *)CALLOC(BUFSIZ,sizeof(Obj));
	asir_OperandStackPtr = -1;
}

static void push_one(obj)
Obj obj;
{
	if ( !obj || OID(obj) != O_VOID )
		asir_OperandStack[++asir_OperandStackPtr] = obj;
}

static Obj pop_one() {
	if ( asir_OperandStackPtr >= 0 ) {
		return asir_OperandStack[asir_OperandStackPtr--];
	}
}

