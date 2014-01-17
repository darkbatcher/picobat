#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <libDos9.h>

int hlp_make_IsEncodingUtf8=1;

char hlp_make_SeekUnclosed(ESTR* lpEsContent)
{
    char* lpContent=Dos9_EsToChar(lpEsContent);
    char cCurrentDelim=0;

    while (*lpContent) {
        switch(*lpContent) {

            case '\x10':
                cCurrentDelim=0;
                break;

            case '\x11':
            case '\x12':
            case '\x13':
            case '\x14':
                cCurrentDelim=*lpContent;

        }

        lpContent++;
    }

    return cCurrentDelim;
}

void hlp_make_LoadHead(char *lpName, ESTR* lpContent)
{
    FILE* pFile;
    ESTR* lpEsTmp=Dos9_EsInit();

    if ( !(pFile=fopen(lpName, "r")) ) {
        perror("hlp_make :: erreur, impossible d'ouvir l'en-tete ");
        exit(-1);
    }

    while (!Dos9_EsGet(lpEsTmp, pFile)) {
        Dos9_EsCatE(lpContent, lpEsTmp);
    }

    fclose(pFile);
    Dos9_EsFree(lpEsTmp);
}

int hlp_make_LineLength(const char* lpLine)
{
    int i=0;

    while (*lpLine) {

        switch (*lpLine) {
            case '\x10':
            case '\x11':
            case '\x12':
            case '\x13':
            case '\x14':
                break;

            default:
                i++;
        }

        lpLine=(const char*)Dos9_GetNextChar(lpLine);
    }

    return i;
}

void hlp_make_LoadContent(char *lpName, ESTR* lpContent)
{
    FILE* pFile;
    ESTR* lpEsTmp=Dos9_EsInit();
    ESTR* lpEsAppend=Dos9_EsInit();
    char escapeBuf[]="\x10\n";
    char charBuf[]=" ";
    char cChar=0;
    char* lpToken;

    if ( !(pFile=fopen(lpName, "r")) ) {
        fprintf(stderr, "hlp_make :: %s : erreur, impossible d'ouvir le texte", lpName);
        exit(-1);
    }

    while (!Dos9_EsGet(lpEsTmp, pFile)) {

        if (cChar) {

            charBuf[0]=cChar;

            Dos9_EsCpy(lpEsAppend, charBuf);
            Dos9_EsCatE(lpEsAppend, lpEsTmp);
            Dos9_EsCpyE(lpEsTmp, lpEsAppend);

        }

        if ( (lpToken=strchr(Dos9_EsToChar(lpEsTmp), '\n')) ) {

            *lpToken='\0';

        }

        /* filling line with space */
        while (hlp_make_LineLength(Dos9_EsToChar(lpEsTmp))<79) {

            Dos9_EsCat(lpEsTmp, " ");

        }


        Dos9_EsCat(lpEsTmp, "\n");

        if ( (cChar=hlp_make_SeekUnclosed(lpEsTmp)) ) {

            Dos9_EsReplace(lpEsTmp, "\n", escapeBuf);

        }

        Dos9_EsCatE(lpContent, lpEsTmp);

    }

    fclose(pFile);
    Dos9_EsFree(lpEsTmp);
    Dos9_EsFree(lpEsAppend);
}


 void hlp_make_Output(char* lpContentName, ESTR* lpContent, ESTR* lpHead)
 {
     FILE* pOut;

     if ( !(pOut=fopen(lpContentName, "w+")) ) {
        return;
     }

     fprintf(pOut, "\x01%s\x02%s", Dos9_EsToChar(lpHead), Dos9_EsToChar(lpContent));

     fclose(pOut);
 }

int main(int argc, char *argv[])
{

    if (Dos9_LibInit() == -1) {

        puts("Error : Unable to load LibDos9. Exiting ...");
        exit(-1);
    }

    ESTR* lpHead=Dos9_EsInit();
    ESTR* lpContent=Dos9_EsInit();

    char* lpContentName=NULL;

    if (argc<1) {
            Dos9_EsFree(lpHead);
            Dos9_EsFree(lpContent);
            return -1;
    }

    lpContentName=argv[1];

    Dos9_SetNewLineMode(DOS9_NEWLINE_LINUX);

    hlp_make_LoadHead("makeman.tmp", lpHead);

    hlp_make_LoadContent(lpContentName,  lpContent);

    hlp_make_Output(lpContentName, lpContent, lpHead);


    Dos9_EsFree(lpHead);
    Dos9_EsFree(lpContent);

    return 0;
}
