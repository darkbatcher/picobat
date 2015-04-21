/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2015 DarkBatcher
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

#include <libDos9.h>

#include "Dos9_Start.h"
#include "../core/Dos9_Core.h"

#include "../errors/Dos9_Errors.h"
#include "../lang/Dos9_ShowHelp.h"

/*

	start [/Wait] [/MIN] [/MAX] [/D dir] cmd param

 */

#define START_MODE_MAX  3
#define START_MODE_MIN  2
#define START_MODE_NONE 5

#if defined(WIN32)

#include <windows.h>

int Dos9_StartFile(const char* file, const char* args, const char* dir,
					int mode, int wait)
{
	SHELLEXECUTEINFO info;

	memset(&info, 0, sizeof(info));
	info.cbSize = sizeof(info);
	info.fMask = SEE_MASK_INVOKEIDLIST;
	info.lpVerb = "open";
	info.lpFile = file;
	info.lpParameters = args;
	info.lpDirectory = dir;
	info.nShow = mode;

	ShellExecuteEx(&info);

	if (wait)
		WaitForSingleObject(info.hProcess, INFINITE);

	CloseHandle(info.hProcess);

	return 0;
}

#elif defined(XDG_OPEN) && defined(_POSIX_C_SOURCE)

int Dos9_StartFile(const char* file, const char* args, const char* dir,
					int mode, int wait)
{
    pid_t pid;

    pid = fork();

    ESTR* tmp;
    char* arg[FILENAME_MAX];
    int status, i;

    if (pid == 0) {

        /* we are in the son */
        if (dir && chdir(dir) == -1) {
            Dos9_ShowErrorMessage(DOS9_DIRECTORY_ERROR | DOS9_PRINT_C_ERROR,
                                    dir,
                                    FALSE);
            exit(-1);
        }

        arg[0] = XDG_OPEN;
        arg[1] = file;

        i=2;

        while ((i < (FILENAME_MAX-1))
               && (args = Dos9_GetNextParameterEs(args, tmp=Dos9_EsInit()))) {
            arg[i++] = Dos9_EsToChar(tmp);
        }

        arg[i] = NULL;

        Dos9_OpenOutput(lppsStreamStack, "NULL", DOS9_STDOUT | DOS9_STDERR, 0);

        if (execvp(XDG_OPEN, arg) == -1) {
            Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR | DOS9_PRINT_C_ERROR,
                                    XDG_OPEN,
                                    FALSE);
            exit(-1);
        }

    } else if (pid == -1) {

        Dos9_ShowErrorMessage(DOS9_FAILED_FORK | DOS9_PRINT_C_ERROR,
                        __FILE__ "/Dos9_StartFile()",
                        FALSE);
        return -1;

    } else {

        if (wait)
            waitpid(pid, &status, 0);

    }

    return 0;

}

#elif defined _POSIX_C_SOURCE

int Dos9_StartFile(const char* file, const char* args, const char* dir,
					int mode, int wait)
{
    pid_t pid;

    pid = fork();

    ESTR* tmp;
    char* arg[FILENAME_MAX];
    int status, i;

    if (pid == 0) {

        Dos9_OpenOutput(lppsStreamStack, "NULL", DOS9_STDOUT | DOS9_STDERR, 0);

        /* we are in the son */
        if (dir && chdir(dir) == -1)
            Dos9_ShowErrorMessage(DOS9_DIRECTORY_ERROR | DOS9_PRINT_C_ERROR,
                                    dir,
                                    1);

        arg[0] = file;
        i=1;

        while ((i < (FILENAME_MAX-1))
               && (args = Dos9_GetNextParameterEs(args, tmp=Dos9_EsInit()))) {
            arg[i++] = Dos9_EsToChar(tmp);
        }

        arg[i] = NULL;

        if (execvp(file, arg) == -1)
            Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR | DOS9_PRINT_C_ERROR,
                                    file,
                                    1);

    } else if (pid == -1) {

        Dos9_ShowErrorMessage(DOS9_FAILED_FORK | DOS9_PRINT_C_ERROR,
                        __FILE__ "/Dos9_StartFile()",
                        FALSE);
        return -1;

    } else {

        if (wait)
            waitpid(pid, &status, 0);

    }

    return 0;

}

#endif // defined

void Dos9_UseBackSlash(char* line)
{
	while (line = strchr(line, '/')) *(line ++) = '\\';

}

int Dos9_CmdStart(char* line)
{
	ESTR* param = Dos9_EsInit();

	char wait = FALSE,
		 mode = START_MODE_NONE;

	char dirbuf[FILENAME_MAX],
		 file[FILENAME_MAX],
		 *dir = NULL;

	line += 5;

	while (line = Dos9_GetNextParameterEs(line, param)) {

		if (!stricmp("/wait", Dos9_EsToChar(param))) {

			wait = TRUE;

		} else if (!stricmp("/min", Dos9_EsToChar(param))) {

			mode = START_MODE_MIN;

		} else if (!stricmp("/max", Dos9_EsToChar(param))) {

			mode = START_MODE_MAX;

		} else if (!stricmp("/d", Dos9_EsToChar(param))) {

			if (((line = Dos9_GetNextParameterEs(line, param)) == NULL)
				|| dir != NULL) {

				Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, "/d", NULL);
				goto error;

			}

			strncpy(dirbuf, Dos9_EsToChar(param), sizeof(dirbuf));
			dirbuf[FILENAME_MAX-1]='\0';

			dir = dirbuf;

		} else {

			break;

		}

	}

	if (line == NULL) {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "START", NULL);
		goto error;

	}

	strncpy(file, Dos9_EsToChar(param), sizeof(file));
	file[FILENAME_MAX-1]='\0';

	Dos9_GetEndOfLine(line, param);

#ifdef WIN32
	Dos9_UseBackSlash(line);
#endif // WIN32

	if (Dos9_StartFile(file,
						Dos9_EsToChar(param),
						dir,
						mode,
						wait))
		goto error;

	Dos9_EsFree(param);
	return 0;

	error:

		Dos9_EsFree(param);
		return -1;

}
