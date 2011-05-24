
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
     CSMA_NSTATIONS = 277,
     CSMA_PROB = 278,
     CSMA_MAXBACKOFF = 279,
     CSMA_COLLISION = 280,
     CSMA_SLOTTIME = 281,
     PROTOCOL = 282,
     PROTO_CSMA = 283,
     PROTO_ONE_QUEUE = 284,
     LIBS_RANDOM = 285,
     LIB_RAND_IRAND = 286,
     LIB_RAND_RANDLIB = 287,
     R_MARKOV = 288,
     R_UNF = 289,
     R_FILE = 290,
     R_OTHER = 291,
     DEBUG_ERROR = 292,
     DEBUG_WARNING = 293,
     DEBUG_INFO = 294,
     YES = 295,
     NO = 296,
     EQ = 297,
     ENDLINE = 298,
     REALNUM = 299,
     INTNUM = 300,
     STRING = 301
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
#define CSMA_NSTATIONS 277
#define CSMA_PROB 278
#define CSMA_MAXBACKOFF 279
#define CSMA_COLLISION 280
#define CSMA_SLOTTIME 281
#define PROTOCOL 282
#define PROTO_CSMA 283
#define PROTO_ONE_QUEUE 284
#define LIBS_RANDOM 285
#define LIB_RAND_IRAND 286
#define LIB_RAND_RANDLIB 287
#define R_MARKOV 288
#define R_UNF 289
#define R_FILE 290
#define R_OTHER 291
#define DEBUG_ERROR 292
#define DEBUG_WARNING 293
#define DEBUG_INFO 294
#define YES 295
#define NO 296
#define EQ 297
#define ENDLINE 298
#define REALNUM 299
#define INTNUM 300
#define STRING 301




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 16 "parser.y"
char* str; int ival; double dval;


/* Line 1676 of yacc.c  */
#line 148 "parser.h"
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



