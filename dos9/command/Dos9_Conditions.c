#include "Dos9_CmdLib.h"
#include "Dos9_Conditions.h"
#include "../lang/Dos9_Lang.h"
#include "../errors/Dos9_Errors.h"
#include "../core/Dos9_Run.h"

int Dos9_CmdIf(char* lpParam)
{
    char lpArgument[FILENAME_MAX], *lpNext, *lpToken;
    int iFlag=0, iResult;
    char cCharSave;
    CMPTYPE cmpCompType;
    ESTR *lpComparison, *lpOtherPart;
    LPFILELIST lpflFileList;
    /* syntaxe possible
            IF [/i] [NOT] test==valeur (...)
            IF [/i] test2 CMP test2 (...)
            IF [NOT] EXIST fichier (...)
            IF [NOT] DEFINED var (...)
            IF [NOT] ERRORLEVEL code (...) -> DEPRECATED !
    */
    lpParam+=2;

    if ((lpNext=Dos9_GetNextParameter(lpParam, lpArgument, 11))) {
        if (!strcmp(lpArgument, "/?")) {
            puts(lpHlpDeprecated);
            return 0;
        }
        if (!stricmp(lpArgument, "/i")){
            iFlag|=DOS9_IF_CASE_UNSENSITIVE;
            lpParam=lpNext;
            if (!(lpNext=Dos9_GetNextParameter(lpNext, lpArgument, 11))) {
                Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);
                return 0;
            }
        }

        if (!stricmp(lpArgument, "NOT")) {
            iFlag|=DOS9_IF_NEGATION;
            lpParam=lpNext;
            if (!(lpNext=Dos9_GetNextParameter(lpNext, lpArgument, 11))) {
                Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);
                return 0;
            }
        }

        if (!stricmp(lpArgument, "EXIST")) {
            iFlag|=DOS9_IF_EXIST;
        } else if (!stricmp(lpArgument, "DEFINED")) {
            iFlag|=DOS9_IF_DEFINED;
        } else if (!stricmp(lpArgument, "ERRORLEVEL")) {
            iFlag|=DOS9_IF_ERRORLEVEL;
        }

        if ((iFlag & DOS9_IF_CASE_UNSENSITIVE) && (iFlag & (DOS9_IF_ERRORLEVEL | DOS9_IF_EXIST | DOS9_IF_DEFINED))) {
            Dos9_ShowErrorMessage(DOS9_INCOMPATIBLE_ARGS, "'/i' (DEFINED, EXIST, ERRORLEVEL)", FALSE);
            return 0;
        }
        if (iFlag & (DOS9_IF_EXIST | DOS9_IF_DEFINED | DOS9_IF_ERRORLEVEL)) lpParam=lpNext;
        //lpParam=lpNext;

    }

    if (iFlag & DOS9_IF_ERRORLEVEL) {
        /* the script used 'ERRORLEVEL' Keyword */
        if (!(lpNext=Dos9_GetNextParameter(lpParam, lpArgument, FILENAME_MAX))) {
            Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);
            return -1;
        }
        iResult=!strcmp(getenv("ERRORLEVEL"), lpArgument);
        if (iFlag & DOS9_IF_NEGATION) iResult=!iResult;
        lpParam=lpNext;


    } else if (iFlag & DOS9_IF_EXIST) {
        /* the script used 'EXIST' keyWord */
        if (!(lpNext=Dos9_GetNextParameter(lpParam, lpArgument, FILENAME_MAX))) {
            Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);
            return -1;
        }
        lpflFileList=Dos9_GetMatchFileList(lpArgument, DOS9_SEARCH_DEFAULT);
        iResult=(int)lpflFileList;
        if (lpflFileList) Dos9_FreeFileList(lpflFileList);
        if (iFlag & DOS9_IF_NEGATION) iResult=!iResult;
        lpParam=lpNext;

    } else if (iFlag & DOS9_IF_DEFINED) {
        /* the script used 'DEFINED' */
        if (!(lpNext=Dos9_GetNextParameter(lpParam, lpArgument, FILENAME_MAX))) {
            Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);
            return -1;
        }
        iResult=(int)getenv(lpArgument);
        if (iFlag & DOS9_IF_NEGATION) iResult=!iResult;
        lpParam=lpNext;
    } else {
        /* the script uses normal comparisons */
        lpComparison=Dos9_EsInit();
        if ((lpNext=Dos9_GetNextParameterEs(lpParam, lpComparison))) {

            if ((lpToken=strstr(Dos9_EsToChar(lpComparison), "=="))) {
                /* if scipt uses old c-style comparison */
                *lpToken='\0';
                lpToken+=2;

                if (iFlag & DOS9_IF_CASE_UNSENSITIVE) iResult=!stricmp(Dos9_EsToChar(lpComparison), lpToken);
                else iResult=!strcmp(Dos9_EsToChar(lpComparison), lpToken);

                if (iFlag & DOS9_IF_NEGATION) iResult=!iResult;


            } else {
                /* if script uses new basic-like comparison (e.g. EQU, NEQ ...) */

                if (!(lpNext=Dos9_GetNextParameter(lpNext, lpArgument, 11))) {
                    Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);
                    return -1;
                }

                if (!stricmp(lpArgument, "EQU")) {
                    cmpCompType=CMP_EQUAL;
                } else if (!stricmp(lpArgument, "NEQ")) {
                    cmpCompType=CMP_DIFFERENT;
                } else if (!stricmp(lpArgument, "GEQ")) {
                    cmpCompType=CMP_GREATER_OR_EQUAL;
                } else if (!stricmp(lpArgument, "GTR")) {
                    cmpCompType=CMP_GREATER;
                } else if (!stricmp(lpArgument, "LEQ")) {
                    cmpCompType=CMP_LESSER_OR_EQUAL;
                } else if (!stricmp(lpArgument, "LSS")) {
                    cmpCompType=CMP_LESSER_OR_EQUAL;
                } else {
                    Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpArgument, FALSE);
                    return -1;
                }

                lpOtherPart=Dos9_EsInit();

                if (!(lpNext=Dos9_GetNextParameterEs(lpNext, lpOtherPart))) {
                    Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", TRUE);
                    return -1;
                }

                switch (cmpCompType) {
                    case CMP_EQUAL:
                        if (iFlag & DOS9_IF_CASE_UNSENSITIVE) iResult=!stricmp(Dos9_EsToChar(lpOtherPart), Dos9_EsToChar(lpComparison));
                        else iResult=!strcmp(Dos9_EsToChar(lpOtherPart), Dos9_EsToChar(lpComparison));
                        break;

                    case CMP_DIFFERENT:
                        if (iFlag & DOS9_IF_CASE_UNSENSITIVE) iResult=stricmp(Dos9_EsToChar(lpOtherPart), Dos9_EsToChar(lpComparison));
                        else iResult=strcmp(Dos9_EsToChar(lpOtherPart), Dos9_EsToChar(lpComparison));
                        break;

                    case CMP_GREATER:
                        iResult = (atoi(Dos9_EsToChar(lpComparison)) > atoi(Dos9_EsToChar(lpOtherPart)));
                        break;

                    case CMP_GREATER_OR_EQUAL:
                        iResult = (atoi(Dos9_EsToChar(lpComparison)) >= atoi(Dos9_EsToChar(lpOtherPart)));
                        break;

                    case CMP_LESSER:
                        iResult = (atoi(Dos9_EsToChar(lpComparison)) < atoi(Dos9_EsToChar(lpOtherPart)));
                        break;

                    case CMP_LESSER_OR_EQUAL:
                        iResult = (atoi(Dos9_EsToChar(lpComparison)) <= atoi(Dos9_EsToChar(lpOtherPart)));
                        break;
                }
                Dos9_EsFree(lpOtherPart);
            }
        } else {
            Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);
            Dos9_EsFree(lpComparison);
            return -1;
        }
        lpParam=lpNext;
        Dos9_EsFree(lpComparison);

    }

    if ((lpToken=Dos9_GetNextBlock(lpParam, &lpNext))) {
        if (iResult) {
            cCharSave=*lpNext;
            *lpNext='\0';

            Dos9_RunBlock(lpToken);

            *lpNext=cCharSave;
        } else {

            //puts("COMMAND[IF] ::  Looking for an else ");
            //printf("COMMAND[IF] :: Next content {%s}\n", lpNext);
            if (!strnicmp(lpNext, ") ELSE (", sizeof(") ELSE (")-1)) {

                lpNext+=(sizeof(") ELSE (")-2);
                //puts("COMMAND[IF] :: Else found");
                if ((lpToken=Dos9_GetNextBlock(lpNext, &lpNext))) {

                    cCharSave=*lpNext;
                    *lpNext='\0';
                    //printf("Runing block {%s}\n", lpToken);
                    Dos9_RunBlock(lpToken);

                    *lpNext=cCharSave;
                } else {
                    Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);
                }
            }
        }
    } else {
        Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);
        return -1;
    }

    return 0;
}
