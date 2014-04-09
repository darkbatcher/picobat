#ifndef DOS9_EXPAND_H
#define DOS9_EXPAND_H

#define Dos9_ReplaceVars(lpEsStr) Dos9_ExpandVar(lpEsStr, '%')

void Dos9_ExpandSpecialVar(ESTR* ptrCommandLine);
void Dos9_ExpandVar(ESTR* ptrCommandLine, char cDelimiter);
void Dos9_DelayedExpand(ESTR* ptrCommandLine, char cEnableDelayedExpansion);

void  Dos9_RemoveEscapeChar(char* lpLine);
char* Dos9_StrToken(char* lpString, char cToken);

#endif
