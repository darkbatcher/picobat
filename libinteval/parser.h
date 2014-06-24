/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
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
     NOMBRE = 258,
     VARIABLE = 259,
     PLUS = 260,
     MOINS = 261,
     FOIS = 262,
     DIVISE = 263,
     NON_LOG = 264,
     NON_BAB = 265,
     OU_LOG = 266,
     ET_LOG = 267,
     OU_BAB = 268,
     ET_BAB = 269,
     XOR_BAB = 270,
     DEC_DROITE = 271,
     DEC_GAUCHE = 272,
     MODULO = 273,
     PARENTHESE_GAUCHE = 274,
     PARENTHESE_DROITE = 275,
     FIN = 276,
     NEG = 277
   };
#endif
/* Tokens.  */
#define NOMBRE 258
#define VARIABLE 259
#define PLUS 260
#define MOINS 261
#define FOIS 262
#define DIVISE 263
#define NON_LOG 264
#define NON_BAB 265
#define OU_LOG 266
#define ET_LOG 267
#define OU_BAB 268
#define ET_BAB 269
#define XOR_BAB 270
#define DEC_DROITE 271
#define DEC_GAUCHE 272
#define MODULO 273
#define PARENTHESE_GAUCHE 274
#define PARENTHESE_DROITE 275
#define FIN 276
#define NEG 277




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE IntEval_lval;


