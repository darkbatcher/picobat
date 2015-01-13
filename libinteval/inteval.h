#ifndef INTEVAL_H
#define INTEVAL_H

#define INTEVAL_NOERROR 0
#define INTEVAL_ALLOCERROR 1
#define INTEVAL_SYNTAXERROR 2

int IntEval_Eval(const char* lpLine, int(*fn)(const char*));

extern int IntEval_Error;
extern int(*Inteval_GetVar)(const char*);

#endif // INTEVAL_H

