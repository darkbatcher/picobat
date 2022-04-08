/*
 * Copyright (C) 1999, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2011,
 * 2012, 2013, 2014 Free Software Foundation, Inc.
 * Copyright (C) 2022 Astie Teddy and pBat developers.
 *
 * This file is part of pBat libfasteval based on libmatheval
 *
 * pBat libfasteval is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * pBat libfasteval is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with pBat libfasteval.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

%{
/*
 * Copyright (C) 1999, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2011,
 * 2012, 2013, 2014 Free Software Foundation, Inc.
 * Copyright (C) 2022 Astie Teddy and pBat developers.
 *
 * This file is part of pBat libfasteval
 *
 * pBat libfasteval is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * pBat libfasteval is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with pBat libfasteval.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include "parser.h"

/* Redefine macro to redirect scanner input from string instead of
 * standard input.  */
#define YY_INPUT( buffer, result, max_size ) \
{ result = input_from_string (buffer, max_size); }

/* Variables used to communicate with code using scanner.  */
extern _Thread_local char *input_string; /* String representing function.  */
extern double (*get_var)(const char *);
extern double (*set_var)(char *, double);

double evaluate_function(const char *name, double arg);
double lookup_variable(char *name);

/* Read next max_size character from string into buffer.  */
static int input_from_string (char *buffer, int max_size);
%}

/* do not include the default yywrap, as long as it is not needed */
%option noyywrap

/* Token definitions.  */
whitespace [ \t]+
digit [0-9]
integer ((0x[0-9a-fA-F]+)|{digit}+)
float ({digit}+\.{digit}*|{digit}*\.{digit}+)([Ee][+\-]?{digit}+)?
constant "e"|"log2e"|"log10e"|"ln2"|"ln10"|"pi"|"pi_2"|"pi_4"|"1_pi"|"2_pi"|"2_sqrtpi"|"sqrt2"|"sqrt1_2"
function "exp"|"log"|"sqrt"|"sin"|"cos"|"tan"|"cot"|"sec"|"csc"|"asin"|"acos"|"atan"|"acot"|"asec"|"acsc"|"sinh"|"cosh"|"tanh"|"coth"|"sech"|"csch"|"asinh"|"acosh"|"atanh"|"acoth"|"asech"|"acsch"|"abs"|"step"|"delta"|"nandelta"|"erf"
nodelim	   [^+\-*%/&>~<%\|=()^\n\t ]
nodelimnb  [^+\-*%/&>~<%\|=()^\n\t 0-9]
identifier {nodelim}*{nodelimnb}*{nodelim}*
%%

{whitespace}

{integer} {
  evaluator_lval.number.is_double = 0;
  evaluator_lval.number.value.i = strtoll(yytext, NULL, 0);
  return NUMBER;
}

{float} {
  evaluator_lval.number.is_double = 1;
  evaluator_lval.number.value.d = strtod(yytext, NULL);
  return NUMBER;
}

{constant} {
  evaluator_lval.number.is_double = 1;
  evaluator_lval.number.value.d = lookup_variable(yytext);
  return NUMBER;
}

{function} {
  /* Find symbol table record corresponding to function name.  */
  evaluator_lval.name = strdup(yytext);
  return FUNCTION;
}

{identifier} {
  evaluator_lval.name = strdup(yytext);
  return VARIABLE;
}

"~" {
		return '~';
}

">>" {
  return '>';
}

"<<" {
  return '<';
}

"&" {
  return '&';
}

"&&" {
  return 'a';
}

"||" {
  return 'o';
}

"|" {
  return '|';
}

"^" {
  return '^';
}


"+" {
  return '+';
}

"-" {
  return '-';
}

"*" {
  return '*';
}

"/" {
  return '/';
}

"%" {
  return '%';
}

"(" {
  return '(';
}

")" {
  return ')';
}

"=" {
  return '=';
}

"\n" {
  return '\n';
}

%%

static int input_from_string (char *buffer, int max_size)
{
  int count; /* Count of characters to copy from input string to buffer.  */

  /* Calculate count of characters to copy.  */
  count = strlen (input_string);
  if (count > max_size)
          count = max_size;

  /* Perform copy operation and update input string.  */
  memcpy(buffer, input_string, count);
  input_string += count;

  return count;
}

void input_reset()
{
  /* Forget contents of input buffer.  */
  YY_FLUSH_BUFFER;
}
