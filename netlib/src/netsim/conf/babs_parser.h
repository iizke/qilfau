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
     ARRIVAL_TYPE = 258,
     ARRIVAL_FROM = 259,
     ARRIVAL_TO = 260,
     ARRIVAL_LAMBDA = 261,
     ARRIVAL_SAVETO = 262,
     ARRIVAL_LOADFROM = 263,
     ARRIVAL_BURST_TYPE = 264,
     ARRIVAL_BURST_FROM = 265,
     ARRIVAL_BURST_TO = 266,
     QUEUE_KIND = 267,
     BURST_FIFO_QUEUE = 268,
     QUEUE_MAXLENGTH = 269,
     QUEUE_SERVERS = 270,
     SERVICE_TYPE = 271,
     SERVICE_LAMBDA = 272,
     SERVICE_TO = 273,
     SERVICE_FROM = 274,
     SERVICE_SAVETO = 275,
     SERVICE_LOADFROM = 276,
     DEPARTURE_SAVETO = 277,
     STOP_MAXTIME = 278,
     STOP_MAXARRIVAL = 279,
     STOP_QUEUEZERO = 280,
     NTHREADS = 281,
     PROTOCOL = 282,
     PROTO_BABSQ = 283,
     LIBS_RANDOM = 284,
     LIB_RAND_IRAND = 285,
     LIB_RAND_RANDLIB = 286,
     R_MARKOV = 287,
     R_UNF = 288,
     R_FILE = 289,
     R_MMPP = 290,
     R_MMPP_R = 291,
     R_OTHER = 292,
     DEBUG_ERROR = 293,
     DEBUG_WARNING = 294,
     DEBUG_INFO = 295,
     YES = 296,
     NO = 297,
     EQ = 298,
     ENDLINE = 299,
     REALNUM = 300,
     INTNUM = 301,
     STRING = 302
   };
#endif
/* Tokens.  */
#define ARRIVAL_TYPE 258
#define ARRIVAL_FROM 259
#define ARRIVAL_TO 260
#define ARRIVAL_LAMBDA 261
#define ARRIVAL_SAVETO 262
#define ARRIVAL_LOADFROM 263
#define ARRIVAL_BURST_TYPE 264
#define ARRIVAL_BURST_FROM 265
#define ARRIVAL_BURST_TO 266
#define QUEUE_KIND 267
#define BURST_FIFO_QUEUE 268
#define QUEUE_MAXLENGTH 269
#define QUEUE_SERVERS 270
#define SERVICE_TYPE 271
#define SERVICE_LAMBDA 272
#define SERVICE_TO 273
#define SERVICE_FROM 274
#define SERVICE_SAVETO 275
#define SERVICE_LOADFROM 276
#define DEPARTURE_SAVETO 277
#define STOP_MAXTIME 278
#define STOP_MAXARRIVAL 279
#define STOP_QUEUEZERO 280
#define NTHREADS 281
#define PROTOCOL 282
#define PROTO_BABSQ 283
#define LIBS_RANDOM 284
#define LIB_RAND_IRAND 285
#define LIB_RAND_RANDLIB 286
#define R_MARKOV 287
#define R_UNF 288
#define R_FILE 289
#define R_MMPP 290
#define R_MMPP_R 291
#define R_OTHER 292
#define DEBUG_ERROR 293
#define DEBUG_WARNING 294
#define DEBUG_INFO 295
#define YES 296
#define NO 297
#define EQ 298
#define ENDLINE 299
#define REALNUM 300
#define INTNUM 301
#define STRING 302




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 23 "babs_parser.y"
char* str; int ival; double dval;


/* Line 2068 of yacc.c  */
#line 148 "babs_parser.h"
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



