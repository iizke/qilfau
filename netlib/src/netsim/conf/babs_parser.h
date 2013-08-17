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
     ARRIVAL_MEAN = 264,
     ARRIVAL_SDEV = 265,
     BURST_TYPE = 266,
     BURST_FROM = 267,
     BURST_TO = 268,
     BURST_LAMBDA = 269,
     BURST_SAVETO = 270,
     BURST_LOADFROM = 271,
     BURST_MEAN = 272,
     BURST_SDEV = 273,
     QUEUE_KIND = 274,
     BURST_FIFO_QUEUE = 275,
     BURST_SCHED1_QUEUE = 276,
     QUEUE_MAXLENGTH = 277,
     QUEUE_SERVERS = 278,
     SERVICE_TYPE = 279,
     SERVICE_LAMBDA = 280,
     SERVICE_TO = 281,
     SERVICE_FROM = 282,
     SERVICE_SAVETO = 283,
     SERVICE_LOADFROM = 284,
     SERVICE_MEAN = 285,
     SERVICE_SDEV = 286,
     DEPARTURE_SAVETO = 287,
     STOP_MAXTIME = 288,
     STOP_MAXARRIVAL = 289,
     STOP_QUEUEZERO = 290,
     NTHREADS = 291,
     PROTOCOL = 292,
     PROTO_BABSQ = 293,
     LIBS_RANDOM = 294,
     LIB_RAND_IRAND = 295,
     LIB_RAND_RANDLIB = 296,
     R_MARKOV = 297,
     R_UNF = 298,
     R_FILE = 299,
     R_MMPP = 300,
     R_MMPP_R = 301,
     R_POISSON = 302,
     R_OTHER = 303,
     R_NORMAL = 304,
     DEBUG_ERROR = 305,
     DEBUG_WARNING = 306,
     DEBUG_INFO = 307,
     YES = 308,
     NO = 309,
     EQ = 310,
     ENDLINE = 311,
     REALNUM = 312,
     INTNUM = 313,
     STRING = 314
   };
#endif
/* Tokens.  */
#define ARRIVAL_TYPE 258
#define ARRIVAL_FROM 259
#define ARRIVAL_TO 260
#define ARRIVAL_LAMBDA 261
#define ARRIVAL_SAVETO 262
#define ARRIVAL_LOADFROM 263
#define ARRIVAL_MEAN 264
#define ARRIVAL_SDEV 265
#define BURST_TYPE 266
#define BURST_FROM 267
#define BURST_TO 268
#define BURST_LAMBDA 269
#define BURST_SAVETO 270
#define BURST_LOADFROM 271
#define BURST_MEAN 272
#define BURST_SDEV 273
#define QUEUE_KIND 274
#define BURST_FIFO_QUEUE 275
#define BURST_SCHED1_QUEUE 276
#define QUEUE_MAXLENGTH 277
#define QUEUE_SERVERS 278
#define SERVICE_TYPE 279
#define SERVICE_LAMBDA 280
#define SERVICE_TO 281
#define SERVICE_FROM 282
#define SERVICE_SAVETO 283
#define SERVICE_LOADFROM 284
#define SERVICE_MEAN 285
#define SERVICE_SDEV 286
#define DEPARTURE_SAVETO 287
#define STOP_MAXTIME 288
#define STOP_MAXARRIVAL 289
#define STOP_QUEUEZERO 290
#define NTHREADS 291
#define PROTOCOL 292
#define PROTO_BABSQ 293
#define LIBS_RANDOM 294
#define LIB_RAND_IRAND 295
#define LIB_RAND_RANDLIB 296
#define R_MARKOV 297
#define R_UNF 298
#define R_FILE 299
#define R_MMPP 300
#define R_MMPP_R 301
#define R_POISSON 302
#define R_OTHER 303
#define R_NORMAL 304
#define DEBUG_ERROR 305
#define DEBUG_WARNING 306
#define DEBUG_INFO 307
#define YES 308
#define NO 309
#define EQ 310
#define ENDLINE 311
#define REALNUM 312
#define INTNUM 313
#define STRING 314




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 24 "babs_parser.y"
char* str; int ival; double dval;


/* Line 2068 of yacc.c  */
#line 172 "babs_parser.h"
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



