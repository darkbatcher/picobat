#ifndef DOS9_FILECOMMANDS_H_INCLUDED
#define DOS9_FILECOMMANDS_H_INCLUDED

#include <time.h>

#include "libDos9.h"
#include "Dos9_CmdLib.h"
#include "../lang/Dos9_Lang.h"

#define DOS9_ASK_CONFIRMATION 0x01
#define DOS9_DELETE_READONLY 0x02
#define DOS9_DONT_ASK_GENERIC 0x04

char* Dos9_StrToken(char* lpString, char cToken);

int Dos9_Dir(char* lpLine);
int Dos9_CmdDel(char* lpLine);
int Dos9_CmdMove(char* lpLine);
int Dos9_CmdCopy(char* lpLine);
int Dos9_CmdRen(char* lpLine);
int Dos9_CmdRmdir(char* lpLine);
int Dos9_CmdMkdir(char* lpLine);

#endif // DOS9_FILECOMMANDS_H_INCLUDED
