#ifndef DOS9_SCRIPTCOMMAND_H
#define DOS9_SCRIPTCOMMAND_H

#include "Dos9_CmdLib.h"

void Dos9_ScriptCommandInit(void);
void Dos9_ScriptCommandEnd(void);

int Dos9_CmdEcho(char* lpLine);
int Dos9_CmdPause(char* lpLine);
int Dos9_CmdSet(char* lpLine);
int Dos9_CmdExit(char* lpLine);
int Dos9_CmdSetLocal(char* lpLine);
int Dos9_CmdHelp(char* lpLine);
int Dos9_CmdRem(char* lpLine);
int Dos9_CmdTitle(char* lpLine);
int Dos9_CmdCls(char* lpLine);
int Dos9_CmdColor(char* lpLine);
int Dos9_CmdGoto(char* lpLine);
int Dos9_CmdType(char* lpLine);
int Dos9_CmdCd(char* lpLine);
int Dos9_CmdDir(char* lpLine);

#endif
