#include <sys/types.h>
#include <string.h>
#include <errno.h>

#ifdef __linux
    #include <sys/wait.h>
#endif

#include "Dos9_Core.h"
#include "../command/Dos9_CmdLib.h"
#include "../errors/Dos9_Errors.h"

int Dos9_RunBatch(int isScript)
{
    PARSED_STREAM_START* lppssStreamStart;
    PARSED_STREAM* lppsStream;
    DEBUG("Runner initialized");
    while (!Dos9_SendMessage(DOS9_READ_MODULE, MODULE_READ_ISEOF, NULL, NULL))
    {
        DEBUG("Start parsing");
        Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_NEWLINE, NULL, NULL);
        DEBUG("parses new line");
        lppssStreamStart=(PARSED_STREAM_START*)Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_READ_LINE_PARSE, NULL, (void*)isScript);
        if (!lppssStreamStart) {
            DEBUG("Error : Unable to reparse the stream [next line : reason]");
            DEBUG(strerror(errno));
            continue;
        }
        DEBUG("Line roughly parsed");
        Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_PARSED_START_EXEC, lppssStreamStart, NULL);
        DEBUG("Parsing succeded");
        lppsStream=lppssStreamStart->lppsStream;
        DEBUG("Starting command loop !");
        do {
            Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_PIPE_OPEN, lppsStream, NULL);
            Dos9_RunCommand(lppsStream->lpCmdLine);
        } while ((lppsStream=(PARSED_STREAM*)Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_PARSED_LINE_EXEC, lppsStream, NULL)));

        Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_FREE_PARSED_LINE, lppssStreamStart, NULL);
        Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_ENDLINE, NULL, NULL);
    }
    return 0;
}

int Dos9_RunLine(ESTR* lpLine)
{
        PARSED_STREAM_START* lppssStreamStart;
        PARSED_STREAM* lppsStream;

        Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_NEWLINE, NULL, NULL);
        lppssStreamStart=(PARSED_STREAM_START*)Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_PARSE_ESTR, lpLine, NULL);
        Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_PARSED_START_EXEC, lppssStreamStart, NULL);
        DEBUG("Parsing succeded");
        lppsStream=lppssStreamStart->lppsStream;
        DEBUG("Starting command loop !");
        do {
            Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_PIPE_OPEN, lppsStream, NULL);
            Dos9_RunCommand(lppsStream->lpCmdLine);
        } while ((lppsStream=(PARSED_STREAM*)Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_PARSED_LINE_EXEC, lppsStream, NULL)));
        Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_FREE_PARSED_LINE, lppssStreamStart, NULL);
        Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_ENDLINE, NULL, NULL);

        return 0;
}

int Dos9_RunCommand(ESTR* lpCommand)
{
    int (*lpProc)(char*);
    char lpErrorlevel[]="ERRORLEVEL=-3000000000";
    static int lastErrorLevel=0;
    char *lpCmdLine;

    lpCmdLine=Dos9_EsToChar(lpCommand);
    Dos9_RemoveEscapeChar(lpCmdLine);
    while (*lpCmdLine==' ' || *lpCmdLine=='\t') lpCmdLine++;
    switch (*lpCmdLine) {
        case ':' :
            return 0;
        case '@' :
            lpCmdLine++;
        default:;
    }
    switch(Dos9_GetCommandProc(lpCmdLine, lpclCommands, (void**)&lpProc))
    {
       case 0:
            DEBUG("Running Command");
            iErrorLevel=lpProc(lpCmdLine);
            break;
       case -1:
            DEBUG("NO command found");
            Dos9_RunExternalCommand(lpCmdLine);
    }

    if (iErrorLevel!=lastErrorLevel) {
        snprintf(lpErrorlevel+11, sizeof(lpErrorlevel)-11, "%d", iErrorLevel);
        Dos9_PutEnv(lpErrorlevel);
        lastErrorLevel=iErrorLevel;
    }
    return 0;
}


int Dos9_RunBlock(char* lpToken)
{
    char *lpBeginToken;
    ESTR*  lpEsLine=Dos9_EsInit();
    int iParentheseNb=0;
    int iOldState;

    iOldState=Dos9_GetStreamStackLockState(lppsStreamStack);
    Dos9_SetStreamStackLockState(lppsStreamStack, TRUE);

    while (*lpToken) {
        lpBeginToken=lpToken;

        for (;*lpToken && (*lpToken!='\n' || iParentheseNb);lpToken++) {
            switch (*lpToken) {
                case '^':
                    lpToken++;
                case '(':
                    iParentheseNb++;
                    break;
                case  ')':
                    if (iParentheseNb>0) iParentheseNb--;
                    break;
            }
        }

        if (lpToken==lpBeginToken)  {
            if (*lpToken) lpToken++;
            continue;
        }

        if (*lpToken) {

            *lpToken='\0';
            Dos9_EsCpy(lpEsLine, lpBeginToken);
            Dos9_RunLine(lpEsLine);

            *lpToken='\n';
            lpToken++;

        } else {
            Dos9_EsCpy(lpEsLine, lpBeginToken);
            Dos9_RunLine(lpEsLine);
        }
    }

    Dos9_SetStreamStackLockState(lppsStreamStack, iOldState);
    Dos9_EsFree(lpEsLine);
    return 0;
}

int Dos9_RunExternalCommand(char* lpCommandLine)
{
    char* lpArguments[FILENAME_MAX];
    ESTR* lpEstr[FILENAME_MAX];
    int i=0;
    pid_t iPid;

    Dos9_GetParamArrayEs(lpCommandLine, lpEstr, FILENAME_MAX);

    if (lpEstr[0]) Dos9_EsReplace(lpEstr[0], "\"", "");

    for (;lpEstr[i];i++) {
        lpArguments[i]=Dos9_EsToChar(lpEstr[i]);
    }

    lpArguments[i]=0;

    errno=0;

#ifdef WIN32

    /* in windows the result is directly returned */
    iErrorLevel=spawnvp(_P_WAIT, lpArguments[0], (DOS9_SPAWN_CAST)lpArguments);

    if (errno==ENOENT)
    {

        Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR, lpArguments[0] , 0);

    }

#elif defined _POSIX_C_SOURCE

    iPid=fork();
    if (iPid == 0 ) {
      /* if we are in the son */

      if ( execvp(lpArguments[0], lpArguments) == -1) {

        Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR, lpArguments[0], 0);

      }

      exit(0);

    } else {
      /* if we are in the father */

      if (iPid == (pid_t)-1) {
        /* the execution failed */
        Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR, lpArguments[0], 0);
      } else {
        wait(&iErrorLevel);
      }

    }

#endif

    for (i=0;lpEstr[i];i++) {
        Dos9_EsFree(lpEstr[i]);
    }

    DEBUG("Executable ran");
    return 0;

}
