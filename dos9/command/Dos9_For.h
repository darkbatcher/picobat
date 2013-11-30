#ifndef DOS9_FOR_H
#define DOS9_FOR_H

#include "Dos9_CmdLib.h"

#define TOKENINFO_NB_MAX 128-32
#define MAX_UNICODE_CHAR 5
#define TOKENNB_ALL_REMAINING 0xFFFF

#define HIGHWORD(nb) ((nb & 0xFFFF0000) >> 16)
#define LOWWORD(nb) (nb & 0xFFFF)

#define TOHIGH(nb) (nb <<16)
#define TOLOW(nb) (nb & 0xFFFF)

#define FOR_LOOP_SIMPLE 0
#define FOR_LOOP_R 1
#define FOR_LOOP_F 2
#define FOR_LOOP_L 4

typedef struct FORINFO {
	char lpDelims[FILENAME_MAX];
		/*  the delims */
	char lpEol[FILENAME_MAX];
		/* the eol character, in a unicode character */
	int iSkip;
		/* the number of lines to be skipped */
	char cFirstVar;
		/* position of the first special var */
    int bUsebackq;
    int iTokenNb;
        /* the max value of token information */

	int	lpToken[TOKENINFO_NB_MAX];
		/* an array that describes characteristics of the
		different special var. The syntax is the following :

            [high order word] [low order word]
            [     start     ] [     end      ]

            - if the high order word is set, then the token to
              be considered should be the cast of a range of tokens,
              starting from the p-th token up to n-th token, where
              p is the number stored in the high order word, and n
              the number stored in the low order word

            - if the low order word is set to -1 (0xFFFF), then
              the token is the cast of the p-th and all remaining
              tokens.

		*/

} FORINFO;

typedef struct STRINGINFO {
    char* lpString;
        /* the location of the original string. Used
           for freeing memory */
    char* lpToken;
        /* a token to be processed */
} STRINGINFO;

typedef struct STREAMINFO {
    int iPipeFds[2];
    FILE* pInputStream;
} STREAMINFO;

#define INPUTINFO_TYPE_STREAM 0
#define INPUTINFO_TYPE_STRING 1
#define INPUTINFO_TYPE_COMMAND 2 // unused in the struct

union _INPUTINFO_UNION {
        STRINGINFO StringInfo;
        STREAMINFO StreamInfo;
        FILE* pInputFile;
};

typedef struct INPUTINFO {
    char cType;
    union _INPUTINFO_UNION Info;
} INPUTINFO;



int Dos9_CmdFor(char* lpCommand);
	/* This is the function used for running
	   ``for'' loops */

int Dos9_CmdForSimple(ESTR* lpInput, BLOCKINFO* lpbkCommand, char cVarName);
    /* this handles simple ``for'' loops */

int Dos9_CmdForF(ESTR* lpInput, BLOCKINFO* lpbkCommand, FORINFO* lpfrInfo);
    /* this handles more comple ``for'' loops
       ie. For /F
    */

int Dos9_CmdForL(ESTR* lpInput, BLOCKINFO* lpbkCommand, char cVarName);
    /* this handle the ``for /L'' loop */


int Dos9_ForMakeInfo(char* lpOptions, FORINFO* lpfrInfo);
    /* this makes token from a parameter command
       i.e. from a string like "tokens=1,3,4 delims=, "
    */

void Dos9_ForAdjustParameter(char* lpOptions, ESTR* lpParam);

int  Dos9_ForMakeTokens(char* lpToken, FORINFO* lpfrInfo);

void Dos9_ForSplitTokens(ESTR* lpContent, FORINFO* lpfrInfo);
	/*
       this split a line into different tokens
	   for parsing with tokens

    */

void Dos9_ForGetToken(ESTR* lpContent, FORINFO* lpfrInfo, int iPos, ESTR* lpReturn);
    /*
        gets token from lpContent, and in
        the order of iRange.
    */


int Dos9_ForMakeInputInfo(ESTR* lpInput, INPUTINFO* lpipInfo, FORINFO* lpfrInfo);

int Dos9_ForAdjustInput(char* lpInput);

int Dos9_ForInputProcess(ESTR* lpInput, INPUTINFO* lpipInfo, int* iPipeFd);

int Dos9_ForGetInputLine(ESTR* lpReturn, INPUTINFO* lpipInfo);

void Dos9_ForCloseInputInfo(INPUTINFO* lpipInfo);

void Dos9_ForVarUnassign(FORINFO* lpfrInfo);

#endif /* DOS9_FOR_H */
