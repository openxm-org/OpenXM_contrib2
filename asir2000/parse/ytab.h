#ifndef BISON_Y_TAB_H
# define BISON_Y_TAB_H

#ifndef YYSTYPE
typedef union {
	FNODE f;
	SNODE s;
	NODE n;
	NODE2 n2;
	int i;
	pointer p;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	STRUCT	257
# define	POINT	258
# define	NEWSTRUCT	259
# define	ANS	260
# define	FDEF	261
# define	PFDEF	262
# define	MODDEF	263
# define	MODEND	264
# define	GLOBAL	265
# define	MGLOBAL	266
# define	LOCAL	267
# define	LOCALF	268
# define	CMP	269
# define	OR	270
# define	AND	271
# define	CAR	272
# define	CDR	273
# define	QUOTED	274
# define	COLONCOLON	275
# define	DO	276
# define	WHILE	277
# define	FOR	278
# define	IF	279
# define	ELSE	280
# define	BREAK	281
# define	RETURN	282
# define	CONTINUE	283
# define	PARIF	284
# define	MAP	285
# define	RECMAP	286
# define	TIMER	287
# define	GF2NGEN	288
# define	GFPNGEN	289
# define	GFSNGEN	290
# define	GETOPT	291
# define	FOP_AND	292
# define	FOP_OR	293
# define	FOP_IMPL	294
# define	FOP_REPL	295
# define	FOP_EQUIV	296
# define	FOP_NOT	297
# define	LOP	298
# define	FORMULA	299
# define	UCASE	300
# define	LCASE	301
# define	STR	302
# define	SELF	303
# define	BOPASS	304
# define	PLUS	305
# define	MINUS	306


extern YYSTYPE yylval;

#endif /* not BISON_Y_TAB_H */
