%{
/*
 *
 *   libinteval, a lib for evaluating integer expression
 *   Copyright (C) 2010-2016 Romain GARBI
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "global.h"
#include "parser.h"

%}

%token  NOMBRE VARIABLE
%token  PLUS  MOINS FOIS  DIVISE NON_LOG NON_BAB OU_LOG ET_LOG OU_BAB ET_BAB XOR_BAB  DEC_DROITE DEC_GAUCHE MODULO
%token  PARENTHESE_GAUCHE PARENTHESE_DROITE
%token  FIN

%left OU_LOG OU_BAB
%left XOR_BAB
%left ET_LOG ET_BAB
%left DEC_GAUCHE DEC_DROITE

%left PLUS  MOINS
%left FOIS  DIVISE MODULO
%left EGAL
%left NEG NON_LOG NON_BAB

%start Input
%%

Input:
    /* Vide */
  | Input Ligne
  ;

Ligne:
    FIN
  | Expression FIN    { IntEval_Result=IntEval_GetValue($$); }
  ;

Expression:
    NOMBRE      { $$=IntEval_MakeString(IntEval_GetValue($1)); }
  | VARIABLE    { $$=IntEval_MakeString(IntEval_GetValue($1)); }
  | Expression PLUS Expression  { $$=IntEval_MakeString(IntEval_GetValue($1) + IntEval_GetValue($3)); }
  | Expression MOINS Expression { $$=IntEval_MakeString(IntEval_GetValue($1) - IntEval_GetValue($3)); }
  | Expression FOIS Expression  { $$=IntEval_MakeString(IntEval_GetValue($1) * IntEval_GetValue($3)); }
  | Expression DIVISE Expression { $$=IntEval_MakeString(IntEval_GetValue($1) / IntEval_GetValue($3)); }
  | Expression MODULO Expression { $$=IntEval_MakeString(IntEval_GetValue($1) % IntEval_GetValue($3)); }
  | MOINS Expression %prec NEG  { $$=IntEval_MakeString(-IntEval_GetValue($2)); }
  | PARENTHESE_GAUCHE Expression PARENTHESE_DROITE  { $$=IntEval_MakeString(IntEval_GetValue($2)); }
  | NON_LOG Expression %prec NEG { $$=IntEval_MakeString(!IntEval_GetValue($2)); }
  | Expression OU_LOG Expression {$$=IntEval_MakeString(IntEval_GetValue($1) || IntEval_GetValue($3));}
  | Expression ET_LOG Expression {$$=IntEval_MakeString(IntEval_GetValue($1) && IntEval_GetValue($3));}
  | Expression EGAL Expression {$$=IntEval_MakeString(IntEval_Set($1,IntEval_GetValue($3)));}
  | NON_BAB Expression %prec NEG { $$=IntEval_MakeString(~IntEval_GetValue($2));}
  | Expression OU_BAB Expression {$$=IntEval_MakeString(IntEval_GetValue($1) | IntEval_GetValue($3));}
  | Expression ET_BAB Expression {$$=IntEval_MakeString(IntEval_GetValue($1) & IntEval_GetValue($3));}
  | Expression XOR_BAB Expression {$$=IntEval_MakeString(IntEval_GetValue($1) ^ IntEval_GetValue($3));}
  | Expression DEC_GAUCHE Expression {$$=IntEval_MakeString(IntEval_GetValue($1) << IntEval_GetValue($3));}
  | Expression DEC_DROITE Expression {$$=IntEval_MakeString(IntEval_GetValue($1) >> IntEval_GetValue($3));}
  ;

%%

char* IntEval_String;
int   IntEval_Result;

int yyerror(char *s) {
  IntEval_Error=INTEVAL_SYNTAXERROR;
}
