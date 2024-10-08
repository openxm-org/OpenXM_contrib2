/* A Bison parser, made by GNU Bison 3.7.5.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30705

/* Bison version string.  */
#define YYBISON_VERSION "3.7.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 50 "parse/parse.y"

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

#line 102 "parse/y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
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

#include "ytab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_STRUCT = 3,                     /* STRUCT  */
  YYSYMBOL_PNT = 4,                        /* PNT  */
  YYSYMBOL_NEWSTRUCT = 5,                  /* NEWSTRUCT  */
  YYSYMBOL_ANS = 6,                        /* ANS  */
  YYSYMBOL_FDEF = 7,                       /* FDEF  */
  YYSYMBOL_PFDEF = 8,                      /* PFDEF  */
  YYSYMBOL_MODDEF = 9,                     /* MODDEF  */
  YYSYMBOL_MODEND = 10,                    /* MODEND  */
  YYSYMBOL_GLOBAL = 11,                    /* GLOBAL  */
  YYSYMBOL_MGLOBAL = 12,                   /* MGLOBAL  */
  YYSYMBOL_LOCAL = 13,                     /* LOCAL  */
  YYSYMBOL_LOCALF = 14,                    /* LOCALF  */
  YYSYMBOL_CMP = 15,                       /* CMP  */
  YYSYMBOL_OR = 16,                        /* OR  */
  YYSYMBOL_AND = 17,                       /* AND  */
  YYSYMBOL_CAR = 18,                       /* CAR  */
  YYSYMBOL_CDR = 19,                       /* CDR  */
  YYSYMBOL_QUOTED = 20,                    /* QUOTED  */
  YYSYMBOL_COLONCOLON = 21,                /* COLONCOLON  */
  YYSYMBOL_SEMISEMI = 22,                  /* SEMISEMI  */
  YYSYMBOL_DO = 23,                        /* DO  */
  YYSYMBOL_WHILE = 24,                     /* WHILE  */
  YYSYMBOL_FOR = 25,                       /* FOR  */
  YYSYMBOL_IF = 26,                        /* IF  */
  YYSYMBOL_ELSE = 27,                      /* ELSE  */
  YYSYMBOL_BREAK = 28,                     /* BREAK  */
  YYSYMBOL_RETURN = 29,                    /* RETURN  */
  YYSYMBOL_CONTINUE = 30,                  /* CONTINUE  */
  YYSYMBOL_PARIF = 31,                     /* PARIF  */
  YYSYMBOL_MAP = 32,                       /* MAP  */
  YYSYMBOL_RECMAP = 33,                    /* RECMAP  */
  YYSYMBOL_TIMER = 34,                     /* TIMER  */
  YYSYMBOL_GF2NGEN = 35,                   /* GF2NGEN  */
  YYSYMBOL_GFPNGEN = 36,                   /* GFPNGEN  */
  YYSYMBOL_GFSNGEN = 37,                   /* GFSNGEN  */
  YYSYMBOL_GETOPT = 38,                    /* GETOPT  */
  YYSYMBOL_FOP_AND = 39,                   /* FOP_AND  */
  YYSYMBOL_FOP_OR = 40,                    /* FOP_OR  */
  YYSYMBOL_FOP_IMPL = 41,                  /* FOP_IMPL  */
  YYSYMBOL_FOP_REPL = 42,                  /* FOP_REPL  */
  YYSYMBOL_FOP_EQUIV = 43,                 /* FOP_EQUIV  */
  YYSYMBOL_FOP_NOT = 44,                   /* FOP_NOT  */
  YYSYMBOL_LOP = 45,                       /* LOP  */
  YYSYMBOL_FORMULA = 46,                   /* FORMULA  */
  YYSYMBOL_UCASE = 47,                     /* UCASE  */
  YYSYMBOL_LCASE = 48,                     /* LCASE  */
  YYSYMBOL_STR = 49,                       /* STR  */
  YYSYMBOL_SELF = 50,                      /* SELF  */
  YYSYMBOL_BOPASS = 51,                    /* BOPASS  */
  YYSYMBOL_52_ = 52,                       /* '+'  */
  YYSYMBOL_53_ = 53,                       /* '-'  */
  YYSYMBOL_54_ = 54,                       /* '*'  */
  YYSYMBOL_55_ = 55,                       /* '/'  */
  YYSYMBOL_56_ = 56,                       /* '^'  */
  YYSYMBOL_57_ = 57,                       /* '%'  */
  YYSYMBOL_58_ = 58,                       /* '='  */
  YYSYMBOL_59_ = 59,                       /* '?'  */
  YYSYMBOL_60_ = 60,                       /* ':'  */
  YYSYMBOL_61_ = 61,                       /* '`'  */
  YYSYMBOL_62_ = 62,                       /* '&'  */
  YYSYMBOL_PLUS = 63,                      /* PLUS  */
  YYSYMBOL_MINUS = 64,                     /* MINUS  */
  YYSYMBOL_65_ = 65,                       /* '!'  */
  YYSYMBOL_66_ = 66,                       /* '{'  */
  YYSYMBOL_67_ = 67,                       /* '}'  */
  YYSYMBOL_68_ = 68,                       /* '('  */
  YYSYMBOL_69_ = 69,                       /* ')'  */
  YYSYMBOL_70_ = 70,                       /* ';'  */
  YYSYMBOL_71_ = 71,                       /* '$'  */
  YYSYMBOL_72_ = 72,                       /* ','  */
  YYSYMBOL_73_ = 73,                       /* '|'  */
  YYSYMBOL_74_ = 74,                       /* '['  */
  YYSYMBOL_75_ = 75,                       /* ']'  */
  YYSYMBOL_76_ = 76,                       /* '<'  */
  YYSYMBOL_77_ = 77,                       /* '>'  */
  YYSYMBOL_YYACCEPT = 78,                  /* $accept  */
  YYSYMBOL_start = 79,                     /* start  */
  YYSYMBOL_stat = 80,                      /* stat  */
  YYSYMBOL_81_1 = 81,                      /* $@1  */
  YYSYMBOL_82_2 = 82,                      /* $@2  */
  YYSYMBOL_83_3 = 83,                      /* $@3  */
  YYSYMBOL_84_4 = 84,                      /* $@4  */
  YYSYMBOL_85_5 = 85,                      /* $@5  */
  YYSYMBOL_86_6 = 86,                      /* $@6  */
  YYSYMBOL_87_7 = 87,                      /* $@7  */
  YYSYMBOL_88_8 = 88,                      /* $@8  */
  YYSYMBOL_89_9 = 89,                      /* $@9  */
  YYSYMBOL_tail = 90,                      /* tail  */
  YYSYMBOL_desc = 91,                      /* desc  */
  YYSYMBOL_complex = 92,                   /* complex  */
  YYSYMBOL_members = 93,                   /* members  */
  YYSYMBOL_vars = 94,                      /* vars  */
  YYSYMBOL_pvars = 95,                     /* pvars  */
  YYSYMBOL_stats = 96,                     /* stats  */
  YYSYMBOL_node = 97,                      /* node  */
  YYSYMBOL__node = 98,                     /* _node  */
  YYSYMBOL_optlist = 99,                   /* optlist  */
  YYSYMBOL_rawstr = 100,                   /* rawstr  */
  YYSYMBOL_opt = 101,                      /* opt  */
  YYSYMBOL_pexpr = 102,                    /* pexpr  */
  YYSYMBOL_expr = 103                      /* expr  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
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

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
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
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  102
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1381

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  78
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  26
/* YYNRULES -- Number of rules.  */
#define YYNRULES  119
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  294

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   308


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    65,     2,     2,    71,    57,    62,     2,
      68,    69,    54,    52,    72,    53,     2,    55,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    60,    70,
      76,    58,    77,    59,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    74,     2,    75,    56,     2,    61,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    66,    73,    67,     2,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    51,    63,    64
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   130,   130,   139,   141,   141,   141,   143,   143,   143,
     145,   145,   145,   147,   149,   151,   153,   155,   157,   159,
     161,   163,   165,   167,   169,   171,   173,   175,   177,   177,
     177,   177,   183,   189,   191,   194,   196,   198,   202,   203,
     206,   209,   211,   214,   216,   219,   221,   225,   226,   230,
     231,   234,   236,   239,   241,   244,   246,   249,   252,   254,
     256,   258,   260,   262,   264,   285,   291,   298,   304,   310,
     316,   322,   331,   335,   339,   343,   350,   357,   361,   366,
     380,   382,   384,   386,   399,   409,   412,   414,   416,   418,
     420,   422,   424,   426,   428,   430,   432,   434,   436,   438,
     440,   442,   444,   446,   453,   455,   457,   459,   461,   463,
     465,   467,   469,   471,   473,   475,   477,   479,   481,   483
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "STRUCT", "PNT",
  "NEWSTRUCT", "ANS", "FDEF", "PFDEF", "MODDEF", "MODEND", "GLOBAL",
  "MGLOBAL", "LOCAL", "LOCALF", "CMP", "OR", "AND", "CAR", "CDR", "QUOTED",
  "COLONCOLON", "SEMISEMI", "DO", "WHILE", "FOR", "IF", "ELSE", "BREAK",
  "RETURN", "CONTINUE", "PARIF", "MAP", "RECMAP", "TIMER", "GF2NGEN",
  "GFPNGEN", "GFSNGEN", "GETOPT", "FOP_AND", "FOP_OR", "FOP_IMPL",
  "FOP_REPL", "FOP_EQUIV", "FOP_NOT", "LOP", "FORMULA", "UCASE", "LCASE",
  "STR", "SELF", "BOPASS", "'+'", "'-'", "'*'", "'/'", "'^'", "'%'", "'='",
  "'?'", "':'", "'`'", "'&'", "PLUS", "MINUS", "'!'", "'{'", "'}'", "'('",
  "')'", "';'", "'$'", "','", "'|'", "'['", "']'", "'<'", "'>'", "$accept",
  "start", "stat", "$@1", "$@2", "$@3", "$@4", "$@5", "$@6", "$@7", "$@8",
  "$@9", "tail", "desc", "complex", "members", "vars", "pvars", "stats",
  "node", "_node", "optlist", "rawstr", "opt", "pexpr", "expr", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,    43,    45,    42,    47,    94,    37,    61,    63,
      58,    96,    38,   307,   308,    33,   123,   125,    40,    41,
      59,    36,    44,   124,    91,    93,    60,    62
};
#endif

#define YYPACT_NINF (-247)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     478,   -15,   -32,   -58,  -247,   -25,   -12,    -1,   -15,  -247,
    -247,  -247,    13,   -22,    12,    59,  -247,   478,    62,    63,
      73,   -15,   601,   -15,    74,    79,    82,    93,  -247,  -247,
    -247,    97,   713,  -247,   108,    68,  -247,   713,   713,   713,
     713,   713,  -247,   550,  -247,  -247,   713,   713,    95,  -247,
    -247,  -247,    -2,   904,  -247,  -247,  -247,   101,   -32,  -247,
     110,   -15,  -247,   133,   133,   133,  -247,   -18,   713,   713,
     713,   163,   713,   713,   713,  -247,   109,  -247,   904,  -247,
     140,   141,   142,   713,   -39,  1200,   713,   713,   713,   143,
      83,    83,  1200,   -44,   334,   -32,   713,    34,    27,   119,
    1148,   -57,  -247,   -32,   713,   713,   713,   713,   713,   713,
     713,   713,   713,   713,  -247,   713,   713,   713,   713,   713,
     713,   713,   713,   713,  -247,  -247,   -32,   123,   130,   713,
    -247,  -247,   131,   131,   131,   156,  -247,   949,   970,  1015,
     137,   144,   136,   146,   713,  -247,   -38,   139,   145,   814,
    -247,   147,   149,   153,   -45,  -247,  -247,   152,  1036,  -247,
     713,  -247,   713,   713,  -247,  -247,   752,   507,  1316,    99,
    1298,  1273,  1252,   129,  1221,   828,  1148,    83,    83,   -21,
     -21,   -21,   -21,  1148,  1102,   -53,  -247,  -247,  -247,   154,
     165,   -15,   -15,   -15,  -247,  -247,  -247,  -247,   713,   478,
     713,   478,   -11,  -247,   713,   713,   713,   713,  -247,  -247,
     157,   162,   -32,   268,   158,   783,  1148,   193,  -247,   713,
     -15,   -32,   713,   -15,  -247,  -247,  -247,  -247,   159,  -247,
     160,   204,  -247,   168,   -10,    -9,   859,   713,   181,   -24,
     182,  -247,   662,    -2,   713,  -247,  -247,  1169,  -247,  -247,
    -247,  -247,   -15,   713,   478,  -247,  -247,   -32,  -247,   -32,
     713,   172,   713,  -247,   -32,   713,    28,   184,  -247,   188,
    -247,     9,    56,  1081,  -247,   904,  -247,  1148,  -247,   -32,
     210,   478,  -247,  -247,  -247,  -247,    57,  -247,   197,  -247,
    -247,  -247,   406,  -247
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,    60,     0,     0,     0,     0,     4,
       7,    10,     0,     0,     0,     0,    37,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    61,    62,
      63,     0,     0,    59,    83,    64,    58,     0,     0,     0,
       0,     0,    47,     0,    35,    36,    49,    49,     0,     2,
       3,    16,    86,     0,    34,    55,    56,     0,     0,    28,
       0,     0,    33,     0,     0,     0,    43,     0,     0,     0,
       0,     0,    49,    49,    49,    17,    64,    19,     0,    18,
       0,     0,     0,     0,     0,   106,    49,    49,    49,    91,
      93,    94,   119,   102,     0,     0,     0,     0,     0,    50,
      51,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    90,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   103,    15,     0,     0,     0,    49,
      32,    45,     5,     8,    11,     0,    13,     0,     0,     0,
       0,     0,     0,     0,    49,    20,     0,     0,     0,     0,
      73,     0,     0,     0,     0,    40,    48,     0,     0,    82,
       0,    92,     0,     0,   114,    85,     0,   101,   104,   105,
     107,   108,   109,   110,   111,   112,    89,    95,    96,    97,
      98,   100,    99,    88,     0,     0,    41,   116,    29,     0,
       0,     0,     0,     0,    44,    80,    81,   117,    49,     0,
      49,     0,     0,    75,    49,    49,    49,     0,    72,    79,
       0,    65,     0,     0,     0,     0,    52,     0,    84,     0,
       0,     0,    49,     0,    46,     6,     9,    12,     0,    24,
       0,    21,    65,     0,     0,     0,     0,    49,     0,     0,
       0,    53,     0,    87,    49,   118,   115,   113,    14,    42,
      30,    27,     0,    49,     0,    76,    67,     0,    69,     0,
       0,     0,     0,    66,     0,     0,     0,     0,    25,     0,
      22,     0,     0,     0,    71,     0,    54,    57,    77,     0,
      38,     0,    68,    70,    74,    26,     0,    39,     0,    23,
      78,    47,     0,    31
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -247,  -247,     1,  -247,  -247,  -247,  -247,  -247,  -247,  -247,
    -247,  -247,     4,  -247,  -247,  -247,  -247,     2,   -27,    85,
    -247,  -246,    40,     3,    53,     0
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    48,   156,    63,   191,    64,   192,    65,   193,   128,
     222,   267,    50,   288,    51,   185,    67,   132,    94,    98,
      99,   239,   240,   241,    52,   100
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      53,    49,   103,   163,    16,    54,   114,    16,    55,    56,
      58,   271,    62,   272,   220,    55,    56,    53,    71,   221,
     164,   124,    78,    59,   211,    75,    77,    79,   212,   114,
     150,   203,    85,   286,   204,   120,    60,    89,    90,    91,
      92,    93,    57,    97,   124,   263,    68,    61,   264,   105,
     106,   107,    44,    45,   135,    44,    45,   125,   232,   256,
     258,    66,   212,   257,   259,   130,   133,   134,   137,   138,
     139,   136,   104,   108,   109,   110,   111,   112,   282,   113,
      69,   264,   145,   149,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,    53,   102,   158,   278,   127,   124,
     160,   279,   161,   159,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   105,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   151,   283,   290,    70,   264,   264,
      72,    73,   101,   114,    87,   157,    88,   118,   119,   120,
     121,    74,    80,   165,   105,   106,   107,    81,   124,   114,
      82,   116,   117,   118,   119,   120,   121,   141,   142,   143,
     215,    83,   216,   217,   124,    84,   186,   126,   108,   109,
     110,   152,   153,   154,   113,    87,    86,   144,   129,   114,
     131,   116,   117,   118,   119,   120,   121,   140,   146,   147,
     148,   162,   187,   114,   124,   225,   226,   227,   188,    53,
     229,    53,   231,   190,   194,   198,   200,   236,   105,   106,
     107,   205,   224,   199,   189,   201,   208,   206,   209,   247,
     210,   213,   238,   223,   248,   237,   244,   251,   252,   202,
     253,   254,   108,   109,   110,   111,   112,   255,   113,   262,
     265,   274,    97,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   280,    53,   270,   268,   281,   124,   287,
     273,   249,   275,   291,   292,   277,   243,   276,     0,     0,
     246,     0,     0,     0,     4,     0,     0,     0,     0,   285,
       0,    53,   289,   228,     0,   230,    13,    14,     0,   233,
     234,   235,    53,     0,     0,     0,     0,     0,     0,    24,
      25,    26,    27,    28,    29,    30,    31,   250,     0,     0,
       0,     0,     0,     0,    33,    34,    76,    36,     0,     0,
       0,     0,   261,     0,     0,     0,     0,     0,     0,   266,
       0,     0,     0,     0,     0,     1,   242,     2,   269,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,     0,
       0,     0,    13,    14,    15,     0,    16,    17,    18,    19,
      20,     0,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,     0,     0,     0,     0,     0,    32,     0,
      33,    34,    35,    36,    37,     0,    38,    39,     0,     0,
       0,     0,     0,     0,     0,    40,     0,     0,     0,    41,
      42,   155,    43,     0,    44,    45,     0,     1,    46,     2,
      47,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,     0,     0,     0,    13,    14,    15,     0,    16,    17,
      18,    19,    20,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,     0,     0,     0,     0,     0,
      32,     0,    33,    34,    35,    36,    37,     0,    38,    39,
       0,     0,     0,     0,     0,     0,     0,    40,     0,     0,
       0,    41,    42,   293,    43,     0,    44,    45,     0,     1,
      46,     2,    47,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,     0,     0,     0,    13,    14,    15,     0,
      16,    17,    18,    19,    20,     0,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,     0,     0,     0,
       0,     0,    32,     0,    33,    34,    35,    36,    37,     0,
      38,    39,     0,     0,     0,     0,     0,     0,     0,    40,
       0,     0,     0,    41,    42,     0,    43,     0,    44,    45,
       0,     0,    46,    95,    47,     3,     4,   114,     0,   116,
     117,   118,   119,   120,   121,     0,     0,     0,    13,    14,
      15,     0,   124,     0,     0,     0,     0,     0,     0,     0,
       0,    24,    25,    26,    27,    28,    29,    30,    31,     0,
       0,     0,     0,     0,    32,     0,    33,    34,    76,    36,
      37,     0,    38,    39,    96,     0,     3,     4,     0,     0,
       0,    40,     0,     0,     0,    41,     0,     0,    43,    13,
      14,    15,     0,    16,    46,     0,    47,     0,     0,     0,
       0,     0,    24,    25,    26,    27,    28,    29,    30,    31,
       0,     0,     0,     0,     0,    32,     0,    33,    34,    76,
      36,    37,     0,    38,    39,     0,     0,     0,     0,     0,
       0,     0,    40,     0,     0,     0,    41,     3,     4,    43,
       0,    44,    45,     0,     0,    46,     0,    47,     0,     0,
      13,    14,    15,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    24,    25,    26,    27,    28,    29,    30,
      31,     0,     0,     0,     0,     0,    32,     0,    33,    34,
      76,    36,    37,     0,    38,    39,    96,     0,     3,     4,
       0,     0,     0,    40,     0,     0,     0,    41,     0,     0,
      43,    13,    14,    15,     0,     0,    46,     0,    47,     0,
       0,     0,     0,     0,    24,    25,    26,    27,    28,    29,
      30,    31,     0,     0,     0,     0,     0,    32,     0,    33,
      34,    76,    36,    37,     0,    38,    39,   105,   106,   107,
       0,     0,     0,     0,    40,     0,     0,     0,    41,     0,
       0,    43,     0,     0,     0,     0,     0,    46,     0,    47,
       0,   108,   109,   110,   111,   112,     0,   113,   105,   106,
     107,     0,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,     0,     0,     0,     0,     0,   124,     0,     0,
       0,     0,   108,   109,   110,   111,   112,   218,   113,   105,
     106,   107,     0,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   105,   106,   107,     0,     0,   124,     0,
       0,     0,     0,   108,   109,   110,   111,   112,   245,   113,
       0,     0,     0,     0,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   105,   106,   107,     0,   114,   124,
     116,   117,   118,   119,   120,   121,   207,     0,     0,     0,
       0,     0,     0,   124,     0,     0,     0,     0,   108,   109,
     110,   111,   112,     0,   113,     0,     0,     0,     0,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   105,
     106,   107,     0,     0,   124,     0,    16,     0,     0,     0,
       0,   260,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   108,   109,   110,   111,   112,     0,   113,
       0,     0,     0,     0,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   105,   106,   107,     0,     0,   124,
       0,     0,     0,     0,    44,    45,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   105,   106,   107,   108,   109,
     110,   111,   112,     0,   113,     0,     0,     0,     0,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   108,
     109,   110,   111,   112,   124,   113,     0,     0,   195,     0,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     105,   106,   107,     0,     0,   124,     0,     0,     0,   196,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   105,   106,   107,   108,   109,   110,   111,   112,     0,
     113,     0,     0,     0,     0,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   108,   109,   110,   111,   112,
     124,   113,     0,     0,   197,     0,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   105,   106,   107,     0,
       0,   124,     0,     0,     0,   214,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   105,   106,   107,
     108,   109,   110,   111,   112,     0,   113,     0,     0,     0,
       0,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   108,   109,   110,   111,   112,   124,   113,     0,     0,
     284,     0,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   219,   105,   106,   107,     0,   124,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   105,   106,   107,   108,   109,   110,
     111,   112,     0,   113,     0,     0,     0,     0,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   108,   109,
     110,   111,   112,   124,   113,   105,   106,   107,     0,   114,
       0,   116,   117,   118,   119,   120,   121,     0,   123,     0,
       0,     0,     0,     0,   124,     0,   105,   106,   107,   108,
     109,   110,   111,   112,     0,   113,     0,     0,     0,     0,
     114,     0,   116,   117,   118,   119,   120,   121,     0,     0,
     108,   109,   110,   111,     0,   124,   113,   105,   106,   107,
       0,   114,     0,   116,   117,   118,   119,   120,   121,     0,
       0,     0,     0,     0,     0,     0,   124,     0,   105,   106,
     107,   108,   109,     0,     0,     0,     0,   113,     0,     0,
       0,     0,   114,     0,   116,   117,   118,   119,   120,   121,
       0,     0,   108,   105,   106,   107,     0,   124,   113,     0,
       0,     0,     0,   114,     0,   116,   117,   118,   119,   120,
     121,   105,     0,   107,     0,     0,     0,     0,   124,     0,
       0,     0,     0,   113,     0,     0,     0,     0,   114,     0,
     116,   117,   118,   119,   120,   121,     0,     0,     0,     0,
       0,     0,     0,   124,     0,     0,   114,     0,   116,   117,
     118,   119,   120,   121,     0,     0,     0,     0,     0,     0,
       0,   124
};

static const yytype_int16 yycheck[] =
{
       0,     0,     4,    60,    22,     1,    50,    22,    47,    48,
      68,   257,     8,   259,    67,    47,    48,    17,    17,    72,
      77,    65,    22,    48,    69,    21,    22,    23,    73,    50,
      69,    69,    32,   279,    72,    56,    48,    37,    38,    39,
      40,    41,     2,    43,    65,    69,    68,    48,    72,    15,
      16,    17,    70,    71,    72,    70,    71,    53,    69,    69,
      69,    48,    73,    73,    73,    61,    64,    65,    68,    69,
      70,    67,    74,    39,    40,    41,    42,    43,    69,    45,
      68,    72,    78,    83,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    94,     0,    96,    69,    58,    65,
      73,    73,    75,    69,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,    15,   115,   116,   117,   118,   119,
     120,   121,   122,   123,    84,    69,    69,    68,    72,    72,
      68,    68,    47,    50,    66,    95,    68,    54,    55,    56,
      57,    68,    68,   103,    15,    16,    17,    68,    65,    50,
      68,    52,    53,    54,    55,    56,    57,    72,    73,    74,
     160,    68,   162,   163,    65,    68,   126,    66,    39,    40,
      41,    86,    87,    88,    45,    66,    68,    68,    68,    50,
      47,    52,    53,    54,    55,    56,    57,    24,    48,    48,
      48,    72,    69,    50,    65,   191,   192,   193,    68,   199,
     199,   201,   201,    72,    48,    68,    70,   207,    15,    16,
      17,    72,    47,    69,   129,    69,    69,    72,    69,   219,
      67,    69,    60,    69,   220,    68,    68,   223,    69,   144,
      70,    27,    39,    40,    41,    42,    43,    69,    45,    58,
      58,    69,   242,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    69,   254,   254,   252,    69,    65,    49,
     260,   221,   262,    66,   291,   265,   213,   264,    -1,    -1,
      77,    -1,    -1,    -1,     6,    -1,    -1,    -1,    -1,   275,
      -1,   281,   281,   198,    -1,   200,    18,    19,    -1,   204,
     205,   206,   292,    -1,    -1,    -1,    -1,    -1,    -1,    31,
      32,    33,    34,    35,    36,    37,    38,   222,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    47,    48,    49,    -1,    -1,
      -1,    -1,   237,    -1,    -1,    -1,    -1,    -1,    -1,   244,
      -1,    -1,    -1,    -1,    -1,     1,    68,     3,   253,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    18,    19,    20,    -1,    22,    23,    24,    25,
      26,    -1,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    -1,    -1,    -1,    -1,    -1,    44,    -1,
      46,    47,    48,    49,    50,    -1,    52,    53,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,    -1,    65,
      66,    67,    68,    -1,    70,    71,    -1,     1,    74,     3,
      76,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    18,    19,    20,    -1,    22,    23,
      24,    25,    26,    -1,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    -1,    -1,    -1,    -1,    -1,
      44,    -1,    46,    47,    48,    49,    50,    -1,    52,    53,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,
      -1,    65,    66,    67,    68,    -1,    70,    71,    -1,     1,
      74,     3,    76,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    18,    19,    20,    -1,
      22,    23,    24,    25,    26,    -1,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    -1,    -1,
      -1,    -1,    44,    -1,    46,    47,    48,    49,    50,    -1,
      52,    53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      -1,    -1,    -1,    65,    66,    -1,    68,    -1,    70,    71,
      -1,    -1,    74,     3,    76,     5,     6,    50,    -1,    52,
      53,    54,    55,    56,    57,    -1,    -1,    -1,    18,    19,
      20,    -1,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    31,    32,    33,    34,    35,    36,    37,    38,    -1,
      -1,    -1,    -1,    -1,    44,    -1,    46,    47,    48,    49,
      50,    -1,    52,    53,    54,    -1,     5,     6,    -1,    -1,
      -1,    61,    -1,    -1,    -1,    65,    -1,    -1,    68,    18,
      19,    20,    -1,    22,    74,    -1,    76,    -1,    -1,    -1,
      -1,    -1,    31,    32,    33,    34,    35,    36,    37,    38,
      -1,    -1,    -1,    -1,    -1,    44,    -1,    46,    47,    48,
      49,    50,    -1,    52,    53,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    61,    -1,    -1,    -1,    65,     5,     6,    68,
      -1,    70,    71,    -1,    -1,    74,    -1,    76,    -1,    -1,
      18,    19,    20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    32,    33,    34,    35,    36,    37,
      38,    -1,    -1,    -1,    -1,    -1,    44,    -1,    46,    47,
      48,    49,    50,    -1,    52,    53,    54,    -1,     5,     6,
      -1,    -1,    -1,    61,    -1,    -1,    -1,    65,    -1,    -1,
      68,    18,    19,    20,    -1,    -1,    74,    -1,    76,    -1,
      -1,    -1,    -1,    -1,    31,    32,    33,    34,    35,    36,
      37,    38,    -1,    -1,    -1,    -1,    -1,    44,    -1,    46,
      47,    48,    49,    50,    -1,    52,    53,    15,    16,    17,
      -1,    -1,    -1,    -1,    61,    -1,    -1,    -1,    65,    -1,
      -1,    68,    -1,    -1,    -1,    -1,    -1,    74,    -1,    76,
      -1,    39,    40,    41,    42,    43,    -1,    45,    15,    16,
      17,    -1,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    65,    -1,    -1,
      -1,    -1,    39,    40,    41,    42,    43,    75,    45,    15,
      16,    17,    -1,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    15,    16,    17,    -1,    -1,    65,    -1,
      -1,    -1,    -1,    39,    40,    41,    42,    43,    75,    45,
      -1,    -1,    -1,    -1,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    15,    16,    17,    -1,    50,    65,
      52,    53,    54,    55,    56,    57,    72,    -1,    -1,    -1,
      -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,    39,    40,
      41,    42,    43,    -1,    45,    -1,    -1,    -1,    -1,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    15,
      16,    17,    -1,    -1,    65,    -1,    22,    -1,    -1,    -1,
      -1,    72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    39,    40,    41,    42,    43,    -1,    45,
      -1,    -1,    -1,    -1,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    15,    16,    17,    -1,    -1,    65,
      -1,    -1,    -1,    -1,    70,    71,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    16,    17,    39,    40,
      41,    42,    43,    -1,    45,    -1,    -1,    -1,    -1,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    39,
      40,    41,    42,    43,    65,    45,    -1,    -1,    69,    -1,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      15,    16,    17,    -1,    -1,    65,    -1,    -1,    -1,    69,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    15,    16,    17,    39,    40,    41,    42,    43,    -1,
      45,    -1,    -1,    -1,    -1,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    39,    40,    41,    42,    43,
      65,    45,    -1,    -1,    69,    -1,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    15,    16,    17,    -1,
      -1,    65,    -1,    -1,    -1,    69,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,    16,    17,
      39,    40,    41,    42,    43,    -1,    45,    -1,    -1,    -1,
      -1,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    39,    40,    41,    42,    43,    65,    45,    -1,    -1,
      69,    -1,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    15,    16,    17,    -1,    65,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    15,    16,    17,    39,    40,    41,
      42,    43,    -1,    45,    -1,    -1,    -1,    -1,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    39,    40,
      41,    42,    43,    65,    45,    15,    16,    17,    -1,    50,
      -1,    52,    53,    54,    55,    56,    57,    -1,    59,    -1,
      -1,    -1,    -1,    -1,    65,    -1,    15,    16,    17,    39,
      40,    41,    42,    43,    -1,    45,    -1,    -1,    -1,    -1,
      50,    -1,    52,    53,    54,    55,    56,    57,    -1,    -1,
      39,    40,    41,    42,    -1,    65,    45,    15,    16,    17,
      -1,    50,    -1,    52,    53,    54,    55,    56,    57,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    65,    -1,    15,    16,
      17,    39,    40,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      -1,    -1,    50,    -1,    52,    53,    54,    55,    56,    57,
      -1,    -1,    39,    15,    16,    17,    -1,    65,    45,    -1,
      -1,    -1,    -1,    50,    -1,    52,    53,    54,    55,    56,
      57,    15,    -1,    17,    -1,    -1,    -1,    -1,    65,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    -1,    -1,    50,    -1,
      52,    53,    54,    55,    56,    57,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    65,    -1,    -1,    50,    -1,    52,    53,
      54,    55,    56,    57,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    65
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     3,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    18,    19,    20,    22,    23,    24,    25,
      26,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    44,    46,    47,    48,    49,    50,    52,    53,
      61,    65,    66,    68,    70,    71,    74,    76,    79,    80,
      90,    92,   102,   103,    90,    47,    48,   100,    68,    48,
      48,    48,    90,    81,    83,    85,    48,    94,    68,    68,
      68,    80,    68,    68,    68,    90,    48,    90,   103,    90,
      68,    68,    68,    68,    68,   103,    68,    66,    68,   103,
     103,   103,   103,   103,    96,     3,    54,   103,    97,    98,
     103,    97,     0,     4,    74,    15,    16,    17,    39,    40,
      41,    42,    43,    45,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    65,    90,    66,   100,    87,    68,
      90,    47,    95,    95,    95,    72,    90,   103,   103,   103,
      24,    97,    97,    97,    68,    90,    48,    48,    48,   103,
      69,   100,    97,    97,    97,    67,    80,   100,   103,    69,
      73,    75,    72,    60,    77,   100,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   103,    93,   100,    69,    68,    97,
      72,    82,    84,    86,    48,    69,    69,    69,    68,    69,
      70,    69,    97,    69,    72,    72,    72,    72,    69,    69,
      67,    69,    73,    69,    69,   103,   103,   103,    75,    60,
      67,    72,    88,    69,    47,    90,    90,    90,    97,    80,
      97,    80,    69,    97,    97,    97,   103,    68,    60,    99,
     100,   101,    68,   102,    68,    75,    77,   103,    90,   100,
      97,    90,    69,    70,    27,    69,    69,    73,    69,    73,
      72,    97,    58,    69,    72,    58,    97,    89,    90,    97,
      80,    99,    99,   103,    69,   103,   101,   103,    69,    73,
      69,    69,    69,    69,    69,    90,    99,    49,    91,    80,
      69,    66,    96,    67
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    78,    79,    80,    81,    82,    80,    83,    84,    80,
      85,    86,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    87,    88,
      89,    80,    80,    80,    80,    90,    90,    90,    91,    91,
      92,    93,    93,    94,    94,    95,    95,    96,    96,    97,
      97,    98,    98,    99,    99,   100,   100,   101,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     0,     0,     5,     0,     0,     5,
       0,     0,     5,     3,     6,     2,     1,     2,     2,     2,
       3,     5,     7,     9,     5,     7,     8,     6,     0,     0,
       0,    12,     3,     2,     2,     1,     1,     1,     0,     1,
       3,     1,     3,     1,     3,     1,     3,     0,     2,     0,
       1,     1,     3,     1,     3,     1,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     4,     6,     6,     8,     6,
       8,     7,     4,     3,     8,     4,     6,     7,     9,     4,
       4,     4,     3,     1,     4,     3,     1,     5,     3,     3,
       2,     2,     3,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     3,     3,     2,     3,     3,     3,
       3,     3,     3,     5,     3,     5,     4,     4,     5,     2
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

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

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


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
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
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
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
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
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

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
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
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
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
  case 2: /* start: stat  */
#line 131 "parse/parse.y"
                        { 
				parse_snode = (yyvsp[0].s); 
				if ( yychar >= 0 ) 
					fprintf(stderr,
						"Warning: a token was wasted after an 'if' statement without 'else'.\n");
				YYACCEPT; 
			}
#line 1614 "parse/y.tab.c"
    break;

  case 3: /* stat: tail  */
#line 140 "parse/parse.y"
                        { (yyval.s) = 0; }
#line 1620 "parse/y.tab.c"
    break;

  case 4: /* $@1: %empty  */
#line 141 "parse/parse.y"
                         { gdef=1; }
#line 1626 "parse/y.tab.c"
    break;

  case 5: /* $@2: %empty  */
#line 141 "parse/parse.y"
                                           { gdef=0; }
#line 1632 "parse/y.tab.c"
    break;

  case 6: /* stat: GLOBAL $@1 pvars $@2 tail  */
#line 142 "parse/parse.y"
                        { (yyval.s) = 0; NOPR; }
#line 1638 "parse/y.tab.c"
    break;

  case 7: /* $@3: %empty  */
#line 143 "parse/parse.y"
                          { mgdef=1; }
#line 1644 "parse/y.tab.c"
    break;

  case 8: /* $@4: %empty  */
#line 143 "parse/parse.y"
                                             { mgdef=0; }
#line 1650 "parse/y.tab.c"
    break;

  case 9: /* stat: MGLOBAL $@3 pvars $@4 tail  */
#line 144 "parse/parse.y"
                        { (yyval.s) = 0; NOPR; }
#line 1656 "parse/y.tab.c"
    break;

  case 10: /* $@5: %empty  */
#line 145 "parse/parse.y"
                        { ldef=1; }
#line 1662 "parse/y.tab.c"
    break;

  case 11: /* $@6: %empty  */
#line 145 "parse/parse.y"
                                          { ldef=0; }
#line 1668 "parse/y.tab.c"
    break;

  case 12: /* stat: LOCAL $@5 pvars $@6 tail  */
#line 146 "parse/parse.y"
                        { (yyval.s) = 0; NOPR; }
#line 1674 "parse/y.tab.c"
    break;

  case 13: /* stat: LOCALF vars tail  */
#line 148 "parse/parse.y"
                        { appenduflist((yyvsp[-1].n)); (yyval.s) = 0; NOPR; }
#line 1680 "parse/y.tab.c"
    break;

  case 14: /* stat: STRUCT rawstr '{' members '}' tail  */
#line 150 "parse/parse.y"
                        { structdef((yyvsp[-4].p),(yyvsp[-2].n)); (yyval.s) = 0; NOPR; }
#line 1686 "parse/y.tab.c"
    break;

  case 15: /* stat: expr tail  */
#line 152 "parse/parse.y"
                        { (yyval.s) = mksnode(1,S_SINGLE,(yyvsp[-1].f)); }
#line 1692 "parse/y.tab.c"
    break;

  case 16: /* stat: complex  */
#line 154 "parse/parse.y"
                        { (yyval.s) = (yyvsp[0].s); }
#line 1698 "parse/y.tab.c"
    break;

  case 17: /* stat: BREAK tail  */
#line 156 "parse/parse.y"
                        { (yyval.s) = mksnode(0,S_BREAK); }
#line 1704 "parse/y.tab.c"
    break;

  case 18: /* stat: CONTINUE tail  */
#line 158 "parse/parse.y"
                        { (yyval.s) = mksnode(0,S_CONTINUE); }
#line 1710 "parse/y.tab.c"
    break;

  case 19: /* stat: RETURN tail  */
#line 160 "parse/parse.y"
                        { (yyval.s) = mksnode(1,S_RETURN,NULLP); }
#line 1716 "parse/y.tab.c"
    break;

  case 20: /* stat: RETURN expr tail  */
#line 162 "parse/parse.y"
                        { (yyval.s) = mksnode(1,S_RETURN,(yyvsp[-1].f)); }
#line 1722 "parse/y.tab.c"
    break;

  case 21: /* stat: IF '(' node ')' stat  */
#line 164 "parse/parse.y"
                        { (yyval.s) = mksnode(4,S_IFELSE,(yyvsp[-4].i),(yyvsp[-2].n),(yyvsp[0].s),NULLP); (yyvsp[0].s)?(yyval.s)->ln=(yyvsp[0].s)->ln:0; NOPR; }
#line 1728 "parse/y.tab.c"
    break;

  case 22: /* stat: IF '(' node ')' stat ELSE stat  */
#line 166 "parse/parse.y"
                        { (yyval.s) = mksnode(4,S_IFELSE,(yyvsp[-6].i),(yyvsp[-4].n),(yyvsp[-2].s),(yyvsp[0].s)); (yyvsp[0].s)?(yyval.s)->ln=(yyvsp[0].s)->ln:0; NOPR; }
#line 1734 "parse/y.tab.c"
    break;

  case 23: /* stat: FOR '(' node ';' node ';' node ')' stat  */
#line 168 "parse/parse.y"
                        { (yyval.s) = mksnode(5,S_FOR,(yyvsp[-8].i),(yyvsp[-6].n),(yyvsp[-4].n)?(yyvsp[-4].n):ONENODE,(yyvsp[-2].n),(yyvsp[0].s)); (yyvsp[0].s)?(yyval.s)->ln=(yyvsp[0].s)->ln:0; NOPR; }
#line 1740 "parse/y.tab.c"
    break;

  case 24: /* stat: WHILE '(' node ')' stat  */
#line 170 "parse/parse.y"
                        { (yyval.s) = mksnode(5,S_FOR,(yyvsp[-4].i),NULLP,(yyvsp[-2].n),NULLP,(yyvsp[0].s)); (yyvsp[0].s)?(yyval.s)->ln=(yyvsp[0].s)->ln:0; NOPR; }
#line 1746 "parse/y.tab.c"
    break;

  case 25: /* stat: DO stat WHILE '(' node ')' tail  */
#line 172 "parse/parse.y"
                        { (yyval.s) = mksnode(3,S_DO,(yyvsp[-6].i),(yyvsp[-5].s),(yyvsp[-2].n)); NOPR; }
#line 1752 "parse/y.tab.c"
    break;

  case 26: /* stat: LCASE '(' node ')' ':' '=' expr tail  */
#line 174 "parse/parse.y"
                        { (yyval.s) = mksnode(3,S_PFDEF,(yyvsp[-7].p),(yyvsp[-5].n),(yyvsp[-1].f)); NOPR; }
#line 1758 "parse/y.tab.c"
    break;

  case 27: /* stat: PFDEF LCASE '(' node ')' tail  */
#line 176 "parse/parse.y"
                        { (yyval.s) = mksnode(3,S_PFDEF,(yyvsp[-4].p),(yyvsp[-2].n),NULLP); NOPR; }
#line 1764 "parse/y.tab.c"
    break;

  case 28: /* $@7: %empty  */
#line 177 "parse/parse.y"
                             { mkpvs((yyvsp[0].p)); }
#line 1770 "parse/y.tab.c"
    break;

  case 29: /* $@8: %empty  */
#line 177 "parse/parse.y"
                                                { ldef = 1; }
#line 1776 "parse/y.tab.c"
    break;

  case 30: /* $@9: %empty  */
#line 177 "parse/parse.y"
                                                                   { ldef = -1; }
#line 1782 "parse/y.tab.c"
    break;

  case 31: /* stat: FDEF LCASE $@7 '(' $@8 node $@9 ')' desc '{' stats '}'  */
#line 178 "parse/parse.y"
                        {
				mkuf((yyvsp[-10].p),asir_infile->name,(yyvsp[-6].n),
					mksnode(1,S_CPLX,(yyvsp[-1].n)),(yyvsp[-11].i),asir_infile->ln,(yyvsp[-3].p),CUR_MODULE); 
				(yyval.s) = 0; NOPR;
			}
#line 1792 "parse/y.tab.c"
    break;

  case 32: /* stat: MODDEF LCASE tail  */
#line 184 "parse/parse.y"
                        { 
				CUR_MODULE = mkmodule((yyvsp[-1].p));
				MPVS = CUR_MODULE->pvs; 
				(yyval.s) = mksnode(1,S_MODULE,CUR_MODULE); NOPR;
			}
#line 1802 "parse/y.tab.c"
    break;

  case 33: /* stat: MODEND tail  */
#line 190 "parse/parse.y"
                        { CUR_MODULE = 0; MPVS = 0; (yyval.s) = mksnode(1,S_MODULE,NULLP); NOPR; }
#line 1808 "parse/y.tab.c"
    break;

  case 34: /* stat: error tail  */
#line 192 "parse/parse.y"
                        { yyerrok; (yyval.s) = 0; }
#line 1814 "parse/y.tab.c"
    break;

  case 35: /* tail: ';'  */
#line 195 "parse/parse.y"
                        { if ( main_parser ) prresult = 1; }
#line 1820 "parse/y.tab.c"
    break;

  case 36: /* tail: '$'  */
#line 197 "parse/parse.y"
                        { if ( main_parser ) prresult = 0; }
#line 1826 "parse/y.tab.c"
    break;

  case 37: /* tail: SEMISEMI  */
#line 199 "parse/parse.y"
                        { if ( main_parser ) prresult = 0; }
#line 1832 "parse/y.tab.c"
    break;

  case 38: /* desc: %empty  */
#line 202 "parse/parse.y"
                        { (yyval.p) = 0; }
#line 1838 "parse/y.tab.c"
    break;

  case 39: /* desc: STR  */
#line 204 "parse/parse.y"
                        { (yyval.p) = (yyvsp[0].p); }
#line 1844 "parse/y.tab.c"
    break;

  case 40: /* complex: '{' stats '}'  */
#line 207 "parse/parse.y"
                        { (yyval.s) = mksnode(1,S_CPLX,(yyvsp[-1].n)); }
#line 1850 "parse/y.tab.c"
    break;

  case 41: /* members: rawstr  */
#line 210 "parse/parse.y"
                        { MKNODE((yyval.n),(yyvsp[0].p),0); }
#line 1856 "parse/y.tab.c"
    break;

  case 42: /* members: members ',' rawstr  */
#line 212 "parse/parse.y"
                        { appendtonode((yyvsp[-2].n),(yyvsp[0].p),&(yyval.n)); }
#line 1862 "parse/y.tab.c"
    break;

  case 43: /* vars: LCASE  */
#line 215 "parse/parse.y"
                        { MKNODE((yyval.n),(yyvsp[0].p),0); }
#line 1868 "parse/y.tab.c"
    break;

  case 44: /* vars: vars ',' LCASE  */
#line 217 "parse/parse.y"
                        { appendtonode((yyvsp[-2].n),(yyvsp[0].p),&(yyval.n)); }
#line 1874 "parse/y.tab.c"
    break;

  case 45: /* pvars: UCASE  */
#line 220 "parse/parse.y"
                        { val = (pointer)((long)makepvar((yyvsp[0].p))); MKNODE((yyval.n),val,0); }
#line 1880 "parse/y.tab.c"
    break;

  case 46: /* pvars: pvars ',' UCASE  */
#line 222 "parse/parse.y"
                        { appendtonode((yyvsp[-2].n),(pointer)((long)makepvar((yyvsp[0].p))),&(yyval.n)); }
#line 1886 "parse/y.tab.c"
    break;

  case 47: /* stats: %empty  */
#line 225 "parse/parse.y"
                        { (yyval.n) = 0; }
#line 1892 "parse/y.tab.c"
    break;

  case 48: /* stats: stats stat  */
#line 227 "parse/parse.y"
                        { appendtonode((yyvsp[-1].n),(pointer)(yyvsp[0].s),&(yyval.n)); }
#line 1898 "parse/y.tab.c"
    break;

  case 49: /* node: %empty  */
#line 230 "parse/parse.y"
                        { (yyval.n) = 0; }
#line 1904 "parse/y.tab.c"
    break;

  case 50: /* node: _node  */
#line 232 "parse/parse.y"
                        { (yyval.n) = (yyvsp[0].n); }
#line 1910 "parse/y.tab.c"
    break;

  case 51: /* _node: expr  */
#line 235 "parse/parse.y"
                        { MKNODE((yyval.n),(yyvsp[0].f),0); }
#line 1916 "parse/y.tab.c"
    break;

  case 52: /* _node: _node ',' expr  */
#line 237 "parse/parse.y"
                        { appendtonode((yyvsp[-2].n),(pointer)(yyvsp[0].f),&(yyval.n)); }
#line 1922 "parse/y.tab.c"
    break;

  case 53: /* optlist: opt  */
#line 240 "parse/parse.y"
                        { MKNODE((yyval.n),(yyvsp[0].f),0); }
#line 1928 "parse/y.tab.c"
    break;

  case 54: /* optlist: optlist ',' opt  */
#line 242 "parse/parse.y"
                        { appendtonode((yyvsp[-2].n),(pointer)(yyvsp[0].f),&(yyval.n)); }
#line 1934 "parse/y.tab.c"
    break;

  case 55: /* rawstr: UCASE  */
#line 245 "parse/parse.y"
                        { (yyval.p) = (yyvsp[0].p); }
#line 1940 "parse/y.tab.c"
    break;

  case 56: /* rawstr: LCASE  */
#line 247 "parse/parse.y"
                        { (yyval.p) = (yyvsp[0].p); }
#line 1946 "parse/y.tab.c"
    break;

  case 57: /* opt: rawstr '=' expr  */
#line 250 "parse/parse.y"
                        { (yyval.f) = mkfnode(2,I_OPT,(yyvsp[-2].p),(yyvsp[0].f)); }
#line 1952 "parse/y.tab.c"
    break;

  case 58: /* pexpr: STR  */
#line 253 "parse/parse.y"
                        { (yyval.f) = mkfnode(1,I_STR,(yyvsp[0].p)); }
#line 1958 "parse/y.tab.c"
    break;

  case 59: /* pexpr: FORMULA  */
#line 255 "parse/parse.y"
                        { (yyval.f) = mkfnode(1,I_FORMULA,(yyvsp[0].p)); }
#line 1964 "parse/y.tab.c"
    break;

  case 60: /* pexpr: ANS  */
#line 257 "parse/parse.y"
                        { (yyval.f) = mkfnode(1,I_ANS,(yyvsp[0].i)); }
#line 1970 "parse/y.tab.c"
    break;

  case 61: /* pexpr: GF2NGEN  */
#line 259 "parse/parse.y"
                        { (yyval.f) = mkfnode(0,I_GF2NGEN); }
#line 1976 "parse/y.tab.c"
    break;

  case 62: /* pexpr: GFPNGEN  */
#line 261 "parse/parse.y"
                        { (yyval.f) = mkfnode(0,I_GFPNGEN); }
#line 1982 "parse/y.tab.c"
    break;

  case 63: /* pexpr: GFSNGEN  */
#line 263 "parse/parse.y"
                        { (yyval.f) = mkfnode(0,I_GFSNGEN); }
#line 1988 "parse/y.tab.c"
    break;

  case 64: /* pexpr: LCASE  */
#line 265 "parse/parse.y"
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
#line 2013 "parse/y.tab.c"
    break;

  case 65: /* pexpr: LCASE '(' node ')'  */
#line 286 "parse/parse.y"
                        {
				gen_searchf((yyvsp[-3].p),(FUNC *)&val);
				print_crossref(val);
				(yyval.f) = mkfnode(2,I_FUNC,val,mkfnode(1,I_LIST,(yyvsp[-1].n)));
			}
#line 2023 "parse/y.tab.c"
    break;

  case 66: /* pexpr: LCASE '(' node '|' optlist ')'  */
#line 292 "parse/parse.y"
                        {
				gen_searchf((yyvsp[-5].p),(FUNC *)&val);
				print_crossref(val);
				(yyval.f) = mkfnode(3,I_FUNC_OPT,val,
					mkfnode(1,I_LIST,(yyvsp[-3].n)),mkfnode(1,I_LIST,(yyvsp[-1].n)));
			}
#line 2034 "parse/y.tab.c"
    break;

  case 67: /* pexpr: MAP '(' LCASE ',' node ')'  */
#line 299 "parse/parse.y"
                        {
				gen_searchf((yyvsp[-3].p),(FUNC *)&val);
				print_crossref(val);
				(yyval.f) = mkfnode(3,I_MAP,val,mkfnode(1,I_LIST,(yyvsp[-1].n)),0);
			}
#line 2044 "parse/y.tab.c"
    break;

  case 68: /* pexpr: MAP '(' LCASE ',' node '|' optlist ')'  */
#line 305 "parse/parse.y"
                        {
				gen_searchf((yyvsp[-5].p),(FUNC *)&val);
				print_crossref(val);
				(yyval.f) = mkfnode(3,I_MAP,val,mkfnode(1,I_LIST,(yyvsp[-3].n)),mkfnode(1,I_LIST,(yyvsp[-1].n)));
			}
#line 2054 "parse/y.tab.c"
    break;

  case 69: /* pexpr: RECMAP '(' LCASE ',' node ')'  */
#line 311 "parse/parse.y"
                        {
				gen_searchf((yyvsp[-3].p),(FUNC *)&val);
				print_crossref(val);
				(yyval.f) = mkfnode(3,I_RECMAP,val,mkfnode(1,I_LIST,(yyvsp[-1].n)),0);
			}
#line 2064 "parse/y.tab.c"
    break;

  case 70: /* pexpr: RECMAP '(' LCASE ',' node '|' optlist ')'  */
#line 317 "parse/parse.y"
                        {
				gen_searchf((yyvsp[-5].p),(FUNC *)&val);
				print_crossref(val);
				(yyval.f) = mkfnode(3,I_RECMAP,val,mkfnode(1,I_LIST,(yyvsp[-3].n)),mkfnode(1,I_LIST,(yyvsp[-1].n)));
			}
#line 2074 "parse/y.tab.c"
    break;

  case 71: /* pexpr: LCASE '{' node '}' '(' node ')'  */
#line 323 "parse/parse.y"
                        {
				searchpf((yyvsp[-6].p),(FUNC *)&val);
				if ( val == 0 ) {
					fprintf(stderr,"%s : no such function.\n",(char *)(yyvsp[-6].p));
                    YYABORT;
                }
				(yyval.f) = mkfnode(3,I_PFDERIV,val,mkfnode(1,I_LIST,(yyvsp[-1].n)),mkfnode(1,I_LIST,(yyvsp[-4].n)));
			}
#line 2087 "parse/y.tab.c"
    break;

  case 72: /* pexpr: GETOPT '(' rawstr ')'  */
#line 332 "parse/parse.y"
                        {
				(yyval.f) = mkfnode(2,I_GETOPT,(yyvsp[-1].p));
			}
#line 2095 "parse/y.tab.c"
    break;

  case 73: /* pexpr: GETOPT '(' ')'  */
#line 336 "parse/parse.y"
                        {
				(yyval.f) = mkfnode(2,I_GETOPT,NULLP);
			}
#line 2103 "parse/y.tab.c"
    break;

  case 74: /* pexpr: TIMER '(' expr ',' expr ',' expr ')'  */
#line 340 "parse/parse.y"
                        {
				(yyval.f) = mkfnode(3,I_TIMER,(yyvsp[-5].f),(yyvsp[-3].f),(yyvsp[-1].f));
			}
#line 2111 "parse/y.tab.c"
    break;

  case 75: /* pexpr: PARIF '(' LCASE ')'  */
#line 344 "parse/parse.y"
                        {
				searchf(parif,(yyvsp[-1].p),(FUNC *)&val);
				if ( !val )
					mkparif((yyvsp[-1].p),(FUNC *)&val);
				(yyval.f) = mkfnode(2,I_FUNC,val,NULLP);
			}
#line 2122 "parse/y.tab.c"
    break;

  case 76: /* pexpr: PARIF '(' LCASE ',' node ')'  */
#line 351 "parse/parse.y"
                        {
				searchf(parif,(yyvsp[-3].p),(FUNC *)&val);
				if ( !val )
					mkparif((yyvsp[-3].p),(FUNC *)&val);
				(yyval.f) = mkfnode(2,I_FUNC,val,mkfnode(1,I_LIST,(yyvsp[-1].n)));
			}
#line 2133 "parse/y.tab.c"
    break;

  case 77: /* pexpr: '(' '*' expr ')' '(' node ')'  */
#line 358 "parse/parse.y"
                        {
				(yyval.f) = mkfnode(2,I_IFUNC,(yyvsp[-4].f),mkfnode(1,I_LIST,(yyvsp[-1].n)),NULLP);
			}
#line 2141 "parse/y.tab.c"
    break;

  case 78: /* pexpr: '(' '*' expr ')' '(' node '|' optlist ')'  */
#line 362 "parse/parse.y"
                        {
				(yyval.f) = mkfnode(3,I_IFUNC,(yyvsp[-6].f),mkfnode(1,I_LIST,(yyvsp[-3].n)),
					mkfnode(1,I_LIST,(yyvsp[-1].n)));
			}
#line 2150 "parse/y.tab.c"
    break;

  case 79: /* pexpr: UCASE '(' node ')'  */
#line 367 "parse/parse.y"
                        {
				if ( main_parser || allow_create_var )
					t = mkfnode(2,I_PVAR,makepvar((yyvsp[-3].p)),NULLP);
				else {
					ind = searchpvar((yyvsp[-3].p));
					if ( ind == -1 ) {
						fprintf(stderr,"%s : no such variable.\n",(char *)(yyvsp[-3].p));
						YYABORT;
					} else
						t = mkfnode(2,I_PVAR,ind,NULLP);
				}
				(yyval.f) = mkfnode(2,I_IFUNC,t,mkfnode(1,I_LIST,(yyvsp[-1].n)));
			}
#line 2168 "parse/y.tab.c"
    break;

  case 80: /* pexpr: CAR '(' expr ')'  */
#line 381 "parse/parse.y"
                        { (yyval.f) = mkfnode(1,I_CAR,(yyvsp[-1].f)); }
#line 2174 "parse/y.tab.c"
    break;

  case 81: /* pexpr: CDR '(' expr ')'  */
#line 383 "parse/parse.y"
                        { (yyval.f) = mkfnode(1,I_CDR,(yyvsp[-1].f)); }
#line 2180 "parse/y.tab.c"
    break;

  case 82: /* pexpr: '(' expr ')'  */
#line 385 "parse/parse.y"
                        { (yyval.f) = mkfnode(1,I_PAREN,(yyvsp[-1].f)); }
#line 2186 "parse/y.tab.c"
    break;

  case 83: /* pexpr: UCASE  */
#line 387 "parse/parse.y"
                        { 
				if ( main_parser || allow_create_var )
					(yyval.f) = mkfnode(2,I_PVAR,makepvar((yyvsp[0].p)),NULLP);
				else {
					ind = searchpvar((yyvsp[0].p));
					if ( ind == -1 ) {
						fprintf(stderr,"%s : no such variable.\n",(char *)(yyvsp[0].p));
						YYABORT;
					} else
						(yyval.f) = mkfnode(2,I_PVAR,ind,NULLP);
				}
			}
#line 2203 "parse/y.tab.c"
    break;

  case 84: /* pexpr: pexpr '[' expr ']'  */
#line 400 "parse/parse.y"
                        {
				if ( (yyvsp[-3].f)->id == I_PVAR || (yyvsp[-3].f)->id == I_INDEX ) {
					appendtonode((NODE)(yyvsp[-3].f)->arg[1],(pointer)(yyvsp[-1].f),&a);
					(yyvsp[-3].f)->arg[1] = (pointer)a; (yyval.f) = (yyvsp[-3].f);
				} else {
					MKNODE(a,(yyvsp[-1].f),0);
					(yyval.f) = mkfnode(2,I_INDEX,(pointer)(yyvsp[-3].f),a);
				}
			}
#line 2217 "parse/y.tab.c"
    break;

  case 85: /* pexpr: pexpr PNT rawstr  */
#line 410 "parse/parse.y"
                        { (yyval.f) = mkfnode(2,I_POINT,(yyvsp[-2].f),(yyvsp[0].p)); }
#line 2223 "parse/y.tab.c"
    break;

  case 86: /* expr: pexpr  */
#line 413 "parse/parse.y"
                        { (yyval.f) = (yyvsp[0].f); }
#line 2229 "parse/y.tab.c"
    break;

  case 87: /* expr: '(' STRUCT rawstr ')' pexpr  */
#line 415 "parse/parse.y"
                        { (yyval.f) = mkfnode(3,I_CAST,structtoindex((yyvsp[-2].p)),(yyvsp[0].f),NULLP); }
#line 2235 "parse/y.tab.c"
    break;

  case 88: /* expr: expr '=' expr  */
#line 417 "parse/parse.y"
                        { (yyval.f) = mkfnode(2,I_ASSPVAR,(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2241 "parse/y.tab.c"
    break;

  case 89: /* expr: expr BOPASS expr  */
#line 419 "parse/parse.y"
                        { (yyval.f) = mkfnode(2,I_ASSPVAR,(yyvsp[-2].f),mkfnode(3,I_BOP,(yyvsp[-1].p),(yyvsp[-2].f),(yyvsp[0].f))); }
#line 2247 "parse/y.tab.c"
    break;

  case 90: /* expr: expr SELF  */
#line 421 "parse/parse.y"
                        { (yyval.f) = mkfnode(2,I_POSTSELF,(yyvsp[0].p),(yyvsp[-1].f)); }
#line 2253 "parse/y.tab.c"
    break;

  case 91: /* expr: SELF expr  */
#line 423 "parse/parse.y"
                        { (yyval.f) = mkfnode(2,I_PRESELF,(yyvsp[-1].p),(yyvsp[0].f)); }
#line 2259 "parse/y.tab.c"
    break;

  case 92: /* expr: '[' node ']'  */
#line 425 "parse/parse.y"
                        { (yyval.f) = mkfnode(1,I_LIST,(yyvsp[-1].n)); }
#line 2265 "parse/y.tab.c"
    break;

  case 93: /* expr: '+' expr  */
#line 427 "parse/parse.y"
                        { (yyval.f) = (yyvsp[0].f); }
#line 2271 "parse/y.tab.c"
    break;

  case 94: /* expr: '-' expr  */
#line 429 "parse/parse.y"
                        { (yyval.f) = mkfnode(1,I_MINUS,(yyvsp[0].f)); }
#line 2277 "parse/y.tab.c"
    break;

  case 95: /* expr: expr '+' expr  */
#line 431 "parse/parse.y"
                        { (yyval.f) = mkfnode(3,I_BOP,(yyvsp[-1].p),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2283 "parse/y.tab.c"
    break;

  case 96: /* expr: expr '-' expr  */
#line 433 "parse/parse.y"
                        { (yyval.f) = mkfnode(3,I_BOP,(yyvsp[-1].p),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2289 "parse/y.tab.c"
    break;

  case 97: /* expr: expr '*' expr  */
#line 435 "parse/parse.y"
                        { (yyval.f) = mkfnode(3,I_BOP,(yyvsp[-1].p),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2295 "parse/y.tab.c"
    break;

  case 98: /* expr: expr '/' expr  */
#line 437 "parse/parse.y"
                        { (yyval.f) = mkfnode(3,I_BOP,(yyvsp[-1].p),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2301 "parse/y.tab.c"
    break;

  case 99: /* expr: expr '%' expr  */
#line 439 "parse/parse.y"
                        { (yyval.f) = mkfnode(3,I_BOP,(yyvsp[-1].p),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2307 "parse/y.tab.c"
    break;

  case 100: /* expr: expr '^' expr  */
#line 441 "parse/parse.y"
                        { (yyval.f) = mkfnode(3,I_BOP,(yyvsp[-1].p),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2313 "parse/y.tab.c"
    break;

  case 101: /* expr: expr CMP expr  */
#line 443 "parse/parse.y"
                        { (yyval.f) = mkfnode(3,I_COP,(yyvsp[-1].i),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2319 "parse/y.tab.c"
    break;

  case 102: /* expr: '!' expr  */
#line 445 "parse/parse.y"
                        { (yyval.f) = mkfnode(1,I_NOT,(yyvsp[0].f)); }
#line 2325 "parse/y.tab.c"
    break;

  case 103: /* expr: expr '!'  */
#line 447 "parse/parse.y"
                        { 
				gen_searchf("factorial",(FUNC *)&val);
				print_crossref(val);
				MKNODE(a,(yyvsp[-1].f),0);
				(yyval.f) = mkfnode(2,I_FUNC,val,mkfnode(1,I_LIST,a));
			}
#line 2336 "parse/y.tab.c"
    break;

  case 104: /* expr: expr OR expr  */
#line 454 "parse/parse.y"
                        { (yyval.f) = mkfnode(2,I_OR,(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2342 "parse/y.tab.c"
    break;

  case 105: /* expr: expr AND expr  */
#line 456 "parse/parse.y"
                        { (yyval.f) = mkfnode(2,I_AND,(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2348 "parse/y.tab.c"
    break;

  case 106: /* expr: FOP_NOT expr  */
#line 458 "parse/parse.y"
                        { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[-1].i),(yyvsp[0].f),NULLP); }
#line 2354 "parse/y.tab.c"
    break;

  case 107: /* expr: expr FOP_AND expr  */
#line 460 "parse/parse.y"
                        { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[-1].i),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2360 "parse/y.tab.c"
    break;

  case 108: /* expr: expr FOP_OR expr  */
#line 462 "parse/parse.y"
                        { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[-1].i),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2366 "parse/y.tab.c"
    break;

  case 109: /* expr: expr FOP_IMPL expr  */
#line 464 "parse/parse.y"
                        { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[-1].i),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2372 "parse/y.tab.c"
    break;

  case 110: /* expr: expr FOP_REPL expr  */
#line 466 "parse/parse.y"
                        { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[-1].i),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2378 "parse/y.tab.c"
    break;

  case 111: /* expr: expr FOP_EQUIV expr  */
#line 468 "parse/parse.y"
                        { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[-1].i),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2384 "parse/y.tab.c"
    break;

  case 112: /* expr: expr LOP expr  */
#line 470 "parse/parse.y"
                        { (yyval.f) = mkfnode(3,I_LOP,(yyvsp[-1].i),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2390 "parse/y.tab.c"
    break;

  case 113: /* expr: expr '?' expr ':' expr  */
#line 472 "parse/parse.y"
                        { (yyval.f) = mkfnode(3,I_CE,(yyvsp[-4].f),(yyvsp[-2].f),(yyvsp[0].f)); }
#line 2396 "parse/y.tab.c"
    break;

  case 114: /* expr: '<' node '>'  */
#line 474 "parse/parse.y"
                        { (yyval.f) = mkfnode(1,I_EV,(yyvsp[-1].n)); }
#line 2402 "parse/y.tab.c"
    break;

  case 115: /* expr: '<' node ':' expr '>'  */
#line 476 "parse/parse.y"
                        { (yyval.f) = mkfnode(2,I_EVM,(yyvsp[-3].n),(yyvsp[-1].f)); }
#line 2408 "parse/y.tab.c"
    break;

  case 116: /* expr: NEWSTRUCT '(' rawstr ')'  */
#line 478 "parse/parse.y"
                        { (yyval.f) = mkfnode(1,I_NEWCOMP,(int)structtoindex((yyvsp[-1].p))); }
#line 2414 "parse/y.tab.c"
    break;

  case 117: /* expr: QUOTED '(' expr ')'  */
#line 480 "parse/parse.y"
                        { MKQUOTE(quote,(yyvsp[-1].f)); (yyval.f) = mkfnode(1,I_FORMULA,(pointer)quote); }
#line 2420 "parse/y.tab.c"
    break;

  case 118: /* expr: '[' node '|' expr ']'  */
#line 482 "parse/parse.y"
                        { (yyval.f) = mkfnode(2,I_CONS,(yyvsp[-3].n),(yyvsp[-1].f)); }
#line 2426 "parse/y.tab.c"
    break;

  case 119: /* expr: '`' expr  */
#line 484 "parse/parse.y"
                        { MKQUOTE(quote,(yyvsp[0].f)); (yyval.f) = mkfnode(1,I_FORMULA,(pointer)quote); }
#line 2432 "parse/y.tab.c"
    break;


#line 2436 "parse/y.tab.c"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

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
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
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

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

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


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
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
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 486 "parse/parse.y"

