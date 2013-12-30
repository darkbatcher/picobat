#include <stdio.h>
#include <stdlib.h>

#include "Tea_Lib.h"
#include "libDos9.h"

#define UNICODE_BYTE 0x80
#define UNICODE_FOLOWING_BYTE_MASK 0xC0
#define UNICODE_FOLOWING_BYTE_MARK 0x80

extern int teaIsEncodingUtf8=FALSE;

int         Tea_OutputParagraph(FILE* pOutput, TEASTYLE* lpTeaStyle, TEAOUTPUTMODIFIERS* lpTeaMods, TEANODE* lpTeaNode)
{
    char *lpToken, *lpLastWord;
    int iNbCharsWritten=0;

    int iOldMarginWidth;
    int iIndentedWidth=0;
    int iFirstLoop=TRUE;
    int iNextNodeWLength=0;

    int iMarginWidth=lpTeaStyle->iMarginWidth; /* the margin left to paragraph */
    int iTotalWidth=lpTeaStyle->iWidth; /* la largeur totale du paragraphe */
    int iIndentWidth=lpTeaStyle->iIndentWidth; /* la largueur de l'indentation de début de paragraphe */

    /* on recherche des puces pour savoir si il faut modifier la marge */


    if (*(lpTeaNode->lpContent)=='-') {
        iIndentWidth=-2;
        iOldMarginWidth=iMarginWidth;
        lpToken=lpTeaNode->lpContent;

        while (*lpToken=='-') {
            iMarginWidth+=iOldMarginWidth;
            lpToken++;
            iIndentedWidth++;
        }

    }

    Tea_OutputRunModifier(lpTeaMods->lpParagraphStart, pOutput, (char*)iIndentedWidth);

    if ((iMarginWidth + iIndentWidth) >= iTotalWidth)
    {
        printf("Erreur ! paramètres incoherents pour les paragraphes\n");
        exit(-1);
    }

    /* on écrit la marge et l'indentation d'abord */
    while (iNbCharsWritten<(iMarginWidth+iIndentWidth)) {
        fputc(' ', pOutput);
        iNbCharsWritten++;
    }

    /* partie compliquée de l'affichage du paragraphe */
    while (lpTeaNode) {


        switch (lpTeaNode->iNodeType) {

            case TEA_NODE_EMPHASIS:
                Tea_OutputRunModifier(lpTeaMods->lpEmphasisStart, pOutput, lpTeaNode->lpContent);
                break;

            case TEA_NODE_LINK:
                Tea_OutputRunModifier(lpTeaMods->lpLinkStart, pOutput, lpTeaNode->lpTarget);

            default:;
        }


        /* on récupère le contenu du noeud */
        lpToken=lpTeaNode->lpContent;

        /* on supprimme les '-' en trop s'il s'agit du premier noeud */

        if (iFirstLoop && *lpToken) {
            while (*(lpToken+1)=='-') {
                lpToken++;
            }
            iFirstLoop=FALSE;
        }

        while (*lpToken) {
            lpLastWord=lpToken;

            while (*lpToken && *lpToken!=' ') {
                lpToken=Tea_OutputGetNextChar(lpToken); /* besoin du support d'unicode */
                iNbCharsWritten++;
            }

            iNextNodeWLength = Hlp_GetNextNodeWLenght(lpTeaNode);

            if (*lpToken==' ' && (iNbCharsWritten == iTotalWidth)) {

                while (lpLastWord != lpToken) {
                    fputc(*lpLastWord, pOutput);
                    lpLastWord++;
                }

                if (*lpToken) lpToken++;

            }

            /* si le mot dépasse de la ligne */
            else if (((iNbCharsWritten >= iTotalWidth)) ||
                     (*lpToken=='\0' &&  iNextNodeWLength && (iNbCharsWritten+iNextNodeWLength >= iTotalWidth))) {

                /* on termine la ligne */
                fputc('\n', pOutput);
                iNbCharsWritten=0;

                /* on écrit la marge d'abord */
                while (iNbCharsWritten<iMarginWidth) {
                    fputc(' ', pOutput);
                    iNbCharsWritten++;
                }

                lpToken=lpLastWord;

            } else {
                /* si le mot passe dans la ligne */

                /* on l'affiche */
                if (*lpToken) lpToken++;
                iNbCharsWritten++;

                while (lpLastWord != lpToken) {
                    fputc(*lpLastWord, pOutput);
                    lpLastWord++;
                }

            }

        }

        switch (lpTeaNode->iNodeType) {

            case TEA_NODE_EMPHASIS:
                Tea_OutputRunModifier(lpTeaMods->lpEmphasisEnd, pOutput, lpTeaNode->lpContent);
                break;

            case TEA_NODE_LINK:
                Tea_OutputRunModifier(lpTeaMods->lpLinkEnd, pOutput, lpTeaNode->lpTarget);

            default:;
        }


        lpTeaNode=lpTeaNode->lpTeaNodeNext;
    }


    Tea_OutputRunModifier(lpTeaMods->lpParagraphEnd, pOutput, iIndentedWidth);

    return 0;
}

int         Tea_OutputCode(FILE* pOutput, TEASTYLE* lpTeaStyle, char* lpContent)
{
    char *lpLastWord;
    int iNbCharsWritten=0;
    int iNewLine=0;

    int iMarginWidth=lpTeaStyle->iMarginWidth; /* the margin left to paragraph */
    int iTotalWidth=lpTeaStyle->iWidth; /* la largeur totale du paragraphe */
    int iIndentWidth=lpTeaStyle->iIndentWidth;
                    /* la largueur de l'indentation de début de paragraphe */

    if ((iMarginWidth + iIndentWidth) >= iTotalWidth)
    {
        printf("Erreur ! paramètres incoherents pour les paragraphes\n");
        exit(-1);
    }

    /* on écrit la marge et l'indentation d'abord */
    while (iNbCharsWritten<(iMarginWidth)) {
        fputc(' ', pOutput);
        iNbCharsWritten++;
    }


    while (*lpContent) {
        lpLastWord=lpContent;

        while (*lpContent && *lpContent!=' ' && *lpContent!='\n') {
            lpContent=Tea_OutputGetNextChar(lpContent);
            iNbCharsWritten++;
        }

        /* si le mot dépasse de la ligne */
        if (iNbCharsWritten >= iTotalWidth) {

            /* on termine la ligne */
            fputc('\n', pOutput);
            iNbCharsWritten=0;

            /* on écrit la marge d'abord */
            while (iNbCharsWritten<(iIndentWidth+iMarginWidth)) {
                fputc(' ', pOutput);
                iNbCharsWritten++;
            }

            lpContent=lpLastWord;

        } else {
            /* si le mot passe dans la ligne */
            if (*lpContent=='\n') {
                iNewLine=TRUE;
            } else if (*lpContent) {
                lpContent++;
            }

            /* on l'affiche */
            while (lpLastWord != lpContent) {
                fputc(*lpLastWord, pOutput);
                lpLastWord++;
            }

            if (iNewLine) {
                fputc('\n', pOutput);
                iNbCharsWritten=0;

                /* on écrit la marge d'abord */
                while (iNbCharsWritten<iMarginWidth) {
                    fputc(' ', pOutput);
                    iNbCharsWritten++;
                }

                iNewLine=FALSE;
                lpContent++;
            }

            iNbCharsWritten++;
        }

    }

    return 0;

}

int         Tea_OutputPage(FILE* pOutput, TEAOUTPUT* lpTeaOutput, TEAPAGE* lpTeaPage)
{
    while (lpTeaPage) {
        switch (lpTeaPage->iBlockType) {

            case TEA_BLOCK_PARAGRAPH:
                Tea_OutputParagraph(pOutput , &(lpTeaOutput->teaParagraphStyle) , &(lpTeaOutput->teaOutputMod), lpTeaPage->lpTeaNode);
                break;

            case TEA_BLOCK_CODE:
                Tea_OutputRunModifier(lpTeaOutput->teaOutputMod.lpCodeStart, pOutput, NULL);
                Tea_OutputCode(pOutput, &(lpTeaOutput->teaCodeStyle), lpTeaPage->lpBlockContent);
                Tea_OutputRunModifier(lpTeaOutput->teaOutputMod.lpCodeEnd, pOutput, NULL);
                break;

            case TEA_BLOCK_HEADING:
                Tea_OutputRunModifier(lpTeaOutput->teaOutputMod.lpHeadingStart, pOutput, lpTeaPage->lpBlockContent);
                fprintf(pOutput, "%s", lpTeaPage->lpBlockContent);
                Tea_OutputRunModifier(lpTeaOutput->teaOutputMod.lpHeadingEnd, pOutput, NULL);
                break;

            default:
                printf("Error !");

        }
        fprintf(pOutput, lpTeaOutput->lpBlockSpacing);
        lpTeaPage=lpTeaPage->lpTeaNext;
    }
}

void         Tea_OutputRunModifier(LP_OUTPUT_MODIFIER lpModifier, FILE* pOutput, char* lpContent)
{
    if (lpModifier) {
        lpModifier(pOutput, lpContent);
    }
}

char*       Tea_OutputGetNextChar(char* lpContent)
{

    int iUnicodeState;

    if (teaIsEncodingUtf8) {
        /* système de gestion des caractères UTF-8 */

        if (!(*lpContent & UNICODE_BYTE)) {
            /* il s'agit d'un caractère de la norme ASCII */
            return lpContent+1;
        }

        /* sinon on boucle pour parvenir au prochain caractère */

        iUnicodeState=Tea_IsCharUtf8FolowingByte(lpContent);

        lpContent++; /* si le code est conforme on devrait pas avoir de problème
                        vu que le point doit être suivit par des caractères */
        if (iUnicodeState) {
            /* on part d'un octet suivant, c'est probablement du little-endian
               donc on continue jusqu'au prochain octet non suivant */

            while (Tea_IsCharUtf8FolowingByte(lpContent)==TRUE) {
                lpContent++;
            }

            /* on arrive donc, soit sur octet de tête, ou sur un octet de
                l'alphabet ascii, on vérifie les deux */

            if (!(*lpContent & UNICODE_BYTE)) {
                /* on retourne direct vu que c'est un caractère ascii
                   même si en toute logique, il ne devrait pas être là il
                   faut être prudent pour rattraper les erreurs d'encodage */
                return lpContent;

            } else {
                /* on est bien dans le cas du petit-boutant, donc on passe au bloc suivant */
                return lpContent+1;
            }
        } else {
            /* on part d'un octet de tête, donc il s'agit surement de gros boutant */

            /* on va jusqu'a prochain octet non suivant */
            while (Tea_IsCharUtf8FolowingByte(lpContent)==TRUE) {
                lpContent++;
            }

            return lpContent;
        }

    } else {
        return lpContent+1;
    }

}

int         Tea_IsCharUtf8FolowingByte(char* lpChar)
{
    if ((*lpChar & UNICODE_FOLOWING_BYTE_MASK) == UNICODE_FOLOWING_BYTE_MARK) {
        return TRUE;
    } else {
        return FALSE;
    }
}

int         Hlp_GetNextNodeWLenght(TEANODE* lpNode)
{
    int iReturn=0;
    char* lpToken;

    lpNode=lpNode->lpTeaNodeNext;

    while (lpNode) {

        lpToken=lpNode->lpContent;

        while (*lpToken && *lpToken!=' ') {

            lpToken = Tea_OutputGetNextChar(lpToken);
            iReturn++;

        }

        if (*lpToken==' ') {
             iReturn++;
             break;
        }

        lpNode=lpNode->lpTeaNodeNext;

    }

    return iReturn;
}
