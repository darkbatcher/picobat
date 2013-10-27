#ifndef DOS9_FOR_H
#define DOS9_FOR_H

#include "Dos9_CmdLib.h"

#define TOKENINFO_NB_MAX 128-32
#define MAX_UNICODE_CHAR 5
#define TOKENNB_ALL_REMAINING 0xFFFF

#define HIGHWORD(nb) (nb & 0xFFFF0000) >> 16)
#define LOWWORD(nb) (nb & 0xFFFF)

#define FOR_LOOP_SIMPLE 0
#define FOR_LOOP_R 1
#define FOR_LOOP_F 2
#define FOR_LOOP_L 4

typedef struct FORINFO {
    char lpNewLines[FILENAME_MAX];
        /* the new line delimiter */
	char lpDelims[FILENAME_MAX];
		/*  the delims */
	char lpEol[FILENAME_MAX];
		/* the eol character, in a unicode character */
	int iSkip;
		/* the number of lines to be skipped */
	char cTokenBegin;
		/* position of the first special var */
	int	lpTokenNb[TOKENINFO_NB_MAX];
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

int Dos9_CmdFor(char* lpCommand);
	/* This is the function used for running
	   ``for'' loops */

int Dos9_CmdForSimple(ESTR* lpInput, BLOCKINFO* lpbkCommand, char cVarName);


int Dos9_ForMakeInfo(char* lpOptions, FORINFO* lpfrInfo);
    /* this makes token from a parameter command
       i.e. from a string like "tokens=1,3,4 delims=, "
    */

void Dos9_ForAdjustParameter(char* lpOptions, ESTR* lpParam);

int  Dos9_ForMakeTokens(char* lpTokens, FORINFO* lpfrInfo);

int Dos9_ForSplitTokens(char* lpContent, FORINFO* lpfrInfo);
	/* this split a line into different tokens
	   for parsing with tokens */


#endif /* DOS9_FOR_H */
