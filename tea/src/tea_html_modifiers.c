#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tea_modes.h"
#include "tea_modifier.h"
#include "LibDos9.h"

void   tea_HtmlHeadingStart(FILE* pOut, char* pTitle)
{
    static int iIsFirst=TRUE;

    if (iIsFirst) {
        fputs("<h1>", pOut);

        Dos9_EsCat(lpFrontEndCommandLine, "/T \"");
        Dos9_EsCat(lpFrontEndCommandLine, pTitle);
        Dos9_EsCat(lpFrontEndCommandLine, "\" ");

        iIsFirst=FALSE;
    } else {
        fputs("<h2>", pOut);
    }
}

void   tea_HtmlHeadingEnd(FILE* pOut, char* pTitle)
{
    static int iIsFirst=TRUE;

    if (iIsFirst) {
        fputs("</h1>", pOut);
        iIsFirst=FALSE;
    } else {
        fputs("</h2>", pOut);
    }
}

void   tea_HtmlCodeStart(FILE* pOut, char* pTitle)
{
    fputs("<code>", pOut);
}

void   tea_HtmlCodeEnd(FILE* pOut, char* pTitle)
{
    fputs("</code>", pOut);
}

void   tea_HtmlParagraphStart(FILE* pOut, char* pTitle)
{
    int iLvl=(int)pTitle;
    int i=0;

    if (iLvl) {
        while (i<iLvl) {
            fputs("<ul><li>", pOut);
            i++;
        }
    } else {
        fputs("<p>", pOut);
    }
}

void   tea_HtmlParagraphEnd(FILE* pOut, char* pTitle)
{
    int iLvl=(int)pTitle;
    int i=0;

    if (iLvl) {
        while (i<iLvl) {
            fputs("</li></ul>", pOut);
            i++;
        }
    } else {
        fputs("</p>", pOut);
    }
}

void   tea_HtmlEmphasisStart(FILE* pOut, char* pTitle)
{
    fputs("<strong>", pOut);
}

void   tea_HtmlEmphasisEnd(FILE* pOut, char* pTitle)
{
    fputs("</strong>", pOut);
}

void   tea_HtmlLinkStart(FILE* pOut, char* lpTarget)
{
    char* lpExtension="";
    char* lpToken, *lpTokenBackSlash;

    if ((lpToken=strrchr(lpTarget, '/')) || (lpTokenBackSlash=strrchr(lpTarget, '\\'))) {

        if (lpTokenBackSlash > lpToken) lpToken=lpTokenBackSlash;


    } else {
        lpToken=lpTarget;
    }

    if ( !(strchr(lpToken, '.')) && strncmp(lpTarget, "http:", sizeof("http:")) && strncmp(lpTarget, "mailto:", sizeof("mailto:")) && strncmp(lpTarget, "ftp:", sizeof("mailto:"))) {

            /* si il n'y a pas d'extension spécifiée */

        lpExtension=".html";

    }

    fprintf(pOut, "<a href=\"%s%s\" >", lpTarget, lpExtension);

}

void   tea_HtmlLinkEnd(FILE* pOut, char* pTitle)
{
    fputs("</a>", pOut);
}
