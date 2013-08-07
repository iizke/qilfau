/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
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
     R_MARKOV = 258,
     R_UNF = 259,
     R_FILE = 260,
     R_OTHER = 261,
     R_CONST = 262,
     LINK_BEGIN = 263,
     LINK_END = 264,
     LINK_DELAY_TYPE = 265,
     LINK_DELAY_TO = 266,
     LINK_DELAY_FROM = 267,
     LINK_DELAY_LAMBDA = 268,
     LINK_DELAY_CONST = 269,
     LINK_SRC = 270,
     LINK_DEST = 271,
     YES = 272,
     NO = 273,
     EQ = 274,
     ENDLINE = 275,
     REALNUM = 276,
     INTNUM = 277,
     STRING = 278
   };
#endif
/* Tokens.  */
#define R_MARKOV 258
#define R_UNF 259
#define R_FILE 260
#define R_OTHER 261
#define R_CONST 262
#define LINK_BEGIN 263
#define LINK_END 264
#define LINK_DELAY_TYPE 265
#define LINK_DELAY_TO 266
#define LINK_DELAY_FROM 267
#define LINK_DELAY_LAMBDA 268
#define LINK_DELAY_CONST 269
#define LINK_SRC 270
#define LINK_DEST 271
#define YES 272
#define NO 273
#define EQ 274
#define ENDLINE 275
#define REALNUM 276
#define INTNUM 277
#define STRING 278




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 17 "channelparser.y"
char* str; int ival; double dval;


/* Line 2068 of yacc.c  */
#line 100 "channelparser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif



#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



