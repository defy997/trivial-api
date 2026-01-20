/* A Bison parser, made by GNU Bison 3.8.2.  */

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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 7 "/app/TrivialDB-master/src/parser/sql.y"

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "execute.h"

void yyerror(const char *s);

#include "sql.yy.c"


#line 83 "/app/TrivialDB-master/src/parser/sql.tab.c"

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

#include "sql.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_TRUE = 3,                       /* TRUE  */
  YYSYMBOL_FALSE = 4,                      /* FALSE  */
  YYSYMBOL_NULL_TOKEN = 5,                 /* NULL_TOKEN  */
  YYSYMBOL_MIN = 6,                        /* MIN  */
  YYSYMBOL_MAX = 7,                        /* MAX  */
  YYSYMBOL_SUM = 8,                        /* SUM  */
  YYSYMBOL_AVG = 9,                        /* AVG  */
  YYSYMBOL_COUNT = 10,                     /* COUNT  */
  YYSYMBOL_LIKE = 11,                      /* LIKE  */
  YYSYMBOL_IS = 12,                        /* IS  */
  YYSYMBOL_OR = 13,                        /* OR  */
  YYSYMBOL_AND = 14,                       /* AND  */
  YYSYMBOL_NOT = 15,                       /* NOT  */
  YYSYMBOL_NEQ = 16,                       /* NEQ  */
  YYSYMBOL_GEQ = 17,                       /* GEQ  */
  YYSYMBOL_LEQ = 18,                       /* LEQ  */
  YYSYMBOL_INTEGER = 19,                   /* INTEGER  */
  YYSYMBOL_DOUBLE = 20,                    /* DOUBLE  */
  YYSYMBOL_FLOAT = 21,                     /* FLOAT  */
  YYSYMBOL_CHAR = 22,                      /* CHAR  */
  YYSYMBOL_VARCHAR = 23,                   /* VARCHAR  */
  YYSYMBOL_DATE = 24,                      /* DATE  */
  YYSYMBOL_INTO = 25,                      /* INTO  */
  YYSYMBOL_FROM = 26,                      /* FROM  */
  YYSYMBOL_WHERE = 27,                     /* WHERE  */
  YYSYMBOL_VALUES = 28,                    /* VALUES  */
  YYSYMBOL_JOIN = 29,                      /* JOIN  */
  YYSYMBOL_INNER = 30,                     /* INNER  */
  YYSYMBOL_OUTER = 31,                     /* OUTER  */
  YYSYMBOL_LEFT = 32,                      /* LEFT  */
  YYSYMBOL_RIGHT = 33,                     /* RIGHT  */
  YYSYMBOL_FULL = 34,                      /* FULL  */
  YYSYMBOL_ASC = 35,                       /* ASC  */
  YYSYMBOL_DESC = 36,                      /* DESC  */
  YYSYMBOL_ORDER = 37,                     /* ORDER  */
  YYSYMBOL_BY = 38,                        /* BY  */
  YYSYMBOL_IN = 39,                        /* IN  */
  YYSYMBOL_ON = 40,                        /* ON  */
  YYSYMBOL_AS = 41,                        /* AS  */
  YYSYMBOL_DISTINCT = 42,                  /* DISTINCT  */
  YYSYMBOL_GROUP = 43,                     /* GROUP  */
  YYSYMBOL_USING = 44,                     /* USING  */
  YYSYMBOL_INDEX = 45,                     /* INDEX  */
  YYSYMBOL_TABLE = 46,                     /* TABLE  */
  YYSYMBOL_DATABASE = 47,                  /* DATABASE  */
  YYSYMBOL_DATABASES = 48,                 /* DATABASES  */
  YYSYMBOL_TABLES = 49,                    /* TABLES  */
  YYSYMBOL_DEFAULT = 50,                   /* DEFAULT  */
  YYSYMBOL_UNIQUE = 51,                    /* UNIQUE  */
  YYSYMBOL_PRIMARY = 52,                   /* PRIMARY  */
  YYSYMBOL_FOREIGN = 53,                   /* FOREIGN  */
  YYSYMBOL_REFERENCES = 54,                /* REFERENCES  */
  YYSYMBOL_CHECK = 55,                     /* CHECK  */
  YYSYMBOL_KEY = 56,                       /* KEY  */
  YYSYMBOL_OUTPUT = 57,                    /* OUTPUT  */
  YYSYMBOL_USE = 58,                       /* USE  */
  YYSYMBOL_CREATE = 59,                    /* CREATE  */
  YYSYMBOL_DROP = 60,                      /* DROP  */
  YYSYMBOL_SELECT = 61,                    /* SELECT  */
  YYSYMBOL_INSERT = 62,                    /* INSERT  */
  YYSYMBOL_UPDATE = 63,                    /* UPDATE  */
  YYSYMBOL_DELETE = 64,                    /* DELETE  */
  YYSYMBOL_SHOW = 65,                      /* SHOW  */
  YYSYMBOL_SET = 66,                       /* SET  */
  YYSYMBOL_EXIT = 67,                      /* EXIT  */
  YYSYMBOL_IDENTIFIER = 68,                /* IDENTIFIER  */
  YYSYMBOL_DATE_LITERAL = 69,              /* DATE_LITERAL  */
  YYSYMBOL_STRING_LITERAL = 70,            /* STRING_LITERAL  */
  YYSYMBOL_FLOAT_LITERAL = 71,             /* FLOAT_LITERAL  */
  YYSYMBOL_INT_LITERAL = 72,               /* INT_LITERAL  */
  YYSYMBOL_73_ = 73,                       /* ';'  */
  YYSYMBOL_74_ = 74,                       /* '='  */
  YYSYMBOL_75_ = 75,                       /* '('  */
  YYSYMBOL_76_ = 76,                       /* ')'  */
  YYSYMBOL_77_ = 77,                       /* ','  */
  YYSYMBOL_78_ = 78,                       /* '*'  */
  YYSYMBOL_79_ = 79,                       /* '.'  */
  YYSYMBOL_80_ = 80,                       /* '<'  */
  YYSYMBOL_81_ = 81,                       /* '>'  */
  YYSYMBOL_82_ = 82,                       /* '+'  */
  YYSYMBOL_83_ = 83,                       /* '-'  */
  YYSYMBOL_84_ = 84,                       /* '/'  */
  YYSYMBOL_85_ = 85,                       /* '`'  */
  YYSYMBOL_YYACCEPT = 86,                  /* $accept  */
  YYSYMBOL_sql_stmts = 87,                 /* sql_stmts  */
  YYSYMBOL_sql_stmt = 88,                  /* sql_stmt  */
  YYSYMBOL_create_table_stmt = 89,         /* create_table_stmt  */
  YYSYMBOL_create_database_stmt = 90,      /* create_database_stmt  */
  YYSYMBOL_use_database_stmt = 91,         /* use_database_stmt  */
  YYSYMBOL_drop_database_stmt = 92,        /* drop_database_stmt  */
  YYSYMBOL_show_database_stmt = 93,        /* show_database_stmt  */
  YYSYMBOL_show_databases_stmt = 94,       /* show_databases_stmt  */
  YYSYMBOL_show_tables_stmt = 95,          /* show_tables_stmt  */
  YYSYMBOL_drop_table_stmt = 96,           /* drop_table_stmt  */
  YYSYMBOL_show_table_stmt = 97,           /* show_table_stmt  */
  YYSYMBOL_insert_stmt = 98,               /* insert_stmt  */
  YYSYMBOL_insert_values = 99,             /* insert_values  */
  YYSYMBOL_insert_columns = 100,           /* insert_columns  */
  YYSYMBOL_delete_stmt = 101,              /* delete_stmt  */
  YYSYMBOL_update_stmt = 102,              /* update_stmt  */
  YYSYMBOL_select_stmt = 103,              /* select_stmt  */
  YYSYMBOL_table_refs = 104,               /* table_refs  */
  YYSYMBOL_table_item = 105,               /* table_item  */
  YYSYMBOL_select_expr_list_s = 106,       /* select_expr_list_s  */
  YYSYMBOL_select_expr_list = 107,         /* select_expr_list  */
  YYSYMBOL_select_expr = 108,              /* select_expr  */
  YYSYMBOL_aggregate_expr = 109,           /* aggregate_expr  */
  YYSYMBOL_aggregate_term = 110,           /* aggregate_term  */
  YYSYMBOL_aggregate_op = 111,             /* aggregate_op  */
  YYSYMBOL_where_clause = 112,             /* where_clause  */
  YYSYMBOL_table_extra_options = 113,      /* table_extra_options  */
  YYSYMBOL_table_extra_option_list = 114,  /* table_extra_option_list  */
  YYSYMBOL_table_extra_option = 115,       /* table_extra_option  */
  YYSYMBOL_column_ref = 116,               /* column_ref  */
  YYSYMBOL_column_list = 117,              /* column_list  */
  YYSYMBOL_table_fields = 118,             /* table_fields  */
  YYSYMBOL_table_field = 119,              /* table_field  */
  YYSYMBOL_default_expr = 120,             /* default_expr  */
  YYSYMBOL_field_flags = 121,              /* field_flags  */
  YYSYMBOL_field_flag = 122,               /* field_flag  */
  YYSYMBOL_field_width = 123,              /* field_width  */
  YYSYMBOL_field_type = 124,               /* field_type  */
  YYSYMBOL_logical_op = 125,               /* logical_op  */
  YYSYMBOL_compare_op = 126,               /* compare_op  */
  YYSYMBOL_condition = 127,                /* condition  */
  YYSYMBOL_cond_term = 128,                /* cond_term  */
  YYSYMBOL_expr_list = 129,                /* expr_list  */
  YYSYMBOL_expr = 130,                     /* expr  */
  YYSYMBOL_factor = 131,                   /* factor  */
  YYSYMBOL_term = 132,                     /* term  */
  YYSYMBOL_literal = 133,                  /* literal  */
  YYSYMBOL_literal_list = 134,             /* literal_list  */
  YYSYMBOL_literal_list_expr = 135,        /* literal_list_expr  */
  YYSYMBOL_table_name = 136,               /* table_name  */
  YYSYMBOL_database_name = 137             /* database_name  */
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

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
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

#if 1

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
#endif /* 1 */

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
#define YYFINAL  70
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   308

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  86
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  52
/* YYNRULES -- Number of rules.  */
#define YYNRULES  129
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  259

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   327


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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      75,    76,    78,    82,    77,    83,    79,    84,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    73,
      80,    74,    81,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    85,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    80,    80,    81,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   103,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   125,   130,   137,   143,   151,   158,   167,   175,   180,
     187,   192,   200,   201,   203,   208,   215,   216,   218,   223,
     228,   235,   242,   243,   244,   245,   248,   249,   252,   253,
     256,   261,   268,   275,   285,   290,   297,   302,   309,   314,
     322,   323,   326,   337,   338,   340,   341,   344,   345,   346,
     349,   350,   353,   354,   355,   356,   357,   358,   361,   362,
     364,   365,   366,   367,   368,   369,   370,   373,   379,   382,
     388,   394,   399,   404,   409,   410,   415,   422,   427,   434,
     440,   446,   449,   455,   461,   464,   469,   474,   475,   479,
     482,   487,   492,   497,   504,   509,   516,   522,   523,   526
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "TRUE", "FALSE",
  "NULL_TOKEN", "MIN", "MAX", "SUM", "AVG", "COUNT", "LIKE", "IS", "OR",
  "AND", "NOT", "NEQ", "GEQ", "LEQ", "INTEGER", "DOUBLE", "FLOAT", "CHAR",
  "VARCHAR", "DATE", "INTO", "FROM", "WHERE", "VALUES", "JOIN", "INNER",
  "OUTER", "LEFT", "RIGHT", "FULL", "ASC", "DESC", "ORDER", "BY", "IN",
  "ON", "AS", "DISTINCT", "GROUP", "USING", "INDEX", "TABLE", "DATABASE",
  "DATABASES", "TABLES", "DEFAULT", "UNIQUE", "PRIMARY", "FOREIGN",
  "REFERENCES", "CHECK", "KEY", "OUTPUT", "USE", "CREATE", "DROP",
  "SELECT", "INSERT", "UPDATE", "DELETE", "SHOW", "SET", "EXIT",
  "IDENTIFIER", "DATE_LITERAL", "STRING_LITERAL", "FLOAT_LITERAL",
  "INT_LITERAL", "';'", "'='", "'('", "')'", "','", "'*'", "'.'", "'<'",
  "'>'", "'+'", "'-'", "'/'", "'`'", "$accept", "sql_stmts", "sql_stmt",
  "create_table_stmt", "create_database_stmt", "use_database_stmt",
  "drop_database_stmt", "show_database_stmt", "show_databases_stmt",
  "show_tables_stmt", "drop_table_stmt", "show_table_stmt", "insert_stmt",
  "insert_values", "insert_columns", "delete_stmt", "update_stmt",
  "select_stmt", "table_refs", "table_item", "select_expr_list_s",
  "select_expr_list", "select_expr", "aggregate_expr", "aggregate_term",
  "aggregate_op", "where_clause", "table_extra_options",
  "table_extra_option_list", "table_extra_option", "column_ref",
  "column_list", "table_fields", "table_field", "default_expr",
  "field_flags", "field_flag", "field_width", "field_type", "logical_op",
  "compare_op", "condition", "cond_term", "expr_list", "expr", "factor",
  "term", "literal", "literal_list", "literal_list_expr", "table_name",
  "database_name", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-187)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-128)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     131,   -55,    93,   113,    25,    11,   -48,    -1,   158,   -10,
     -12,   106,  -187,    32,    46,    75,    77,    81,    82,   114,
     126,   127,   128,   137,   139,   140,  -187,  -187,   -48,   -48,
     -55,   -48,   -48,   -55,  -187,  -187,  -187,  -187,  -187,     9,
      19,  -187,  -187,  -187,  -187,    52,  -187,    52,    62,   160,
     147,  -187,  -187,   133,  -187,   -16,   -40,  -187,  -187,   150,
     -48,  -187,   148,   -48,   -48,   -55,  -187,  -187,   157,  -187,
    -187,  -187,  -187,  -187,  -187,  -187,  -187,  -187,  -187,  -187,
    -187,  -187,  -187,  -187,  -187,   155,   159,  -187,   161,  -187,
    -187,   -39,    49,  -187,   152,   -48,    43,   -25,    52,    52,
      52,    52,   164,   205,   163,   -25,   208,  -187,  -187,   169,
     172,   173,   174,   167,   168,  -187,  -187,  -187,   -13,  -187,
     204,  -187,   170,   -40,   -40,  -187,  -187,  -187,   175,   -25,
     177,     0,  -187,   176,   171,   156,   178,  -187,   180,  -187,
    -187,   -48,  -187,   184,  -187,    52,   181,  -187,     2,    52,
    -187,  -187,     0,     0,    68,  -187,   145,  -187,   186,  -187,
    -187,  -187,  -187,  -187,  -187,   179,    36,   185,   187,  -187,
    -187,    40,   -16,   182,  -187,   -25,    -6,  -187,    -3,   135,
    -187,  -187,     0,  -187,    94,  -187,  -187,  -187,   188,  -187,
    -187,  -187,    52,  -187,   190,  -187,   189,   192,   197,   191,
     193,  -187,  -187,  -187,  -187,  -187,    52,    52,  -187,  -187,
    -187,  -187,  -187,   260,   151,   -16,   195,    92,   -25,   194,
     198,     0,   130,   -16,    57,  -187,  -187,   199,   196,  -187,
     262,   151,  -187,   212,  -187,  -187,   201,   206,   207,     4,
    -187,  -187,   151,  -187,  -187,  -187,  -187,  -187,   202,   203,
    -187,  -187,  -187,   226,   213,   209,   214,   210,  -187
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     2,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   129,    23,     0,     0,
       0,     0,     0,     0,   118,    54,    55,    52,    53,     0,
      66,   122,   123,   121,   120,     0,    43,     0,     0,     0,
      42,    45,    47,     0,   115,    46,   111,   114,   117,     0,
       0,   127,     0,     0,     0,     0,    26,    27,     0,    17,
       1,     3,     4,     5,     6,    10,     7,     8,     9,    12,
      11,    13,    15,    14,    16,     0,     0,    22,     0,    28,
      24,     0,     0,   116,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    33,     0,    57,    29,    25,     0,
       0,     0,     0,     0,     0,    51,   119,   128,    57,    39,
      40,    44,     0,   109,   110,   112,   113,    67,     0,     0,
       0,     0,    35,     0,     0,     0,    59,    70,     0,    50,
      49,     0,    37,     0,    48,     0,    30,    69,     0,     0,
     105,   106,     0,     0,    56,    98,     0,    18,     0,    82,
      84,    83,    85,    87,    86,    81,     0,     0,     0,    38,
      41,     0,   108,     0,    34,     0,    57,   103,     0,     0,
      89,    88,     0,    96,     0,    95,    94,    93,     0,    90,
      91,    92,     0,    19,     0,    76,     0,     0,     0,     0,
      58,    61,    71,    21,    20,    31,     0,     0,    68,    36,
     104,    97,   101,     0,     0,    99,     0,    74,     0,     0,
       0,     0,     0,   107,     0,   102,   125,   126,     0,    80,
       0,     0,    78,     0,    72,    75,     0,     0,     0,     0,
      60,    32,     0,   100,    77,    73,    79,    64,     0,     0,
      65,   124,    62,     0,     0,     0,     0,     0,    63
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -187,  -187,   272,  -187,  -187,  -187,  -187,  -187,  -187,  -187,
    -187,  -187,  -187,  -187,  -187,  -187,  -187,  -187,  -187,   144,
    -187,  -187,   200,  -187,   211,  -187,  -111,  -187,  -187,    65,
     -89,  -187,  -187,   122,  -187,  -187,  -187,  -187,  -187,  -187,
    -187,  -147,  -140,    83,    -4,     3,   -38,  -186,  -187,  -187,
      -5,   -11
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,   146,   103,    23,    24,    25,   118,   119,
      49,    50,    51,    52,   114,    53,   132,   167,   200,   201,
      54,   148,   136,   137,   234,   217,   235,   195,   165,   182,
     192,   154,   155,   171,   156,    56,    57,    58,   227,   228,
      59,    27
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      55,    62,   115,   150,   151,    34,   178,   142,   115,    93,
     180,   181,   177,    26,   131,   152,   130,   180,   181,    87,
      61,   131,    90,    85,    86,    63,    88,    89,   226,    40,
      34,    35,    36,    37,    38,    39,    60,    48,   100,   113,
     147,    92,   211,    40,   101,   245,    48,    68,    34,    35,
      36,    37,    38,    39,   108,   104,   251,    34,   106,   107,
      48,    69,   125,   126,   141,   209,    98,    99,    40,    41,
      42,    43,    44,   210,   239,   153,    98,    99,   174,   175,
     250,   180,   181,    47,    91,    48,   208,   196,   197,   198,
     120,   199,    55,    40,    41,    42,    43,    44,  -127,   212,
      45,   123,   124,    46,   135,    72,    70,   230,    47,   213,
      48,    40,    41,    42,    43,    44,   205,   206,    45,    73,
      40,    41,    42,    43,    44,   116,    47,    45,    48,   236,
      94,    98,    99,   241,   206,    47,   120,    48,    28,    29,
      30,   172,   231,   232,   233,   176,   183,   184,    74,   179,
      75,   185,   186,   187,    76,    77,   183,   184,    31,    32,
      33,   185,   186,   187,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,   188,   159,   160,   161,   162,   163,
     164,   196,   197,   198,   188,   199,    95,    78,   215,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    79,
      80,    81,   223,   172,    64,    65,    66,    67,    97,   189,
      82,   116,    83,    84,   105,   190,   191,    98,    99,   189,
      41,    42,    43,    44,    96,   190,   191,    98,    99,   102,
     110,   109,   127,   128,   111,   131,   112,   117,   129,   133,
     134,   135,   138,   139,   140,   143,   144,   158,   219,   157,
     145,   149,   170,   220,   194,   166,   168,   207,   173,   193,
     204,   203,   216,   214,   218,   225,   221,   244,   246,   237,
     222,   229,   243,   238,   248,   249,   242,   247,   252,   253,
     254,   255,   257,    71,   256,   169,   258,   240,   202,     0,
     224,     0,     0,     0,     0,     0,   121,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   122
};

static const yytype_int16 yycheck[] =
{
       4,     6,    91,     3,     4,     5,   153,   118,    97,    47,
      13,    14,   152,    68,    27,    15,   105,    13,    14,    30,
      68,    27,    33,    28,    29,    26,    31,    32,   214,    68,
       5,     6,     7,     8,     9,    10,    25,    85,    78,    78,
     129,    45,   182,    68,    84,   231,    85,    57,     5,     6,
       7,     8,     9,    10,    65,    60,   242,     5,    63,    64,
      85,    73,   100,   101,    77,   176,    82,    83,    68,    69,
      70,    71,    72,    76,   221,    75,    82,    83,    76,    77,
      76,    13,    14,    83,    75,    85,   175,    51,    52,    53,
      95,    55,    96,    68,    69,    70,    71,    72,    79,     5,
      75,    98,    99,    78,    68,    73,     0,    15,    83,    15,
      85,    68,    69,    70,    71,    72,    76,    77,    75,    73,
      68,    69,    70,    71,    72,    76,    83,    75,    85,   218,
      68,    82,    83,    76,    77,    83,   141,    85,    45,    46,
      47,   145,    50,    51,    52,   149,    11,    12,    73,   153,
      73,    16,    17,    18,    73,    73,    11,    12,    45,    46,
      47,    16,    17,    18,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    39,    19,    20,    21,    22,    23,
      24,    51,    52,    53,    39,    55,    26,    73,   192,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    73,
      73,    73,   206,   207,    46,    47,    48,    49,    75,    74,
      73,    76,    73,    73,    66,    80,    81,    82,    83,    74,
      69,    70,    71,    72,    77,    80,    81,    82,    83,    79,
      75,    74,    68,    28,    75,    27,    75,    85,    75,    70,
      68,    68,    68,    76,    76,    41,    76,    76,    56,    73,
      75,    74,    68,    56,    75,    77,    76,    75,    77,    73,
      73,    76,    72,    75,    75,     5,    75,     5,    56,    75,
      77,    76,    76,    75,    68,    68,    77,    76,    76,    76,
      54,    68,    68,    11,    75,   141,    76,   222,   166,    -1,
     207,    -1,    -1,    -1,    -1,    -1,    96,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    97
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,   101,   102,   103,    68,   137,    45,    46,
      47,    45,    46,    47,     5,     6,     7,     8,     9,    10,
      68,    69,    70,    71,    72,    75,    78,    83,    85,   106,
     107,   108,   109,   111,   116,   130,   131,   132,   133,   136,
      25,    68,   136,    26,    46,    47,    48,    49,    57,    73,
       0,    88,    73,    73,    73,    73,    73,    73,    73,    73,
      73,    73,    73,    73,    73,   136,   136,   137,   136,   136,
     137,    75,   130,   132,    68,    26,    77,    75,    82,    83,
      78,    84,    79,   100,   136,    66,   136,   136,   137,    74,
      75,    75,    75,    78,   110,   116,    76,    85,   104,   105,
     136,   108,   110,   131,   131,   132,   132,    68,    28,    75,
     116,    27,   112,    70,    68,    68,   118,   119,    68,    76,
      76,    77,   112,    41,    76,    75,    99,   116,   117,    74,
       3,     4,    15,    75,   127,   128,   130,    73,    76,    19,
      20,    21,    22,    23,    24,   124,    77,   113,    76,   105,
      68,   129,   130,    77,    76,    77,   130,   128,   127,   130,
      13,    14,   125,    11,    12,    16,    17,    18,    39,    74,
      80,    81,   126,    73,    75,   123,    51,    52,    53,    55,
     114,   115,   119,    76,    73,    76,    77,    75,   116,   112,
      76,   128,     5,    15,    75,   130,    72,   121,    75,    56,
      56,    75,    77,   130,   129,     5,   133,   134,   135,    76,
      15,    50,    51,    52,   120,   122,   116,    75,    75,   127,
     115,    76,    77,    76,     5,   133,    56,    76,    68,    68,
      76,   133,    76,    76,    54,    68,    75,    68,    76
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    86,    87,    87,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    88,    88,    88,    88,    88,    88,    88,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,    99,   100,   100,   101,   102,   103,   104,   104,
     105,   105,   106,   106,   107,   107,   108,   108,   109,   109,
     109,   110,   111,   111,   111,   111,   112,   112,   113,   113,
     114,   114,   115,   115,   115,   115,   116,   116,   117,   117,
     118,   118,   119,   120,   120,   121,   121,   122,   122,   122,
     123,   123,   124,   124,   124,   124,   124,   124,   125,   125,
     126,   126,   126,   126,   126,   126,   126,   127,   127,   128,
     128,   128,   128,   128,   128,   128,   128,   129,   129,   130,
     130,   130,   131,   131,   131,   132,   132,   132,   132,   132,
     133,   133,   133,   133,   134,   134,   135,   136,   136,   137
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     5,     7,
       7,     7,     3,     2,     3,     3,     2,     2,     3,     3,
       5,     3,     5,     1,     4,     4,     7,     5,     3,     1,
       1,     3,     1,     1,     3,     1,     1,     1,     4,     4,
       4,     1,     1,     1,     1,     1,     2,     0,     2,     0,
       3,     1,     5,    10,     4,     4,     1,     3,     3,     1,
       1,     3,     5,     2,     0,     2,     0,     2,     1,     2,
       3,     0,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     1,     3,
       5,     3,     4,     2,     3,     1,     1,     3,     1,     3,
       3,     1,     3,     3,     1,     1,     2,     1,     1,     3,
       1,     1,     1,     1,     3,     1,     1,     1,     3,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


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


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
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
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
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
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
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
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
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
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
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
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


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

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

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
    YYNOMEM;
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
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
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
  case 4: /* sql_stmt: create_table_stmt ';'  */
#line 84 "/app/TrivialDB-master/src/parser/sql.y"
                                       { execute_create_table((yyvsp[-1].table_def)); }
#line 1674 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 5: /* sql_stmt: create_database_stmt ';'  */
#line 85 "/app/TrivialDB-master/src/parser/sql.y"
                                               { execute_create_database((yyvsp[-1].val_s)); }
#line 1680 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 6: /* sql_stmt: use_database_stmt ';'  */
#line 86 "/app/TrivialDB-master/src/parser/sql.y"
                                               { execute_use_database((yyvsp[-1].val_s)); }
#line 1686 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 7: /* sql_stmt: show_database_stmt ';'  */
#line 87 "/app/TrivialDB-master/src/parser/sql.y"
                                               { execute_show_database((yyvsp[-1].val_s)); }
#line 1692 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 8: /* sql_stmt: show_databases_stmt ';'  */
#line 88 "/app/TrivialDB-master/src/parser/sql.y"
                                               { execute_show_databases(); }
#line 1698 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 9: /* sql_stmt: show_tables_stmt ';'  */
#line 89 "/app/TrivialDB-master/src/parser/sql.y"
                                               { execute_show_tables(); }
#line 1704 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 10: /* sql_stmt: drop_database_stmt ';'  */
#line 90 "/app/TrivialDB-master/src/parser/sql.y"
                                               { execute_drop_database((yyvsp[-1].val_s)); }
#line 1710 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 11: /* sql_stmt: show_table_stmt ';'  */
#line 91 "/app/TrivialDB-master/src/parser/sql.y"
                                               { execute_show_table((yyvsp[-1].val_s)); }
#line 1716 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 12: /* sql_stmt: drop_table_stmt ';'  */
#line 92 "/app/TrivialDB-master/src/parser/sql.y"
                                               { execute_drop_table((yyvsp[-1].val_s)); }
#line 1722 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 13: /* sql_stmt: insert_stmt ';'  */
#line 93 "/app/TrivialDB-master/src/parser/sql.y"
                                               { execute_insert((yyvsp[-1].insert_info)); }
#line 1728 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 14: /* sql_stmt: update_stmt ';'  */
#line 94 "/app/TrivialDB-master/src/parser/sql.y"
                                               { execute_update((yyvsp[-1].update_info)); }
#line 1734 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 15: /* sql_stmt: delete_stmt ';'  */
#line 95 "/app/TrivialDB-master/src/parser/sql.y"
                                               { execute_delete((yyvsp[-1].delete_info)); }
#line 1740 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 16: /* sql_stmt: select_stmt ';'  */
#line 96 "/app/TrivialDB-master/src/parser/sql.y"
                                               { execute_select((yyvsp[-1].select_info)); }
#line 1746 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 17: /* sql_stmt: EXIT ';'  */
#line 97 "/app/TrivialDB-master/src/parser/sql.y"
                                               { execute_quit(); exit(0); }
#line 1752 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 18: /* sql_stmt: SET OUTPUT '=' STRING_LITERAL ';'  */
#line 98 "/app/TrivialDB-master/src/parser/sql.y"
                                                         { execute_switch_output((yyvsp[-1].val_s)); }
#line 1758 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 19: /* sql_stmt: CREATE INDEX table_name '(' IDENTIFIER ')' ';'  */
#line 99 "/app/TrivialDB-master/src/parser/sql.y"
                                                                     { execute_create_index((yyvsp[-4].val_s), (yyvsp[-2].val_s)); }
#line 1764 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 20: /* sql_stmt: DROP INDEX table_name '(' IDENTIFIER ')' ';'  */
#line 100 "/app/TrivialDB-master/src/parser/sql.y"
                                                                     { execute_drop_index((yyvsp[-4].val_s), (yyvsp[-2].val_s)); }
#line 1770 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 21: /* create_table_stmt: CREATE TABLE table_name '(' table_fields table_extra_options ')'  */
#line 103 "/app/TrivialDB-master/src/parser/sql.y"
                                                                                     {
				  	(yyval.table_def) = (table_def_t*)malloc(sizeof(table_def_t));
					(yyval.table_def)->name = (yyvsp[-4].val_s);
					(yyval.table_def)->fields = (yyvsp[-2].field_items);
					(yyval.table_def)->constraints = (yyvsp[-1].list);
				  }
#line 1781 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 22: /* create_database_stmt: CREATE DATABASE database_name  */
#line 111 "/app/TrivialDB-master/src/parser/sql.y"
                                                       { (yyval.val_s) = (yyvsp[0].val_s); }
#line 1787 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 23: /* use_database_stmt: USE database_name  */
#line 112 "/app/TrivialDB-master/src/parser/sql.y"
                                                       { (yyval.val_s) = (yyvsp[0].val_s); }
#line 1793 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 24: /* drop_database_stmt: DROP DATABASE database_name  */
#line 113 "/app/TrivialDB-master/src/parser/sql.y"
                                                       { (yyval.val_s) = (yyvsp[0].val_s); }
#line 1799 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 25: /* show_database_stmt: SHOW DATABASE database_name  */
#line 114 "/app/TrivialDB-master/src/parser/sql.y"
                                                       { (yyval.val_s) = (yyvsp[0].val_s); }
#line 1805 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 26: /* show_databases_stmt: SHOW DATABASES  */
#line 115 "/app/TrivialDB-master/src/parser/sql.y"
                                                      { (yyval.val_s) = NULL; }
#line 1811 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 27: /* show_tables_stmt: SHOW TABLES  */
#line 116 "/app/TrivialDB-master/src/parser/sql.y"
                                                       { (yyval.val_s) = NULL; }
#line 1817 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 28: /* drop_table_stmt: DROP TABLE table_name  */
#line 117 "/app/TrivialDB-master/src/parser/sql.y"
                                                       { (yyval.val_s) = (yyvsp[0].val_s); }
#line 1823 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 29: /* show_table_stmt: SHOW TABLE table_name  */
#line 118 "/app/TrivialDB-master/src/parser/sql.y"
                                                       { (yyval.val_s) = (yyvsp[0].val_s); }
#line 1829 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 30: /* insert_stmt: INSERT INTO insert_columns VALUES insert_values  */
#line 119 "/app/TrivialDB-master/src/parser/sql.y"
                                                                       {
					 	(yyval.insert_info) = (yyvsp[-2].insert_info);
						(yyval.insert_info)->values = (yyvsp[0].list);
					 }
#line 1838 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 31: /* insert_values: '(' expr_list ')'  */
#line 125 "/app/TrivialDB-master/src/parser/sql.y"
                                         {
					 	(yyval.list) = (linked_list_t*)malloc(sizeof(linked_list_t));
						(yyval.list)->data = (yyvsp[-1].list);
						(yyval.list)->next = NULL;
					 }
#line 1848 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 32: /* insert_values: insert_values ',' '(' expr_list ')'  */
#line 130 "/app/TrivialDB-master/src/parser/sql.y"
                                                                               {
					 	(yyval.list) = (linked_list_t*)malloc(sizeof(linked_list_t));
						(yyval.list)->data = (yyvsp[-1].list);
						(yyval.list)->next = (yyvsp[-4].list);
					 }
#line 1858 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 33: /* insert_columns: table_name  */
#line 137 "/app/TrivialDB-master/src/parser/sql.y"
                                  {
					 	(yyval.insert_info) = (insert_info_t*)malloc(sizeof(insert_info_t));
						(yyval.insert_info)->table   = (yyvsp[0].val_s);
						(yyval.insert_info)->columns = NULL;
						(yyval.insert_info)->values  = NULL;
					 }
#line 1869 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 34: /* insert_columns: table_name '(' column_list ')'  */
#line 143 "/app/TrivialDB-master/src/parser/sql.y"
                                                                          {
					 	(yyval.insert_info) = (insert_info_t*)malloc(sizeof(insert_info_t));
						(yyval.insert_info)->table   = (yyvsp[-3].val_s);
						(yyval.insert_info)->columns = (yyvsp[-1].list);
						(yyval.insert_info)->values  = NULL;
					 }
#line 1880 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 35: /* delete_stmt: DELETE FROM table_name where_clause  */
#line 151 "/app/TrivialDB-master/src/parser/sql.y"
                                                          {
					 	(yyval.delete_info) = (delete_info_t*)malloc(sizeof(delete_info_t));
						(yyval.delete_info)->table = (yyvsp[-1].val_s);
						(yyval.delete_info)->where = (yyvsp[0].expr);
					}
#line 1890 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 36: /* update_stmt: UPDATE table_name SET column_ref '=' expr where_clause  */
#line 158 "/app/TrivialDB-master/src/parser/sql.y"
                                                                             {
					 	(yyval.update_info) = (update_info_t*)malloc(sizeof(update_info_t));
						(yyval.update_info)->table = (yyvsp[-5].val_s);
						(yyval.update_info)->value = (yyvsp[-1].expr);
						(yyval.update_info)->where = (yyvsp[0].expr);
						(yyval.update_info)->column_ref = (yyvsp[-3].column_ref);
					}
#line 1902 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 37: /* select_stmt: SELECT select_expr_list_s FROM table_refs where_clause  */
#line 167 "/app/TrivialDB-master/src/parser/sql.y"
                                                                             {
					 	(yyval.select_info) = (select_info_t*)malloc(sizeof(select_info_t));
						(yyval.select_info)->tables = (yyvsp[-1].list);
						(yyval.select_info)->exprs  = (yyvsp[-3].list);
						(yyval.select_info)->where  = (yyvsp[0].expr);
					}
#line 1913 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 38: /* table_refs: table_refs ',' table_item  */
#line 175 "/app/TrivialDB-master/src/parser/sql.y"
                                                {
						(yyval.list) = (linked_list_t*)malloc(sizeof(linked_list_t));
						(yyval.list)->data = (yyvsp[0].join_info);
						(yyval.list)->next = (yyvsp[-2].list);
					}
#line 1923 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 39: /* table_refs: table_item  */
#line 180 "/app/TrivialDB-master/src/parser/sql.y"
                                                     {
						(yyval.list) = (linked_list_t*)malloc(sizeof(linked_list_t));
						(yyval.list)->data = (yyvsp[0].join_info);
						(yyval.list)->next = NULL;
					}
#line 1933 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 40: /* table_item: table_name  */
#line 187 "/app/TrivialDB-master/src/parser/sql.y"
                                 {
					 	(yyval.join_info) = (table_join_info_t*)calloc(1, sizeof(table_join_info_t));
						(yyval.join_info)->join_type = TABLE_JOIN_NONE;
						(yyval.join_info)->table = (yyvsp[0].val_s);
					}
#line 1943 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 41: /* table_item: table_name AS IDENTIFIER  */
#line 192 "/app/TrivialDB-master/src/parser/sql.y"
                                                               {
					 	(yyval.join_info) = (table_join_info_t*)calloc(1, sizeof(table_join_info_t));
						(yyval.join_info)->join_type = TABLE_JOIN_NONE;
						(yyval.join_info)->table = (yyvsp[-2].val_s);
						(yyval.join_info)->alias = (yyvsp[0].val_s);
					}
#line 1954 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 42: /* select_expr_list_s: select_expr_list  */
#line 200 "/app/TrivialDB-master/src/parser/sql.y"
                                       { (yyval.list) = (yyvsp[0].list); }
#line 1960 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 43: /* select_expr_list_s: '*'  */
#line 201 "/app/TrivialDB-master/src/parser/sql.y"
                                                           { (yyval.list) = NULL; }
#line 1966 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 44: /* select_expr_list: select_expr_list ',' select_expr  */
#line 203 "/app/TrivialDB-master/src/parser/sql.y"
                                                       {
						(yyval.list) = (linked_list_t*)malloc(sizeof(linked_list_t));
						(yyval.list)->data = (yyvsp[0].expr);
						(yyval.list)->next = (yyvsp[-2].list);
					}
#line 1976 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 45: /* select_expr_list: select_expr  */
#line 208 "/app/TrivialDB-master/src/parser/sql.y"
                                                      {
						(yyval.list) = (linked_list_t*)malloc(sizeof(linked_list_t));
						(yyval.list)->data = (yyvsp[0].expr);
						(yyval.list)->next = NULL;
					}
#line 1986 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 46: /* select_expr: expr  */
#line 215 "/app/TrivialDB-master/src/parser/sql.y"
                                      { (yyval.expr) = (yyvsp[0].expr); }
#line 1992 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 47: /* select_expr: aggregate_expr  */
#line 216 "/app/TrivialDB-master/src/parser/sql.y"
                                                          { (yyval.expr) = (yyvsp[0].expr); }
#line 1998 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 48: /* aggregate_expr: aggregate_op '(' aggregate_term ')'  */
#line 218 "/app/TrivialDB-master/src/parser/sql.y"
                                                          {
						(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
						(yyval.expr)->left  = (yyvsp[-1].expr);
						(yyval.expr)->op    = (yyvsp[-3].val_i);
					}
#line 2008 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 49: /* aggregate_expr: COUNT '(' aggregate_term ')'  */
#line 223 "/app/TrivialDB-master/src/parser/sql.y"
                                                                       {
						(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
						(yyval.expr)->left  = (yyvsp[-1].expr);
						(yyval.expr)->op    = OPERATOR_COUNT;
					}
#line 2018 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 50: /* aggregate_expr: COUNT '(' '*' ')'  */
#line 228 "/app/TrivialDB-master/src/parser/sql.y"
                                                            {
						(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
						(yyval.expr)->left  = NULL;
						(yyval.expr)->op    = OPERATOR_COUNT;
					}
#line 2028 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 51: /* aggregate_term: column_ref  */
#line 235 "/app/TrivialDB-master/src/parser/sql.y"
                                 {
						(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
						(yyval.expr)->column_ref = (yyvsp[0].column_ref);
						(yyval.expr)->term_type  = TERM_COLUMN_REF;
					}
#line 2038 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 52: /* aggregate_op: SUM  */
#line 242 "/app/TrivialDB-master/src/parser/sql.y"
                            { (yyval.val_i) = OPERATOR_SUM; }
#line 2044 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 53: /* aggregate_op: AVG  */
#line 243 "/app/TrivialDB-master/src/parser/sql.y"
                                                { (yyval.val_i) = OPERATOR_AVG; }
#line 2050 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 54: /* aggregate_op: MIN  */
#line 244 "/app/TrivialDB-master/src/parser/sql.y"
                                                { (yyval.val_i) = OPERATOR_MIN; }
#line 2056 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 55: /* aggregate_op: MAX  */
#line 245 "/app/TrivialDB-master/src/parser/sql.y"
                                                { (yyval.val_i) = OPERATOR_MAX; }
#line 2062 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 56: /* where_clause: WHERE condition  */
#line 248 "/app/TrivialDB-master/src/parser/sql.y"
                                      { (yyval.expr) = (yyvsp[0].expr); }
#line 2068 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 57: /* where_clause: %empty  */
#line 249 "/app/TrivialDB-master/src/parser/sql.y"
                                                          { (yyval.expr) = NULL; }
#line 2074 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 58: /* table_extra_options: ',' table_extra_option_list  */
#line 252 "/app/TrivialDB-master/src/parser/sql.y"
                                                   { (yyval.list) = (yyvsp[0].list); }
#line 2080 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 59: /* table_extra_options: %empty  */
#line 253 "/app/TrivialDB-master/src/parser/sql.y"
                                                                       { (yyval.list) = NULL; }
#line 2086 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 60: /* table_extra_option_list: table_extra_option_list ',' table_extra_option  */
#line 256 "/app/TrivialDB-master/src/parser/sql.y"
                                                                         {
							(yyval.list) = (linked_list_t*)malloc(sizeof(linked_list_t));
							(yyval.list)->data = (yyvsp[0].constraint);
							(yyval.list)->next = (yyvsp[-2].list);
						}
#line 2096 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 61: /* table_extra_option_list: table_extra_option  */
#line 261 "/app/TrivialDB-master/src/parser/sql.y"
                                                                     {
							(yyval.list) = (linked_list_t*)malloc(sizeof(linked_list_t));
							(yyval.list)->data = (yyvsp[0].constraint);
							(yyval.list)->next = NULL;
						}
#line 2106 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 62: /* table_extra_option: PRIMARY KEY '(' IDENTIFIER ')'  */
#line 268 "/app/TrivialDB-master/src/parser/sql.y"
                                                    {
				   	(yyval.constraint) = (table_constraint_t*)calloc(1, sizeof(table_constraint_t));
					(yyval.constraint)->column_ref = (column_ref_t*)malloc(sizeof(column_ref_t));
					(yyval.constraint)->column_ref->table = NULL;
					(yyval.constraint)->column_ref->column = (yyvsp[-1].val_s);
					(yyval.constraint)->type = TABLE_CONSTRAINT_PRIMARY_KEY;
				   }
#line 2118 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 63: /* table_extra_option: FOREIGN KEY '(' IDENTIFIER ')' REFERENCES IDENTIFIER '(' IDENTIFIER ')'  */
#line 275 "/app/TrivialDB-master/src/parser/sql.y"
                                                                                                             {
				   	(yyval.constraint) = (table_constraint_t*)calloc(1, sizeof(table_constraint_t));
					(yyval.constraint)->column_ref = (column_ref_t*)malloc(sizeof(column_ref_t));
					(yyval.constraint)->column_ref->table = NULL;
					(yyval.constraint)->column_ref->column = (yyvsp[-6].val_s);
					(yyval.constraint)->foreign_column_ref = (column_ref_t*)malloc(sizeof(column_ref_t));
					(yyval.constraint)->foreign_column_ref->table = (yyvsp[-3].val_s);
					(yyval.constraint)->foreign_column_ref->column = (yyvsp[-1].val_s);
					(yyval.constraint)->type = TABLE_CONSTRAINT_FOREIGN_KEY;
				   }
#line 2133 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 64: /* table_extra_option: UNIQUE '(' column_ref ')'  */
#line 285 "/app/TrivialDB-master/src/parser/sql.y"
                                                               {
				   	(yyval.constraint) = (table_constraint_t*)calloc(1, sizeof(table_constraint_t));
					(yyval.constraint)->type = TABLE_CONSTRAINT_UNIQUE;
					(yyval.constraint)->column_ref = (yyvsp[-1].column_ref);
				   }
#line 2143 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 65: /* table_extra_option: CHECK '(' condition ')'  */
#line 290 "/app/TrivialDB-master/src/parser/sql.y"
                                                             {
				   	(yyval.constraint) = (table_constraint_t*)calloc(1, sizeof(table_constraint_t));
					(yyval.constraint)->type = TABLE_CONSTRAINT_CHECK;
					(yyval.constraint)->check_cond = (yyvsp[-1].expr);
				   }
#line 2153 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 66: /* column_ref: IDENTIFIER  */
#line 297 "/app/TrivialDB-master/src/parser/sql.y"
                          {
			 	(yyval.column_ref) = (column_ref_t*)malloc(sizeof(column_ref_t));
				(yyval.column_ref)->table  = NULL;
				(yyval.column_ref)->column = (yyvsp[0].val_s);
			 }
#line 2163 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 67: /* column_ref: table_name '.' IDENTIFIER  */
#line 302 "/app/TrivialDB-master/src/parser/sql.y"
                                                     {
			 	(yyval.column_ref) = (column_ref_t*)malloc(sizeof(column_ref_t));
				(yyval.column_ref)->table  = (yyvsp[-2].val_s);
				(yyval.column_ref)->column = (yyvsp[0].val_s);
			 }
#line 2173 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 68: /* column_list: column_list ',' column_ref  */
#line 309 "/app/TrivialDB-master/src/parser/sql.y"
                                          {
				(yyval.list) = (linked_list_t*)malloc(sizeof(linked_list_t));
				(yyval.list)->data = (yyvsp[0].column_ref);
				(yyval.list)->next = (yyvsp[-2].list);
			 }
#line 2183 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 69: /* column_list: column_ref  */
#line 314 "/app/TrivialDB-master/src/parser/sql.y"
                                      {
			 	(yyval.list) = (linked_list_t*)malloc(sizeof(linked_list_t));
				(yyval.list)->data = (yyvsp[0].column_ref);
				(yyval.list)->next = NULL;
			 }
#line 2193 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 70: /* table_fields: table_field  */
#line 322 "/app/TrivialDB-master/src/parser/sql.y"
                                            { (yyval.field_items) = (yyvsp[0].field_items); }
#line 2199 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 71: /* table_fields: table_fields ',' table_field  */
#line 323 "/app/TrivialDB-master/src/parser/sql.y"
                                                        { (yyval.field_items) = (yyvsp[0].field_items); (yyval.field_items)->next = (yyvsp[-2].field_items); }
#line 2205 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 72: /* table_field: IDENTIFIER field_type field_width field_flags default_expr  */
#line 326 "/app/TrivialDB-master/src/parser/sql.y"
                                                                          {
			 	(yyval.field_items) = (field_item_t*)malloc(sizeof(field_item_t));
				(yyval.field_items)->name = (yyvsp[-4].val_s);
				(yyval.field_items)->type = (yyvsp[-3].val_i);
				(yyval.field_items)->width = (yyvsp[-2].val_i);
				(yyval.field_items)->flags = (yyvsp[-1].val_i);
				(yyval.field_items)->default_value = (yyvsp[0].expr);
				(yyval.field_items)->next = NULL;
			 }
#line 2219 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 73: /* default_expr: DEFAULT literal  */
#line 337 "/app/TrivialDB-master/src/parser/sql.y"
                               { (yyval.expr) = (yyvsp[0].expr); }
#line 2225 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 74: /* default_expr: %empty  */
#line 338 "/app/TrivialDB-master/src/parser/sql.y"
                                           { (yyval.expr) = NULL; }
#line 2231 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 75: /* field_flags: field_flags field_flag  */
#line 340 "/app/TrivialDB-master/src/parser/sql.y"
                                      { (yyval.val_i) = (yyvsp[-1].val_i) | (yyvsp[0].val_i); }
#line 2237 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 76: /* field_flags: %empty  */
#line 341 "/app/TrivialDB-master/src/parser/sql.y"
                                                  { (yyval.val_i) = 0; }
#line 2243 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 77: /* field_flag: NOT NULL_TOKEN  */
#line 344 "/app/TrivialDB-master/src/parser/sql.y"
                              { (yyval.val_i) = FIELD_FLAG_NOTNULL; }
#line 2249 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 78: /* field_flag: UNIQUE  */
#line 345 "/app/TrivialDB-master/src/parser/sql.y"
                                          { (yyval.val_i) = FIELD_FLAG_UNIQUE; }
#line 2255 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 79: /* field_flag: PRIMARY KEY  */
#line 346 "/app/TrivialDB-master/src/parser/sql.y"
                                          { (yyval.val_i) = FIELD_FLAG_PRIMARY; }
#line 2261 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 80: /* field_width: '(' INT_LITERAL ')'  */
#line 349 "/app/TrivialDB-master/src/parser/sql.y"
                                   { (yyval.val_i) = (yyvsp[-1].val_i); }
#line 2267 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 81: /* field_width: %empty  */
#line 350 "/app/TrivialDB-master/src/parser/sql.y"
                                               { (yyval.val_i) = 0; }
#line 2273 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 82: /* field_type: INTEGER  */
#line 353 "/app/TrivialDB-master/src/parser/sql.y"
                      { (yyval.val_i) = FIELD_TYPE_INT; }
#line 2279 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 83: /* field_type: FLOAT  */
#line 354 "/app/TrivialDB-master/src/parser/sql.y"
                              { (yyval.val_i) = FIELD_TYPE_FLOAT; }
#line 2285 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 84: /* field_type: DOUBLE  */
#line 355 "/app/TrivialDB-master/src/parser/sql.y"
                              { (yyval.val_i) = FIELD_TYPE_FLOAT; }
#line 2291 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 85: /* field_type: CHAR  */
#line 356 "/app/TrivialDB-master/src/parser/sql.y"
                              { (yyval.val_i) = FIELD_TYPE_CHAR; }
#line 2297 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 86: /* field_type: DATE  */
#line 357 "/app/TrivialDB-master/src/parser/sql.y"
                              { (yyval.val_i) = FIELD_TYPE_DATE; }
#line 2303 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 87: /* field_type: VARCHAR  */
#line 358 "/app/TrivialDB-master/src/parser/sql.y"
                              { (yyval.val_i) = FIELD_TYPE_VARCHAR; }
#line 2309 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 88: /* logical_op: AND  */
#line 361 "/app/TrivialDB-master/src/parser/sql.y"
                  { (yyval.val_i) = OPERATOR_AND; }
#line 2315 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 89: /* logical_op: OR  */
#line 362 "/app/TrivialDB-master/src/parser/sql.y"
                          { (yyval.val_i) = OPERATOR_OR; }
#line 2321 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 90: /* compare_op: '='  */
#line 364 "/app/TrivialDB-master/src/parser/sql.y"
                  { (yyval.val_i) = OPERATOR_EQ; }
#line 2327 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 91: /* compare_op: '<'  */
#line 365 "/app/TrivialDB-master/src/parser/sql.y"
                          { (yyval.val_i) = OPERATOR_LT; }
#line 2333 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 92: /* compare_op: '>'  */
#line 366 "/app/TrivialDB-master/src/parser/sql.y"
                          { (yyval.val_i) = OPERATOR_GT; }
#line 2339 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 93: /* compare_op: LEQ  */
#line 367 "/app/TrivialDB-master/src/parser/sql.y"
                          { (yyval.val_i) = OPERATOR_LEQ; }
#line 2345 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 94: /* compare_op: GEQ  */
#line 368 "/app/TrivialDB-master/src/parser/sql.y"
                          { (yyval.val_i) = OPERATOR_GEQ; }
#line 2351 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 95: /* compare_op: NEQ  */
#line 369 "/app/TrivialDB-master/src/parser/sql.y"
                          { (yyval.val_i) = OPERATOR_NEQ; }
#line 2357 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 96: /* compare_op: LIKE  */
#line 370 "/app/TrivialDB-master/src/parser/sql.y"
                          { (yyval.val_i) = OPERATOR_LIKE; }
#line 2363 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 97: /* condition: condition logical_op cond_term  */
#line 373 "/app/TrivialDB-master/src/parser/sql.y"
                                            {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->left  = (yyvsp[-2].expr);
				(yyval.expr)->right = (yyvsp[0].expr);
				(yyval.expr)->op    = (yyvsp[-1].val_i);
		   }
#line 2374 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 98: /* condition: cond_term  */
#line 379 "/app/TrivialDB-master/src/parser/sql.y"
                               { (yyval.expr) = (yyvsp[0].expr); }
#line 2380 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 99: /* cond_term: expr compare_op expr  */
#line 382 "/app/TrivialDB-master/src/parser/sql.y"
                                  {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->left  = (yyvsp[-2].expr);
				(yyval.expr)->right = (yyvsp[0].expr);
				(yyval.expr)->op    = (yyvsp[-1].val_i);
		   }
#line 2391 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 100: /* cond_term: expr IN '(' literal_list_expr ')'  */
#line 388 "/app/TrivialDB-master/src/parser/sql.y"
                                                       {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->left  = (yyvsp[-4].expr);
				(yyval.expr)->right = (yyvsp[-1].expr);
				(yyval.expr)->op    = OPERATOR_IN;
		   }
#line 2402 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 101: /* cond_term: expr IS NULL_TOKEN  */
#line 394 "/app/TrivialDB-master/src/parser/sql.y"
                                        {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->left  = (yyvsp[-2].expr);
				(yyval.expr)->op    = OPERATOR_ISNULL;
		   }
#line 2412 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 102: /* cond_term: expr IS NOT NULL_TOKEN  */
#line 399 "/app/TrivialDB-master/src/parser/sql.y"
                                            {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->left  = (yyvsp[-3].expr);
				(yyval.expr)->op    = OPERATOR_NOTNULL;
		   }
#line 2422 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 103: /* cond_term: NOT cond_term  */
#line 404 "/app/TrivialDB-master/src/parser/sql.y"
                                   {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->left  = (yyvsp[0].expr);
				(yyval.expr)->op    = OPERATOR_NOT;
		   }
#line 2432 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 104: /* cond_term: '(' condition ')'  */
#line 409 "/app/TrivialDB-master/src/parser/sql.y"
                                       { (yyval.expr) = (yyvsp[-1].expr); }
#line 2438 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 105: /* cond_term: TRUE  */
#line 410 "/app/TrivialDB-master/src/parser/sql.y"
                          {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->val_b     = 1;
				(yyval.expr)->term_type = TERM_BOOL;
		   }
#line 2448 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 106: /* cond_term: FALSE  */
#line 415 "/app/TrivialDB-master/src/parser/sql.y"
                           {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->val_b     = 0;
				(yyval.expr)->term_type = TERM_BOOL;
		   }
#line 2458 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 107: /* expr_list: expr_list ',' expr  */
#line 422 "/app/TrivialDB-master/src/parser/sql.y"
                                {
				(yyval.list) = (linked_list_t*)malloc(sizeof(linked_list_t));
				(yyval.list)->data = (yyvsp[0].expr);
				(yyval.list)->next = (yyvsp[-2].list);
		   }
#line 2468 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 108: /* expr_list: expr  */
#line 427 "/app/TrivialDB-master/src/parser/sql.y"
                          {
				(yyval.list) = (linked_list_t*)malloc(sizeof(linked_list_t));
				(yyval.list)->data = (yyvsp[0].expr);
				(yyval.list)->next = NULL;
		   }
#line 2478 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 109: /* expr: expr '+' factor  */
#line 434 "/app/TrivialDB-master/src/parser/sql.y"
                             {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->left  = (yyvsp[-2].expr);
				(yyval.expr)->right = (yyvsp[0].expr);
				(yyval.expr)->op    = OPERATOR_ADD;
		   }
#line 2489 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 110: /* expr: expr '-' factor  */
#line 440 "/app/TrivialDB-master/src/parser/sql.y"
                                     {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->left  = (yyvsp[-2].expr);
				(yyval.expr)->right = (yyvsp[0].expr);
				(yyval.expr)->op    = OPERATOR_MINUS;
		   }
#line 2500 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 111: /* expr: factor  */
#line 446 "/app/TrivialDB-master/src/parser/sql.y"
                            { (yyval.expr) = (yyvsp[0].expr); }
#line 2506 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 112: /* factor: factor '*' term  */
#line 449 "/app/TrivialDB-master/src/parser/sql.y"
                             {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->left  = (yyvsp[-2].expr);
				(yyval.expr)->right = (yyvsp[0].expr);
				(yyval.expr)->op    = OPERATOR_MUL;
		   }
#line 2517 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 113: /* factor: factor '/' term  */
#line 455 "/app/TrivialDB-master/src/parser/sql.y"
                                     {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->left  = (yyvsp[-2].expr);
				(yyval.expr)->right = (yyvsp[0].expr);
				(yyval.expr)->op    = OPERATOR_DIV;
		   }
#line 2528 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 114: /* factor: term  */
#line 461 "/app/TrivialDB-master/src/parser/sql.y"
                          { (yyval.expr) = (yyvsp[0].expr); }
#line 2534 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 115: /* term: column_ref  */
#line 464 "/app/TrivialDB-master/src/parser/sql.y"
                        {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->column_ref = (yyvsp[0].column_ref);
				(yyval.expr)->term_type  = TERM_COLUMN_REF;
		   }
#line 2544 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 116: /* term: '-' term  */
#line 469 "/app/TrivialDB-master/src/parser/sql.y"
                              {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->left  = (yyvsp[0].expr);
				(yyval.expr)->op    = OPERATOR_NEGATE;
		   }
#line 2554 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 117: /* term: literal  */
#line 474 "/app/TrivialDB-master/src/parser/sql.y"
                                  { (yyval.expr) = (yyvsp[0].expr); }
#line 2560 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 118: /* term: NULL_TOKEN  */
#line 475 "/app/TrivialDB-master/src/parser/sql.y"
                                {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->term_type  = TERM_NULL;
		   }
#line 2569 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 119: /* term: '(' expr ')'  */
#line 479 "/app/TrivialDB-master/src/parser/sql.y"
                                  { (yyval.expr) = (yyvsp[-1].expr); }
#line 2575 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 120: /* literal: INT_LITERAL  */
#line 482 "/app/TrivialDB-master/src/parser/sql.y"
                         {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->val_i      = (yyvsp[0].val_i);
				(yyval.expr)->term_type  = TERM_INT;
		   }
#line 2585 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 121: /* literal: FLOAT_LITERAL  */
#line 487 "/app/TrivialDB-master/src/parser/sql.y"
                                   {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->val_f      = (yyvsp[0].val_f);
				(yyval.expr)->term_type  = TERM_FLOAT;
		   }
#line 2595 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 122: /* literal: DATE_LITERAL  */
#line 492 "/app/TrivialDB-master/src/parser/sql.y"
                                  {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->val_s      = (yyvsp[0].val_s);
				(yyval.expr)->term_type  = TERM_DATE;
		   }
#line 2605 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 123: /* literal: STRING_LITERAL  */
#line 497 "/app/TrivialDB-master/src/parser/sql.y"
                                    {
		   		(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
				(yyval.expr)->val_s      = (yyvsp[0].val_s);
				(yyval.expr)->term_type  = TERM_STRING;
		   }
#line 2615 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 124: /* literal_list: literal_list ',' literal  */
#line 504 "/app/TrivialDB-master/src/parser/sql.y"
                                        {
				(yyval.list) = (linked_list_t*)malloc(sizeof(linked_list_t));
				(yyval.list)->data = (yyvsp[0].expr);
				(yyval.list)->next = (yyvsp[-2].list);
			 }
#line 2625 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 125: /* literal_list: literal  */
#line 509 "/app/TrivialDB-master/src/parser/sql.y"
                                   {
				(yyval.list) = (linked_list_t*)malloc(sizeof(linked_list_t));
				(yyval.list)->data = (yyvsp[0].expr);
				(yyval.list)->next = NULL;
			 }
#line 2635 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 126: /* literal_list_expr: literal_list  */
#line 516 "/app/TrivialDB-master/src/parser/sql.y"
                                 {
					(yyval.expr) = (expr_node_t*)calloc(1, sizeof(expr_node_t));
					(yyval.expr)->literal_list = (yyvsp[0].list);
					(yyval.expr)->term_type    = TERM_LITERAL_LIST;
				  }
#line 2645 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 127: /* table_name: IDENTIFIER  */
#line 522 "/app/TrivialDB-master/src/parser/sql.y"
                                 { (yyval.val_s) = (yyvsp[0].val_s); }
#line 2651 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 128: /* table_name: '`' IDENTIFIER '`'  */
#line 523 "/app/TrivialDB-master/src/parser/sql.y"
                                         { (yyval.val_s) = (yyvsp[-1].val_s); }
#line 2657 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;

  case 129: /* database_name: IDENTIFIER  */
#line 526 "/app/TrivialDB-master/src/parser/sql.y"
                                 { (yyval.val_s) = (yyvsp[0].val_s); }
#line 2663 "/app/TrivialDB-master/src/parser/sql.tab.c"
    break;


#line 2667 "/app/TrivialDB-master/src/parser/sql.tab.c"

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
      {
        yypcontext_t yyctx
          = {yyssp, yytoken};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
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
  ++yynerrs;

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
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
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
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 529 "/app/TrivialDB-master/src/parser/sql.y"


void yyerror(const char *msg)
{
	fprintf(stderr, "[Error] %s\n", msg);
}

int yywrap()
{
	return 1;
}

char run_parser(const char *input)
{
	char ret;
	if(input) {
		YY_BUFFER_STATE buf = yy_scan_string(input);
		yy_switch_to_buffer(buf);
		ret = yyparse();
		yy_delete_buffer(buf);
	} else {
		ret = yyparse();
	}

	execute_quit();

	return ret;
}
