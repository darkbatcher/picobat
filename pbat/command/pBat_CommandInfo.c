 /*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
 *   Copyright (C) 2010-2016 Romain GARBI
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/* include the header that defines all function name */
#include "pBat_Commands.h"

#include "../core/pBat_Core.h"

#define STRLEN(str) (sizeof(str)-1)


/* the flag paramater detemines wether a space should be
   searched */

COMMANDINFO lpCmdInfo[]= {
	{"ECHO", pBat_CmdEcho, 0},
	{"EXIT", pBat_CmdExit, STRLEN("EXIT")},
	{"PAUSE", pBat_CmdPause, STRLEN("PAUSE")},
	{"SET", pBat_CmdSet, STRLEN("SET")},
	{"SETLOCAL", pBat_CmdSetLocal, STRLEN("SETLOCAL")},
	{"ENDLOCAL", pBat_CmdEndLocal, STRLEN("ENDLOCAL")},
	{"HELP", pBat_CmdHelp, STRLEN("HELP")},
	{"REM", pBat_CmdRem, STRLEN("REM")},
	{"CLS", pBat_CmdCls, STRLEN("CLS")},
	{"COLOR", pBat_CmdColor, STRLEN("COLOR")},
	{"TITLE", pBat_CmdTitle, STRLEN("TITLE")},
	{"TYPE", pBat_CmdType, STRLEN("TYPE")},
	{"MORE", pBat_CmdMore, STRLEN("MORE")},
	{"GOTO", pBat_CmdGoto, STRLEN("GOTO")},
	{"GOTO:", pBat_CmdGoto, 0},
	{"START", pBat_CmdStart, STRLEN("START")},
	{"CD", pBat_CmdCd, STRLEN("CD")},
	{"CHDIR", pBat_CmdCd, STRLEN("CHDIR")},
	{"DIR", pBat_CmdDir, STRLEN("DIR")},
	{"IF", pBat_CmdIf, STRLEN("IF") | PBAT_COMMAND_LOOKAHEAD},
	{"DEL", pBat_CmdDel, STRLEN("DEL")},
	{"ERASE", pBat_CmdDel, STRLEN("ERASE")},
	{"REN", pBat_CmdRen, STRLEN("REN")},
	{"RENAME", pBat_CmdRen, STRLEN("RENAME")},
	{"MOVE", pBat_CmdCopy, STRLEN("MOVE")},
	{"MD", pBat_CmdMkdir, STRLEN("MD")},
	{"MKDIR", pBat_CmdMkdir, STRLEN("MKDIR")},
	{"RD", pBat_CmdRmdir, STRLEN("RD")},
	{"RMDIR", pBat_CmdRmdir, STRLEN("RMDIR")},
	{"COPY", pBat_CmdCopy, STRLEN("COPY")},
	{"FOR", pBat_CmdFor, STRLEN("FOR") | PBAT_COMMAND_LOOKAHEAD},
	{"(", pBat_CmdBlock, 0},
	{"CALL", pBat_CmdCall, STRLEN("CALL")},
	{"CALL:", pBat_CmdCall, 0},
	{"ALIAS", pBat_CmdAlias, STRLEN("ALIAS")},
	{"SHIFT", pBat_CmdShift, STRLEN("SHIFT")},
	{"FIND", pBat_CmdFind, STRLEN("FIND")},
	{"BREAK", pBat_CmdBreak, STRLEN("BREAK")},
	{"CHCP", pBat_CmdChcp, STRLEN("CHCP")},
	{"PUSHD", pBat_CmdPushd, STRLEN("PUSHD")},
	{"POPD", pBat_CmdPopd, STRLEN("POPD")},
	{"WC", pBat_CmdWc, STRLEN("WC")},
	{"XARGS", pBat_CmdXargs, STRLEN("XARGS")},
	{"PROMPT", pBat_CmdPrompt, STRLEN("PROMPT")},
	{"PECHO", pBat_CmdPecho, STRLEN("PECHO")},
	{"TIMEOUT", pBat_CmdTimeout, STRLEN("TIMEOUT")},
	{"MOD", pBat_CmdMod, STRLEN("MOD")},
	{"LOCALE", pBat_CmdLocale, STRLEN("LOCALE")},
	{"VER", pBat_CmdVer, STRLEN("VER")}
};

const int iCmdInfoNb = sizeof(lpCmdInfo)/sizeof(lpCmdInfo[0]);
