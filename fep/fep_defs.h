/*	Copyright (c) 1987, 1988 by Software Research Associates, Inc.	*/

#ifndef lint
# define FEP_DEFS \
    "$Header$ (SRA)"
#endif /* lint */

#define	MAXCMDLEN		512	/* maximum command length */
#define MAXARGS			64	/* maximum number of arguments */
#define ON			1	/* on switch */
#define OFF			0	/* off switch */
#define DFL_HISTLEN		100	/* default history length */
#define DFL_SHOWHIST		20	/* default show history length */
#define	IGNORED			2	/* ignored */
#define	PROCESSED		1	/* processed or not by history */
#define	NOT_PROCESSED		0	/* built-in functions */

#define BS			'\b'	/* backspace character */
#define SP			' '	/* space character */

#define	DEFAULT_DELIMITERS	" \t"	/* default delimiter character */

/*
 * Pseudo functions
 */
#define eq(a,b)		(strcmp(a,b) == 0)
#define abs(a)		((a)>0?(a):-(a))
#define max(a,b)	((a)>(b)?(a):(b))
#define min(a,b)	((a)<(b)?(a):(b))
#ifdef KANJI
# define isctlchar(c)	(c && !iskanji(c) && (!(c&0140) || c=='\177'))
# define iswordchar(c)	(c && (iskanji(c) || isalnum(c) || iscntrl(c)))
# define isWordchar(c)	((c) && !isspace((c)))
#else /* KANJI */
# define isctlchar(c)	(c && (!(c&0140) || c=='\177'))
# define iswordchar(c)	(isalnum(c) || iscntrl(c))
# define isWordchar(c)	((c) && !isspace((c)))
#endif /* KANJI */
#define unctl(c)	(((c)=='\177') ? '?' : ((c) >= 040) ? (c) : (c)|0100)
#define toctrl(c)	((c)&~0100)
#define ctrl(c)		((c)&037)

#if defined(__alpha) || defined(__x86_64__) || defined(__amd64__) || defined(ANDROID) || defined(__aarch64__)
#define INDIRECTED      (((u_long)1)<<63)
#define isIndirect(f)   ((u_long)(f)&(u_long)INDIRECTED)
#define setIndirect(f)  (FUNC)((u_long)(f)|(u_long)INDIRECTED)
#define maskIndirect(f) (FUNC *)((u_long)(f)&~(u_long)INDIRECTED)
#else
#define	INDIRECTED	(1<<(sizeof(char*)*8-1))
			/* this is actually 0x80000000 on 32 bit machine,
			that addresses kernel address space */
#define isIndirect(f)	((u_int)(f)&(u_int)INDIRECTED)
#define setIndirect(f)	(FUNC)((u_int)(f)|(u_int)INDIRECTED)
#define	maskIndirect(f)	(FUNC *)((u_int)(f)&~(u_int)INDIRECTED)
#endif

/*
 * Type of character
 */
#ifdef KANJI
# define CHAR		unsigned char
# define CHARMASK	0377
#else /* KANJI */
# define CHAR		char
# define CHARMASK	0177
#endif /* KANJI */

/*
 * Only one machine I know alloca() works is vax.
 */
#ifdef vax
# define ALLOCA
#endif /* vax */

/*
 * Typedef's
 */
typedef	int	(*FUNC)();		/* pointer to funciton */

typedef enum {				/* edit status */
	EDITING,
	NOTEDITING
} EDITSTATUS;

typedef enum {				/* edit mode */
	NOTYET,
	EMACS,
	VI
} EDITMODE;

typedef struct {			/* bind table entry type */
	char	*bt_s;
	FUNC	bt_func;
} BINDENT;

typedef struct _var {
	char *v_name;
	char *v_value;
	char *v_help;
	struct _var *v_next;
} VAR;

typedef struct {
	char *b_buf;			/* buffer */
	char *b_lastbuf;		/* last i/o done pointer */
	int b_max;			/* max size */
	int b_next;			/* next read point */
	int b_count;			/* count */
	int b_hiwater;			/* high water mark */
} BUFFER;

#define buf_count(b) ((b)->b_count)
#define buf_remain(b) ((b)->b_size-(b)->b_count)
/*
char buf_char (b, n)
    BUFFER *b;
    int n;
{
    if (n >= b->b_count || -n > b->b_count)
	return (-1);
    if (b->b_next+n >= b->b_count)
	return (b->b_buf[b->b_next+n-b->b_count]);
    else if (b->b_next+n < 0)
	return (b->b_buf[b->b_next+n+b->b_count]);
    else
	return (b->b_buf[b->b_next+n]);
}
*/	    
#define buf_char(b,n) \
	(((n) >= (b)->b_count || -(n) > (b)->b_count) \
	  ?((char)-1) \
	  :((((b)->b_next + (n)) >= (b)->b_count) \
	    ?((b)->b_buf[(b)->b_next + (n) - (b)->b_count]) \
	    :((((b)->b_next + (n)) < 0) \
	      ?((b)->b_buf[(b)->b_next + (n) + (b)->b_count]) \
	      :((b)->b_buf[(b)->b_next + (n)]))))
