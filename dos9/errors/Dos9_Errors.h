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

extern const char* lpErrorMsg[14];
extern const char* lpQuitMessage;

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


#endif
