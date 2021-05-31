/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
 *   Copyright (C) 2010-2014 Romain GARBI
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
#include <errno.h>

#include <libpBat.h>
#include <locale.h>
#include "pBat_Errors.h"
#include "../core/pBat_Core.h"

#include "../../config.h"
#include <gettext.h>

const char* lpErrorMsg[PBAT_ERROR_MESSAGE_NUMBER];
const char* lpQuitMessage;



void pBat_LoadErrors(void)
{

	lpErrorMsg[PBAT_NO_ERROR]=
        gettext("No error: OK.");

	lpErrorMsg[PBAT_FILE_ERROR]=
	    gettext("Error : Unable to access file \"%s\".");

	lpErrorMsg[PBAT_DIRECTORY_ERROR]=
	    gettext("Error : Unable to find folder \"%s\".");

	lpErrorMsg[PBAT_COMMAND_ERROR]=
	    gettext("Error : \"%s\" is not recognized as an internal or"
	            " external command, an operable program or a batch file.");

	lpErrorMsg[PBAT_UNEXPECTED_ELEMENT]=
	    gettext("Error : \"%s\" was unexpected.");

	lpErrorMsg[PBAT_BAD_COMMAND_LINE]=
	    gettext("Error : Invalid command line for %s.");

	lpErrorMsg[PBAT_LABEL_ERROR]=
	    gettext("Error : Unable to find label \"%s\".");

	lpErrorMsg[PBAT_EXTENSION_DISABLED_ERROR]=
	    gettext("Warning : Using a picobat extension but the CMDLYCORRECT option is set.");

	lpErrorMsg[PBAT_EXPECTED_MORE]=
	    gettext("Error : \"%s\" expected more arguments.");

	lpErrorMsg[PBAT_INCOMPATIBLE_ARGS]=
	    gettext("Error : Incompatible arguments %s.");

	lpErrorMsg[PBAT_UNABLE_RENAME]=
	    gettext("Error : Unable to rename \"%s\".");

	lpErrorMsg[PBAT_MATH_OUT_OF_RANGE]=
	    gettext("Error : Overflown maximum value.");

	lpErrorMsg[PBAT_MATH_DIVIDE_BY_0]=
	    gettext("Error : Divide by 0.");

	lpErrorMsg[PBAT_MKDIR_ERROR]=
	    gettext("Error : Unable to create folder \"%s\".");

	lpErrorMsg[PBAT_RMDIR_ERROR]=
	    gettext("Error : Unable to delete folder \"%s\".");

	lpErrorMsg[PBAT_STREAM_MODULE_ERROR]=
	    gettext("Error : Stream module : \"%s\".");

	lpErrorMsg[PBAT_SPECIAL_VAR_NON_ASCII]=
	    gettext("Error : \"%c\" is an invalid character for special variables. "
	            "Special variables require their name to be strict ascii "
	            "characters, excluding controls characters and space "
	            "(0x00 - 0x30).");

	lpErrorMsg[PBAT_ARGUMENT_NOT_BLOCK]=
	    gettext("Error : \"%s\" is not a valid block (should at least be "
	            "enclosed within parenthesis).");

	lpErrorMsg[PBAT_FOR_BAD_TOKEN_SPECIFIER]=
	    gettext("Error : \"%s\" is not a valid token specifier.");

	lpErrorMsg[PBAT_FOR_TOKEN_OVERFLOW]=
	    gettext("Error : Attempted to specify more than "
	            "TOKEN_NB_MAX (%d) different tokens.");

	lpErrorMsg[PBAT_FOR_USEBACKQ_VIOLATION]=
	    gettext("Error : \"%s\" violates the ``Usebackq'' syntax.");

	lpErrorMsg[PBAT_FAILED_ALLOCATION]=
	    gettext("Error : Unable to allocate memory (in %s).");

	lpErrorMsg[PBAT_CREATE_PIPE]=
	    gettext("Error : Can't create pipe (in %s).");

	lpErrorMsg[PBAT_FOR_LAUNCH_ERROR]=
	    gettext("Error : unable to run a subprocess of picobat to process "
	            "the given input (\"%s\").");

	lpErrorMsg[PBAT_FOR_BAD_INPUT_SPECIFIER]=
	    gettext("Error : Invalid token specifier. \"%s\" breaks input "
	            "specifying rules.");

	lpErrorMsg[PBAT_FOR_TRY_REASSIGN_VAR]=
	    gettext("Error : FOR loop is trying to reassign already used %%%%%c "
	            "variable.");

	lpErrorMsg[PBAT_INVALID_EXPRESSION]=
	    gettext("Error : \"%s\" is not a valid mathematical expression.");

	lpErrorMsg[PBAT_INVALID_TOP_BLOCK]=
	    gettext("Error : \"%s\" is not a valid top-level block.");

	lpErrorMsg[PBAT_UNABLE_DUPLICATE_FD]=
	    gettext("Error : Unable to duplicate file descriptor (%d).");

	lpErrorMsg[PBAT_UNABLE_CREATE_PIPE]=
	    gettext("Error : Unable to create pipe at function (%s).");

	lpErrorMsg[PBAT_UNABLE_SET_ENVIRONMENT]=
	    gettext("Error : Unable to set environment string (%s).");

	lpErrorMsg[PBAT_INVALID_REDIRECTION]=
	    gettext("Error : \"%s\" is an invalid stream redirection.");

	lpErrorMsg[PBAT_ALREADY_REDIRECTED]=
	    gettext("Error : Redirection for \"%s\" was already specified.");

	lpErrorMsg[PBAT_MALFORMED_BLOCKS]=
	    gettext("Error : Encountered malformed blocks (\"%s\").");

	lpErrorMsg[PBAT_NONCLOSED_BLOCKS]=
	    gettext("Error : Blocks left unclosed at end-of-file.");

	lpErrorMsg[PBAT_UNABLE_ADD_COMMAND]=
	    gettext("Error : Unable to add command \"%s\".");

	lpErrorMsg[PBAT_UNABLE_REMAP_COMMANDS]=
	    gettext("Error : Unable to remap commands (in function %s).");

	lpErrorMsg[PBAT_TRY_REDEFINE_COMMAND]=
		gettext("Error : Trying to redefine already used \"%s\" command.");

	lpErrorMsg[PBAT_UNABLE_REPLACE_COMMAND]=
		gettext("Error : Unable to redefine \"%s\" command.");

	lpErrorMsg[PBAT_UNABLE_SET_OPTION]=
		gettext("Error : Unable to set option \"%s\", it is built "
			"statically inside your version of pBat.");

    lpErrorMsg[PBAT_COMPARISON_FORBIDS_STRING]=
        gettext("Error : The \"%s\" comparison forbids use of strings.");

    lpErrorMsg[PBAT_FAILED_FORK]=
        gettext("Error : Unable to fork the current process (in function %s).");

    lpErrorMsg[PBAT_NO_MATCH]=
        gettext("Error : Pathspec \"%s\" does not match any file.");

    lpErrorMsg[PBAT_NO_VALID_FILE]=
        gettext("Error : %s does not recieved any valid file to process.");

    lpErrorMsg[PBAT_INVALID_NUMBER]=
        gettext("Error : \"%s\" is not a valid number.");

    lpErrorMsg[PBAT_TOO_MANY_ARGS] =
        gettext("Error : Too many arguments for %s command");

    lpErrorMsg[PBAT_INVALID_IF_EXPRESSION] =
        gettext("Error : \"%s\" is not a valid expression for IF command.");

    lpErrorMsg[PBAT_UNABLE_COPY] =
        gettext("Error : Unable to copy file \"%s\".");

    lpErrorMsg[PBAT_UNABLE_MOVE] =
        gettext("Error : Unable to move file \"%s\".");

    lpErrorMsg[PBAT_UNABLE_MKDIR] =
        gettext("Error : Unable to create directory \"%s\".");

    lpErrorMsg[PBAT_UNABLE_DELETE] =
        gettext("Error : Unable to delete file \"%s\".");

    lpErrorMsg[PBAT_UNABLE_RMDIR] =
        gettext("Error : Unable to delete directory \"%s\".");

    lpErrorMsg[PBAT_INVALID_CODEPAGE] =
        gettext("Error : \"%s\" is not a valid codepage on your system.");

    lpErrorMsg[PBAT_MOVE_NOT_RENAME] =
        gettext("Error : \"%s\" is a relative path, RENAME (or REN) do not "
                "handles relative path. If you want to move the file and "
                "rename it at the same time, you should use MOVE command.");

    lpErrorMsg[PBAT_BREAK_ERROR] =
        gettext("Error : Unable to access picobat main thread to perform break.");

    lpErrorMsg[PBAT_INVALID_LABEL] =
        gettext("Error : Invalid label encountered within a block (at line %d).");

    lpErrorMsg[PBAT_LOCK_MUTEX_ERROR] =
        gettext("Error : Unable to lock mutex (%s).");

    lpErrorMsg[PBAT_RELEASE_MUTEX_ERROR] =
        gettext("Error : Unable to release mutex (%s).");

    lpErrorMsg[PBAT_UNABLE_CAT] =
        gettext("Error : Unable to catenate file \"%s\".");

    lpErrorMsg[PBAT_UNABLE_LOAD_MODULE] =
        gettext("Error : Unable to load module \"%s\".");

    lpErrorMsg[PBAT_INCOMPATIBLE_MODULE] =
        gettext("Error : Incompatible module \"%s\".");

    lpErrorMsg[PBAT_MODULE_LOADED] =
        gettext("Error : Module \"%s\" is already loaded.");

	lpQuitMessage=
	    gettext("Aborting current command, press any key to end picobat.");

}

void pBat_ShowErrorMessage(unsigned int iErrorNumber,
                           const char* lpComplement,
                           int iExitCode)
{
    char* msg;

	pBat_SetConsoleTextColor(fError, PBAT_GET_BACKGROUND(colColor) | PBAT_FOREGROUND_IRED);

	if (iErrorNumber & PBAT_PRINT_C_ERROR)
        msg = strerror(errno);

	if ((iErrorNumber & ~PBAT_PRINT_C_ERROR) < sizeof(lpErrorMsg)) {

        fprintf(fError,
		        lpErrorMsg[iErrorNumber & ~PBAT_PRINT_C_ERROR],
		        lpComplement
		       );
		fputs(PBAT_NL, fError);

	}

	if (iErrorNumber & PBAT_PRINT_C_ERROR) {

		fprintf(fError,
				"\tReason: %s" PBAT_NL,
				msg
				);

	}

	if (iExitCode) {

		pBat_SetConsoleTextColor(fError, colColor);

        fprintf(fError, PBAT_NL "%s" PBAT_NL, lpQuitMessage);

		pBat_Getch(fInput);

		exit(iErrorNumber & (~PBAT_PRINT_C_ERROR));

	} else {

		pBat_SetConsoleTextColor(fError, colColor);

	}

}
