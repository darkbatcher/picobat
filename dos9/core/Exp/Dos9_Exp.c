#include "Dos9_Exp.h"

int Dos9_EvalMath(const char* ptrExpression, LPEVALRES lpResult, LPEVALFUNC lpFunct, char mode)
{
    /* this is a function to parse Dos9 expressions
     */
     int iError=0;

     if (mode==DOS9_EVALMATH_INTEGERS) {
        lpResult->iResult=_Dos9_EvalSubExpI(ptrExpression, lpFunct, &iError);
     } else {
        lpResult->dResult=_Dos9_EvalSubExpD(ptrExpression, lpFunct, &iError);
     }

     return iError;
}

int _Dos9_EvalSubExpI(const char* lpExp, LPEVALFUNC lpFunct, int* iError)
/* this must be perfectly reentrant, indeed it's used recursively */
/* the way that the parsed proceeds :
*/
{
    const char* lpTokenBegin=lpExp;
    char* lpEnd;
    int iSign=1,
        iNextSign,
        iReturn=0,
        iContinue,
        iNbParenthese=0,
        iConv,
        iMainLoop=1;

    if (*lpExp=='-') {
        lpExp++;
        iSign=-1;
    }

    lpTokenBegin=lpExp;

    while (*lpExp && iMainLoop) {

        iContinue=1;
        iNextSign=1;

        while (*lpExp && iContinue) {

            switch (*lpExp) {
                case '-':
                    iNextSign=-1;
                case '+':
                    if (!iNbParenthese) {
                        iContinue=0;
                        continue;
                    }
                    break;

                case '(':
                    iNbParenthese++;
                    break;

                case ')':
                    if (iNbParenthese>0) iNbParenthese--;
                    else {
                            iContinue=0;
                            iMainLoop=0;
                            continue;
                    }
            }

            lpExp++;
        }

        iConv=strtol(lpTokenBegin, &lpEnd, 0);
        if (errno==ERANGE) *iError=1;
        if (lpEnd==lpExp) {
            //printf("Adding %d\n", iSign*iConv);
            iReturn+=iSign*iConv;
        } else {
            iReturn+=iSign*_Dos9_CalculateSubExpI(lpTokenBegin, lpFunct, iError);
        }
        if (*lpExp) {
            lpExp++;
            lpTokenBegin=lpExp;
            iSign=iNextSign;
        }
    }
    return iReturn;
}

int _Dos9_CalculateSubExpI(const char* lpExp, LPEVALFUNC lpFunct, int* iError)
/* these calculate expression */
{
    const char* lpTokenBegin;
    char pBuf[FILENAME_MAX];
    char* lpEnd;
    int iSign=1,
        iNextSign,
        iReturn=1,
        iContinue,
        iNbParenthese=0,
        iConv,
        iMainLoop=1;

    if (*lpExp=='-') {
        lpExp++;
        iReturn=-1;
    }

    lpTokenBegin=lpExp;
    //puts("================ New sub-exp");
    //puts(lpExp);
    //puts("===========================");
    while (*lpExp && iMainLoop) {

        iContinue=1;
        iNextSign=1;

        while (*lpExp && iContinue) {

            switch (*lpExp) {
                case '/':
                    iNextSign=-1;
                case '*':
                    if (!iNbParenthese) {
                        iContinue=0;
                        continue;
                    }
                    break;
                case '%':
                    iNextSign=2;
                    if (!iNbParenthese) {
                        iContinue=0;
                        continue;
                    }
                    break;
                case '(':
                    iNbParenthese++;
                    break;

                case ')':
                    if (iNbParenthese>0) {
                            iNbParenthese--;
                    } else {
                            iContinue=0;
                            iMainLoop=0;
                            continue;
                    }
            }

            lpExp++;
        }
        //printf("[*] Operation %s\tEnd %s\n", lpTokenBegin, lpExp);
        if (*lpTokenBegin=='(') {
            lpTokenBegin++;
            iConv=_Dos9_EvalSubExpI(lpTokenBegin, lpFunct, iError);
        } else {
            iConv=strtol(lpTokenBegin, &lpEnd, 0);
            if (errno==ERANGE) *iError=DOS9_EVALMATH_OUT_OF_RANGE;

            if (lpEnd+1==lpExp && *lpEnd==')') {
                    // nothing to be done
            } else if (lpEnd!=lpExp) {
                strncpy(pBuf, lpTokenBegin, FILENAME_MAX);
                pBuf[FILENAME_MAX-1]='\0';
                iConv=strtol(lpFunct(pBuf), NULL, 0); /* getting value from the function */
                if (errno==ERANGE) *iError=DOS9_EVALMATH_OUT_OF_RANGE;
            }
        }
        switch (iSign) {
            case 1:
                iReturn*=iConv;
                break;
            case -1:
                if (iConv!=0) iReturn/=iConv;
                else *iError=DOS9_EVALMATH_DIVIDE_BY_0;
            case 2:
                iReturn%=iConv;
        }
        //printf("New Operation result: %d\n", iReturn);
        if (*lpExp) {
            lpExp++;
            lpTokenBegin=lpExp;
            iSign=iNextSign;
        }
    }
    return iReturn;
}

/*** ---------------------------------------------------------------------------------------
   *  Float-associated functions
   *
   *  These functions are used to perform float-side parsing
   *
   *  Don't mix the code because those codes are quite difficult to distinguish
   *
   * ----------------------------------------------------------------------------------------
   */


double _Dos9_EvalSubExpD(const char* lpExp, LPEVALFUNC lpFunct, int* iError)
/* this must be perfectly reentrant, indeed it's used recursively */
/* the way that the parsed proceeds :
*/
{
    const char* lpTokenBegin=lpExp;
    char* lpEnd;
    double dSign=1.0, dReturn=0.0, dConv, dNextSign;
    int iContinue,
        iNbParenthese=0,
        iMainLoop=1;

    if (*lpExp=='-') {
        lpExp++;
        dSign=-1.0;
    }

    lpTokenBegin=lpExp;

    while (*lpExp && iMainLoop) {

        iContinue=1;
        dNextSign=1.0;

        while (*lpExp && iContinue) {

            switch (*lpExp) {
                case '-':
                    dNextSign=-1.0;
                case '+':
                    if (!iNbParenthese) {
                        iContinue=0;
                        continue;
                    }
                    break;

                case '(':
                    iNbParenthese++;
                    break;

                case ')':
                    if (iNbParenthese>0) iNbParenthese--;
                    else {
                            iContinue=0;
                            iMainLoop=0;
                            continue;
                    }
            }

            lpExp++;
        }

        dConv=strtod(lpTokenBegin, &lpEnd);
        if (errno==ERANGE) *iError=DOS9_EVALMATH_OUT_OF_RANGE;
        if (lpEnd==lpExp) {
            //printf("Adding %f\n", dSign*dConv);
            dReturn+=dSign*dConv;
        } else {
            dReturn+=dSign*_Dos9_CalculateSubExpD(lpTokenBegin, lpFunct, iError);
        }
        if (*lpExp) {
                lpExp++;
                lpTokenBegin=lpExp;
                dSign=dNextSign;
        }
    }
    return dReturn;
}

double _Dos9_CalculateSubExpD(const char* lpExp, LPEVALFUNC lpFunct, int* iError)
/* these calculate expression */
{
    const char* lpTokenBegin;
    char pBuf[FILENAME_MAX];
    char* lpEnd;

    double dReturn=1.0, dConv;

    int iSign=1,
        iNextSign,
        iContinue,
        iNbParenthese=0,
        iMainLoop=1;

    if (*lpExp=='-') {
        lpExp++;
        dReturn=-1.0;
    }

    lpTokenBegin=lpExp;
    //puts("================ New sub-exp");
    //puts(lpExp);
    //puts("===========================");
    while (*lpExp && iMainLoop) {

        iContinue=1;
        iNextSign=1;

        while (*lpExp && iContinue) {

            switch (*lpExp) {
                case '/':
                    iNextSign=-1;
                case '*':
                    if (!iNbParenthese) {
                        iContinue=0;
                        continue;
                    }
                    break;

                case '(':
                    iNbParenthese++;
                    break;

                case ')':
                    if (iNbParenthese>0) {
                            iNbParenthese--;
                    } else {
                            iContinue=0;
                            iMainLoop=0;
                            continue;
                    }
            }

            lpExp++;
        }
        //printf("[*] Operation %s\tEnd %s\n", lpTokenBegin, lpExp);
        if (*lpTokenBegin=='(') {
            lpTokenBegin++;
            dConv=_Dos9_EvalSubExpD(lpTokenBegin, lpFunct, iError);
        } else {
            dConv=strtod(lpTokenBegin, &lpEnd);
            if (errno==ERANGE) *iError=DOS9_EVALMATH_OUT_OF_RANGE;

            if (lpEnd+1==lpExp && *lpEnd==')') {
                    // nothing to be done
            } else if (lpEnd!=lpExp) {
                strncpy(pBuf, lpTokenBegin, FILENAME_MAX);
                pBuf[FILENAME_MAX-1]='\0';
                dConv=strtod(lpFunct(pBuf), NULL); /* getting value from the function */
                if (errno==ERANGE) *iError=DOS9_EVALMATH_OUT_OF_RANGE;
            }
        }
        switch (iSign) {
            case 1:
                dReturn*=dConv;
                break;
            case -1:
                if (dConv!=0.0) dReturn/=dConv;
                else *iError=DOS9_EVALMATH_DIVIDE_BY_0;
        }
        //printf("New Operation result: %f\n", dReturn);
        if (*lpExp) {
            lpExp++;
            lpTokenBegin=lpExp;
            iSign=iNextSign;
        }
    }
    return dReturn;
}

