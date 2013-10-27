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

const char* lpErrorMsg[DOS9_ERROR_MESSAGE_NUMBER];
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

    lpErrorMsg[DOS9_FILE_ERROR]=gettext("Error : Unable to acces to file \"%s\"\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[DOS9_DIRECTORY_ERROR]=gettext("Error : Can't find folder \"%s\"\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[DOS9_COMMAND_ERROR]=gettext("Error : \"%s\" is not recognized as an internal or external command, an operable program or a batch file\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[DOS9_UNEXPECTED_ELEMENT]=gettext("Error : \"%s\" was unexpected\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[DOS9_BAD_COMMAND_LINE]=gettext("Error : Invalid command line\n");

    lpErrorMsg[DOS9_LABEL_ERROR]=gettext("Error : Unable to find label \"%s\"\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[DOS9_EXTENSION_DISABLED_ERROR]=gettext("Warning : This requires Dos9's extensions to be enabled\n");

    lpErrorMsg[DOS9_EXPECTED_MORE]=gettext("Error : \"%s\" expected more arguments\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[DOS9_INCOMPATIBLE_ARGS]=gettext("Error : Uncompatible arguments %s\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[DOS9_UNABLE_RENAME]=gettext("Error : Unable to rename \"%s\"\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[DOS9_MATH_OUT_OF_RANGE]=gettext("Error : Overflowed maximum value\n");

    lpErrorMsg[DOS9_MATH_DIVIDE_BY_0]=gettext("Error : Divide by 0\n");

    lpErrorMsg[DOS9_MKDIR_ERROR]=gettext("Error : Unable to create folder \"%s\"\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[DOS9_RMDIR_ERROR]=gettext("Error : Unable to delete folder \"%s\"\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[DOS9_STREAM_MODULE_ERROR]=gettext("Error : Stream module : \"%s\"\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[DOS9_SPECIAL_VAR_NON_ASCII]=gettext("Error : \"%c\" is an invalid character for special variables. "
                           "Special variables require their name to be strict ascii characters,"
                           " exluding controls characters and space (0x00 - 0x30)\n");
    // TRANSLATORS : Don't remove the %s because the program needs it

    lpErrorMsg[DOS9_ARGUMENT_NOT_BLOCK]=gettext("Error : \"%s\" is not a valid block (should at least be enclosed"
                                                " within parenthesis).");

    lpQuitMessage=gettext("\nDos9 will abort current command and quit, hit any key to continue...");

}

void Dos9_ShowErrorMessage(unsigned int iErrorNumber,  char* lpComplement, int iExitCode)
{

    Dos9_SetConsoleTextColor(DOS9_BACKGROUND_DEFAULT | DOS9_FOREGROUND_IRED);
    if (iErrorNumber < sizeof(lpErrorMsg)) fprintf(stderr, lpErrorMsg[iErrorNumber], lpComplement);
    if (iExitCode) {
        puts(lpQuitMessage);
        getch();
        exit(iExitCode);
    } else {
        Dos9_SetConsoleTextColor(DOS9_COLOR_DEFAULT);
    }

}
