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

#ifndef MKARGDEBUG

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#if defined(__CYGWIN__) || defined(sun) || defined(ANDROID) || defined(__FreeBSD__)
#include <dirent.h>
#else
#include <sys/dir.h>
#endif
#include <ctype.h>
#include "fep_defs.h"
#include "fep_funcs.h"

void message(char *messageString)
{
    write (2, messageString, strlen (messageString));
}

void errorBell()
{
    write (2, "\007", 1);
}

void ctlprint(char *string)
{
    register char  *cp;

    for (cp = string; *cp; cp++) {
	if (isctlchar (*cp)) {
	    putchar ('^');
	    putchar (unctl (*cp));
	}
	else
	    putchar (*cp);
    }
    fputs ("\r\n", stdout);
    fflush (stdout);
}

/*
 * Print string using "^" for control characters
 */
void printS (char *string)
{
    char *cp;

    for (cp = string; *cp; cp++)
	putChar (*cp);
}

/*
 * Check the line is empty or not
 */
int is_empty_line(char *line)
{
    register char *cp;

    for (cp = line; *cp; cp++) {
        if (!isspace(*cp)) {
	    return(0);
	}
    }
    return(1);
}

/*
 * Put character using "^" for control characters
 */
void putChar(char c)
{
    if (isctlchar(c)) {
	(void) putchar('^');
	(void) putchar(unctl(c));
    }
    else
	(void) putchar(c);
}

char * x_dirname (char *dir)
{
    static char dirname [256];
#if !defined(ANDROID)
    char *index();
#endif

    if (*dir != '~')
	strcpy (dirname, dir);
    else {
	struct passwd *pw;

	if (*(dir+1) == '/' || *(dir+1) == '\0') {
	    pw = getpwuid (getuid ());
	}
	else {
	    char user [64], *sp, *dp;

	    for (sp = dir+1, dp = user; *sp && *sp != '/'; sp++, dp++)
		*dp = *sp;
	    *dp = '\0';
	    pw = getpwnam (user);
	}

	if (pw) {
	    strcpy (dirname, pw->pw_dir);
	    if (any ('/', dir))
		strcat (dirname, index (dir, '/'));
	}
	else {
	    strcpy (dirname, dir);
	}    
    }

    return (dirname);
}

DIR * x_opendir (char *dir)
{
    return (opendir (x_dirname (dir)));
}

/*
 * Strring compare for qsort
 */
int scmp (char **a, char **b)
{

    return (strcmp (*a, *b));
}

/*
 * Return 1 if "str" is prefixed by "sub"
 */
int prefix (char *sub, char *str)
{

    for (;;) {
	if (*sub == 0)
	    return (1);
	if (*str == 0)
	    return (0);
	if (*sub++ != *str++)
	    return (0);
    }
}

/*
 * Return 1 if s includes character c
 */
int any (int c, char *s)
{

    while (*s)
	if (*s++ == c)
	    return(1);
    return(0);
}

#ifndef max
/*
 * Return maximum number of d1 and d2
 */
int max (int d1, int d2)
{
    return (d1 > d2 ? d1 : d2);
}
#endif /* max */

#else /* MKARGDEBUG */

#include <stdio.h>
#include <ctype.h>

#define MAXARGS	64

main()
{
    char s[128];
    char *argv[MAXARGS];

    while (gets (s)) {
	register int c;

	showArgs (s);
    }
}
#endif /* MKARGDEBUG */

void showArgs (char *comline)
{
    char *argv[MAXARGS];
    register int c;
    register char **argp;
    register int i;

    c = mkargv (comline, argv, MAXARGS);
    if (c < 0) {
	printf ("%s\n", argv[0]);
	return;
    }
    printf ("argc = %d\n", c);
    for (i = 0, argp = argv; *argp; argp++, i++) {
	printf ("\"%s\" ", *argp);
    }
    printf ("\n");
}

int mkargv (char *s, char *argv[], int maxarg)
{
    register char *cp;
    register int argc = 0;
    int insquot = 0, indquot = 0, ignorenext = 0;
    enum {STRING, SPACE} status = SPACE;
    static char buf[1024], *bp;

    for (cp = s, bp = buf; *cp; cp++) {

	if (argc > maxarg)
	    return (argc);

	/*
	 * Found white space
	 */
	if (isspace (*cp)) {
	    /*
	     * In outside of quotation
	     */
	    if (!ignorenext && !insquot && !indquot) {
		/*
		 * If status was in string, go next arg
		 */
		if (status == STRING) {
		    status = SPACE;
		    *bp++ = '\0';
		    continue;
		}
		else
		    continue;
	    }
	}

#	define SPECIALCHARS "\"\'\\"
	if (!ignorenext && index (SPECIALCHARS, *cp)) {
	    switch (*cp) {

		/*
		 * Literal next character
		 */
		case '\\':
		    if (indquot || insquot)
			goto THROUGH;
		    else {
			ignorenext = 1;
			continue;
		    }

		/*
		 * Double quotation
		 */
		case '\"':
		    if (insquot)
			goto THROUGH;
		    if (indquot && *(bp-1) == '\\') {
			bp--;
			goto THROUGH;
		    }
		    indquot = !indquot;
		    break;

		/*
		 * Single quotation
		 */
		case '\'':
		    if (indquot)
			goto THROUGH;
		    if (insquot && *(bp-1) == '\\') {
			bp--;
			goto THROUGH;
		    }
		    insquot = !insquot;
		    break;
    	    }

	    /*
	     * Only in " or ' case.
	     */
	    if (status == SPACE) {
		status = STRING;
		argc++;
		argv[argc-1] = bp;
		*bp = 0;
	    }
	    continue;
	}

THROUGH:
	/*
	 * Found non-space character
	 */
	ignorenext = 0;
	if (status == SPACE) {
	    status = STRING;
	    argc++;
	    argv[argc-1] = bp;
	}
	*bp++ = *cp;
    }

    if (indquot || insquot) {
	argv[0] = indquot ? "Unmatched \"." : "Unmatched \'.";
	return (-1);
    }

    *bp = '\0';
    argv[argc] = (char *)0;
    return (argc);
}

void reverse_strcpy (char *to, char *from)
{
    register int len;

    for (len = strlen (from); len >= 0; len--)
	*(to + len) = *(from + len);
}

char *search_string (char *s, char *lookup)
{
    int len = strlen (lookup);
    enum {SQUOTE, DQUOTE, NORMAL} status = NORMAL;

    while (*s) {
	switch (*s) {
	case '\'':
	    if (status == DQUOTE) break;
	    status = (status == SQUOTE) ? NORMAL : SQUOTE;
	    break;

	case '\"':
	    if (status == SQUOTE) break;
	    status = (status == DQUOTE) ? NORMAL : DQUOTE;
	    break;

	case '\\':
	    ++s;
	    break;
	}
	if (!*s) break;
	if (status == NORMAL && strncmp (s, lookup, len) == 0)
	    return s;
	++s;
    }
    return ((char*) 0);
}

#ifdef KANJI
/*
 * Uuuuuuuum. I hate search kanji in Shift-JIS code from the end of string
 * This function check if i'th character is first byte of KANJI code
 * in string starting from s.
 * It is assumed that first byte of strint s can't be second byte of KANJI
 * code.
 */
int iskanji_in_string (char *s, int i)
{
    register char *cp = s, *target = s + i;

    if (i < 0)
	return (0);

    while (cp < target) {
	if (iskanji (*cp))
	    cp += 2;
	else
	    cp++;
    }

    if (cp != target)
	return (0);
    else
	return (iskanji (*cp));
}
#endif /* KANJI */
