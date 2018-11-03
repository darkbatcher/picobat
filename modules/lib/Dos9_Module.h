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

#define DOS9_NO_ERROR                        0
#define DOS9_FILE_ERROR                      1
#define DOS9_DIRECTORY_ERROR                 2
#define DOS9_COMMAND_ERROR                   3
#define DOS9_UNEXPECTED_ELEMENT              4
#define DOS9_BAD_COMMAND_LINE                5
#define DOS9_LABEL_ERROR                     6
#define DOS9_EXTENSION_DISABLED_ERROR        7
#define DOS9_EXPECTED_MORE                   8
#define DOS9_INCOMPATIBLE_ARGS               9
#define DOS9_UNABLE_RENAME                  10
#define DOS9_MATH_OUT_OF_RANGE              11
#define DOS9_MATH_DIVIDE_BY_0               12
#define DOS9_MKDIR_ERROR                    13
#define DOS9_RMDIR_ERROR                    14
#define DOS9_STREAM_MODULE_ERROR            15
#define DOS9_SPECIAL_VAR_NON_ASCII          16
#define DOS9_ARGUMENT_NOT_BLOCK             17
#define DOS9_FOR_BAD_TOKEN_SPECIFIER        18
#define DOS9_FOR_TOKEN_OVERFLOW             19
#define DOS9_FOR_USEBACKQ_VIOLATION         20
#define DOS9_FAILED_ALLOCATION              21
#define DOS9_CREATE_PIPE                    22
#define DOS9_FOR_LAUNCH_ERROR               23
#define DOS9_FOR_BAD_INPUT_SPECIFIER        24
#define DOS9_FOR_TRY_REASSIGN_VAR           25
#define DOS9_INVALID_EXPRESSION             26
#define DOS9_INVALID_TOP_BLOCK              27
#define DOS9_UNABLE_DUPLICATE_FD            28
#define DOS9_UNABLE_CREATE_PIPE             29
#define DOS9_UNABLE_SET_ENVIRONMENT         30
#define DOS9_INVALID_REDIRECTION            31
#define DOS9_ALREADY_REDIRECTED             32
#define DOS9_MALFORMED_BLOCKS               33
#define DOS9_NONCLOSED_BLOCKS               34
#define DOS9_UNABLE_ADD_COMMAND             35
#define DOS9_UNABLE_REMAP_COMMANDS          36
#define DOS9_TRY_REDEFINE_COMMAND           37
#define DOS9_UNABLE_REPLACE_COMMAND         38
#define DOS9_UNABLE_SET_OPTION		        39
#define DOS9_COMPARISON_FORBIDS_STRING      40
#define DOS9_FAILED_FORK                    41
#define DOS9_NO_MATCH                       42
#define DOS9_NO_VALID_FILE                  43
#define DOS9_INVALID_NUMBER                 44
#define DOS9_TOO_MANY_ARGS                  45
#define DOS9_INVALID_IF_EXPRESSION          46
#define DOS9_UNABLE_MOVE                    47
#define DOS9_UNABLE_COPY                    48
#define DOS9_UNABLE_DELETE                  49
#define DOS9_UNABLE_MKDIR                   50
#define DOS9_UNABLE_RMDIR                   51
#define DOS9_MOVE_NOT_RENAME                52
#define DOS9_INVALID_CODEPAGE               53
#define DOS9_BREAK_ERROR                    54
#define DOS9_INVALID_LABEL                  55
#define DOS9_LOCK_MUTEX_ERROR               56
#define DOS9_RELEASE_MUTEX_ERROR            57
#define DOS9_UNABLE_CAT                     58
#define DOS9_UNABLE_LOAD_MODULE             59
#define DOS9_INCOMPATIBLE_MODULE            60
#define DOS9_MODULE_LOADED                  61
#define DOS9_ERROR_MESSAGE_NUMBER           62

#endif // DOS9_MODULES_H
