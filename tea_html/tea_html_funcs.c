#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "libDos9.h"
#include "tea_html.h"

void tea_html_AdjustHtmlCode(ESTR* lpEsContent)
{
    int i=0;

    for (i=0;i<10;i++) {
            Dos9_EsReplace(lpEsContent, "</li></ul>\n\n<ul><li>", "</li>\n\n<li>");
            Dos9_EsReplace(lpEsContent, "</ul></li>\n\n<li><ul>", "</ul>\n\n</ul>");
            Dos9_EsReplace(lpEsContent, "</ul>\n\n</ul>", "\n\n");
            Dos9_EsReplace(lpEsContent, "</li>\n\n<li><ul>", "\n\n<ul>");
    }

    Dos9_EsReplace(lpEsContent, "<li>-", "<li>");
}

void tea_html_ReadEntireFile(ESTR* lpEsContent, FILE* pFile)
{
    ESTR* lpEsTmp=Dos9_EsInit();

    Dos9_EsCpy(lpEsContent, "");

    while (!(Dos9_EsGet(lpEsTmp, pFile))) {
        Dos9_EsCatE(lpEsContent, lpEsTmp);
    }

    Dos9_EsFree(lpEsTmp);
}

void tea_html_MakeHtmlRootOpen(FILE *pFile)
{
    fputs("<!DOCTYPE html>\n<html>\n", pFile);
}

void tea_html_MakeHtmlMeta(FILE* pFile, char* lpTitle, char* lpEncoding, char* lpFileMeta)
{
    FILE* pMetaFile;

    char lpBuf[FILENAME_MAX];

    fputs("<head>\n\t<title>", pFile);

    if (lpTitle) {
        fputs(lpTitle, pFile);
    }

    fputs("</title>\n\t<meta charset=\"", pFile);

    if (lpEncoding) {
        fputs(lpEncoding, pFile);
    }

    fputs("\" />\n", pFile);

    if (lpFileMeta) {
        if ((pMetaFile=fopen(lpFileMeta, "r"))) {
            while (fgets(lpBuf, FILENAME_MAX, pMetaFile)) {
                fputs(lpBuf, pFile);
            }
            fclose(pMetaFile);
        } else {
            perror("tea_html :: Fichier meta ignore (ouverture impossible) ");
        }
    }

    fputs("</head>\n", pFile);
}

void tea_html_MakeHtmlBody(FILE* pFile, ESTR* lpEsContent, char* lpFileHeader, char* lpFileFooter)
{
    FILE *pAddFile;
    char lpBuf[FILENAME_MAX];

    fputs("<body>\n", pFile);

    if (lpFileHeader) {
        if ((pAddFile=fopen(lpFileHeader, "r"))) {
            while (fgets(lpBuf, FILENAME_MAX, pAddFile)) {
                fputs(lpBuf, pFile);
            }
            fclose(pAddFile);
        } else {
            perror("tea_html :: Fichier header ignore (ouverture impossible) ");
        }
    }

    fputs(Dos9_EsToChar(lpEsContent), pFile);

    if (lpFileFooter) {
        if ((pAddFile=fopen(lpFileFooter, "r"))) {
            while (fgets(lpBuf, FILENAME_MAX, pAddFile)) {
                fputs(lpBuf, pFile);
            }
            fclose(pAddFile);
        } else {
            perror("tea_html :: Fichier footer ignore (ouverture impossible) ");
        }
    }

    fputs("</body>\n", pFile);
}

void tea_html_MakeHtmlRootClose(FILE* pFile)
{
    fputs("</html>", pFile);
}

void tea_hmtl_MakeXmlRootOpen(FILE* pFile, char* lpEncoding)
{
    fprintf(pFile, "<?xml version=\"1.0\" encoding=\"%s\"?>\n", lpEncoding);
}
