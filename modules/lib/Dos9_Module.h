#ifndef DOS9_MODULES_H
#define DOS9_MODULES_H

#include <libDos9.h>

extern void*(*Dos9_GetSymbol)(int);
extern void(*Dos9_SetEnv)(const char*, const char*);
extern char*(*Dos9_GetEnv)(const char*);
extern const char*(*Dos9_GetCurrentDir)(void);
extern int(*Dos9_SetCurrentDir)(const char*);
extern FILE*(*Dos9_GetfInput)(void);
extern FILE*(*Dos9_GetfOutput)(void);
extern FILE*(*Dos9_GetfError)(void);
extern int(*Dos9_GetbIsScript)(void);

extern char*(*Dos9_GetNextParameterEs)(const char*, ESTR*);
extern char*(*Dos9_EsToFullPath)(ESTR*);
extern int(*Dos9_RegisterCommand)(const char*, int(*handler)(char*));
extern void(*Dos9_ShowErrorMessage)(int,const char*,int);

#define fError Dos9_GetfError()
#define fOutput Dos9_GetfOutput()
#define fInput Dos9_GetfInput()

#define DOS9_MOD_API_VERSION 0


#endif // DOS9_MODULES_H
