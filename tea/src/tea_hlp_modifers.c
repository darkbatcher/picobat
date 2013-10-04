#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tea_modifier.h"


char*   tea_HlpModifierLink(char* lpContent, char* lpTarget)
{
    size_t iSizeAllocate=sizeof(" [")+sizeof("]")+10;
    char* lpModifier;
    static int iNbLink=0;
    /* on calcule la taille qu'il faut allouer */

    iSizeAllocate+=strlen(lpContent);

    /* on alloue l'espace nécessaire */

    if (!(lpModifier=malloc(iSizeAllocate))) {
        perror("TEA tea_HlpModifierLink() :: impossible d'allouer de la memoire ");
        exit(-1);
    }

    sprintf(lpModifier, "%s [%d]", lpContent, iNbLink++);

    return lpModifier;
}

void   tea_HlpHeadingStart(FILE* pOut, char* pTitle)
{
    static int iIsFirst=1;
    FILE* pDat;

    if (iIsFirst) {
        fputc(TEA_HLP_TITLE_COLOR, pOut);
        if ((pDat=fopen("makeman.tmp", "a"))) {
            fprintf(pDat ,"title:%s\n", pTitle);
            fclose(pDat);
        }

        iIsFirst=0;
    } else {
        fputc(TEA_HLP_TITLE_COLOR, pOut);
    }
}

void   tea_HlpCodeStart(FILE* pOut, char* pTitle)
{
    fputc(TEA_HLP_CODE_COLOR, pOut);
}

void   tea_HlpEmphasisStart(FILE* pOut, char* pTitle)
{
    fputc(TEA_HLP_EM_COLOR, pOut);
}

void   tea_HlpLinkStart(FILE* pOut, char* lpTarget)
{
    static int i=0;
    FILE* pDat;

    fputc(TEA_HLP_LINK_COLOR, pOut);
    if ((pDat=fopen("makeman.tmp", "a"))) {
        fprintf(pDat ,"link[%d]:%s\n", i, lpTarget);
        fclose(pDat);
    }

    i++;
}

void   tea_HlpEnd(FILE* pOut, char* lpTarget)
{
    fputc(TEA_HLP_DEFAULT_COLOR, pOut);
}
