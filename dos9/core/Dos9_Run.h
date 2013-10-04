#ifndef DOS9_CORE_RUN_H
#define DOS9_CORE_RUN_H

#ifdef WIN32
    #include <process.h>
    #define DOS9_SPAWN_CAST const char* const*
#else
    #include <spawn.h>
    #define DOS9_SPAWN_CAST char* const*
    #define spawnvp(a,b,c) posix_spawnp(NULL, b, NULL, NULL, c, environ)
#endif

#ifndef P_WAIT
    #define P_WAIT _P_WAIT
#endif

int Dos9_RunCommand(ESTR* lpCommand); // the fucbtions that run every command
int Dos9_RunLine(ESTR* lpLine);
int Dos9_RunBlock(char* lpToken); // the function that run blocks
int Dos9_RunBatch(int isScript); // the function that runs the batch
int Dos9_RunExternalCommand(char* lpCommandLine);

#endif
