#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "inteval.h"
#include "global.h"
#include "parser.h"

int IntEval_Error=INTEVAL_NOERROR;

int IntEval_Eval(const char* lpLine)
{
    char* lpInteval;
	
	if (!(IntEval_String = strdup(lpLine))) {

		IntEval_Error=INTEVAL_ALLOCERROR;
        return 0;

    }
	
	
	lpInteval=IntEval_String;
	
	IntEval_InputReset();
	IntEval_Error=INTEVAL_NOERROR;
	IntEval_Result=0;
	
    IntEval_parse();
	
	free(lpInteval);

    return IntEval_Result;
}
