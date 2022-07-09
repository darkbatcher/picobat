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
#include <fcntl.h>

#include <libpBat.h>
#include "pBat_Commands.h"
#include "../core/pBat_Core.h"
#include "../errors/pBat_Errors.h"
#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

#include "pBat_Mod.h"

#ifdef PBAT_USE_MODULES
#include <dlfcn.h>
#endif

void pBat_ModSetEnv(const char* name, const char* content)
{
    pBat_SetEnv(lpeEnv, name, content);
}

char* pBat_ModGetEnv(const char* name)
{
    return pBat_GetEnv(lpeEnv, name);
}

const char* pBat_ModGetCurrentDir(void)
{
    return lpCurrentDir;
}

int pBat_ModSetCurrentDir(const char* dir)
{
    return pBat_SetCurrentDir(dir);
}

FILE* pBat_ModGetfInput(void)
{
    return fInput;
}

FILE* pBat_ModGetfOutput(void)
{
    return fOutput;
}

FILE* pBat_ModGetfError(void)
{
    return fError;
}

int pBat_ModGetbIsScript(void)
{
    return bIsScript;
}

char* pBat_ModGetNextParameterEs(const char* line, ESTR* recv)
{
    return pBat_GetNextParameterEs(line, recv);
}

char* pBat_ModEsToFullPath(ESTR* str)
{
    return pBat_EsToFullPath(str);
}

int pBat_ModRegisterCommand(const char* name, int(*handler)(char*))
{
    COMMANDINFO command;
    COMMANDLIST *new;
    void* p;
    int status = PBAT_NO_ERROR;

    command.ptrCommandName = name;
    command.lpCommandProc = handler;
    command.cfFlag = strlen(name);

    if (pBat_GetCommandProc(name, lpclCommands, &p) != -1) {

        pBat_ShowErrorMessage(PBAT_TRY_REDEFINE_COMMAND,
							  name,
							  FALSE
							 );

        status = PBAT_TRY_REDEFINE_COMMAND;

        goto error;

    }

    if ((pBat_AddCommandDynamic(&command, &lpclCommands))) {

		pBat_ShowErrorMessage(PBAT_UNABLE_ADD_COMMAND,
		                      name,
		                      FALSE
		                     );

        status = PBAT_UNABLE_ADD_COMMAND;

		goto error;

	}

	if (!(new=pBat_ReMapCommandInfo(lpclCommands))) {

		pBat_ShowErrorMessage(PBAT_UNABLE_REMAP_COMMANDS,
		                      __FILE__ "/pBat_CmdAlias()",
		                      FALSE
		                     );

		status = PBAT_UNABLE_REMAP_COMMANDS;

		goto error;

	}

    pBat_FreeCommandList(lpclCommands);

	lpclCommands=new;

error:
    return status;
}

void* pBat_GetSymbol(int id)
{
    switch (id) {

    case PBAT_MOD_SETENV:
        return pBat_ModSetEnv;
    case PBAT_MOD_GETENV:
        return pBat_ModGetEnv;
    case PBAT_MOD_GETCURRENTDIR:
        return pBat_ModGetCurrentDir;
    case PBAT_MOD_SETCURRENTDIR:
        return pBat_ModSetCurrentDir;
    case PBAT_MOD_GETFINPUT:
        return pBat_ModGetfInput;
    case PBAT_MOD_GETFOUTPUT:
        return pBat_ModGetfOutput;
    case PBAT_MOD_GETFERROR:
        return pBat_ModGetfError;
    case PBAT_MOD_GETBISSCRIPT:
        return pBat_ModGetbIsScript;
    case PBAT_MOD_GETNEXTPARAMETERES:
        return pBat_ModGetNextParameterEs;
    case PBAT_MOD_ESTOFULLPATH:
        return pBat_ModEsToFullPath;
    case PBAT_MOD_REGISTERCOMMAND:
        return pBat_ModRegisterCommand;
    case PBAT_MOD_SHOWERRORMESSAGE:
        return pBat_ShowErrorMessage;
    case PBAT_MOD_RUNLINE:
        return pBat_RunLine;
    default:
        return NULL;

    }
}

struct mod_list_t {
    char* name;
    void* handle;
    int(*handler)(void*(*)(int), int);
    struct mod_list_t* next;
};

struct mod_list_t* modules = NULL;

void pBat_ListMods(void)
{
    struct mod_list_t* item = modules;

    fprintf(fOutput, "%s" PBAT_NL, lpModuleList);

    while (item) {

        fprintf(fOutput, "\t* %s" PBAT_NL, item->name);
        item = item->next;

    }
}

int pBat_CheckLoadedMods(const char* mod)
{
    struct mod_list_t* item = modules;

    while (item) {

        if (!stricmp(mod, item->name))
            return 1;

        item = item->next;

    }

    return 0;
}

int pBat_LoadMod(const char* mod)
{
    void *handle;
    int(*handler)(void*(*)(int), int);
    struct mod_list_t* item;

    #ifdef PBAT_USE_MODULES
    if ((handle = dlopen(mod, RTLD_LAZY)) == NULL
        || (handler = dlsym(handle, "pBat_ModuleHandler")) == NULL) {

        pBat_ShowErrorMessage(PBAT_UNABLE_LOAD_MODULE, mod, 0);

        if (handle)
            dlclose(handle);

        return PBAT_UNABLE_LOAD_MODULE;

    }

    if ((item = malloc(sizeof(struct mod_list_t))) == NULL)
        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION,
                                __FILE__ "/pBat_LoadMod", -1);

    if (handler(pBat_GetSymbol, PBAT_MOD_API_VERSION) == -1) {

        pBat_ShowErrorMessage(PBAT_INCOMPATIBLE_MODULE, mod, 0);
        dlclose(handle);
        free(item);
        return PBAT_INCOMPATIBLE_MODULE;

    }

    item->handle = handle;
    item->handler = handler;

    if ((item->name = strdup(mod)) == NULL)
        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION,
                              __FILE__ "/pBat_LoadMod", -1);

    item->next = modules;
    modules = item;
    #else
    fprintf(fOutput, lpModulesNotSupported);
    return PBAT_UNABLE_LOAD_MODULE;
    #endif

    return PBAT_NO_ERROR;
}

/*  MOD [/l] [/q] [module ...]

    Load or manage module

    - /l : list loaded modules

    - /q : ignore silently module if it has already been loaded

 */

#define LOUD 0
#define QUIET 1
int pBat_CmdMod(char* line)
{
    ESTR* param = pBat_EsInit_Cached();
    char *path;
    int mode = LOUD, status = PBAT_NO_ERROR;
    line += 3;

    if (pBat_LockMutex(&mModLock))
        pBat_ShowErrorMessage(PBAT_LOCK_MUTEX_ERROR,
                               __FILE__ "/pBat_CmdMod",
                               -1);

    if ((path = malloc(FILENAME_MAX)) == NULL) {

        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION,
                                __FILE__ "/pBat_CmdMod", 0);
        status = PBAT_FAILED_ALLOCATION;
        goto end;

    }

    while (line = pBat_GetNextParameterEs(line, param)) {

        if (!stricmp(param->str, "/l")) {

            pBat_ListMods();
            goto end;

        } else if (!strcmp(param->str, "/?")) {

            pBat_ShowInternalHelp(PBAT_HELP_MOD);
            goto end;

        } else if (!stricmp(param->str, "/q"))
            mode = QUIET;
        else {

            if (!PBAT_TEST_ABSOLUTE_PATH(param->str)) {

                /* if the path is not absolute, then the command is refferring
                   to something in %PBAT_PATH%/modules */

                ESTR* tmp = pBat_EsInit_Cached();

                pBat_MakePath(tmp, 3, pBat_GetEnv(lpeEnv, "PBAT_PATH"), "modules", param->str);

                pBat_EsFree_Cached(param);
                param = tmp;


            }

            path = param->str;

            if (!pBat_FileExists(path)) {

                pBat_ShowErrorMessage(PBAT_FILE_ERROR, param->str, 0);
                status = PBAT_FILE_ERROR;

                goto end;

            }

            if (pBat_CheckLoadedMods(path))
                if (mode == LOUD) {

                    pBat_ShowErrorMessage(PBAT_MODULE_LOADED, path, 0);
                    status = PBAT_MODULE_LOADED;
                    goto end;

                } else
                    continue;

            if (status = pBat_LoadMod(path))
                goto end;
        }
    }

end:

    if (pBat_ReleaseMutex(&mModLock))
        pBat_ShowErrorMessage(PBAT_RELEASE_MUTEX_ERROR,
                               __FILE__ "/pBat_CmdMod",
                               -1);

    pBat_EsFree_Cached(param);

    return status;
}
