#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "LibDos9.h"
#include "tea_html.h"

int main(int argc, char *argv[])
{

    FILE* pFile;

    char *lpFileName=NULL, *lpFileMeta=NULL, *lpFileHeader=NULL, *lpFileFooter=NULL, *lpEncoding="utf-8";
    int iFlag=0;

    int i=1;

    if (Dos9_LibInit() == -1) {

        puts("Error : Unable to load LibDos9. Exiting ...");
        exit(-1);
    }

    Dos9_SetNewLineMode(DOS9_NEWLINE_LINUX);

    ESTR *lpEsContent=Dos9_EsInit(), *lpEsTmp=Dos9_EsInit();
    ESTR *lpTitle=Dos9_EsInit();


    if (argc<1) {
        fprintf(stderr ,"tea_html :: erreur : ligne de commande incorrecte");
        return -1;
    }

    while (argv[i]) {
        if (!stricmp("/H", argv[i])) {
            iFlag=(iFlag & ~TEA_HTML_MODE_XML) | TEA_HTML_MODE_HTML;
        } else if (!stricmp("/X", argv[i])) {
            iFlag=(iFlag & ~TEA_HTML_MODE_HTML) | TEA_HTML_MODE_XML;
        } else if (!stricmp("/C", argv[i])) {
            iFlag|=TEA_HTML_MODE_CONTENT_ONLY;
        } else if (!stricmp("/T", argv[i])) {
            i++;

            if (argv[i]==NULL) {
                fprintf(stderr, "tea_html :: ligne de commande incorrecte");
                return -1;
            }

            if (*Dos9_EsToChar(lpTitle)!='\0') {

                Dos9_EsCat(lpTitle, " ");

            }

            Dos9_EsCat(lpTitle, argv[i]);

        } else if (!stricmp("/M", argv[i])) {
            i++;

            if (argv[i]==NULL) {
                fprintf(stderr, "tea_html :: ligne de commande incorrecte");
                return -1;
            }

            lpFileMeta=argv[i];

        } else if (!stricmp("/HH", argv[i])) {
            i++;

            if (argv[i]==NULL) {
                fprintf(stderr, "tea_html :: ligne de commande incorrecte");
                return -1;
            }

            lpFileHeader=argv[i];

        } else if (!stricmp("/F", argv[i])) {
            i++;

            if (argv[i]==NULL) {
                fprintf(stderr, "tea_html :: ligne de commande incorrecte");
                return -1;
            }

            lpFileFooter=argv[i];

        } else {

            if (!lpFileName) {
                lpFileName=argv[i];
            } else {
                fprintf(stderr, "tea_html :: ligne de commande incorrecte");
                return -1;
            }

        }

        i++;
    }

    if (!(pFile=fopen(lpFileName, "r"))) {
        fprintf(stderr ,"tea_html :: impossible d'ouvrir ``%s'' : %s", lpFileName, strerror(errno));
        return errno;
    }

    tea_html_ReadEntireFile(lpEsContent, pFile);

    fclose(pFile);

    tea_html_AdjustHtmlCode(lpEsContent);


    if (!(pFile=fopen(lpFileName, "w+"))) {
        fprintf(stderr, "tea_html :: impossible d'ouvrir ``%s'' : %s", lpFileName, strerror(errno));
        return errno;
    }

    if (!(iFlag & TEA_HTML_MODE_CONTENT_ONLY)) {
        if ((iFlag & TEA_HTML_MODE_HTML)) {
            tea_html_MakeHtmlRootOpen(pFile);
            tea_html_MakeHtmlMeta(pFile, Dos9_EsToChar(lpTitle), lpEncoding, lpFileMeta);
            tea_html_MakeHtmlBody(pFile, lpEsContent, lpFileHeader, lpFileFooter);
            tea_html_MakeHtmlRootClose(pFile);
        } else {
            tea_hmtl_MakeXmlRootOpen(pFile, lpEncoding);
            tea_html_MakeHtmlBody(pFile, lpEsContent, lpFileFooter, lpFileFooter);
        }
    } else {
        fprintf(pFile, "%s", Dos9_EsToChar(lpEsContent));
    }

    fclose(pFile);

    Dos9_EsFree(lpEsContent);
    Dos9_EsFree(lpEsTmp);
    Dos9_EsFree(lpTitle);

    return 0;
}
