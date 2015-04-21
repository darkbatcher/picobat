#ifndef DOS9_EXPAND_H
#define DOS9_EXPAND_H


void Dos9_ReplaceVars(ESTR* lpEsStr);
void Dos9_ExpandSpecialVar(ESTR* ptrCommandLine, ESTR** buf);
void Dos9_ExpandVar(ESTR* ptrCommandLine, char cDelimiter, ESTR** buf);
void Dos9_DelayedExpand(ESTR* ptrCommandLine, char cEnableDelayedExpansion);

void  Dos9_RemoveEscapeChar(char* lpLine);
char* Dos9_StrToken(char* lpString, char cToken);

#endif
