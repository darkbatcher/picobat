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

#include <libpBat.h>

#include "pBat_Start.h"
#include "../core/pBat_Core.h"

#include "../errors/pBat_Errors.h"
#include "../lang/pBat_ShowHelp.h"

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

    - title : new window title (unused by pBat)

    - cmd : the command

    - param : the parameters

 */

void pBat_StartCommandExec(struct pipe_launch_data_t* data)
{
    bIgnoreExit = TRUE;


    pBat_RunLine(data->str);

    pBat_EsFree(data->str);

    /* don't forget to free unneeded memory */
    free(data);
}


int pBat_StartCommandBackground(const char* cmdline, int wait)
{
    struct pipe_launch_data_t* data;
    THREAD handle;
    void* ret;

    if ((data = malloc(sizeof(struct pipe_launch_data_t))) == NULL)
        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION,
                                __FILE__ "/pBat_StartCommandBackground()",
                                -1);

    data->str = pBat_EsInit();
    pBat_EsCat(data->str, cmdline);

    handle = pBat_CloneInstance((void(*)(void*))pBat_StartCommandExec, data);

    if (wait)
        pBat_WaitForThread(&handle, &ret);
    pBat_CloseThread(&handle);

    return 0;
}

int pBat_CmdStart(char* line)
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

    ESTR *cmdline = pBat_EsInit_Cached(),
         *param = pBat_EsInit_Cached(),
         *dir = NULL,
         *title = NULL;

    info.title = NULL;
    info.dir = NULL;
    info.file = NULL;
    info.flags = PBAT_EXEC_SEPARATE_WINDOW;

    line +=5;

    while (next = pBat_GetNextParameterEs(line, param)) {

        if (!stricmp(param->str, "/?")) {

            pBat_ShowInternalHelp(PBAT_HELP_START);
            goto end;

        } else if (!stricmp(param->str, "/WAIT")
            || !stricmp(param->str, "/W")) {

            info.flags |= PBAT_EXEC_WAIT;

        } else if (!stricmp(param->str, "/B")) {

            info.flags &= ~PBAT_EXEC_SEPARATE_WINDOW;

        } else if (!stricmp(param->str, "/NORMAL")              /* \        */
                    ||!stricmp(param->str, "/SHARED")           /* |        */
                    || !stricmp(param->str, "/SEPARATE")        /* |        */
                    || !stricmp(param->str, "/HIGH")            /* |        */
                    || !stricmp(param->str, "/LOW")             /* |        */
                    || !stricmp(param->str, "/REALTIME")        /* | Ignore */
                    || !stricmp(param->str, "/ABOVENORMAL")     /* | all    */
                    || !stricmp(param->str, "/BELOWNORMAL")) {  /* | that   */
                                                                /* | shitty */
            /* do nothing */                                    /* | opts   */
        } else if (!stricmp(param->str, "/NODE")                /* |        */
                    || !stricmp(param->str, "/AFFINITY")) {     /* |        */
                                                                /* |        */
            if (next)                                           /* |        */
                next = pBat_GetNextParameterEs(next, param);    /* |        */
                                                                /* |        */
            /* do nothing */                                    /* /        */

        } else if (!stricmp(param->str, "/D")) {

            line = next;

            if (info.dir) {

                pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, param->str, 0);
                status = PBAT_UNEXPECTED_ELEMENT;

                goto error;
            }

            if ((next = pBat_GetNextParameterEs(line, param)) == NULL) {

                pBat_ShowErrorMessage(PBAT_EXPECTED_MORE, "START", 0);
                status = PBAT_EXPECTED_MORE;

                goto error;
            }

            info.dir = pBat_EsToFullPath(param);
            dir = param;
            param = pBat_EsInit_Cached();

        } else if (info.file == NULL) {

            /* WOW... this might as well be the title or the real file
               to run ... Well try to guess using some tests */

            if (*(pBat_SkipBlanks(line)) == '"'
                && (info.title == NULL))
                quotes = 1;
            else
                quotes = 0;

            if (quotes) {

                /* Starts with a quote... must be the title*/
                info.title = param->str;
                backtrack = line;
                title = param;
                param = pBat_EsInit_Cached();

            } else if ((pBat_GetCommandProc(param->str,
                                            lpclCommands, &p) != -1)) {

                if (!(info.flags & PBAT_EXEC_SEPARATE_WINDOW)) {

                    command = 1;

                    snprintf(buf, sizeof(buf), "%s", param->str);
                    info.file = buf;
                    pBat_EsCpy(param, line);

                } else {

                    info.file = lppBatExec;
                    pBat_EsCpy(param, lppBatExec);
                    pBat_EsCat(param, " /a:q");

                    if (!bEchoOn)
                        pBat_EsCat(param, "e");

                    if (bCmdlyCorrect)
                        pBat_EsCat(param, "c");

                    pBat_EsCat(param, " /C ");
                    pBat_EsCat(param, line);

                }

                break;

            } else if (*(param->str)
                        && *(param->str + 1) != ':'
                        && strchr(param->str, ':')) {

                /* the path contains an URI */
                snprintf(buf, sizeof(buf), "%s", param->str);

                info.file = buf;
                pBat_EsCpy(param, line);
                break;

            } else {

                /* this is apparently a file, set it as the file name */

                /* Search %PATH% first, if it is a failure, try to produce an
                   absolute path */
                if (pBat_GetFilePath(buf, param->str, sizeof(buf)) == -1)
                    pBat_MakeFullPath(buf, param->str, sizeof(buf));

                /* do something if its a *.bat */
                pBat_SplitPath(buf, NULL, NULL, NULL, ext);

                if (!stricmp(ext, ".bat")
                    || !stricmp(ext, ".cmd")) {

                    info.file = lppBatExec;
                    pBat_EsCpy(param, lppBatExec);
                    pBat_EsCat(param, " /a:q");

                    if (!bEchoOn)
                        pBat_EsCat(param, "e");

                    if (bCmdlyCorrect)
                        pBat_EsCat(param, "c");

                    pBat_EsCat(param, " ");
                    pBat_EsCat(param, line);

                } else {

                    info.file = buf;
                    pBat_EsCpy(param, line);

                }

                break;
            }

        }

        line = next;

    }

    if (info.file == NULL) {

        if (info.title == NULL) {

            pBat_ShowErrorMessage(PBAT_EXPECTED_MORE, "START", 0);
            status = PBAT_EXPECTED_MORE;
            goto error;

        }

        info.file = lppBatExec;
        pBat_EsCpy(param, lppBatExec);
        pBat_EsCat(param, " /a:q");

        if (!bEchoOn)
            pBat_EsCat(param, "e");

        if (bCmdlyCorrect)
            pBat_EsCat(param, "c");

    }

    pBat_GetEndOfLine(param->str, cmdline);
    info.cmdline = pBat_SkipBlanks(cmdline->str);

    if ((list = pBat_GetParamList(param->str)) == NULL) {

        pBat_ShowErrorMessage(PBAT_EXPECTED_MORE, "START", 0);
        status = PBAT_EXPECTED_MORE;
        goto error;

    }

    n = 0;
    item = list;
    while (item) {
        n++;
        item = item->next;
    }

    if ((args = malloc(sizeof(char*)*(n + 1))) == NULL) {

        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION,
                                __FILE__ "/pBat_CmdStart()",
                                0);
        status = PBAT_FAILED_ALLOCATION;
        goto error;

    }

    item = list;
    n = 0;
    while (item) {
        args[n ++] = item->param->str;
        item = item->next;
    }

    args[n] = NULL;

    info.args = (const char* const*)args;
    if (info.dir == NULL)
        info.dir = lpCurrentDir;

    if (command)
        status = pBat_StartCommandBackground(info.cmdline,
                                             info.flags & PBAT_EXEC_WAIT);
    else
        status = pBat_ExecuteFile(&info);

end:
error:
    pBat_EsFree_Cached(cmdline);
    pBat_EsFree_Cached(param);

    if (dir)
        pBat_EsFree_Cached(dir);

    if (title)
        pBat_EsFree_Cached(title);

    if (list)
        pBat_FreeParamList(list);

    if (args)
        free(args);

    return status;
}
