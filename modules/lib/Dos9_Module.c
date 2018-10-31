#include "Dos9_Module.h"
#include <libDos9.h>

void*(*Dos9_GetSymbol)(int);
#define DOS9_MOD_SETENV 0
void(*Dos9_SetEnv)(const char*, const char*);
#define DOS9_MOD_GETENV 1
char*(*Dos9_GetEnv)(const char*);
#define DOS9_MOD_GETCURRENTDIR 2
const char*(*Dos9_GetCurrentDir)(void);
#define DOS9_MOD_SETCURRENTDIR 3
int(*Dos9_SetCurrentDir)(const char*);
#define DOS9_MOD_GETFINPUT 4
FILE*(*Dos9_GetfInput)(void);
#define DOS9_MOD_GETFOUTPUT 5
FILE*(*Dos9_GetfOutput)(void);
#define DOS9_MOD_GETFERROR 6
FILE*(*Dos9_GetfError)(void);
#define DOS9_MOD_GETBISSCRIPT 7
int(*Dos9_GetbIsScript)(void);

#define DOS9_MOD_GETNEXTPARAMETERES 8
char*(*Dos9_GetNextParameterEs)(const char*, ESTR*);
#define DOS9_MOD_ESTOFULLPATH 9
char*(*Dos9_EsToFullPath)(ESTR*);
#define DOS9_MOD_REGISTERCOMMAND 10
int(*Dos9_RegisterCommand)(const char*, int(*handler)(char*));
#define DOS9_MOD_SHOWERRORMESSAGE 11
void(*Dos9_ShowErrorMessage)(int,const char*,int);


/* On Unices, please compile with -fvisibility=hidden */
#ifdef WIN32
#define MODULE_EXPORT __declspec(dllexport)
#else
#define MODULE_EXPORT __attribute__ ((visibility ("default")))
#endif // WIN32


extern void Dos9_ModuleAttach(void);

int MODULE_EXPORT Dos9_ModuleHandler(void*(*fn)(int), int api);

int Dos9_ModuleHandler(void*(*fn)(int), int api)
{

    if (api < DOS9_MOD_API_VERSION)
        return -1;

    Dos9_GetSymbol = fn;
    Dos9_SetEnv = Dos9_GetSymbol(DOS9_MOD_SETENV);
    Dos9_GetEnv = Dos9_GetSymbol(DOS9_MOD_GETENV);
    Dos9_GetCurrentDir = Dos9_GetSymbol(DOS9_MOD_GETCURRENTDIR);
    Dos9_SetCurrentDir = Dos9_GetSymbol(DOS9_MOD_SETCURRENTDIR);
    Dos9_GetfInput = Dos9_GetSymbol(DOS9_MOD_GETFINPUT);
    Dos9_GetfError = Dos9_GetSymbol(DOS9_MOD_GETFERROR);
    Dos9_GetfOutput = Dos9_GetSymbol(DOS9_MOD_GETFOUTPUT);
    Dos9_GetNextParameterEs = Dos9_GetSymbol(DOS9_MOD_GETNEXTPARAMETERES);
    Dos9_EsToFullPath = Dos9_GetSymbol(DOS9_MOD_ESTOFULLPATH);
    Dos9_RegisterCommand = Dos9_GetSymbol(DOS9_MOD_REGISTERCOMMAND);
    Dos9_ShowErrorMessage = Dos9_GetSymbol(DOS9_MOD_SHOWERRORMESSAGE);
    Dos9_ModuleAttach();

    return 0;
}


