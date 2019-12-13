/*	Copyright (c) 1987, 1988 by Software Research Associates, Inc.	*/

#if defined(ANDROID)
#include <strings.h>
#define index(s,c) strchr(s,c)
#define rindex(s,c) strrchr(s,c)
#endif

#ifndef lint
static char rcsid[]=
"$Header$ (SRA)";
#endif /* lint */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
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
#include <ctype.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#if defined(sun)
#include <sys/fcntl.h>
#endif
#include "fep_defs.h"
#include "fep_glob.h"
#include "fep_funcs.h"

int	tty_fix_bell = OFF;	/* ring bell if the tty mode is changed */

typedef struct {
	int cur_line;
	int max_line;
} MORE;
MORE *create_more();

#if defined(ANDROID)
#define S_IREAD S_IRUSR
#define S_IWRITE S_IWUSR
#define S_IEXEC S_IXUSR
#endif
/*
 * Check command line if it call built-in function or not and execute it
 */
executeBuiltInFunction (comline, more)
    char *comline, **more;
{
    register FunctionTableEnt *ftp;
    char linebuf[MAXCMDLEN], *line;
    char *search_string();
    int argc;

    /*
     * Skip white space.
     */
    while (isspace (*comline))
	comline++;

    line = linebuf;
    strcpy (line, comline);

    if (more) {
	if (*more = search_string (comline, ";")) {
	    *(line + (*more - comline)) = '\0';
	    do {
		*more += 1;
	    } while (**more == ';');
	}
	else {
	    *more = (char*) 0;
	}
    }

    /*
     * Skip comment and blank line
     */
    if (*line == '#' || *line == '\0')
	return (IGNORED);

    /*
     * All built-in command should be prefixed by 'fep-'
     */
    if (strncmp (line, "fep-", 4) != 0)
	return (NOT_PROCESSED);

    for (ftp = BuiltinFuncTable; ftp->name; ftp++) {
	if (is_same_command (line, ftp->name)) {
	    if (debug)
		showArgs (line);
	    if (
		condition()
		/* control structure should be processed on any condition */
		|| strncmp (line + 4, "if", 2) == 0
		|| strncmp (line + 4, "endif", 5) == 0
		|| strncmp (line + 4, "elseif", 6) == 0
		|| strncmp (line + 4, "else", 4) == 0
	    )
		(*ftp->func)(line);
	    return (PROCESSED);
	}
    }
    return (NOT_PROCESSED);
}

is_same_command (a, b)
    register char *a, *b;
{

    while (*a && *b && *a == *b)
	++a, ++b;
    if ((*a == '\0' || isspace ((unsigned char)*a)) && (*b == '\0' || isspace ((unsigned char)*b)))
	return 1;
    else
	return 0;
}

/*
 * Process 'fep-if' and 'fep-elseif'
 */
fep_if (comline)
    char *comline;
{
    char *argv[MAXARGS];
    int argc;
    char *err;
    int cond;
    int i;

    argc = mkargv (comline, argv, MAXARGS);

    if (argc != 2 && argc != 4) {
	printf ("%s: Illegal number of arguments\n", argv[0]);
	return;
    }

    /*
     * In case of only one argument,
     * treat as true if the variable is set.
     */
    if (argc == 2) {
	char *cp;

	if (argv[1][0] == '$')
	    cp = &argv[1][1];
	else
	    cp = &argv[1][0];

	cond = look_var (cp) ? 1 : 0;
    }
    else {
	int value;

	/*
	 * Substitute variable prefixed by '$' mark.
	 */
	for (i = 0; i < argc; i++) {
	    if (argv[i][0] == '$') {
		char *v;

		if (v = look_var (&argv[i][1]))
		    argv[i] = v;
		else
		    argv[i] = "";
	    }
	}

	if (debug) {
	    int i;
	    char **argp;

	    for (i = 0, argp = argv; *argp; argp++, i++)
		printf ("argv[%d] = \"%s\"\n", i, *argp);
	}

        /*
	 * Check operator.
	 */
	if (eq (argv[2], "==") || eq (argv[2], "="))
	    value = 1;
	else if (eq (argv[2], "!="))
	    value = 0;
	else {
	    printf ("%s: Unknown opperator \"%s\"", argv[0], argv[2]);
	    return;
	}

	if (eq (argv[1], argv[3]))
	    cond = value;
	else
	    cond = !value;
    }

    if (eq (argv[0], "fep-elseif"))
	err = change_condition (cond);
    else
	err = push_condition (cond);

    if (err)
	printf ("%s: %s", argv[0], err);
    else if (debug)
	printf ("%s: %s\n", comline, cond ? "TRUE" : "FALSE");

    return;
}

fep_else ()
{
    char *err;

    if (err = change_condition (1))
	printf ("fep-else: %s", err);

    return;
}

fep_endif ()
{
    char *err;

    if (err = pop_condition ())
	printf ("fep-endif: %s", err);

    return;
}

bind_to_key (comline)
    char *comline;
{
    register FunctionTableEnt *fnte;
    char *argv[MAXARGS];
    int argc;

    argc = mkargv (comline, argv, MAXARGS);

    /*
     * Something error occured. Print message and return
     */
    if (argc < 0) {
	printf ("%s\n", argv[0]);
	return;
    }

    /*
     * Number of arguments should be three.
     */
    if (argc != 3) {
	printf ("Invalid number of arguments\n");
	return;
    }

    /*
     * Find the function name from function name table.
     */
    for (fnte = FunctionNameTable; fnte->func; fnte++) {
	if (strcmp (fnte->name, argv[1]) == 0) {
	    bind_key (curFuncTab, fnte->func, argv[2], fep_abort);
	    break;
	}
    }

    /*
     * Couldn't find such a function
     */
    if (fnte->func == NULL)
	printf ("%s: no such built-in command\n", argv[1]);
}

alias(comline)
    char *comline;
{
    char *argv[MAXARGS];
    int argc;

    argc = mkargv (comline, argv, MAXARGS);

    switch (argc) {
	case 1:
	    show_aliaslist (NULL);
	    break;

	case 2:
	    show_aliaslist (argv[1]);
	    break;

	case 3:
	    set_alias (argv[1], argv[2]);
	    break;

	default:
	    printf ("%s: Illegal number of arguments.\n", argv[0]);
    }
    return;
}

unalias (comline)
    char *comline;
{
    char *argv[MAXARGS];
    int argc;
    int i;

    argc = mkargv (comline, argv, MAXARGS);

    for (i=1; i<argc; i++)
	unset_alias (argv[i]);

    return;
}

set (comline)
    char *comline;
{
    char line[MAXCMDLEN];
#if defined(ANDROID)
    char *cp;
#else
    char *cp, *index();
#endif
    char *argv[MAXARGS];
    int argc;

    /*
     * If there is '=' character in command line, change it to space.
     */
    strcpy (line, comline);
    if (cp = index (line, '='))
	*cp = ' ';

    argc = mkargv (line, argv, MAXARGS);

    switch (argc) {

	/* set */
	case 1:
	    show_varlist ();
	    return;
	
	/* set var */
	case 2:
	    set_var (argv[1], "");
	    return;

	/* set var = val */
	case 3:
	    set_var (argv[1], argv[2]);
	    break;

	default:
	    printf ("Invalid number of arguments\n");
	    return;
    }
}

unset(comline)
    char *comline;
{
    char **vp;
    char *argv[MAXARGS];
    int argc;

    argc = mkargv (comline, argv, MAXARGS);

    if (argc < 2) {
	printf ("Invalid number of arguments\n");
	return;
    }

    for (vp = &argv[1]; *vp; vp++)
	unset_var (*vp);
}

extern	int Transparency;
extern	int Through;
#ifdef TERMIOS
#define ttystruct termios
#elif defined(TIOCSETN)
#define ttystruct sgttyb
#endif
struct	ttystruct master_ttymode;		/* master tty mode */
struct	ttystruct slave_ttymode;		/* slave tty mode */
extern	int master, slave;
extern	char slave_tty[];

/*
 * Toggle transparency switch.
 * If variable Transparency is ON, fep doesn't care about input.
 * If OFF, line editing will be done by fep.
 * But this Transparency is set automaticaly by getcharacter() routine,
 * if the variable auto-tty-fix is ON.
 */
toggle_through()
{
    int r;
    int slave_fd;
#ifdef TERMIOS
    struct termios s;
#else
    struct sgttyb s;
#endif

    if (Through == OFF) {

	slave_fd = open (slave_tty, O_WRONLY);
	if (slave_fd < 0) {
	    perror (slave_tty);
	    return;
	}

#ifdef TERMIOS
	r = tcgetattr(slave_fd, &s);
#else
	r = ioctl (slave_fd, TIOCGETP, (char *) &s);
#endif
	if (r < 0) {
	    perror (slave_tty);
	    (void) close (slave_fd);
	    return;
	}

#ifdef TERMIOS
	s.c_lflag &= ~(ICANON);
	s.c_cc[VMIN] = 1;
	s.c_cc[VTIME] = 0;
	r = tcsetattr(0, TCSANOW, &s);
#else
	s.sg_flags |= CBREAK;
	r = ioctl (0, TIOCSETN, (char *) & s);
#endif
	if (r < 0) {
	    perror (slave_tty);
	    (void) close (slave_fd);
	}
	(void) close (slave_fd);
    }
    else
#ifdef TERMIOS
	r = tcsetattr(0, TCSANOW, & master_ttymode);
#else
	r = ioctl (0, TIOCSETN, (char *) & master_ttymode);
#endif

    if (r < 0) {
	printf ("Can't change pty mode.\n");
	return;
    }

    Through = !Through;
}

/*
 * Check tty mode of slave tty and fix stdout tty mode
 */
fix_transparency()
{
    int r;
#ifdef TERMIOS
    struct termios s;
#else
    struct sgttyb s;
#endif

    if (Through)
	return;

    if (slave < 0)
	return;

#ifdef TERMIOS
    r = tcgetattr(slave, &s);
    s.c_iflag |= ICRNL;
    s.c_oflag |= ONLCR;
#else
    r = ioctl (slave, TIOCGETP, (char *) &s);
    /*
     * slave CRMOD is off, but master should be.
     */
    s.sg_flags |= CRMOD;
#endif
    if (r < 0) {
	perror (slave_tty);
	return;
    }

    /*
     * If find slave tty mode is cbreak or raw, fix tty mode of stdout to
     * same mode as slave and set Transparency ON.
     */

#ifdef TERMIOS
    if ((s.c_lflag & ICANON) == 0)
#else
    if (s.sg_flags & (CBREAK|RAW))
#endif
    {
	if (Transparency == OFF) {
#ifdef TERMIOS
	    r = tcsetattr(0, TCSANOW, & s);
#else
	    r = ioctl (0, TIOCSETN, (char *) & s);
#endif
	    if (r < 0) {
		perror ("stdout");
		return;
	    }
	    if (tty_fix_bell) errorBell ();
	    Transparency = ON;
	}
    }
    else {
	if (Transparency == ON) {
#ifdef TERMIOS
	    r = tcsetattr(0, TCSANOW, & master_ttymode);
#else
	    r = ioctl (0, TIOCSETN, (char *) & master_ttymode);
#endif
	    if (r < 0) {
		perror ("stdout");
		return;
	    }
	    if (tty_fix_bell) errorBell ();
	    Transparency = OFF;
	}
    }

    if (r < 0) {
	printf ("Can't change pty mode.\n");
	return;
    }
}

putch (c)
    int c;
{
    putchar (c);
    fflush (stdout);
}

/*
int crt, sline;
*/

show_bindings ()
{
    MORE *m;

    m = create_more(lines);
    if (!m) {
	errorBell ();
	return (0);
    }

    clear_edit_line ();
    (void) showBindingTbl (m, curFuncTab, "");
    recover_edit_line (1);

    destroy_more(m);
    return (0);
}

showBindingTbl (m, ft, prefix)
    MORE *m;
    FUNC ft[];
    char *prefix;
{
    register FunctionTableEnt *fnte;
    register int i;

    for (i = 0; i < 128; i++) {
	if (ft[i] == self_insert || ft[i] == fep_abort)
	    continue;

	/*
	 * If the pointer to function has indirect flag print "indirect".
	 */
	if (isIndirect(ft[i])) {
	    char pf[64];

	    sprintf (pf, "%s%s%c-",
		prefix, (i == 0 || isctlchar(i)) ? "^" : "", unctl(i));
	    if (showBindingTbl (m, maskIndirect(ft[i]), pf) == 0)
		break;
	    continue;
	}

	/*
	 * Search function name table
	 */
	for (fnte = FunctionNameTable; fnte->func; fnte++) {
	    if (ft[i] == fnte->func) {

		if (!more(m))
		    return (0);

		/*
		 * Show binding
		 */
		printf ("%s%s%c\t%s\n",
			prefix,
			i == 0 || isctlchar(i) ? "^" : "",
			unctl(i),
			fnte->name
			);
		break;
	    }
	}

	/*
	 * Can't find such a function
	 */
	if (fnte->func == NULL)
	    printf (
		"%s%c\tunknown function (0x%x)\n",
		i == 0 || isctlchar(i) ? "^" : "",
		unctl(i),
		ft[i]
	    );
    }
    return (1);
}

show_help ()
{
    MORE *m;

    m = create_more(lines);
    if (m == 0) {
	errorBell ();
	return;
    }

    clear_edit_line ();

    more(m)				&&
    (printf ("Functions:\n") || 1)	&&
    showTable (m, FunctionNameTable)	&&
    more(m)				&&
    (printf ("Commands:\n") || 1)	&&
    showTable (m, BuiltinFuncTable)	&&
    more(m)				&&
    (printf ("Variables:\n") || 1)	&&
    showVariables (m);

    recover_edit_line (1);
}

showTable (m, fnte)
    MORE *m;
    FunctionTableEnt *fnte;
{
    int i;

    /*
     * Search function name table
     */
    for (; fnte->func; fnte++) {
	if (!more(m))
	    return (0);
	printf ("\t%-30s %s\n", fnte->name, fnte->help);
    }

    return (1);
}

showVariables (m)
    MORE *m;
{
    extern VAR default_set_vars[], default_unset_vars[];
    VAR *vp;

    for (vp = default_set_vars; vp->v_name; ++vp) {
	if (!vp->v_help)
	    continue;
	if (!more(m))
	    return (0);
	printf ("\t%-30s %s\n", vp->v_name, vp->v_help);
    }

    for (vp = default_unset_vars; vp->v_name; ++vp) {
	if (!vp->v_help)
	    continue;
	if (!more(m))
	    return (0);
	printf ("\t%-30s %s\n", vp->v_name, vp->v_help);
    }
    return (1);
}

MORE *create_more(maxline)
    int maxline;
{
    MORE *mp;

    mp = (MORE *) malloc (sizeof (MORE));

    if (mp == 0)
	return ((MORE*)0);
    else {
	mp->cur_line = 0;
	mp->max_line = maxline;
	return (mp);
    }
}

destroy_more(mp)
    MORE *mp;
{
    if (mp)
	free (mp);
}

more (mp)
    MORE *mp;
{

    /*
     * Print more message
     */
#   define PUTMORE	printf (  "--More--");
#   define DELMORE	printf ("\r        \r");
    if (mp->max_line && ++mp->cur_line >= mp->max_line) {

	PUTMORE;
	fflush (stdout);
	
	switch (getcharacter()) {
	case '\n': case '\r': case 'j':
	    --mp->cur_line;
	    break;

	case 'd': case ctrl('D'):
	    mp->cur_line /= 2;
	    break;

	case 'q': case 'Q': case ctrl('C'):
	    DELMORE;
	    return (0);

	default:
	    mp->cur_line = 1;
	    break;
	}
	DELMORE;
    }

    if (mp->cur_line == 1 && look_var ("clear-repaint") && term_clear)
	tputs (term_clear, 1, putch);

    return (1);
}

/*
 * Change directory
 */
fep_chdir (line)
char *line;
{
    char *argv[MAXARGS];
    int argc;

    switch (mkargv (line, argv, MAXARGS)) {

	/*
	 * Change directory with no arguments cause to chdir to home directory
	 */
        case 1: {
	    char *home, *getenv();

	    if (home = getenv ("HOME"))
		argv[1] = home;
	    else {
		printf ("Where is your home directory?\n");
		return;
	    }
	    break;
	}

	/*
	 * Change directory command with argument
	 */
	case 2:
	    break;

	/*
	 * Something error occured in mkargv.
	 */
	case -1:
	    printf ("%s\n", argv[0]);
	    return;

	default:
	    printf ("fep-chdir: Invalid number of arguments.\n");
	    return;
    }

    /*
     * Chane directory.
     * Keep in mind that end process still in old directory
     */
    if (chdir (argv[1]) < 0) {
	perror (argv[1]);
	return;
    }
}

fep_pwd (line)
    char *line;
{
    char cwd[MAXPATHLEN];

    (void) getcwd (cwd, sizeof(cwd));
    printf ("%s\n", cwd);
}

fep_echo (comline)
    char *comline;
{
    char *argv[MAXARGS];
    int argc;
    char **argp;
    int putnewline = 1, first;

    argc = mkargv (comline, argv, MAXARGS);
    
    argp = &argv[1];
    if (*argp && strcmp (*argp, "-n") == 0) {
	putnewline = 0;
	++argp;
    }

    for (first = 1; *argp; argp++) {
	char *cp;

	/*
	 * Put space
	 */
	if (! first)
	    printf ("%s", " ");

	/*
	 * Print argument
	 */
	if (**argp == '$' && (cp = look_var (*argp + 1)))
	    printf ("%s", cp);
	else
	    printf ("%s", *argp);

	first = 0;
    }

    if (putnewline)
	printf ("%c", '\n');
}

fep_command (comline)
    char *comline;
{
    char *argv[MAXARGS];
    int argc;
    int i;
    char **argp;
    char buf[256];

    argc = mkargv (comline, argv, MAXARGS);

    if (argc == 1) {
	printf ("Invalid number of arguments.\n");
	return;
    }

    strcpy (buf, "");
    for (i=1; i<argc; i++) {
	strcat (buf, argv[i]);
	strcat (buf, " ");
    }

    invoke_command (buf);
}

fep_source (comline)
    char *comline;
{
    FILE *fp;
    static char *argv[MAXARGS];
    char file [MAXPATHLEN];
    int argc;

    argc = mkargv (comline, argv, MAXARGS);

    if (argc != 2) {
	printf ("Invalid number of arguments.\n");
	return;
    }

    strcpy (file, argv[1]);
    source_file (file);

    return;
}

sourceRcFile ()
{
    char *home, filename[64], *getenv();
    char line[256];
    struct stat stb_home, stb_cur;

    if (!(home = getenv ("HOME")))
	return;

    strcpy (filename, home);
    strcat (filename, "/.feprc");

    /*
     * Source .feprc in home directory.
     */
    stb_home.st_ino = 0;
    if (stat (filename, &stb_home) >= 0 && (stb_home.st_mode&S_IREAD))
	source_file (filename);

    /*
     * Source .feprc in current directory.
     */
    if ((stat (".feprc", &stb_cur) >= 0 && stb_cur.st_ino != stb_home.st_ino))
	source_file (".feprc");

    return;
}

source_file (file)
    char *file;
{
    FILE *fp;
    char line[512], line2[512];
    int i = 0;

    if ((fp = fopen (file, "r")) == NULL) {
	perror (file);
	return;
    }

    while (fgets (line, 256, fp)) {
	i++;
	if (executeBuiltInFunction (line, 0) == NOT_PROCESSED) {
	    char *cp = line;

	    while (isspace (*cp))
		cp++;
	    strcpy (line2, "fep-");
	    strcat (line2, cp);
	    if (executeBuiltInFunction (line2, 0) == NOT_PROCESSED) {
		printf ("\"%s\", line %d: cannot be executed\n", file, i);
		printf (">>> %s", line);
	    }
	}
    }
	
    fclose (fp);
    return;
    
}

#define MAX_IF_NEST 10

#define CONDITION_MASK	0x00ff
#define HAS_BEEN_TRUE	0x0100

int condition_stack [MAX_IF_NEST] = {1};
int current_if_stack = 0;

condition ()
{
    int cond = 1, i;

    if (current_if_stack == 0)
	return (1);

    for (i = 1; i <= current_if_stack; i++)
	cond &= condition_stack [i];

    return (cond & CONDITION_MASK);
}

char *
change_condition (cond)
    int cond;
{
    if (debug)
	printf ("old=0x%x, new=0x%x\n",
	    condition_stack [current_if_stack],	cond);
    if (current_if_stack > 0) {
	if (condition_stack [current_if_stack] & HAS_BEEN_TRUE)
	    cond = 0;
	else if (cond != 0)
	    condition_stack [current_if_stack] |= HAS_BEEN_TRUE;

	condition_stack [current_if_stack] &= ~CONDITION_MASK;
	condition_stack [current_if_stack] |= cond;
	return ((char *)0);
    }
    else
	return ("Not in if close\n");
}

char *
push_condition (cond)
    int cond;
{
    if (current_if_stack < MAX_IF_NEST){
	++current_if_stack;
	condition_stack [current_if_stack] = cond;
	if (cond == 1)
	    condition_stack [current_if_stack] |= HAS_BEEN_TRUE;
	return ((char*)0);
    }
    else
	return ("If stack over flow\n");
}

char *
pop_condition ()
{

    if (current_if_stack > 0) {
	--current_if_stack;
	return ((char*)0);
    }
    else
	return ("No more if stack\n");
}

invoke_shell ()
{
    char *shell = "/bin/sh";

    if (look_var ("shell"))
	shell = look_var ("shell");

    invoke_command (shell);
}

invoke_command (cmd)
    char *cmd;
{
    int catchsig();
    int (*func)();

    clear_edit_line ();
    if (look_var ("verbose"))
	printf ("Invoke %s\n", cmd);
    fflush (stdout);
    recover_tty();
    recover_signal ();
    system (cmd);
    fix_signal ();
    fix_tty();
    if (look_var ("verbose"))
	printf ("Return to fep\n");
    recover_edit_line (1);
}

FILE *redirect_fp = NULL;
int redirect_line = 0;

fep_read_from_file (comline)
    char *comline;
{
    FILE *fp;
    static char *argv[MAXARGS];
    char file [MAXPATHLEN];
    int argc;

    argc = mkargv (comline, argv, MAXARGS);

    if (argc != 2) {
	printf ("Invalid number of arguments.\n");
	return;
    }

    if (redirect_fp) {
	fclose (redirect_fp);
	redirect_fp = NULL;
    }

    redirect_fp = fopen (argv[1], "r");

    if (redirect_fp = fopen (argv[1], "r")) {
	if (look_var ("verbose"))
	    printf ("Input redirected from %s\n", argv[1]);
	errorBell ();
	redirect_line = 0;
    }
    else
	perror (argv[0]);

    return;
}

/*
 * Process ID of the process redirecting from.
 */
int redirect_pid = 0;

fep_read_from_command (comline)
    char *comline;
{
    static char *argv[MAXARGS];
    char buf[256];
    int argc;
    int i;
    FILE *popen();

    argc = mkargv (comline, argv, MAXARGS);

    if (argc == 1) {
	printf ("Invalid number of arguments.\n");
	return;
    }

    if (redirect_fp) {
	fclose (redirect_fp);
	redirect_fp = NULL;
    }

    strcpy (buf, "");
    for (i=1; i<argc; i++) {
	strcat (buf, argv[i]);
	strcat (buf, " ");
    }

    if (redirect_fp = popen (buf, "r")) {
	if (look_var ("verbose"))
	    printf ("Input redirected from %s\n", argv[1]);
	errorBell ();
	redirect_line = 0;
    }
    else
	perror (argv[0]);

    return;
}

char script_file[128];

fep_start_script (comline)
    char *comline;
{
    char *name;

    /*
     * Caution!! If editstatus is EDITING, comline argument is never passed.
     */
    if (editstatus == NOTEDITING) {
	int argc;
	static char *argv[MAXARGS];
	char buf[256];

	argc = mkargv (comline, argv, MAXARGS);
	switch (argc) {

	    case 1:
		name = look_var ("script-file");
		break;

	    case 2:
		name = argv[1];
		break;

	    default:
		printf ("%s: Illegal number of arguments.\n", argv[0]);
	}
    }
    else
	name = look_var ("script-file");

    /*
     * If script is running alread, reatun.
     */
    if (script_fp) {
	clear_edit_line ();
	errorBell ();
	printf ("script is already running.\n");
	recover_edit_line (1);
	return;
    }

    if (!name) {
	clear_edit_line ();
	printf ("script-file is not set.\n");
	recover_edit_line (1);
	return;
    }

    if ((script_fp = fopen (name, "a")) == NULL) {
	clear_edit_line ();
	perror (name);
	recover_edit_line (1);
	return;
    }

    strncpy (script_file, name, sizeof (script_file));

    clear_edit_line ();
    printf ("script start (file=\"%s\").\n", script_file);
    recover_edit_line (1);
}

fep_end_script ()
{
    if (!script_fp) {
	clear_edit_line ();
	errorBell ();
	printf ("script is not started.\n");
	return;
    }

    fclose (script_fp);
    script_fp = NULL;

    clear_edit_line ();
    printf ("script end (file=\"%s\").\n", script_file);
    recover_edit_line (1);

    return;
}

fep_repaint(comline)
    char *comline;
{
    int i;
    int line;
    CHAR ch;
    char *crt_hight;
    BUFFER *bp = output_buffer;

    /*
     * Caution!! If editstatus is EDITING, comline argument is never passed.
     */
    if (editstatus == NOTEDITING && comline) {
	int argc;
	static char *argv[MAXARGS];
	char buf[256];

	argc = mkargv (comline, argv, MAXARGS);
	switch (argc) {

	    case 1:
		crt_hight = look_var ("crt");
		break;

	    case 2:
		crt_hight = argv[1];
		break;

	    default:
		printf ("%s: Illegal number of arguments.\n", argv[0]);
	}
    }
    else
	crt_hight = look_var ("crt");

    line = atoi (crt_hight);

    clear_edit_line ();
    
    ch = buf_char (bp, -1);
    for (i = -1; ; --i) {
	if ((ch = buf_char(bp, i)) == '\n')
	    --line;
	if (ch == (CHAR)-1 || line <= 0)
	    break;
    }
    i += 1;

    if (look_var("clear-repaint") && term_clear)
	tputs (term_clear, 1, putch);

    for (; i < 0; i++)
	putchar (buf_char(bp, i));

    /*
     * In this case, prompt should not be printed.
     * Saving prompt is ugly solution, but...
     */
    recover_edit_line (0);
    fflush (stdout);
}

view_buffer (comline)
    char *comline;
{
    BUFFER *bp = output_buffer;
    MORE *m;
    int maxchar = buf_count (bp);
    int i;
    char c;

    /*
     * Skip first line
     */
    for (i = 0; (c = buf_char (bp, i)) != (char)-1 && c != '\n'; i++)
	;
    i += 1;

    if (c == -1)
	return (0);

    if (!(m = create_more (lines)))
	return (0);

    clear_edit_line ();
    for (; i < maxchar; i++) {
	if (!more(m))
	    break;
	while ((c = buf_char (bp, i)) != (char)-1 && c != '\n') {
	    putchar (c);
	    i++;
	}
	if (c == '\n')
	    putchar ('\n');
    }

    /*
     * If all output is shown, prompt not to be shown.
     */
    if (i < maxchar)
	recover_edit_line (1);
    else
	recover_edit_line (0);

    destroy_more(m);
    return (0);
}

#ifdef STAT

#include "fep_stat.h"

long stat_obyte = 0;
long stat_ibyte = 0;
long stat_rerror = 0;
long stat_werror = 0;
long stat_nselect = 0;

struct statistics stat_info[] = {
    "Command Output",	&stat_obyte,
    "Command Input ",	&stat_ibyte,
    "Read error    ",	&stat_rerror,
    "Write error   ",	&stat_werror,
    "Select count  ",	&stat_nselect,
    NULL, NULL
};

fep_showstat ()
{
    struct statistics *sp = stat_info;
    BUFFER *bp = output_buffer;

    printf ("I/O and system calls:\n");
    for (sp = stat_info; sp->info_name; sp++) {
	printf ("\t%s: %d\n", sp->info_name, *(sp->info_valp));
    }

    printf ("\nI/O Buffer:\n");
    printf ("\tMax  : %d\n", bp->b_max);
    printf ("\tHiWtr: %d\n", bp->b_hiwater);
    printf ("\tCount: %d\n", bp->b_count);
    printf ("\tNext : %d\n", bp->b_next);

}
#endif
