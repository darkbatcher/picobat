#ifndef GLOBAL_H
#define GLOBAL_H

#define	INTEVAL_STYPE int
extern INTEVAL_STYPE IntEval_lval;

extern char* IntEval_String;
extern int   IntEval_Result;
extern int	 IntEval_Error;

void IntEval_InputReset();

#define INTEVAL_NOERROR 0
#define INTEVAL_ALLOCERROR 1
#define INTEVAL_SYNTAXERROR 2

#endif // GLOBAL_H
