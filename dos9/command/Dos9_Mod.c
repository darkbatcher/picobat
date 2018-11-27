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
#include <fcntl.h>

#ifdef DOS9_USE_MODULES
#include <dlfcn.h>
#endif

#include <libDos9.h>
#include "Dos9_Commands.h"
#include "../core/Dos9_Core.h"
#include "../errors/Dos9_Errors.h"
#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

#include "Dos9_Mod.h"



void Dos9_ModSetEnv(const char* name, const char* content)
{
    Dos9_SetEnv(lpeEnv, name, content);
}

char* Dos9_ModGetEnv(const char* name)
{
    return Dos9_GetEnv(lpeEnv, name);
}

const char* Dos9_ModGetCurrentDir(void)
{
    return lpCurrentDir;
}

int Dos9_ModSetCurrentDir(const char* dir)
{
    return Dos9_SetCurrentDir(dir);
}

FILE* Dos9_ModGetfInput(void)
{
    return fInput;
}

FILE* Dos9_ModGetfOutput(void)
{
    return fOutput;
}

FILE* Dos9_ModGetfError(void)
{
    return fError;
}

int Dos9_ModGetbIsScript(void)
{
    return bIsScript;
}

char* Dos9_ModGetNextParameterEs(const char* line, ESTR* recv)
{
    return Dos9_GetNextParameterEs(line, recv);
}

char* Dos9_ModEsToFullPath(ESTR* str)
{
    return Dos9_EsToFullPath(str);
}

int Dos9_ModRegisterCommand(const char* name, int(*handler)(char*))
{
    COMMANDINFO command;
    COMMANDLIST *new;
    void* p;
    int status = DOS9_NO_ERROR;

    command.ptrCommandName = name;
    command.lpCommandProc = handler;
    command.cfFlag = strlen(name);

    if (Dos9_GetCommandProc(name, lpclCommands, &p) != -1) {

        Dos9_ShowErrorMessage(DOS9_TRY_REDEFINE_COMMAND,
							  name,
							  FALSE
							 );

        status = DOS9_TRY_REDEFINE_COMMAND;

        goto error;

    }

    if ((Dos9_AddCommandDynamic(&command, &lpclCommands))) {

		Dos9_ShowErrorMessage(DOS9_UNABLE_ADD_COMMAND,
		                      name,
		                      FALSE
		                     );

        status = DOS9_UNABLE_ADD_COMMAND;

		goto error;

	}

	if (!(new=Dos9_ReMapCommandInfo(lpclCommands))) {

		Dos9_ShowErrorMessage(DOS9_UNABLE_REMAP_COMMANDS,
		                      __FILE__ "/Dos9_CmdAlias()",
		                      FALSE
		                     );

		status = DOS9_UNABLE_REMAP_COMMANDS;

		goto error;

	}

    Dos9_FreeCommandList(lpclCommands);

	lpclCommands=new;

error:
    return status;
}

void* Dos9_GetSymbol(int id)
{
    switch (id) {

    case DOS9_MOD_SETENV:
        return Dos9_ModSetEnv;
    case DOS9_MOD_GETENV:
        return Dos9_ModGetEnv;
    case DOS9_MOD_GETCURRENTDIR:
        return Dos9_ModGetCurrentDir;
    case DOS9_MOD_SETCURRENTDIR:
        return Dos9_ModSetCurrentDir;
    case DOS9_MOD_GETFINPUT:
        return Dos9_ModGetfInput;
    case DOS9_MOD_GETFOUTPUT:
        return Dos9_ModGetfOutput;
    case DOS9_MOD_GETFERROR:
        return Dos9_ModGetfError;
    case DOS9_MOD_GETBISSCRIPT:
        return Dos9_ModGetbIsScript;
    case DOS9_MOD_GETNEXTPARAMETERES:
        return Dos9_ModGetNextParameterEs;
    case DOS9_MOD_ESTOFULLPATH:
        return Dos9_ModEsToFullPath;
    case DOS9_MOD_REGISTERCOMMAND:
        return Dos9_ModRegisterCommand;
    case DOS9_MOD_SHOWERRORMESSAGE:
        return Dos9_ShowErrorMessage;
    case DOS9_MOD_RUNLINE:
        return Dos9_RunLine;
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

void Dos9_ListMods(void)
{
    struct mod_list_t* item = modules;

    fprintf(fOutput, lpModuleList);

    while (item) {

        fprintf(fOutput, "\t* %s" DOS9_NL, item->name);
        item = item->next;

    }
}

int Dos9_CheckLoadedMods(const char* mod)
{
    struct mod_list_t* item = modules;

    while (item) {

        if (!stricmp(mod, item->name))
            return 1;

    }

    return 0;
}

int Dos9_LoadMod(const char* mod)
{
    void *handle;
    int(*handler)(void*(*)(int), int);
    struct mod_list_t* item;

    #if DOS9_USE_MODULES
    if ((handle = dlopen(mod, RTLD_LAZY)) == NULL
        || (handler = dlsym(handle, "Dos9_ModuleHandler")) == NULL) {

        Dos9_ShowErrorMessage(DOS9_UNABLE_LOAD_MODULE, mod, 0);

        if (handle)
            dlclose(handle);

        return DOS9_UNABLE_LOAD_MODULE;

    }

    if ((item = malloc(sizeof(struct mod_list_t))) == NULL)
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION,
                                __FILE__ "/Dos9_LoadMod", -1);

    if (handler(Dos9_GetSymbol, DOS9_MOD_API_VERSION) == -1) {

        Dos9_ShowErrorMessage(DOS9_INCOMPATIBLE_MODULE, mod, 0);
        dlclose(handle);
        free(item);
        return DOS9_INCOMPATIBLE_MODULE;

    }

    item->handle = handle;
    item->handler = handler;

    if ((item->name = strdup(mod)) == NULL)
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION,
                              __FILE__ "/Dos9_LoadMod", -1);

    item->next = modules;
    modules = item;
    #else
    fprintf(fOutput, lpModulesNotSupported);
    return DOS9_UNABLE_LOAD_MODULE;
    #endif

    return DOS9_NO_ERROR;
}

/*  MOD [/l] [/q] [module ...]

    Load or manage module

    - /l : list loaded modules

    - /q : ignore silently module if it has already been loaded

 */

#define LOUD 0
#define QUIET 1
int Dos9_CmdMod(char* line)
{
    ESTR* param = Dos9_EsInit();
    char *path;
    int mode = LOUD, status = DOS9_NO_ERROR;
    line += 3;

    if (Dos9_LockMutex(&mModLock))
        Dos9_ShowErrorMessage(DOS9_LOCK_MUTEX_ERROR,
                               __FILE__ "/Dos9_CmdMod",
                               -1);

    if ((path = malloc(FILENAME_MAX)) == NULL) {

        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION,
                                __FILE__ "/Dos9_CmdMod", 0);
        status = DOS9_FAILED_ALLOCATION;
        goto end;

    }

    while (line = Dos9_GetNextParameterEs(line, param)) {

        if (!stricmp(param->str, "/l")) {

            Dos9_ListMods();
            goto end;

        } else if (!strcmp(param->str, "/?")) {

            Dos9_ShowInternalHelp(DOS9_HELP_MOD);
            goto end;

        } else if (!stricmp(param->str, "/q"))
            mode = QUIET;
        else {

            if (Dos9_GetFilePath(path, param->str, FILENAME_MAX) == -1) {

                Dos9_ShowErrorMessage(DOS9_FILE_ERROR, param->str, 0);
                status = DOS9_FILE_ERROR;

                goto end;

            }

            if (Dos9_CheckLoadedMods(path))
                if (mode == LOUD) {

                    Dos9_ShowErrorMessage(DOS9_MODULE_LOADED, path, 0);
                    status = DOS9_MODULE_LOADED;
                    goto end;

                } else
                    continue;

            if (status = Dos9_LoadMod(path))
                goto end;
        }
    }

end:

    if (Dos9_ReleaseMutex(&mModLock))
        Dos9_ShowErrorMessage(DOS9_RELEASE_MUTEX_ERROR,
                               __FILE__ "/Dos9_CmdMod",
                               -1);

    if (path)
        free(path);
    Dos9_EsFree(param);

    return status;
}
