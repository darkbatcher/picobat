#ifndef DOS9_MODULE_PARSE
#define DOS9_MODULE_PARSE

#include "../../Dos9_Core.h"

 #ifdef WIN32
    #include <process.h>
#endif

/* A structure to store command line
 * These information are stored in two different ways:
 * - Normal line : part of a normal line
 * - Blocks of commands : blocks of commands defined by usi braces
 */

struct PARSED_STREAM {
    ESTR* lpCmdLine;
    char cNodeType;
    struct PARSED_STREAM* lppsNode;
};

struct PARSED_STREAM_START {
    struct PARSED_STREAM* lppsStream;
    char cOutputMode;
    char* lpOutputFile;
    char* lpInputFile;
};

typedef struct PARSED_STREAM PARSED_STREAM,*LPPARSED_STREAM;
typedef struct PARSED_STREAM_START PARSED_STREAM_START,*LPPARSED_STREAM_START;

extern int bEchoOn;
extern LPSTREAMSTACK lppsStreamStack;

#define PARSED_STREAM_NODE_NONE 0x00
#define PARSED_STREAM_NODE_YES 0x01
#define PARSED_STREAM_NODE_NOT 0x02
#define PARSED_STREAM_NODE_PIPE 0x03
#define PARSED_STREAM_NODE_RESET -1

#define PARSED_STREAM_START_MODE_OUT DOS9_STDOUT
#define PARSED_STREAM_START_MODE_ERROR DOS9_STDERR
#define PARSED_STREAM_START_MODE_TRUNCATE 0x04

#define MODULE_PARSE_INIT 0x00
#define MODULE_PARSE_READ_LINE_PARSE 0x01
/* LPPARSED_LINE_START (void)
Reads  and parse line */

#define MODULE_PARSE_PARSED_LINE_EXEC 0x02
#define MODULE_PARSE_PARSED_START_EXEC 0x03
#define MODULE_PARSE_NEWLINE 0x04
#define MODULE_PARSE_ENDLINE 0x05
#define MODULE_PARSE_PIPE_OPEN 0x07
#define MODULE_PARSE_FREE_PARSED_LINE 0x06
/* void (LPPARSED_LINE_START)
 frees a parsed line
 */

#define MODULE_PARSE_PARSE_ESTR 0x08
/* LPPARSED_LINE_START (ESTR*)
  parses a string */

int MODULE_MAIN Dos9_ParseModule(int iMsg, void* param1, void* param2);

int Dos9_CountParenthese(char* lpLine, int iLastNb);
void Dos9_AdjustStringNl(char* lpLine);

void Dos9_AdjustString(char* lpBegin, char* lpEnd);
char* Dos9_GetPathToken(char* lpBegin, char* lpAdjust);

PARSED_STREAM_START* Dos9_ParseStreamOutput(char *lpLine);
PARSED_STREAM_START* Dos9_AllocParsedStreamStart(void);
PARSED_STREAM* Dos9_AllocParsedStream(PARSED_STREAM* lppsStream);
PARSED_STREAM* Dos9_ParseStream(char* lpLine);

void Dos9_FreeParsedStream(PARSED_STREAM* lppsStream);
void Dos9_FreeParsedStreamStart(PARSED_STREAM_START* lppssStart);


void Dos9_ReplaceVars(ESTR* ptrCommandLine);
char* Dos9_StrToken(char* lpString, char cToken);

#endif
