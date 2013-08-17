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
     QUEUE_MAXLENGTH = 276,
     QUEUE_SERVERS = 277,
     SERVICE_TYPE = 278,
     SERVICE_LAMBDA = 279,
     SERVICE_TO = 280,
     SERVICE_FROM = 281,
     SERVICE_SAVETO = 282,
     SERVICE_LOADFROM = 283,
     SERVICE_MEAN = 284,
     SERVICE_SDEV = 285,
     DEPARTURE_SAVETO = 286,
     STOP_MAXTIME = 287,
     STOP_MAXARRIVAL = 288,
     STOP_QUEUEZERO = 289,
     NTHREADS = 290,
     PROTOCOL = 291,
     PROTO_BABSQ = 292,
     LIBS_RANDOM = 293,
     LIB_RAND_IRAND = 294,
     LIB_RAND_RANDLIB = 295,
     R_MARKOV = 296,
     R_UNF = 297,
     R_FILE = 298,
     R_MMPP = 299,
     R_MMPP_R = 300,
     R_POISSON = 301,
     R_OTHER = 302,
     R_NORMAL = 303,
     DEBUG_ERROR = 304,
     DEBUG_WARNING = 305,
     DEBUG_INFO = 306,
     YES = 307,
     NO = 308,
     EQ = 309,
     ENDLINE = 310,
     REALNUM = 311,
     INTNUM = 312,
     STRING = 313
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
#define QUEUE_MAXLENGTH 276
#define QUEUE_SERVERS 277
#define SERVICE_TYPE 278
#define SERVICE_LAMBDA 279
#define SERVICE_TO 280
#define SERVICE_FROM 281
#define SERVICE_SAVETO 282
#define SERVICE_LOADFROM 283
#define SERVICE_MEAN 284
#define SERVICE_SDEV 285
#define DEPARTURE_SAVETO 286
#define STOP_MAXTIME 287
#define STOP_MAXARRIVAL 288
#define STOP_QUEUEZERO 289
#define NTHREADS 290
#define PROTOCOL 291
#define PROTO_BABSQ 292
#define LIBS_RANDOM 293
#define LIB_RAND_IRAND 294
#define LIB_RAND_RANDLIB 295
#define R_MARKOV 296
#define R_UNF 297
#define R_FILE 298
#define R_MMPP 299
#define R_MMPP_R 300
#define R_POISSON 301
#define R_OTHER 302
#define R_NORMAL 303
#define DEBUG_ERROR 304
#define DEBUG_WARNING 305
#define DEBUG_INFO 306
#define YES 307
#define NO 308
#define EQ 309
#define ENDLINE 310
#define REALNUM 311
#define INTNUM 312
#define STRING 313




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 24 "babs_parser.y"
char* str; int ival; double dval;


/* Line 2068 of yacc.c  */
#line 170 "babs_parser.h"
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



