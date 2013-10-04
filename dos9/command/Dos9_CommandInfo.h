#ifndef DOS9_COMMANDINFO_H
#define DOS9_COMMANDINFO_H

#include "Dos9_ScriptCommands.h"
#include "Dos9_FileCommands.h"
#include "Dos9_Conditions.h"

#define DOS9_COMMANDINFO_DEXPANDALL 1

COMMANDINFO lpCmdInfo[]={
                            {"ECHO ", Dos9_CmdEcho, 0},
                            {"ECHO.", Dos9_CmdEcho, 0},
                            {"EXIT ", Dos9_CmdExit, 0},
                            {"PAUSE ", Dos9_CmdPause, 0},
                            {"SET ", Dos9_CmdSet, 0},
                            {"SETLOCAL ", Dos9_CmdSetLocal, 0},
                            {"HELP ", Dos9_CmdHelp, 0},
                            {"REM ", Dos9_CmdRem, 0},
                            {"CLS ", Dos9_CmdCls, 0},
                            {"COLOR ", Dos9_CmdColor, 0},
                            {"TITLE ", Dos9_CmdTitle, 0},
                            {"TYPE ", Dos9_CmdType, 0},
                            {"MORE ", Dos9_CmdType, 0},
                            {"GOTO ", Dos9_CmdGoto, 0},
                            {"GOTO:", Dos9_CmdGoto, 0},
                            {"CD ", Dos9_CmdCd, 0},
                            {"CHDIR ", Dos9_CmdCd, 0},
                            {"DIR ", Dos9_CmdDir, 0},
                            {"IF ", Dos9_CmdIf, 0},
                            {"DEL", Dos9_CmdDel, 0},
                            {"ERASE", Dos9_CmdDel, 0},
                            {"REN", Dos9_CmdRen, 0},
                            {"RENAME", Dos9_CmdRen, 0},
                            {"MOVE", Dos9_CmdMove, 0},
                            {"MD", Dos9_CmdMkdir, 0},
                            {"MKDIR", Dos9_CmdMkdir, 0},
                            {"RD", Dos9_CmdRmdir, 0},
                            {"RMDIR", Dos9_CmdRmdir, 0},
                            {"COPY", Dos9_CmdCopy, 0}
                        };

#endif
