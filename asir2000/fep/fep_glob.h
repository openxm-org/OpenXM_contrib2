/*	Copyright (c) 1987, 1988 by Software Research Associates, Inc.	*/

#ifndef lint
# define FEP_GLOB \
    "$Header$ (SRA)"
#endif /* lint */

extern FUNC	*curFuncTab;	/* function table */
extern FUNC	*altFuncTab;	/* altanative function table */
extern int	debug;		/* debug flag */
extern char	*prompt;	/* prompt string */
extern char	*delimiters;	/* delimiter characters */
extern EDITMODE	editmode;	/* edit mode */
extern FILE	*redirect_fp;	/* FILE pointer for I/O redirection */
extern FILE	*script_fp;	/* FILE pointer for script */
extern int	redirect_line;	/* number of line for redirecting */
extern int	redirect_pid;	/* process id redirecting from */
extern int	auto_tty_fix;	/* fix tty mode automatically */
extern int	tty_fix_bell;	/* ring bell when tty mode is changed */
extern int	Transparency;	/* transparent flag */
extern int	Through;	/* through flag */
extern EDITSTATUS   editstatus;	/* edit status */
extern BUFFER	*output_buffer;	/* output buffer */

extern int	lines, columns;	/* terminal sizes */
extern char	*term_clear;	/* terminal clear code */

/*
 * tty control caracters.
 * defined in fep_main.c
 */
#ifdef TERMIOS
extern struct termios initial_ttymode;
#elif defined(TIOCSETN)
extern struct tchars tchars_buf;
extern struct ltchars ltchars_buf;
extern struct sgttyb initial_ttymode;
#endif

