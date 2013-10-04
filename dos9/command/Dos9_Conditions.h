#ifndef DOS9_CMD_CONDITION_H
#define DOS9_CMD_CONDITION_H

/* this headers defines which function for commands such as 'FOR', or 'IF'
   and any other command that could be included in the next versions
   */

#define DOS9_IF_CASE_UNSENSITIVE 1
#define DOS9_IF_NEGATION 2
#define DOS9_IF_EXIST 4
#define DOS9_IF_ERRORLEVEL 8
#define DOS9_IF_DEFINED 16

typedef enum CMPTYPE {
    CMP_EQUAL,
    CMP_DIFFERENT,
    CMP_GREATER,
    CMP_GREATER_OR_EQUAL,
    CMP_LESSER,
    CMP_LESSER_OR_EQUAL,
} CMPTYPE;

int Dos9_CmdIf(char* lpParam); // function to support if

#endif // DOS9_CMD_CONDITION_H
