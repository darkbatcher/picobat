#include <stdio.h>
#include <stdlib.h>

#include "Dos9_Exp.h"

int main(void)
{
    char pStr[]="143+1+200000000000000000000000000000000";
    EVALRES evResult;

    switch (Dos9_EvalMath(pStr, &evResult, &getenv, 0)) {
        case DOS9_EVALMATH_DIVIDE_BY_0:
            puts("Error -> divided by 0");
            break;
        case DOS9_EVALMATH_OUT_OF_RANGE:
            puts("Error -> out of range");
            break;
        default:
            printf("%s=%d\n", pStr, evResult.iResult);
    }
    return 0;
}
