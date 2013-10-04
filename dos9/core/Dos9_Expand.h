#ifndef DOS9_EXPAND_H
#define DOS9_EXPAND_H

int Dos9_ExpandLocalVar(ESTR* ptrCommandLine);
int Dos9_DelayedExpand(ESTR* ptrCommandLine, char cEnableDelayedExpansion);

void  Dos9_ExpandInit(void);
void Dos9_ExpandEnd(void);
void  Dos9_RemoveEscapeChar(char* lpLine);
char* Dos9_StrToken(char* lpString, char cToken);

#endif
