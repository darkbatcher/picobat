#ifndef DOS9_MESSAGES_H
#define DOS9_MESSAGES_H



#ifdef DOS9_DEBUG_MODE
    #define DEBUG(a) fprintf( stderr, "$ %s (line %d) * %s\n", __func__, __LINE__, a)
    #define DEBUG_(a) fprintf( stderr, "$ %s (line %d) * %d\n", __func__, __LINE__, a)
    #define DOS9_DEBUG(a) fprintf( stderr, "$ %s (line %d) * %s\n", __func__, __LINE__, a)
#else
    #define DOS9_DEBUG(a)
    #define DEBUG(a)
    #define DEBUG_(a)
    #define DEBUG1(a) fprintf( stdout, "$ %s (line %d) * %s\n", __func__, __LINE__, a)
    #define DEBUG1_(a) fprintf( stderr, "$ %s (line %d) * %d\n", __func__, __LINE__, a)
#endif

void Dos9_ShowErrorMessage(unsigned int iErrorNumber, char* lpComplement, int iExitCode);
void Dos9_LoadErrors(void);

#define DOS9_FILE_ERROR 0
#define DOS9_DIRECTORY_ERROR 1
#define DOS9_COMMAND_ERROR 2
#define DOS9_UNEXPECTED_ELEMENT 3
#define DOS9_BAD_COMMAND_LINE 4
#define DOS9_LABEL_ERROR 5
#define DOS9_EXTENSION_DISABLED_ERROR 6
#define DOS9_EXPECTED_MORE 7
#define DOS9_INCOMPATIBLE_ARGS 8
#define DOS9_UNABLE_RENAME 9
#define DOS9_MATH_OUT_OF_RANGE 10
#define DOS9_MATH_DIVIDE_BY_0 11
#define DOS9_MKDIR_ERROR 12
#define DOS9_RMDIR_ERROR 13
#define DOS9_STREAM_MODULE_ERROR 14
#define DOS9_SPECIAL_VAR_NON_ASCII 15
#define DOS9_ARGUMENT_NOT_BLOCK 16
#define DOS9_FOR_BAD_TOKEN_SPECIFIER 17
#define DOS9_FOR_TOKEN_OVERFLOW 18
#define DOS9_FOR_USEBACKQ_VIOLATION 19
#define DOS9_FAILED_ALLOCATION 20
#define DOS9_CREATE_PIPE 21
#define DOS9_FOR_LAUNCH_ERROR 22
#define DOS9_FOR_BAD_INPUT_SPECIFIER 23

#define DOS9_ERROR_MESSAGE_NUMBER 24

#define DOS9_PRINT_C_ERROR 0x80

extern const char* lpErrorMsg[DOS9_ERROR_MESSAGE_NUMBER];
extern const char* lpQuitMessage;

#endif
