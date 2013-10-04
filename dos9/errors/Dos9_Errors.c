#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
    #include <conio.h>
#else
    #define getch() getchar()
#endif

#include "LibDos9.h"
#include "Dos9_Errors.h"

#ifdef WIN32

    #include "libintl.h"

#else

    #include <libintl.h>

#endif

const char* lpErrorMsg[14];
const char* lpQuitMessage;

void Dos9_LoadErrors(void)
{
    char lpPath[FILENAME_MAX];
    char lpSharePath[FILENAME_MAX];
	char lpEncoding[15]="ASCII";

    Dos9_GetExePath(lpPath, FILENAME_MAX);
	Dos9_GetConsoleEncoding(lpEncoding, sizeof(lpEncoding));

    snprintf(lpSharePath, FILENAME_MAX, "%s/share/locale", lpPath);

    bindtextdomain("Dos9-errors", lpSharePath);
	bind_textdomain_codeset("Dos9-errors", lpEncoding);
    textdomain("Dos9-errors");

    lpErrorMsg[0]=gettext("Error : Unable to acces to file \"%s\"\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[1]=gettext("Error : Can't find folder \"%s\"\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[2]=gettext("Error : \"%s\" is not recognized as an internal or external command, an operable program or a batch file\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[3]=gettext("Error : \"%s\" was unexpected\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[4]=gettext("Error : Invalid command line\n");

    lpErrorMsg[5]=gettext("Error : Unable to find label \"%s\"\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[6]=gettext("Warning : This requires Dos9's extensions to be enabled\n");

    lpErrorMsg[7]=gettext("Error : \"%s\" expected more arguments\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[8]=gettext("Error : Uncompatible arguments %s\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[9]=gettext("Error : Unable to rename \"%s\"\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[10]=gettext("Error : Overflowed maximum value\n");

    lpErrorMsg[11]=gettext("Error : Divide by 0\n");

    lpErrorMsg[12]=gettext("Error : Unable to create folder \"%s\"\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[13]=gettext("Error : Unable to delete folder \"%s\"\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[13]=gettext("Error : Stream module : \"%s\"\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpQuitMessage=gettext("\nDos9 will abort current command and quit, hit any key to continue...");

}

void Dos9_ShowErrorMessage(unsigned int iErrorNumber,  char* lpComplement, int iExitCode)
{

    Dos9_SetConsoleTextColor(DOS9_BACKGROUND_DEFAULT | DOS9_FOREGROUND_IRED);
    if (iErrorNumber < sizeof(lpErrorMsg)) printf(lpErrorMsg[iErrorNumber], lpComplement);
    if (iExitCode) {
        puts(lpQuitMessage);
        getch();
        exit(iExitCode);
    } else {
        Dos9_SetConsoleTextColor(DOS9_COLOR_DEFAULT);
    }

}
