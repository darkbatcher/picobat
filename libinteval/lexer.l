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

#include "global.h"
#include "parser.h"

#include <stdlib.h>

#define YY_INPUT( buffer, result, max_size ) \
{ result = IntEval_InputFromString(buffer, max_size); }

static int IntEval_InputFromString(char* buffer, int max_size);

%}

%option noyywrap

blancs    [ \t]+
chiffre   [0-9]
entier    ((0x)?[0-9a-fA-F]+)|({chiffre}+)
variable  [_a-zA-Z][[_a-zA-Z0-9]*

%%

{blancs}  { /* On ignore */ }

{variable} {
		IntEval_lval=yytext;
		return(VARIABLE);
	}

{entier}   {
      IntEval_lval=yytext;
      return(NOMBRE);
    }


"+"   return(PLUS);
"-"   return(MOINS);

"="	  return(EGAL);

"*"   return(FOIS);
"/"   return(DIVISE);
"%"   return(MODULO);

"("   return (PARG);
")"   return (PARD);

"!"   return(NON_LOG);
"\~"  return(NON_BAB);

"||" return(OU_LOG);
"&&" return(ET_LOG); 

"|"  return(OU_BAB);
"&"	 return(ET_BAB);
"\^"  return(XOR_BAB);

">>" return(DEC_DROITE);
"<<" return(DEC_GAUCHE);

"\n"  return(FIN);

%%

static int IntEval_InputFromString(char *buffer, int max_size)
{
        int count; /* Count of characters to copy from input string to buffer.  */

        /* Calculate count of characters to copy.  */
        count = strlen (IntEval_String);
        if (count > max_size)
                count = max_size;

        /* Perform copy operation and update input string.  */
        memcpy(buffer, IntEval_String, count);
        IntEval_String += count;

        return count;
}

void IntEval_InputReset()
{
        /* Forget contents of input buffer.  */
        YY_FLUSH_BUFFER;
}
