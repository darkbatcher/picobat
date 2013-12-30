#include <stdio.h>
#include <stdlib.h>

#include "Dos9_Lang.h"

#ifdef WIN32

    #include "libintl.h"

#else

    #include <libintl.h>

#endif

#include "LibDos9.h"

const char* lpIntroduction;

const char* lpMsgEchoOn;
const char* lpMsgEchoOff;
const char* lpMsgPause;

const char* lpHlpMain;

const char* lpDirNoFileFound;
const char* lpDirListTitle;
const char* lpDirFileDirNb;

const char* lpHlpDeprecated;


void Dos9_LoadStrings(void)
{
    /* this loads strings */
    char lpPath[FILENAME_MAX];
	char lpEncoding[15];
    char lpSharePath[FILENAME_MAX];

    Dos9_GetExePath(lpPath, FILENAME_MAX);
	Dos9_GetConsoleEncoding(lpEncoding, sizeof(lpEncoding));

    snprintf(lpSharePath, FILENAME_MAX, "%s/share/locale", lpPath);

    bindtextdomain("Dos9-msg", lpSharePath);
	bind_textdomain_codeset("Dos9-msg", lpEncoding);
    textdomain("Dos9-msg");

    /* texte des commandes ECHO et PAUSE */
    lpMsgEchoOn=gettext("Echo command enabled");
    lpMsgEchoOff=gettext("Echo command disabled");
    lpMsgPause=gettext("Press any key to continue...");

    /* texte de la commande DIR */
    lpDirNoFileFound=gettext("\tNo files found\n");
    lpDirListTitle=gettext("\nLast change\t\tSize\tAttr.\tName\n");
    lpDirFileDirNb=gettext("\t\t\t\t\t\t%d Files\n\t\t\t\t\t\t%d Folders\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpHlpDeprecated=gettext("Dos9 does not support ``/?'' switch for help anymore, use ``HLP command'' instead.");



    lpHlpMain=gettext("DOS9 COMMAND\n\
\n\
        DOS9 is a free, cross-platform command prompt used for batch scripts \n\
     and command processing.\n\
\n\
        DOS9 is a free software, designed to be as compatible as possible with\n\
     CMD.EXE proprietary software from MICROSOFT.\n\
\n\
SYNOPSIS\n\
\n\
   DOS9 [/v] [/n] [/f] [/e] [file]\n\
\n\
        Run a command file or wait for an user input.\n\
\n\
        - FILE : Path of the batch script to be run.\n\
\n\
        - /V : Enables delayed expansion (see `spec/xvar').\n\
\n\
        - /N : Enables dos9's extension (see `spec/ext').\n\
\n\
        - /F : Enables floating numbers (see `spec/exp').\n\
\n\
        - /E : Disable current directory echoing.\n\
\n\
COMPATIBILITY\n\
\n\
        Compatible with any version of DOS9. Incompatible with CMD.EXE.\n\
\n\
        Dos9 is cross-platform so that it can be run either on MS-WINDOWS or \n\
     GNU/LINUX. Theorically, a port of Dos9 should be possible in any \n\
     POSIX-compatible system just by recompiling sources using a C89 compiler.\n\
\n\
LICENSE\n\
\n\
        DOS9 is a free software distributed under GNU General Public License \n\
     (see `http://www.gnu.org/licenses/gpl.html') terms. For more \n\
     informations about free software's philosophy, see Free Software \n\
     Fundation's (see `http://www.fsf.org') website.\n\
\n\
AUTHOR\n\
\n\
        DOS9 have been written since 2010 by DARKBATCHER (ROMAIN GARBI). It \n\
     uses severals helpful free libraries from the GNU operating system (see \n\
     `http://www.gnu.org/'), mostly libmatheval (see \n\
     `http://www.gnu.org/software/libmatheval/'), gettext (see \n\
     `https://www.gnu.org/software/gettext/').\n\
\n\
        Dos9's windows packages also distribute some binaries from the GnuWin \n\
     project (see `http://gnuwin32.sourceforge.net/'). These binaries are GNU \n\
     iconv (see `iconv') and GNU nano (see `nano').\n\
\n\
SEE ALSO\n\
\n\
        ECHO Command (see `echo'), Command scripts (see `script'), Commands \n\
     list (see `commands') Commands list (see `commands')\n");
     // TRANSLATORS : Don't translate this, just pick it up from the documentation
     // or, obviously, if the translation does not exist, translate it.
}
