/* A Bison parser, made by GNU Bison 3.3.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.3.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 50 "parse.y" /* yacc.c:337  */

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

#line 101 "y.tab.c" /* yacc.c:337  */
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    STRUCT = 258,
    PNT = 259,
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

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 83 "parse.y" /* yacc.c:352  */

	FNODE f;
	SNODE s;
	NODE n;
	NODE2 n2;
	int i;
	pointer p;

#line 257 "y.tab.c" /* yacc.c:352  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */



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
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  101
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1381

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  77
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  26
/* YYNRULES -- Number of rules.  */
#define YYNRULES  116
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  287

#define YYUNDEFTOK  2
#define YYMAXUTOK   307

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
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
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   130,   130,   139,   141,   141,   141,   143,   143,   143,
     145,   145,   145,   147,   149,   151,   153,   155,   157,   159,
     161,   163,   165,   167,   169,   171,   173,   175,   177,   177,
     177,   177,   183,   189,   191,   194,   196,   200,   201,   204,
     207,   209,   212,   214,   217,   219,   223,   224,   228,   229,
     232,   234,   237,   239,   242,   244,   247,   250,   252,   254,
     256,   258,   260,   262,   283,   289,   296,   302,   308,   313,
     317,   321,   325,   332,   339,   343,   348,   362,   364,   366,
     368,   381,   391,   394,   396,   398,   400,   402,   404,   406,
     408,   410,   412,   414,   416,   418,   420,   422,   424,   426,
     428,   435,   437,   439,   441,   443,   445,   447,   449,   451,
     453,   455,   457,   461,   463,   465,   467
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "STRUCT", "PNT", "NEWSTRUCT", "ANS",
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
  "optlist", "rawstr", "opt", "pexpr", "expr", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
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

#define YYPACT_NINF -60

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-60)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     475,    27,     6,   -52,   -60,   -25,   -20,   -12,    27,   -60,
     -60,   -60,    13,    19,    23,    25,   475,    31,    35,    56,
      27,   596,    27,    58,    59,    60,    62,   -60,   -60,   -60,
      63,   706,   -60,    67,    -4,   -60,   706,   706,   706,   706,
     706,   -60,   546,   -60,   -60,   706,   706,    94,   -60,   -60,
     -60,    -2,   937,   -60,   -60,   -60,    76,     6,   -60,    75,
      27,   -60,    97,    97,    97,   -60,    14,   706,   706,   706,
     121,   706,   706,   706,   -60,     7,   -60,   937,   -60,    98,
      99,   100,   706,   -40,  1227,   706,   706,   706,   101,   155,
     155,  1227,   -45,   333,     6,   706,   981,    17,    78,  1197,
     -56,   -60,     6,   706,   706,   706,   706,   706,   706,   706,
     706,   706,   706,   -60,   706,   706,   706,   706,   706,   706,
     706,   706,   706,   -60,   -60,     6,    86,    93,   706,   -60,
     -60,    81,    81,    81,   106,   -60,  1012,  1043,  1074,   102,
     105,    96,   115,   706,   -60,   -27,    95,   103,   849,   -60,
     116,   117,   120,   -39,   -60,   -60,   119,  1105,   -60,   706,
     -60,   706,   706,   -60,   -60,   774,    24,   788,   504,    84,
     214,  1317,  1287,  1257,    -6,  1197,   155,   155,   -41,   -41,
     -41,   -41,  1197,  1167,   -53,   -60,   -60,   -60,   129,   122,
      27,    27,    27,   -60,   -60,   -60,   -60,   706,   475,   706,
     475,   -38,   -60,   706,   706,   706,   706,   -60,   -60,   135,
     108,     6,   145,   136,   818,  1197,   744,   -60,   706,    27,
       6,   706,    27,   -60,   -60,   -60,   -60,   137,   -60,   138,
     162,   -60,   146,   147,   148,   893,   706,   132,   -14,   160,
     -60,   656,    -2,   706,   -60,   -60,   256,   -60,   -60,   -60,
     -60,    27,   706,   475,   -60,   -60,   -60,   706,   152,   706,
     -60,     6,   706,   -17,   153,   -60,   154,   -60,  1136,   -60,
     937,   -60,  1197,   -60,     6,   176,   475,   -60,   -60,    -9,
     -60,   167,   -60,   -60,   -60,   404,   -60
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
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
       0,     0,     0,     0,   103,    48,    48,    48,    88,    90,
      91,   116,    99,     0,     0,     0,     0,     0,    49,    50,
       0,     1,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    87,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   100,    15,     0,     0,     0,    48,    32,
      44,     5,     8,    11,     0,    13,     0,     0,     0,     0,
       0,     0,     0,    48,    20,     0,     0,     0,     0,    70,
       0,     0,     0,     0,    39,    47,     0,     0,    79,     0,
      89,     0,     0,   111,    82,     0,    98,   101,   102,   104,
     105,   106,   107,   108,   109,    86,    92,    93,    94,    95,
      97,    96,    85,     0,     0,    40,   113,    29,     0,     0,
       0,     0,     0,    43,    77,    78,   114,    48,     0,    48,
       0,     0,    72,    48,    48,    48,     0,    69,    76,     0,
      64,     0,     0,     0,     0,    51,     0,    81,     0,     0,
       0,    48,     0,    45,     6,     9,    12,     0,    24,     0,
      21,    64,     0,     0,     0,     0,    48,     0,     0,     0,
      52,     0,    84,    48,   115,   112,   110,    14,    41,    30,
      27,     0,    48,     0,    73,    66,    67,     0,     0,     0,
      65,     0,     0,     0,     0,    25,     0,    22,     0,    68,
       0,    53,    56,    74,     0,    37,     0,    71,    26,     0,
      38,     0,    23,    75,    46,     0,    31
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -60,   -60,     1,   -60,   -60,   -60,   -60,   -60,   -60,   -60,
     -60,   -60,     4,   -60,   -60,   -60,   -60,     2,   -59,    85,
     -60,   -47,    30,   -28,    22,     0
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    47,   155,    62,   190,    63,   191,    64,   192,   127,
     221,   264,    49,   281,    50,   184,    66,   131,    93,    97,
      98,   238,   239,   240,    51,    99
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint16 yytable[] =
{
      52,    48,   102,   162,   113,    53,    54,    55,   113,   104,
     105,   106,    61,   219,   119,    57,    52,    70,   220,   123,
     163,    77,    58,   123,    74,    76,    78,    59,   149,   210,
     231,    84,    56,   211,   211,    60,    88,    89,    90,    91,
      92,   202,    96,   113,   203,   115,   116,   117,   118,   119,
     120,   273,    54,    55,   260,   274,   124,   261,   123,   283,
      65,    86,   261,    87,   129,   132,   133,   136,   137,   138,
     135,   103,    86,   113,   143,   115,   116,   117,   118,   119,
     120,   144,   148,    43,    44,   134,    67,   126,   123,   159,
      68,   160,    69,    52,   101,   157,    43,    44,    71,   104,
     105,   106,    72,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   150,   175,   176,   177,   178,   179,   180,
     181,   182,   183,    73,   156,    79,    80,    81,   112,    82,
      83,   100,   164,   113,    85,   115,   116,   117,   118,   119,
     120,   125,   128,   130,   139,   145,   146,   147,   123,   161,
     113,     4,   189,   193,   186,   185,   140,   141,   142,   214,
     187,   215,   216,    13,    14,   199,   204,   237,   223,   197,
     151,   152,   153,   198,   205,    23,    24,    25,    26,    27,
      28,    29,    30,   200,   207,   208,   209,   212,   253,   259,
      32,    33,    75,    35,   224,   225,   226,   222,    52,   228,
      52,   230,   236,   243,   113,   251,   235,   252,   117,   118,
     119,   120,   241,   188,   254,   255,   256,   262,   246,   123,
     269,   275,   276,   247,   280,   285,   250,   279,   201,   104,
     105,   106,   284,   271,   242,     0,     0,     0,     0,     0,
       0,    96,     0,     0,     0,     0,     0,     0,     0,     0,
     248,     0,   107,    52,   267,   265,     0,   268,   112,   270,
       0,     0,   272,   113,     0,   115,   116,   117,   118,   119,
     120,   104,   105,   106,   278,     0,    52,   282,   123,     0,
       0,     0,   227,     0,   229,    52,     0,     0,   232,   233,
     234,     0,     0,     0,   107,   108,   109,   110,   111,     0,
     112,     0,     0,     0,     0,   113,   249,   115,   116,   117,
     118,   119,   120,     0,   122,     0,     0,     0,     0,     0,
     123,   258,     0,     0,     0,     0,     0,     0,   263,     0,
       0,     0,     0,     0,     1,     0,     2,   266,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,     0,     0,
       0,    13,    14,    15,     0,    16,    17,    18,    19,     0,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,     0,     0,     0,     0,     0,    31,     0,    32,    33,
      34,    35,    36,     0,    37,    38,     0,     0,     0,     0,
       0,     0,     0,    39,     0,     0,     0,    40,    41,   154,
      42,     0,    43,    44,     0,     1,    45,     2,    46,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,     0,
       0,     0,    13,    14,    15,     0,    16,    17,    18,    19,
       0,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,     0,     0,     0,     0,     0,    31,     0,    32,
      33,    34,    35,    36,     0,    37,    38,     0,     0,     0,
       0,     0,     0,     0,    39,     0,     0,     0,    40,    41,
     286,    42,     0,    43,    44,     0,     1,    45,     2,    46,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
       0,     0,     0,    13,    14,    15,     0,    16,    17,    18,
      19,     0,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,     0,     0,     0,     0,     0,    31,   104,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,     0,     0,     0,    39,     0,     0,     0,    40,
      41,     0,    42,     0,    43,    44,     0,     0,    45,    94,
      46,     3,     4,   113,     0,   115,   116,   117,   118,   119,
     120,     0,     0,     0,    13,    14,    15,     0,   123,     0,
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
       0,    46,   107,   108,   109,   110,   111,     0,   112,   104,
     105,   106,     0,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   104,     0,   106,     0,     0,   123,     0,
       0,     0,   107,   108,   109,   110,   111,     0,   112,     0,
     245,     0,     0,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   104,   105,   106,     0,   113,   123,   115,
     116,   117,   118,   119,   120,     0,     0,     0,   217,     0,
       0,     0,   123,     0,     0,     0,   107,   108,   109,   110,
     111,     0,   112,     0,   104,   105,   106,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,     0,     0,     0,
       0,     0,   123,     0,     0,     0,     0,   107,   108,   109,
     110,   111,   244,   112,     0,     0,     0,     0,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   104,   105,
     106,     0,     0,   123,     0,     0,     0,     0,     0,     0,
     206,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   107,   108,   109,   110,   111,     0,   112,     0,     0,
       0,     0,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   104,   105,   106,     0,     0,   123,     0,     0,
       0,     0,     0,     0,   257,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   107,   108,   109,   110,   111,
       0,   112,     0,     0,     0,     0,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   104,   105,   106,     0,
       0,   123,     0,     0,     0,     0,    43,    44,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   107,
     108,   109,   110,   111,     0,   112,     0,   104,   105,   106,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
       0,     0,     0,     0,     0,   123,     0,     0,     0,   158,
     107,   108,   109,   110,   111,     0,   112,     0,   104,   105,
     106,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,     0,     0,     0,     0,     0,   123,     0,     0,     0,
     194,   107,   108,   109,   110,   111,     0,   112,     0,   104,
     105,   106,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,     0,     0,     0,     0,     0,   123,     0,     0,
       0,   195,   107,   108,   109,   110,   111,     0,   112,     0,
     104,   105,   106,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,     0,     0,     0,     0,     0,   123,     0,
       0,     0,   196,   107,   108,   109,   110,   111,     0,   112,
       0,   104,   105,   106,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,     0,     0,     0,     0,     0,   123,
       0,     0,     0,   213,   107,   108,   109,   110,   111,     0,
     112,     0,   104,   105,   106,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,     0,     0,     0,     0,     0,
     123,     0,     0,     0,   277,   107,   108,   109,   110,   111,
       0,   112,   104,   105,   106,     0,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   218,     0,     0,     0,
       0,   123,     0,     0,     0,   107,   108,   109,   110,   111,
       0,   112,   104,   105,   106,     0,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,     0,     0,     0,     0,
       0,   123,     0,     0,     0,   107,   108,   109,   110,   111,
       0,   112,   104,   105,   106,     0,   113,     0,   115,   116,
     117,   118,   119,   120,     0,     0,     0,     0,     0,     0,
       0,   123,     0,     0,     0,   107,   108,   109,   110,     0,
       0,   112,   104,   105,   106,     0,   113,     0,   115,   116,
     117,   118,   119,   120,     0,     0,     0,     0,     0,     0,
       0,   123,     0,     0,     0,   107,   108,   109,     0,     0,
       0,   112,   104,   105,   106,     0,   113,     0,   115,   116,
     117,   118,   119,   120,     0,     0,     0,     0,     0,     0,
       0,   123,     0,     0,     0,   107,   108,     0,     0,     0,
       0,   112,     0,     0,     0,     0,   113,     0,   115,   116,
     117,   118,   119,   120,     0,     0,     0,     0,     0,     0,
       0,   123
};

static const yytype_int16 yycheck[] =
{
       0,     0,     4,    59,    49,     1,    46,    47,    49,    15,
      16,    17,     8,    66,    55,    67,    16,    16,    71,    64,
      76,    21,    47,    64,    20,    21,    22,    47,    68,    68,
      68,    31,     2,    72,    72,    47,    36,    37,    38,    39,
      40,    68,    42,    49,    71,    51,    52,    53,    54,    55,
      56,    68,    46,    47,    68,    72,    52,    71,    64,    68,
      47,    65,    71,    67,    60,    63,    64,    67,    68,    69,
      66,    73,    65,    49,    67,    51,    52,    53,    54,    55,
      56,    77,    82,    69,    70,    71,    67,    57,    64,    72,
      67,    74,    67,    93,     0,    95,    69,    70,    67,    15,
      16,    17,    67,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,    83,   114,   115,   116,   117,   118,   119,
     120,   121,   122,    67,    94,    67,    67,    67,    44,    67,
      67,    46,   102,    49,    67,    51,    52,    53,    54,    55,
      56,    65,    67,    46,    23,    47,    47,    47,    64,    71,
      49,     6,    71,    47,    68,   125,    71,    72,    73,   159,
      67,   161,   162,    18,    19,    69,    71,    59,    46,    67,
      85,    86,    87,    68,    71,    30,    31,    32,    33,    34,
      35,    36,    37,    68,    68,    68,    66,    68,    26,    57,
      45,    46,    47,    48,   190,   191,   192,    68,   198,   198,
     200,   200,    67,    67,    49,    68,   206,    69,    53,    54,
      55,    56,    67,   128,    68,    68,    68,    57,   218,    64,
      68,    68,    68,   219,    48,   284,   222,   274,   143,    15,
      16,    17,    65,   261,   212,    -1,    -1,    -1,    -1,    -1,
      -1,   241,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     220,    -1,    38,   253,   253,   251,    -1,   257,    44,   259,
      -1,    -1,   262,    49,    -1,    51,    52,    53,    54,    55,
      56,    15,    16,    17,   270,    -1,   276,   276,    64,    -1,
      -1,    -1,   197,    -1,   199,   285,    -1,    -1,   203,   204,
     205,    -1,    -1,    -1,    38,    39,    40,    41,    42,    -1,
      44,    -1,    -1,    -1,    -1,    49,   221,    51,    52,    53,
      54,    55,    56,    -1,    58,    -1,    -1,    -1,    -1,    -1,
      64,   236,    -1,    -1,    -1,    -1,    -1,    -1,   243,    -1,
      -1,    -1,    -1,    -1,     1,    -1,     3,   252,     5,     6,
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
      35,    36,    37,    -1,    -1,    -1,    -1,    -1,    43,    15,
      45,    46,    47,    48,    49,    -1,    51,    52,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    60,    -1,    -1,    -1,    64,
      65,    -1,    67,    -1,    69,    70,    -1,    -1,    73,     3,
      75,     5,     6,    49,    -1,    51,    52,    53,    54,    55,
      56,    -1,    -1,    -1,    18,    19,    20,    -1,    64,    -1,
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
      -1,    75,    38,    39,    40,    41,    42,    -1,    44,    15,
      16,    17,    -1,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    15,    -1,    17,    -1,    -1,    64,    -1,
      -1,    -1,    38,    39,    40,    41,    42,    -1,    44,    -1,
      76,    -1,    -1,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    15,    16,    17,    -1,    49,    64,    51,
      52,    53,    54,    55,    56,    -1,    -1,    -1,    74,    -1,
      -1,    -1,    64,    -1,    -1,    -1,    38,    39,    40,    41,
      42,    -1,    44,    -1,    15,    16,    17,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    -1,    -1,
      -1,    -1,    64,    -1,    -1,    -1,    -1,    38,    39,    40,
      41,    42,    74,    44,    -1,    -1,    -1,    -1,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    15,    16,
      17,    -1,    -1,    64,    -1,    -1,    -1,    -1,    -1,    -1,
      71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    38,    39,    40,    41,    42,    -1,    44,    -1,    -1,
      -1,    -1,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    15,    16,    17,    -1,    -1,    64,    -1,    -1,
      -1,    -1,    -1,    -1,    71,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    38,    39,    40,    41,    42,
      -1,    44,    -1,    -1,    -1,    -1,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    15,    16,    17,    -1,
      -1,    64,    -1,    -1,    -1,    -1,    69,    70,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,
      39,    40,    41,    42,    -1,    44,    -1,    15,    16,    17,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      -1,    -1,    -1,    -1,    -1,    64,    -1,    -1,    -1,    68,
      38,    39,    40,    41,    42,    -1,    44,    -1,    15,    16,
      17,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    -1,    -1,    -1,    -1,    -1,    64,    -1,    -1,    -1,
      68,    38,    39,    40,    41,    42,    -1,    44,    -1,    15,
      16,    17,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    -1,    -1,    -1,    -1,    64,    -1,    -1,
      -1,    68,    38,    39,    40,    41,    42,    -1,    44,    -1,
      15,    16,    17,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    -1,    -1,    -1,    -1,    64,    -1,
      -1,    -1,    68,    38,    39,    40,    41,    42,    -1,    44,
      -1,    15,    16,    17,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    -1,    -1,    -1,    -1,    64,
      -1,    -1,    -1,    68,    38,    39,    40,    41,    42,    -1,
      44,    -1,    15,    16,    17,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    -1,    -1,    -1,    -1,    -1,
      64,    -1,    -1,    -1,    68,    38,    39,    40,    41,    42,
      -1,    44,    15,    16,    17,    -1,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    -1,
      -1,    64,    -1,    -1,    -1,    38,    39,    40,    41,    42,
      -1,    44,    15,    16,    17,    -1,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    -1,    -1,    -1,    -1,
      -1,    64,    -1,    -1,    -1,    38,    39,    40,    41,    42,
      -1,    44,    15,    16,    17,    -1,    49,    -1,    51,    52,
      53,    54,    55,    56,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    64,    -1,    -1,    -1,    38,    39,    40,    41,    -1,
      -1,    44,    15,    16,    17,    -1,    49,    -1,    51,    52,
      53,    54,    55,    56,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    64,    -1,    -1,    -1,    38,    39,    40,    -1,    -1,
      -1,    44,    15,    16,    17,    -1,    49,    -1,    51,    52,
      53,    54,    55,    56,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    64,    -1,    -1,    -1,    38,    39,    -1,    -1,    -1,
      -1,    44,    -1,    -1,    -1,    -1,    49,    -1,    51,    52,
      53,    54,    55,    56,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    64
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
      56,    57,    58,    64,    89,    65,    99,    86,    67,    89,
      46,    94,    94,    94,    71,    89,   102,   102,   102,    23,
      96,    96,    96,    67,    89,    47,    47,    47,   102,    68,
      99,    96,    96,    96,    66,    79,    99,   102,    68,    72,
      74,    71,    59,    76,    99,   102,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   102,    92,    99,    68,    67,    96,    71,
      81,    83,    85,    47,    68,    68,    68,    67,    68,    69,
      68,    96,    68,    71,    71,    71,    71,    68,    68,    66,
      68,    72,    68,    68,   102,   102,   102,    74,    59,    66,
      71,    87,    68,    46,    89,    89,    89,    96,    79,    96,
      79,    68,    96,    96,    96,   102,    67,    59,    98,    99,
     100,    67,   101,    67,    74,    76,   102,    89,    99,    96,
      89,    68,    69,    26,    68,    68,    68,    71,    96,    57,
      68,    71,    57,    96,    88,    89,    96,    79,   102,    68,
     102,   100,   102,    68,    72,    68,    68,    68,    89,    98,
      48,    90,    79,    68,    65,    95,    66
};

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
     102,   102,   102,   102,   102,   102,   102
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
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
       2,     3,     3,     2,     3,     3,     3,     3,     3,     3,
       5,     3,     5,     4,     4,     5,     2
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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
            else
              goto append;

          append:
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

  return (YYSIZE_T) (yystpcpy (yyres, yystr) - yyres);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
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
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yynewstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  *yyssp = (yytype_int16) yystate;

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = (YYSIZE_T) (yyssp - yyss + 1);

# if defined yyoverflow
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
# else /* defined YYSTACK_RELOCATE */
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
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
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
      if (yytable_value_is_error (yyn))
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
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

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
#line 131 "parse.y" /* yacc.c:1652  */
    { 
				parse_snode = (yyvsp[0].s); 
				if ( yychar >= 0 ) 
					fprintf(stderr,
						"Warning: a token was wasted after an 'if' statement without 'else'.\n");
				YYACCEPT; 
			}
#line 1763 "y.tab.c" /* yacc.c:1652  */
    break;

  case 3:
#line 140 "parse.y" /* yacc.c:1652  */
    { (yyval.s) = 0; }
#line 1769 "y.tab.c" /* yacc.c:1652  */
    break;

  case 4:
#line 141 "parse.y" /* yacc.c:1652  */
    { gdef=1; }
#line 1775 "y.tab.c" /* yacc.c:1652  */
    break;

  case 5:
#line 141 "parse.y" /* yacc.c:1652  */
    { gdef=0; }
#line 1781 "y.tab.c" /* yacc.c:1652  */
    break;

  case 6:
#line 142 "parse.y" /* yacc.c:1652  */
    { (yyval.s) = 0; NOPR; }
#line 1787 "y.tab.c" /* yacc.c:1652  */
    break;

  case 7:
#line 143 "parse.y" /* yacc.c:1652  */
    { mgdef=1; }
#line 1793 "y.tab.c" /* yacc.c:1652  */
    break;

  case 8:
#line 143 "parse.y" /* yacc.c:1652  */
    { mgdef=0; }
#line 1799 "y.tab.c" /* yacc.c:1652  */
    break;

  case 9:
#line 144 "parse.y" /* yacc.c:1652  */
    { (yyval.s) = 0; NOPR; }
#line 1805 "y.tab.c" /* yacc.c:1652  */
    break;

  case 10:
#line 145 "parse.y" /* yacc.c:1652  */
    { ldef=1; }
#line 1811 "y.tab.c" /* yacc.c:1652  */
    break;

  case 11:
#line 145 "parse.y" /* yacc.c:1652  */
    { ldef=0; }
#line 1817 "y.tab.c" /* yacc.c:1652  */
    break;

  case 12:
#line 146 "parse.y" /* yacc.c:1652  */
    { (yyval.s) = 0; NOPR; }
#line 1823 "y.tab.c" /* yacc.c:1652  */
    break;

  case 13:
#line 148 "parse.y" /* yacc.c:1652  */
    { appenduflist((yyvsp[-1].n)); (yyval.s) = 0; NOPR; }
#line 1829 "y.tab.c" /* yacc.c:1652  */
    break;

  case 14:
#line 150 "parse.y" /* yacc.c:1652  */
    { structdef((yyvsp[-4].p),(yyvsp[-2].n)); (yyval.s) = 0; NOPR; }
#line 1835 "y.tab.c" /* yacc.c:1652  */
    break;

  case 15:
#line 152 "parse.y" /* yacc.c:1652  */
    { (yyval.s) = mksnode(1,S_SINGLE,(yyvsp[-1].f)); }
#line 1841 "y.tab.c" /* yacc.c:1652  */
    break;

  case 16:
#line 154 "parse.y" /* yacc.c:1652  */
    { (yyval.s) = (yyvsp[0].s); }
#line 1847 "y.tab.c" /* yacc.c:1652  */
    break;

  case 17:
#line 156 "parse.y" /* yacc.c:1652  */
    { (yyval.s) = mksnode(0,S_BREAK); }
#line 1853 "y.tab.c" /* yacc.c:1652  */
    break;

  case 18:
#line 158 "parse.y" /* yacc.c:1652  */
    { (yyval.s) = mksnode(0,S_CONTINUE); }
#line 1859 "y.tab.c" /* yacc.c:1652  */
    break;

  case 19:
#line 160 "parse.y" /* yacc.c:1652  */
    { (yyval.s) = mksnode(1,S_RETURN,NULLP); }
#line 1865 "y.tab.c" /* yacc.c:1652  */
    break;

  case 20:
#line 162 "parse.y" /* yacc.c:1652  */
    { (yyval.s) = mksnode(1,S_RETURN,(yyvsp[-1].f)); }
#line 1871 "y.tab.c" /* yacc.c:1652  */
    break;

  case 21:
#line 164 "parse.y" /* yacc.c:1652  */
    { (yyval.s) = mksnode(4,S_IFELSE,(yyvsp[-4].i),(yyvsp[-2].n),(yyvsp[0].s),NULLP); (yyvsp[0].s)?(yyval.s)->ln=(yyvsp[0].s)->ln:0; NOPR; }
#line 1877 "y.tab.c" /* yacc.c:1652  */
    break;

  case 22:
#line 166 "parse.y" /* yacc.c:1652  */
    { (yyval.s) = mksnode(4,S_IFELSE,(yyvsp[-6].i),(yyvsp[-4].n),(yyvsp[-2].s),(yyvsp[0].s)); (yyvsp[0].s)?(yyval.s)->ln=(yyvsp[0].s)->ln:0; NOPR; }
#line 1883 "y.tab.c" /* yacc.c:1652  */
    break;

  case 23:
#line 168 "parse.y" /* yacc.c:1652  */
    { (yyval.s) = mksnode(5,S_FOR,(yyvsp[-8].i),(yyvsp[-6].n),(yyvsp[-4].n)?(yyvsp[-4].n):ONENODE,(yyvsp[-2].n),(yyvsp[0].s)); (yyvsp[0].s)?(yyval.s)->ln=(yyvsp[0].s)->ln:0; NOPR; }
#line 1889 "y.tab.c" /* yacc.c:1652  */
    break;

  case 24:
#line 170 "parse.y" /* yacc.c:1652  */
    { (yyval.s) = mksnode(5,S_FOR,(yyvsp[-4].i),NULLP,(yyvsp[-2].n),NULLP,(yyvsp[0].s)); (yyvsp[0].s)?(yyval.s)->ln=(yyvsp[0].s)->ln:0; NOPR; }
#line 1895 "y.tab.c" /* yacc.c:1652  */
    break;

  case 25:
#line 172 "parse.y" /* yacc.c:1652  */
    { (yyval.s) = mksnode(3,S_DO,(yyvsp[-6].i),(yyvsp[-5].s),(yyvsp[-2].n)); NOPR; }
#line 1901 "y.tab.c" /* yacc.c:1652  */
    break;

  case 26:
#line 174 "parse.y" /* yacc.c:1652  */
    { (yyval.s) = mksnode(3,S_PFDEF,(yyvsp[-7].p),(yyvsp[-5].n),(yyvsp[-1].f)); NOPR; }
#line 1907 "y.tab.c" /* yacc.c:1652  */
    break;

  case 27:
#line 176 "parse.y" /* yacc.c:1652  */
    { (yyval.s) = mksnode(3,S_PFDEF,(yyvsp[-4].p),(yyvsp[-2].n),NULLP); NOPR; }
#line 1913 "y.tab.c" /* yacc.c:1652  */
    break;

  case 28:
#line 177 "parse.y" /* yacc.c:1652  */
    { mkpvs((yyvsp[0].p)); }
#line 1919 "y.tab.c" /* yacc.c:1652  */
    break;

  case 29:
#line 177 "parse.y" /* yacc.c:1652  */
    { ldef = 1; }
#line 1925 "y.tab.c" /* yacc.c:1652  */
    break;

  case 30:
#line 177 "parse.y" /* yacc.c:1652  */
    { ldef = -1; }
#line 1931 "y.tab.c" /* yacc.c:1652  */
    break;

  case 31:
#line 178 "parse.y" /* yacc.c:1652  */
    {
				mkuf((yyvsp[-10].p),asir_infile->name,(yyvsp[-6].n),
					mksnode(1,S_CPLX,(yyvsp[-1].n)),(yyvsp[-11].i),asir_infile->ln,(yyvsp[-3].p),CUR_MODULE); 
				(yyval.s) = 0; NOPR;
			}
#line 1941 "y.tab.c" /* yacc.c:1652  */
    break;

  case 32:
#line 184 "parse.y" /* yacc.c:1652  */
    { 
				CUR_MODULE = mkmodule((yyvsp[-1].p));
				MPVS = CUR_MODULE->pvs; 
				(yyval.s) = mksnode(1,S_MODULE,CUR_MODULE); NOPR;
			}
#line 1951 "y.tab.c" /* yacc.c:1652  */
    break;

  case 33:
#line 190 "parse.y" /* yacc.c:1652  */
    { CUR_MODULE = 0; MPVS = 0; (yyval.s) = mksnode(1,S_MODULE,NULLP); NOPR; }
#line 1957 "y.tab.c" /* yacc.c:1652  */
    break;

  case 34:
#line 192 "parse.y" /* yacc.c:1652  */
    { yyerrok; (yyval.s) = 0; }
#line 1963 "y.tab.c" /* yacc.c:1652  */
    break;

  case 35:
#line 195 "parse.y" /* yacc.c:1652  */
    { if ( main_parser ) prresult = 1; }
#line 1969 "y.tab.c" /* yacc.c:1652  */
    break;

  case 36:
#line 197 "parse.y" /* yacc.c:1652  */
    { if ( main_parser ) prresult = 0; }
#line 1975 "y.tab.c" /* yacc.c:1652  */
    break;

  case 37:
#line 200 "parse.y" /* yacc.c:1652  */
    { (yyval.p) = 0; }
#line 1981 "y.tab.c" /* yacc.c:1652  */
    break;

  case 38:
#line 202 "parse.y" /* yacc.c:1652  */
    { (yyval.p) = (yyvsp[0].p); }
#line 1987 "y.tab.c" /* yacc.c:1652  */
    break;

  case 39:
#line 205 "parse.y" /* yacc.c:1652  */
    { (yyval.s) = mksnode(1,S_CPLX,(yyvsp[-1].n)); }
#line 1993 "y.tab.c" /* yacc.c:1652  */
    break;

  case 40:
#line 208 "parse.y" /* yacc.c:1652  */
    { MKNODE((yyval.n),(yyvsp[0].p),0); }
#line 1999 "y.tab.c" /* yacc.c:1652  */
    break;

  case 41:
#line 210 "parse.y" /* yacc.c:1652  */
    { appendtonode((yyvsp[-2].n),(yyvsp[0].p),&(yyval.n)); }
#line 2005 "y.tab.c" /* yacc.c:1652  */
    break;

  case 42:
#line 213 "parse.y" /* yacc.c:1652  */
    { MKNODE((yyval.n),(yyvsp[0].p),0); }
#line 2011 "y.tab.c" /* yacc.c:1652  */
    break;

  case 43:
#line 215 "parse.y" /* yacc.c:1652  */
    { appendtonode((yyvsp[-2].n),(yyvsp[0].p),&(yyval.n)); }
#line 2017 "y.tab.c" /* yacc.c:1652  */
    break;

  case 44:
#line 218 "parse.y" /* yacc.c:1652  */
    { val = (pointer)((long)makepvar((yyvsp[0].p))); MKNODE((yyval.n),val,0); }
#line 2023 "y.tab.c" /* yacc.c:1652  */
    break;

  case 45:
#line 220 "parse.y" /* yacc.c:1652  */
    { appendtonode((yyvsp[-2].n),(pointer)((long)makepvar((yyvsp[0].p))),&(yyval.n)); }
#line 2029 "y.tab.c" /* yacc.c:1652  */
    break;

  case 46:
#line 223 "parse.y" /* yacc.c:1652  */
    { (yyval.n) = 0; }
#line 2035 "y.tab.c" /* yacc.c:1652  */
    break;

  case 47:
#line 225 "parse.y" /* yacc.c:1652  */
    { appendtonode((yyvsp[-1].n),(pointer)(yyvsp[0].s),&(yyval.n)); }
#line 2041 "y.tab.c" /* yacc.c:1652  */
    break;

  case 48:
#line 228 "parse.y" /* yacc.c:1652  */
    { (yyval.n) = 0; }
#line 2047 "y.tab.c" /* yacc.c:1652  */
    break;

  case 49:
#line 230 "parse.y" /* yacc.c:1652  */
    { (yyval.n) = (yyvsp[0].n); }
#line 2053 "y.tab.c" /* yacc.c:1652  */
    break;

  case 50:
#line 233 "parse.y" /* yacc.c:1652  */
    { MKNODE((yyval.n),(yyvsp[0].f),0); }
#line 2059 "y.tab.c" /* yacc.c:1652  */
    break;

  case 51:
#line 235 "parse.y" /* yacc.c:1652  */
    { appendtonode((yyvsp[-2].n),(pointer)(yyvsp[0].f),&(yyval.n)); }
#line 2065 "y.tab.c" /* yacc.c:1652  */
    break;

  case 52:
#line 238 "parse.y" /* yacc.c:1652  */
    { MKNODE((yyval.n),(yyvsp[0].f),0); }
#line 2071 "y.tab.c" /* yacc.c:1652  */
    break;

  case 53:
#line 240 "parse.y" /* yacc.c:1652  */
    { appendtonode((yyvsp[-2].n),(pointer)(yyvsp[0].f),&(yyval.n)); }
#line 2077 "y.tab.c" /* yacc.c:1652  */
    break;

  case 54:
#line 243 "parse.y" /* yacc.c:1652  */
    { (yyval.p) = (yyvsp[0].p); }
#line 2083 "y.tab.c" /* yacc.c:1652  */
    break;

  case 55:
#line 245 "parse.y" /* yacc.c:1652  */
    { (yyval.p) = (yyvsp[0].p); }
#line 2089 "y.tab.c" /* yacc.c:1652  */
    break;

  case 56:
#line 248 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(2,I_OPT,(yyvsp[-2].p),(yyvsp[0].f)); }
#line 2095 "y.tab.c" /* yacc.c:1652  */
    break;

  case 57:
#line 251 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(1,I_STR,(yyvsp[0].p)); }
#line 2101 "y.tab.c" /* yacc.c:1652  */
    break;

  case 58:
#line 253 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(1,I_FORMULA,(yyvsp[0].p)); }
#line 2107 "y.tab.c" /* yacc.c:1652  */
    break;

  case 59:
#line 255 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(1,I_ANS,(yyvsp[0].i)); }
#line 2113 "y.tab.c" /* yacc.c:1652  */
    break;

  case 60:
#line 257 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(0,I_GF2NGEN); }
#line 2119 "y.tab.c" /* yacc.c:1652  */
    break;

  case 61:
#line 259 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(0,I_GFPNGEN); }
#line 2125 "y.tab.c" /* yacc.c:1652  */
    break;

  case 62:
#line 261 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(0,I_GFSNGEN); }
#line 2131 "y.tab.c" /* yacc.c:1652  */
    break;

  case 63:
#line 263 "parse.y" /* yacc.c:1652  */
    {
				FUNC f;

				searchf(noargsysf,(yyvsp[0].p),&f);
				if ( f )
					 (yyval.f) = mkfnode(2,I_FUNC,f,NULLP);
				else {
					searchc((yyvsp[0].p),&f);
					if ( f )
						(yyval.f) = mkfnode(2,I_FUNC,f,mkfnode(1,I_LIST,NULLP));
					else {
						gen_searchf_searchonly((yyvsp[0].p),(FUNC *)&f,1);
						if ( f )
							makesrvar(f,(P *)&val);
						else
							makevar((yyvsp[0].p),(P *)&val);
						(yyval.f) = mkfnode(1,I_FORMULA,val);
					}
				}
			}
#line 2156 "y.tab.c" /* yacc.c:1652  */
    break;

  case 64:
#line 284 "parse.y" /* yacc.c:1652  */
    {
				gen_searchf((yyvsp[-3].p),(FUNC *)&val);
				print_crossref(val);
				(yyval.f) = mkfnode(2,I_FUNC,val,mkfnode(1,I_LIST,(yyvsp[-1].n)));
			}
#line 2166 "y.tab.c" /* yacc.c:1652  */
    break;

  case 65:
#line 290 "parse.y" /* yacc.c:1652  */
    {
				gen_searchf((yyvsp[-5].p),(FUNC *)&val);
				print_crossref(val);
				(yyval.f) = mkfnode(3,I_FUNC_OPT,val,
					mkfnode(1,I_LIST,(yyvsp[-3].n)),mkfnode(1,I_LIST,(yyvsp[-1].n)));
			}
#line 2177 "y.tab.c" /* yacc.c:1652  */
    break;

  case 66:
#line 297 "parse.y" /* yacc.c:1652  */
    {
				gen_searchf((yyvsp[-3].p),(FUNC *)&val);
				print_crossref(val);
				(yyval.f) = mkfnode(2,I_MAP,val,mkfnode(1,I_LIST,(yyvsp[-1].n)));
			}
#line 2187 "y.tab.c" /* yacc.c:1652  */
    break;

  case 67:
#line 303 "parse.y" /* yacc.c:1652  */
    {
				gen_searchf((yyvsp[-3].p),(FUNC *)&val);
				print_crossref(val);
				(yyval.f) = mkfnode(2,I_RECMAP,val,mkfnode(1,I_LIST,(yyvsp[-1].n)));
			}
#line 2197 "y.tab.c" /* yacc.c:1652  */
    break;

  case 68:
#line 309 "parse.y" /* yacc.c:1652  */
    {
				searchpf((yyvsp[-6].p),(FUNC *)&val);
				(yyval.f) = mkfnode(3,I_PFDERIV,val,mkfnode(1,I_LIST,(yyvsp[-1].n)),mkfnode(1,I_LIST,(yyvsp[-4].n)));
			}
#line 2206 "y.tab.c" /* yacc.c:1652  */
    break;

  case 69:
#line 314 "parse.y" /* yacc.c:1652  */
    {
				(yyval.f) = mkfnode(2,I_GETOPT,(yyvsp[-1].p));
			}
#line 2214 "y.tab.c" /* yacc.c:1652  */
    break;

  case 70:
#line 318 "parse.y" /* yacc.c:1652  */
    {
				(yyval.f) = mkfnode(2,I_GETOPT,NULLP);
			}
#line 2222 "y.tab.c" /* yacc.c:1652  */
    break;

  case 71:
#line 322 "parse.y" /* yacc.c:1652  */
    {
				(yyval.f) = mkfnode(3,I_TIMER,(yyvsp[-5].f),(yyvsp[-3].f),(yyvsp[-1].f));
			}
#line 2230 "y.tab.c" /* yacc.c:1652  */
    break;

  case 72:
#line 326 "parse.y" /* yacc.c:1652  */
    {
				searchf(parif,(yyvsp[-1].p),(FUNC *)&val);
				if ( !val )
					mkparif((yyvsp[-1].p),(FUNC *)&val);
				(yyval.f) = mkfnode(2,I_FUNC,val,NULLP);
			}
#line 2241 "y.tab.c" /* yacc.c:1652  */
    break;

  case 73:
#line 333 "parse.y" /* yacc.c:1652  */
    {
				searchf(parif,(yyvsp[-3].p),(FUNC *)&val);
				if ( !val )
					mkparif((yyvsp[-3].p),(FUNC *)&val);
				(yyval.f) = mkfnode(2,I_FUNC,val,mkfnode(1,I_LIST,(yyvsp[-1].n)));
			}
#line 2252 "y.tab.c" /* yacc.c:1652  */
    break;

  case 74:
#line 340 "parse.y" /* yacc.c:1652  */
    {
				(yyval.f) = mkfnode(2,I_IFUNC,(yyvsp[-4].f),mkfnode(1,I_LIST,(yyvsp[-1].n)),NULLP);
			}
#line 2260 "y.tab.c" /* yacc.c:1652  */
    break;

  case 75:
#line 344 "parse.y" /* yacc.c:1652  */
    {
				(yyval.f) = mkfnode(3,I_IFUNC,(yyvsp[-6].f),mkfnode(1,I_LIST,(yyvsp[-3].n)),
					mkfnode(1,I_LIST,(yyvsp[-1].n)));
			}
#line 2269 "y.tab.c" /* yacc.c:1652  */
    break;

  case 76:
#line 349 "parse.y" /* yacc.c:1652  */
    {
				if ( main_parser || allow_create_var )
					t = mkfnode(2,I_PVAR,makepvar((yyvsp[-3].p)),NULLP);
				else {
					ind = searchpvar((yyvsp[-3].p));
					if ( ind == -1 ) {
						fprintf(stderr,"%s : no such variable.\n",(yyvsp[-3].p));
						YYABORT;
					} else
						t = mkfnode(2,I_PVAR,ind,NULLP);
				}
				(yyval.f) = mkfnode(2,I_IFUNC,t,mkfnode(1,I_LIST,(yyvsp[-1].n)));
			}
#line 2287 "y.tab.c" /* yacc.c:1652  */
    break;

  case 77:
#line 363 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(1,I_CAR,(yyvsp[-1].f)); }
#line 2293 "y.tab.c" /* yacc.c:1652  */
    break;

  case 78:
#line 365 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(1,I_CDR,(yyvsp[-1].f)); }
#line 2299 "y.tab.c" /* yacc.c:1652  */
    break;

  case 79:
#line 367 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(1,I_PAREN,(yyvsp[-1].f)); }
#line 2305 "y.tab.c" /* yacc.c:1652  */
    break;

  case 80:
#line 369 "parse.y" /* yacc.c:1652  */
    { 
				if ( main_parser || allow_create_var )
					(yyval.f) = mkfnode(2,I_PVAR,makepvar((yyvsp[0].p)),NULLP);
				else {
					ind = searchpvar((yyvsp[0].p));
					if ( ind == -1 ) {
						fprintf(stderr,"%s : no such variable.\n",(yyvsp[0].p));
						YYABORT;
					} else
						(yyval.f) = mkfnode(2,I_PVAR,ind,NULLP);
				}
			}
#line 2322 "y.tab.c" /* yacc.c:1652  */
    break;

  case 81:
#line 382 "parse.y" /* yacc.c:1652  */
    {
				if ( (yyvsp[-3].f)->id == I_PVAR || (yyvsp[-3].f)->id == I_INDEX ) {
					appendtonode((NODE)(yyvsp[-3].f)->arg[1],(pointer)(yyvsp[-1].f),&a);
					(yyvsp[-3].f)->arg[1] = (pointer)a; (yyval.f) = (yyvsp[-3].f);
				} else {
					MKNODE(a,(yyvsp[-1].f),0);
					(yyval.f) = mkfnode(2,I_INDEX,(pointer)(yyvsp[-3].f),a);
				}
			}
#line 2336 "y.tab.c" /* yacc.c:1652  */
    break;

  case 82:
#line 392 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(2,I_POINT,(yyvsp[-2].f),(yyvsp[0].p)); }
#line 2342 "y.tab.c" /* yacc.c:1652  */
    break;

  case 83:
#line 395 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = (yyvsp[0].f); }
#line 2348 "y.tab.c" /* yacc.c:1652  */
    break;

  case 84:
#line 397 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(3,I_CAST,structtoindex((yyvsp[-2].p)),(yyvsp[0].f),NULLP); }
#line 2354 "y.tab.c" /* yacc.c:1652  */
    break;

  case 85:
#line 399 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(2,I_ASSPVAR,(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2360 "y.tab.c" /* yacc.c:1652  */
    break;

  case 86:
#line 401 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(2,I_ASSPVAR,(yyvsp[-2].f),mkfnode(3,I_BOP,(yyvsp[-1].p),(yyvsp[-2].f),(yyvsp[0].f))); }
#line 2366 "y.tab.c" /* yacc.c:1652  */
    break;

  case 87:
#line 403 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(2,I_POSTSELF,(yyvsp[0].p),(yyvsp[-1].f)); }
#line 2372 "y.tab.c" /* yacc.c:1652  */
    break;

  case 88:
#line 405 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(2,I_PRESELF,(yyvsp[-1].p),(yyvsp[0].f)); }
#line 2378 "y.tab.c" /* yacc.c:1652  */
    break;

  case 89:
#line 407 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(1,I_LIST,(yyvsp[-1].n)); }
#line 2384 "y.tab.c" /* yacc.c:1652  */
    break;

  case 90:
#line 409 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = (yyvsp[0].f); }
#line 2390 "y.tab.c" /* yacc.c:1652  */
    break;

  case 91:
#line 411 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(1,I_MINUS,(yyvsp[0].f)); }
#line 2396 "y.tab.c" /* yacc.c:1652  */
    break;

  case 92:
#line 413 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(3,I_BOP,(yyvsp[-1].p),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2402 "y.tab.c" /* yacc.c:1652  */
    break;

  case 93:
#line 415 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(3,I_BOP,(yyvsp[-1].p),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2408 "y.tab.c" /* yacc.c:1652  */
    break;

  case 94:
#line 417 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(3,I_BOP,(yyvsp[-1].p),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2414 "y.tab.c" /* yacc.c:1652  */
    break;

  case 95:
#line 419 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(3,I_BOP,(yyvsp[-1].p),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2420 "y.tab.c" /* yacc.c:1652  */
    break;

  case 96:
#line 421 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(3,I_BOP,(yyvsp[-1].p),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2426 "y.tab.c" /* yacc.c:1652  */
    break;

  case 97:
#line 423 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(3,I_BOP,(yyvsp[-1].p),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2432 "y.tab.c" /* yacc.c:1652  */
    break;

  case 98:
#line 425 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(3,I_COP,(yyvsp[-1].i),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2438 "y.tab.c" /* yacc.c:1652  */
    break;

  case 99:
#line 427 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(1,I_NOT,(yyvsp[0].f)); }
#line 2444 "y.tab.c" /* yacc.c:1652  */
    break;

  case 100:
#line 429 "parse.y" /* yacc.c:1652  */
    { 
				gen_searchf("factorial",(FUNC *)&val);
				print_crossref(val);
				MKNODE(a,(yyvsp[-1].f),0);
				(yyval.f) = mkfnode(2,I_FUNC,val,mkfnode(1,I_LIST,a));
			}
#line 2455 "y.tab.c" /* yacc.c:1652  */
    break;

  case 101:
#line 436 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(2,I_OR,(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2461 "y.tab.c" /* yacc.c:1652  */
    break;

  case 102:
#line 438 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(2,I_AND,(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2467 "y.tab.c" /* yacc.c:1652  */
    break;

  case 103:
#line 440 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[-1].i),(yyvsp[0].f),NULLP); }
#line 2473 "y.tab.c" /* yacc.c:1652  */
    break;

  case 104:
#line 442 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[-1].i),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2479 "y.tab.c" /* yacc.c:1652  */
    break;

  case 105:
#line 444 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[-1].i),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2485 "y.tab.c" /* yacc.c:1652  */
    break;

  case 106:
#line 446 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[-1].i),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2491 "y.tab.c" /* yacc.c:1652  */
    break;

  case 107:
#line 448 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[-1].i),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2497 "y.tab.c" /* yacc.c:1652  */
    break;

  case 108:
#line 450 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[-1].i),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2503 "y.tab.c" /* yacc.c:1652  */
    break;

  case 109:
#line 452 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[-1].i),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2509 "y.tab.c" /* yacc.c:1652  */
    break;

  case 110:
#line 454 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(3,I_CE,(yyvsp[-4].f),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2515 "y.tab.c" /* yacc.c:1652  */
    break;

  case 111:
#line 456 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(1,I_EV,(yyvsp[-1].n)); }
#line 2521 "y.tab.c" /* yacc.c:1652  */
    break;

  case 112:
#line 458 "parse.y" /* yacc.c:1652  */
    { 
        (yyval.f) = mkfnode(2,I_EVM,(yyvsp[-3].n),(yyvsp[-1].f)); 
      }
#line 2529 "y.tab.c" /* yacc.c:1652  */
    break;

  case 113:
#line 462 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(1,I_NEWCOMP,(int)structtoindex((yyvsp[-1].p))); }
#line 2535 "y.tab.c" /* yacc.c:1652  */
    break;

  case 114:
#line 464 "parse.y" /* yacc.c:1652  */
    { MKQUOTE(quote,(yyvsp[-1].f)); (yyval.f) = mkfnode(1,I_FORMULA,(pointer)quote); }
#line 2541 "y.tab.c" /* yacc.c:1652  */
    break;

  case 115:
#line 466 "parse.y" /* yacc.c:1652  */
    { (yyval.f) = mkfnode(2,I_CONS,(yyvsp[-3].n),(yyvsp[-1].f)); }
#line 2547 "y.tab.c" /* yacc.c:1652  */
    break;

  case 116:
#line 468 "parse.y" /* yacc.c:1652  */
    { MKQUOTE(quote,(yyvsp[0].f)); (yyval.f) = mkfnode(1,I_FORMULA,(pointer)quote); }
#line 2553 "y.tab.c" /* yacc.c:1652  */
    break;


#line 2557 "y.tab.c" /* yacc.c:1652  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
#line 470 "parse.y" /* yacc.c:1918  */

