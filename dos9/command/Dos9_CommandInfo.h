#ifndef DOS9_COMMANDINFO_H
#define DOS9_COMMANDINFO_H

#include "Dos9_ScriptCommands.h"
#include "Dos9_FileCommands.h"
#include "Dos9_Conditions.h"
#include "Dos9_For.h"

#define STRLEN(str) (sizeof(str)-1)

/* the flag paramater detemines wether a space should be
   searched */

COMMANDINFO lpCmdInfo[]={
                            {"ECHO", Dos9_CmdEcho, 0},
                            {"EXIT", Dos9_CmdExit, STRLEN("EXIT")},
                            {"PAUSE", Dos9_CmdPause, STRLEN("PAUSE")},
                            {"SET", Dos9_CmdSet, STRLEN("SET")},
                            {"SETLOCAL", Dos9_CmdSetLocal, STRLEN("SETLOCAL")},
                            {"HELP", Dos9_CmdHelp, STRLEN("HELP")},
                            {"REM", Dos9_CmdRem, STRLEN("REM")},
                            {"CLS", Dos9_CmdCls, STRLEN("CLS")},
                            {"COLOR", Dos9_CmdColor, STRLEN("COLOR")},
                            {"TITLE", Dos9_CmdTitle, STRLEN("TITLE")},
                            {"TYPE", Dos9_CmdType, STRLEN("TYPE")},
                            {"MORE", Dos9_CmdType, STRLEN("MORE")},
                            {"GOTO", Dos9_CmdGoto, STRLEN("GOTO")},
                            {"GOTO:", Dos9_CmdGoto, 0},
                            {"CD", Dos9_CmdCd, STRLEN("CD")},
                            {"CHDIR", Dos9_CmdCd, STRLEN("CHDIR")},
                            {"DIR", Dos9_CmdDir, STRLEN("DIR")},
                            {"IF", Dos9_CmdIf, STRLEN("IF")},
                            {"DEL", Dos9_CmdDel, STRLEN("DEL")},
                            {"ERASE", Dos9_CmdDel, STRLEN("ERASE")},
                            {"REN", Dos9_CmdRen, STRLEN("REN")},
                            {"RENAME", Dos9_CmdRen, STRLEN("RENAME")},
                            {"MOVE", Dos9_CmdMove, STRLEN("MOVE")},
                            {"MD", Dos9_CmdMkdir, STRLEN("MD")},
                            {"MKDIR", Dos9_CmdMkdir, STRLEN("MKDIR")},
                            {"RD", Dos9_CmdRmdir, STRLEN("RD")},
                            {"RMDIR", Dos9_CmdRmdir, STRLEN("RMDIR")},
                            {"COPY", Dos9_CmdCopy, STRLEN("COPY")},
                            {"FOR", Dos9_CmdFor, STRLEN("FOR")},
                            {"(", Dos9_CmdBlock, 0}
                        };

#endif
