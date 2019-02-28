/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
     OTHER = 258,
     LINECOMM = 259,
     BLOCKCOMM = 260,
     IF = 261,
     ELSE = 262,
     WHILE = 263,
     FOR = 264,
     FUNCTION = 265,
     RETURN = 266,
     BREAK = 267,
     CONTINUE = 268,
     AND = 269,
     NOT = 270,
     OR = 271,
     LOCAL = 272,
     TRUE = 273,
     FALSE = 274,
     NIL = 275,
     CURL_O = 276,
     CURL_C = 277,
     BRAC_O = 278,
     BRAC_C = 279,
     ANGL_O = 280,
     ANGL_C = 281,
     SEMI = 282,
     COMMA = 283,
     COLON = 284,
     DCOLON = 285,
     DOT = 286,
     DOTDOT = 287,
     ASSIGN = 288,
     PLUS = 289,
     MINUS = 290,
     MUL = 291,
     SLASH = 292,
     PERC = 293,
     EQUALS = 294,
     NEQUALS = 295,
     INCR = 296,
     DECR = 297,
     GREATER = 298,
     LESS = 299,
     GREATER_E = 300,
     LESS_E = 301,
     SUB = 302,
     ADD = 303,
     DIV = 304,
     EXP = 305,
     EQ = 306
   };
#endif
/* Tokens.  */
#define OTHER 258
#define LINECOMM 259
#define BLOCKCOMM 260
#define IF 261
#define ELSE 262
#define WHILE 263
#define FOR 264
#define FUNCTION 265
#define RETURN 266
#define BREAK 267
#define CONTINUE 268
#define AND 269
#define NOT 270
#define OR 271
#define LOCAL 272
#define TRUE 273
#define FALSE 274
#define NIL 275
#define CURL_O 276
#define CURL_C 277
#define BRAC_O 278
#define BRAC_C 279
#define ANGL_O 280
#define ANGL_C 281
#define SEMI 282
#define COMMA 283
#define COLON 284
#define DCOLON 285
#define DOT 286
#define DOTDOT 287
#define ASSIGN 288
#define PLUS 289
#define MINUS 290
#define MUL 291
#define SLASH 292
#define PERC 293
#define EQUALS 294
#define NEQUALS 295
#define INCR 296
#define DECR 297
#define GREATER 298
#define LESS 299
#define GREATER_E 300
#define LESS_E 301
#define SUB 302
#define ADD 303
#define DIV 304
#define EXP 305
#define EQ 306




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE alpha_yylval;

