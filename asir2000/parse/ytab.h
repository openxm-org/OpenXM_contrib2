#ifndef YYERRCODE
#define YYERRCODE 256
#endif

#define STRUCT 257
#define POINT 258
#define NEWSTRUCT 259
#define ANS 260
#define FDEF 261
#define PFDEF 262
#define GLOBAL 263
#define CMP 264
#define OR 265
#define AND 266
#define CAR 267
#define CDR 268
#define QUOTED 269
#define DO 270
#define WHILE 271
#define FOR 272
#define IF 273
#define ELSE 274
#define BREAK 275
#define RETURN 276
#define CONTINUE 277
#define PARIF 278
#define MAP 279
#define RECMAP 280
#define TIMER 281
#define GF2NGEN 282
#define GFPNGEN 283
#define GFSNGEN 284
#define GETOPT 285
#define FOP_AND 286
#define FOP_OR 287
#define FOP_IMPL 288
#define FOP_REPL 289
#define FOP_EQUIV 290
#define FOP_NOT 291
#define LOP 292
#define FORMULA 293
#define UCASE 294
#define LCASE 295
#define STR 296
#define SELF 297
#define BOPASS 298
#define PLUS 299
#define MINUS 300
typedef union {
	FNODE f;
	SNODE s;
	NODE n;
	NODE2 n2;
	int i;
	pointer p;
} YYSTYPE;
extern YYSTYPE yylval;
