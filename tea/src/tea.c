#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <libDos9.h>

#include "Tea.h"
#include "out/tea_out.h"


ESTR* lpFrontEndCommandLine;

int main(int argc, char *argv[])
{
    TEAPAGE* lpTeaPage;
    LP_OUTPUT_HANDLER pOutputHandler=Tea_TextOutputHandler;
    LP_PARSE_HANDLER pParseHandler=Tea_TextParseHandler;

    if (Dos9_LibInit() == -1) {

        puts("Error : Unable to load LibDos9. Exiting ...");
        exit(-1);
    }

    FILE* pOutput;

    char* lpPagePath=NULL;
    char* lpOutputPath=NULL;

    int i=1,
        iContinue=TRUE;

    lpFrontEndCommandLine=Dos9_EsInit();

    if (argc < 1) {
        printf("TEA :: Mauvaise ligne de commande");
        return -1;
    }

    Dos9_SetNewLineMode(DOS9_NEWLINE_LINUX);

    while (argv[i] && iContinue) {

        if (!strnicmp("/O", argv[i], 2)) {

            argv[i]+=2;
            if (*(argv[i])==':') argv[i]++;

            if (!stricmp("text", argv[i])) {

                puts("TEA :: selection du mode TEXT");
                pOutputHandler=Tea_TextOutputHandler;
                pParseHandler=Tea_TextParseHandler;

            } else if (!stricmp("hlp-man", argv[i])) {

                puts("TEA :: selection du mode HLP-MAN");
                //pOutputHandler=Tea_HlpOutputHandler;

            } else if (!stricmp("HTML", argv[i])) {

                puts("TEA :: selection du mode HTML");
                pOutputHandler=Tea_HtmlOutputHandler;
                pParseHandler=Tea_HtmlParseHandler;

            } else {

                printf("TEA :: ``%s'' n'est pas un mode de sortie correct\n",  argv[i]);
                exit(-1);
            }
        } else if (!stricmp("/F", argv[i])) {

            /* on change les paramètres du front-end */
            i++;

            if (argv[i] && Dos9_EsToChar(lpFrontEndCommandLine)) {
                Dos9_EsCat(lpFrontEndCommandLine, argv[i]);
                Dos9_EsCat(lpFrontEndCommandLine, " ");
            }

        } else if (!strnicmp("/E", argv[i], 2)) {

            argv[i]+=2;
            if (*(argv[i])==':') argv[i]++;

            if (!stricmp("UTF-8" ,argv[i])) {

                puts("TEA :: selection de l'encodage UTF-8");
                Dos9_SetEncoding(DOS9_UTF8_ENCODING);

            } else {

                puts("TEA :: selecton de l'encodage BYTE");
                Dos9_SetEncoding(DOS9_BYTE_ENCODING);

            }

        } else if (!strcmp("/?", argv[i])) {

            puts("Consultez ``tea.txt'' ou entrez ``HLP TEA''");
            return 0;

        } else {

            if (!lpPagePath) {

                lpPagePath=argv[i];

            } else if (!lpOutputPath) {

                lpOutputPath=argv[i];

            } else {

                iContinue=FALSE;
                continue;

            }

        }

        i++;
    }

    if (!(lpTeaPage=Tea_PageLoad(lpPagePath, pParseHandler))) {
        printf("TEA :: Impossible d'ouvrir le modele TEA ``%s'' : %s", lpPagePath, strerror(errno));
        return errno;
    }

    if (lpOutputPath) {
        if (!(pOutput=fopen(lpOutputPath, "w+"))) {
            printf("TEA :: Impossible d'ouvrir la sortie TEA ``%s'' : %s", lpOutputPath, strerror(errno));
            return errno;
        }
    } else {
        pOutput=stdout;
    }

    pOutputHandler(lpTeaPage, pOutput, i, argv);

    Tea_PageFree(lpTeaPage);

    fclose(pOutput);
    return 0;
}
