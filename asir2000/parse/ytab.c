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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 50 "parse.y"

#define malloc(x) GC_malloc(x)
#define realloc(x,y) GC_realloc(x,y)
#define free(x) GC_free(x)

#if defined(TOWNS)
#include <alloca.h>
#endif
#include <ctype.h>
#include "ca.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "parse.h"

#define NOPR (prresult=0)

extern int gdef,mgdef,ldef;
extern SNODE parse_snode;
extern int main_parser, allow_create_var;

int prresult,saveresult;

static int ind;
static FNODE t;
static NODE a,b;
static NODE2 a2;
static pointer val;
static QUOTE quote;
extern jmp_buf env;


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

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
/* Line 191 of yacc.c.  */
#line 220 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 232 "y.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  101
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1290

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  77
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  26
/* YYNRULES -- Number of rules. */
#define YYNRULES  113
/* YYNRULES -- Number of states. */
#define YYNSTATES  280

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   307

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    64,     2,     2,    70,    56,    61,     2,
      67,    68,    53,    51,    71,    52,     2,    54,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    59,    69,
      75,    57,    76,    58,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    73,     2,    74,    55,     2,    60,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    65,    72,    66,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    62,    63
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     5,     7,     8,     9,    15,    16,    17,
      23,    24,    25,    31,    35,    42,    45,    47,    50,    53,
      56,    60,    66,    74,    84,    90,    98,   107,   114,   115,
     116,   117,   130,   134,   137,   140,   142,   144,   145,   147,
     151,   153,   157,   159,   163,   165,   169,   170,   173,   174,
     176,   178,   182,   184,   188,   190,   192,   196,   198,   200,
     202,   204,   206,   208,   210,   215,   222,   229,   236,   244,
     249,   253,   262,   267,   274,   282,   287,   292,   297,   301,
     303,   308,   312,   314,   320,   324,   328,   331,   334,   338,
     341,   344,   348,   352,   356,   360,   364,   368,   372,   375,
     379,   383,   386,   390,   394,   398,   402,   406,   410,   416,
     420,   425,   430,   436
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      78,     0,    -1,    79,    -1,    89,    -1,    -1,    -1,    11,
      80,    94,    81,    89,    -1,    -1,    -1,    12,    82,    94,
      83,    89,    -1,    -1,    -1,    13,    84,    94,    85,    89,
      -1,    14,    93,    89,    -1,     3,    99,    65,    92,    66,
      89,    -1,   102,    89,    -1,    91,    -1,    27,    89,    -1,
      29,    89,    -1,    28,    89,    -1,    28,   102,    89,    -1,
      25,    67,    96,    68,    79,    -1,    25,    67,    96,    68,
      79,    26,    79,    -1,    24,    67,    96,    69,    96,    69,
      96,    68,    79,    -1,    23,    67,    96,    68,    79,    -1,
      22,    79,    23,    67,    96,    68,    89,    -1,    47,    67,
      96,    68,    59,    57,   102,    89,    -1,     8,    47,    67,
      96,    68,    89,    -1,    -1,    -1,    -1,     7,    47,    86,
      67,    87,    96,    88,    68,    90,    65,    95,    66,    -1,
       9,    47,    89,    -1,    10,    89,    -1,     1,    89,    -1,
      69,    -1,    70,    -1,    -1,    48,    -1,    65,    95,    66,
      -1,    99,    -1,    92,    71,    99,    -1,    47,    -1,    93,
      71,    47,    -1,    46,    -1,    94,    71,    46,    -1,    -1,
      95,    79,    -1,    -1,    97,    -1,   102,    -1,    97,    71,
     102,    -1,   100,    -1,    98,    71,   100,    -1,    46,    -1,
      47,    -1,    99,    57,   102,    -1,    48,    -1,    45,    -1,
       6,    -1,    34,    -1,    35,    -1,    36,    -1,    47,    -1,
      47,    67,    96,    68,    -1,    47,    67,    96,    72,    98,
      68,    -1,    31,    67,    47,    71,    96,    68,    -1,    32,
      67,    47,    71,    96,    68,    -1,    47,    65,    96,    66,
      67,    96,    68,    -1,    37,    67,    99,    68,    -1,    37,
      67,    68,    -1,    33,    67,   102,    71,   102,    71,   102,
      68,    -1,    30,    67,    47,    68,    -1,    30,    67,    47,
      71,    96,    68,    -1,    67,    53,   102,    68,    67,    96,
      68,    -1,    46,    67,    96,    68,    -1,    18,    67,   102,
      68,    -1,    19,    67,   102,    68,    -1,    67,   102,    68,
      -1,    46,    -1,   101,    73,   102,    74,    -1,   101,     4,
      99,    -1,   101,    -1,    67,     3,    99,    68,   101,    -1,
     102,    57,   102,    -1,   102,    50,   102,    -1,   102,    49,
      -1,    49,   102,    -1,    73,    96,    74,    -1,    51,   102,
      -1,    52,   102,    -1,   102,    51,   102,    -1,   102,    52,
     102,    -1,   102,    53,   102,    -1,   102,    54,   102,    -1,
     102,    56,   102,    -1,   102,    55,   102,    -1,   102,    15,
     102,    -1,    64,   102,    -1,   102,    16,   102,    -1,   102,
      17,   102,    -1,    43,   102,    -1,   102,    38,   102,    -1,
     102,    39,   102,    -1,   102,    40,   102,    -1,   102,    41,
     102,    -1,   102,    42,   102,    -1,   102,    44,   102,    -1,
     102,    58,   102,    59,   102,    -1,    75,    96,    76,    -1,
       5,    67,    99,    68,    -1,    20,    67,   102,    68,    -1,
      73,    96,    72,   102,    74,    -1,    60,   102,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   130,   130,   139,   141,   141,   141,   143,   143,   143,
     145,   145,   145,   147,   149,   151,   153,   155,   157,   159,
     161,   163,   165,   167,   169,   171,   173,   175,   177,   177,
     177,   177,   183,   189,   191,   194,   196,   200,   201,   204,
     207,   209,   212,   214,   217,   219,   223,   224,   228,   229,
     232,   234,   237,   239,   242,   244,   247,   250,   252,   254,
     256,   258,   260,   262,   283,   290,   297,   303,   309,   314,
     318,   322,   326,   333,   340,   344,   358,   360,   362,   364,
     377,   387,   390,   392,   394,   396,   398,   400,   402,   404,
     406,   408,   410,   412,   414,   416,   418,   420,   422,   424,
     426,   428,   430,   432,   434,   436,   438,   440,   442,   444,
     446,   448,   450,   452
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "STRUCT", "POINT", "NEWSTRUCT", "ANS", 
  "FDEF", "PFDEF", "MODDEF", "MODEND", "GLOBAL", "MGLOBAL", "LOCAL", 
  "LOCALF", "CMP", "OR", "AND", "CAR", "CDR", "QUOTED", "COLONCOLON", 
  "DO", "WHILE", "FOR", "IF", "ELSE", "BREAK", "RETURN", "CONTINUE", 
  "PARIF", "MAP", "RECMAP", "TIMER", "GF2NGEN", "GFPNGEN", "GFSNGEN", 
  "GETOPT", "FOP_AND", "FOP_OR", "FOP_IMPL", "FOP_REPL", "FOP_EQUIV", 
  "FOP_NOT", "LOP", "FORMULA", "UCASE", "LCASE", "STR", "SELF", "BOPASS", 
  "'+'", "'-'", "'*'", "'/'", "'^'", "'%'", "'='", "'?'", "':'", "'`'", 
  "'&'", "PLUS", "MINUS", "'!'", "'{'", "'}'", "'('", "')'", "';'", "'$'", 
  "','", "'|'", "'['", "']'", "'<'", "'>'", "$accept", "start", "stat", 
  "@1", "@2", "@3", "@4", "@5", "@6", "@7", "@8", "@9", "tail", "desc", 
  "complex", "members", "vars", "pvars", "stats", "node", "_node", 
  "optlist", "rawstr", "opt", "pexpr", "expr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,    43,    45,    42,    47,    94,    37,    61,    63,    58,
      96,    38,   306,   307,    33,   123,   125,    40,    41,    59,
      36,    44,   124,    91,    93,    60,    62
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    77,    78,    79,    80,    81,    79,    82,    83,    79,
      84,    85,    79,    79,    79,    79,    79,    79,    79,    79,
      79,    79,    79,    79,    79,    79,    79,    79,    86,    87,
      88,    79,    79,    79,    79,    89,    89,    90,    90,    91,
      92,    92,    93,    93,    94,    94,    95,    95,    96,    96,
      97,    97,    98,    98,    99,    99,   100,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
     101,   101,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   102
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     1,     0,     0,     5,     0,     0,     5,
       0,     0,     5,     3,     6,     2,     1,     2,     2,     2,
       3,     5,     7,     9,     5,     7,     8,     6,     0,     0,
       0,    12,     3,     2,     2,     1,     1,     0,     1,     3,
       1,     3,     1,     3,     1,     3,     0,     2,     0,     1,
       1,     3,     1,     3,     1,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     4,     6,     6,     6,     7,     4,
       3,     8,     4,     6,     7,     4,     4,     4,     3,     1,
       4,     3,     1,     5,     3,     3,     2,     2,     3,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     2,     3,
       3,     2,     3,     3,     3,     3,     3,     3,     5,     3,
       4,     4,     5,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     0,     0,     0,    59,     0,     0,     0,     0,     4,
       7,    10,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    60,    61,    62,
       0,     0,    58,    79,    63,    57,     0,     0,     0,     0,
       0,    46,     0,    35,    36,    48,    48,     0,     2,     3,
      16,    82,     0,    34,    54,    55,     0,     0,    28,     0,
       0,    33,     0,     0,     0,    42,     0,     0,     0,     0,
       0,    48,    48,    48,    17,    63,    19,     0,    18,     0,
       0,     0,     0,     0,   101,    48,    48,    48,    87,    89,
      90,   113,    98,     0,     0,     0,     0,     0,    49,    50,
       0,     1,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    86,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    15,     0,     0,     0,    48,    32,    44,
       5,     8,    11,     0,    13,     0,     0,     0,     0,     0,
       0,     0,    48,    20,     0,     0,     0,     0,    70,     0,
       0,     0,     0,    39,    47,     0,     0,    78,     0,    88,
       0,   109,    81,     0,    97,    99,   100,   102,   103,   104,
     105,   106,   107,    85,    91,    92,    93,    94,    96,    95,
      84,     0,     0,    40,   110,    29,     0,     0,     0,     0,
       0,    43,    76,    77,   111,    48,     0,    48,     0,     0,
      72,    48,    48,    48,     0,    69,    75,     0,    64,     0,
       0,     0,     0,    51,    80,     0,     0,     0,    48,     0,
      45,     6,     9,    12,     0,    24,     0,    21,    64,     0,
       0,     0,     0,    48,     0,     0,     0,    52,     0,    83,
      48,   112,   108,    14,    41,    30,    27,     0,    48,     0,
      73,    66,    67,     0,     0,     0,    65,     0,     0,     0,
       0,    25,     0,    22,     0,    68,     0,    53,    56,    74,
      37,     0,    71,    26,    38,     0,    23,    46,     0,    31
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    47,   154,    62,   188,    63,   189,    64,   190,   126,
     218,   260,    49,   275,    50,   182,    66,   130,    93,    97,
      98,   235,   236,   237,    51,    99
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -65
static const short yypact[] =
{
     465,   -36,     8,   -64,   -65,   -27,    19,    26,   -36,   -65,
     -65,   -65,    33,   -16,    21,    24,   465,    56,    60,    64,
     -36,   586,   -36,    67,    68,    70,    78,   -65,   -65,   -65,
      80,   696,   -65,    81,    25,   -65,   696,   696,   696,   696,
     696,   -65,   536,   -65,   -65,   696,   696,   133,   -65,   -65,
     -65,    -2,   853,   -65,   -65,   -65,    84,     8,   -65,    86,
     -36,   -65,   100,   100,   100,   -65,   -12,   696,   696,   696,
     132,   696,   696,   696,   -65,    61,   -65,   853,   -65,   112,
     114,   115,   696,   -33,  1173,   696,   696,   696,   118,   -26,
     -26,  1173,   118,   323,     8,   696,   897,    -9,    92,  1113,
     101,   -65,     8,   696,   696,   696,   696,   696,   696,   696,
     696,   696,   696,   -65,   696,   696,   696,   696,   696,   696,
     696,   696,   696,   -65,     8,   108,   111,   696,   -65,   -65,
     109,   109,   109,   134,   -65,   928,   959,   990,   120,   123,
     110,   127,   696,   -65,   -19,   129,   130,   765,   -65,   135,
     139,   116,   -53,   -65,   -65,   140,  1021,   -65,   696,   -65,
     696,   -65,   -65,   249,    23,   262,    87,  1234,  1217,  1192,
     173,    45,    -8,  1113,   -26,   -26,   -45,   -45,   -45,   -45,
    1113,  1083,   -60,   -65,   -65,   -65,   141,   156,   -36,   -36,
     -36,   -65,   -65,   -65,   -65,   696,   465,   696,   465,   -50,
     -65,   696,   696,   696,   696,   -65,   -65,   143,   155,     8,
     138,   149,   734,  1113,   -65,   696,   -36,     8,   696,   -36,
     -65,   -65,   -65,   -65,   150,   -65,   161,   193,   -65,   163,
     164,   165,   809,   696,   177,   -18,   178,   -65,   646,    -2,
     696,   -65,  1143,   -65,   -65,   -65,   -65,   -36,   696,   465,
     -65,   -65,   -65,   696,   168,   696,   -65,     8,   696,   169,
     171,   -65,   172,   -65,  1052,   -65,   853,   -65,  1113,   -65,
     194,   465,   -65,   -65,   -65,   176,   -65,   -65,   394,   -65
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -65,   -65,     1,   -65,   -65,   -65,   -65,   -65,   -65,   -65,
     -65,   -65,     4,   -65,   -65,   -65,   -65,    66,   -34,    79,
     -65,   -65,    30,   -13,    35,     0
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned short yytable[] =
{
      52,    48,   102,    57,   113,    53,   216,   104,   105,   106,
     119,   217,    61,    54,    55,   208,    52,    70,   228,   209,
      58,    77,   209,   113,    74,    76,    78,   117,   118,   119,
     120,    84,    56,    43,    44,   148,    88,    89,    90,    91,
      92,   113,    96,   115,   116,   117,   118,   119,   120,   200,
     256,    67,   201,   257,    54,    55,   123,    43,    44,   133,
     104,   105,   106,   158,   128,   159,    59,   135,   136,   137,
     134,   103,   113,    60,   115,   116,   117,   118,   119,   120,
      65,   143,   147,   107,   108,   109,   110,   125,    68,   112,
      86,    69,    87,    52,   113,   156,   115,   116,   117,   118,
     119,   120,   104,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   149,   173,   174,   175,   176,   177,   178,
     179,   180,   181,    71,   155,   100,    86,    72,   142,   131,
     132,    73,   162,   101,    79,    80,   113,    81,   115,   116,
     117,   118,   119,   120,     4,    82,   129,    83,    85,   124,
     139,   140,   141,   127,   183,   138,    13,    14,   212,   144,
     213,   145,   146,   160,   150,   151,   152,   113,    23,    24,
      25,    26,    27,    28,    29,    30,   184,   161,   185,   197,
     187,   191,   207,    32,    33,    75,    35,   195,   104,   105,
     106,   196,   221,   222,   223,   198,    52,   225,    52,   227,
     202,   203,   220,   205,   232,   238,   186,   206,   210,   219,
     233,   107,   108,   109,   234,   242,   240,   112,   247,   249,
     243,   199,   113,   246,   115,   116,   117,   118,   119,   120,
     248,   250,   251,   252,   255,   258,   265,   269,    96,   270,
     271,   277,   274,   278,   267,   239,     0,   244,     0,    52,
     263,   261,     0,   264,     0,   266,     0,     0,   268,     0,
       0,     0,     0,     0,   104,   105,   106,     0,     0,     0,
     273,    52,   276,     0,   224,     0,   226,   104,    52,   106,
     229,   230,   231,     0,     0,     0,     0,   107,   108,   109,
     110,   111,     0,   112,     0,     0,     0,   245,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,     0,     0,
       0,   113,   254,   115,   116,   117,   118,   119,   120,   259,
       0,     0,     0,   214,     1,     0,     2,   262,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,     0,     0,
       0,    13,    14,    15,     0,    16,    17,    18,    19,     0,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,     0,     0,     0,     0,     0,    31,     0,    32,    33,
      34,    35,    36,     0,    37,    38,     0,     0,     0,     0,
       0,     0,     0,    39,     0,     0,     0,    40,    41,   153,
      42,     0,    43,    44,     0,     1,    45,     2,    46,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,     0,
       0,     0,    13,    14,    15,     0,    16,    17,    18,    19,
       0,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,     0,     0,     0,     0,     0,    31,     0,    32,
      33,    34,    35,    36,     0,    37,    38,     0,     0,     0,
       0,     0,     0,     0,    39,     0,     0,     0,    40,    41,
     279,    42,     0,    43,    44,     0,     1,    45,     2,    46,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
       0,     0,     0,    13,    14,    15,     0,    16,    17,    18,
      19,     0,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,     0,     0,     0,     0,     0,    31,     0,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,     0,     0,     0,    39,     0,     0,     0,    40,
      41,     0,    42,     0,    43,    44,     0,     0,    45,    94,
      46,     3,     4,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    13,    14,    15,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    23,    24,    25,    26,
      27,    28,    29,    30,     0,     0,     0,     0,     0,    31,
       0,    32,    33,    75,    35,    36,     0,    37,    38,    95,
       0,     3,     4,     0,     0,     0,    39,     0,     0,     0,
      40,     0,     0,    42,    13,    14,    15,     0,     0,    45,
       0,    46,     0,     0,     0,     0,    23,    24,    25,    26,
      27,    28,    29,    30,     0,     0,     0,     0,     0,    31,
       0,    32,    33,    75,    35,    36,     0,    37,    38,     0,
       0,     0,     0,     0,     0,     0,    39,     0,     0,     0,
      40,     3,     4,    42,     0,    43,    44,     0,     0,    45,
       0,    46,     0,     0,    13,    14,    15,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    23,    24,    25,    26,
      27,    28,    29,    30,     0,     0,     0,     0,     0,    31,
       0,    32,    33,    75,    35,    36,     0,    37,    38,    95,
       0,     3,     4,     0,     0,     0,    39,     0,     0,     0,
      40,     0,     0,    42,    13,    14,    15,     0,     0,    45,
       0,    46,     0,     0,     0,     0,    23,    24,    25,    26,
      27,    28,    29,    30,     0,     0,     0,     0,     0,    31,
       0,    32,    33,    75,    35,    36,     0,    37,    38,   104,
     105,   106,     0,     0,     0,     0,    39,     0,     0,     0,
      40,     0,     0,    42,     0,     0,     0,     0,     0,    45,
       0,    46,   107,   108,   109,   110,   111,     0,   112,     0,
     104,   105,   106,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   107,   108,   109,   110,   111,   241,   112,
       0,     0,     0,     0,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   104,   105,   106,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   204,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   107,   108,   109,
     110,   111,     0,   112,     0,     0,     0,     0,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   104,   105,
     106,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     253,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   107,   108,   109,   110,   111,     0,   112,     0,     0,
       0,     0,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   104,   105,   106,     0,     0,     0,     0,     0,
       0,     0,    43,    44,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   107,   108,   109,   110,   111,
       0,   112,     0,   104,   105,   106,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   157,   107,   108,   109,   110,
     111,     0,   112,     0,   104,   105,   106,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   192,   107,   108,   109,
     110,   111,     0,   112,     0,   104,   105,   106,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   193,   107,   108,
     109,   110,   111,     0,   112,     0,   104,   105,   106,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   194,   107,
     108,   109,   110,   111,     0,   112,     0,   104,   105,   106,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   211,
     107,   108,   109,   110,   111,     0,   112,     0,   104,   105,
     106,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     272,   107,   108,   109,   110,   111,     0,   112,   104,   105,
     106,     0,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   215,     0,     0,     0,     0,     0,     0,     0,
       0,   107,   108,   109,   110,   111,     0,   112,   104,   105,
     106,     0,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   107,   108,   109,   110,   111,     0,   112,   104,   105,
     106,     0,   113,     0,   115,   116,   117,   118,   119,   120,
       0,   122,     0,     0,     0,     0,     0,   104,   105,   106,
       0,   107,   108,   109,   110,   111,     0,   112,     0,     0,
       0,     0,   113,     0,   115,   116,   117,   118,   119,   120,
     107,   108,   104,   105,   106,     0,   112,     0,     0,     0,
       0,   113,     0,   115,   116,   117,   118,   119,   120,   104,
     105,   106,     0,     0,     0,   107,     0,     0,     0,     0,
       0,   112,     0,     0,     0,     0,   113,     0,   115,   116,
     117,   118,   119,   120,     0,     0,     0,     0,   112,     0,
       0,     0,     0,   113,     0,   115,   116,   117,   118,   119,
     120
};

static const short yycheck[] =
{
       0,     0,     4,    67,    49,     1,    66,    15,    16,    17,
      55,    71,     8,    46,    47,    68,    16,    16,    68,    72,
      47,    21,    72,    49,    20,    21,    22,    53,    54,    55,
      56,    31,     2,    69,    70,    68,    36,    37,    38,    39,
      40,    49,    42,    51,    52,    53,    54,    55,    56,    68,
      68,    67,    71,    71,    46,    47,    52,    69,    70,    71,
      15,    16,    17,    72,    60,    74,    47,    67,    68,    69,
      66,    73,    49,    47,    51,    52,    53,    54,    55,    56,
      47,    77,    82,    38,    39,    40,    41,    57,    67,    44,
      65,    67,    67,    93,    49,    95,    51,    52,    53,    54,
      55,    56,    15,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,    83,   114,   115,   116,   117,   118,   119,
     120,   121,   122,    67,    94,    46,    65,    67,    67,    63,
      64,    67,   102,     0,    67,    67,    49,    67,    51,    52,
      53,    54,    55,    56,     6,    67,    46,    67,    67,    65,
      71,    72,    73,    67,   124,    23,    18,    19,   158,    47,
     160,    47,    47,    71,    85,    86,    87,    49,    30,    31,
      32,    33,    34,    35,    36,    37,    68,    76,    67,    69,
      71,    47,    66,    45,    46,    47,    48,    67,    15,    16,
      17,    68,   188,   189,   190,    68,   196,   196,   198,   198,
      71,    71,    46,    68,   204,    67,   127,    68,    68,    68,
      67,    38,    39,    40,    59,   215,    67,    44,    68,    26,
     216,   142,    49,   219,    51,    52,    53,    54,    55,    56,
      69,    68,    68,    68,    57,    57,    68,    68,   238,    68,
      68,    65,    48,   277,   257,   210,    -1,   217,    -1,   249,
     249,   247,    -1,   253,    -1,   255,    -1,    -1,   258,    -1,
      -1,    -1,    -1,    -1,    15,    16,    17,    -1,    -1,    -1,
     266,   271,   271,    -1,   195,    -1,   197,    15,   278,    17,
     201,   202,   203,    -1,    -1,    -1,    -1,    38,    39,    40,
      41,    42,    -1,    44,    -1,    -1,    -1,   218,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    -1,
      -1,    49,   233,    51,    52,    53,    54,    55,    56,   240,
      -1,    -1,    -1,    74,     1,    -1,     3,   248,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    18,    19,    20,    -1,    22,    23,    24,    25,    -1,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    -1,    -1,    -1,    -1,    -1,    43,    -1,    45,    46,
      47,    48,    49,    -1,    51,    52,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    60,    -1,    -1,    -1,    64,    65,    66,
      67,    -1,    69,    70,    -1,     1,    73,     3,    75,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    18,    19,    20,    -1,    22,    23,    24,    25,
      -1,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    -1,    -1,    -1,    -1,    -1,    43,    -1,    45,
      46,    47,    48,    49,    -1,    51,    52,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    60,    -1,    -1,    -1,    64,    65,
      66,    67,    -1,    69,    70,    -1,     1,    73,     3,    75,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    18,    19,    20,    -1,    22,    23,    24,
      25,    -1,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    -1,    -1,    -1,    -1,    -1,    43,    -1,
      45,    46,    47,    48,    49,    -1,    51,    52,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    60,    -1,    -1,    -1,    64,
      65,    -1,    67,    -1,    69,    70,    -1,    -1,    73,     3,
      75,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    18,    19,    20,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,    43,
      -1,    45,    46,    47,    48,    49,    -1,    51,    52,    53,
      -1,     5,     6,    -1,    -1,    -1,    60,    -1,    -1,    -1,
      64,    -1,    -1,    67,    18,    19,    20,    -1,    -1,    73,
      -1,    75,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,    43,
      -1,    45,    46,    47,    48,    49,    -1,    51,    52,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    60,    -1,    -1,    -1,
      64,     5,     6,    67,    -1,    69,    70,    -1,    -1,    73,
      -1,    75,    -1,    -1,    18,    19,    20,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,    43,
      -1,    45,    46,    47,    48,    49,    -1,    51,    52,    53,
      -1,     5,     6,    -1,    -1,    -1,    60,    -1,    -1,    -1,
      64,    -1,    -1,    67,    18,    19,    20,    -1,    -1,    73,
      -1,    75,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,    43,
      -1,    45,    46,    47,    48,    49,    -1,    51,    52,    15,
      16,    17,    -1,    -1,    -1,    -1,    60,    -1,    -1,    -1,
      64,    -1,    -1,    67,    -1,    -1,    -1,    -1,    -1,    73,
      -1,    75,    38,    39,    40,    41,    42,    -1,    44,    -1,
      15,    16,    17,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    38,    39,    40,    41,    42,    74,    44,
      -1,    -1,    -1,    -1,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    15,    16,    17,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    71,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,    39,    40,
      41,    42,    -1,    44,    -1,    -1,    -1,    -1,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    15,    16,
      17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    38,    39,    40,    41,    42,    -1,    44,    -1,    -1,
      -1,    -1,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    15,    16,    17,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    38,    39,    40,    41,    42,
      -1,    44,    -1,    15,    16,    17,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    68,    38,    39,    40,    41,
      42,    -1,    44,    -1,    15,    16,    17,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    68,    38,    39,    40,
      41,    42,    -1,    44,    -1,    15,    16,    17,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    38,    39,
      40,    41,    42,    -1,    44,    -1,    15,    16,    17,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    38,
      39,    40,    41,    42,    -1,    44,    -1,    15,    16,    17,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,
      38,    39,    40,    41,    42,    -1,    44,    -1,    15,    16,
      17,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    38,    39,    40,    41,    42,    -1,    44,    15,    16,
      17,    -1,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    38,    39,    40,    41,    42,    -1,    44,    15,    16,
      17,    -1,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    38,    39,    40,    41,    42,    -1,    44,    15,    16,
      17,    -1,    49,    -1,    51,    52,    53,    54,    55,    56,
      -1,    58,    -1,    -1,    -1,    -1,    -1,    15,    16,    17,
      -1,    38,    39,    40,    41,    42,    -1,    44,    -1,    -1,
      -1,    -1,    49,    -1,    51,    52,    53,    54,    55,    56,
      38,    39,    15,    16,    17,    -1,    44,    -1,    -1,    -1,
      -1,    49,    -1,    51,    52,    53,    54,    55,    56,    15,
      16,    17,    -1,    -1,    -1,    38,    -1,    -1,    -1,    -1,
      -1,    44,    -1,    -1,    -1,    -1,    49,    -1,    51,    52,
      53,    54,    55,    56,    -1,    -1,    -1,    -1,    44,    -1,
      -1,    -1,    -1,    49,    -1,    51,    52,    53,    54,    55,
      56
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     3,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    18,    19,    20,    22,    23,    24,    25,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    43,    45,    46,    47,    48,    49,    51,    52,    60,
      64,    65,    67,    69,    70,    73,    75,    78,    79,    89,
      91,   101,   102,    89,    46,    47,    99,    67,    47,    47,
      47,    89,    80,    82,    84,    47,    93,    67,    67,    67,
      79,    67,    67,    67,    89,    47,    89,   102,    89,    67,
      67,    67,    67,    67,   102,    67,    65,    67,   102,   102,
     102,   102,   102,    95,     3,    53,   102,    96,    97,   102,
      96,     0,     4,    73,    15,    16,    17,    38,    39,    40,
      41,    42,    44,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    89,    65,    99,    86,    67,    89,    46,
      94,    94,    94,    71,    89,   102,   102,   102,    23,    96,
      96,    96,    67,    89,    47,    47,    47,   102,    68,    99,
      96,    96,    96,    66,    79,    99,   102,    68,    72,    74,
      71,    76,    99,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,    92,    99,    68,    67,    96,    71,    81,    83,
      85,    47,    68,    68,    68,    67,    68,    69,    68,    96,
      68,    71,    71,    71,    71,    68,    68,    66,    68,    72,
      68,    68,   102,   102,    74,    59,    66,    71,    87,    68,
      46,    89,    89,    89,    96,    79,    96,    79,    68,    96,
      96,    96,   102,    67,    59,    98,    99,   100,    67,   101,
      67,    74,   102,    89,    99,    96,    89,    68,    69,    26,
      68,    68,    68,    71,    96,    57,    68,    71,    57,    96,
      88,    89,    96,    79,   102,    68,   102,   100,   102,    68,
      68,    68,    68,    89,    48,    90,    79,    65,    95,    66
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 131 "parse.y"
    { 
				parse_snode = yyvsp[0].s; 
				if ( yychar >= 0 ) 
					fprintf(stderr,
						"Warning: a token was wasted after an 'if' statement without 'else'.\n");
				YYACCEPT; 
			}
    break;

  case 3:
#line 140 "parse.y"
    { yyval.s = 0; }
    break;

  case 4:
#line 141 "parse.y"
    { gdef=1; }
    break;

  case 5:
#line 141 "parse.y"
    { gdef=0; }
    break;

  case 6:
#line 142 "parse.y"
    { yyval.s = 0; NOPR; }
    break;

  case 7:
#line 143 "parse.y"
    { mgdef=1; }
    break;

  case 8:
#line 143 "parse.y"
    { mgdef=0; }
    break;

  case 9:
#line 144 "parse.y"
    { yyval.s = 0; NOPR; }
    break;

  case 10:
#line 145 "parse.y"
    { ldef=1; }
    break;

  case 11:
#line 145 "parse.y"
    { ldef=0; }
    break;

  case 12:
#line 146 "parse.y"
    { yyval.s = 0; NOPR; }
    break;

  case 13:
#line 148 "parse.y"
    { appenduflist(yyvsp[-1].n); yyval.s = 0; NOPR; }
    break;

  case 14:
#line 150 "parse.y"
    { structdef(yyvsp[-4].p,yyvsp[-2].n); yyval.s = 0; NOPR; }
    break;

  case 15:
#line 152 "parse.y"
    { yyval.s = mksnode(1,S_SINGLE,yyvsp[-1].f); }
    break;

  case 16:
#line 154 "parse.y"
    { yyval.s = yyvsp[0].s; }
    break;

  case 17:
#line 156 "parse.y"
    { yyval.s = mksnode(0,S_BREAK); }
    break;

  case 18:
#line 158 "parse.y"
    { yyval.s = mksnode(0,S_CONTINUE); }
    break;

  case 19:
#line 160 "parse.y"
    { yyval.s = mksnode(1,S_RETURN,0); }
    break;

  case 20:
#line 162 "parse.y"
    { yyval.s = mksnode(1,S_RETURN,yyvsp[-1].f); }
    break;

  case 21:
#line 164 "parse.y"
    { yyval.s = mksnode(4,S_IFELSE,yyvsp[-4].i,yyvsp[-2].n,yyvsp[0].s,0); yyvsp[0].s?yyval.s->ln=yyvsp[0].s->ln:0; NOPR; }
    break;

  case 22:
#line 166 "parse.y"
    { yyval.s = mksnode(4,S_IFELSE,yyvsp[-6].i,yyvsp[-4].n,yyvsp[-2].s,yyvsp[0].s); yyvsp[0].s?yyval.s->ln=yyvsp[0].s->ln:0; NOPR; }
    break;

  case 23:
#line 168 "parse.y"
    { yyval.s = mksnode(5,S_FOR,yyvsp[-8].i,yyvsp[-6].n,yyvsp[-4].n?yyvsp[-4].n:ONENODE,yyvsp[-2].n,yyvsp[0].s); yyvsp[0].s?yyval.s->ln=yyvsp[0].s->ln:0; NOPR; }
    break;

  case 24:
#line 170 "parse.y"
    { yyval.s = mksnode(5,S_FOR,yyvsp[-4].i,0,yyvsp[-2].n,0,yyvsp[0].s); yyvsp[0].s?yyval.s->ln=yyvsp[0].s->ln:0; NOPR; }
    break;

  case 25:
#line 172 "parse.y"
    { yyval.s = mksnode(3,S_DO,yyvsp[-6].i,yyvsp[-5].s,yyvsp[-2].n); NOPR; }
    break;

  case 26:
#line 174 "parse.y"
    { yyval.s = mksnode(3,S_PFDEF,yyvsp[-7].p,yyvsp[-5].n,yyvsp[-1].f); NOPR; }
    break;

  case 27:
#line 176 "parse.y"
    { yyval.s = mksnode(3,S_PFDEF,yyvsp[-4].p,yyvsp[-2].n,0); NOPR; }
    break;

  case 28:
#line 177 "parse.y"
    { mkpvs(yyvsp[0].p); }
    break;

  case 29:
#line 177 "parse.y"
    { ldef = 1; }
    break;

  case 30:
#line 177 "parse.y"
    { ldef = -1; }
    break;

  case 31:
#line 178 "parse.y"
    {
				mkuf(yyvsp[-10].p,asir_infile->name,yyvsp[-6].n,
					mksnode(1,S_CPLX,yyvsp[-1].n),yyvsp[-11].i,asir_infile->ln,yyvsp[-3].p,CUR_MODULE); 
				yyval.s = 0; NOPR;
			}
    break;

  case 32:
#line 184 "parse.y"
    { 
				CUR_MODULE = mkmodule(yyvsp[-1].p);
				MPVS = CUR_MODULE->pvs; 
				yyval.s = mksnode(1,S_MODULE,CUR_MODULE); NOPR;
			}
    break;

  case 33:
#line 190 "parse.y"
    { CUR_MODULE = 0; MPVS = 0; yyval.s = mksnode(1,S_MODULE,0); NOPR; }
    break;

  case 34:
#line 192 "parse.y"
    { yyerrok; yyval.s = 0; }
    break;

  case 35:
#line 195 "parse.y"
    { if ( main_parser ) prresult = 1; }
    break;

  case 36:
#line 197 "parse.y"
    { if ( main_parser ) prresult = 0; }
    break;

  case 37:
#line 200 "parse.y"
    { yyval.p = 0; }
    break;

  case 38:
#line 202 "parse.y"
    { yyval.p = yyvsp[0].p; }
    break;

  case 39:
#line 205 "parse.y"
    { yyval.s = mksnode(1,S_CPLX,yyvsp[-1].n); }
    break;

  case 40:
#line 208 "parse.y"
    { MKNODE(yyval.n,yyvsp[0].p,0); }
    break;

  case 41:
#line 210 "parse.y"
    { appendtonode(yyvsp[-2].n,yyvsp[0].p,&yyval.n); }
    break;

  case 42:
#line 213 "parse.y"
    { MKNODE(yyval.n,yyvsp[0].p,0); }
    break;

  case 43:
#line 215 "parse.y"
    { appendtonode(yyvsp[-2].n,yyvsp[0].p,&yyval.n); }
    break;

  case 44:
#line 218 "parse.y"
    { val = (pointer)makepvar(yyvsp[0].p); MKNODE(yyval.n,val,0); }
    break;

  case 45:
#line 220 "parse.y"
    { appendtonode(yyvsp[-2].n,(pointer)makepvar(yyvsp[0].p),&yyval.n); }
    break;

  case 46:
#line 223 "parse.y"
    { yyval.n = 0; }
    break;

  case 47:
#line 225 "parse.y"
    { appendtonode(yyvsp[-1].n,(pointer)yyvsp[0].s,&yyval.n); }
    break;

  case 48:
#line 228 "parse.y"
    { yyval.n = 0; }
    break;

  case 49:
#line 230 "parse.y"
    { yyval.n = yyvsp[0].n; }
    break;

  case 50:
#line 233 "parse.y"
    { MKNODE(yyval.n,yyvsp[0].f,0); }
    break;

  case 51:
#line 235 "parse.y"
    { appendtonode(yyvsp[-2].n,(pointer)yyvsp[0].f,&yyval.n); }
    break;

  case 52:
#line 238 "parse.y"
    { MKNODE(yyval.n,yyvsp[0].f,0); }
    break;

  case 53:
#line 240 "parse.y"
    { appendtonode(yyvsp[-2].n,(pointer)yyvsp[0].f,&yyval.n); }
    break;

  case 54:
#line 243 "parse.y"
    { yyval.p = yyvsp[0].p; }
    break;

  case 55:
#line 245 "parse.y"
    { yyval.p = yyvsp[0].p; }
    break;

  case 56:
#line 248 "parse.y"
    { yyval.f = mkfnode(2,I_OPT,yyvsp[-2].p,yyvsp[0].f); }
    break;

  case 57:
#line 251 "parse.y"
    { yyval.f = mkfnode(1,I_STR,yyvsp[0].p); }
    break;

  case 58:
#line 253 "parse.y"
    { yyval.f = mkfnode(1,I_FORMULA,yyvsp[0].p); }
    break;

  case 59:
#line 255 "parse.y"
    { yyval.f = mkfnode(1,I_ANS,yyvsp[0].i); }
    break;

  case 60:
#line 257 "parse.y"
    { yyval.f = mkfnode(0,I_GF2NGEN); }
    break;

  case 61:
#line 259 "parse.y"
    { yyval.f = mkfnode(0,I_GFPNGEN); }
    break;

  case 62:
#line 261 "parse.y"
    { yyval.f = mkfnode(0,I_GFSNGEN); }
    break;

  case 63:
#line 263 "parse.y"
    {
				FUNC f;

				searchf(noargsysf,yyvsp[0].p,&f);
				if ( f )
					 yyval.f = mkfnode(2,I_FUNC,f,0);
				else {
					searchc(yyvsp[0].p,&f);
					if ( f )
						yyval.f = mkfnode(2,I_FUNC,f,mkfnode(1,I_LIST,0));
					else {
						gen_searchf_searchonly(yyvsp[0].p,(FUNC *)&f);
						if ( f )
							makesrvar(f,(P *)&val);
						else
							makevar(yyvsp[0].p,(P *)&val);
						yyval.f = mkfnode(1,I_FORMULA,val);
					}
				}
			}
    break;

  case 64:
#line 284 "parse.y"
    {
				gen_searchf(yyvsp[-3].p,(FUNC *)&val);
				print_crossref(val);
				yyval.f = mkfnode(2,I_FUNC,val,mkfnode(1,I_LIST,yyvsp[-1].n));
			}
    break;

  case 65:
#line 291 "parse.y"
    {
				gen_searchf(yyvsp[-5].p,(FUNC *)&val);
				print_crossref(val);
				yyval.f = mkfnode(3,I_FUNC_OPT,val,
					mkfnode(1,I_LIST,yyvsp[-3].n),mkfnode(1,I_LIST,yyvsp[-1].n));
			}
    break;

  case 66:
#line 298 "parse.y"
    {
				gen_searchf(yyvsp[-3].p,(FUNC *)&val);
				print_crossref(val);
				yyval.f = mkfnode(2,I_MAP,val,mkfnode(1,I_LIST,yyvsp[-1].n));
			}
    break;

  case 67:
#line 304 "parse.y"
    {
				gen_searchf(yyvsp[-3].p,(FUNC *)&val);
				print_crossref(val);
				yyval.f = mkfnode(2,I_RECMAP,val,mkfnode(1,I_LIST,yyvsp[-1].n));
			}
    break;

  case 68:
#line 310 "parse.y"
    {
				searchpf(yyvsp[-6].p,(FUNC *)&val);
				yyval.f = mkfnode(3,I_PFDERIV,val,mkfnode(1,I_LIST,yyvsp[-1].n),mkfnode(1,I_LIST,yyvsp[-4].n));
			}
    break;

  case 69:
#line 315 "parse.y"
    {
				yyval.f = mkfnode(2,I_GETOPT,yyvsp[-1].p);
			}
    break;

  case 70:
#line 319 "parse.y"
    {
				yyval.f = mkfnode(2,I_GETOPT,0);
			}
    break;

  case 71:
#line 323 "parse.y"
    {
				yyval.f = mkfnode(3,I_TIMER,yyvsp[-5].f,yyvsp[-3].f,yyvsp[-1].f);
			}
    break;

  case 72:
#line 327 "parse.y"
    {
				searchf(parif,yyvsp[-1].p,(FUNC *)&val);
				if ( !val )
					mkparif(yyvsp[-1].p,(FUNC *)&val);
				yyval.f = mkfnode(2,I_FUNC,val,0);
			}
    break;

  case 73:
#line 334 "parse.y"
    {
				searchf(parif,yyvsp[-3].p,(FUNC *)&val);
				if ( !val )
					mkparif(yyvsp[-3].p,(FUNC *)&val);
				yyval.f = mkfnode(2,I_FUNC,val,mkfnode(1,I_LIST,yyvsp[-1].n));
			}
    break;

  case 74:
#line 341 "parse.y"
    {
				yyval.f = mkfnode(2,I_IFUNC,yyvsp[-4].f,mkfnode(1,I_LIST,yyvsp[-1].n));
			}
    break;

  case 75:
#line 345 "parse.y"
    {
				if ( main_parser || allow_create_var )
					t = mkfnode(2,I_PVAR,makepvar(yyvsp[-3].p),0);
				else {
					ind = searchpvar(yyvsp[-3].p);
					if ( ind == -1 ) {
						fprintf(stderr,"%s : no such variable.\n",yyvsp[-3].p);
						YYABORT;
					} else
						t = mkfnode(2,I_PVAR,ind,0);
				}
				yyval.f = mkfnode(2,I_IFUNC,t,mkfnode(1,I_LIST,yyvsp[-1].n));
			}
    break;

  case 76:
#line 359 "parse.y"
    { yyval.f = mkfnode(1,I_CAR,yyvsp[-1].f); }
    break;

  case 77:
#line 361 "parse.y"
    { yyval.f = mkfnode(1,I_CDR,yyvsp[-1].f); }
    break;

  case 78:
#line 363 "parse.y"
    { yyval.f = mkfnode(1,I_PAREN,yyvsp[-1].f); }
    break;

  case 79:
#line 365 "parse.y"
    { 
				if ( main_parser || allow_create_var )
					yyval.f = mkfnode(2,I_PVAR,makepvar(yyvsp[0].p),0);
				else {
					ind = searchpvar(yyvsp[0].p);
					if ( ind == -1 ) {
						fprintf(stderr,"%s : no such variable.\n",yyvsp[0].p);
						YYABORT;
					} else
						yyval.f = mkfnode(2,I_PVAR,ind,0);
				}
			}
    break;

  case 80:
#line 378 "parse.y"
    {
				if ( yyvsp[-3].f->id == I_PVAR || yyvsp[-3].f->id == I_INDEX ) {
					appendtonode((NODE)yyvsp[-3].f->arg[1],(pointer)yyvsp[-1].f,&a);
					yyvsp[-3].f->arg[1] = (pointer)a; yyval.f = yyvsp[-3].f;
				} else {
					MKNODE(a,yyvsp[-1].f,0);
					yyval.f = mkfnode(2,I_INDEX,(pointer)yyvsp[-3].f,a);
				}
			}
    break;

  case 81:
#line 388 "parse.y"
    { yyval.f = mkfnode(2,I_POINT,yyvsp[-2].f,yyvsp[0].p); }
    break;

  case 82:
#line 391 "parse.y"
    { yyval.f = yyvsp[0].f; }
    break;

  case 83:
#line 393 "parse.y"
    { yyval.f = mkfnode(3,I_CAST,structtoindex(yyvsp[-2].p),yyvsp[0].f,0); }
    break;

  case 84:
#line 395 "parse.y"
    { yyval.f = mkfnode(2,I_ASSPVAR,yyvsp[-2].f,yyvsp[0].f); }
    break;

  case 85:
#line 397 "parse.y"
    { yyval.f = mkfnode(2,I_ASSPVAR,yyvsp[-2].f,mkfnode(3,I_BOP,yyvsp[-1].p,yyvsp[-2].f,yyvsp[0].f)); }
    break;

  case 86:
#line 399 "parse.y"
    { yyval.f = mkfnode(2,I_POSTSELF,yyvsp[0].p,yyvsp[-1].f); }
    break;

  case 87:
#line 401 "parse.y"
    { yyval.f = mkfnode(2,I_PRESELF,yyvsp[-1].p,yyvsp[0].f); }
    break;

  case 88:
#line 403 "parse.y"
    { yyval.f = mkfnode(1,I_LIST,yyvsp[-1].n); }
    break;

  case 89:
#line 405 "parse.y"
    { yyval.f = yyvsp[0].f; }
    break;

  case 90:
#line 407 "parse.y"
    { yyval.f = mkfnode(1,I_MINUS,yyvsp[0].f); }
    break;

  case 91:
#line 409 "parse.y"
    { yyval.f = mkfnode(3,I_BOP,yyvsp[-1].p,yyvsp[-2].f,yyvsp[0].f); }
    break;

  case 92:
#line 411 "parse.y"
    { yyval.f = mkfnode(3,I_BOP,yyvsp[-1].p,yyvsp[-2].f,yyvsp[0].f); }
    break;

  case 93:
#line 413 "parse.y"
    { yyval.f = mkfnode(3,I_BOP,yyvsp[-1].p,yyvsp[-2].f,yyvsp[0].f); }
    break;

  case 94:
#line 415 "parse.y"
    { yyval.f = mkfnode(3,I_BOP,yyvsp[-1].p,yyvsp[-2].f,yyvsp[0].f); }
    break;

  case 95:
#line 417 "parse.y"
    { yyval.f = mkfnode(3,I_BOP,yyvsp[-1].p,yyvsp[-2].f,yyvsp[0].f); }
    break;

  case 96:
#line 419 "parse.y"
    { yyval.f = mkfnode(3,I_BOP,yyvsp[-1].p,yyvsp[-2].f,yyvsp[0].f); }
    break;

  case 97:
#line 421 "parse.y"
    { yyval.f = mkfnode(3,I_COP,yyvsp[-1].i,yyvsp[-2].f,yyvsp[0].f); }
    break;

  case 98:
#line 423 "parse.y"
    { yyval.f = mkfnode(1,I_NOT,yyvsp[0].f); }
    break;

  case 99:
#line 425 "parse.y"
    { yyval.f = mkfnode(2,I_OR,yyvsp[-2].f,yyvsp[0].f); }
    break;

  case 100:
#line 427 "parse.y"
    { yyval.f = mkfnode(2,I_AND,yyvsp[-2].f,yyvsp[0].f); }
    break;

  case 101:
#line 429 "parse.y"
    { yyval.f = mkfnode(3,I_LOP,yyvsp[-1].i,yyvsp[0].f,0); }
    break;

  case 102:
#line 431 "parse.y"
    { yyval.f = mkfnode(3,I_LOP,yyvsp[-1].i,yyvsp[-2].f,yyvsp[0].f); }
    break;

  case 103:
#line 433 "parse.y"
    { yyval.f = mkfnode(3,I_LOP,yyvsp[-1].i,yyvsp[-2].f,yyvsp[0].f); }
    break;

  case 104:
#line 435 "parse.y"
    { yyval.f = mkfnode(3,I_LOP,yyvsp[-1].i,yyvsp[-2].f,yyvsp[0].f); }
    break;

  case 105:
#line 437 "parse.y"
    { yyval.f = mkfnode(3,I_LOP,yyvsp[-1].i,yyvsp[-2].f,yyvsp[0].f); }
    break;

  case 106:
#line 439 "parse.y"
    { yyval.f = mkfnode(3,I_LOP,yyvsp[-1].i,yyvsp[-2].f,yyvsp[0].f); }
    break;

  case 107:
#line 441 "parse.y"
    { yyval.f = mkfnode(3,I_LOP,yyvsp[-1].i,yyvsp[-2].f,yyvsp[0].f); }
    break;

  case 108:
#line 443 "parse.y"
    { yyval.f = mkfnode(3,I_CE,yyvsp[-4].f,yyvsp[-2].f,yyvsp[0].f); }
    break;

  case 109:
#line 445 "parse.y"
    { yyval.f = mkfnode(1,I_EV,yyvsp[-1].n); }
    break;

  case 110:
#line 447 "parse.y"
    { yyval.f = mkfnode(1,I_NEWCOMP,(int)structtoindex(yyvsp[-1].p)); }
    break;

  case 111:
#line 449 "parse.y"
    { MKQUOTE(quote,yyvsp[-1].f); yyval.f = mkfnode(1,I_FORMULA,(pointer)quote); }
    break;

  case 112:
#line 451 "parse.y"
    { yyval.f = mkfnode(2,I_CONS,yyvsp[-3].n,yyvsp[-1].f); }
    break;

  case 113:
#line 453 "parse.y"
    { MKQUOTE(quote,yyvsp[0].f); yyval.f = mkfnode(1,I_FORMULA,(pointer)quote); }
    break;


    }

/* Line 999 of yacc.c.  */
#line 2208 "y.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 455 "parse.y"


