/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2018 Romain GARBI
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libdos9.h>

#include "Dos9_Ver.h"
#include "../core/Dos9_Core.h"
#include "../lang/Dos9_ShowHelp.h"

#if defined (__linux__)
#include <sys/utsname.h>
#endif

int Dos9_CmdVer(char* lpArg)
{
	char BuffChar[4];
	lpArg +=3;
	
	if ((int) strlen(lpArg) > 0) {
		
		Dos9_GetNextParameter(lpArg, BuffChar, 4);
		
		if (!strcmpi(BuffChar, "/?"))
		{
			Dos9_ShowInternalHelp(DOS9_HELP_VER);
			return 0;
		}
	}
	
	
	#ifdef WIN32
	system("ver");
	#elif defined (__linux__)
	struct utsname DATA;
	uname(&DATA);
	printf("%s [Version %s]\n", DATA.sysname, DATA.version);
	#else
	printf("%s [Version : unknown]\n", DOS9_OS);
	#endif
	return 0;
}