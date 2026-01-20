/* A Bison parser, made by GNU Bison 3.8.2.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_APP_TRIVIALDB_MASTER_SRC_PARSER_SQL_TAB_H_INCLUDED
# define YY_YY_APP_TRIVIALDB_MASTER_SRC_PARSER_SQL_TAB_H_INCLUDED
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
    TRUE = 258,                    /* TRUE  */
    FALSE = 259,                   /* FALSE  */
    NULL_TOKEN = 260,              /* NULL_TOKEN  */
    MIN = 261,                     /* MIN  */
    MAX = 262,                     /* MAX  */
    SUM = 263,                     /* SUM  */
    AVG = 264,                     /* AVG  */
    COUNT = 265,                   /* COUNT  */
    LIKE = 266,                    /* LIKE  */
    IS = 267,                      /* IS  */
    OR = 268,                      /* OR  */
    AND = 269,                     /* AND  */
    NOT = 270,                     /* NOT  */
    NEQ = 271,                     /* NEQ  */
    GEQ = 272,                     /* GEQ  */
    LEQ = 273,                     /* LEQ  */
    INTEGER = 274,                 /* INTEGER  */
    DOUBLE = 275,                  /* DOUBLE  */
    FLOAT = 276,                   /* FLOAT  */
    CHAR = 277,                    /* CHAR  */
    VARCHAR = 278,                 /* VARCHAR  */
    DATE = 279,                    /* DATE  */
    INTO = 280,                    /* INTO  */
    FROM = 281,                    /* FROM  */
    WHERE = 282,                   /* WHERE  */
    VALUES = 283,                  /* VALUES  */
    JOIN = 284,                    /* JOIN  */
    INNER = 285,                   /* INNER  */
    OUTER = 286,                   /* OUTER  */
    LEFT = 287,                    /* LEFT  */
    RIGHT = 288,                   /* RIGHT  */
    FULL = 289,                    /* FULL  */
    ASC = 290,                     /* ASC  */
    DESC = 291,                    /* DESC  */
    ORDER = 292,                   /* ORDER  */
    BY = 293,                      /* BY  */
    IN = 294,                      /* IN  */
    ON = 295,                      /* ON  */
    AS = 296,                      /* AS  */
    DISTINCT = 297,                /* DISTINCT  */
    GROUP = 298,                   /* GROUP  */
    USING = 299,                   /* USING  */
    INDEX = 300,                   /* INDEX  */
    TABLE = 301,                   /* TABLE  */
    DATABASE = 302,                /* DATABASE  */
    DATABASES = 303,               /* DATABASES  */
    TABLES = 304,                  /* TABLES  */
    DEFAULT = 305,                 /* DEFAULT  */
    UNIQUE = 306,                  /* UNIQUE  */
    PRIMARY = 307,                 /* PRIMARY  */
    FOREIGN = 308,                 /* FOREIGN  */
    REFERENCES = 309,              /* REFERENCES  */
    CHECK = 310,                   /* CHECK  */
    KEY = 311,                     /* KEY  */
    OUTPUT = 312,                  /* OUTPUT  */
    USE = 313,                     /* USE  */
    CREATE = 314,                  /* CREATE  */
    DROP = 315,                    /* DROP  */
    SELECT = 316,                  /* SELECT  */
    INSERT = 317,                  /* INSERT  */
    UPDATE = 318,                  /* UPDATE  */
    DELETE = 319,                  /* DELETE  */
    SHOW = 320,                    /* SHOW  */
    SET = 321,                     /* SET  */
    EXIT = 322,                    /* EXIT  */
    IDENTIFIER = 323,              /* IDENTIFIER  */
    DATE_LITERAL = 324,            /* DATE_LITERAL  */
    STRING_LITERAL = 325,          /* STRING_LITERAL  */
    FLOAT_LITERAL = 326,           /* FLOAT_LITERAL  */
    INT_LITERAL = 327              /* INT_LITERAL  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 19 "/app/TrivialDB-master/src/parser/sql.y"

	char *val_s;
	int   val_i;
	float val_f;
	struct field_item_t       *field_items;
	struct table_def_t        *table_def;
	struct column_ref_t       *column_ref;
	struct linked_list_t      *list;
	struct table_constraint_t *constraint;
	struct insert_info_t      *insert_info;
	struct update_info_t      *update_info;
	struct delete_info_t      *delete_info;
	struct select_info_t      *select_info;
	struct table_join_info_t  *join_info;
	struct expr_node_t        *expr;

#line 153 "/app/TrivialDB-master/src/parser/sql.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_APP_TRIVIALDB_MASTER_SRC_PARSER_SQL_TAB_H_INCLUDED  */
