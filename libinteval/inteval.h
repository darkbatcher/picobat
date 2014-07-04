#ifndef INTEVAL_H
#define INTEVAL_H

#define INTEVAL_NOERROR 0
#define INTEVAL_ALLOCERROR 1
#define INTEVAL_SYNTAXERROR 2

int IntEval_Eval(const char* lpLine);

extern int IntEval_Error;

#endif // INTEVAL_H

