/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2015 Romain GARBI
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

#include "Dos9_Lang.h"

#include "../../config.h"
#include "../gettext.h"

#include <libDos9.h>

const char* lpIntroduction;

const char* lpMsgEchoOn;
const char* lpMsgEchoOff;
const char* lpMsgPause;

const char* lpHlpMain;

const char* lpDirNoFileFound;
const char* lpDirListTitle;
const char* lpDirFile;
const char* lpDirDir;

const char* lpHlpDeprecated;

const char* lpDelConfirm;
const char* lpRmdirConfirm;
const char* lpCopyConfirm;

const char* lpBreakConfirm;

const char* lpAskYn;
const char* lpAskyN;
const char* lpAskyn;

const char* lpAskYna;
const char* lpAskyNa;
const char* lpAskynA;
const char* lpAskyna;

const char* lpAskYes;
const char* lpAskYesA;

const char* lpAskNo;
const char* lpAskNoA;

const char* lpAskAll;
const char* lpAskAllA;

const char* lpAskInvalid;

void Dos9_LoadStrings(void)
{
	/* this loads strings */
	char lpPath[FILENAME_MAX];
	char lpEncoding[15];
	char lpSharePath[FILENAME_MAX];

#ifdef WIN32
    /* On windows, the best is to suppose that *all* the read-only
       files are in the same folder as the binary. */

    Dos9_GetExePath(lpPath, FILENAME_MAX);
	Dos9_GetConsoleEncoding(lpEncoding, sizeof(lpEncoding));

    snprintf(lpSharePath, FILENAME_MAX, "%s/share/locale", lpPath);
#else
    /* Under *nixes, get the po files from the ordinary read-only
       directory */
    snprintf(lpSharePath, FILENAME_MAX, DATA_PATH "/locale");
#endif // WIN32

	bindtextdomain("Dos9-msg", lpSharePath);
#if defined(WIN32) && !(defined(DOS9_USE_LIBCU8))
    /* This is not useful at all, libcu8 is able to convert utf-8 by
       itself */
	bind_textdomain_codeset("Dos9-msg", lpEncoding);
#elif defined(DOS9_USE_LIBCU8)
    bind_textdomain_codeset("Dos9-msg", "UTF8");
#endif
	textdomain("Dos9-msg");

	/* texte des commandes ECHO et PAUSE */
	lpMsgEchoOn=gettext("Echo command enabled");
	lpMsgEchoOff=gettext("Echo command disabled");
	lpMsgPause=gettext("Press any key to continue...");

	/* texte de la commande DIR */
	lpDirNoFileFound=gettext("\tNo files found");
	lpDirListTitle=gettext("Last change\t\tSize\tAttr.\tName");
	lpDirFile=gettext("Files");
	lpDirDir=gettext("Folders");

	lpDelConfirm=gettext("Are you sure you want to delete file \"%s\" ?");
	lpRmdirConfirm=gettext("Are you sure you want to remove \"%s\" directory ?");

	lpCopyConfirm=gettext("File \"%s\" already exists, replace it by \"%s\" ?");

	lpBreakConfirm=gettext("Do you really want to exit command script ?");

	lpAskYn=gettext(" (Yes/no) ");
	lpAskyN=gettext(" (yes/No) ");
	lpAskyn=gettext(" (yes/no) ");

	lpAskYna=gettext(" (Yes/no/all) ");
	lpAskyNa=gettext(" (yes/No/all) ");
	lpAskynA=gettext(" (yes/no/All) ");
	lpAskyna=gettext(" (yes/no/all) ");

	lpAskYes=gettext("YES");
	lpAskYesA=gettext("Y");

	lpAskNo=gettext("NO");
	lpAskNoA=gettext("N");

	lpAskAll=gettext("ALL");
	lpAskAllA=gettext("A");

	lpAskInvalid=gettext("Please enter a correct choice (or type enter to choose default) !\n");

	lpHlpMain=gettext("Usage:\tDOS9 [/A[:]attr] [/I in] [/O out] [file ... | /K cmd ...|/C cmd ...]\n\n\
\t/A:attr\tA list of attributes for Dos9\n\
\t\tC\tEnable CMDLYCORRECT option to enable more compatibility\n\
\t\t\twith cmd.exe.\n\
\t\tF\tEnable floating numbers support.\n\
\t\tE\tDisable echo command (disable current path printing).\n\
\t\tQ\tQuiet Mode (does not print start screen).\n\
\t/I in\tFile descriptor to used as input.\n\
\t/O out\tFile descriptor to be used as output.\n\
\tfile\tA batch file to be executed.\n\
\t/C cmd\tRun cmd command and exit.\n\
\t/K cmd\tRun cmd command and stay active.\n");
}
