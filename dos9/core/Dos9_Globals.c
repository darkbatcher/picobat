#include "LibDos9.h"
#include "Dos9_Core.h"

int bDelayedExpansion=FALSE;
int bUseFloats=FALSE;
int bDos9Extension=FALSE;
int bEchoOn=TRUE;
int iErrorLevel=0;
LPCOMMANDLIST lpclCommands;
LOCAL_VAR_BLOCK* lpvLocalVars;
LPSTREAMSTACK lppsStreamStack;
COLOR colColor;

#ifdef WIN32
    #define environ _environ
#else
    extern char** environ;
#endif

char* lpInitVar[]={
    "DOS9_VERSION=0.7.01",
#ifdef WIN32
    "DOS9_OS=WINDOWS",
    NULL,
#else
    "DOS9_OS=LINUX",
    NULL,
#endif
    NULL
};
