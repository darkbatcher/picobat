/*
 *
 *   libDos9 - The Dos9 project
 *   Copyright (C) 2010-2016 Romain GARBI
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef LIBDOS9_INCLUDED_H
#define LIBDOS9_INCLUDED_H

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef  DLL_EXPORT
#define __LIBDOS9__DLL
#endif /* DLL_EXPORT */

#ifndef LIBDOS9
#ifdef __LIBDOS9__DLL
#define LIBDOS9 __declspec(dllexport)
#define EXTERN extern
#else
#define LIBDOS9 extern
#define EXTERN
#endif /* __LIBDOS9__DLL */
#endif /* LIBDOS9  */

#if defined WIN32

    #include <windows.h>
    #include <io.h>

    #define strdup Dos9_strdup

    #define DOS9_FILE_ARCHIVE FILE_ATTRIBUTE_ARCHIVE
    #define DOS9_FILE_COMPRESSED FILE_ATTRIBUTE_COMPRESSED
    #define DOS9_FILE_HIDDEN FILE_ATTRIBUTE_HIDDEN
    #define DOS9_FILE_OFFLINE FILE_ATTRIBUTE_OFFLINE
    #define DOS9_FILE_READONLY FILE_ATTRIBUTE_READONLY
    #define DOS9_FILE_SYSTEM FILE_ATTRIBUTE_SYSTEM
    #define DOS9_FILE_DIR FILE_ATTRIBUTE_DIRECTORY

    /*
     * provide some wrappers over the windows-defined constants
     */
    #define O_WRONLY _O_WRONLY
    #define O_RDONLY _O_RDONLY
    #define O_TRUNC _O_TRUNC
    #define O_TEXT _O_TEXT
	#ifndef S_IRUSR
		#define S_IRUSR _S_IREAD
	#endif
	#ifndef S_IWUSR
		#define S_IWUSR _S_IWRITE
	#endif

    /*
     * provide some wrapper over windows pseudo-POSIX functions
     */
    #define flushall() _flushall()
    #define dup(a) _dup(a)
    #define dup2(a,b) _dup2(a,b)
    #define write(a,b,c) _write(a,b,c)
    #define read(a,b,c) _read(a,b,c)
    #define access(a,b) _access(a,b)

    #define _Dos9_Pipe(descriptors, size, mode) _pipe(descriptors, size, mode)
#else

    #include <pthread.h>

    #define DOS9_FILE_DIR S_IFDIR
    #define DOS9_FILE_EXECUTE S_IXUSR
    #define DOS9_FILE_READ S_IRUSR
    #define DOS9_FILE_WRITE S_IWUSR
    #define DOS9_FILE_REGULAR S_IFREG
    #define DOS9_FILE_ARCHIVE 0
    #define DOS9_FILE_COMPRESSED 0
    #define DOS9_FILE_HIDDEN 0
    #define DOS9_FILE_OFFLINE 0
    #define DOS9_FILE_READONLY 0
    #define DOS9_FILE_SYSTEM 0
    #define O_BINARY 0

    #define _Dos9_Pipe(descriptors, size, mode) pipe(descriptors)

    #define stricmp strcasecmp
    #define strnicmp strncasecmp

#endif // WIN32

/* define TRUE and FALSE CONSTANTS if they are not
   defined yet */

#ifndef TRUE
#define TRUE 1
#endif // TRUE

#ifndef FALSE
#define FALSE 0
#endif // TRUE


#ifdef WIN32

typedef HANDLE THREAD;
typedef HANDLE MUTEX;
typedef int    PROCESS;

#else

struct dos9_thread_t {
    pthread_t thread;
    int joined;
};

typedef struct dos9_thread_t   THREAD;
typedef pthread_mutex_t MUTEX;
typedef pid_t           PROCESS;

#endif

LIBDOS9 int      Dos9_BeginThread(THREAD* lpThId, void(*pFunc)(void*),
					int iMemAmount, void* arg);
LIBDOS9 void     Dos9_EndThread(void* iReturn);
LIBDOS9 void     Dos9_AbortThread(THREAD* lpThId);
LIBDOS9 int      Dos9_WaitForThread(THREAD* thId, void** pRet);
LIBDOS9 void     Dos9_CloseThread(THREAD* thId);

LIBDOS9 int      Dos9_WaitForAllThreads(void);
LIBDOS9 void     Dos9_AbortAllThreads(void);

LIBDOS9 int      Dos9_CreateMutex(MUTEX* lpMuId);
LIBDOS9 int      Dos9_CloseMutex(MUTEX* lpMuId);
LIBDOS9 int      Dos9_LockMutex(MUTEX* lpMuId);
LIBDOS9 int      Dos9_ReleaseMutex(MUTEX* lpMuId);

/* exports initialisation functions */
LIBDOS9 	int Dos9_LibInit(void);
LIBDOS9 	void Dos9_LibClose(void);

typedef struct STACK {
    int iFlag;
    void* ptrContent;
    struct STACK* lpcsPrevious;
} STACK,*LPSTACK;

LIBDOS9     LPSTACK Dos9_PushStack(LPSTACK lpcsStack, void* ptrContent);
LIBDOS9     LPSTACK Dos9_PopStack(LPSTACK lpcsStack, void(*pFunc)(void*));
LIBDOS9     int Dos9_GetStack(LPSTACK lpcsStack, void** ptrContent);
LIBDOS9     int Dos9_ClearStack(LPSTACK lpcsStack, void(*pFunc)(void*));

typedef struct ESTR {
    char* str;
    int len;
} ESTR, *LPESTR;

#define EsToChar(a) a->str
#define Dos9_EsToChar(a) a->str

#define DOS9_NEWLINE_WINDOWS 0
#define DOS9_NEWLINE_UNIX 1
#define DOS9_NEWLINE_LINUX DOS9_NEWLINE_UNIX
#define DOS9_NEWLINE_MAC 2

#define Dos9_SetNewLineMode(type) _Dos9_NewLine=type

EXTERN LIBDOS9 int      _Dos9_NewLine;
LIBDOS9 ESTR*           Dos9_EsInit(void);
LIBDOS9 char*           Dos9_strdup(const char* src);
LIBDOS9 int             Dos9_EsGet(ESTR* ptrESTR, FILE* ptrFile);
LIBDOS9 int             Dos9_EsCpy(ESTR* ptrESTR, const char* ptrChaine);
LIBDOS9 int             Dos9_EsCpyN(ESTR* ptrESTR, const char* ptrChaine, size_t iSize);
LIBDOS9 int             Dos9_EsCat(ESTR* ptrESTR, const char* ptrChaine);
LIBDOS9 int             Dos9_EsCatN(ESTR* ptrESTR, const char* ptrChaine, size_t iSize);
LIBDOS9 int             Dos9_EsFree(ESTR* ptrESTR);
LIBDOS9 int             Dos9_EsCpyE(ESTR* ptrSource, const ESTR* ptrDest);
LIBDOS9 int             Dos9_EsCatE(ESTR* ptrDest, const ESTR* ptrSource);
LIBDOS9 int             Dos9_EsReplace(ESTR* ptrESTR, const char* ptrPattern, const char* ptrReplace);
LIBDOS9 int             Dos9_EsReplaceN(ESTR* ptrESTR, const char* ptrPattern, const char* ptrReplace, int iN);
LIBDOS9 int Dos9_EsReplaceI(ESTR* ptrESTR, const char* ptrPattern, const char* ptrReplace);


typedef int COMMANDFLAG;

typedef struct COMMANDLIST
{
    void* lpCommandProc;
    int iLenght;
    char* ptrCommandName;
    COMMANDFLAG cfFlag;
    struct COMMANDLIST* lpclLeftRoot;
    struct COMMANDLIST* lpclRightRoot;
} COMMANDLIST, *LPCOMMANDLIST;

typedef struct COMMANDINFO {
    char* ptrCommandName;
    void* lpCommandProc;
    COMMANDFLAG cfFlag;
} COMMANDINFO,*LPCOMMANDINFO;

#define DOS9_ALIAS_FLAG 0x80000000

LIBDOS9 LPCOMMANDLIST   Dos9_MapCommandInfo(LPCOMMANDINFO lpciCommandInfo, int i);
LIBDOS9 LPCOMMANDLIST   Dos9_ReMapCommandInfo(LPCOMMANDLIST lpclCommandList);
LIBDOS9 LPCOMMANDLIST   Dos9_DuplicateCommandList(LPCOMMANDLIST lpclList);
LIBDOS9 int             Dos9_AddCommandDynamic(LPCOMMANDINFO lpciCommandInfo, LPCOMMANDLIST* lpclListEntry);
LIBDOS9 int				Dos9_ReplaceCommand(LPCOMMANDINFO lpciCommand, LPCOMMANDLIST lpclCommandList);
LIBDOS9 int             Dos9_FreeCommandList(LPCOMMANDLIST lpclList);
LIBDOS9 COMMANDFLAG     Dos9_GetCommandProc(char* lpCommandLine, LPCOMMANDLIST lpclCommandList,void** lpcpCommandProcedure);


#define DOS9_CURSOR_SHOW 1
#define DOS9_CURSOR_HIDE 1

#if defined WIN32
    #define DOS9_FOREGROUND_RED FOREGROUND_RED
    #define DOS9_FOREGROUND_BLUE FOREGROUND_BLUE
    #define DOS9_FOREGROUND_GREEN FOREGROUND_GREEN
    #define DOS9_BACKGROUND_GREEN BACKGROUND_GREEN
    #define DOS9_BACKGROUND_BLUE BACKGROUND_BLUE
    #define DOS9_BACKGROUND_RED BACKGROUND_RED

    #define DOS9_BACKGROUND_INT BACKGROUND_INTENSITY
    #define DOS9_FOREGROUND_INT FOREGROUND_INTENSITY

    #define DOS9_BACKGROUND_DEFAULT 0
    #define DOS9_FOREGROUND_DEFAULT DOS9_FOREGROUND_GREEN | DOS9_FOREGROUND_RED | DOS9_FOREGROUND_BLUE

    #define DOS9_GET_FOREGROUND(a) (a & 0x0F)
    #define DOS9_GET_BACKGROUND(a) (a & 0xF0)
    #define DOS9_GET_BACKGROUND_(a) ((a & 0xF0)>>4)

#else


    #define DOS9_FOREGROUND_RED 0x01
    #define DOS9_FOREGROUND_BLUE 0x04
    #define DOS9_FOREGROUND_GREEN 0x02
    #define DOS9_BACKGROUND_GREEN 0x20
    #define DOS9_BACKGROUND_BLUE 0x40
    #define DOS9_BACKGROUND_RED 0x10

    #define DOS9_BACKGROUND_INT 0
    #define DOS9_FOREGROUND_INT 0x08

    #define DOS9_BACKGROUND_DEFAULT 0x0100
    #define DOS9_FOREGROUND_DEFAULT 0x0200

    #define DOS9_GET_FOREGROUND(a) (a & 0x20F)
    #define DOS9_GET_FOREGROUND_(a) (a & 0x07)
    #define DOS9_GET_BACKGROUND(a) (a & 0x1F0)
    #define DOS9_GET_BACKGROUND_(a) ((a & 0xF0)>>4)


#endif

#define DOS9_BACKGROUND_IBLUE DOS9_BACKGROUND_BLUE | DOS9_BACKGROUND_INT
#define DOS9_BACKGROUND_IRED DOS9_BACKGROUND_RED | DOS9_BACKGROUND_INT
#define DOS9_BACKGROUND_IGREEN DOS9_BACKGROUND_GREEN | DOS9_BACKGROUND_INT
#define DOS9_BACKGROUND_YELLOW DOS9_BACKGROUND_RED | DOS9_BACKGROUND_GREEN
#define DOS9_BACKGROUND_CYAN DOS9_BACKGROUND_BLUE | DOS9_BACKGROUND_GREEN
#define DOS9_BACKGROUND_MAGENTA DOS9_BACKGROUND_BLUE | DOS9_BACKGROUND_RED
#define DOS9_BACKGROUND_WHITE DOS9_BACKGROUND_BLUE | DOS9_BACKGROUND_RED | DOS9_BACKGROUND_GREEN
#define DOS9_BACKGROUND_BLACK 0
#define DOS9_BACKGROUND_IYELLOW DOS9_BACKGROUND_RED | DOS9_BACKGROUND_GREEN | DOS9_BACKGROUND_INT
#define DOS9_BACKGROUND_ICYAN DOS9_BACKGROUND_BLUE | DOS9_BACKGROUND_GREEN | DOS9_BACKGROUND_INT
#define DOS9_BACKGROUND_IMAGENTA DOS9_BACKGROUND_BLUE | DOS9_BACKGROUND_RED | DOS9_BACKGROUND_INT
#define DOS9_BACKGROUND_IWHITE DOS9_BACKGROUND_BLUE | DOS9_BACKGROUND_RED | DOS9_BACKGROUND_GREEN | DOS9_BACKGROUND_INT
#define DOS9_BACKGROUND_IBLACK DOS9_BACKGROUND_INT
#define DOS9_FOREGROUND_YELLOW DOS9_FOREGROUND_RED | DOS9_FOREGROUND_GREEN
#define DOS9_FOREGROUND_CYAN DOS9_FOREGROUND_BLUE | DOS9_FOREGROUND_GREEN
#define DOS9_FOREGROUND_MAGENTA DOS9_FOREGROUND_BLUE | DOS9_FOREGROUND_RED
#define DOS9_FOREGROUND_WHITE DOS9_FOREGROUND_BLUE | DOS9_FOREGROUND_RED | DOS9_FOREGROUND_GREEN
#define DOS9_FOREGROUND_BLACK 0
#define DOS9_FOREGROUND_IBLUE DOS9_FOREGROUND_BLUE | DOS9_FOREGROUND_INT
#define DOS9_FOREGROUND_IRED DOS9_FOREGROUND_RED | DOS9_FOREGROUND_INT
#define DOS9_FOREGROUND_IGREEN DOS9_FOREGROUND_GREEN | DOS9_FOREGROUND_INT
#define DOS9_FOREGROUND_IYELLOW DOS9_FOREGROUND_RED | DOS9_FOREGROUND_GREEN | DOS9_FOREGROUND_INT
#define DOS9_FOREGROUND_ICYAN DOS9_FOREGROUND_BLUE | DOS9_FOREGROUND_GREEN | DOS9_FOREGROUND_INT
#define DOS9_FOREGROUND_IMAGENTA DOS9_FOREGROUND_BLUE | DOS9_FOREGROUND_RED | DOS9_FOREGROUND_INT
#define DOS9_FOREGROUND_IWHITE DOS9_FOREGROUND_BLUE | DOS9_FOREGROUND_RED | DOS9_FOREGROUND_GREEN | DOS9_FOREGROUND_INT
#define DOS9_FOREGROUND_IBLACK DOS9_FOREGROUND_INT
#define DOS9_COLOR_DEFAULT DOS9_BACKGROUND_DEFAULT | DOS9_FOREGROUND_DEFAULT

typedef int COLOR;


#ifndef WIN32
    /** A structure to store console coordinates */
    typedef struct CONSOLECOORD {
        short X; /**< The x coordinate (column number). Starts at 0 */
        short Y; /**< The y coordinate (line number). Starts at 0 */
    } CONSOLECOORD, *LPCONSOLECOORD;
#else

    typedef COORD CONSOLECOORD;

#endif

enum {
	NOTHING = 0,
    LEFT_BUTTON,
    RIGHT_BUTTON,
    D_LEFT_BUTTON,
    D_RIGHT_BUTTON,
    MIDDLE_BUTTON,
    SCROLL_UP,
    SCROLL_DOWN,
    RELEASE

	/* Planned :

		Grabbing :
		  - G_LEFT_BUTTON
		  - G_RIGHT_BUTTON
		  - G_MIDDLE_BUTTON

	 */
};

LIBDOS9 void            Dos9_ClearConsoleScreen(void);
LIBDOS9 void            Dos9_ClearConsoleLine(void);
LIBDOS9 void            Dos9_SetConsoleColor(COLOR cColor);
LIBDOS9 void            Dos9_SetConsoleTextColor(COLOR cColor);
LIBDOS9 void            Dos9_SetConsoleCursorPosition(CONSOLECOORD iCoord);
LIBDOS9 CONSOLECOORD    Dos9_GetConsoleCursorPosition(void);
LIBDOS9 void            Dos9_SetConsoleCursorState(int bVisible, int iSize);
LIBDOS9 void            Dos9_SetConsoleTitle(char* lpTitle);
LIBDOS9 int             Dos9_Kbhit(void);
LIBDOS9 int             Dos9_Getch(void);
LIBDOS9 void            Dos9_GetMousePos(char on_move, CONSOLECOORD* coords, int* type);

#define Dos9_GetAccessTime(lpList) lpList->stFileStats.st_atime
#define Dos9_GetCreateTime(lpList) lpList->stFileStats.st_ctime
#define Dos9_GetModifTime(lpList) lpList->stFileStats.st_mtime
#define Dos9_GetFileSize(lpList) lpList->stFileStats.st_size
#define Dos9_GetFileMode(lpList) lpList->stFileStats.st_mode

/* Match everything in the directory */
#define DOS9_SEARCH_DEFAULT 0x00
/* Browse sub-dir for matches */
#define DOS9_SEARCH_RECURSIVE 0x01
/* Stop after the first match is found */
#define DOS9_SEARCH_GET_FIRST_MATCH 0x02
/* Do not grab informations about files */
#define DOS9_SEARCH_NO_STAT 0x04
/* Do not list pseudo dirs '.' and '..' */
#define DOS9_SEARCH_NO_PSEUDO_DIR 0x08
/* Search mode equivalent DIR (ie "dir" is equivalent to "dir/*" */
#define DOS9_SEARCH_DIR_MODE 0x10

#ifndef WIN32
#define TEST_SEPARATOR(p) (*(p) == '/')
#define TEST_ABSOLUTE_PATH(p) TEST_SEPARATOR(p)
#define TEST_UNC_PATH(p)  0
#define TEST_ROOT_PATH(p) (TEST_SEPARATOR(p) && !TEST_SEPARATOR(p+1))
#define TEST_DRIVE_PATH(p) 0
#define DEF_DELIMITER "/"
#define DEF_SEPARATOR '/'
#else
/* The path may refer to a unc-type path */
#define TEST_SEPARATOR(p) (*(p) == '/' || *(p) == '\\')
#define TEST_UNC_PATH(p)  (TEST_SEPARATOR(p) && TEST_SEPARATOR(p+1))
#define TEST_ROOT_PATH(p) (TEST_SEPARATOR(p) && !TEST_SEPARATOR(p+1))
#define TEST_DRIVE_PATH(p) (*(p) && *(p+1)==':' && TEST_SEPARATOR(p+2))
#define TEST_ABSOLUTE_PATH(p) (TEST_UNC_PATH(p) || TEST_ROOT_PATH(p) \
                                || TEST_DRIVE_PATH(p))
#define DEF_DELIMITER "\\"
#define DEF_SEPARATOR '\\'

#define TEST_SEPARATOR_W(p) (*(p) == L'/' || *(p) == L'\\')
#define TEST_UNC_PATH_W(p)  (TEST_SEPARATOR_W(p) && TEST_SEPARATOR_W(p+1))
#define TEST_ROOT_PATH_W(p) (TEST_SEPARATOR_W(p) && !TEST_SEPARATOR_W(p+1))
#define TEST_DRIVE_PATH_W(p) (*(p) && *(p+1)==':' && TEST_SEPARATOR_W(p+2))
#define TEST_ABSOLUTE_PATH_W(p) (TEST_UNC_PATH_W(p) || TEST_ROOT_PATH_W(p) \
                                || TEST_DRIVE_PATH_W(p))
#define DEF_DELIMITER_W L"\\"
#define DEF_SEPARATOR_W L'\\'
#endif // _POSIX_C_SOURCE


typedef struct FILELIST {
    char  lpFileName[FILENAME_MAX]; /* filename */
    struct stat stFileStats; /* file stats */
    struct FILELIST* lpflNext; /* pointer to next match */
    struct FILELIST* lpflPrevious; /* Use only internally, do not use this */
} FILELIST,*LPFILELIST;

LIBDOS9 int         Dos9_RegExpMatch(const char* restrict lpRegExp, const char* restrict lpMatch);
LIBDOS9 int         Dos9_RegExpCaseMatch(const char* restrict lpRegExp, const char* restrict lpMatch);
LIBDOS9 LPFILELIST  Dos9_GetMatchFileList(char* lpPathMatch, int iFlag);
LIBDOS9 int         Dos9_GetMatchFileCallback(char* lpPathMatch, int iFlag, void(*pCallBack)(FILELIST*));
LIBDOS9 int         Dos9_FreeFileList(LPFILELIST lpflFileList);
LIBDOS9 int         Dos9_FormatFileSize (char* lpBuf, int iLenght, unsigned int iSize);
LIBDOS9 int         Dos9_GetStaticPart(const char* lpPathMatch, char* lpStaticPart, size_t size);
LIBDOS9 size_t      Dos9_GetStaticLength(const char* str);

#define DOS9_CMD_ATTR_READONLY 0x01
#define DOS9_CMD_ATTR_READONLY_N 0x02
#define DOS9_CMD_ATTR_VOID 0x04
#define DOS9_CMD_ATTR_VOID_N 0x08
#define DOS9_CMD_ATTR_HIDEN 0x10
#define DOS9_CMD_ATTR_HIDEN_N 0x20
#define DOS9_CMD_ATTR_SYSTEM 0x40
#define DOS9_CMD_ATTR_SYSTEM_N 0x80
#define DOS9_CMD_ATTR_ARCHIVE 0x100
#define DOS9_CMD_ATTR_ARCHIVE_N 0x200
#define DOS9_CMD_ATTR_DIR 0x400
#define DOS9_CMD_ATTR_DIR_N 0x800
#define DOS9_CMD_ATTR_ALL 0x000

#define DOS9_ATTR_DIR         (DOS9_CMD_ATTR_DIR)
#define DOS9_ATTR_NO_DIR      (DOS9_CMD_ATTR_DIR | DOS9_CMD_ATTR_DIR_N)
#define DOS9_ATTR_VOID        (DOS9_CMD_ATTR_VOID)
#define DOS9_ATTR_NO_VOID     (DOS9_CMD_ATTR_VOID | DOS9_CMD_ATTR_VOID_N)
#define DOS9_ATTR_HIDEN       (DOS9_CMD_ATTR_HIDEN)
#define DOS9_ATTR_NO_HIDEN    (DOS9_CMD_ATTR_HIDEN | DOS9_CMD_ATTR_HIDEN_N)
#define DOS9_ATTR_READONLY    (DOS9_CMD_ATTR_READONLY)
#define DOS9_ATTR_NO_READONLY (DOS9_CMD_ATTR_READONLY | DOS9_CMD_ATTR_READONLY_N)
#define DOS9_ATTR_SYSTEM      (DOS9_CMD_ATTR_SYSTEM)
#define DOS9_ATTR_NO_SYSTEM   (DOS9_CMD_ATTR_SYSTEM | DOS9_CMD_ATTR_SYSTEM_N)
#define DOS9_ATTR_ARCHIVE     (DOS9_CMD_ATTR_ARCHIVE)
#define DOS9_ATTR_NO_ARCHIVE  (DOS9_CMD_ATTR_ARCHIVE | DOS9_CMD_ATTR_ARCHIVE_N)
#define DOS9_ATTR_ALL         (DOS9_CMD_ATTR_ALL)

LIBDOS9 short Dos9_MakeFileAttributes(const char* lpToken);
LIBDOS9 int Dos9_CheckFileAttributes(short wAttr, const FILELIST* lpflList);
LIBDOS9 int Dos9_AttributesSplitFileList(short wAttr, FILELIST* pIn,
                            FILELIST** pSelected, FILELIST** pRemaining);

EXTERN LIBDOS9 int _Dos9_TextMode;
#define DOS9_BYTE_ENCODING 0
#define DOS9_UTF8_ENCODING 1
#define Dos9_SetEncoding(encoding) _Dos9_TextMode=encoding

LIBDOS9 char* Dos9_GetNextChar(const char* lpContent);
LIBDOS9 int Dos9_GetConsoleEncoding(char* lpEnc, size_t iSize);
LIBDOS9 const char* Dos9_ConsoleCP2Encoding(int cp);

LIBDOS9 int Dos9_FileExists(const char* lpPath);
LIBDOS9 char* Dos9_GetFirstExistingFile(char** files);
LIBDOS9 int Dos9_DirExists(const char* lpPath);
LIBDOS9 int Dos9_SetFileMode(const char* file, int attr);
LIBDOS9 int Dos9_GetExePath(char* lpBuf, size_t iBufSize);
LIBDOS9 int Dos9_GetExeFilename(char* lpBuf, size_t iBuffSize);

#define Dos9_IsDelim(ch) \
    ((ch==' ') || (ch=='\t') || (ch==',') || (ch==';') || (ch=='\n'))

LIBDOS9 char* Dos9_SkipBlanks(const char* lpCh);
LIBDOS9 char* Dos9_SkipAllBlanks(const char* lpCh);
LIBDOS9 char* Dos9_SearchChar(const char* lpCh, int cChar);
LIBDOS9 char* Dos9_SearchLastChar(const char* lpCh, int cChar);
LIBDOS9 void  Dos9_UnEscape(char* lpCh);
LIBDOS9 char* Dos9_GetNextNonEscaped(const char* lpCh);
LIBDOS9 char* Dos9_SearchToken(const char* restrict lpCh, const char* restrict lpDelims);
LIBDOS9 char* Dos9_SearchLastToken(const char* restrict lpCh, const char* restrict lpDelims);
LIBDOS9 char* Dos9_SearchChar_OutQuotes(const char* lpCh, int cChar);
LIBDOS9 char* Dos9_SearchToken_OutQuotes(const char* restrict lpCh, const char* restrict lpDelims);
LIBDOS9 char* Dos9_SearchToken_Hybrid(const char* restrict pch, const char* restrict delims,
                                                            const char* restrict qdelims);
LIBDOS9 void Dos9_StripEndDelims(char* str);

#endif // LIBDOS9_INCLUDED_H
