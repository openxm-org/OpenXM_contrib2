/*	Copyright (c) 1987, 1988 by Software Research Associates, Inc.	*/

#if defined(ANDROID)
#include <strings.h>
#define index(s,c) strchr(s,c)
#define rindex(s,c) strrchr(s,c)
#endif

#ifndef lint
static char rcsid[]=
"$Id$ (SRA)";
#endif /* lint */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#if defined(sun)
#include <sys/fcntl.h>
#include <sys/ttold.h>
#endif
#ifdef TERMIOS
#include <termios.h>
#if defined(__linux__) || defined(__CYGWIN__)
#ifndef _POSIX_VDISABLE
#define _POSIX_VDISABLE '\0'
#endif
#endif
#else
#include <sgtty.h>
#endif
#include <sys/file.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/errno.h>

#include "fep_defs.h"
#include "fep_glob.h"
#include "fep_funcs.h"
#ifdef STAT
#include "fep_stat.h"
#endif

#ifndef lint
  static char fep_defsrc[] = FEP_DEFS;
  static char fep_globrc[] = FEP_GLOB;
  static char fep_funcsrc[] = FEP_FUNCS;
#ifdef STAT
  static char fep_statrc[] = FEP_STAT;
#endif
#endif /* lint */

char	*myself;			/* the command name */
char	*prompt = "";			/* prompt string */
char	*delimiters = DEFAULT_DELIMITERS;
					/* delimiter characters */
int	master;				/* file descriptor for pty master */
int	slave;				/* file descriptor for pty slave */
fd_set	mastermask;			/* 1<<master */
fd_set	stdinmask;			/* 1<<fileno(stdin) */
fd_set	selectmask;			/* stdinmask | mastermask */
int	selectnfds;			/* max (fileno(stdin), master) + 1*/
int	child_pid;			/* child pid */
int	ptyflag = ON;			/* flag to use pty or not */
int	histlen = -1;			/* history length */
int	debug = OFF;			/* debug switch */
int	auto_tty_fix = ON;		/* fix tty mode automatically */
FILE	*script_fp = NULL;		/* script file pointer */
#if defined(sun)
void	catchsig (int);		/* function take care SIGCHILD */
#else
void	catchsig __P((int));		/* function take care SIGCHILD */
#endif

#ifdef TERMIOS
#define ttystruct termios
#elif defined(TIOCSETN)
#define ttystruct sgttyb
#endif
struct	ttystruct initial_ttymode;		/* initial tty mode */
struct	ttystruct master_ttymode;		/* master tty mode */
struct	ttystruct slave_ttymode;		/* slave tty mode */

int	lines;				/* terminal line size */
int	columns;			/* terminal column size */
char	*term_clear;			/* terminal clear code */

#if defined(sun)
void	(*sighup)(int), (*sigchld)(int), (*sigtstp)(int);
#else
void	(*sighup) __P((int)), (*sigchld) __P((int)), (*sigtstp) __P((int));
#endif
					/* function buffer for signal */

#ifdef TIOCSETN
struct	tchars tchars_buf;		/* tty characters */
struct	ltchars ltchars_buf;		/* tty characters */
#endif
int	lmode_buf;			/* local mode */
int	line_desc;			/* line descipline */
#ifdef KANJI
struct	jtchars jtchars_buf;		/* kanji tty characters */
int	kmode_buf;			/* kanji mode */
#endif /* KANJI */

char	master_tty[16];			/* master tty name */
char	slave_tty[16];			/* slave tty name */

BUFFER iobuffer;			/* buffer self */
BUFFER *output_buffer = &iobuffer;	/* buffer pointer */

struct cmdinfo {
	char *command;
	char *prompt;
	char *delimiters;
} cmdinfo_tab [] = {
	{"/bin/sh",		"$ ",		" \t()<>{};&|='\""},
	{"sh",			"$ ",		" \t()<>{};&|='\""},
	{"jsh",			"$ ",		" \t()<>{};&|='\""},
	{"/bin/csh",		"% ",		" \t()<>{};&|='\""},
	{"csh",			"% ",		" \t()<>{};&|='\""},
	{"jcsh",		"% ",		" \t()<>{};&|='\""},
	{"/usr/ucb/dbx",	"(dbx) ",	" \t>"},
	{"dbx",			"(dbx) ",	" \t>"},
	{"/etc/lpc",		"lpc> ",	" \t"},
	{"lpc",			"lpc> ",	" \t"},
	{"/usr/ucb/mail",	"& ",		" \t"},
	{"mail",		"& ",		" \t"},
	{"/usr/lib/sendmail",	"> ",		" \t"},
	{"sendmail",		"> ",		" \t"},
	{"/usr/sra/calc",	"CALC: ",	" \t"},
	{"calc",		"CALC: ",	" \t"},
	{0,			0}
};

int main(int argc, char *argv[])
{
    int     i;
    char    *cp;
    char    **commandv;
    char    *allocAndCopyThere(char *);

    myself = argv[0];

    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    /*
     * Initialize binding table
     */
    init_bind_table ();

    /*
     * Set default variables before process arguments.
     */
    set_default_vars ();


    /*
     * Look environment variable first.
     */
    /* EDITMODE */
    if ((cp=getenv("EDITMODE")) != 0 && (eq(cp,"emacs")||eq(cp,"vi")))
	set_only_var ("editmode", cp);
    /* USER */
    if ((cp=getenv("USER"))!=0)
	set_only_var ("user", cp);
    /* HOME */
    if ((cp=getenv("HOME"))!=0)
	set_only_var ("home", cp);
    /* TERM */
    if ((cp=getenv("TERM"))!=0)
	set_only_var ("term", cp);
    /* SHELL */
    if ((cp=getenv("SHELL"))!=0)
	set_only_var ("shell", cp);

    /*
     * Check arguments.
     */
    while (argv[1] && argv[1][0] == '-') {
	switch (argv[1][1]) {

	    case '\0': /* - */
		ptyflag = 0;
		break;

	    case 'd':  /* -d */
		debug = ON;
		break;

	    case 'e': /* -emacs */
		if (strcmp (argv[1], "-emacs") == 0)
		    set_only_var ("editmode", "emacs");
		else
		    goto DEFAULT;
		break;

	    case 'h': /* -h */
		if (argv[1][2] == '\0') {
		    argv++;
		    argc--;
		    if (argc == 1)
			usageAndExit ();
		    histlen = atoi (argv[1]);
		}
		else {
		    histlen = atoi (argv[1] + 2);
		}
		break;

	    case 'p': /* -p */
		if (argv[1][2] == '\0') {
		    argv++;
		    argc--;
		    if (argc == 1)
			usageAndExit ();
		    prompt = allocAndCopyThere (argv[1]);
		}
		else {
		    prompt = allocAndCopyThere (argv[1] + 2);
		}
		break;

	    case 'v': /* -vi */
		if (strcmp (argv[1], "-vi") == 0)
		    set_only_var ("editmode", "vi");
		else
		    goto DEFAULT;
		break;
DEFAULT:
	    default: 
		printf ("Unknown option \"%s\"\n", argv[1]);
	}
	argv++;
	argc--;
    }
    fflush (stdout);

    if (argc < 2 && ptyflag) {
	usageAndExit ();
    }

    look_cmdinfo (argv[1]);

    /*
     * Set variable of command name.
     */
    {
#if defined(ANDROID)
	char *cp = argv[1];
#else
	char *cp = argv[1];
#endif

	if (any ('/', cp))
	    cp = rindex (cp, '/') + 1;
	set_var (cp, "1");
	set_var ("command", cp);
    }

    commandv = &argv[1];

    if (! isatty (0)) {
#if defined(__INTERIX)
	execvp (*commandv, commandv);
#else
	execvp (*commandv, commandv);
#endif
	perror (*commandv);
	exit (1);
    }

    get_pty_master ();
    fix_tty ();

    if (histlen < 0) {
	if (getenv ("HISTLEN"))
	    histlen = atoi (getenv ("HISTLEN"));
	else
	    histlen = DFL_HISTLEN;
    }

    init_hist (histlen);
    init_edit_params ();

    /*
     * Initialize output buffer.
     */
    if (!output_buffer->b_buf) {
	int size;

	if ((size = atoi (look_var ("buffer"))) <= 0)
	    size = 5120;
	if (!set_buffer (output_buffer, size)) {
	    fprintf (stderr, "Can't allocate enough memory\n");
	    kill_process ();
	    exit (1);
	}
    }

    if (ptyflag) {
	child_pid = fork ();
	if (child_pid < 0) {
	    perror ("fork");
	    kill_process ();
	    exit (1);
	}
	if (child_pid == 0)
	    exec_to_command (commandv);
    }
    fix_signal ();

    input_handler ();
}

void fix_signal ()
{
#ifdef SIGWINCH
#if defined(sun)
    void sigwinch (int);
#else
    void sigwinch __P((int));
#endif
#endif

    sighup = signal (SIGHUP, terminate);
    sigchld = signal (SIGCHLD, catchsig);
    sigtstp = signal (SIGTSTP, SIG_IGN);    
    signal (SIGINT, (void(*)())send_int_sig);
#ifdef SIGWINCH
    (void) signal(SIGWINCH, sigwinch);
#endif
}

void recover_signal ()
{

    (void) signal (SIGHUP, sighup);
    (void) signal (SIGCHLD, sigchld);
    (void) signal (SIGTSTP, sigtstp);
}

void input_handler()
{
    char   *inputline;
    char   *mygetline ();

    /*
     * Get slave tty descriptor for auto-tty-fix
     */
    if ((slave = open (slave_tty, O_RDONLY)) < 0)
	perror ("open");

    while ((inputline = mygetline ())!=0) {
	/*
	 * XXX: nbyte should be greater than 0 only for ^@ input in emacs.
	 * This solution is very ugly.. but it will takes a half day
	 * to fix this problem in more right way.  I will fix this problem
	 * in future release.
	 */
	register int nbyte = max (strlen (inputline), 1);

	/*
	 * Write to master pty
	 */
	write (master, inputline, nbyte);

	/*
	 * NOTE:
	 * Saving command line to output buffer is done in mygetline().
	 * Because inputline here is converted by alias.
	 */

#ifdef STAT
	stat_ibyte += nbyte;
#endif

	/*
	 * Write to script file.
	 */
	if (Through == OFF && Transparency == OFF && script_fp)
	    fwrite (inputline, sizeof(CHAR), strlen (inputline), script_fp);
    }
    terminate (0);
}

#define INPUT_BUFFER_SIZE 1024

#ifdef USE_TIMEOUT
/*
 * NOTE:
 * Now these time mechanism is not used.
 * Terminal status is get at every input from stdin.
 */
struct timeval timeout_0s	= {0L, 0L};
struct timeval timeout_500ms	= {0L, 500000L};
struct timeval timeout_1s	= {1L, 0L};
struct timeval timeout_5s	= {5L, 0L};
struct timeval timeout_10s	= {10L, 0L};
struct timeval timeout_60s	= {60L, 0L};

#define TIMEOUT_IMMID		&timeout_500ms
#define TIMEOUT_SOON		&timeout_1s
#define TIMEOUT_SHORT		&timeout_5s
#define TIMEOUT_MID		&timeout_10s
#define TIMEOUT_LONG		&timeout_60s
#define TIMEOUT_FOREVER		(struct timeval *)0
#define TIMEOUT_NOBLOCK		&timeout_0s

struct timeval *timeout_list[] = {
	TIMEOUT_IMMID,
	TIMEOUT_SOON,
	TIMEOUT_SHORT,
	TIMEOUT_MID,
	TIMEOUT_LONG,
	TIMEOUT_FOREVER
};
#else
struct timeval timeout_0s	= {0L, 0L};
#define TIMEOUT_FOREVER		(struct timeval *)0
#define TIMEOUT_NOBLOCK		&timeout_0s
#endif

struct timeval *notimeout[] = {
	TIMEOUT_FOREVER
};

int getcharacter()
{
    char c;
    int n;
    int nfound;
    fd_set readfd, writefd, exceptfd;
#ifdef USE_TIMEOUT
    struct timeval **timeout = auto_tty_fix ? timeout_list : notimeout;
#else
    struct timeval **timeout = notimeout;
#endif
    extern int errno;


    /*
     * Sorry, this cording depends to an implementation of getc().
     */
    FD_ZERO(&writefd);
    FD_ZERO(&exceptfd);

RETRY:
    readfd = selectmask;

#ifdef STAT
    stat_nselect++;
#endif
    while ((nfound = select (selectnfds, &readfd, 0, 0, *timeout)) < 0)
	if (errno != EINTR) {
		perror ("select");
		terminate(0);
	}

    /*
     * Found output from pty.
     */
    if (FD_ISSET(master, &readfd)) {
	int nbyte;

	/*
	 * Read from pty.
	 */
	nbyte = buf_read (master, output_buffer);

	if (nbyte > 0) {
	    /*
	     * Write to stdout
	     */
	    write (1, output_buffer->b_lastbuf, nbyte);

	    /*
	     * Write to script file
	     */
	    if (script_fp)
		fwrite (output_buffer->b_lastbuf,
			sizeof(CHAR), nbyte, script_fp);

#ifdef STAT
	    stat_obyte += nbyte;
#endif
	}
	else if (nbyte < 0) {
	    perror ("read");
#ifdef STAT
	    stat_rerror++;
#endif
	}
    }

    /*
     * Found input from terminal
     */
    if (FD_ISSET(fileno(stdin), &readfd)) {

#ifndef USE_TIMEOUT
	/*
	 * Look slave tty mode before every input.
	 * This will be done only before real input from stdin, because
	 * if character remained in FILE buffer, control goes to RETURNCHAR
	 * label.
	 */
    	if (Through == OFF && auto_tty_fix)
	    (void) fix_transparency ();
#endif

	RETURNCHAR:
	if ((c = getc (stdin)) == EOF) {
	    if (debug)
		printf ("EOF caught\n");
	    terminate (0);
	}
	else
	    return (c & CHARMASK);
    }

#ifdef USE_TIMEOUT
    /*
     * In the case of timeout.
     */
    if (nfound == 0) {

	/*
	 * Only to make sure
	 */
    	if (!auto_tty_fix) {
	    printf ("Timeout should not happen!!\n");
	    timeout = notimeout;
	    goto RETRY;
	}

	(void) fix_transparency ();
	++timeout;

	if (debug)
	    errorBell();
    }
#endif

    goto RETRY;
}

int set_buffer (BUFFER *bp, int size)
{
    char *newbuf;

    if (bp->b_buf)
	newbuf = (char *) realloc (bp->b_buf, size);
    else {
	newbuf = (char *) malloc (size);
	bp->b_count = bp->b_next = 0;
    }

    if (newbuf == 0)
	return (0);

    bp->b_buf = newbuf;
    bp->b_max = size;
    bp->b_hiwater = size * 4 / 5;

    if (bp->b_next > bp->b_hiwater) {
	bp->b_count = bp->b_next;
	bp->b_next = 0;
    }
    return (1);
}
	
/*    int fd;			file descriptor */
/*    BUFFER *bp;		buffer pointer */

int buf_read (int fd, BUFFER *bp)
{
    int nbyte;

    /*
     * save previous next pointer
     */
    bp->b_lastbuf = bp->b_buf + bp->b_next;

    /*
     * read from fd as possible
     */
    nbyte = read (fd, bp->b_buf + bp->b_next, bp->b_max - bp->b_next);

    /*
     * move next read pointer
     */
    bp->b_next += nbyte;

    /*
     * If count has not reached high-water mark, increment count
     * by read count.
     */
    if (bp->b_count < bp->b_hiwater)
	bp->b_count += nbyte;
	
    /*
     * If next pointer goes further than high-water mark, discard contents
     * after next pointer, and move next pointer to buffer top.
     */
    if (bp->b_next > bp->b_hiwater) {
	bp->b_count = bp->b_next;
	bp->b_next = 0;
    }

    return (nbyte);
}

/*
    BUFFER *bp;			buffer pointer
    char *s;			string pointer
*/

void buf_put (BUFFER *bp, char *s)
{
    int nbyte;
    int slen;

    nbyte = strlen (s);

    while (nbyte > 0) {

	slen = min (bp->b_max - bp->b_next, nbyte);
	strncpy (bp->b_buf + bp->b_next, s, slen);
	s += slen;
	nbyte -= slen;
	bp->b_next += slen;

	if (bp->b_count < bp->b_hiwater)
	    bp->b_count += slen;

	if (bp->b_next > bp->b_hiwater) {
	    bp->b_count = bp->b_next;
	    bp->b_next = 0;
	}
    }
}

void swallow_output()
{
    fd_set readfd = mastermask;
    int r;
    int nbyte;
    int ncount = 10;

    while (
	ncount-- &&
	select (selectnfds, &readfd, 0, 0, TIMEOUT_NOBLOCK) > 0 &&
	FD_ISSET(master, &mastermask)
    ) {
	nbyte = buf_read (master, output_buffer);
	if (nbyte > 0) {
	    write (1, output_buffer->b_lastbuf, nbyte);
#ifdef STAT
	    stat_obyte += nbyte;
	    stat_nselect++;
#endif

	    /*
	     * Write to script file
	     */
	    if (script_fp)
		fwrite (output_buffer->b_lastbuf,
			sizeof(CHAR), nbyte, script_fp);
	}
	else if (nbyte < 0) {
	    perror ("read");
#ifdef STAT
	    stat_rerror++;
#endif
	}
    }
    return;
}

#include <sys/wait.h>
#ifdef AIX
#include <sys/m_wait.h>
#endif

#if defined(__INTERIX) || defined(ANDROID)
#define wait3(s,opt,rp)   (waitpid((-1),(s),(opt)))
#endif

void catchsig(int n)
{
    int status;
    struct rusage   ru;

    if (wait3 (&status, WNOHANG | WUNTRACED, &ru) != child_pid)
	return;
    if (WIFSTOPPED (status) /* || WIFSIGNALED (status) */) {
	if (debug) {
	    message ("Child has sttoped!!\n");
	}
	suspend ();
	return;
    }
    terminate (0);
}

void exec_to_command(char *argv[])
{
    int t;

    /*
     * Disconnect control terminal
     */
    t = open ("/dev/tty", 2);
    if (t >= 0) {
#if !defined(__CYGWIN32__) && !defined(__INTERIX)
	ioctl (t, TIOCNOTTY, (char *) 0);
#endif
	(void) close (t);
    }

    get_pty_slave ();

    (void) close (master);
    dup2 (slave, 0);
    dup2 (slave, 1);
    dup2 (slave, 2);
    (void) close (slave);

#ifdef TERMIOS
    tcsetattr(0, TCSANOW, &slave_ttymode);
#elif defined(TIOCSETN)
    ioctl (0, TIOCSETN, (char *) & slave_ttymode);
#endif
#if defined(__INTERIX)
    execvp (*argv, argv);
#else
    execvp (*argv, argv);
#endif
    perror (*argv);
    exit (1);
}

#ifdef TERMIOS
void fix_tty()
{
    int i;
    master_ttymode = initial_ttymode;
    slave_ttymode = initial_ttymode;
    master_ttymode.c_lflag &= ~(ECHO|ECHOE|ECHOK|ICANON);

    for (i = 0; i < NCCS; i++)
	master_ttymode.c_cc[i] = _POSIX_VDISABLE;

    master_ttymode.c_cc[VMIN] = 1;
    master_ttymode.c_cc[VTIME] = 0;
    slave_ttymode.c_lflag &= ~(ECHO|ECHOE|ECHOK);
    slave_ttymode.c_iflag &= ~(ICRNL);
//    slave_ttymode.c_oflag &= ~(ONLCR);
    tcsetattr(0, TCSANOW, &master_ttymode);
    fprintf(stderr,"afo\n");
}

#elif defined(TIOCSETN)

void fix_tty()
{
    struct tchars tcbuf;
    struct ltchars lcbuf;

    master_ttymode = initial_ttymode;
    slave_ttymode = initial_ttymode;
    tcbuf = tchars_buf;
    lcbuf = ltchars_buf;

    master_ttymode.sg_flags |= CBREAK;
    master_ttymode.sg_flags &= ~ECHO;

    slave_ttymode.sg_erase = -1;
    slave_ttymode.sg_kill = -1;
    slave_ttymode.sg_flags &= ~(ECHO|CRMOD);

    tcbuf.t_intrc = -1;
    tcbuf.t_quitc = -1;
#ifdef AIX
    tcbuf.t_eofc = 0;
#else
    tcbuf.t_eofc = -1;
#endif
    tcbuf.t_brkc = -1;

    lcbuf.t_suspc = -1;
    lcbuf.t_dsuspc = -1;
    lcbuf.t_rprntc = -1;
    lcbuf.t_flushc = -1;
    lcbuf.t_werasc = -1;
    lcbuf.t_lnextc = -1;

    ioctl (0, TIOCSETN, (char *) & master_ttymode);
    ioctl (0, TIOCSETC, (char *) & tcbuf);
    ioctl (0, TIOCSLTC, (char *) & lcbuf);
}
#endif

void kill_process()
{

    if (child_pid)
	(void) killpg (child_pid, SIGTERM);
}

void terminate(int n)
{
    extern int errno;

    /*
     * Save history if 'history-file' is set
     */
    {
	char *cp, *mk_home_relative(char *), *look_var(char *);
	char buf [256];
	int num;

	if (look_var ("savehist") && (cp = look_var ("history-file"))) {

	    num = lookd_var ("savehist");
	    strcpy (buf, mk_home_relative (cp));
	    save_history (buf, num);
	}
    }

    /*
     * If scripting, close script file.
     */
    if (script_fp)
	fclose (script_fp);

    (void) signal (SIGCHLD, SIG_IGN);
    if (killpg (child_pid, SIGTERM) < 0 && errno != ESRCH)
	if (killpg (child_pid, SIGHUP) < 0)
	    if (killpg (child_pid, SIGKILL) < 0)
		perror ("kill");

#ifdef TERMIOS
    tcsetattr(0, TCSANOW, &initial_ttymode);
#elif defined(TIOCSETN)
    ioctl (0, TIOCSETN, (char *) & initial_ttymode);
    ioctl (0, TIOCSETC, (char *) & tchars_buf);
    ioctl (0, TIOCSLTC, (char *) & ltchars_buf);
#endif
    exit (0);
}

int send_int_sig() {
#ifndef __CYGWIN__
    kill(child_pid,SIGINT);
#endif
	return 0;
}

void get_pty_master()
{
    char    c;
    struct stat stb;
    int     i;

    if (ptyflag == 0) {
	master = 1;
	return;
    }
#if defined(__CYGWIN32__) || defined(__linux__) || defined(__APPLE__)
    sprintf (master_tty, "/dev/ptmx");
    master = open (master_tty, O_RDWR);
    if (master >= 0) {
#if defined(__linux__)
	char name[BUFSIZ];
	grantpt(master);
	unlockpt(master);
	if ( !ptsname_r(master,name,sizeof(name)) ) {
	    strcpy(slave_tty, name);
	    goto FOUND;
	}
#elif defined(__APPLE__)
        char *name;
	grantpt(master);
	unlockpt(master);
	if ( (name = (char *)ptsname(master))!=0 ) {
	    strcpy(slave_tty, name);
	    goto FOUND;
	}
#else
	char *name;
	name = (char *)ptsname(master);
	if ( name != 0 ) {
	    strcpy(slave_tty, name);
	    goto FOUND;
	}
#endif
    }
#else
    for (c = 'p'; c <= 's'; c++) {
	for (i = 0; i < 16; i++) {
	    sprintf (master_tty, "/dev/pty%c%x", c, i);
	    master = open (master_tty, O_RDWR);
	    if (master >= 0) {
		sprintf (slave_tty, "/dev/tty%c%x", c, i);
		goto FOUND;
	    }
	}
    }
#endif
    /*
     * Can't get master tty
     */
    if (master < 0) {
	fprintf (stderr, "Couldn't open pseudo tty\n");
	kill_process ();
	exit (1);
    }

 FOUND:
#ifdef TERMIOS
    tcgetattr(0, &initial_ttymode);
#elif defined(TIOCSETN)
    ioctl (0, TIOCGETP, (char *) &initial_ttymode);
    ioctl (0, TIOCGETC, (char *) &tchars_buf);
    ioctl (0, TIOCGETD, (char *) &line_desc);
    ioctl (0, TIOCGLTC, (char *) &ltchars_buf);
    ioctl (0, TIOCLGET, (char *) &lmode_buf);
#endif

#ifdef TIOCGWINSZ
    {
	struct winsize win;

	if (ioctl (0, TIOCGWINSZ, &win) >= 0) {
	    lines = win.ws_row;
	    columns = win.ws_col;
	}
    }
#endif

#ifdef KANJI
# if defined(TIOCKGET) && defined(TIOCKSET)
    ioctl (0, TIOCKGET, (char *) &kmode_buf);
# endif
# if defined(TIOCKGETC) && defined(TIOCKSETC)
    ioctl (0, TIOCKGETC, (char *) &jtchars_buf);
# endif
#endif /* KANJI */

    FD_ZERO(&stdinmask);
    FD_ZERO(&mastermask);
    FD_ZERO(&selectmask);
    FD_SET(fileno(stdin), &stdinmask);
    FD_SET(master, &mastermask);
    FD_SET(fileno(stdin), &selectmask);
    FD_SET(master, &selectmask);
    selectnfds = max (fileno(stdin), master) + 1;

    return;
}

void get_pty_slave()
{

    slave = open (slave_tty, 2);
    if (slave < 0) {
	perror (slave_tty);
	exit (1);
    }
#ifdef TERMIOS
    tcsetattr(slave, TCSANOW, &initial_ttymode);
#elif defined(TIOCSETN)
    ioctl (slave, TIOCSETN, (char *) &initial_ttymode);
    ioctl (slave, TIOCSETC, (char *) &tchars_buf);
    ioctl (slave, TIOCSLTC, (char *) &ltchars_buf);
    ioctl (slave, TIOCLSET, (char *) &lmode_buf);
    ioctl (slave, TIOCSETD, (char *) &line_desc);
#endif

#ifdef KANJI
# if defined(TIOCKGET) && defined(TIOCKSET)
    ioctl (slave, TIOCKSET, (char *) &kmode_buf);
# endif
# if defined(TIOCKGETC) && defined(TIOCKSETC)
    ioctl (slave, TIOCKSETC, (char *) &jtchars_buf);
# endif
#endif /* KANJI */

#ifdef TIOCSWINSZ
    {
	struct winsize win;

	win.ws_row = lines;
	win.ws_col = columns;
	(void) ioctl (slave, TIOCSWINSZ, &win);
    }
#endif
}

void recover_tty()
{
#ifdef TERMIOS
    tcsetattr(0, TCSANOW, &initial_ttymode);
#elif defined(TIOCSETN)
    ioctl (0, TIOCSETN, (char *) & initial_ttymode);
    ioctl (0, TIOCSETC, (char *) & tchars_buf);
    ioctl (0, TIOCSLTC, (char *) & ltchars_buf);
#endif
}

int suspend()
{
    long	pid;
#if defined(sun)
    void	(*func) (int);
#else
    void	(*func) __P((int));
#endif
#ifndef __CYGWIN32__
    int		omask;
#endif
    extern int	errno;

    pid = getpid ();
 /* reset signal handler so kill below stops us */
    func = signal (SIGCHLD, SIG_IGN);
    signal (SIGTSTP, SIG_DFL);
    recover_tty();
#define	mask(s)	(1 << ((s)-1))
#if !defined(__CYGWIN32__) && !defined(sun)
    omask = sigsetmask (sigblock (0) & ~mask (SIGTSTP));
#endif
    kill (0, SIGTSTP);

    if (kill (child_pid, SIGCONT) < 0 && errno == ESRCH) {
	printf ("Where my child has gone?!\n");
	terminate (0);
    }
    killpg (child_pid, SIGCONT);
    kill (0, SIGCONT);
    signal (SIGCHLD, func);
    signal (SIGTSTP, SIG_IGN);
#if !defined(__CYGWIN32__) && !defined(sun)
    sigblock (mask (SIGTSTP));
#endif
    fix_tty ();
    if (look_var ("auto-repaint"))
	fep_repaint(0);
}

void look_cmdinfo (char *command)
{
    struct cmdinfo *p;
    char *allocAndCopyThere(char *);

    if (strcmp (prompt, "") != 0)
	return;

    for (p = cmdinfo_tab; p->command; p++) {
	if (strcmp (command, p->command) == 0) {
	    prompt = allocAndCopyThere (p->prompt);
	    set_var ("prompt", p->prompt);
	    set_var ("delimiters", p->delimiters);
	    break;
	}
    }
}


void usageAndExit()
{

    printf ("Usage: %s [-emacs|-vi] command\n", myself);
    exit (1);
}

#ifdef SIGWINCH
/*
 * Propagate window size changes to the slave tty.
 */
void sigwinch(int dummy)
{
    char *itoa(int i);
#ifdef TIOCGWINSZ			/* 4.3BSD */
    struct winsize win;

    if (ioctl (0, TIOCGWINSZ, (char *)&win) >= 0) {
	(void) ioctl (slave, TIOCSWINSZ, (char *)&win);
	set_var ("crt", itoa (win.ws_row));
	set_var ("showhist", itoa (win.ws_row - 1));
    }
#else
#ifdef TIOCGSIZE			/* SONY NEWS OS2.2 */
    struct ttysize win;

    if (ioctl(0, TIOCGSIZE, (char *)&win) >= 0) {
	(void) ioctl(slave, TIOCSSIZE, (char *)&win);
	set_var ("crt", itoa (win.ts_lines));
	set_var ("showhist", itoa (win.ts_lines - 1));
    }
#endif /* TIOCGWINSZ */
#endif /* TIOCGSIZE */
}
#endif /* SIGWINCH */
