#ifndef HLP_LOAD_H_INCLUDED
#define HLP_LOAD_H_INCLUDED

#include "libDos9.h"

#define HLP_ELEMENT_TEXT 0
#define HLP_ELEMENT_HEADING 1
#define HLP_ELEMENT_CODE 2
#define HLP_ELEMENT_EMPHASIS 3
#define HLP_ELEMENT_LINK 4

#define HANDLE_ERROR(Msg, ...) Dos9_SetConsoleTextColor(DOS9_COLOR_DEFAULT);\
                               /* Dos9_ClearConsoleScreen(); */\
                               puts("");\
                               fprintf(stderr, Msg, ##__VA_ARGS__ ); \
                               puts("\n"); \
                               _exit(-1) \

typedef struct HLPELEMENT {
    int iType;
    char* lpContent;
    struct HLPELEMENT* lpNext;
} HLPELEMENT;

typedef struct HLPPAGE {
    struct HLPELEMENT* lpLineContent;
    struct HLPPAGE* lpPreviousLine;
    struct HLPPAGE* lpNextLine;
} HLPPAGE;

typedef struct HLPLINKS {
    int iLinkNb;
    char* lpTarget;
    struct HLPLINKS* lpNextLink;
} HLPLINKS;

int Hlp_LoadManPage(char* lpPageName, char* lpPageTitle, size_t iSize, HLPPAGE** lpPageRecieve, HLPLINKS** lpLinksRecieve);
void Hlp_FreeManPage(HLPPAGE* lpPage);
void Hlp_FreeManPageLinks(HLPLINKS* lpLinks);

HLPPAGE* Hlp_AllocPage(void);
void     Hlp_FreePage(HLPPAGE* lpPage);

HLPELEMENT* Hlp_AllocHlpElement(void);
void        Hlp_FreeHlpElement(HLPELEMENT* lpElement);

HLPELEMENT* Hlp_ParseLine(char* lpLine);
void        Hlp_FreeLine(HLPELEMENT* lpLine);

HLPLINKS* Hlp_AllocLinkElement(void);
void      Hlp_FreeLinkElement(HLPLINKS* lpLink);


#endif // HLP_LOAD_H_INCLUDED
