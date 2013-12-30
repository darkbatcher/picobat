#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "libDos9.h"
#include "Tea_Lib.h"
#include "tea_modes.h"

const char* lpTeaOpenDelimiters[]={"{{" , "${", NULL};
const char* lpTeaCloseDelimiters[]={"}}", NULL};
const char* lpNewParagraphMark[]={"\n", NULL};


TEAPAGE*    Tea_PageLoad(const char* lpFilename, const TEAMODIFIERS* lpTeaMod)
{
    FILE* pFile;
    ESTR* lpEstr=Dos9_EsInit();
    ESTR* lpTotalFile=Dos9_EsInit();
    TEAPAGE *lpTeaPage;

    int iFirstline=1;

    /* ouverture du fichier */
    if (!(pFile=fopen(lpFilename, "r"))) return NULL;

    /* récupération de tout le contenu du fichier */
    while (!(Dos9_EsGet(lpEstr, pFile))) {

        if (teaIsEncodingUtf8==TRUE && !strncmp(Dos9_EsToChar(lpEstr), "\xEF\xBB\xBF", 3) && iFirstline) {

            if (*(Dos9_EsToChar(lpEstr)+3)!='#') Dos9_EsCat(lpTotalFile, Dos9_EsToChar(lpEstr)+3);
            iFirstline=0;
            continue;

        }

        if (*Dos9_EsToChar(lpEstr)=='#') continue ; /* a line begining with '#' is a comment */
        Dos9_EsCatE(lpTotalFile, lpEstr);
    }

    /* fermeture du fichier */
    fclose(pFile);

    if (lpTeaMod==&teaHTMLMods) {

        Dos9_EsReplace(lpTotalFile, "&", "&amp;");
        Dos9_EsReplace(lpTotalFile, ">", "&gt;");
        Dos9_EsReplace(lpTotalFile, "<", "&lt;");

    }

    lpTeaPage=Tea_ParseStringBlock(Dos9_EsToChar(lpTotalFile), lpTeaMod); /* on parse le fichier */
    Tea_PurifyPage(lpTeaPage); /* on nettoie la page des espaces inutiles et on re-découpe les paragraphes */
    lpTeaPage=Tea_RemoveVoidBlocks(lpTeaPage, lpTeaMod); /* on supprime les blocs vides */
    Tea_ParseParagraphs(lpTeaPage, lpTeaMod);

    Dos9_EsFree(lpTotalFile);
    Dos9_EsFree(lpEstr);

    return lpTeaPage;
}

TEAPAGE*    Tea_ParseStringBlock(char* lpContent, const TEAMODIFIERS* lpTeaMod)
{
    char *lpNextToken;
    int iTokenFound;
    TEAPAGE *lpTeaPage=NULL, *lpTeaPageBegin=NULL;

    lpTeaPageBegin=Tea_AllocTeaPage();
    lpTeaPage=lpTeaPageBegin;

    while ((lpNextToken=Tea_SeekNextDelimiter(lpContent, lpTeaOpenDelimiters, &iTokenFound))) {
        /* a chaque nouveau block trouvé, on effectue un parsage sur ce block */

        /* on stocke la chaine précédente dans un bloc de type paragraphe */
        *lpNextToken='\0';

        if (!(lpTeaPage->lpBlockContent=strdup(lpContent))) {
            perror("TEA :: impossible d'allouer de la mémoire ");
            exit(-1);
        }

        /* on alloue un nouveau block pour stocker le block trouvé */
        lpTeaPage->lpTeaNext=Tea_AllocTeaPage();
        lpTeaPage=lpTeaPage->lpTeaNext;

        /* on passe au block suivant */
        lpContent=lpNextToken+2;

        switch (iTokenFound) {
            case 0:
                /* le block est forcément un titre */
                lpTeaPage->iBlockType=TEA_BLOCK_HEADING;

                /* la fermeture du block est forcément '}}' */
                if (!(lpNextToken=Tea_SeekNextDelimiter(lpContent, lpTeaCloseDelimiters, &iTokenFound))) {
                    fprintf(stderr ,"TEA :: Bloc non terminé :\n%s\n", lpContent);
                    exit(-1);
                }

                /* on termine la chaine */
                *lpNextToken='\0';

                /* on fini de remplir la structure du bloc */

                if (!(lpTeaPage->lpBlockContent=strdup(lpContent))) {
                    perror("TEA :: impossible d'allouer de la mémoire ");
                    exit(-1);
                }

                if (lpTeaMod->lpHeadingModifier) {
                    lpTeaPage->lpBlockContent=lpTeaMod->lpHeadingModifier(lpTeaPage->lpBlockContent);
                }

                /* on prépare lpCotent pour la prochaine boucle */
                lpContent=lpNextToken+2;

                break;

            case 1:
                /* le block est focément un bloc de code */
                lpTeaPage->iBlockType=TEA_BLOCK_CODE;

                /* la fermeture du block est forcément '}}' */
                if (!(lpNextToken=Tea_SeekNextClosingBrace(lpContent))) {
                    fprintf(stderr ,"TEA :: Bloc non terminé :\n%s\n", lpContent);
                    exit(-1);
                }

                /* on termine la chaine */
                *lpNextToken='\0';

                /* on fini de remplir la structure du bloc */
                if (!(lpTeaPage->lpBlockContent=strdup(lpContent))) {
                    perror("TEA :: impossible d'allouer de la mémoire ");
                    exit(-1);
                }

                /* on prépare lpCotent pour la prochaine boucle */
                lpContent=lpNextToken+1;

        }

        /* on prépare lpTeaPage pour la prochaine boucle */
        lpTeaPage->lpTeaNext=Tea_AllocTeaPage();
        lpTeaPage=lpTeaPage->lpTeaNext;

    }

    /* on finit le dernier block */

    lpTeaPage->lpBlockContent=strdup(lpContent);

    return lpTeaPageBegin;
}


TEAPAGE* Tea_AllocTeaPage(void)
{
    TEAPAGE* lpTeaPage;

    if (lpTeaPage=malloc(sizeof(TEAPAGE))) {
        lpTeaPage->iBlockType=TEA_BLOCK_PARAGRAPH;
        lpTeaPage->lpBlockContent=NULL;
        lpTeaPage->lpTeaNext=NULL;
        lpTeaPage->lpTeaNode=NULL;

        return lpTeaPage;

    } else {
        perror("Erreur ! allocation impossible : %s");
        exit(-1);
    }
}

void   Tea_FreeTeaPage(TEAPAGE* lpTeaPage, const TEAMODIFIERS* lpTeaMod)
{
    if (lpTeaPage->iBlockType==TEA_BLOCK_HEADING && lpTeaMod->lpHeadingFree) {
        lpTeaMod->lpHeadingFree(lpTeaPage->lpBlockContent);
    } else {
        free(lpTeaPage->lpBlockContent);
    }
    free(lpTeaPage);
}

int    Tea_PageFree(TEAPAGE* lpTeaPage, const TEAMODIFIERS* lpTeaMod)
{
    TEAPAGE* lpTeaTmp;
    TEANODE *lpTeaNode, *lpTeaNodeTmp;

    while (lpTeaPage)
    {
        if (lpTeaPage->iBlockType == TEA_BLOCK_PARAGRAPH) {

            lpTeaNode=lpTeaPage->lpTeaNode;

            while (lpTeaNode) {
                lpTeaNodeTmp=lpTeaNode->lpTeaNodeNext;
                Tea_FreeTeaNode(lpTeaNode, lpTeaMod);
                lpTeaNode=lpTeaNodeTmp;
            }

        }

        lpTeaTmp=lpTeaPage->lpTeaNext;
        Tea_FreeTeaPage(lpTeaPage, lpTeaMod);
        lpTeaPage=lpTeaTmp;
    }
    return 0;
}

char* Tea_SeekNextDelimiter(const char* lpBeginPos, const char** lpToken, int* iDelimPos)
{
    char* lpDelimPos=NULL;
    char* lpTokenPos;
    int iTokenPos=0;

    while (*lpToken) {
        lpTokenPos=strstr(lpBeginPos, *lpToken);
        if ((((unsigned int) lpTokenPos < (unsigned int)lpDelimPos)  || lpDelimPos==NULL) && lpTokenPos) {
            lpDelimPos=lpTokenPos;
            *iDelimPos=iTokenPos;
        }
        lpToken++;
        iTokenPos++;
    }
    return lpDelimPos;
}

char*       Tea_SeekNextClosingBrace(const char* lpBeginPos)
{
    int iBraceLvl=0;

    while (*lpBeginPos) {
        switch (*lpBeginPos) {
            case '\\':
                if (*(lpBeginPos+1)) {
                    lpBeginPos+=2;
                    continue;
                } else {
                    return 0;
                }

            case '{':
                iBraceLvl++;
                break;

            case '}':
                if (!iBraceLvl) {
                    return lpBeginPos;
                } else {
                    iBraceLvl--;
                }

                break;
        }
        lpBeginPos++;
    }

    return NULL;
}

int         Tea_PurifyPage(TEAPAGE* lpTeaPage)
{
    while (lpTeaPage) {
        if (lpTeaPage->iBlockType==TEA_BLOCK_PARAGRAPH) {
            Tea_BreakParagraph(lpTeaPage); /* découpe le paragraphe en sous paragraphe s'il y a lieu */
        } else if (lpTeaPage->iBlockType==TEA_BLOCK_HEADING) {
            Tea_SweepSpace(lpTeaPage->lpBlockContent); /* on supprimme les espaces superflus */
        } else if (lpTeaPage->iBlockType==TEA_BLOCK_CODE) {
            Tea_SweepCodeTabs(lpTeaPage->lpBlockContent);
        }
        lpTeaPage=lpTeaPage->lpTeaNext;
    }
    return 0;
}

void        Tea_ParseParagraphs(TEAPAGE* lpTeaPage, const TEAMODIFIERS* lpTeaMod)
{
    while (lpTeaPage) {
        if (lpTeaPage->iBlockType==TEA_BLOCK_PARAGRAPH) {
            lpTeaPage->lpTeaNode=Tea_ParseStringNode(lpTeaPage->lpBlockContent, lpTeaMod); /* on parse le contenu du bloc en sous chaine */
            Tea_PurifyNode(lpTeaPage->lpTeaNode); /* on nettoie les espaces superflus */
            lpTeaPage->lpTeaNode=Tea_RemoveVoidNode(lpTeaPage->lpTeaNode, lpTeaMod); /* on nettoie les noeuds vides */
        }
        lpTeaPage=lpTeaPage->lpTeaNext;
    }
}

int         Tea_BreakParagraph(TEAPAGE* lpTeaPage)
{
    char *lpContent=lpTeaPage->lpBlockContent, *lpNextToken, *lpBeginSearch, *lpToken;
    int iTokenNb;
    TEAPAGE* lpTeaTemp;

    lpBeginSearch=lpContent;

    while (lpNextToken=Tea_SeekNextDelimiter(lpBeginSearch, lpNewParagraphMark, &iTokenNb)) {


        /* on vérifie si le token correspond */
        lpToken=lpNextToken+1;
        while (*lpToken==' ' || *lpToken=='\t') lpToken++;

        if (*lpToken!='\n') {
            lpBeginSearch=lpNextToken+1;
            continue;
        }

        /* on isole le token */
        *lpNextToken='\0';

        /* on crée un nouveau paragraphe */
        lpTeaTemp=Tea_AllocTeaPage();
        lpTeaTemp->lpTeaNext=lpTeaPage->lpTeaNext;

        /* on stocke le contenu du nouveau paragraphe dans le nouveau paragraphe alloué */
        lpContent=lpNextToken+2;
        if (!(lpTeaTemp->lpBlockContent=strdup(lpContent))) {
            perror("Erreur : impossible d'allouer de la mémoire : %s");
            exit(-1);
        }

        /* on lie le tout */
        lpTeaPage->lpTeaNext=lpTeaTemp;

        /* on met a jour lpTeaPage et lpContent pour la prochaine boucle */
        lpTeaPage=lpTeaTemp;
        lpContent=lpTeaPage->lpBlockContent;
        lpBeginSearch=lpContent;

    }
    return 0;
}

TEAPAGE*    Tea_RemoveVoidBlocks(TEAPAGE* lpTeaPage, const TEAMODIFIERS* lpTeaMod)
{
    TEAPAGE *lpTeaBegin=lpTeaPage, *lpTeaPrevious=NULL;
    char* lpContent;

    while (lpTeaPage) {
        lpContent=lpTeaPage->lpBlockContent;

        while (*lpContent==' ' || *lpContent=='\t' || *lpContent=='\n') lpContent++;

        if (*(lpContent)=='\0') {
            /* si le block est vide */
            if (lpTeaPage==lpTeaBegin) {
                /* si il s'agit du premier block du document */
                lpTeaBegin=lpTeaPage->lpTeaNext;
                Tea_FreeTeaPage(lpTeaPage, lpTeaMod);

                /* on reprend la boucle au début */
                lpTeaPage=lpTeaBegin;
                continue;

            } else {
                lpTeaPrevious->lpTeaNext=lpTeaPage->lpTeaNext;

                Tea_FreeTeaPage(lpTeaPage, lpTeaMod);

                lpTeaPage=lpTeaPrevious->lpTeaNext;

                continue;
            }

        }
        lpTeaPrevious=lpTeaPage;
        lpTeaPage=lpTeaPage->lpTeaNext;
    }

    return lpTeaBegin;
}

int         Tea_SweepSpace(char* lpContent)
{
    int iSpaceGroup=TRUE;
    char* lpNextContent=lpContent;

    while (*lpContent) {
        switch (*lpContent) {
            case '\\':
                if (*(lpContent+1)) {
                    lpContent++;
                } else {
                    return 0;
                }
                break;

            case '\n':
            case '\t':
                *lpContent=' ';

            case ' ':
                if (iSpaceGroup) {
                    lpContent++;
                    continue;
                } else {
                    iSpaceGroup=TRUE;
                }
                break;

            default:
                iSpaceGroup=FALSE;
        }
        *lpNextContent=*lpContent;
        lpNextContent++;
        lpContent++;
    }
    *lpNextContent=*lpContent;
    return 0;
}

int Tea_SweepCodeTabs(char* lpContent)
{
    while (*lpContent) {
        if (*lpContent=='\t') *lpContent=' ';
        lpContent++;
    }
    return 0;
}
