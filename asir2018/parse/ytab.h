/* A Bison parser, made by GNU Bison 3.7.5.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSE_Y_TAB_H_INCLUDED
# define YY_YY_PARSE_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    STRUCT = 258,                  /* STRUCT  */
    PNT = 259,                   /* PNT  */
    NEWSTRUCT = 260,               /* NEWSTRUCT  */
    ANS = 261,                     /* ANS  */
    FDEF = 262,                    /* FDEF  */
    PFDEF = 263,                   /* PFDEF  */
    MODDEF = 264,                  /* MODDEF  */
    MODEND = 265,                  /* MODEND  */
    GLOBAL = 266,                  /* GLOBAL  */
    MGLOBAL = 267,                 /* MGLOBAL  */
    LOCAL = 268,                   /* LOCAL  */
    LOCALF = 269,                  /* LOCALF  */
    CMP = 270,                     /* CMP  */
    OR = 271,                      /* OR  */
    AND = 272,                     /* AND  */
    CAR = 273,                     /* CAR  */
    CDR = 274,                     /* CDR  */
    QUOTED = 275,                  /* QUOTED  */
    COLONCOLON = 276,              /* COLONCOLON  */
    SEMISEMI = 277,                /* SEMISEMI  */
    DO = 278,                      /* DO  */
    WHILE = 279,                   /* WHILE  */
    FOR = 280,                     /* FOR  */
    IF = 281,                      /* IF  */
    ELSE = 282,                    /* ELSE  */
    BREAK = 283,                   /* BREAK  */
    RETURN = 284,                  /* RETURN  */
    CONTINUE = 285,                /* CONTINUE  */
    PARIF = 286,                   /* PARIF  */
    MAP = 287,                     /* MAP  */
    RECMAP = 288,                  /* RECMAP  */
    TIMER = 289,                   /* TIMER  */
    GF2NGEN = 290,                 /* GF2NGEN  */
    GFPNGEN = 291,                 /* GFPNGEN  */
    GFSNGEN = 292,                 /* GFSNGEN  */
    GETOPT = 293,                  /* GETOPT  */
    FOP_AND = 294,                 /* FOP_AND  */
    FOP_OR = 295,                  /* FOP_OR  */
    FOP_IMPL = 296,                /* FOP_IMPL  */
    FOP_REPL = 297,                /* FOP_REPL  */
    FOP_EQUIV = 298,               /* FOP_EQUIV  */
    FOP_NOT = 299,                 /* FOP_NOT  */
    LOP = 300,                     /* LOP  */
    FORMULA = 301,                 /* FORMULA  */
    UCASE = 302,                   /* UCASE  */
    LCASE = 303,                   /* LCASE  */
    STR = 304,                     /* STR  */
    SELF = 305,                    /* SELF  */
    BOPASS = 306,                  /* BOPASS  */
    PLUS = 307,                    /* PLUS  */
    MINUS = 308                    /* MINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define STRUCT 258
#define PNT 259
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
#define SEMISEMI 277
#define DO 278
#define WHILE 279
#define FOR 280
#define IF 281
#define ELSE 282
#define BREAK 283
#define RETURN 284
#define CONTINUE 285
#define PARIF 286
#define MAP 287
#define RECMAP 288
#define TIMER 289
#define GF2NGEN 290
#define GFPNGEN 291
#define GFSNGEN 292
#define GETOPT 293
#define FOP_AND 294
#define FOP_OR 295
#define FOP_IMPL 296
#define FOP_REPL 297
#define FOP_EQUIV 298
#define FOP_NOT 299
#define LOP 300
#define FORMULA 301
#define UCASE 302
#define LCASE 303
#define STR 304
#define SELF 305
#define BOPASS 306
#define PLUS 307
#define MINUS 308

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 83 "parse/parse.y"

	FNODE f;
	SNODE s;
	NODE n;
	NODE2 n2;
	int i;
	pointer p;

#line 182 "parse/y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSE_Y_TAB_H_INCLUDED  */
