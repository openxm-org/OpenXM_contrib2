/*	Copyright (c) 1987, 1988 by Software Research Associates, Inc.	*/

#ifndef lint
static char rcsid[]=
"$Header$ (SRA)";
#endif /* lint */

#include <stdio.h>
#include <ctype.h>
#include "fep_defs.h"
#include "fep_glob.h"

typedef struct _alias {
	char *al_name;
	char *al_value;
	struct _alias *al_next;
} ALIAS;

ALIAS	alias_top = {"top", "top", (ALIAS *)0};
ALIAS	*alias_list = &alias_top;
CHAR	aliased_line[MAXCMDLEN+1];

/*
 * Functions
 */
int	set_alias	(/* char *name, char *value */);
char	*look_alias	(/* char *name */);
ALIAS	*getap		(/* char *name */);

extern	char	*allocAndCopyThere();
extern	char	*prompt;

/*
 * Check alias list, and if found alias change command by its value
 */
CHAR *
check_alias (comline)
    char *comline;
{
    char *argv[MAXARGS];
    int argc;
    char *av;
    int substitute = 0;

    while (isspace (*comline))
	++comline;
    if (*comline == '\0')
	return ((CHAR *)0);

    argc = mkargv (comline, argv, MAXARGS);

    if (av = look_alias (argv[0])) {
	int len = strlen (argv[0]);
	char *cp;
	CHAR *ap;

	*aliased_line = '\0';
	ap = aliased_line;
	cp = av;
	for (; *cp; ++cp) {

	    if (*cp != '!')
		*ap++ = *cp;
	    else {
		switch (*(cp + 1)) {
		case '^':
		    if (argc > 1) {
			strcpy (ap, argv[1]);
			ap += strlen (argv[1]);
		    }
		    substitute++;
		    break;

		case '$':
		    if (argc > 1) {
			strcpy (ap, argv[argc-1]);
			ap += strlen (argv[argc - 1]);
		    }
		    substitute++;
		    break;

		case '*':
		{
		    int j;

		    for (j = 1; j < argc; j++) {
			strcpy (ap, " ");
			strcat (ap, argv[j]);
			ap += strlen (argv[j]) + 1;
		    }
		    substitute++;
		    break;
		}

		default:
		    *ap = *cp;
		    *++ap = *(cp + 1);
		    break;
		}
		cp++;
	    }
	}
	*ap = '\0';
		    
	/*
	 * If there is no ! notation, copy all arguments in command line.
	 */
	if (substitute == 0)
	    strcat (aliased_line, comline + len);

	return (aliased_line);
    }
    else
	return ((CHAR *)0);
}

/*
 * Set alias
 */
set_alias (name, value)
    char *name, *value;
{
    ALIAS *vp;

    vp = getap (name, 1, 0);

    if (vp->al_value)
	free (vp->al_value);

    vp->al_value = allocAndCopyThere (value);
}

/*
 * Unset alias
 */
unset_alias (name)
    char *name;
{
    ALIAS *vp, *prev;

    vp = getap (name, 0, &prev);

    if (!vp)
	return;

    prev->al_next = vp->al_next;
    free (vp->al_name);
    free (vp->al_value);
    free (vp);
    return;
}

/*
 * Look up alias
 */
char *
look_alias (name)
    char *name;
{

    ALIAS *vp;

    vp = getap (name, 0, 0);

    if (vp && vp->al_value)
	return (vp->al_value);
    else
	return ((char *)0);
}

/*
 * Show alias list
 */
show_aliaslist (a)
    char *a;
{
    register ALIAS *vp;

    for (vp = alias_list->al_next; vp; vp = vp->al_next) {
	if (a && strcmp (a, vp->al_name))
	    continue;
	printf ("%-16s %s\n", vp->al_name, vp->al_value);
    }
}


/*
 * Get pointer to ALIAS.
 * If there is no memoly associated to the alias and alloc argument is 1,
 * allocate the area and initialize name field.
 */
ALIAS *
getap (name, alloc, lastvp)
    char *name;
    int alloc;
    ALIAS **lastvp;
{
    ALIAS *vp, *last = (ALIAS *)0;

    for (vp = alias_list->al_next, last = alias_list;
	 vp; last = vp, vp = vp->al_next) {
	int r;

	r = strcmp (name, vp->al_name);
	if (r == 0) {
	    if (lastvp)
		*lastvp = last;
	    return (vp);
	}
	else if (r < 0)
	    break;
    }

    if (alloc == 0)
	return (0);

    vp = (ALIAS *) calloc (sizeof (ALIAS), 1);
    vp->al_value = (char *)0;
    vp->al_next = (ALIAS *) 0;
    vp->al_name = allocAndCopyThere (name);

    vp->al_next = last->al_next;
    last->al_next = vp;
    return (vp);
}
