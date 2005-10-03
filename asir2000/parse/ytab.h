/* A Bison parser, made by GNU Bison 1.875b.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     STRUCT = 258,
     POINT = 259,
     NEWSTRUCT = 260,
     ANS = 261,
     FDEF = 262,
     PFDEF = 263,
     MODDEF = 264,
     MODEND = 265,
     GLOBAL = 266,
     MGLOBAL = 267,
     LOCAL = 268,
     LOCALF = 269,
     CMP = 270,
     OR = 271,
     AND = 272,
     CAR = 273,
     CDR = 274,
     QUOTED = 275,
     COLONCOLON = 276,
     DO = 277,
     WHILE = 278,
     FOR = 279,
     IF = 280,
     ELSE = 281,
     BREAK = 282,
     RETURN = 283,
     CONTINUE = 284,
     PARIF = 285,
     MAP = 286,
     RECMAP = 287,
     TIMER = 288,
     GF2NGEN = 289,
     GFPNGEN = 290,
     GFSNGEN = 291,
     GETOPT = 292,
     FOP_AND = 293,
     FOP_OR = 294,
     FOP_IMPL = 295,
     FOP_REPL = 296,
     FOP_EQUIV = 297,
     FOP_NOT = 298,
     LOP = 299,
     FORMULA = 300,
     UCASE = 301,
     LCASE = 302,
     STR = 303,
     SELF = 304,
     BOPASS = 305,
     PLUS = 306,
     MINUS = 307
   };
#endif
#define STRUCT 258
#define POINT 259
#define NEWSTRUCT 260
#define ANS 261
#define FDEF 262
#define PFDEF 263
#define MODDEF 264
#define MODEND 265
#define GLOBAL 266
#define MGLOBAL 267
#define LOCAL 268
#define LOCALF 269
#define CMP 270
#define OR 271
#define AND 272
#define CAR 273
#define CDR 274
#define QUOTED 275
#define COLONCOLON 276
#define DO 277
#define WHILE 278
#define FOR 279
#define IF 280
#define ELSE 281
#define BREAK 282
#define RETURN 283
#define CONTINUE 284
#define PARIF 285
#define MAP 286
#define RECMAP 287
#define TIMER 288
#define GF2NGEN 289
#define GFPNGEN 290
#define GFSNGEN 291
#define GETOPT 292
#define FOP_AND 293
#define FOP_OR 294
#define FOP_IMPL 295
#define FOP_REPL 296
#define FOP_EQUIV 297
#define FOP_NOT 298
#define LOP 299
#define FORMULA 300
#define UCASE 301
#define LCASE 302
#define STR 303
#define SELF 304
#define BOPASS 305
#define PLUS 306
#define MINUS 307




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 83 "parse.y"
typedef union YYSTYPE {
	FNODE f;
	SNODE s;
	NODE n;
	NODE2 n2;
	int i;
	pointer p;
} YYSTYPE;
/* Line 1252 of yacc.c.  */
#line 150 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



