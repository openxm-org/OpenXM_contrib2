/*	Copyright (c) 1987, 1988 by Software Research Associates, Inc.	*/

#ifndef lint
static char rcsid[]=
"$Header$ (SRA)";
#endif /* lint */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fep_defs.h"
#include "fep_funcs.h"
#include "fep_glob.h"
void initViBindings (FUNC cft[], FUNC aft[]);

VAR default_set_vars [] = {
	{"expand-tilde",	"",
	 "Expand ~ to home directory name",	(VAR*)0},
	{"ignore-empty-line",	"",
	 "Don't put empty line to history",	(VAR*)0},
	{"ignore-same-line",	"",
	 "Don't put same line to history",	(VAR*)0},
	{"editmode",		"emacs",
	 "Fep command line edit mode",		(VAR*)0},
	{"history-file",	".fephistory",
	 "Name of history file",		(VAR*)0},
	{"shell",		"/bin/sh",
	 "Shell name used by invoke-shell",	(VAR*)0},
	{"auto-tty-fix",	"",
	 "Fix tty mode automatically",		(VAR*)0},
	{"script-file",		"fepscript",
	 "Script file name",			(VAR*)0},
	{"crt",			"24",
	 "Terminal lines",			(VAR*)0},
	{"showhist",		"24",
	 "History length used by show-history",	(VAR*)0},
	{"delimiters",		" \t",
	 "Argument delemiter characters",	(VAR*)0},
	{"buffer",		"5120",
	"Output buffer size",			(VAR*)0},
	{NULL,			NULL,
	 NULL,					(VAR*)0}
};

VAR default_unset_vars [] = {
	/* below from here is unset by default */
	{"alarm-on-eof",	"",
	 "Alarm once to eof character",		(VAR*)0},
	{"ignore-eof",		"",
	 "Never treat eof character as eof",	(VAR*)0}, 
	{"savehist",		"",
	 "Length of save history",		(VAR*)0},
	{"verbose",		"",
	 NULL,					(VAR*)0}, 
	{"search-string",	"",
	 NULL,					(VAR*)0},
	{"noalias",		"",
	 "Not use alias",			(VAR*)0},
	{"tty-fix-bell",	"",
	 "Ring bell when tty mode changed",	(VAR*)0},
	{"auto-repaint",	"",
	 "Repaint screen when restarting",	(VAR*)0},
	{"clear-repaint",	"",
	 "Clear screen before repaint",		(VAR*)0},
	{"ignore-short-line",	"3",
	"Don't put short line to history",	(VAR*)0},
	{NULL,			NULL,
	 NULL,					(VAR*)0}
};

#ifdef HASH
#define HASHNUM	10
VAR *var_htab[HASHNUM];
#else /* HASH */
VAR var_top = {"top", "top", (char *)0, (VAR *)0};
VAR *var_list = &var_top;
#endif /* HASH */

/*
 * Functions
 */
extern	char	*allocAndCopyThere();
extern	char	*prompt;

/*
 * Set default variables
 */
void set_default_vars ()
{
    register VAR *vp;

    for (vp = default_set_vars; vp->v_name; vp++)
	set_only_var (vp->v_name, vp->v_value);
}

/*
 * Set variable
 */
int set_var (char *name, char *value)
{
    /*
     * Process special variable
     */
    if (eq (name, "history")) {
	if (!changeHistorySize (atoi (value)))
	    return (0);
    }
    else if (eq (name, "prompt")) {
	free (prompt);
	prompt = allocAndCopyThere (value);
    }
    else if (eq (name, "editmode")) {
	if (eq (value, "emacs")) {
	    editmode = EMACS;
	    initEmacsBindings (curFuncTab, altFuncTab);
	}
	else if (eq (value, "vi")) {
	    editmode = VI;
	    initViBindings (curFuncTab, altFuncTab);
	}
	else
	    printf ("%s: Unknown editmode\n", value);
    }
    else if (eq (name, "auto-tty-fix"))
	auto_tty_fix = ON;
    else if (eq (name, "debug"))
	debug = ON;
    else if (eq (name, "tty-fix-bell"))
	tty_fix_bell = ON;
    else if (eq (name, "delimiters")) {
	/* only in this case, variable points allocated area */
	set_only_var (name, value);
	delimiters = look_var ("delimiters");
    }
    else if (eq (name, "crt"))
	lines = atoi (value);
    else if (eq(name, "buffer")) {
	int s;

	if ((s = atoi(value)) <= 0)
	    return;

	if (!set_buffer (output_buffer, s))
	    return;
    }

    set_only_var (name, value);
}

void set_only_var (char *name, char *value)
{
    VAR *vp;

    vp = getvp (name, 1, 0);

    if (vp->v_value == value)
	return;

    if (vp->v_value)
	free (vp->v_value);

    vp->v_value = allocAndCopyThere (value);
}

/*
 * Unset variable
 */
void unset_var (char *name)
{
    VAR *vp, *prev;

    vp = getvp (name, 0, &prev);

    if (!vp)
	return;

    prev->v_next = vp->v_next;
    free (vp->v_name);
    free (vp->v_value);
    free (vp);

    if (eq (name, "auto-tty-fix"))
	auto_tty_fix = OFF;

    if (eq (name, "debug"))
	debug = OFF;

    if (eq (name, "tty-fix-bell"))
	tty_fix_bell = OFF;

    if (eq (name, "delimiters"))
	delimiters = DEFAULT_DELIMITERS;
    return;
}

/*
 * Look up variable
 */
char * look_var (char *name)
{

    VAR *vp;

    vp = getvp (name, 0, 0);

    if (vp && vp->v_value)
	return (vp->v_value);
    else
	return ((char *)0);
}

/*
 * Look up variable and get integer result
 */
int lookd_var (char *name)
{
    VAR *vp;

    vp = getvp (name, 0, 0);

    if (vp && vp->v_value)
	return (atoi (vp->v_value));
    else
	return (0);
}

/*
 * Show variable list
 */
void show_varlist ()
{
#ifdef HASH
    register int i;

    for (i = 0; i< HASHNUM; i++) {
	register VAR *vp;

	vp = var_htab[i];
	if (vp == (VAR *)0)
	    continue;

	for (; vp != (VAR *)0; vp = vp->v_next)
	    printf ("%-16s %s\n", vp->v_name, vp->v_value);
    }
#else /* HASH */
    register VAR *vp;

    for (vp = var_list->v_next; vp; vp = vp->v_next)
	printf ("%-16s %s\n", vp->v_name, vp->v_value);
#endif /* HASH */
}

#ifdef HASH
/*
 * Get hash index from variable name
 */
static getindex (char *s)
{
    register int sum = 0;

    while (*s)
	sum += *s++;

    return (sum % HASHNUM);
}
#endif /* HASH */

/*
 * Get pointer to VAR.
 * If there is no memoly associated to the variable and alloc argument is 1,
 * allocate the area and initialize name field.
 */
VAR * getvp (char *name, int alloc, VAR **lastvp)
{
#ifdef HASH
    register i = getindex (name);
#endif /* HASH */
    VAR *vp, *last = (VAR *)0;

#ifdef HASH
    for (vp = var_htab[i]; vp; last = vp, vp->v_next) {
	if (strcmp (name, vp->v_value) == 0)
	    return (vp);
    }
#else /* HASH */
    for (vp = var_list->v_next, last = var_list; vp; last = vp, vp = vp->v_next) {
	int r;

	r = strcmp (name, vp->v_name);
	if (r == 0) {
	    if (lastvp)
		*lastvp = last;
	    return (vp);
	}
	else if (r < 0)
	    break;
    }
#endif /* HASH */

    if (alloc == 0)
	return (0);

    vp = (VAR *) calloc (sizeof (VAR), 1);
    vp->v_value = (char *)0;
    vp->v_next = (VAR *) 0;
    vp->v_name = allocAndCopyThere (name);

#ifdef HASH
    if (last) {
	vp->v_next = last->v_next;
	last->v_next = vp;
    }
    else {
	var_htab[i] = vp;
    }
#else /* HASH */
    vp->v_next = last->v_next;
    last->v_next = vp;
#endif /* HASH */
    return (vp);
}
