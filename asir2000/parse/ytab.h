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
#define TIMER 280
#define GF2NGEN 281
#define GFPNGEN 282
#define GETOPT 283
#define FOP_AND 284
#define FOP_OR 285
#define FOP_IMPL 286
#define FOP_REPL 287
#define FOP_EQUIV 288
#define FOP_NOT 289
#define LOP 290
#define FORMULA 291
#define UCASE 292
#define LCASE 293
#define STR 294
#define SELF 295
#define BOPASS 296
#define PLUS 297
#define MINUS 298
typedef union {
	FNODE f;
	SNODE s;
	NODE n;
	NODE2 n2;
	int i;
	pointer p;
} YYSTYPE;
extern YYSTYPE yylval;
