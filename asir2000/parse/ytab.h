
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


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
/* Tokens.  */
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




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 83 "parse.y"

	FNODE f;
	SNODE s;
	NODE n;
	NODE2 n2;
	int i;
	pointer p;



/* Line 1676 of yacc.c  */
#line 167 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


