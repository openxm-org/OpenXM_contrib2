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
#define DO 269
#define WHILE 270
#define FOR 271
#define IF 272
#define ELSE 273
#define BREAK 274
#define RETURN 275
#define CONTINUE 276
#define PARIF 277
#define MAP 278
#define TIMER 279
#define GF2NGEN 280
#define GFPNGEN 281
#define GETOPT 282
#define FOP_AND 283
#define FOP_OR 284
#define FOP_IMPL 285
#define FOP_REPL 286
#define FOP_EQUIV 287
#define FOP_NOT 288
#define LOP 289
#define FORMULA 290
#define UCASE 291
#define LCASE 292
#define STR 293
#define SELF 294
#define BOPASS 295
#define PLUS 296
#define MINUS 297
typedef union {
	FNODE f;
	SNODE s;
	NODE n;
	NODE2 n2;
	int i;
	pointer p;
} YYSTYPE;
extern YYSTYPE yylval;
