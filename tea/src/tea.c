#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "Tea_Lib.h"
#include "tea_modes.h"
#include "libDos9.h"

ESTR* lpFrontEndCommandLine;

int main(int argc, char *argv[])
{
    TEAPAGE* lpTeaPage;

    TEAMODIFIERS* lpTeaMods=&teaTextMods;
    TEAOUTPUT* lpTeaOutput=&teaTextOutput;

    if (Dos9_LibInit() == -1) {

        puts("Error : Unable to load LibDos9. Exiting ...");
        exit(-1);
    }

    ESTR* lpEncodingStartCommandLine=Dos9_EsInit();
    ESTR* lpEncodingEndCommandLine=Dos9_EsInit();

    FILE* pOutput;

    char* lpPagePath=NULL;
    char* lpOutputPath=NULL;

    int i=1;

    lpFrontEndCommandLine=Dos9_EsInit();

    if (argc < 1) {
        printf("TEA :: Mauvaise ligne de commande");
        return -1;
    }

    Dos9_SetNewLineMode(DOS9_NEWLINE_LINUX);

    while (argv[i]) {

        if (!strnicmp("/O", argv[i], 2)) {

            argv[i]+=2;
            if (*(argv[i])==':') argv[i]++;

            if (!stricmp("text", argv[i])) {

                puts("TEA :: selection du mode TEXT");
                lpTeaMods=&teaTextMods;
                lpTeaOutput=&teaTextOutput;

            } else if (!stricmp("hlp-man", argv[i])) {

                puts("TEA :: selection du mode HLP-MAN");
                remove("makeman.tmp");
                lpTeaMods=&teaHlpMods;
                lpTeaOutput=&teaHlpOutput;
                Dos9_EsCpy(lpFrontEndCommandLine, lpTeaHlpFrontEnd);

            } else if (!stricmp("HTML", argv[i])) {

                puts("TEA :: selection du mode HTML");
                lpTeaMods=&teaHTMLMods;
                lpTeaOutput=&teaHTMLOutput;
                Dos9_EsCpy(lpFrontEndCommandLine, lpTeaHTMLFrontEnd);
                Dos9_EsCat(lpFrontEndCommandLine, " /H "); /* question de sécurité */

            } else if (!stricmp("XML", argv[i])) {

                puts("TEA :: selection du mode XML");
                lpTeaMods=&teaHTMLMods;
                lpTeaOutput=&teaHTMLOutput;
                Dos9_EsCpy(lpFrontEndCommandLine, lpTeaHTMLFrontEnd);
                Dos9_EsCat(lpFrontEndCommandLine, " /X "); /* question de sécurité */

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
                teaIsEncodingUtf8=TRUE;

            } else {
                puts("TEA :: selecton de l'encodage BYTE");
                teaIsEncodingUtf8=FALSE;
                if (*Dos9_EsToChar(lpFrontEndCommandLine)) {
                    Dos9_EsCat(lpFrontEndCommandLine, "/E:");
                    Dos9_EsCat(lpFrontEndCommandLine, argv[i]);
                    Dos9_EsCat(lpFrontEndCommandLine, " ");

                }
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
                printf("TEA :: ``%s'' etait innatendu\n", argv[i]);
                exit(-1);
            }
        }
        i++;
    }

    if (!(lpTeaPage=Tea_PageLoad(lpPagePath, lpTeaMods))) {
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

    Tea_OutputPage(pOutput, lpTeaOutput, lpTeaPage);

    Tea_PageFree(lpTeaPage, lpTeaMods);

    if (pOutput!=stdout) fclose(pOutput);

    if (*Dos9_EsToChar(lpFrontEndCommandLine)) {
        Dos9_EsCat(lpFrontEndCommandLine, "\"");
        Dos9_EsCat(lpFrontEndCommandLine, lpOutputPath);
        Dos9_EsCat(lpFrontEndCommandLine, "\"");
        printf("TEA :: lancement du front-end : %s\n", Dos9_EsToChar(lpFrontEndCommandLine));
        if (system(Dos9_EsToChar(lpFrontEndCommandLine))) { /* on execute la commande */
            printf("TEA :: Echec de l'execution du front-end\n");
            return -1;
        }
    }

    Dos9_EsFree(lpFrontEndCommandLine);
    Dos9_EsFree(lpEncodingEndCommandLine);
    Dos9_EsFree(lpEncodingStartCommandLine);

    return 0;
}
