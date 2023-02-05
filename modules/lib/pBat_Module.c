#include "pBat_Module.h"
#include <libpBat.h>

void*(*pBat_GetSymbol)(int);
#define PBAT_MOD_SETENV 0
void(*pBat_SetEnv)(const char*, const char*);
#define PBAT_MOD_GETENV 1
char*(*pBat_GetEnv)(const char*);
#define PBAT_MOD_GETCURRENTDIR 2
const char*(*pBat_GetCurrentDir)(void);
#define PBAT_MOD_SETCURRENTDIR 3
int(*pBat_SetCurrentDir)(const char*);
#define PBAT_MOD_GETFINPUT 4
FILE*(*pBat_GetfInput)(void);
#define PBAT_MOD_GETFOUTPUT 5
FILE*(*pBat_GetfOutput)(void);
#define PBAT_MOD_GETFERROR 6
FILE*(*pBat_GetfError)(void);
#define PBAT_MOD_GETBISSCRIPT 7
int(*pBat_GetbIsScript)(void);

#define PBAT_MOD_GETNEXTPARAMETERES 8
char*(*pBat_GetNextParameterEs)(const char*, ESTR*);
#define PBAT_MOD_ESTOFULLPATH 9
char*(*pBat_EsToFullPath)(ESTR*);
#define PBAT_MOD_REGISTERCOMMAND 10
int(*pBat_RegisterCommand)(const char*, int(*handler)(char*));
#define PBAT_MOD_SHOWERRORMESSAGE 11
void(*pBat_ShowErrorMessage)(int,const char*,int);
#define PBAT_MOD_RUNLINE 12
int (*pBat_RunLine)(ESTR*);


/* On Unices, please compile with -fvisibility=hidden */
#ifdef WIN32
#define MODULE_EXPORT __declspec(dllexport)
#else
#define MODULE_EXPORT __attribute__ ((visibility ("default")))
#endif // WIN32


extern void pBat_ModuleAttach(void);

int MODULE_EXPORT pBat_ModuleHandler(void*(*fn)(int), int api);

int pBat_ModuleHandler(void*(*fn)(int), int api)
{

    if (api < PBAT_MOD_API_VERSION)
        return -1;

    pBat_GetSymbol = fn;
    pBat_SetEnv = pBat_GetSymbol(PBAT_MOD_SETENV);
    pBat_GetEnv = pBat_GetSymbol(PBAT_MOD_GETENV);
    pBat_GetCurrentDir = pBat_GetSymbol(PBAT_MOD_GETCURRENTDIR);
    pBat_SetCurrentDir = pBat_GetSymbol(PBAT_MOD_SETCURRENTDIR);
    pBat_GetfInput = pBat_GetSymbol(PBAT_MOD_GETFINPUT);
    pBat_GetfError = pBat_GetSymbol(PBAT_MOD_GETFERROR);
    pBat_GetfOutput = pBat_GetSymbol(PBAT_MOD_GETFOUTPUT);
    pBat_GetNextParameterEs = pBat_GetSymbol(PBAT_MOD_GETNEXTPARAMETERES);
    pBat_EsToFullPath = pBat_GetSymbol(PBAT_MOD_ESTOFULLPATH);
    pBat_RegisterCommand = pBat_GetSymbol(PBAT_MOD_REGISTERCOMMAND);
    pBat_ShowErrorMessage = pBat_GetSymbol(PBAT_MOD_SHOWERRORMESSAGE);
    pBat_RunLine = pBat_GetSymbol(PBAT_MOD_RUNLINE);
    pBat_ModuleAttach();

    return 0;
}


