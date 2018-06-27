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

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(WIN32)
#include <sys/wait.h>
#include <strings.h>
#endif

#include <libDos9.h>

#include "Dos9_Start.h"
#include "../core/Dos9_Core.h"

#include "../errors/Dos9_Errors.h"
#include "../lang/Dos9_ShowHelp.h"

/*

	start [/W[ait]] [/MIN] [/MAX] [/b] [/D dir] title cmd param

	Starts a command in a separate window. This is by far the
	most incompatible command in the batch language. Indeed,
	while it is quite straightforward under windows, this may
    be a pity to implement under unices that possess a
    variety of windows manager. To work around this issue, we are
    currently using the xdg-open script if available.

    - /W[ait] : Waits for the process

    - /Min : Minimized window (only under windows)

    - /Max : Maximized window (only under windows)

    - /b : background mode

    - /D dir : new window current directory

    - title : new window title (unused by Dos9)

    - cmd : the command

    - param : the parameters

 */

void Dos9_StartCommandExec(struct pipe_launch_data_t* data)
{
    bIgnoreExit = TRUE;


    Dos9_RunLine(data->str);

    Dos9_EsFree(data->str);

    /* don't forget to free unneeded memory */
    free(data);
}


int Dos9_StartCommandBackground(char* cmdline, int wait)
{
    struct pipe_launch_data_t* data;
    THREAD handle;
    void* ret;

    if ((data = malloc(sizeof(struct pipe_launch_data_t))) == NULL)
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION,
                                __FILE__ "/Dos9_StartCommandBackground()",
                                -1);

    data->str = Dos9_EsInit();
    Dos9_EsCat(data->str, cmdline);

    handle = Dos9_CloneInstance(Dos9_StartCommandExec, data);

    if (wait)
        Dos9_WaitForThread(&handle, &ret);
    Dos9_CloseThread(&handle);

    return 0;
}

int Dos9_CmdStart(char* line)
{
	PARAMLIST *list = NULL, *item;
    EXECINFO info;

    char buf[FILENAME_MAX], ext[FILENAME_MAX];

    int n, status = 0,
        quotes = 0,
        command = 0;

    char **args = NULL,
         *next,
         *backtrack = NULL;
    void* p;

    ESTR *cmdline = Dos9_EsInit(),
         *param = Dos9_EsInit(),
         *dir = NULL,
         *title = NULL;

    info.title = NULL;
    info.dir = NULL;
    info.file = NULL;
    info.flags = DOS9_EXEC_SEPARATE_WINDOW;

    line +=5;

    while (next = Dos9_GetNextParameterEs(line, param)) {

        if (!stricmp(param->str, "/WAIT")
            || !stricmp(param->str, "/W")) {

            info.flags |= DOS9_EXEC_WAIT;

        } else if (!stricmp(param->str, "/B")) {

            info.flags &= ~DOS9_EXEC_SEPARATE_WINDOW;

        } else if (!stricmp(param->str, "/NORMAL")) {

        } else if (!stricmp(param->str, "/D")) {

            line = next;

            if (info.dir) {

                Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, param->str, 0);
                status = DOS9_UNEXPECTED_ELEMENT;

                goto error;
            }

            if ((next = Dos9_GetNextParameterEs(line, param)) == NULL) {

                Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "START", 0);
                status = DOS9_EXPECTED_MORE;

                goto error;
            }

            info.dir = Dos9_EsToFullPath(param);
            dir = param;
            param = Dos9_EsInit();

        } else if (info.file == NULL) {

            /* WOW... this might as well be the title or the real file
               to run ... Well try to guess using some tests */

            if (*(Dos9_SkipBlanks(line)) == '"'
                && (info.title == NULL))
                quotes = 1;
            else
                quotes = 0;

            if (quotes) {

                /* Starts with a quote... must be the title*/
                info.title = param->str;
                backtrack = line;
                title = param;
                param = Dos9_EsInit();

            } else if ((Dos9_GetCommandProc(param->str,
                                            lpclCommands, &p) != -1)) {

                if (!(info.flags & DOS9_EXEC_SEPARATE_WINDOW)) {

                    command = 1;

                    snprintf(buf, sizeof(buf), "%s", param->str);
                    info.file = buf;
                    Dos9_EsCpy(param, line);

                } else {

                    Dos9_GetExeFilename(buf, sizeof(buf));
                    info.file = buf;
                    Dos9_EsCpy(param, buf);
                    Dos9_EsCat(param, " /a:q");

                    if (!bEchoOn)
                        Dos9_EsCat(param, "e");

                    if (bUseFloats)
                        Dos9_EsCat(param, "f");

                    if (bCmdlyCorrect)
                        Dos9_EsCat(param, "c");

                    Dos9_EsCat(param, " /C ");
                    Dos9_EsCat(param, line);

                }

                break;

            } else if (*(param->str)
                        && *(param->str + 1) != ':'
                        && strchr(param->str, ':')) {

                /* the path contains an URI */
                snprintf(buf, sizeof(buf), "%s", param->str);

                info.file = buf;
                Dos9_EsCpy(param, line);
                break;

            } else {

                /* this is apparently a file, set it as the file name */

                if (Dos9_GetFilePath(buf, param->str, sizeof(buf)) == -1)
                    Dos9_MakeFullPath(buf, param->str, sizeof(buf));

                /* do something if its a *.bat */
                Dos9_SplitPath(buf, NULL, NULL, NULL, ext);

                if (!stricmp(ext, ".bat")
                    || !stricmp(ext, ".cmd")) {

                    Dos9_GetExeFilename(buf, sizeof(buf));
                    info.file = buf;
                    Dos9_EsCpy(param, buf);
                    Dos9_EsCat(param, " /a:q");

                    if (!bEchoOn)
                        Dos9_EsCat(param, "e");

                    if (bUseFloats)
                        Dos9_EsCat(param, "f");

                    if (bCmdlyCorrect)
                        Dos9_EsCat(param, "c");

                    Dos9_EsCat(param, " ");
                    Dos9_EsCat(param, line);

                } else {

                    info.file = buf;
                    Dos9_EsCpy(param, line);

                }

                break;
            }

        }

        line = next;

    }

    if (info.file == NULL) {

        if (info.title == NULL) {

            Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "START", 0);
            status = DOS9_EXPECTED_MORE;
            goto error;

        }

        Dos9_GetExeFilename(buf, sizeof(buf));
        info.file = buf;
        Dos9_EsCpy(param, buf);
        Dos9_EsCat(param, " /a:q");

        if (!bEchoOn)
            Dos9_EsCat(param, "e");

        if (bUseFloats)
            Dos9_EsCat(param, "f");

        if (bCmdlyCorrect)
            Dos9_EsCat(param, "c");

    }

    Dos9_GetEndOfLine(param->str, cmdline);
    info.cmdline = Dos9_SkipBlanks(cmdline->str);

    if ((list = Dos9_GetParamList(param->str)) == NULL) {

        Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "START", 0);
        status = DOS9_EXPECTED_MORE;
        goto error;

    }

    n = 0;
    item = list;
    while (item) {
        n++;
        item = item->next;
    }

    if ((args = malloc(sizeof(char*)*(n + 1))) == NULL) {

        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION,
                                __FILE__ "/Dos9_CmdStart()",
                                0);
        status = DOS9_FAILED_ALLOCATION;
        goto error;

    }

    item = list;
    n = 0;
    while (item) {
        args[n ++] = item->param->str;
        item = item->next;
    }

    args[n] = NULL;

    info.args = args;
    if (info.dir == NULL)
        info.dir = lpCurrentDir;

    if (command)
        status = Dos9_StartCommandBackground(info.cmdline,
                                             info.flags & DOS9_EXEC_WAIT);
    else
        status = Dos9_ExecuteFile(&info);


error:
    Dos9_EsFree(cmdline);
    Dos9_EsFree(param);

    if (dir)
        Dos9_EsFree(dir);

    if (title)
        Dos9_EsFree(title);

    if (list)
        Dos9_FreeParamList(list);

    if (args)
        free(args);

    return status;
}
