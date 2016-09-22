/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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

#ifndef DOS9_COMMANDINFO_H
#define DOS9_COMMANDINFO_H

/* include the header that defines all function name */
#include "Dos9_Commands.h"

#include "../core/Dos9_Core.h"

#define STRLEN(str) (sizeof(str)-1)


/* the flag paramater detemines wether a space should be
   searched */

COMMANDINFO lpCmdInfo[]= {
	{"ECHO", Dos9_CmdEcho, 0},
	{"EXIT", Dos9_CmdExit, STRLEN("EXIT")},
	{"PAUSE", Dos9_CmdPause, STRLEN("PAUSE")},
	{"SET", Dos9_CmdSet, STRLEN("SET")},
	{"SETLOCAL", Dos9_CmdSetLocal, STRLEN("SETLOCAL")},
	{"HELP", "hlp.bat", STRLEN("HELP") | DOS9_ALIAS_FLAG},
	{"REM", Dos9_CmdRem, STRLEN("REM")},
	{"CLS", Dos9_CmdCls, STRLEN("CLS")},
	{"COLOR", Dos9_CmdColor, STRLEN("COLOR")},
	{"TITLE", Dos9_CmdTitle, STRLEN("TITLE")},
	{"TYPE", Dos9_CmdType, STRLEN("TYPE")},
	{"MORE", Dos9_CmdMore, STRLEN("MORE")},
	{"GOTO", Dos9_CmdGoto, STRLEN("GOTO")},
	{"GOTO:", Dos9_CmdGoto, 0},
	{"START", Dos9_CmdStart, STRLEN("START")},
	{"CD", Dos9_CmdCd, STRLEN("CD")},
	{"CHDIR", Dos9_CmdCd, STRLEN("CHDIR")},
	{"DIR", Dos9_CmdDir, STRLEN("DIR")},
	{"IF", Dos9_CmdIf, STRLEN("IF")},
	{"DEL", Dos9_CmdDel, STRLEN("DEL")},
	{"ERASE", Dos9_CmdDel, STRLEN("ERASE")},
	{"REN", Dos9_CmdRen, STRLEN("REN")},
	{"RENAME", Dos9_CmdRen, STRLEN("RENAME")},
	{"MOVE", Dos9_CmdCopy, STRLEN("MOVE")},
	{"MD", Dos9_CmdMkdir, STRLEN("MD")},
	{"MKDIR", Dos9_CmdMkdir, STRLEN("MKDIR")},
	{"RD", Dos9_CmdRmdir, STRLEN("RD")},
	{"RMDIR", Dos9_CmdRmdir, STRLEN("RMDIR")},
	{"COPY", Dos9_CmdCopy, STRLEN("COPY")},
	{"FOR", Dos9_CmdFor, STRLEN("FOR")},
	{"(", Dos9_CmdBlock, 0},
	{"CALL", Dos9_CmdCall, STRLEN("CALL")},
	{"CALL:", Dos9_CmdCall, 0},
	{"ALIAS", Dos9_CmdAlias, STRLEN("ALIAS")},
	{"SHIFT", Dos9_CmdShift, STRLEN("SHIFT")},
	{"FIND", Dos9_CmdFind, STRLEN("FIND")},
	{"BREAK", Dos9_CmdBreak, STRLEN("BREAK")},
	{"CHCP", Dos9_CmdChcp, STRLEN("CHCP")},
	{"PUSHD", Dos9_CmdPushd, STRLEN("PUSHD")},
	{"POPD", Dos9_CmdPopd, STRLEN("POPD")}
};

#endif
