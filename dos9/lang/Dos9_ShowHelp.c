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

#include "../core/Dos9_Core.h"
#include "Dos9_ShowHelp.h"

/* the main purpose of this functions is to provide little
   help embedded inside the binary, so that user can use
   this little help, even if they have no 'HLP' no
   documentation installed right there with their Dos9
   installation.

   Note that help message may contain '\n' characters, as
   those are translated at runtime, in contrary to messages
   designed to be used directly in commands, that are in
   dos9/lang/Dos9_Lang.c */

static const char* lpInternalHelp[DOS9_HELP_ARRAY_SIZE];
/* static const char* lpExternalMsg; */

void Dos9_LoadInternalHelp(void)
{
	lpInternalHelp[DOS9_HELP_ALIAS]
	    =gettext("Create an alias on a command.\n"
	             "Usage: ALIAS [/f] alias=command\n");

	lpInternalHelp[DOS9_HELP_CD]
	    =gettext("Change current working directory.\n"
	             "Usage: CD  [[/d] path]\n"
	             "       CHDIR [[/d] path]\n");

	lpInternalHelp[DOS9_HELP_CLS]
	    =gettext("Clear console screen\n"
	             "Usage: CLS\n");

	lpInternalHelp[DOS9_HELP_COLOR]
	    =gettext("Change console color to the given code.\n"
	             "Usage: COLOR [code]\n");

	lpInternalHelp[DOS9_HELP_COPY]
	    =gettext("Copy file(s) to another location.\n"
	             "Usage: COPY [/R] [/-Y | /Y] [/A[:]attributes] source [+] ... destination\n");

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
	    =gettext("Print a message, or enable/disable command echoing.\n"
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
	             "Usage: GOTO [/Q] [/F[:]file] [:]label\n");

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
	             "       )\n"
                 "       IF [ expression ] (\n"
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
	    =gettext("Remove a directory.\n"
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
	    =gettext("Set or list environment variables.\n"
	             "Usage: SET variable=content\n"
	             "       SET /a[[:][i|f]] variable=expression\n"
	             "       SET /p variable=question\n"
	             "       SET var\n");

	lpInternalHelp[DOS9_HELP_SETLOCAL]
	    =gettext("Set local environment and options of the Dos9 command prompt.\n"
	             "Usage: SETLOCAL [ENABLEDELAYEDEXPANSION | DISABLEDELAYEDEXPANSION] [ENABLEFLOATS | DISABLEFLOATS] [CMDLYCORRECT | CMDLYINCORRECT]\n");

	lpInternalHelp[DOS9_HELP_ENDLOCAL]
        =gettext("Reverts changes to environments and options made after a call to SETLOCAL.\n"
                 "Usage: ENDLOCAL\n");

	lpInternalHelp[DOS9_HELP_TITLE]
	    =gettext("Set console title.\n"
	             "Usage: TITLE title\n");

	lpInternalHelp[DOS9_HELP_TYPE]
	    =gettext("Print a file.\n"
	             "Usage: TYPE [file ...]\n");

	lpInternalHelp[DOS9_HELP_EXIT]
	    =gettext("Exit from the command prompt.\n"
	             "Usage: EXIT [/b] [code]\n");

	lpInternalHelp[DOS9_HELP_SHIFT]
	    =gettext("Change positions of command-line parameters.\n"
	             "Usage: SHIFT [/start_number | /s[:]start_number] [/d[:]displacement]\n");

    lpInternalHelp[DOS9_HELP_FIND]
        =gettext("Search occurrence of string in a set of files.\n"
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
 		=gettext("For each path, change current directory and pushd to stack\n"
	    		 "the previous current directory.\n"
		    	 "Usage: PUSHD [path1] [path2] [pathN] ...\n");

	lpInternalHelp[DOS9_HELP_POPD]
		=gettext("Pull a directory on the stack to change current directory.\n"
	        	 "Usage: POPD\n");

    lpInternalHelp[DOS9_HELP_WC]
        =gettext("Display counts of a file or of stdin.\n"
                 "Usage: WC [/L] [/C] [/M] [/W] [files ...]\n");

    lpInternalHelp[DOS9_HELP_XARGS]
        =gettext("Run a command with arguments from standard input file.\n"
                 "Usage: XARGS [command ...]\n");

    lpInternalHelp[DOS9_HELP_PROMPT]
        =gettext("Changes dos9 command prompt.\n"
                 "Usage: PROMPT prompt-format\n");

    lpInternalHelp[DOS9_HELP_PECHO]
        =gettext("Print a message using prompt format.\n"
                 "Usage: PECHO prompt-format\n"
                 "       PECHO.prompt-format\n");

    lpInternalHelp[DOS9_HELP_TIMEOUT]
        =gettext("Sleep for a delay in seconds.\n"
                 "Usage: TIMEOUT [/T] [/NOBREAK] seconds[.milliseconds]\n");

    lpInternalHelp[DOS9_HELP_MOD]
        =gettext("Load or list modules.\n"
                 "Usage: MOD [/L] [/Q] module\n");

	lpInternalHelp[DOS9_HELP_VER]
		=gettext("Print the OS type and version\n"
				 "Usage: VER\n");
}


void Dos9_ShowInternalHelp(int cmdId)
{
    const char *nl, *p;

	if ((cmdId >= 0)
	    && (cmdId < DOS9_HELP_ARRAY_SIZE)) {

        p = lpInternalHelp[cmdId];

        /* Translate '\n' to DOS9_NL */
        while (nl = strchr(p, '\n')) {

            fwrite(p, 1, (size_t)(nl - p), fError);
            fputs(DOS9_NL, fError);

            p = nl + 1;
        }

        if (*p)
            fputs(p, fError);

	}

}
