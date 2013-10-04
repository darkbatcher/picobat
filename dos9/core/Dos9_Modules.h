#ifndef DOS9_MODULE_H
#define DOS9_MODULE_H

typedef void* MODULE;

extern int iErrorLevel;

#define MODULE_MAIN

// prototypes pour les noms de messages pour le module Call
#define MODULE_CALL_INIT 0x00
#define MODULE_CALL_RUN_PROC 0x01
#define MODULE_CALL_RUN_FILE 0x02

#define DOS9_READ_MODULE Dos9_ReadModule
#define DOS9_PARSE_MODULE Dos9_ParseModule
#define DOS9_CALL_MODULE Dos9_CallModule

#define MODULE_MAIN

#define Dos9_SendMessage(Module, Msg, Param1, Param2) Module(Msg, Param1, Param2)

#include "Stream/Dos9_Stream.h"
#include "Modules/Parse/Dos9_Parse.h"
#include "Modules/Read/Dos9_Read.h"

#endif
