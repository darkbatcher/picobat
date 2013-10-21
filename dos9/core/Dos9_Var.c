#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "LibDos9.h"

#include "Dos9_Core.h"

#include "../errors/Dos9_Errors.h"


/* this file contains code of function used for parsing variables content
   (e.g. %~* and !:*! */

#ifndef WIN32

void strupr(char* lpBuf)
{
    while (*lpBuf) {
        *lpBuf=toupper(*lpBuf);
        lpBuf++;
    }
}

#endif


int Dos9_InitVar(char* lpArray[])
{
    int i;
    for (i=0;lpArray[i];i++) {
        Dos9_PutEnv(lpArray[i]);
    }
    return 0;
}

int Dos9_GetVar(char* lpName, ESTR* lpRecieve)
{
    char        *lpVarContent, /* a pointer to the environment var string */
                *lpToken, /* a pointer used to tokenize vars like %var:a=b% */
                *lpNextToken=NULL, /* a pointer used to tokenize '=' or ',' in vars like %var:a=b% */
                *lpNameCpy, /* a pointer used to duplicate lpName (because function should avoid bordering effect)*/
                *lpZeroPos=NULL;; /* a pointer to the zero put in the environment string */

    char        lpBuf[12];
    int         iVarState=0, /* the status of the var interpreter
                                1 means replace
                                2 means cut */
                iTotalLen,
                iBegin=0, /* the start position */
                iLen=0; /* the lenght to be cut */
    char        cCharSave=0; /* the backup of the character replaced by '\0' */;
    struct tm* lTime;
    time_t iTime;

    if (!(lpNameCpy=strdup(lpName))) return FALSE;
    if ((lpToken=strchr(lpNameCpy, ':'))) {
        if ((lpNextToken=strchr(lpToken, '='))) {
            /* char are about to be replaced */
            *lpToken='\0';
            lpToken++;
            *lpNextToken='\0';
            lpNextToken++;
            iVarState=1;
        } else if (*(lpToken+1)=='~') {
            /* string is about to be truncated */
            *lpToken='\0';
            lpToken+=2;
            if ((lpNextToken=strchr(lpToken, ','))) {
                *lpNextToken='\0';
                lpNextToken++;
                iLen=atol(lpNextToken);
            }
            iBegin=atol(lpToken);
            iVarState=2;
        }
    }

    #ifndef _POSIX_C_SOURCE

        /* Non windows systems are case sensitive, and thus,
           capitalization of charcter is needed */
        strupr(lpNameCpy);

    #endif

    if (!stricmp(lpNameCpy, "RANDOM")) {
            /* requested TIME */
        lpVarContent=lpBuf;
        sprintf(lpBuf, "%d", rand());
    } else if (!stricmp(lpNameCpy, "DATE")) {
        iTime=time(NULL);
        lTime=localtime(&iTime);
        lpVarContent=lpBuf;
        sprintf(lpBuf, "%02d/%02d/%02d", lTime->tm_mday, lTime->tm_mon+1, lTime->tm_year+1900);
    } else if (!stricmp(lpNameCpy, "TIME")) {
        iTime=time(NULL);
        lTime=localtime(&iTime);
        lpVarContent=lpBuf;
        sprintf(lpBuf, "%02d:%02d:%02d,00", lTime->tm_hour, lTime->tm_min, lTime->tm_sec);
    } else if (!(lpVarContent=getenv(lpNameCpy))) {
        free(lpNameCpy);
        return FALSE;

    }

    iTotalLen=strlen(lpVarContent);

    if (iVarState==2) {
        if (iBegin<0 || iBegin>= iTotalLen) {
            /* skip because these values are not valid
                indeed iBegin must not be negative and
                must not overflow the buffer */
        } else if (iLen>=0) {
            if ((iBegin+iLen)<= iTotalLen) {
                /* if the strings is right */
                lpZeroPos=lpVarContent+iBegin+iLen;
                cCharSave=*lpZeroPos;
                *lpZeroPos='\0';
                lpVarContent+=iBegin;
            }
        } else if (iLen < 0) {
            if (abs(iLen) <= iTotalLen-iBegin){
                /* if the string is reight too */
                lpZeroPos=lpVarContent+iTotalLen+iLen;
                cCharSave=*lpZeroPos;
                *lpZeroPos='\0';
                lpVarContent+=iBegin;
            }
        }
    }

    Dos9_EsCpy(lpRecieve, lpVarContent);

    if (iVarState==1) {
        Dos9_EsReplace(lpRecieve, lpToken, lpNextToken);
    }

    if (iVarState==2) {
        *lpZeroPos=cCharSave;
    }

    free(lpNameCpy);

    return TRUE;
}

LOCAL_VAR_BLOCK* Dos9_GetLocalBlock(void)
{
    return (LOCAL_VAR_BLOCK*)calloc(LOCAL_VAR_BLOCK_SIZE, sizeof(LOCAL_VAR_BLOCK));
}

void Dos9_FreeLocalBlock(LOCAL_VAR_BLOCK* lpBlock)
{
    return free(lpBlock);
}

int Dos9_SetLocalVar(LOCAL_VAR_BLOCK* lpvBlock, char cVarName, char* cVarContent)
{

    DOS9_TEST_VARNAME(cVarName);
        /* Perform test on value cName, to test its
        specification conformance, i.e. the character must be
        a strict ASCII character, exculuding control characters
        and space (code range from 0x00 to 0x30 included) */

    if (lpvBlock[(int)cVarName]) free(lpvBlock[(int)cVarName]);

    lpvBlock[(int)cVarName]=strdup(cVarContent);

    return 0;
}


char* Dos9_GetLocalVar(LOCAL_VAR_BLOCK* lpvBlock, char* lpName, ESTR* lpRecieve)
{
    char *lpPos, *lpNext;
    char lpDrive[_MAX_DRIVE], lpDir[_MAX_DIR], lpFileName[_MAX_FNAME], lpExt[_MAX_EXT];
    char cFlag[DOS9_VAR_MAX_OPTION+1]={DOS9_ALL_PATH};
    char lpBuffer[FILENAME_MAX];
    char bSeekFile=FALSE, bSplitPath=FALSE;

    struct tm* lTime;
    struct stat stFileInfo;

    int i=0;

    Dos9_EsCpy(lpRecieve, "");

    if (*lpName!='~') {

        DOS9_TEST_VARNAME(*lpName);

        if (!lpvBlock[(int)*lpName]) return NULL;
        Dos9_EsCpy(lpRecieve, lpvBlock[(int)*lpName]);
        return lpName+1;
    }

    lpName++;
    if (!*lpName) return NULL;

    for (;*(lpName+1) && !isblank(*(lpName+1)) && i<DOS9_VAR_MAX_OPTION;lpName++) {
        if (isdigit(*lpName)) {
            break;
        }
        switch(*lpName | 32)
        {
            case 'd':
                cFlag[i]=DOS9_DISK_LETTER;
                i++;
                bSplitPath=TRUE;
                break;
            case 'n':
                cFlag[i]=DOS9_FILENAME;
                i++;
                bSplitPath=TRUE;
                break;
            case 'p':
                cFlag[i]=DOS9_PATH_PART;
                i++;
                bSplitPath=TRUE;
                break;
            case 'x':
                cFlag[i]=DOS9_EXTENSION;
                i++;
                bSplitPath=TRUE;
                break;
            case 'z':
                cFlag[i]=DOS9_FILESIZE;
                i++;
                bSeekFile=TRUE;
                break;
            case 't':
                cFlag[i]=DOS9_TIME;
                i++;
                bSeekFile=TRUE;
                break;
            case 'a':
                cFlag[i]=DOS9_ATTR;
                i++;
                bSeekFile=TRUE;
                break;
            default:
                Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpName, FALSE);
                return NULL;
        }
    }

    DOS9_TEST_VARNAME(*lpName);

    if (!lpvBlock[(int)*lpName]) return NULL;

    Dos9_EsCpy(lpRecieve, lpvBlock[(int)*lpName]);
    lpPos=Dos9_EsToChar(lpRecieve);

    if (*lpPos=='"') lpPos++;
    if ((lpNext=strchr(lpPos, '"'))) *lpNext='\0';

    if (bSeekFile) {
        stat(lpPos, &stFileInfo);
        #if defined WIN32
            stFileInfo.st_mode=Dos9_GetFileAttributes(lpPos);
        #endif
    }

    if (bSplitPath) Dos9_SplitPath(lpPos, lpDrive, lpDir, lpFileName, lpExt);

    if (cFlag[0]!=DOS9_ALL_PATH) {
        Dos9_EsCpy(lpRecieve, "");
        for (i=0;cFlag[i]!=0; i++) {
            switch (cFlag[i]) {
                case DOS9_DISK_LETTER:
                    Dos9_EsCat(lpRecieve, lpDrive);
                    break;

                case DOS9_PATH_PART:
                    Dos9_EsCat(lpRecieve, lpDir);
                    break;

                case DOS9_FILENAME:
                    Dos9_EsCat(lpRecieve, lpFileName);
                    break;

                case DOS9_EXTENSION:
                    Dos9_EsCat(lpRecieve, lpExt);
                    break;

                case DOS9_FILESIZE:
                    sprintf(lpBuffer, "%d%c", (int)stFileInfo.st_size, (cFlag[i+1]!=0 ? '\t' : '\0'));
                    Dos9_EsCat(lpRecieve, lpBuffer);
                    break;

                case DOS9_TIME:
                    lTime=localtime(&stFileInfo.st_atime);
                    sprintf(lpBuffer, "%02d/%02d/%02d %02d:%02d%c", lTime->tm_mday , lTime->tm_mon+1, 1900+lTime->tm_year, lTime->tm_hour, lTime->tm_min, (cFlag[i+1]!=0 ? '\t' : '\0'));
                    Dos9_EsCat(lpRecieve, lpBuffer);
                    break;

                case DOS9_ATTR:
                    sprintf(lpBuffer, "       %c", (cFlag[i+1]!=0 ? '\t': '\0'));
                    if (stFileInfo.st_mode & DOS9_FILE_DIR) lpBuffer[0]='D';
                    if (stFileInfo.st_mode & DOS9_FILE_READONLY) lpBuffer[2]='R';
                    if (stFileInfo.st_mode & DOS9_FILE_HIDDEN) lpBuffer[3]='H';
                    if (stFileInfo.st_mode & DOS9_FILE_SYSTEM) lpBuffer[4]='S';
                    if (stFileInfo.st_mode & DOS9_FILE_ARCHIVE) lpBuffer[5]='A';
                    Dos9_EsCat(lpRecieve, lpBuffer);
                    break;
            }
        }
    }
    return lpName+1;
}

#ifndef WIN32


int Dos9_PutEnv(char* lpEnv)
{
    char *lpEnvCpy, *lpToken;
    char lpVoid[]="";
    int iRet;
    if (!(lpEnvCpy=strdup(lpEnv))) return -1;
    if ((lpToken=strchr(lpEnvCpy, '='))) {
        *lpToken='\0';
        lpToken++;
    } else {
        lpToken=lpVoid;
    }
    strupr(lpEnvCpy);
    iRet=setenv(lpEnvCpy, lpToken, TRUE);
    free(lpEnvCpy);
    return iRet;
}

#endif
