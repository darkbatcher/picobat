#include "libDos9.h"
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
    "DOS9_VERSION=" DOS9_VERSION,
#ifdef WIN32
    "DOS9_OS=WINDOWS",
#elif defined _POSIX_C_SOURCE
    "DOS9_OS=*NIX",
#else
    "DOS9_OS=UNKNOWN"
#endif
    NULL,
    NULL
};
