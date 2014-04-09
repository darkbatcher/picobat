%{

#include "global.h"
#include "parser_sym.h"
#include "parser.y.h"

#include <stdlib.h>

#define YY_INPUT( buffer, result, max_size ) \
{ result = IntEval_InputFromString(buffer, max_size); }

static int IntEval_InputFromString(char* buffer, int max_size);

%}

%option noyywrap

blancs    [ \t]+

chiffre   [0-9]
entier    ((0x)?[0-9a-fA-F]+)|({chiffre}+)
caractere [_a-zA-Z]
variable  [_a-zA-Z]+[_a-zA-Z0-9]*

%%

{blancs}  { /* On ignore */ }

{variable} {
		char* lpReturn;
		
		if ((lpReturn=getenv(yytext))) {
			IntEval_lval=atoi(lpReturn);
		} else {
			IntEval_lval=0;
		}
		
		return(VARIABLE);
	}

{entier}    {
      IntEval_lval=strtol(yytext, NULL, 0);
      return(NOMBRE);
    }

"+"   return(PLUS);
"-"   return(MOINS);

"*"   return(FOIS);
"/"   return(DIVISE);
"%"   return(MODULO);

"("   return(PARENTHESE_GAUCHE);
")"   return(PARENTHESE_DROITE);

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
