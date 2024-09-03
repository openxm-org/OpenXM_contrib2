/*	Copyright (c) 1987, 1988 by Software Research Associates, Inc.	*/

#ifndef lint
static char rcsid[]=
"$Header$ (SRA)";
#endif /* lint */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "fep_defs.h"
#include "fep_glob.h"
#include "fep_funcs.h"

#if defined(__FreeBSD__) || defined(__CYGWIN__) || defined(__APPLE_CC__) || defined(__INTERIX) || defined(ANDROID)
#include <regex.h>
regex_t Re_pat;
#define re_comp(p) (regcomp(&Re_pat,(p), REG_EXTENDED|REG_NOSUB))
#define re_exec(p) (!regexec(&Re_pat, (p), 0 , NULL, 0))
#else
    char *re_comp();
#endif

char	**HistoryTable;
int	TopOfHist;
int	TailOfHist;
int	CurrentHist;
int	HistorySize;

int	uniqline = 1;

char	*getOldestHistory();
char	*getYoungestHistory();

char *argv[MAXARGS];
int argc;

void init_hist(int size)
{
    char *itoa();

    TopOfHist = 0;
    TailOfHist = 0;
    CurrentHist = 0;
    HistorySize = size;

    /*
     * "set_var" will call "changeHistorySize" function to initialize
     * history table.
     */
    set_var ("history", itoa (HistorySize));
}

char * itoa (int i)
{
    static char buf[64];

    sprintf (buf, "%d", i);
    return (buf);
}

void addHistory(char *string)
{
    char *allocAndCopyThere();
    char *prev;
    int slen;

    if (HistorySize <= 0)
	return;

    CurrentHist = TailOfHist;
    prev = getYoungestHistory ();
    if (look_var ("ignore-same-line") && prev && !strcmp (string, prev))
	return;
    if ((slen = lookd_var ("ignore-short-line"))>0 && strlen (string) <= slen)
	return;

    if (TailOfHist-HistorySize >= TopOfHist) {
	if (HistoryTable[TopOfHist % HistorySize])
	    free(HistoryTable[TopOfHist % HistorySize]);
        TopOfHist++;
    }
    HistoryTable[(TailOfHist++)%HistorySize] = allocAndCopyThere(string);
    CurrentHist = TailOfHist;
}

void resetCurrentHistory()
{
    CurrentHist = TailOfHist;
}

char * getHistory(int num)
{
    if (HistorySize <= 0)
	return (0);

    if (num < TopOfHist || TailOfHist <= num) {
        return ((char *)0);
    }
    else {
        return (HistoryTable[num % HistorySize]);
    }
}

char * getCurrentHistory()
{
    return (getHistory (CurrentHist));
}

char * getPreviousHistory()
{
    if (HistorySize <= 0)
	return (0);

    if (TailOfHist == 0) {
        return((char *)0);
    }
    if (CurrentHist == TopOfHist)
	CurrentHist = TailOfHist - 1;
    else
	CurrentHist--;
    return (getCurrentHistory ());
}

char * getNextHistory()
{
    
    if (HistorySize <= 0)
	return (0);

    if (CurrentHist == TailOfHist || CurrentHist == TailOfHist-1)
	CurrentHist = TopOfHist;
    else
        CurrentHist++;
    return (getCurrentHistory ());
}

int getOldestHistNum()
{

    return (TopOfHist);
}

int getYoungestHistNum()
{

    return (TailOfHist-1);
}

char * getOldestHistory()
{
    register char *cp;

    if (TailOfHist == 0)
        return("");

    cp = HistoryTable[TopOfHist];
    return (cp ? cp : "");
}

char * getYoungestHistory()
{
    register char *cp;

    if (TailOfHist == 0)
        return("");

    cp = getHistory (getYoungestHistNum());
    return (cp ? cp : "");
}

int getCurrentHistNum()
{
    return (CurrentHist);
}

char * allocAndCopyThere(char *string)
{
    register char *cp;
    
    cp = (char *)malloc(strlen(string)+1);

    if (cp == (char *)0)
	return ((char *)0);

    strcpy(cp, string);
    return(cp);
}

char * historyExtract(char *string)
{
    char *search_reverse_history();

    if (HistorySize <= 0)
	return (0);

    switch (*++string) {
	case '0': 
	case '1': 
	case '2': 
	case '3': 
	case '4': 
	case '5': 
	case '6': 
	case '7': 
	case '8': 
	case '9': 
	    {
		register int    histNum;

		histNum = (atoi (string)) - 1;
		if (TopOfHist <= histNum && histNum < TailOfHist) {
		    CurrentHist = histNum;
		    return (HistoryTable[histNum % HistorySize]);
		}
		else {
		    return ((char *) 0);
		}
	    }
	    break;

	case '-':
	    {
		register int    histNum;

		if (! isdigit(*++string))
		    return ((char *) 0);

		histNum = TailOfHist - (atoi (string));
		if (TopOfHist <= histNum && histNum < TailOfHist) {
		    CurrentHist = histNum;
		    return (HistoryTable[histNum % HistorySize]);
		}
		else {
		    return ((char *) 0);
		}
	    }
	    break;

	case '!': 
	    if (TailOfHist != 0) {
		CurrentHist = TailOfHist - 1;
		return (HistoryTable[(TailOfHist - 1) % HistorySize]);
	    }
	    break;

	case '?':
	    return (search_reverse_history (++string));

	default:
	    {
		char buf[64];

		strcpy (buf, "^");
		strncat (buf, string, 64);
		return (search_reverse_history (buf));
	    }
    }
    return ((char *) 0);
}

char * search_reverse_history (char *string)
{
    register int i;

    if (string != NULL) {
	set_var ("search-string", string);
	if (re_comp(string) != (char *)0)
	    return ((char *) 0);
	i = TailOfHist - 1;
    }
    else
	i = CurrentHist - 1;

    for (; i >= TopOfHist; i--) {
	if (re_exec(HistoryTable[i % HistorySize]) == 1) {
	    CurrentHist = i;
	    return(HistoryTable[i % HistorySize]);
	}
    }
    return ((char *) 0);
}

char * search_forward_history (char *string)
{
    register int i;

    if (string != NULL) {
	if (re_comp(string) != (char *)0)
	    return ((char *) 0);
	i = TopOfHist;
    }
    else
	i = CurrentHist + 1;

    for (; i <= TailOfHist; i++) {
	if (re_exec(HistoryTable[i % HistorySize]) == 1) {
	    CurrentHist = i;
	    return(HistoryTable[i % HistorySize]);
	}
    }
    return ((char *) 0);
}

/*
 * Change history table size.
 */
int changeHistorySize(int newsize)
{
    char **newHistTable;
    register int newTop, i;

    if (newsize > 0)
	newHistTable = (char **)calloc(sizeof(char *), newsize);


    if (newHistTable == 0)
	return (0);

    newTop = (TailOfHist - newsize < TopOfHist)
		? TopOfHist : TailOfHist - newsize;

    /*
     * This function can be called for initializing history table
     */
    if (HistoryTable) {
	for (i = TailOfHist-1; i >= TopOfHist && i >= newTop; i--) {
	    newHistTable[i%newsize] = HistoryTable[i%HistorySize];
	}

	for (i = TopOfHist; i < newTop; i++) {
	    if (HistoryTable[i%HistorySize]) {
		free(HistoryTable[i%HistorySize]);
	    }
	}
	free(HistoryTable);
    }

    if (newsize <= 0)
	HistoryTable = (char **) 0;

    TopOfHist = newTop;
    HistorySize = newsize;
    HistoryTable = newHistTable;

    if (look_var ("debug")) {
	printf ("history: top=%d, tail=%d, size=%d\n",
	    TopOfHist, TailOfHist, HistorySize);
    }

    return (1);

}

/*
 * Built-in function "fep-history"
 */
void fep_history (char *comline)
{
    int num;

    argc = mkargv (comline, argv, MAXARGS);

    if (argc == 2)
	num = atoi (argv[1]);
    else
	num = 0;

    hist_showHistory (num);
}

void hist_showHistory (int num)
{
    register int from, to, i;
    char *cp;

    if (num <= 0)
	num = HistorySize;

    from = getOldestHistNum ();
    to = getYoungestHistNum ();

    from = max (from, to - num + 1);

    if (CurrentHist < from) {
	from = max (getOldestHistNum(), CurrentHist - num/2);
	to = min (getYoungestHistNum(), from + num - 1);
    }

    for (i = from; i <= to; i++) {
	if (cp = getHistory(i)) {
	    printf("%c%4d ", (i == CurrentHist) ? '>' : ' ', i+1);
	    ctlprint(cp);
	}
    }
}

char * mk_home_relative (char *cp)
{
    static char buf[256];

    /*
     * If variable "history-file" is not absolute path name,
     * change it to relative path name from home directory.
     */
    if (*cp != '/' && !(*cp == '.' && *(cp+1) == '/')) {
	strcpy (buf, getenv ("HOME"));
	strcat (buf, "/");
	strcat (buf, cp);
    }
    else
	strcpy (buf, cp);

    return (buf);
}

void fep_save_history (char *comline)
{
    char *file;
    char **argp;
    FILE *fp;
    int num = 0;

    argc = mkargv (comline, argv, MAXARGS);

    argp = argv;
    ++argp;

    if (isdigit (**argp))
	num = atoi (*argp++);

    if (*argp == NULL) {
	char *cp;

	if ((cp = look_var ("history-file")) == NULL) {
	    clear_edit_line ();
	    printf (
		"%s: Argument or \"history-file\" variables is required\n",
		argv[0]
	    );
	    recover_edit_line (1);
	    return;
	}
	file = mk_home_relative (cp);
    }
    else
	file = *argp;

    save_history (file, num);
}

#define MAXSAVEHIST 512

void save_history (char *file, int num)
{
    int old, new;
    FILE *fp, *fopen();
    char *cp;

    old = getOldestHistNum ();
    new = getYoungestHistNum ();

    if (num <= 0)
	num = MAXSAVEHIST;

    if (new - num + 1 > old)
	old = new - num + 1;

    if (look_var ("debug")) {
	printf ("save history from %d to %d\n", old, new);
    }

    if ((fp = fopen (file, "w")) == NULL) {
	clear_edit_line ();
	perror (file);
	recover_edit_line (1);
	return;
    }

    while (old <= new) {
	cp = getHistory (old++);
	fprintf (fp, "%s\n", cp);
    }

    fclose (fp);
    return;
}

void fep_read_history (char *comline)
{
    char *file;
    char **argp;
    FILE *fp;

    argc = mkargv (comline, argv, MAXARGS);

    argp = argv;
    ++argp;

    if (*argp == NULL) {
	char *cp;
	
	if ((cp = look_var ("history-file")) == NULL) {
	    clear_edit_line ();
	    printf (
		"%s: Argument or \"history-file\" variables is required\n",
		argv[0]
	    );
	    recover_edit_line (1);
	    return;
	}
	file = mk_home_relative (cp);
    }
    else
	file = *argp;

    read_history (file);
}

void read_history (char *file)
{
    FILE *fp;
    char line [MAXCMDLEN];
    register int i;

    if ((fp = fopen (file, "r")) == NULL) {
	clear_edit_line ();
	perror (file);
	recover_edit_line (1);
	return;
    }

    while (fgets (line, MAXCMDLEN, fp)) {
	i = strlen (line) - 1;
	if (line [i] == '\n')
	    line [i] = '\0';
	addHistory (line);
    }
    return;
}
