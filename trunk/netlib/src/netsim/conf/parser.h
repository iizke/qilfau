
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
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
     QUEUE_KIND = 264,
     FIFO_QUEUE = 265,
     QUEUE_MAXLENGTH = 266,
     QUEUE_SERVERS = 267,
     SERVICE_TYPE = 268,
     SERVICE_LAMBDA = 269,
     SERVICE_TO = 270,
     SERVICE_FROM = 271,
     SERVICE_SAVETO = 272,
     SERVICE_LOADFROM = 273,
     DEPARTURE_SAVETO = 274,
     STOP_MAXTIME = 275,
     STOP_MAXARRIVAL = 276,
     STOP_QUEUEZERO = 277,
     NTHREADS = 278,
     NODETYPE = 279,
     SOURCE = 280,
     SINK = 281,
     TRANSIT = 282,
     CSMA_NSTATIONS = 283,
     CSMA_PROB = 284,
     CSMA_MAXBACKOFF = 285,
     CSMA_COLLISION = 286,
     CSMA_SLOTTIME = 287,
     PROTOCOL = 288,
     PROTO_CSMA = 289,
     PROTO_ONE_QUEUE = 290,
     PROTO_NET_QUEUE = 291,
     LIBS_RANDOM = 292,
     LIB_RAND_IRAND = 293,
     LIB_RAND_RANDLIB = 294,
     R_MARKOV = 295,
     R_UNF = 296,
     R_FILE = 297,
     R_MMPP = 298,
     R_MMPP_R = 299,
     R_OTHER = 300,
     DEBUG_ERROR = 301,
     DEBUG_WARNING = 302,
     DEBUG_INFO = 303,
     YES = 304,
     NO = 305,
     EQ = 306,
     ENDLINE = 307,
     REALNUM = 308,
     INTNUM = 309,
     STRING = 310
   };
#endif
/* Tokens.  */
#define ARRIVAL_TYPE 258
#define ARRIVAL_FROM 259
#define ARRIVAL_TO 260
#define ARRIVAL_LAMBDA 261
#define ARRIVAL_SAVETO 262
#define ARRIVAL_LOADFROM 263
#define QUEUE_KIND 264
#define FIFO_QUEUE 265
#define QUEUE_MAXLENGTH 266
#define QUEUE_SERVERS 267
#define SERVICE_TYPE 268
#define SERVICE_LAMBDA 269
#define SERVICE_TO 270
#define SERVICE_FROM 271
#define SERVICE_SAVETO 272
#define SERVICE_LOADFROM 273
#define DEPARTURE_SAVETO 274
#define STOP_MAXTIME 275
#define STOP_MAXARRIVAL 276
#define STOP_QUEUEZERO 277
#define NTHREADS 278
#define NODETYPE 279
#define SOURCE 280
#define SINK 281
#define TRANSIT 282
#define CSMA_NSTATIONS 283
#define CSMA_PROB 284
#define CSMA_MAXBACKOFF 285
#define CSMA_COLLISION 286
#define CSMA_SLOTTIME 287
#define PROTOCOL 288
#define PROTO_CSMA 289
#define PROTO_ONE_QUEUE 290
#define PROTO_NET_QUEUE 291
#define LIBS_RANDOM 292
#define LIB_RAND_IRAND 293
#define LIB_RAND_RANDLIB 294
#define R_MARKOV 295
#define R_UNF 296
#define R_FILE 297
#define R_MMPP 298
#define R_MMPP_R 299
#define R_OTHER 300
#define DEBUG_ERROR 301
#define DEBUG_WARNING 302
#define DEBUG_INFO 303
#define YES 304
#define NO 305
#define EQ 306
#define ENDLINE 307
#define REALNUM 308
#define INTNUM 309
#define STRING 310




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 16 "parser.y"
char* str; int ival; double dval;


/* Line 1676 of yacc.c  */
#line 166 "parser.h"
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



