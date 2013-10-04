#ifndef HLP_SHOW_DEFINED
#define HLP_SHOW_DEFINED

void Hlp_ShowHeading(char* lpTitle);
void Hlp_ShowPage(HLPPAGE* lpPage, int iLineNb);
void Hlp_ShowLine(HLPELEMENT* lpLine);
void Hlp_GetCommand(char* lpCmd, size_t iSize);
int Hlp_RunCommand(char* lpCmd, char* lpPageName, size_t iNameSize, char* lpTitle, size_t iSize, HLPPAGE** lpPage, HLPLINKS** lpLinks);
void Hlp_MakePath(char* lpNewPath, char* lpOldPath, char* lpRelative, size_t iSize);

int Hlp_GetLine(HLPPAGE* lpPage);
int Hlp_GetLineNb(HLPPAGE* lpPage);

#endif // HLP_SHOW_DEFINED
