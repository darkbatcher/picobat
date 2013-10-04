#ifndef CONSOLE_EXP_H_INCLUDE
#define CONSOLE_EXP_H_INCLUDE

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

typedef char*(*LPEVALFUNC)(const char*);

typedef union EVALRES {
    int iResult;
    double dResult;
} EVALRES,*LPEVALRES;

int Dos9_EvalMath(const char* ptrExpression, LPEVALRES lpResult, LPEVALFUNC lpFunct, char mode);

int _Dos9_EvalSubExpI(const char* ptrExpression, LPEVALFUNC lpFunct, int* iError);
int _Dos9_CalculateSubExpI(const char* lpExp, LPEVALFUNC lpFunct, int* iError);
double _Dos9_EvalSubExpD(const char* ptrExpression, LPEVALFUNC lpFunct, int* iError);
double _Dos9_CalculateSubExpD(const char* lpExp, LPEVALFUNC lpFunct, int* iError);

#define DOS9_EVALMATH_INTEGERS 0
#define DOS9_EVALMATH_FLOATS 1
#define DOS9_EVALMATH_OUT_OF_RANGE 1
#define DOS9_EVALMATH_DIVIDE_BY_0 2

#endif
