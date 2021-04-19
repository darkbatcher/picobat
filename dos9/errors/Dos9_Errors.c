/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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

#include <libDos9.h>
#include <locale.h>
#include "Dos9_Errors.h"
#include "../core/Dos9_Core.h"

#include "../../config.h"
#include <gettext.h>

const char* lpErrorMsg[DOS9_ERROR_MESSAGE_NUMBER];
const char* lpQuitMessage;



void Dos9_LoadErrors(void)
{

	lpErrorMsg[DOS9_NO_ERROR]=
        gettext("No error: OK.");

	lpErrorMsg[DOS9_FILE_ERROR]=
	    gettext("Error : Unable to access file \"%s\".");

	lpErrorMsg[DOS9_DIRECTORY_ERROR]=
	    gettext("Error : Unable to find folder \"%s\".");

	lpErrorMsg[DOS9_COMMAND_ERROR]=
	    gettext("Error : \"%s\" is not recognized as an internal or"
	            " external command, an operable program or a batch file.");

	lpErrorMsg[DOS9_UNEXPECTED_ELEMENT]=
	    gettext("Error : \"%s\" was unexpected.");

	lpErrorMsg[DOS9_BAD_COMMAND_LINE]=
	    gettext("Error : Invalid command line for %s.");

	lpErrorMsg[DOS9_LABEL_ERROR]=
	    gettext("Error : Unable to find label \"%s\".");

	lpErrorMsg[DOS9_EXTENSION_DISABLED_ERROR]=
	    gettext("Warning : Using a Dos9 extension but the CMDLYCORRECT option is set.");

	lpErrorMsg[DOS9_EXPECTED_MORE]=
	    gettext("Error : \"%s\" expected more arguments.");

	lpErrorMsg[DOS9_INCOMPATIBLE_ARGS]=
	    gettext("Error : Incompatible arguments %s.");

	lpErrorMsg[DOS9_UNABLE_RENAME]=
	    gettext("Error : Unable to rename \"%s\".");

	lpErrorMsg[DOS9_MATH_OUT_OF_RANGE]=
	    gettext("Error : Overflown maximum value.");

	lpErrorMsg[DOS9_MATH_DIVIDE_BY_0]=
	    gettext("Error : Divide by 0.");

	lpErrorMsg[DOS9_MKDIR_ERROR]=
	    gettext("Error : Unable to create folder \"%s\".");

	lpErrorMsg[DOS9_RMDIR_ERROR]=
	    gettext("Error : Unable to delete folder \"%s\".");

	lpErrorMsg[DOS9_STREAM_MODULE_ERROR]=
	    gettext("Error : Stream module : \"%s\".");

	lpErrorMsg[DOS9_SPECIAL_VAR_NON_ASCII]=
	    gettext("Error : \"%c\" is an invalid character for special variables. "
	            "Special variables require their name to be strict ascii "
	            "characters, excluding controls characters and space "
	            "(0x00 - 0x30).");

	lpErrorMsg[DOS9_ARGUMENT_NOT_BLOCK]=
	    gettext("Error : \"%s\" is not a valid block (should at least be "
	            "enclosed within parenthesis).");

	lpErrorMsg[DOS9_FOR_BAD_TOKEN_SPECIFIER]=
	    gettext("Error : \"%s\" is not a valid token specifier.");

	lpErrorMsg[DOS9_FOR_TOKEN_OVERFLOW]=
	    gettext("Error : Attempted to specify more than "
	            "TOKEN_NB_MAX (%d) different tokens.");

	lpErrorMsg[DOS9_FOR_USEBACKQ_VIOLATION]=
	    gettext("Error : \"%s\" violates the ``Usebackq'' syntax.");

	lpErrorMsg[DOS9_FAILED_ALLOCATION]=
	    gettext("Error : Unable to allocate memory (in %s).");

	lpErrorMsg[DOS9_CREATE_PIPE]=
	    gettext("Error : Can't create pipe (in %s).");

	lpErrorMsg[DOS9_FOR_LAUNCH_ERROR]=
	    gettext("Error : unable to run a subprocess of Dos9 to process "
	            "the given input (\"%s\").");

	lpErrorMsg[DOS9_FOR_BAD_INPUT_SPECIFIER]=
	    gettext("Error : Invalid token specifier. \"%s\" breaks input "
	            "specifying rules.");

	lpErrorMsg[DOS9_FOR_TRY_REASSIGN_VAR]=
	    gettext("Error : FOR loop is trying to reassign already used %%%%%c "
	            "variable.");

	lpErrorMsg[DOS9_INVALID_EXPRESSION]=
	    gettext("Error : \"%s\" is not a valid mathematical expression.");

	lpErrorMsg[DOS9_INVALID_TOP_BLOCK]=
	    gettext("Error : \"%s\" is not a valid top-level block.");

	lpErrorMsg[DOS9_UNABLE_DUPLICATE_FD]=
	    gettext("Error : Unable to duplicate file descriptor (%d).");

	lpErrorMsg[DOS9_UNABLE_CREATE_PIPE]=
	    gettext("Error : Unable to create pipe at function (%s).");

	lpErrorMsg[DOS9_UNABLE_SET_ENVIRONMENT]=
	    gettext("Error : Unable to set environment string (%s).");

	lpErrorMsg[DOS9_INVALID_REDIRECTION]=
	    gettext("Error : \"%s\" is an invalid stream redirection.");

	lpErrorMsg[DOS9_ALREADY_REDIRECTED]=
	    gettext("Error : Redirection for \"%s\" was already specified.");

	lpErrorMsg[DOS9_MALFORMED_BLOCKS]=
	    gettext("Error : Encountered malformed blocks (\"%s\").");

	lpErrorMsg[DOS9_NONCLOSED_BLOCKS]=
	    gettext("Error : Blocks left unclosed at end-of-file.");

	lpErrorMsg[DOS9_UNABLE_ADD_COMMAND]=
	    gettext("Error : Unable to add command \"%s\".");

	lpErrorMsg[DOS9_UNABLE_REMAP_COMMANDS]=
	    gettext("Error : Unable to remap commands (in function %s).");

	lpErrorMsg[DOS9_TRY_REDEFINE_COMMAND]=
		gettext("Error : Trying to redefine already used \"%s\" command.");

	lpErrorMsg[DOS9_UNABLE_REPLACE_COMMAND]=
		gettext("Error : Unable to redefine \"%s\" command.");

	lpErrorMsg[DOS9_UNABLE_SET_OPTION]=
		gettext("Error : Unable to set option \"%s\", it is built "
			"statically inside your version of Dos9.");

    lpErrorMsg[DOS9_COMPARISON_FORBIDS_STRING]=
        gettext("Error : The \"%s\" comparison forbids use of strings.");

    lpErrorMsg[DOS9_FAILED_FORK]=
        gettext("Error : Unable to fork the current process (in function %s).");

    lpErrorMsg[DOS9_NO_MATCH]=
        gettext("Error : Pathspec \"%s\" does not match any file.");

    lpErrorMsg[DOS9_NO_VALID_FILE]=
        gettext("Error : %s does not recieved any valid file to process.");

    lpErrorMsg[DOS9_INVALID_NUMBER]=
        gettext("Error : \"%s\" is not a valid number.");

    lpErrorMsg[DOS9_TOO_MANY_ARGS] =
        gettext("Error : Too many arguments for %s command");

    lpErrorMsg[DOS9_INVALID_IF_EXPRESSION] =
        gettext("Error : \"%s\" is not a valid expression for IF command.");

    lpErrorMsg[DOS9_UNABLE_COPY] =
        gettext("Error : Unable to copy file \"%s\".");

    lpErrorMsg[DOS9_UNABLE_MOVE] =
        gettext("Error : Unable to move file \"%s\".");

    lpErrorMsg[DOS9_UNABLE_MKDIR] =
        gettext("Error : Unable to create directory \"%s\".");

    lpErrorMsg[DOS9_UNABLE_DELETE] =
        gettext("Error : Unable to delete file \"%s\".");

    lpErrorMsg[DOS9_UNABLE_RMDIR] =
        gettext("Error : Unable to delete directory \"%s\".");

    lpErrorMsg[DOS9_INVALID_CODEPAGE] =
        gettext("Error : \"%s\" is not a valid codepage on your system.");

    lpErrorMsg[DOS9_MOVE_NOT_RENAME] =
        gettext("Error : \"%s\" is a relative path, RENAME (or REN) do not "
                "handles relative path. If you want to move the file and "
                "rename it at the same time, you should use MOVE command.");

    lpErrorMsg[DOS9_BREAK_ERROR] =
        gettext("Error : Unable to access Dos9 main thread to perform break.");

    lpErrorMsg[DOS9_INVALID_LABEL] =
        gettext("Error : Invalid label encountered within a block (at line %d).");

    lpErrorMsg[DOS9_LOCK_MUTEX_ERROR] =
        gettext("Error : Unable to lock mutex (%s).");

    lpErrorMsg[DOS9_RELEASE_MUTEX_ERROR] =
        gettext("Error : Unable to release mutex (%s).");

    lpErrorMsg[DOS9_UNABLE_CAT] =
        gettext("Error : Unable to catenate file \"%s\".");

    lpErrorMsg[DOS9_UNABLE_LOAD_MODULE] =
        gettext("Error : Unable to load module \"%s\".");

    lpErrorMsg[DOS9_INCOMPATIBLE_MODULE] =
        gettext("Error : Incompatible module \"%s\".");

    lpErrorMsg[DOS9_MODULE_LOADED] =
        gettext("Error : Module \"%s\" is already loaded.");

	lpQuitMessage=
	    gettext("Aborting current command, press any key to end Dos9.");

}

void Dos9_ShowErrorMessage(unsigned int iErrorNumber,
                           const char* lpComplement,
                           int iExitCode)
{

	Dos9_SetConsoleTextColor(fError, DOS9_GET_BACKGROUND(colColor) | DOS9_FOREGROUND_IRED);

	if ((iErrorNumber & ~DOS9_PRINT_C_ERROR) < sizeof(lpErrorMsg)) {

        fprintf(fError,
		        lpErrorMsg[iErrorNumber & ~DOS9_PRINT_C_ERROR],
		        lpComplement
		       );
		fputs(DOS9_NL, fError);

	}

	if (iErrorNumber & DOS9_PRINT_C_ERROR) {

		fprintf(fError,
				"\tReason: %s" DOS9_NL,
				strerror(errno)
				);

	}

	if (iExitCode) {

		Dos9_SetConsoleTextColor(fError, colColor);

        fprintf(fError, DOS9_NL "%s" DOS9_NL, lpQuitMessage);

		Dos9_Getch(fInput);

		exit(iErrorNumber & (~DOS9_PRINT_C_ERROR));

	} else {

		Dos9_SetConsoleTextColor(fError, colColor);

	}

}
