/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
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

#include <libpBat.h>

#include "pBat_Ver.h"
#include "../core/pBat_Core.h"
#include "../lang/pBat_ShowHelp.h"

#ifndef WIN32
#include <sys/utsname.h>
#else
#include <windows.h>
#endif

int pBat_CmdVer(char* lpArg)
{
	char BuffChar[3];
	lpArg +=3;

	if (strlen(lpArg) > 0) {

		pBat_GetNextParameter(lpArg, BuffChar, 3);

		if (!stricmp(BuffChar, "/?"))
		{
			pBat_ShowInternalHelp(PBAT_HELP_VER);
			return 0;
		}
	}


	#ifdef WIN32

        OSVERSIONINFOA DATA;
        const char *CurrentWindowsVersion;
        DATA.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

        GetVersionEx(&DATA);
        pBat_CmdVerCheckWinVer(&CurrentWindowsVersion,
                                (int) DATA.dwMajorVersion,
                                (int) DATA.dwMinorVersion);

        fprintf(fOutput, PBAT_NL "Microsoft Windows %s %s [version %d.%d.%d]" PBAT_NL,
                    CurrentWindowsVersion, (char*) DATA.szCSDVersion,
                    (int) DATA.dwMajorVersion, (int) DATA.dwMinorVersion,
                    (int) DATA.dwBuildNumber);

	#else
        struct utsname DATA;

        uname(&DATA);
        fprintf(fOutput, PBAT_NL "%s [version %s]" PBAT_NL, DATA.sysname, DATA.release);

	#endif
	return 0;
}

#ifdef WIN32
void pBat_CmdVerCheckWinVer(char **WindowsVersion, int MajorVersion, int MinorVersion)
{
  switch(MajorVersion) {
		case 10:
			*WindowsVersion = "10";
			break;

		case 6:
			switch(MinorVersion) {
				case 3:
					*WindowsVersion = "8.1";
					break;

				case 2:
					*WindowsVersion = "8 or greater";
					break;

				case 1:
					*WindowsVersion = "7";
					break;

				default:
					*WindowsVersion = "Vista";
					break;
			}
			break;

		case 5:
			switch(MinorVersion) {
				case 2:
					*WindowsVersion = "Server 2003";
					break;

				case 1:
					*WindowsVersion = "XP";
					break;

				default:
					*WindowsVersion = "2000";
					break;
			}
			break;

		default:
			*WindowsVersion = "Unknown";
			break;
  }
  return;
}
#endif
