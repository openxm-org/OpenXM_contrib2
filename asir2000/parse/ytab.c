
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 50 "parse.y"

#define malloc(x) Risa_GC_malloc(x)
#define realloc(x,y) Risa_GC_realloc(x,y)
#define free(x) Risa_GC_free(x)

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


/* Line 189 of yacc.c  */
#line 105 "y.tab.c"

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

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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

/* Line 214 of yacc.c  */
#line 83 "parse.y"

	FNODE f;
	SNODE s;
	NODE n;
	NODE2 n2;
	int i;
	pointer p;



/* Line 214 of yacc.c  */
#line 256 "y.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 268 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  101
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1284

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  77
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  26
/* YYNRULES -- Number of rules.  */
#define YYNRULES  114
/* YYNRULES -- Number of states.  */
#define YYNSTATES  283

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   307

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
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
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     8,     9,    15,    16,    17,
      23,    24,    25,    31,    35,    42,    45,    47,    50,    53,
      56,    60,    66,    74,    84,    90,    98,   107,   114,   115,
     116,   117,   130,   134,   137,   140,   142,   144,   145,   147,
     151,   153,   157,   159,   163,   165,   169,   170,   173,   174,
     176,   178,   182,   184,   188,   190,   192,   196,   198,   200,
     202,   204,   206,   208,   210,   215,   222,   229,   236,   244,
     249,   253,   262,   267,   274,   282,   292,   297,   302,   307,
     311,   313,   318,   322,   324,   330,   334,   338,   341,   344,
     348,   351,   354,   358,   362,   366,   370,   374,   378,   382,
     385,   389,   393,   396,   400,   404,   408,   412,   416,   420,
     426,   430,   435,   440,   446
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
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
      68,    -1,    67,    53,   102,    68,    67,    96,    72,    98,
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
static const yytype_uint16 yyrline[] =
{
       0,   130,   130,   139,   141,   141,   141,   143,   143,   143,
     145,   145,   145,   147,   149,   151,   153,   155,   157,   159,
     161,   163,   165,   167,   169,   171,   173,   175,   177,   177,
     177,   177,   183,   189,   191,   194,   196,   200,   201,   204,
     207,   209,   212,   214,   217,   219,   223,   224,   228,   229,
     232,   234,   237,   239,   242,   244,   247,   250,   252,   254,
     256,   258,   260,   262,   283,   290,   297,   303,   309,   314,
     318,   322,   326,   333,   340,   344,   349,   363,   365,   367,
     369,   382,   392,   395,   397,   399,   401,   403,   405,   407,
     409,   411,   413,   415,   417,   419,   421,   423,   425,   427,
     429,   431,   433,   435,   437,   439,   441,   443,   445,   447,
     449,   451,   453,   455,   457
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "STRUCT", "POINT", "NEWSTRUCT", "ANS",
  "FDEF", "PFDEF", "MODDEF", "MODEND", "GLOBAL", "MGLOBAL", "LOCAL",
  "LOCALF", "CMP", "OR", "AND", "CAR", "CDR", "QUOTED", "COLONCOLON", "DO",
  "WHILE", "FOR", "IF", "ELSE", "BREAK", "RETURN", "CONTINUE", "PARIF",
  "MAP", "RECMAP", "TIMER", "GF2NGEN", "GFPNGEN", "GFSNGEN", "GETOPT",
  "FOP_AND", "FOP_OR", "FOP_IMPL", "FOP_REPL", "FOP_EQUIV", "FOP_NOT",
  "LOP", "FORMULA", "UCASE", "LCASE", "STR", "SELF", "BOPASS", "'+'",
  "'-'", "'*'", "'/'", "'^'", "'%'", "'='", "'?'", "':'", "'`'", "'&'",
  "PLUS", "MINUS", "'!'", "'{'", "'}'", "'('", "')'", "';'", "'$'", "','",
  "'|'", "'['", "']'", "'<'", "'>'", "$accept", "start", "stat", "$@1",
  "$@2", "$@3", "$@4", "$@5", "$@6", "$@7", "$@8", "$@9", "tail", "desc",
  "complex", "members", "vars", "pvars", "stats", "node", "_node",
  "optlist", "rawstr", "opt", "pexpr", "expr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
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
static const yytype_uint8 yyr1[] =
{
       0,    77,    78,    79,    80,    81,    79,    82,    83,    79,
      84,    85,    79,    79,    79,    79,    79,    79,    79,    79,
      79,    79,    79,    79,    79,    79,    79,    79,    86,    87,
      88,    79,    79,    79,    79,    89,    89,    90,    90,    91,
      92,    92,    93,    93,    94,    94,    95,    95,    96,    96,
      97,    97,    98,    98,    99,    99,   100,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   102,   102
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     0,     0,     5,     0,     0,     5,
       0,     0,     5,     3,     6,     2,     1,     2,     2,     2,
       3,     5,     7,     9,     5,     7,     8,     6,     0,     0,
       0,    12,     3,     2,     2,     1,     1,     0,     1,     3,
       1,     3,     1,     3,     1,     3,     0,     2,     0,     1,
       1,     3,     1,     3,     1,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     4,     6,     6,     6,     7,     4,
       3,     8,     4,     6,     7,     9,     4,     4,     4,     3,
       1,     4,     3,     1,     5,     3,     3,     2,     2,     3,
       2,     2,     3,     3,     3,     3,     3,     3,     3,     2,
       3,     3,     2,     3,     3,     3,     3,     3,     3,     5,
       3,     4,     4,     5,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,    59,     0,     0,     0,     0,     4,
       7,    10,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    60,    61,    62,
       0,     0,    58,    80,    63,    57,     0,     0,     0,     0,
       0,    46,     0,    35,    36,    48,    48,     0,     2,     3,
      16,    83,     0,    34,    54,    55,     0,     0,    28,     0,
       0,    33,     0,     0,     0,    42,     0,     0,     0,     0,
       0,    48,    48,    48,    17,    63,    19,     0,    18,     0,
       0,     0,     0,     0,   102,    48,    48,    48,    88,    90,
      91,   114,    99,     0,     0,     0,     0,     0,    49,    50,
       0,     1,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    87,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    15,     0,     0,     0,    48,    32,    44,
       5,     8,    11,     0,    13,     0,     0,     0,     0,     0,
       0,     0,    48,    20,     0,     0,     0,     0,    70,     0,
       0,     0,     0,    39,    47,     0,     0,    79,     0,    89,
       0,   110,    82,     0,    98,   100,   101,   103,   104,   105,
     106,   107,   108,    86,    92,    93,    94,    95,    97,    96,
      85,     0,     0,    40,   111,    29,     0,     0,     0,     0,
       0,    43,    77,    78,   112,    48,     0,    48,     0,     0,
      72,    48,    48,    48,     0,    69,    76,     0,    64,     0,
       0,     0,     0,    51,    81,     0,     0,     0,    48,     0,
      45,     6,     9,    12,     0,    24,     0,    21,    64,     0,
       0,     0,     0,    48,     0,     0,     0,    52,     0,    84,
      48,   113,   109,    14,    41,    30,    27,     0,    48,     0,
      73,    66,    67,     0,     0,     0,    65,     0,     0,     0,
       0,    25,     0,    22,     0,    68,     0,    53,    56,    74,
       0,    37,     0,    71,    26,     0,    38,     0,    23,    75,
      46,     0,    31
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    47,   154,    62,   188,    63,   189,    64,   190,   126,
     218,   260,    49,   277,    50,   182,    66,   130,    93,    97,
      98,   235,   236,   237,    51,    99
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -65
static const yytype_int16 yypact[] =
{
     476,   -36,    83,   -64,   -65,   -27,     3,    18,   -36,   -65,
     -65,   -65,    26,    13,    21,    24,   476,    35,    60,    64,
     -36,   597,   -36,    68,    71,    72,    74,   -65,   -65,   -65,
      75,   707,   -65,    76,    25,   -65,   707,   707,   707,   707,
     707,   -65,   547,   -65,   -65,   707,   707,   137,   -65,   -65,
     -65,    -2,   864,   -65,   -65,   -65,    79,    83,   -65,    78,
     -36,   -65,   100,   100,   100,   -65,   -12,   707,   707,   707,
     124,   707,   707,   707,   -65,    58,   -65,   864,   -65,   101,
     102,   104,   707,   -33,  1184,   707,   707,   707,   106,   -26,
     -26,  1184,   106,   334,    83,   707,   908,    54,    85,  1124,
      81,   -65,    83,   707,   707,   707,   707,   707,   707,   707,
     707,   707,   707,   -65,   707,   707,   707,   707,   707,   707,
     707,   707,   707,   -65,    83,    91,   105,   707,   -65,   -65,
     103,   103,   103,   126,   -65,   939,   970,  1001,   111,   115,
     116,   127,   707,   -65,   -17,   129,   131,   776,   -65,   140,
     141,   144,   -53,   -65,   -65,   143,  1032,   -65,   707,   -65,
     707,   -65,   -65,   260,    23,   273,   135,  1228,   210,  1203,
     190,    45,    -8,  1124,   -26,   -26,   -45,   -45,   -45,   -45,
    1124,  1094,   -60,   -65,   -65,   -65,   145,   157,   -36,   -36,
     -36,   -65,   -65,   -65,   -65,   707,   476,   707,   476,   -50,
     -65,   707,   707,   707,   707,   -65,   -65,   147,   153,    83,
     134,   149,   745,  1124,   -65,   707,   -36,    83,   707,   -36,
     -65,   -65,   -65,   -65,   150,   -65,   152,   193,   -65,   154,
     156,   163,   820,   707,   176,   -16,   178,   -65,   657,    -2,
     707,   -65,  1154,   -65,   -65,   -65,   -65,   -36,   707,   476,
     -65,   -65,   -65,   707,   168,   707,   -65,    83,   707,   -19,
     169,   -65,   172,   -65,  1063,   -65,   864,   -65,  1124,   -65,
      83,   204,   476,   -65,   -65,    -5,   -65,   191,   -65,   -65,
     -65,   405,   -65
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -65,   -65,     1,   -65,   -65,   -65,   -65,   -65,   -65,   -65,
     -65,   -65,     4,   -65,   -65,   -65,   -65,    70,   -23,    90,
     -65,   -10,    30,    10,    59,     0
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
      52,    48,   102,    57,   113,    53,   216,   104,   105,   106,
     119,   217,    61,    54,    55,   208,    52,    70,   228,   209,
      58,    77,   209,   113,    74,    76,    78,   117,   118,   119,
     120,    84,    56,    43,    44,   148,    88,    89,    90,    91,
      92,   113,    96,   115,   116,   117,   118,   119,   120,   269,
      59,   200,   256,   270,   201,   257,   123,    43,    44,   133,
     104,   105,   106,   279,   128,    60,   257,   135,   136,   137,
     134,   103,   113,    65,   115,   116,   117,   118,   119,   120,
      67,   143,   147,   107,   108,   109,   110,   125,    68,   112,
      86,    69,    87,    52,   113,   156,   115,   116,   117,   118,
     119,   120,    71,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   149,   173,   174,   175,   176,   177,   178,
     179,   180,   181,    86,   155,   142,   158,    72,   159,    54,
      55,    73,   162,   131,   132,    79,   100,   101,    80,    81,
       4,    82,    83,    85,   124,   127,   129,   138,   144,   145,
     104,   146,    13,    14,   183,   113,   160,   161,   212,   184,
     213,   139,   140,   141,    23,    24,    25,    26,    27,    28,
      29,    30,   185,   191,   187,   150,   151,   152,   195,    32,
      33,    75,    35,   196,   113,   197,   115,   116,   117,   118,
     119,   120,   221,   222,   223,   198,    52,   225,    52,   227,
     202,   238,   203,   220,   232,   104,   105,   106,   205,   206,
     207,   210,   234,   219,   233,   242,   240,   186,   247,   249,
     243,   248,   250,   246,   251,   104,   105,   106,   107,   108,
     109,   252,   199,   255,   112,   258,   265,   271,    96,   113,
     272,   115,   116,   117,   118,   119,   120,   244,   107,    52,
     263,   261,   276,   264,   112,   266,   280,   281,   268,   113,
     275,   115,   116,   117,   118,   119,   120,   267,     0,   239,
     274,     0,    52,   278,     0,   104,   105,   106,     0,     0,
       0,    52,     0,     0,     0,   224,     0,   226,   104,     0,
     106,   229,   230,   231,     0,     0,     0,     0,   107,   108,
     109,   110,   111,     0,   112,     0,     0,     0,   245,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,     0,
       0,     0,   113,   254,   115,   116,   117,   118,   119,   120,
     259,     0,     0,     0,   214,     1,     0,     2,   262,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,     0,
       0,     0,    13,    14,    15,     0,    16,    17,    18,    19,
       0,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,     0,     0,     0,     0,     0,    31,     0,    32,
      33,    34,    35,    36,     0,    37,    38,     0,     0,     0,
       0,     0,     0,     0,    39,     0,     0,     0,    40,    41,
     153,    42,     0,    43,    44,     0,     1,    45,     2,    46,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
       0,     0,     0,    13,    14,    15,     0,    16,    17,    18,
      19,     0,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,     0,     0,     0,     0,     0,    31,     0,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,     0,     0,     0,    39,     0,     0,     0,    40,
      41,   282,    42,     0,    43,    44,     0,     1,    45,     2,
      46,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,     0,     0,     0,    13,    14,    15,     0,    16,    17,
      18,    19,     0,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,     0,     0,     0,     0,     0,    31,
       0,    32,    33,    34,    35,    36,     0,    37,    38,     0,
       0,     0,     0,     0,     0,     0,    39,     0,     0,     0,
      40,    41,     0,    42,     0,    43,    44,     0,     0,    45,
      94,    46,     3,     4,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    13,    14,    15,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    23,    24,    25,
      26,    27,    28,    29,    30,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    75,    35,    36,     0,    37,    38,
      95,     0,     3,     4,     0,     0,     0,    39,     0,     0,
       0,    40,     0,     0,    42,    13,    14,    15,     0,     0,
      45,     0,    46,     0,     0,     0,     0,    23,    24,    25,
      26,    27,    28,    29,    30,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    75,    35,    36,     0,    37,    38,
       0,     0,     0,     0,     0,     0,     0,    39,     0,     0,
       0,    40,     3,     4,    42,     0,    43,    44,     0,     0,
      45,     0,    46,     0,     0,    13,    14,    15,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    23,    24,    25,
      26,    27,    28,    29,    30,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    75,    35,    36,     0,    37,    38,
      95,     0,     3,     4,     0,     0,     0,    39,     0,     0,
       0,    40,     0,     0,    42,    13,    14,    15,     0,     0,
      45,     0,    46,     0,     0,     0,     0,    23,    24,    25,
      26,    27,    28,    29,    30,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    75,    35,    36,     0,    37,    38,
     104,   105,   106,     0,     0,     0,     0,    39,     0,     0,
       0,    40,     0,     0,    42,     0,     0,     0,     0,     0,
      45,     0,    46,   107,   108,   109,   110,   111,     0,   112,
       0,   104,   105,   106,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   107,   108,   109,   110,   111,   241,
     112,     0,     0,     0,     0,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   104,   105,   106,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   204,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   107,   108,
     109,   110,   111,     0,   112,     0,     0,     0,     0,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   104,
     105,   106,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   253,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   107,   108,   109,   110,   111,     0,   112,     0,
       0,     0,     0,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   104,   105,   106,     0,     0,     0,     0,
       0,     0,     0,    43,    44,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   107,   108,   109,   110,
     111,     0,   112,     0,   104,   105,   106,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   157,   107,   108,   109,
     110,   111,     0,   112,     0,   104,   105,   106,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   192,   107,   108,
     109,   110,   111,     0,   112,     0,   104,   105,   106,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   193,   107,
     108,   109,   110,   111,     0,   112,     0,   104,   105,   106,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   194,
     107,   108,   109,   110,   111,     0,   112,     0,   104,   105,
     106,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     211,   107,   108,   109,   110,   111,     0,   112,     0,   104,
     105,   106,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   273,   107,   108,   109,   110,   111,     0,   112,   104,
     105,   106,     0,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   215,     0,     0,     0,     0,     0,     0,
       0,     0,   107,   108,   109,   110,   111,     0,   112,   104,
     105,   106,     0,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   107,   108,   109,   110,   111,     0,   112,   104,
     105,   106,     0,   113,     0,   115,   116,   117,   118,   119,
     120,     0,   122,     0,     0,     0,     0,     0,   104,   105,
     106,     0,   107,   108,   109,   110,   111,     0,   112,     0,
       0,     0,     0,   113,     0,   115,   116,   117,   118,   119,
     120,   107,   108,   104,   105,   106,     0,   112,     0,     0,
       0,     0,   113,     0,   115,   116,   117,   118,   119,   120,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   112,     0,     0,     0,     0,   113,     0,   115,
     116,   117,   118,   119,   120
};

static const yytype_int16 yycheck[] =
{
       0,     0,     4,    67,    49,     1,    66,    15,    16,    17,
      55,    71,     8,    46,    47,    68,    16,    16,    68,    72,
      47,    21,    72,    49,    20,    21,    22,    53,    54,    55,
      56,    31,     2,    69,    70,    68,    36,    37,    38,    39,
      40,    49,    42,    51,    52,    53,    54,    55,    56,    68,
      47,    68,    68,    72,    71,    71,    52,    69,    70,    71,
      15,    16,    17,    68,    60,    47,    71,    67,    68,    69,
      66,    73,    49,    47,    51,    52,    53,    54,    55,    56,
      67,    77,    82,    38,    39,    40,    41,    57,    67,    44,
      65,    67,    67,    93,    49,    95,    51,    52,    53,    54,
      55,    56,    67,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,    83,   114,   115,   116,   117,   118,   119,
     120,   121,   122,    65,    94,    67,    72,    67,    74,    46,
      47,    67,   102,    63,    64,    67,    46,     0,    67,    67,
       6,    67,    67,    67,    65,    67,    46,    23,    47,    47,
      15,    47,    18,    19,   124,    49,    71,    76,   158,    68,
     160,    71,    72,    73,    30,    31,    32,    33,    34,    35,
      36,    37,    67,    47,    71,    85,    86,    87,    67,    45,
      46,    47,    48,    68,    49,    69,    51,    52,    53,    54,
      55,    56,   188,   189,   190,    68,   196,   196,   198,   198,
      71,    67,    71,    46,   204,    15,    16,    17,    68,    68,
      66,    68,    59,    68,    67,   215,    67,   127,    68,    26,
     216,    69,    68,   219,    68,    15,    16,    17,    38,    39,
      40,    68,   142,    57,    44,    57,    68,    68,   238,    49,
      68,    51,    52,    53,    54,    55,    56,   217,    38,   249,
     249,   247,    48,   253,    44,   255,    65,   280,   258,    49,
     270,    51,    52,    53,    54,    55,    56,   257,    -1,   210,
     266,    -1,   272,   272,    -1,    15,    16,    17,    -1,    -1,
      -1,   281,    -1,    -1,    -1,   195,    -1,   197,    15,    -1,
      17,   201,   202,   203,    -1,    -1,    -1,    -1,    38,    39,
      40,    41,    42,    -1,    44,    -1,    -1,    -1,   218,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      -1,    -1,    49,   233,    51,    52,    53,    54,    55,    56,
     240,    -1,    -1,    -1,    74,     1,    -1,     3,   248,     5,
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
      65,    66,    67,    -1,    69,    70,    -1,     1,    73,     3,
      75,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    18,    19,    20,    -1,    22,    23,
      24,    25,    -1,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,    43,
      -1,    45,    46,    47,    48,    49,    -1,    51,    52,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    60,    -1,    -1,    -1,
      64,    65,    -1,    67,    -1,    69,    70,    -1,    -1,    73,
       3,    75,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    18,    19,    20,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,
      43,    -1,    45,    46,    47,    48,    49,    -1,    51,    52,
      53,    -1,     5,     6,    -1,    -1,    -1,    60,    -1,    -1,
      -1,    64,    -1,    -1,    67,    18,    19,    20,    -1,    -1,
      73,    -1,    75,    -1,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,
      43,    -1,    45,    46,    47,    48,    49,    -1,    51,    52,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    -1,    -1,
      -1,    64,     5,     6,    67,    -1,    69,    70,    -1,    -1,
      73,    -1,    75,    -1,    -1,    18,    19,    20,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,
      43,    -1,    45,    46,    47,    48,    49,    -1,    51,    52,
      53,    -1,     5,     6,    -1,    -1,    -1,    60,    -1,    -1,
      -1,    64,    -1,    -1,    67,    18,    19,    20,    -1,    -1,
      73,    -1,    75,    -1,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,
      43,    -1,    45,    46,    47,    48,    49,    -1,    51,    52,
      15,    16,    17,    -1,    -1,    -1,    -1,    60,    -1,    -1,
      -1,    64,    -1,    -1,    67,    -1,    -1,    -1,    -1,    -1,
      73,    -1,    75,    38,    39,    40,    41,    42,    -1,    44,
      -1,    15,    16,    17,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    38,    39,    40,    41,    42,    74,
      44,    -1,    -1,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    15,    16,    17,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    71,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,    39,
      40,    41,    42,    -1,    44,    -1,    -1,    -1,    -1,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    15,
      16,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    38,    39,    40,    41,    42,    -1,    44,    -1,
      -1,    -1,    -1,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    15,    16,    17,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    69,    70,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    38,    39,    40,    41,
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
      68,    38,    39,    40,    41,    42,    -1,    44,    -1,    15,
      16,    17,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    68,    38,    39,    40,    41,    42,    -1,    44,    15,
      16,    17,    -1,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    38,    39,    40,    41,    42,    -1,    44,    15,
      16,    17,    -1,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    38,    39,    40,    41,    42,    -1,    44,    15,
      16,    17,    -1,    49,    -1,    51,    52,    53,    54,    55,
      56,    -1,    58,    -1,    -1,    -1,    -1,    -1,    15,    16,
      17,    -1,    38,    39,    40,    41,    42,    -1,    44,    -1,
      -1,    -1,    -1,    49,    -1,    51,    52,    53,    54,    55,
      56,    38,    39,    15,    16,    17,    -1,    44,    -1,    -1,
      -1,    -1,    49,    -1,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    44,    -1,    -1,    -1,    -1,    49,    -1,    51,
      52,    53,    54,    55,    56
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
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
      72,    68,    68,    68,    89,    98,    48,    90,    79,    68,
      65,    95,    66
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


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
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
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
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
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



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

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
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

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
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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

/* Line 1455 of yacc.c  */
#line 131 "parse.y"
    { 
				parse_snode = (yyvsp[(1) - (1)].s); 
				if ( yychar >= 0 ) 
					fprintf(stderr,
						"Warning: a token was wasted after an 'if' statement without 'else'.\n");
				YYACCEPT; 
			}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 140 "parse.y"
    { (yyval.s) = 0; }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 141 "parse.y"
    { gdef=1; }
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 141 "parse.y"
    { gdef=0; }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 142 "parse.y"
    { (yyval.s) = 0; NOPR; }
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 143 "parse.y"
    { mgdef=1; }
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 143 "parse.y"
    { mgdef=0; }
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 144 "parse.y"
    { (yyval.s) = 0; NOPR; }
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 145 "parse.y"
    { ldef=1; }
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 145 "parse.y"
    { ldef=0; }
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 146 "parse.y"
    { (yyval.s) = 0; NOPR; }
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 148 "parse.y"
    { appenduflist((yyvsp[(2) - (3)].n)); (yyval.s) = 0; NOPR; }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 150 "parse.y"
    { structdef((yyvsp[(2) - (6)].p),(yyvsp[(4) - (6)].n)); (yyval.s) = 0; NOPR; }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 152 "parse.y"
    { (yyval.s) = mksnode(1,S_SINGLE,(yyvsp[(1) - (2)].f)); }
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 154 "parse.y"
    { (yyval.s) = (yyvsp[(1) - (1)].s); }
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 156 "parse.y"
    { (yyval.s) = mksnode(0,S_BREAK); }
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 158 "parse.y"
    { (yyval.s) = mksnode(0,S_CONTINUE); }
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 160 "parse.y"
    { (yyval.s) = mksnode(1,S_RETURN,NULLP); }
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 162 "parse.y"
    { (yyval.s) = mksnode(1,S_RETURN,(yyvsp[(2) - (3)].f)); }
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 164 "parse.y"
    { (yyval.s) = mksnode(4,S_IFELSE,(yyvsp[(1) - (5)].i),(yyvsp[(3) - (5)].n),(yyvsp[(5) - (5)].s),NULLP); (yyvsp[(5) - (5)].s)?(yyval.s)->ln=(yyvsp[(5) - (5)].s)->ln:0; NOPR; }
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 166 "parse.y"
    { (yyval.s) = mksnode(4,S_IFELSE,(yyvsp[(1) - (7)].i),(yyvsp[(3) - (7)].n),(yyvsp[(5) - (7)].s),(yyvsp[(7) - (7)].s)); (yyvsp[(7) - (7)].s)?(yyval.s)->ln=(yyvsp[(7) - (7)].s)->ln:0; NOPR; }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 168 "parse.y"
    { (yyval.s) = mksnode(5,S_FOR,(yyvsp[(1) - (9)].i),(yyvsp[(3) - (9)].n),(yyvsp[(5) - (9)].n)?(yyvsp[(5) - (9)].n):ONENODE,(yyvsp[(7) - (9)].n),(yyvsp[(9) - (9)].s)); (yyvsp[(9) - (9)].s)?(yyval.s)->ln=(yyvsp[(9) - (9)].s)->ln:0; NOPR; }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 170 "parse.y"
    { (yyval.s) = mksnode(5,S_FOR,(yyvsp[(1) - (5)].i),NULLP,(yyvsp[(3) - (5)].n),NULLP,(yyvsp[(5) - (5)].s)); (yyvsp[(5) - (5)].s)?(yyval.s)->ln=(yyvsp[(5) - (5)].s)->ln:0; NOPR; }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 172 "parse.y"
    { (yyval.s) = mksnode(3,S_DO,(yyvsp[(1) - (7)].i),(yyvsp[(2) - (7)].s),(yyvsp[(5) - (7)].n)); NOPR; }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 174 "parse.y"
    { (yyval.s) = mksnode(3,S_PFDEF,(yyvsp[(1) - (8)].p),(yyvsp[(3) - (8)].n),(yyvsp[(7) - (8)].f)); NOPR; }
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 176 "parse.y"
    { (yyval.s) = mksnode(3,S_PFDEF,(yyvsp[(2) - (6)].p),(yyvsp[(4) - (6)].n),NULLP); NOPR; }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 177 "parse.y"
    { mkpvs((yyvsp[(2) - (2)].p)); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 177 "parse.y"
    { ldef = 1; }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 177 "parse.y"
    { ldef = -1; }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 178 "parse.y"
    {
				mkuf((yyvsp[(2) - (12)].p),asir_infile->name,(yyvsp[(6) - (12)].n),
					mksnode(1,S_CPLX,(yyvsp[(11) - (12)].n)),(yyvsp[(1) - (12)].i),asir_infile->ln,(yyvsp[(9) - (12)].p),CUR_MODULE); 
				(yyval.s) = 0; NOPR;
			}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 184 "parse.y"
    { 
				CUR_MODULE = mkmodule((yyvsp[(2) - (3)].p));
				MPVS = CUR_MODULE->pvs; 
				(yyval.s) = mksnode(1,S_MODULE,CUR_MODULE); NOPR;
			}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 190 "parse.y"
    { CUR_MODULE = 0; MPVS = 0; (yyval.s) = mksnode(1,S_MODULE,NULLP); NOPR; }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 192 "parse.y"
    { yyerrok; (yyval.s) = 0; }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 195 "parse.y"
    { if ( main_parser ) prresult = 1; }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 197 "parse.y"
    { if ( main_parser ) prresult = 0; }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 200 "parse.y"
    { (yyval.p) = 0; }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 202 "parse.y"
    { (yyval.p) = (yyvsp[(1) - (1)].p); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 205 "parse.y"
    { (yyval.s) = mksnode(1,S_CPLX,(yyvsp[(2) - (3)].n)); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 208 "parse.y"
    { MKNODE((yyval.n),(yyvsp[(1) - (1)].p),0); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 210 "parse.y"
    { appendtonode((yyvsp[(1) - (3)].n),(yyvsp[(3) - (3)].p),&(yyval.n)); }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 213 "parse.y"
    { MKNODE((yyval.n),(yyvsp[(1) - (1)].p),0); }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 215 "parse.y"
    { appendtonode((yyvsp[(1) - (3)].n),(yyvsp[(3) - (3)].p),&(yyval.n)); }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 218 "parse.y"
    { val = (pointer)makepvar((yyvsp[(1) - (1)].p)); MKNODE((yyval.n),val,0); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 220 "parse.y"
    { appendtonode((yyvsp[(1) - (3)].n),(pointer)makepvar((yyvsp[(3) - (3)].p)),&(yyval.n)); }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 223 "parse.y"
    { (yyval.n) = 0; }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 225 "parse.y"
    { appendtonode((yyvsp[(1) - (2)].n),(pointer)(yyvsp[(2) - (2)].s),&(yyval.n)); }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 228 "parse.y"
    { (yyval.n) = 0; }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 230 "parse.y"
    { (yyval.n) = (yyvsp[(1) - (1)].n); }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 233 "parse.y"
    { MKNODE((yyval.n),(yyvsp[(1) - (1)].f),0); }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 235 "parse.y"
    { appendtonode((yyvsp[(1) - (3)].n),(pointer)(yyvsp[(3) - (3)].f),&(yyval.n)); }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 238 "parse.y"
    { MKNODE((yyval.n),(yyvsp[(1) - (1)].f),0); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 240 "parse.y"
    { appendtonode((yyvsp[(1) - (3)].n),(pointer)(yyvsp[(3) - (3)].f),&(yyval.n)); }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 243 "parse.y"
    { (yyval.p) = (yyvsp[(1) - (1)].p); }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 245 "parse.y"
    { (yyval.p) = (yyvsp[(1) - (1)].p); }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 248 "parse.y"
    { (yyval.f) = mkfnode(2,I_OPT,(yyvsp[(1) - (3)].p),(yyvsp[(3) - (3)].f)); }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 251 "parse.y"
    { (yyval.f) = mkfnode(1,I_STR,(yyvsp[(1) - (1)].p)); }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 253 "parse.y"
    { (yyval.f) = mkfnode(1,I_FORMULA,(yyvsp[(1) - (1)].p)); }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 255 "parse.y"
    { (yyval.f) = mkfnode(1,I_ANS,(yyvsp[(1) - (1)].i)); }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 257 "parse.y"
    { (yyval.f) = mkfnode(0,I_GF2NGEN); }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 259 "parse.y"
    { (yyval.f) = mkfnode(0,I_GFPNGEN); }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 261 "parse.y"
    { (yyval.f) = mkfnode(0,I_GFSNGEN); }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 263 "parse.y"
    {
				FUNC f;

				searchf(noargsysf,(yyvsp[(1) - (1)].p),&f);
				if ( f )
					 (yyval.f) = mkfnode(2,I_FUNC,f,NULLP);
				else {
					searchc((yyvsp[(1) - (1)].p),&f);
					if ( f )
						(yyval.f) = mkfnode(2,I_FUNC,f,mkfnode(1,I_LIST,NULLP));
					else {
						gen_searchf_searchonly((yyvsp[(1) - (1)].p),(FUNC *)&f);
						if ( f )
							makesrvar(f,(P *)&val);
						else
							makevar((yyvsp[(1) - (1)].p),(P *)&val);
						(yyval.f) = mkfnode(1,I_FORMULA,val);
					}
				}
			}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 284 "parse.y"
    {
				gen_searchf((yyvsp[(1) - (4)].p),(FUNC *)&val);
				print_crossref(val);
				(yyval.f) = mkfnode(2,I_FUNC,val,mkfnode(1,I_LIST,(yyvsp[(3) - (4)].n)));
			}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 291 "parse.y"
    {
				gen_searchf((yyvsp[(1) - (6)].p),(FUNC *)&val);
				print_crossref(val);
				(yyval.f) = mkfnode(3,I_FUNC_OPT,val,
					mkfnode(1,I_LIST,(yyvsp[(3) - (6)].n)),mkfnode(1,I_LIST,(yyvsp[(5) - (6)].n)));
			}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 298 "parse.y"
    {
				gen_searchf((yyvsp[(3) - (6)].p),(FUNC *)&val);
				print_crossref(val);
				(yyval.f) = mkfnode(2,I_MAP,val,mkfnode(1,I_LIST,(yyvsp[(5) - (6)].n)));
			}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 304 "parse.y"
    {
				gen_searchf((yyvsp[(3) - (6)].p),(FUNC *)&val);
				print_crossref(val);
				(yyval.f) = mkfnode(2,I_RECMAP,val,mkfnode(1,I_LIST,(yyvsp[(5) - (6)].n)));
			}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 310 "parse.y"
    {
				searchpf((yyvsp[(1) - (7)].p),(FUNC *)&val);
				(yyval.f) = mkfnode(3,I_PFDERIV,val,mkfnode(1,I_LIST,(yyvsp[(6) - (7)].n)),mkfnode(1,I_LIST,(yyvsp[(3) - (7)].n)));
			}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 315 "parse.y"
    {
				(yyval.f) = mkfnode(2,I_GETOPT,(yyvsp[(3) - (4)].p));
			}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 319 "parse.y"
    {
				(yyval.f) = mkfnode(2,I_GETOPT,NULLP);
			}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 323 "parse.y"
    {
				(yyval.f) = mkfnode(3,I_TIMER,(yyvsp[(3) - (8)].f),(yyvsp[(5) - (8)].f),(yyvsp[(7) - (8)].f));
			}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 327 "parse.y"
    {
				searchf(parif,(yyvsp[(3) - (4)].p),(FUNC *)&val);
				if ( !val )
					mkparif((yyvsp[(3) - (4)].p),(FUNC *)&val);
				(yyval.f) = mkfnode(2,I_FUNC,val,NULLP);
			}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 334 "parse.y"
    {
				searchf(parif,(yyvsp[(3) - (6)].p),(FUNC *)&val);
				if ( !val )
					mkparif((yyvsp[(3) - (6)].p),(FUNC *)&val);
				(yyval.f) = mkfnode(2,I_FUNC,val,mkfnode(1,I_LIST,(yyvsp[(5) - (6)].n)));
			}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 341 "parse.y"
    {
				(yyval.f) = mkfnode(2,I_IFUNC,(yyvsp[(3) - (7)].f),mkfnode(1,I_LIST,(yyvsp[(6) - (7)].n)),NULLP);
			}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 345 "parse.y"
    {
				(yyval.f) = mkfnode(3,I_IFUNC,(yyvsp[(3) - (9)].f),mkfnode(1,I_LIST,(yyvsp[(6) - (9)].n)),
					mkfnode(1,I_LIST,(yyvsp[(8) - (9)].n)));
			}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 350 "parse.y"
    {
				if ( main_parser || allow_create_var )
					t = mkfnode(2,I_PVAR,makepvar((yyvsp[(1) - (4)].p)),NULLP);
				else {
					ind = searchpvar((yyvsp[(1) - (4)].p));
					if ( ind == -1 ) {
						fprintf(stderr,"%s : no such variable.\n",(yyvsp[(1) - (4)].p));
						YYABORT;
					} else
						t = mkfnode(2,I_PVAR,ind,NULLP);
				}
				(yyval.f) = mkfnode(2,I_IFUNC,t,mkfnode(1,I_LIST,(yyvsp[(3) - (4)].n)));
			}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 364 "parse.y"
    { (yyval.f) = mkfnode(1,I_CAR,(yyvsp[(3) - (4)].f)); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 366 "parse.y"
    { (yyval.f) = mkfnode(1,I_CDR,(yyvsp[(3) - (4)].f)); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 368 "parse.y"
    { (yyval.f) = mkfnode(1,I_PAREN,(yyvsp[(2) - (3)].f)); }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 370 "parse.y"
    { 
				if ( main_parser || allow_create_var )
					(yyval.f) = mkfnode(2,I_PVAR,makepvar((yyvsp[(1) - (1)].p)),NULLP);
				else {
					ind = searchpvar((yyvsp[(1) - (1)].p));
					if ( ind == -1 ) {
						fprintf(stderr,"%s : no such variable.\n",(yyvsp[(1) - (1)].p));
						YYABORT;
					} else
						(yyval.f) = mkfnode(2,I_PVAR,ind,NULLP);
				}
			}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 383 "parse.y"
    {
				if ( (yyvsp[(1) - (4)].f)->id == I_PVAR || (yyvsp[(1) - (4)].f)->id == I_INDEX ) {
					appendtonode((NODE)(yyvsp[(1) - (4)].f)->arg[1],(pointer)(yyvsp[(3) - (4)].f),&a);
					(yyvsp[(1) - (4)].f)->arg[1] = (pointer)a; (yyval.f) = (yyvsp[(1) - (4)].f);
				} else {
					MKNODE(a,(yyvsp[(3) - (4)].f),0);
					(yyval.f) = mkfnode(2,I_INDEX,(pointer)(yyvsp[(1) - (4)].f),a);
				}
			}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 393 "parse.y"
    { (yyval.f) = mkfnode(2,I_POINT,(yyvsp[(1) - (3)].f),(yyvsp[(3) - (3)].p)); }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 396 "parse.y"
    { (yyval.f) = (yyvsp[(1) - (1)].f); }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 398 "parse.y"
    { (yyval.f) = mkfnode(3,I_CAST,structtoindex((yyvsp[(3) - (5)].p)),(yyvsp[(5) - (5)].f),NULLP); }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 400 "parse.y"
    { (yyval.f) = mkfnode(2,I_ASSPVAR,(yyvsp[(1) - (3)].f),(yyvsp[(3) - (3)].f)); }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 402 "parse.y"
    { (yyval.f) = mkfnode(2,I_ASSPVAR,(yyvsp[(1) - (3)].f),mkfnode(3,I_BOP,(yyvsp[(2) - (3)].p),(yyvsp[(1) - (3)].f),(yyvsp[(3) - (3)].f))); }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 404 "parse.y"
    { (yyval.f) = mkfnode(2,I_POSTSELF,(yyvsp[(2) - (2)].p),(yyvsp[(1) - (2)].f)); }
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 406 "parse.y"
    { (yyval.f) = mkfnode(2,I_PRESELF,(yyvsp[(1) - (2)].p),(yyvsp[(2) - (2)].f)); }
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 408 "parse.y"
    { (yyval.f) = mkfnode(1,I_LIST,(yyvsp[(2) - (3)].n)); }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 410 "parse.y"
    { (yyval.f) = (yyvsp[(2) - (2)].f); }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 412 "parse.y"
    { (yyval.f) = mkfnode(1,I_MINUS,(yyvsp[(2) - (2)].f)); }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 414 "parse.y"
    { (yyval.f) = mkfnode(3,I_BOP,(yyvsp[(2) - (3)].p),(yyvsp[(1) - (3)].f),(yyvsp[(3) - (3)].f)); }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 416 "parse.y"
    { (yyval.f) = mkfnode(3,I_BOP,(yyvsp[(2) - (3)].p),(yyvsp[(1) - (3)].f),(yyvsp[(3) - (3)].f)); }
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 418 "parse.y"
    { (yyval.f) = mkfnode(3,I_BOP,(yyvsp[(2) - (3)].p),(yyvsp[(1) - (3)].f),(yyvsp[(3) - (3)].f)); }
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 420 "parse.y"
    { (yyval.f) = mkfnode(3,I_BOP,(yyvsp[(2) - (3)].p),(yyvsp[(1) - (3)].f),(yyvsp[(3) - (3)].f)); }
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 422 "parse.y"
    { (yyval.f) = mkfnode(3,I_BOP,(yyvsp[(2) - (3)].p),(yyvsp[(1) - (3)].f),(yyvsp[(3) - (3)].f)); }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 424 "parse.y"
    { (yyval.f) = mkfnode(3,I_BOP,(yyvsp[(2) - (3)].p),(yyvsp[(1) - (3)].f),(yyvsp[(3) - (3)].f)); }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 426 "parse.y"
    { (yyval.f) = mkfnode(3,I_COP,(yyvsp[(2) - (3)].i),(yyvsp[(1) - (3)].f),(yyvsp[(3) - (3)].f)); }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 428 "parse.y"
    { (yyval.f) = mkfnode(1,I_NOT,(yyvsp[(2) - (2)].f)); }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 430 "parse.y"
    { (yyval.f) = mkfnode(2,I_OR,(yyvsp[(1) - (3)].f),(yyvsp[(3) - (3)].f)); }
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 432 "parse.y"
    { (yyval.f) = mkfnode(2,I_AND,(yyvsp[(1) - (3)].f),(yyvsp[(3) - (3)].f)); }
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 434 "parse.y"
    { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[(1) - (2)].i),(yyvsp[(2) - (2)].f),NULLP); }
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 436 "parse.y"
    { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[(2) - (3)].i),(yyvsp[(1) - (3)].f),(yyvsp[(3) - (3)].f)); }
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 438 "parse.y"
    { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[(2) - (3)].i),(yyvsp[(1) - (3)].f),(yyvsp[(3) - (3)].f)); }
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 440 "parse.y"
    { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[(2) - (3)].i),(yyvsp[(1) - (3)].f),(yyvsp[(3) - (3)].f)); }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 442 "parse.y"
    { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[(2) - (3)].i),(yyvsp[(1) - (3)].f),(yyvsp[(3) - (3)].f)); }
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 444 "parse.y"
    { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[(2) - (3)].i),(yyvsp[(1) - (3)].f),(yyvsp[(3) - (3)].f)); }
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 446 "parse.y"
    { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[(2) - (3)].i),(yyvsp[(1) - (3)].f),(yyvsp[(3) - (3)].f)); }
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 448 "parse.y"
    { (yyval.f) = mkfnode(3,I_CE,(yyvsp[(1) - (5)].f),(yyvsp[(3) - (5)].f),(yyvsp[(5) - (5)].f)); }
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 450 "parse.y"
    { (yyval.f) = mkfnode(1,I_EV,(yyvsp[(2) - (3)].n)); }
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 452 "parse.y"
    { (yyval.f) = mkfnode(1,I_NEWCOMP,(int)structtoindex((yyvsp[(3) - (4)].p))); }
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 454 "parse.y"
    { MKQUOTE(quote,(yyvsp[(3) - (4)].f)); (yyval.f) = mkfnode(1,I_FORMULA,(pointer)quote); }
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 456 "parse.y"
    { (yyval.f) = mkfnode(2,I_CONS,(yyvsp[(2) - (5)].n),(yyvsp[(4) - (5)].f)); }
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 458 "parse.y"
    { MKQUOTE(quote,(yyvsp[(2) - (2)].f)); (yyval.f) = mkfnode(1,I_FORMULA,(pointer)quote); }
    break;



/* Line 1455 of yacc.c  */
#line 2796 "y.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
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
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
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


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 460 "parse.y"


