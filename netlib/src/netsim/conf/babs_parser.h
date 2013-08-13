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
     BURST_TYPE = 264,
     BURST_FROM = 265,
     BURST_TO = 266,
     BURST_LAMBDA = 267,
     BURST_SAVETO = 268,
     BURST_LOADFROM = 269,
     QUEUE_KIND = 270,
     BURST_FIFO_QUEUE = 271,
     QUEUE_MAXLENGTH = 272,
     QUEUE_SERVERS = 273,
     SERVICE_TYPE = 274,
     SERVICE_LAMBDA = 275,
     SERVICE_TO = 276,
     SERVICE_FROM = 277,
     SERVICE_SAVETO = 278,
     SERVICE_LOADFROM = 279,
     DEPARTURE_SAVETO = 280,
     STOP_MAXTIME = 281,
     STOP_MAXARRIVAL = 282,
     STOP_QUEUEZERO = 283,
     NTHREADS = 284,
     PROTOCOL = 285,
     PROTO_BABSQ = 286,
     LIBS_RANDOM = 287,
     LIB_RAND_IRAND = 288,
     LIB_RAND_RANDLIB = 289,
     R_MARKOV = 290,
     R_UNF = 291,
     R_FILE = 292,
     R_MMPP = 293,
     R_MMPP_R = 294,
     R_OTHER = 295,
     DEBUG_ERROR = 296,
     DEBUG_WARNING = 297,
     DEBUG_INFO = 298,
     YES = 299,
     NO = 300,
     EQ = 301,
     ENDLINE = 302,
     REALNUM = 303,
     INTNUM = 304,
     STRING = 305
   };
#endif
/* Tokens.  */
#define ARRIVAL_TYPE 258
#define ARRIVAL_FROM 259
#define ARRIVAL_TO 260
#define ARRIVAL_LAMBDA 261
#define ARRIVAL_SAVETO 262
#define ARRIVAL_LOADFROM 263
#define BURST_TYPE 264
#define BURST_FROM 265
#define BURST_TO 266
#define BURST_LAMBDA 267
#define BURST_SAVETO 268
#define BURST_LOADFROM 269
#define QUEUE_KIND 270
#define BURST_FIFO_QUEUE 271
#define QUEUE_MAXLENGTH 272
#define QUEUE_SERVERS 273
#define SERVICE_TYPE 274
#define SERVICE_LAMBDA 275
#define SERVICE_TO 276
#define SERVICE_FROM 277
#define SERVICE_SAVETO 278
#define SERVICE_LOADFROM 279
#define DEPARTURE_SAVETO 280
#define STOP_MAXTIME 281
#define STOP_MAXARRIVAL 282
#define STOP_QUEUEZERO 283
#define NTHREADS 284
#define PROTOCOL 285
#define PROTO_BABSQ 286
#define LIBS_RANDOM 287
#define LIB_RAND_IRAND 288
#define LIB_RAND_RANDLIB 289
#define R_MARKOV 290
#define R_UNF 291
#define R_FILE 292
#define R_MMPP 293
#define R_MMPP_R 294
#define R_OTHER 295
#define DEBUG_ERROR 296
#define DEBUG_WARNING 297
#define DEBUG_INFO 298
#define YES 299
#define NO 300
#define EQ 301
#define ENDLINE 302
#define REALNUM 303
#define INTNUM 304
#define STRING 305




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 23 "babs_parser.y"
char* str; int ival; double dval;


/* Line 2068 of yacc.c  */
#line 154 "babs_parser.h"
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



