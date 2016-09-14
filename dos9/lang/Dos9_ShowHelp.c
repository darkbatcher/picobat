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

#include <libDos9.h>

#include "../../config.h"
#include "../gettext.h"

#include "Dos9_ShowHelp.h"

/* the main purpose of this functions is to provide little
   help embedded inside the binary, so that user can use
   this little help, even if they have no 'HLP' no
   documentation installed right there with their Dos9
   installation */

static const char* lpInternalHelp[DOS9_HELP_ARRAY_SIZE];
static const char* lpExternalMsg;

void Dos9_LoadInternalHelp(void)
{
	char lpPath[FILENAME_MAX];
	char lpEncoding[15];
	char lpSharePath[FILENAME_MAX];

	Dos9_GetExePath(lpPath, FILENAME_MAX);
	Dos9_GetConsoleEncoding(lpEncoding, sizeof(lpEncoding));

	snprintf(lpSharePath, FILENAME_MAX, "%s/share/locale", lpPath);

	bindtextdomain("Dos9-hlp", lpSharePath);
#if !(defined(WIN32) && defined(DOS9_USE_LIBCU8))
    /* This is not useful at all, libcu8 is able to convert utf-8 by
       itself */
	bind_textdomain_codeset("Dos9-hlp", lpEncoding);
#elif defined(DOS9_USE_LIBCU8)
    bind_textdomain_codeset("Dos9-msg", "UTF8");
#endif
	textdomain("Dos9-hlp");

	lpInternalHelp[DOS9_HELP_ALIAS]
	    =gettext("Create an alias on a command.\n"
	             "Usage: ALIAS [/f] alias=command\n");

	lpInternalHelp[DOS9_HELP_CD]
	    =gettext("Change current working directory.\n"
	             "Usage: CD  [[/d] path]\n"
	             "       CHDIR [[/d] path]\n");

	lpInternalHelp[DOS9_HELP_CLS]
	    =gettext("Clear console's screen\n"
	             "Usage: CLS\n");

	lpInternalHelp[DOS9_HELP_COLOR]
	    =gettext("Change console's color to the given code.\n"
	             "Usage: COLOR [code]\n");

	lpInternalHelp[DOS9_HELP_COPY]
	    =gettext("Copy file(s) to another location.\n"
	             "Usage: COPY [/R] [/-Y | /Y] [/A[:]attributes] source destination\n");

	lpInternalHelp[DOS9_HELP_CALL]
	    =gettext("Call a script or a label from the current context.\n"
	             "Usage: CALL [/e] [file] [:label] [parameters ...]\n");


	lpInternalHelp[DOS9_HELP_DEL]
	    =gettext("Delete file(s).\n"
	             "Usage: DEL [/P] [/F] [/S] [/Q] [/A[:]attributes] name\n"
	             "       ERASE [/P] [/F] [/S] [/Q] [/A[:]attributes] name\n");

	lpInternalHelp[DOS9_HELP_DIR]
	    =gettext("List files in a directory and in subdirectories.\n"
	             "Usage: DIR [/A[:]attributes] [/S] [/B] [path]\n");

	lpInternalHelp[DOS9_HELP_ECHO]
	    =gettext("Print a message, or enable/disable commande echoing.\n"
	             "Usage: ECHO [OFF|ON]\n"
	             "       ECHO text\n"
	             "       ECHO.text\n");

	lpInternalHelp[DOS9_HELP_FOR]
	    =gettext("Perform commands against items (files, strings, command output).\n"
	             "Usage: FOR %%A IN (string) DO (\n"
	             "          :: some commands\n"
	             "       )\n\n"
	             "       FOR /F [options] %%A IN (object) DO (\n"
	             "          :: some commands\n"
	             "       )\n\n"
	             "       FOR /L %%A IN (start,increment,end) DO (\n"
	             "          :: some commands\n"
	             "       )\n");

	lpInternalHelp[DOS9_HELP_GOTO]
	    =gettext("Go to a label in a command script.\n"
	             "Usage: GOTO [:]label [file]\n");

	lpInternalHelp[DOS9_HELP_IF]
	    =gettext("Perform commands on some conditions.\n"
	             "Usage: IF [/i] [NOT] string1==string2 (\n"
	             "          :: code to be ran\n"
	             "       )\n\n"
	             "       IF [/i] string1 cmp string2\n"
	             "          :: code to be ran\n"
	             "       )\n\n"
	             "       IF [NOT] [DEFINED | EXIST | ERRORLEVEL] object (\n"
	             "          :: code to be ran\n"
	             "       )\n");

	lpInternalHelp[DOS9_HELP_MD]
	    =gettext("Create the given directory.\n"
	             "Usage: MD directory\n"
	             "       MKDIR directory\n");


	lpInternalHelp[DOS9_HELP_PAUSE]
	    =gettext("Wait for a keystroke from the user.\n"
	             "Usage: PAUSE\n");


	lpInternalHelp[DOS9_HELP_RD]
	    =gettext("Removes a directory.\n"
	             "Usage: RMDIR [/S] [/Q] directory\n"
	             "       RD [/S] [/Q] directory\n");

	lpInternalHelp[DOS9_HELP_REN]
	    =gettext("Renames a file.\n"
	             "Usage: REN file name.ext\n"
	             "       RENAME file name.ext\n");

	lpInternalHelp[DOS9_HELP_REN]
	    =gettext("Introduce a comment.\n"
	             "Usage: REM comment\n");


	lpInternalHelp[DOS9_HELP_SET]
	    =gettext("Set an environment variable.\n"
	             "Usage: SET variable=content\n"
	             "       SET /a[[:][i|f]] variable=expression\n"
	             "       SET /p variable=question\n");

	lpInternalHelp[DOS9_HELP_SETLOCAL]
	    =gettext("Set options of the Dos9 command prompt.\n"
	             "Usage: SETLOCAL [ENABLEDELAYEDEXPANSION | DISABLEDELAYEDEXPANSION] [ENABLEFLOATS | DISABLEFLOATS] [CMDLYCORRECT | CMDLYINCORRECT]\n");

	lpInternalHelp[DOS9_HELP_TITLE]
	    =gettext("Set console's title.\n"
	             "Usage: TITLE title\n");

	lpInternalHelp[DOS9_HELP_TYPE]
	    =gettext("Print a file.\n"
	             "Usage: TYPE [file ...]\n");

	lpInternalHelp[DOS9_HELP_EXIT]
	    =gettext("Exit from the command prompt.\n"
	             "Usage: EXIT [/b code]\n");

	lpInternalHelp[DOS9_HELP_SHIFT]
	    =gettext("Change positions of command-line parameters.\n"
	             "Usage: SHIFT [/start_number | /s[:]start_number] [/d[:]displacement]\n");

    lpInternalHelp[DOS9_HELP_FIND]
        =gettext("Search occurence of string in a set of files.\n"
                 "Usage: FIND [/N] [/C] [/V] [/I] [/E] string [file ...]\n");

    lpInternalHelp[DOS9_HELP_MORE]
        =gettext("Display content of files.\n"
                 "Usage: MORE [/E] [/P] [/S] [/C] [/T[:]n] [+n] [file ...]\n");

    lpInternalHelp[DOS9_HELP_BREAK]
        =gettext("[Obsolete] Sets or Clears Extended CTRL+C checking on DOS system. (Do nothing)\n"
                 "Usage: BREAK [ON | OFF]\n");

    lpInternalHelp[DOS9_HELP_CHCP]
        =gettext("Set Console codepage or encoding.\n"
                 "Usage: CHCP [ codepage | encoding]\n");

	 lpInternalHelp[DOS9_HELP_PUSHD]
 		=gettext("Changes the system's current directory and store the previous folder/path for use by the POPD command.\n"
		    	 "Usage: PUSHD [path]\n");

	lpInternalHelp[DOS9_HELP_POPD]
		=gettext("Change directory back to the path/folder most recently stored by the PUSHD command.\n"
	        	 "Usage: POPD\n");

	lpExternalMsg
	    =gettext("This help is voluntary limited help to fit in the binary. If you need to\n"
	             "see the full documentation, please type:\n"
	             "\n"
	             "\tHELP command");

}


void Dos9_ShowInternalHelp(int cmdId)
{

	if ((cmdId >= 0)
	    && (cmdId < DOS9_HELP_ARRAY_SIZE)) {

		puts(lpInternalHelp[cmdId]);
		puts(lpExternalMsg);

	}

}
