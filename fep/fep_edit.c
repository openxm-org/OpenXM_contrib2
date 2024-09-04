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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
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
#include <sys/time.h>
#include <ctype.h>
#if defined(__CYGWIN__)
#include <sys/dirent.h>
#define direct dirent
#elif defined(sun) || defined(__FreeBSD__) || defined(ANDROID)
#include <dirent.h>
#define direct dirent
#include <unistd.h>
#else
#include <sys/dir.h>
#endif
#include <sys/file.h>
#include <setjmp.h>
#include "fep_defs.h"
#include "fep_glob.h"
#include "fep_funcs.h"

CHAR		*CommandLine;		/* pointer to command line buffer */
CHAR		*KillBuffer;		/* pointer to kill buffer */
int		CurrentPosition;	/* current cursor position */
int		MarkPosition = -1;	/* Marked position */
EDITMODE	editmode = NOTYET;	/* edtimode EMACS, VI */
EDITSTATUS	editstatus = NOTEDITING;/* EDITING, NOTEDITING */

int	maxline = MAXCMDLEN;		/* maximum length of command line */
int	NeedNewLine;			/* add new line flag */
int	NeedSave;			/* need to save to history */
int	Transparency = OFF;		/* transparent flag */
int	Through = OFF;			/* through flag */
int	eof_occured = 0;		/* eof has occurred */
jmp_buf	jbuf;				/* jump buffer */

FUNC	*curFuncTab;			/* current function table */
FUNC	*altFuncTab;			/* alternative function table */

void ls (DIR *dirp, char *prefixstring);

/*
 * Default binding table
 */
BINDENT emacsBindings[] = {
	/* NULL		*/	{"\0",		(FUNC)mark},
	/* ^A		*/	{"\\^A",	(FUNC)beginning_of_line},
	/* ^B		*/	{"\\^B",	(FUNC)backward_character},
	/* ^D		*/	{"\\^D",	(FUNC)delete_next_character},
	/* ^E		*/	{"\\^E",	(FUNC)end_of_line},
	/* ^F		*/	{"\\^F",	(FUNC)forward_character},
	/* ^I		*/	{"\\^I",	(FUNC)insert_tab},
	/* ^J		*/	{"\\^J",	(FUNC)new_line},
	/* ^K		*/	{"\\^K",	(FUNC)kill_to_end_of_line},
	/* ^L		*/	{"\\^L",	(FUNC)clear_screen},
	/* ^M		*/	{"\\^M",	(FUNC)new_line},
	/* ^N		*/	{"\\^N",	(FUNC)next_history},
	/* ^P		*/	{"\\^P",	(FUNC)previous_history},
	/* ^T		*/	{"\\^T",	(FUNC)previous_history},
	/* ^Y		*/	{"\\^Y",	(FUNC)yank_from_kill_buffer},
	/* ^^		*/	{"\\^^",	(FUNC)toggle_through},
	/* esc-b	*/	{"\\^[b",	(FUNC)backward_word},
	/* esc-B	*/	{"\\^[B",	(FUNC)backward_Word},
	/* esc-d	*/	{"\\^[d",	(FUNC)delete_next_word},
	/* esc-D	*/	{"\\^[D",	(FUNC)delete_next_Word},
	/* esc-f	*/	{"\\^[f",	(FUNC)forward_word},
	/* esc-F	*/	{"\\^[F",	(FUNC)forward_Word},
	/* esc-h	*/	{"\\^[h",	(FUNC)delete_previous_word},
	/* esc-H	*/	{"\\^[H",	(FUNC)delete_previous_Word},
	/* esc-l	*/	{"\\^[l",	(FUNC)list_file_name},
	/* esc-L	*/	{"\\^[L",	(FUNC)list_file_name},
	/* esc-n	*/	{"\\^[n",	(FUNC)insert_next_history},
	/* esc-N	*/	{"\\^[N",	(FUNC)insert_next_history},
	/* esc-p	*/	{"\\^[p",	(FUNC)insert_previous_history},
	/* esc-P	*/	{"\\^[P",	(FUNC)insert_previous_history},
	/* esc-.	*/	{"\\^[.",	(FUNC)insert_current_history},
    	/* esc-esc	*/	{"\\^[\\^[",	(FUNC)expand_file_name},
	/* esc-"-"	*/	{"\\^[-",	(FUNC)toggle_through},
	/* esc-_	*/	{"\\^[_",	(FUNC)invoke_shell},
	/* esc-<	*/	{"\\^[<",	(FUNC)search_reverse},
	/* esc->	*/	{"\\^[>",	(FUNC)search_forward},
	/* esc-?	*/	{"\\^[?",	(FUNC)show_help},
	/* ^X-^B	*/	{"\\^X\\^B",	(FUNC)show_bindings},
	/* ^X-B		*/	{"\\^XB",	(FUNC)show_bindings},
	/* ^X-b		*/	{"\\^Xb",	(FUNC)show_bindings},
	/* ^X-^H	*/	{"\\^X\\^H",	(FUNC)show_history},
	/* ^X-H		*/	{"\\^XH",	(FUNC)show_history},
	/* ^X-h		*/	{"\\^Xh",	(FUNC)show_history},
	/* ^X-^V	*/	{"\\^X\\^V",	(FUNC)view_buffer},
	/* ^X-^K	*/	{"\\^X\\^K",	(FUNC)kill_to_top_of_line},
	/* ^X-^L	*/	{"\\^X\\^L",	(FUNC)fep_repaint},
	/* ^X-^C	*/	{"\\^X\\^C",	(FUNC)terminate},
	/* ^X-^D	*/	{"\\^X\\^D",	(FUNC)send_eof},
	/* ^X-(		*/	{"\\^X(",	(FUNC)fep_start_script},
	/* ^X-)		*/	{"\\^X)",	(FUNC)fep_end_script},
	/*		*/	{NULL,		NULL}
};

/*
 * Initialize function table buffer
 */
void init_bind_table ()
{

    curFuncTab = (FUNC *) calloc (sizeof (FUNC), 256);
    altFuncTab = (FUNC *) calloc (sizeof (FUNC), 256);
    if (curFuncTab == 0 || altFuncTab == 0) {
	printf ("Can't allocate space for function table\n");
	exit (1);
    }
}

/*
 * Initialize function table
 */
char strspace [256], *strspace_addr = strspace;

void init_edit_params ()
{
    struct indirect *idp;
    char *cp, *getenv();
    char sbuf[2048], *term, *tgetstr();

    if (term = look_var ("term")) {
	if (tgetent (sbuf, term) == 1) {
	    if (lines == 0)
		lines = tgetnum ("li");
	    if (columns == 0)
		columns = tgetnum ("co");
	    term_clear = tgetstr ("cl", &strspace_addr);
	}
    }

    if (lines == 0)
	lines = 24;
    if (columns == 0)
	columns = 80;

    sprintf (sbuf, "%d", lines);
    set_var ("crt", sbuf);

    if (editmode == NOTYET) {
	if (look_var ("editmode"))
	    set_var ("editmode", look_var ("editmode"));
	else
	    set_var ("editmode", "emacs");
    }

    sprintf (sbuf, "%d", lines);
    set_var ("showhist", sbuf);

    /*
     * Read startup file
     */
    sourceRcFile ();

    /*
     * Read history if 'history-file' is set
     */
    {
	char *cp, *mk_home_relative ();

	if (look_var ("savehist") && (cp = look_var ("history-file"))) {
	    cp = mk_home_relative (cp);

	    if (access (cp, R_OK) == 0)
		read_history (cp);
	}
    }

    /*
     * Initialize command line buffer
     */
    CommandLine = (CHAR *) calloc (maxline, 1);
    KillBuffer = (CHAR *) calloc (maxline, 1);
    if (CommandLine == 0 || KillBuffer == 0) {
	perror ("Edit line buffer");
	exit (1);
    }
    (void) strcpy (CommandLine, "");
    (void) strcpy (KillBuffer, "");
    CurrentPosition = 0;
}

/*
 * Initialize emacs bindings
 */
void initEmacsBindings (FUNC cft[], FUNC aft[])
{
    register int i;
    BINDENT *ftp;

    for (i = 0; i < 256; i++)
	cft[i] = (FUNC)self_insert;

#define import(table,key,fn) if((int)key>0)table[(int)key]=fn

#ifdef TERMIOS
    /* Now, using cbreak mode
    import (cft, initial_ttymode.c_cc[VSTART], (FUNC)ignore);
    import (cft, initial_ttymode.c_cc[VSTOP], (FUNC)ignore);
    */
#if 0
    import (cft, initial_ttymode.c_cc[VINTR], (FUNC)insert_and_flush);
#else
	/* added by noro */
    import (cft, initial_ttymode.c_cc[VINTR], (FUNC)send_int_sig);
	/* XXX : this may not be necessary */
    import (aft, initial_ttymode.c_cc[VINTR], (FUNC)send_int_sig);
#endif
    import (cft, initial_ttymode.c_cc[VQUIT], (FUNC)insert_and_flush);
    /* Now, EOF will be sent on empty line.
    import (cft, initial_ttymode.c_cc[VEOF], (FUNC)send_eof);
    */
#ifdef VSWTC
    import (cft, initial_ttymode.c_cc[VSWTC], (FUNC)insert_and_flush);
#endif
    import (cft, initial_ttymode.c_cc[VSUSP], (FUNC)insert_and_flush);
    /* ^Y is used for yank-from-kill-buffer
    import (cft, initial_ttymode.c_cc[VDSUSP], (FUNC)self_insert);
    */
    import (cft, initial_ttymode.c_cc[VREPRINT], (FUNC)reprint);
#ifdef VDISCARD
    import (cft, initial_ttymode.c_cc[VDISCARD], (FUNC)self_insert);
#endif
    import (cft, initial_ttymode.c_cc[VWERASE], (FUNC)delete_previous_word);
    import (cft, initial_ttymode.c_cc[VLNEXT], (FUNC)literal_next);
    import (cft, initial_ttymode.c_cc[VERASE], (FUNC)delete_previous_character);
    import (cft, initial_ttymode.c_cc[VKILL], (FUNC)delete_line);
#else
    /* Now, using cbreak mode
    import (cft, tchars_buf.t_startc, (FUNC)ignore);
    import (cft, tchars_buf.t_stopc, (FUNC)ignore);
    */
#if 0
    import (cft, tchars_buf.t_intrc, insert_and_flush);
#else
    import (cft, tchars_buf.t_intrc, send_int_sig);
    import (aft, tchars_buf.t_intrc, send_int_sig);
#endif
    import (cft, tchars_buf.t_quitc, insert_and_flush);
    /* Now, EOF will be sent on empty line.
    import (cft, tchars_buf.t_eofc, send_eof);
    */
    import (cft, tchars_buf.t_brkc, insert_and_flush);
    import (cft, ltchars_buf.t_suspc, insert_and_flush);
    /* ^Y is used for yank-from-kill-buffer
    import (cft, ltchars_buf.t_dsuspc, self_insert);
    */
    import (cft, ltchars_buf.t_rprntc, reprint);
    import (cft, ltchars_buf.t_flushc, self_insert);
    import (cft, ltchars_buf.t_werasc, delete_previous_word);
    import (cft, ltchars_buf.t_lnextc, literal_next);
    import (cft, initial_ttymode.sg_erase, delete_previous_character);
    import (cft, initial_ttymode.sg_kill, delete_line);
#endif

#undef import

    /*
     * Set default bindings
     */
    for (ftp = emacsBindings; ftp->bt_s; ftp++) {
	bind_key (cft, ftp->bt_func, ftp->bt_s, fep_abort);
    }
}

/*
 * Main function of front end program
 */
CHAR * mygetline()
{
    int c;
    CHAR *execute_command, *check_alias();
    char *remained;
    int processed;

    (void) strcpy (CommandLine, "");
    CurrentPosition = 0;
    resetCurrentHistory ();

RETRY:
    (void) fflush (stdout);
    NeedNewLine = 0;
    NeedSave = 0;
    editstatus = EDITING;
    processed = 0;
    (void) setjmp (jbuf);

    /*
     * If there is file pointer for I/O redirection,
     * read input from the pointer and return
     */
    if (redirect_fp) {

	/*
	 * Check output from sub-process
	 */
	swallow_output();

	if (fgets (CommandLine, MAXCMDLEN, redirect_fp)) {
	    ++redirect_line;
	    execute_command = CommandLine;
	    goto RETURN;	    
	}
	else {
	    if (look_var ("verbose"))
		printf ("%d lines redirected\n", redirect_line);
	    errorBell ();
	    if (redirect_pid) {
		pclose (redirect_fp);
		redirect_pid = 0;
	    }
	    else
		fclose (redirect_fp);
	    redirect_fp = NULL;
	}
    }

    while ((c = getcharacter ()) >= (CHAR) 0) {
	int status;

	/*
	 * In transparet mode
	 */
	if (Through == ON || Transparency == ON) {
	    if (*curFuncTab[(int) c] == toggle_through ||
	        *curFuncTab[(int) c] == fix_transparency) {
		(*curFuncTab[(int) c])();
		goto RETRY;
	    }
	    else {
		CommandLine[CurrentPosition] = c;
		CommandLine[CurrentPosition + 1] = '\0';
		return (CommandLine);
	    }
	}

	/*
	 * If find EOF character on top of empty line
	 * and the variable "send-eof-on-empty-line" is set
	 * call send_eof
	 */
	if (
#ifdef TERMIOS
	    c == initial_ttymode.c_cc[VEOF]
#else
	    c == tchars_buf.t_eofc
#endif
	    && curFuncTab[c] != send_eof
	    && ! look_var ("ignore-eof")
	    && CommandLine [0] == '\0'
	) {
	    if (!eof_occured && look_var ("alarm-on-eof")) {
		eof_occured = 1;
		status = alarm_on_eof (c);		
	    }
	    else
		status = send_eof (c);
	}
	else {
	    eof_occured = 0;
	    status = callfunc (curFuncTab, c);
	}

	(void) fflush (stdout);
	if (status == 1)	/* end of line editing */
	    break;
	else			/* continue line editing */
	    continue;
    }
    editstatus = NOTEDITING;

 REPROCESS:

    /*
     * Check command line refer history or not
     */
    if (refer_history () == PROCESSED)
	goto RETRY;

    /*
     * Check alias list
     */
    if (!processed && !look_var ("noalias")
	&& (execute_command = check_alias (CommandLine))
    ) {
	if (look_var ("verbose"))
	    printf ("%s\n", execute_command);
    }
    else
	execute_command = CommandLine;
	

    /*
     * Check builtin function, and execute it.
     */
    if (executeBuiltInFunction (execute_command, &remained) == PROCESSED) {
	if (!processed)
	    addHistory (CommandLine);
	CommandLine[0] = '\0';
	CurrentPosition = 0;
	if (remained && *remained) {
	    strcpy (CommandLine, remained);
	    processed = 1;
	    goto REPROCESS;
	}
	if (!redirect_fp)
	    fputs (prompt, stdout);
	goto RETRY;
    }

    if (NeedSave) {
	if (!is_empty_line (CommandLine) || !look_var ("ignore-empty-line"))
	    if (!processed)
		addHistory (CommandLine);
	/*
	 * put string to buffer
	 */
	buf_put (output_buffer, CommandLine);
	if (NeedNewLine == 1)
	    buf_put (output_buffer, "\n");
    }

    if (NeedNewLine == 1)
	(void) strcat (execute_command, "\n");

RETURN:
    return (execute_command);
}

/*
 * Invoke appropriate function according to function table
 * Return value 1 means exit from line editing
 */
int callfunc (FUNC ft[], int c)
{

    if (isIndirect(ft[(int) c])) {
	FUNC *nft = maskIndirect (ft[(int)c]);
	char nc;

	nc = (CHAR) getcharacter();
	return (callfunc (nft, nc));
    }
    else
	return ((*ft[(int) c]) (c));
}

/*
 * Beep and do nothing
 */
int fep_abort()
{
    (void) errorBell ();
    return (0);
}

/*
 * Insert the character self
 */
int self_insert(CHAR c)
{
    register int i, nbyte = 1, currentNull;
#ifdef KANJI
    CHAR byte2;
#endif /* KANJI */

    currentNull = strlen (CommandLine);

    if (currentNull >= maxline) {
        errorBell();
	return (0);
    }

    if (isctlchar(CommandLine[CurrentPosition]))
	(void) putchar (BS);
#ifdef KANJI
    if (iskanji (c)) {
	byte2 = (CHAR) getcharacter ();
	putchar (c);
	putchar (byte2);
	nbyte = 2;
    }
    else
#endif /* KANJI */
	putChar (c);
    reverse_strcpy (
	&CommandLine[CurrentPosition] + nbyte,
	&CommandLine[CurrentPosition]
    );
    CurrentPosition += nbyte;
    CommandLine[CurrentPosition - nbyte] = c;
#ifdef KANJI
    if (nbyte > 1) {
	CommandLine[CurrentPosition - 1] = byte2;
    }
#endif /* KANJI */
    printS (&CommandLine [CurrentPosition]);

    if (CommandLine[CurrentPosition] != '\0') {
	repeat (BS, howlong (&CommandLine[CurrentPosition + 1], 0));
	(void) putchar (BS);
    }
    return (0);
}

/*
 * Insert string in current position
 */
int insert_string (CHAR *s)
{
    register int i, nbyte = strlen (s), currentNull;

    currentNull = strlen (CommandLine);

    if (currentNull + nbyte >= maxline - 1) {
        errorBell();
	return (0);
    }

    if (isctlchar(CommandLine[CurrentPosition]))
	(void) putchar (BS);
    printS (s);
    reverse_strcpy (
	&CommandLine[CurrentPosition] + nbyte,	/* to */ 
	&CommandLine[CurrentPosition]		/* from */
    );
    bcopy (s, &CommandLine[CurrentPosition], nbyte);
    CurrentPosition += nbyte;

    if (CommandLine[CurrentPosition] != '\0') {
	printS (&CommandLine[CurrentPosition]);
	repeat (BS, howlong (&CommandLine[CurrentPosition], 0));
    }
    return (0);
}

/*
 * Yank string from kill buffer.
 */
void yank_from_kill_buffer ()
{
    insert_string (KillBuffer);
}

/*
 * Set mark to current position
 */
int mark ()
{
    set_mark (CurrentPosition);
    return (0);
}

/*
 * Set mark to specified position
 */
int set_mark (int pos)
{
    MarkPosition = pos;
    return (0);
}

/*
 * Delete area from mark to current position to kill buffer
 */
int delete_to_kill_buffer ()
{
    int n = abs (CurrentPosition - MarkPosition);

    if (MarkPosition < 0) {
	errorBell();
	return (0);
    }

    if (CurrentPosition == MarkPosition)
	return;

    if (CurrentPosition > MarkPosition) {
	(void) moveto (MarkPosition);
    }

    return (delete_next_n_character (n));
}

/*
 * Move to specified position.
 */
void moveto (int position)
{
    if (position < CurrentPosition)
	while (position < CurrentPosition)
	    (void) backward_n_character (1);
    else
	while (position > CurrentPosition
#ifdef KANJI
	    && !(CurrentPosition + 1 == position
		    && iskanji (CommandLine[CurrentPosition]))
#endif /* KANJI */
	)
	    (void) forward_n_character (1);
}

/*
 * Move cursor to top of line
 */
int beginning_of_line()
{
    register int i;

    for (i = CurrentPosition; i > 0; i--) {
	if (isctlchar (CommandLine[i]))
	    (void) putchar (BS);
	(void) putchar (BS);
    }
    CurrentPosition = 0;
    return (0);
}

#ifdef KANJI
#define INC(i) if(iskanji(CommandLine[i])) i+=2; else i++;
#define DEC(i) if(i>=2 && iskanji_in_string(CommandLine, i-2)) i-=2; else i--;
#else /* KANJI */
#define INC(i) i++
#define DEC(i) i--
#endif /* KANJI */

/*
 * Move cursor to end of line
 */
int end_of_line()
{
    register int    i;

    for (i = CurrentPosition; CommandLine[i]; i++) {
	if (isctlchar (CommandLine[i]))
	    (void) putchar (unctl(CommandLine[i]));
	else
	    (void) putchar (CommandLine[i]);
	if (isctlchar(CommandLine[i+1]))
	    (void) putchar ('^');
    }
    CurrentPosition = i;
    return (0);
}

/*
 * Move cursor left one space
 */
int backward_character()
{

    return (backward_n_character (1));
}

/*
 * Move cursor left "n" space
 */
int backward_n_character(int n)
{
    int space;
    int i = CurrentPosition;

    if (n == 0)
	return (0);
    else if (n < 0)
	return (forward_n_character (-n));
    else if (n > CurrentPosition) {
	errorBell ();
	return (0);
    }

    while (n-- && i >= 0) {
	if (isctlchar (CommandLine[i]))
	    putchar (BS);
#ifdef KANJI
	if (i > 0 && iskanji_in_string (CommandLine, i-2)) {
	    (void) putchar (BS);
	    (void) putchar (BS);
	    i--;
	}
	else
#endif /* KANJI */
	putchar (BS);
	i--;
    }
    CurrentPosition = i;
    return (0);
}

/*
 * Move cursor backward one word
 */
int backward_word ()
{

    return (backward_n_word (1));
}

/*
 * Move cursor backward n word
 */
int backward_n_word (int n)
{
    register int i = CurrentPosition, nchars = 0;

    if (i == 0) {
	errorBell ();
	return (0);
    }

#ifdef KANJI
    while (n--) {
	DEC(i);
	nchars++;

	while (1) {
	    if (i >= 2 && iskanji_in_string (CommandLine, i-2))
		break;
	    else if (i > 0 && !iswordchar (CommandLine[i]))
		i -= 1;
	    else
		break;
	    
	    nchars++;
	}
	while (1) {
	    if (i >= 2 && iskanji_in_string (CommandLine, i - 2))
		i -= 2;
	    else if (i > 0 && iswordchar (CommandLine [i -1]))
		i -= 1;
	    else
		break;
	    nchars++;
	}
    }
#else /* KANJI */
    while (n--) {
	i--, nchars++;
	while (i > 0 && !iswordchar (CommandLine [i])) {
	    i--, nchars++;
	}
	while (i > 0 && iswordchar (CommandLine [i - 1])) {
	    i--, nchars++;
	}
    }
#endif /* KANJI */
    return (backward_n_character (nchars));
}

/*
 * Move cursor backward one Word
 */
int backward_Word ()
{

    return (backward_n_Word (1));
}

/*
 * Move cursor backward n Word
 */
int backward_n_Word (int n)
{
    register int i = CurrentPosition, nchars = 0;

    if (i == 0) {
	errorBell ();
	return (0);
    }

#ifdef KANJI
    while (n--) {
	DEC(i);
	nchars++;

	while (1) {
	    if (i > 1 && iskanji_in_string (CommandLine, i - 2))
		break;
	    else if (i > 0 && !iswordchar (CommandLine[i]))
		i -= 1;
	    else
		break;
	    
	    nchars++;
	}
	while (1) {
	    if (i > 2 && iskanji_in_string (CommandLine, i - 2))
		i -= 2;
	    else if (i > 0 && iswordchar (CommandLine [i - 1]))
		i -= 1;
	    else
		break;
	    nchars++;
	}
    }
#else /* KANJI */
    while (n--) {
	i--, nchars++;
	while (i > 0 && !isWordchar (CommandLine [i]))
	    i--, nchars++;
	while (i > 0 && isWordchar (CommandLine [i - 1]))
	    i--, nchars++;
    }
#endif /* KANJI */
    return (backward_n_character (nchars));
}

/*
 * Move cursor forward one character
 */
int forward_character()
{

    return (forward_n_character (1));
}

/*
 * Move cursor forward n character
 */
int forward_n_character(int n)
{
    int space;
    register int i = CurrentPosition;

    if (n == 0)
	return (0);
    else if (n < 0)
	return (backward_n_character (-n));

    if (CommandLine [CurrentPosition] == '\0') {
	errorBell ();
	return (0);
    }

#ifdef KANJI
    if (iskanji (CommandLine[i])) {
	(void) putchar (CommandLine[i++]);
	(void) putchar (CommandLine[i++]);
    }
    else
#endif /* KANJI */
    if (isctlchar(CommandLine[i])) {
	(void) putchar (unctl (CommandLine [i]));
	i++;
    }
    else
	(void) putchar (CommandLine[i++]);

    while (--n && CommandLine [i]) {
#ifdef KANJI
	if (iskanji (CommandLine[i])) {
	    (void) putchar (CommandLine[i++]);
	    (void) putchar (CommandLine[i++]);
	}
	else
#endif /* KANJI */
	putChar (CommandLine [i++]);
    }

    if (isctlchar (CommandLine [i]))
	putchar ('^');

    CurrentPosition = i;

    return (0);
}

/*
 * Move cursor forward one word
 */
int forward_word ()
{
    return (forward_n_word (1));
}

/*
 * Move cursor forward n word
 */
int forward_n_word (int n)
{
    register int i = CurrentPosition, nchars = 0;

    if (CommandLine [i] == '\0') {
	errorBell ();
	return (0);
    }

    while (n--) {
	while (CommandLine [i] && iswordchar (CommandLine [i])) {
	    INC(i);
	    nchars++;
	}
	while (CommandLine [i] && !iswordchar (CommandLine [i])) {
	    INC(i);
	    nchars++;
	}
    }
    return (forward_n_character (nchars));
}

/*
 * Move cursor forward one word
 */
int forward_Word ()
{
    return (forward_n_Word (1));
}

/*
 * Move cursor forward n word
 */
int forward_n_Word (int n)
{
    register int i = CurrentPosition, nchars = 0;

    if (CommandLine [i] == '\0') {
	errorBell ();
	return (0);
    }

    while (n--) {
	while (CommandLine [i] && isWordchar (CommandLine [i])) {
	    INC(i);
	     nchars++;
	}
	while (CommandLine [i] && !isWordchar (CommandLine [i])) {
	    INC(i);
	     nchars++;
	}
    }
    return (forward_n_character (nchars));
}

/*
 * Forward to end of word
 */
int forward_to_end_of_word ()
{

    return (forward_to_end_of_n_word (1));
}

/*
 * Forward to end of n word
 */
int forward_to_end_of_n_word (int n)
{    
    register int i = CurrentPosition, nchars = 0;

    if (CommandLine [i] == '\0') {
	errorBell ();
	return (0);
    }

    INC(i);
    while (n--) {
	while (CommandLine [i] && !iswordchar (CommandLine [i])) {
	    INC(i);
	    nchars++;
	}
	while (CommandLine [i] && iswordchar (CommandLine [i])) {
	    INC(i);
	    nchars++;
	}
    }
    DEC(i);
    return (forward_n_character (nchars));
}

/*
 * Forward to end of word
 */
int forward_to_end_of_Word ()
{

    return (forward_to_end_of_n_Word (1));
}

/*
 * Forward to end of n word
 */
int forward_to_end_of_n_Word (int n)
{    
    register int i = CurrentPosition, nchars = 0;

    if (CommandLine [i] == '\0') {
	errorBell ();
	return (0);
    }

    INC(i);
    while (n--) {
	while (CommandLine [i] && !isWordchar (CommandLine [i])) {
	    INC(i);
	    nchars++;
	}
	while (CommandLine [i] && isWordchar (CommandLine [i])) {
	    INC(i);
	    nchars++;
	}
    }
    DEC(i);
    return (forward_n_character (nchars));
}

/*
 * Delete previous one character
 */
int delete_previous_character()
{

    return (delete_previous_n_character (1));
}

/*
 * Delete previous n characters
 */
int delete_previous_n_character(int n)
{
    register int i, nbyte;
    int deleteArea, restArea;

    if (CurrentPosition < n) {
	errorBell ();
	return (0);
    }

#ifdef KANJI
    for (nbyte = 0, i = CurrentPosition; n-- > 0 && i;)
	if (i > 1 && iskanji_in_string (CommandLine, i - 2))
	    i -= 2, nbyte += 2;
	else
	    i -= 1, nbyte += 1;
#else /* KANJI */
    nbyte = n;
#endif /* KANJI */

    deleteArea = howlong (&CommandLine[CurrentPosition - nbyte], nbyte);
    restArea = howlong (&CommandLine[CurrentPosition], 0);
    if (isctlchar(CommandLine[CurrentPosition]))
	(void) putchar (BS);
    repeat(BS, deleteArea);
    CurrentPosition -= nbyte;
    strcpy (KillBuffer, &CommandLine [CurrentPosition]);
    KillBuffer [nbyte] = '\0';
    for (i = CurrentPosition; CommandLine[i + nbyte]; i++) {
	CommandLine[i] = CommandLine[i + nbyte];
	putChar (CommandLine[i]);
    }
    CommandLine[i] = '\0';
    repeat(SP, deleteArea);
    repeat(BS, deleteArea);
    if (isctlchar (CommandLine[CurrentPosition]))
	repeat(BS, restArea - 1);
    else
	repeat(BS, restArea);
    
    return (0);
}

/*
 * Delete previous one word
 */
int delete_previous_word()
{
    
    return (delete_previous_n_word (1));
}

/*
 * Delete previous n word
 */
int delete_previous_n_word(int n)
{
    register int i = CurrentPosition, nchars = 0;

    if (i == 0) {
	errorBell();
	return (0);
    }

#ifdef KANJI
    while (n--) {
	if (i>1 && iskanji_in_string (CommandLine, i-2))
	    i--;
	i--, nchars++;

	while (1) {
	    if (i > 1 && iskanji_in_string (CommandLine, i-2))
		break;
	    else if (i > 0 && !iswordchar (CommandLine[i]))
		i -= 1;
	    else
		break;
	    
	    nchars++;
	}
	while (1) {
	    if (i > 2 && iskanji_in_string (CommandLine, i - 2))
		i -= 2;
	    else if (i > 0 && iswordchar (CommandLine [i -1]))
		i -= 1;
	    else
		break;
	    nchars++;
	}
    }
#else /* KANJI */
    while (n--) {
	i--, nchars++;
	while (i > 0 && !iswordchar (CommandLine [i]))
	    i--, nchars++;
	while (i > 0 && iswordchar (CommandLine [i - 1]))
	    i--, nchars++;
    }
#endif /* KANJI */

    return (delete_previous_n_character (nchars));
}

/*
 * Delete previous one word
 */
int delete_previous_Word()
{
    
    return (delete_previous_n_Word (1));
}

/*
 * Delete previous n word
 */
int delete_previous_n_Word(int n)
{
    register int i = CurrentPosition, nchars = 0;

    if (i == 0) {
	errorBell();
	return (0);
    }

#ifdef KANJI
    while (n--) {
	if (i>1 && iskanji_in_string (CommandLine, i-2))
	    i--;
	i--, nchars++;

	while (1) {
	    if (i > 1 && iskanji_in_string (CommandLine, i-2))
		break;
	    else if (i > 0 && !isWordchar (CommandLine[i]))
		i -= 1;
	    else
		break;
	    
	    nchars++;
	}
	while (1) {
	    if (i > 2 && iskanji_in_string (CommandLine, i - 2))
		i -= 2;
	    else if (i > 0 && isWordchar (CommandLine [i -1]))
		i -= 1;
	    else
		break;
	    nchars++;
	}
    }
#else /* KANJI */
    while (n--) {
	i--, nchars++;
	while (i > 0 && !isWordchar (CommandLine [i]))
	    i--, nchars++;
	while (i > 0 && isWordchar (CommandLine [i - 1]))
	    i--, nchars++;
    }
#endif /* KANJI */

    return (delete_previous_n_character (nchars));
}

/*
 * Delete next one character
 */
int delete_next_character ()
{

    return (delete_next_n_character (1));
}

/*
 * Delete next n character
 */
int delete_next_n_character (int n)
{
    register int i, nbyte;
    int deleteArea, restArea;

    if (strlen (&CommandLine [CurrentPosition]) < n) {
	errorBell ();
	return (0);
    }

#ifdef KANJI
    {
	register CHAR *cp = &CommandLine[CurrentPosition];

	nbyte = 0;
	while (n-- > 0 && *cp)
	    if (iskanji (*cp))
		cp += 2, nbyte += 2;
	    else
		cp++, nbyte++;
    }
#else /* KANJI */
    nbyte = n;
#endif /* KANJI */

    deleteArea = howlong (&CommandLine[CurrentPosition], nbyte);
    restArea = howlong (&CommandLine[CurrentPosition + nbyte], 0);
    if (isctlchar(CommandLine[CurrentPosition]))
	(void) putchar (BS);
    strcpy (KillBuffer, CommandLine + CurrentPosition);
    KillBuffer [nbyte] = '\0';
    for (i = CurrentPosition; CommandLine[i + nbyte]; i++) {
	CommandLine[i] = CommandLine[i + nbyte];
	putChar (CommandLine[i]);
    }
    CommandLine[i] = '\0';
    repeat(SP, deleteArea);
    repeat(BS, deleteArea);
    if (isctlchar (CommandLine[CurrentPosition]))
	repeat(BS, restArea - 1);
    else
	repeat(BS, restArea);
    
    return (0);
}

/*
 * Delete next one word
 */
int delete_next_word ()
{
    return (delete_next_n_word (1));
}

/*
 * Delete next n word
 */
int delete_next_n_word (int n)
{
    register int i = CurrentPosition, nchars = 0;

    if (CommandLine [i] == '\0') {
	errorBell ();
	return (0);
    }

    while (n--) {
	while (CommandLine [i] && iswordchar (CommandLine [i])) {
	    INC(i);
	    nchars++;
	}
	while (CommandLine [i] && !iswordchar (CommandLine [i])) {
	    INC(i);
	    nchars++;
	}
    }
    return (delete_next_n_character (nchars));
}

/*
 * Delete next one word
 */
int delete_next_Word ()
{
    return (delete_next_n_Word (1));
}

/*
 * Delete next n word
 */
int delete_next_n_Word (int n)
{
    register int i = CurrentPosition, nchars = 0;

    if (CommandLine [i] == '\0') {
	errorBell ();
	return (0);
    }

    while (n--) {
	while (CommandLine [i] && isWordchar (CommandLine [i])) {
	    INC(i);
	    nchars++;
	}
	while (CommandLine [i] && !isWordchar (CommandLine [i])) {
	    INC(i);
	    nchars++;
	}
    }
    return (delete_next_n_character (nchars));
}

/*
 * Erase whole line
 */
int delete_line()
{
    register int i = CurrentPosition;
    register int len;

    len = howlong (CommandLine, 0);

    /*
     * If cursor is there right part of line, move it to end of line
     * and erase character by character from end
     */
    if (howlong (CommandLine, CurrentPosition + 1) > len / 2) {
	(void) end_of_line ();
	repeat_string ("\b \b", len);
    }
    /*
     * If cursor is there on left part of line, move it to top of line
     * and erase line at once
     */
    else {
	(void) beginning_of_line ();
	if (isctlchar (CommandLine[0]))
	    putchar (BS);
	repeat (SP, len);
	repeat (BS, len);
    }
    strcpy (KillBuffer, CommandLine);
    CurrentPosition = 0;
    CommandLine [0] = '\0';
    return (0);
}

/*
 * Delete characters from current position to top of line
 */
int kill_to_top_of_line()
{
    int i = CurrentPosition;

    (void) beginning_of_line();
    return (delete_next_n_character (i));
}

/*
 * Delete characters from current position to end of line
 */
int kill_to_end_of_line()
{
    register int    i, backCnt = 0;

    if (isctlchar(CommandLine[CurrentPosition])) {
        (void) putchar(BS);
    }
    for (i = CurrentPosition; CommandLine[i]; i++) {
	if (isctlchar(CommandLine[i])) {
	    fputs("  ", stdout);
	    backCnt++;
	}
	else
	    (void) putchar (SP);
	backCnt++;
    }
    for (; backCnt; backCnt--)
	(void) putchar (BS);
    strcpy (KillBuffer, CommandLine + CurrentPosition);
    CommandLine[CurrentPosition] = '\0';
    return (0);
}

/*
 * Insert tab to current cursor position
 */
int insert_tab()
{

    /* sorry, not implemented */
    return (self_insert ('\t'));
}

/*
 * Process new line
 */
int new_line()
{

    (void) end_of_line;
    fputs ("\r\n", stdout);
    NeedNewLine = 1;
    NeedSave = 1;

    return (1);
}

/*
 * Check current position is top-of-line
 */
int is_tol()
{
    return (CurrentPosition == 0);
}

/*
 * Check current position is end-of-line
 */
int is_eol()
{
    return (CommandLine [CurrentPosition] == '\0');
}

/*
 * Check command line if it refer history or not
 */
int refer_history()
{
    char   *historyExtract ();
    char   *his;

    if (CommandLine[0] != '!')
	return (NOT_PROCESSED);

    if (his = historyExtract (CommandLine)) {
	(void) strcpy (CommandLine, his);
	CurrentPosition = strlen (CommandLine);
    }
    else {
	fputs (CommandLine, stdout);
	fputs (" : Event not found.\r\n", stdout);
	(void) strcpy (CommandLine, "");
	CurrentPosition = 0;
    }
    fputs (prompt, stdout);
    print_com_line ();
    return (PROCESSED);
}

#define FORWARD	    1
#define REVERSE	    2

int search_reverse ()
{
    return (search_history (REVERSE));
}

int search_forward ()
{
    return (search_history (FORWARD));
}

int search_history (int direct)
{
    char *his, *search_reverse_history(), *search_forward_history();
    char *(*func)();

    if (direct == FORWARD)
	func = search_forward_history;
    else
	func = search_reverse_history;

AGAIN:
    if (his = (*func) (0)) {
	if (eq (his, CommandLine))
	    goto AGAIN;

	(void) delete_line ();
	(void) insert_string (his);
	CurrentPosition = strlen (CommandLine);
    }
    else {
	(void) errorBell ();

	if (look_var ("verbose")) {
	    char *cp;

	    if ((cp = look_var ("search-string")) == NULL)
		cp = "";
	    (void) clear_edit_line ();
	    fprintf (stdout, "\"%s\": No match.\r\n", cp);
	    (void) recover_edit_line (1);
	}
    }

    return 0;
}


/*
 * Insert the character and flush buffer
 */
int insert_and_flush(char c)
{
    (void) self_insert (c);
    return (1);
}

/*
 * Insert the character, but it means EOL. Therefore move cursor backward and
 * flush buffer
 */
int send_eof()
{
#ifdef TERMIOS
    char c = initial_ttymode.c_cc[VEOF];
#else
    char c = tchars_buf.t_eofc;
#endif

    (void) self_insert (c);
    if (isctlchar (c))
	fputs ("\b\b", stdout);
    else
	fputs ("\b", stdout);
    return (1);
}

/*
 * Alarm for EOF on only the first time finding eof character
 */
int alarm_on_eof ()
{

    errorBell ();
    (void) clear_edit_line ();
    printf ("EOF -- one more eof character to send eof to the process.\n");
    (void) recover_edit_line (1);
}

/*
 * Clear screen
 */
int clear_screen()
{

    if (term_clear) {
	(void) clear_edit_line ();
	fputs (term_clear, stdout);
	(void) recover_edit_line (1);
    }
    else
	errorBell ();

    return (0);
}

typedef enum {HOP_INSERT, HOP_REPLACE} HISTOP;
typedef enum {HDIR_PREV, HDIR_CURRENT, HDIR_NEXT} HISTDIR;

int serv_history(HISTOP op, HISTDIR dir);

/*
 * Get next history entry
 */
int next_history()
{
    return (serv_history (HOP_REPLACE, HDIR_NEXT));
}

/*
 * Get next history entry
 */
int previous_history()
{
    return (serv_history (HOP_REPLACE, HDIR_PREV));
}

/*
 * Insert next history entry
 */
int insert_current_history()
{
    return (serv_history (HOP_INSERT, HDIR_CURRENT));
}

/*
 * Insert next history entry
 */
int insert_next_history()
{
    return (serv_history (HOP_INSERT, HDIR_NEXT));
}

/*
 * Insert next history entry
 */
int insert_previous_history()
{
    return (serv_history (HOP_INSERT, HDIR_PREV));
}

/*
 * Get previous history
 */
int serv_history(HISTOP op, HISTDIR dir)
{
    register char *cp;
    char *getPreviousHistory (), *getNextHistory (), *getCurrentHistory ();
    int diff;

    switch (dir) {
    case HDIR_PREV: cp = getPreviousHistory (); break;
    case HDIR_NEXT: cp = getNextHistory (); break;
    case HDIR_CURRENT: cp = getCurrentHistory (); break;
    default: cp = (char*)0;
    }

    if (cp == (char *)0) {
	errorBell ();
	return (0);
    }

    if (op == HOP_REPLACE)
	(void) delete_line ();
    (void) insert_string (cp);
    return (0);
}

/*
 * Show history
 */
void show_history()
{

    (void) clear_edit_line ();
    hist_showHistory (lookd_var ("showhist"));
    (void) recover_edit_line (1);
}

/*
 * Do nothing
 */
int ignore()
{
    return(0);
}

/*
 * Next character is literal
 */
int literal_next()
{

    return (self_insert (getcharacter ()));
}

/*
 * Reprint command line
 */
int reprint()
{

    (void) clear_edit_line ();
    (void) recover_edit_line (1);
    return (0);
}

/*
 * Print whole command line and move cursor to the current position
 */
void print_com_line()
{

    printS (CommandLine);
    if (CommandLine[CurrentPosition] != '\0') {
	repeat (BS, howlong(&CommandLine[CurrentPosition + 1], 0));
	(void) putchar (BS);
    }
}

/*
 * Calculate space of string using "^" for control character
 */
int howlong(char *s, int n)
{
    register char *sp;
    register int area = 0;

    if (n == 0)
	n = -1;
    for (sp = s; *sp && n; sp++, n--) {
	if (isctlchar(*sp))
	    area += 2;
	else
	    area += 1;
    }
    return (area);
}

/*
 * Repeat putting character n times
 */
void repeat(char c, int n)
{
    for (n = n; n; n--)
	(void) putchar(c);
}

/*
 * Repeat putting string n times
 */
void repeat_string(char *s, int n)
{
    for (n = n; n; n--)
	fputs(s, stdout);
}

/*
 * Expand file name
 */
int expand_file_name ()
{
    CHAR *cp, *start_expand;
    char *x_dirname();
    char dir[256];
    char *fileList[256];
    CHAR line[256];
    DIR *dirp, *x_opendir();
    struct direct *dp;
    int found = 0;
    int i;
    int tilde_expanded = 0;
#if !defined(ANDROID)
    CHAR *index(), *rindex();
#endif
    
    if (delimiters == NULL)
        delimiters = DEFAULT_DELIMITERS;
    strcpy (line, CommandLine);
    for (cp = &line[CurrentPosition] - 1; cp > line ; --cp) {
        if (index (delimiters, *(cp - 1)))
	    break;
    }
    start_expand = cp;
    if (any ('/', cp)) {
	(void) strcpy (dir, cp);
	*(rindex (dir, '/') + 1) = '\0';
	cp = (CHAR *) (rindex (cp, '/') + 1);
    }
    else
	strcpy (dir, ".");

    if ((dirp = x_opendir (dir)) == NULL) {
	errorBell ();
	return (0);
    }

    for (dp = readdir(dirp), i = 0; dp != NULL; dp = readdir(dirp)) {

	if (*cp == '\0')
	    break;

	if (*cp != '.' && *(dp->d_name) == '.')
	    continue;

	if (prefix (cp, dp->d_name)) {
	    char *fcp;

# ifdef ALLOCA
  	    fcp = (char *) alloca (strlen (dp->d_name) + 1);
# else /* ALLOCA */
  	    fcp = (char *) malloc (strlen (dp->d_name) + 1);
# endif /* ALLOCA */
	    if (fcp == 0) {
		fputs ("\r\n", stdout);
		perror ("alloca:");
		reprint ();
		longjmp (jbuf, 1);
	    }
	    strcpy (fcp, dp->d_name);
	    fileList [i++] = fcp;
	}
    }
    fileList [i] = (char *) 0;

    if (*start_expand == '~' && look_var ("expand-tilde")) {
	char buf [256], *p;

	strcpy (buf, start_expand);
	p = x_dirname (buf);
	if (!eq (p, buf)) {
	    (void) moveto (start_expand - line);
	    (void) kill_to_end_of_line ();
	    (void) insert_string (x_dirname (buf));
	    tilde_expanded = 1;
	}
    }

    switch (i) {

	case 0:
	    if (tilde_expanded == 0)
		errorBell ();
	    break;

	case 1:
	    (void) end_of_line ();
	    (void) insert_string (fileList[0] + strlen (cp));
	    break;

	default:
	    {
		char *one, *other;
		char *c1, *c2;
		int i;

		one = fileList [0];
		for (i = 1; other = fileList [i]; i++) {
		    for (c1 = one, c2 = other; *c1 && *c2; c1++, c2++)
			if (*c1 != *c2)
			    break;
		    *c1 = '\0';
		}
		errorBell ();
		(void) end_of_line ();
		(void) insert_string (fileList[0] + strlen (cp));
	    }
	    break;
    }

# ifndef ALLOCA
    for (i = 0; fileList [i]; i++)
	free (fileList [i]);
# endif /* ALLOCA */

    closedir(dirp);
    return (0);
}

/*
 * List file name
 */
int list_file_name ()
{
    CHAR *cp;
    char dir[256];
    DIR *dirp, *x_opendir();
#if !defined(ANDROID)
    CHAR *index(), *rindex();
#endif

    if (delimiters == NULL)
        delimiters = DEFAULT_DELIMITERS;
    for (cp = (CHAR *) index (CommandLine, '\0'); cp > CommandLine ; --cp) {
        if (index (delimiters, *(cp - 1))) {
	    break;
	}
    }
#ifdef RINFO
    /*
     * This is an experimental code for rinfod, which serves remote
     * information about file, process, etc.
     */
    if (any (':', cp)) {
	char *host, *pattern;

	(void) strcpy (dir, cp);
	host = dir;
	*(index (host, ':')) = '\0';
	pattern = (index (cp, ':') + 1);;

	(void) clear_edit_line();
	list_remote_file (host, pattern);
	(void) recover_edit_line (1);
	return;
    }
#endif
    if (any ('/', cp)) {
	(void) strcpy (dir, cp);
	*(rindex (dir, '/') + 1) = '\0';
	cp = (CHAR *) (rindex (cp, '/') + 1);
    }
    else
	strcpy (dir, ".");


    if ((dirp = x_opendir (dir)) == NULL) {
	errorBell ();
	return (0);
    }

    (void) clear_edit_line();
    ls (dirp, cp);
    closedir(dirp);
    (void) recover_edit_line (1);

    return (0);
}

int	rememberPosition;

void clear_edit_line ()
{

    if (editstatus == NOTEDITING)
	return;

    rememberPosition = CurrentPosition;
    (void) end_of_line ();
    (void) fputs ("\r\n", stdout);
}

void recover_edit_line (int put_prompt)
{

    if (editstatus == NOTEDITING)
	return;

    if (rememberPosition < 0)
	return;
    CurrentPosition = rememberPosition;
    rememberPosition = -1;
    if (put_prompt && prompt)
	fputs (prompt, stdout);
    (void) print_com_line();
}

#define MAXFILES 256

/*
 * Do ls
 */
void ls (DIR *dirp, char *prefixstring)
{
    struct direct *dp;
    char *fileList[MAXFILES + 1];
    int i, j;
    int maxlen = 0;
    int files;		/* number of file */
    int fpl;		/* file par line */
    int cpf;		/* column par file */
    int fpc;		/* file par column */
    int COL = 80;
    char format[10];
    
    for (dp = readdir(dirp), i = 0; dp != NULL; dp = readdir(dirp)) {
	char *fcp;

	if (i >= MAXFILES) {
	    printf ("Too many files\n");
	    break;
	}

	if (dp->d_ino == (unsigned long) 0
    		|| (prefixstring[0] != '.' && dp->d_name[0] == '.'))
	    continue;
	if (prefix (prefixstring, dp->d_name)) {
# ifdef ALLOCA
  	    fcp = (char *) alloca (strlen (dp->d_name) + 1);
# else /* ALLOCA */
  	    fcp = (char *) malloc (strlen (dp->d_name) + 1);
# endif /* ALLOCA */
	    if (fcp == 0) {
		fputs ("\r\n", stdout);
		perror ("alloca:");
		reprint ();
		longjmp (jbuf, 1);
	    }
	    strcpy (fcp, dp->d_name);
	    fileList [i++] = fcp;
	    maxlen = max (maxlen, strlen (dp->d_name));
	}
    }
    fileList [i] = (char *) 0;

    if (i == 0)
	goto BACK;

    files = i;

    qsort (fileList, i, sizeof (char *), (int (*)(const void *,const void *))scmp);

    if (debug)
	printf ("%d files\n", files);

    cpf = maxlen + 1;
    fpl = COL / cpf;
    if (fpl == 0) fpl = 1;
    fpc = (files + fpl - 1) / fpl;

    sprintf (format, "%%-%ds", cpf);
    for (i = 0; i < fpc; i++) {
	for (j = 0; j < fpl - 1; j++)
	    if (i + j * fpc < files)
		printf (format, fileList[i + (j * fpc)]);
	if (i + (fpc * (fpl - 1)) < files)
	    printf ("%s", fileList[i + (fpc * (fpl - 1))]);
	fputs ("\n", stdout);
    }

BACK:

# ifndef ALLOCA
    for (i = 0; fileList [i]; i++)
	free (fileList [i]);
# endif /* ALLOCA */
    return;
}

#ifdef RINFO

#include "../rinfo/rinfo.h"

void list_remote_file (char *host, char *pattern)
{
    struct slist *slp, *getfilelist();
    int i, j;
    char format [64];
    int COL = 80;
    int  maxlen, cpf, fpl, fpc;
    int files;

    slp = getfilelist (host, pattern);
    files = slp->s_cnt;

    COL = 80;
    maxlen = 0;
    for (i = 0; i < files; i++) {
	int len;

	if ((len = strlen (*(slp->s_list + i))) > maxlen)
	    maxlen = len;
    }

    cpf = maxlen + 1;
    fpl = COL / cpf;
    if (fpl == 0)
	fpl = 1;
    fpc = (files + fpl - 1) / fpl;

    sprintf (format, "%%-%ds", cpf);
    for (i = 0; i < fpc; i++) {
	for (j = 0; j < fpl - 1; j++) {
	    if (i + j * fpc < slp->s_cnt)
		printf (format, *(slp->s_list + (i + (j * fpc))));
	}
	if (i + (fpc * (fpl - 1)) < files)
	    printf ("%s", slp->s_list[i + (fpc * (fpl - 1))]);
	fputs ("\n", stdout);
    }
}
#endif /* RINFO */

/*
    FUNC ft[];		Function table
    FUNC func;		Function to be binded
    char *s;		String to bind
    FUNC dfunc;		Default function for table allocating
*/

int bind_key (FUNC ft[], FUNC func, char *s, FUNC dfunc)
{
    char tmps[16];

    if (s[0] == '\\' && s[1] == '^' && s[2] != '\0') {
	tmps[0] = toctrl (s[2]);
	strcpy (&tmps[1], &s[3]);
	s = tmps;
    }

    /*
     * If the string contain only one character, put the function to
     * appropriate position in the table.
     */
    if (*(s+1) == '\0') {
	if (isIndirect (ft[(int) *s]))
	    free (maskIndirect (ft[(int) *s]));

	ft[(int) *s] = func;
	return (1);
    }
    else {
	FUNC *nft = (FUNC *) 0;
	int status;

	/*
	 * If the entry doesn't have indirect function table, allocate it.
	 */
	if (! (isIndirect (ft[(int) *s]))) {
	    register int i;

	    nft = (FUNC *) calloc (sizeof (FUNC), 256);
	    /*
	     * If failed in allocating, return 0.
	     */
	    if (nft == 0)
		return (0);

	    /*
	     * Initialize the table with default function
	     */
	    for (i = 0; i < 256; i++)
		nft [i] = dfunc;
	    ft[(int) *s] = setIndirect (nft);
	}

	status = bind_key (maskIndirect (ft[(int) *s]), func, s+1, dfunc);
	/*
	 * If the binding failed and the table was allocated in this function
	 * free the table and return with same status.
	 */
	if (status == 0 && nft)
	    free (nft);
	return (status);
    }
}
