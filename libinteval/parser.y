%{


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
%left NEG NON_LOG NON_BAB

%start Input
%%

Input:
    /* Vide */
  | Input Ligne
  ;

Ligne:
    FIN
  | Expression FIN    { IntEval_Result=$$; }
  ;

Expression:
    NOMBRE      { $$=$1; }
  | VARIABLE    { $$=$1; }
  | Expression PLUS Expression  { $$=$1+$3; }
  | Expression MOINS Expression { $$=$1-$3; }
  | Expression FOIS Expression  { $$=$1*$3; }
  | Expression DIVISE Expression { $$=$1/$3; }
  | Expression MODULO Expression { $$=$1 % $3;}
  | MOINS Expression %prec NEG  { $$=-$2; }
  | PARENTHESE_GAUCHE Expression PARENTHESE_DROITE  { $$=$2; }
  | NON_LOG Expression %prec NEG {$$=!$2; }
  | Expression OU_LOG Expression {$$=$1 || $3;}
  | Expression ET_LOG Expression {$$=$1 && $3;}
  | NON_BAB Expression %prec NEG {$$=~$2;}
  | Expression OU_BAB Expression {$$=$1 | $3;}
  | Expression ET_BAB Expression {$$=$1 & $3;}
  | Expression XOR_BAB Expression {$$=$1 ^ $3;}
  | Expression DEC_GAUCHE Expression {$$= $1 << $3;}
  | Expression DEC_DROITE Expression {$$= $1 >> $3;}
  ;

%%

char* IntEval_String;
int   IntEval_Result;

int yyerror(char *s) {
  IntEval_Error=INTEVAL_SYNTAXERROR;
}
