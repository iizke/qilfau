
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
     CSMA_NSTATIONS = 278,
     CSMA_PROB = 279,
     CSMA_MAXBACKOFF = 280,
     CSMA_COLLISION = 281,
     CSMA_SLOTTIME = 282,
     PROTOCOL = 283,
     PROTO_CSMA = 284,
     PROTO_ONE_QUEUE = 285,
     LIBS_RANDOM = 286,
     LIB_RAND_IRAND = 287,
     LIB_RAND_RANDLIB = 288,
     R_MARKOV = 289,
     R_UNF = 290,
     R_FILE = 291,
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
#define CSMA_NSTATIONS 278
#define CSMA_PROB 279
#define CSMA_MAXBACKOFF 280
#define CSMA_COLLISION 281
#define CSMA_SLOTTIME 282
#define PROTOCOL 283
#define PROTO_CSMA 284
#define PROTO_ONE_QUEUE 285
#define LIBS_RANDOM 286
#define LIB_RAND_IRAND 287
#define LIB_RAND_RANDLIB 288
#define R_MARKOV 289
#define R_UNF 290
#define R_FILE 291
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

/* Line 1676 of yacc.c  */
#line 16 "parser.y"
char* str; int ival; double dval;


/* Line 1676 of yacc.c  */
#line 150 "parser.h"
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



