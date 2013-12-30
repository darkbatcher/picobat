#ifndef DOS9_CORE_H_INCLUDED
#define DOS9_CORE_H_INCLUDED

#include "libDos9.h"

#include "Dos9_Context.h"

#include "Dos9_Var.h"
#include "Dos9_Expand.h"
#include "Dos9_Run.h"
#include "Dos9_Modules.h"
#include "Dos9_Jump.h"
#include "Dos9_ShowIntro.h"

extern int bDelayedExpansion;
extern int bUseFloats;
extern int bDos9Extension;
extern int bEchoOn;
extern int iErrorLevel;
extern LPCOMMANDLIST lpclCommands;
extern LOCAL_VAR_BLOCK* lpvLocalVars;
extern LPSTREAMSTACK lppsStreamStack;
extern COLOR colColor;

#ifdef WIN32
    #define environ _environ
#else
    extern char** environ;
#endif

extern char* lpInitVar[];


#define DEBUG(a)
#define DOS9_DEBUG(a)
#define DEBUG_(a)

#ifndef WIN32
    #define getch() getchar()
#else
    #include <conio.h>
#endif

#endif // DOS9_CORE_H_INCLUDED
