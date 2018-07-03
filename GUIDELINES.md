# Programming Guidelines to Dos9 #

This documents sums up some quite important guidelines to programmers that are 
interested in extending Dos9. It discusses overall topic about internal Dos9 
features.

Please read this document carefully before attempting any modifications to the 
original code base.

## Of Multi threading ##

It is important to notice that this branch of Dos9 actively uses 
multi-threading capabilities to run various interpretors from a single Dos9 
process, allowing various features run faster. Such features include pipes, or 
even for loops.

For this reason, use of global or static variables \(unless marked as thread 
local storage, eg. declared with \_\_thread attribute\) is strongly 
discouraged.

Creating a new interpretor inside the current process can easily be achieved 
by a call to Dos9\_CloneInstance\(\):

    THREAD Dos9_CloneInstance(void(*proc)(void*), void* data);

Where \*proc refers to a function to be called upon successful launch of the 
new interpretor and \*data refers to a pointer to be passed as a parameter to 
\*proc. Note that for the others interpretors' sake, it proper and thread-safe 
memory management is of high importance. Note that this function always 
succeed. Upon interpretors exit, the thread returns a value corresponding to 
the interpretors last ERRORLEVEL status. 

As a process may host various interpretors, it is advised to pay a particular 
attention to memory leaks as they can affect really badly other interpretors.

## Creating new commands ##

Commands are the easiest extension that can be added to Dos9. To begin with, a 
typical function implementing a command should be of the following form:

    int Dos9_CmdMyCommand(char* line)

Where \*line is a pointer to the command line and the return value is the 
ERRORLEVEL status of the command. Note that you should not assume that \*line 
is writeable. To Get Command line arguments, One of the following command 
should be used :

    /* Get the next command line parameter expanded in a buffer */
    char* Dos9_GetNextParameter(char* line, char* recv, int size);
    /* Similar, but returns a ESTR */
    char* Dos9_GetNextParameterEs(char* line, ESTR* recv);
    /* Get the end of line expanded in an ESTR */
    char* Dos9_GetEndOfLine(char* line, ESTR* recv);
    /* Get a parameter list from the line */
    PARAMLIST* Dos9_GetParamList(char* lpLine);
    void Dos9_FreeParamList(PARAMLIST* list);

Where \*line refers to the current position in the command line, and the 
return value refers to the next position in the command line \(or NULL if the 
end of line has been reached\). The \*recv parameter should refer to a buffer 
of at least size chars, or to an ESTR structure \(a dynamic sized string 
structure provided by Dos9\).

## ESTR structure ##

Dos9 also provides some functions to manage dynamic sized strings. These allow 
dealing with string without caring about the size of the underlying buffer. 
These strings are designed by ESTR structures,

    /* Allocate new ESTR structure */
    ESTR* Dos9_EsInit(void); 
    /* Free ESTR structure */
    void Dos9_EsFree(ESTR* str); 
    /* Copy *orig into *dest */
    int Dos9_EsCpy(ESTR* dest, char* orig);
    int Dos9_EsCpyE(ESTR* dest, ESTR* orig); 
    /* Concatenate *orig to *dest */
    int Dos9_EsCat(ESTR* dest, char* orig); 
    int Dos9_EsCatE(ESTR* dest, ESTR* orig);
    /* Return internal char buffer version of estr */
    char* Dos9_EsToChar(ESTR* estr); 
    /* Return internal char buffer version of estr */
    estr->str;

## Paths ##

As Dos9 is multi threaded, a command shall not refer to OS current directory, 
instead, it shall use one of the following function to retrieve current 
directory. Note that it also implies that it is not safe to use relative paths 
in a command without using these functions to convert them to absolute path.

    /* Convert *full to absolute path and return full->str */
    char* Dos9_EsToFullPath(ESTR* full);
    /* Same as Dos9_MakeFullPath() but check the path exists */ 
    int  Dos9_GetFileFullPath(char* full, const char* partial, size_t size);
    /* Set *full (at least size bytes long) to absolute version of *partial path */
    void Dos9_MakeFullPath(char* full, const char* partial, size_t size);
    /* Set *full to the absolute version of *partial path */
    void Dos9_MakeFullPathEs(ESTR* full, const char* partial);
    /* Duplicate a path converting it to absolute. Behaviour identical to strdup(). */
    char* Dos9_FullPathDup(const char* path);

## Environment ##

Concerning environment variable, Dos9 uses its own internal functions that 
deal with internally stored environment. Dos9 does not rely on functions 
provided by the OS that runs it, as there is too many differences in the 
implementations.

     /* A pointer to the interpretor environment */
    ENVBUF* lpEnv;
    /* Retrieve a var */
    char* Dos9_GetEnv(ENVBUF* pEnv, const char* name);
    /* Set a var */
    void  Dos9_SetEnv(ENVBUF* pEnv, const char* name, const char* content);
    /* Unset a var */
    void  Dos9_UnSetEnv(ENVBUF* pEnv, const char* name);

To add a new internal command to the interpretor, just modify the file 
Dos9\_CommandInfo.h, it is quite straightforward.

## File search functions ##

Dos9 provides a mechanism to get matching files from regular expression in 
libDos9. 

    /* Structure to list matching files */
    typedef struct FILELIST {
        char  lpFileName[FILENAME_MAX]; /* filename */
        struct stat stFileStats; /* file stats */
        struct FILELIST* lpflNext; /* pointer to next match */
        struct FILELIST* lpflPrevious; /* Use only internally, do not use this */
    } FILELIST,*LPFILELIST;
    
    /* Return a list of all matching files to *lpPathMatch */
    FILELIST*  Dos9_GetMatchFileList(char* lpPathMatch, int iFlag);
    /* Free a list of matching files */
    void Dos9_FreeFileList(FILELIST* list);
    

Where \*lpPathMatch is a pointer to a nul-terminated regular expression to 
match. The FILELIST is allocated by the function, do not forget to free it 
with the appropriate function. Dos9\_GetMatchFileList\(\) can accept the 
following flags:

     /* Match everything in the directory */
     #define DOS9_SEARCH_DEFAULT
     /* Browse sub-dir for matches */
     #define DOS9_SEARCH_RECURSIVE
     /* Stop after the first match is found */
     #define DOS9_SEARCH_GET_FIRST_MATCH
     /* Do not grab informations about files */
     #define DOS9_SEARCH_NO_STAT
     /* Do not list pseudo dirs '.' and '..' */
     #define DOS9_SEARCH_NO_PSEUDO_DIR
     /* Search mode equivalent DIR (ie "dir" is equivalent to "dir/*" */
     #define DOS9_SEARCH_DIR_MODE
    

Dos9\_GetMatchFileList\(\) does not search for files with some particular 
attribute. However, Dos9 provides a way to discriminate between files with 
some attributes and others via the following functions:

    /* Returns a short reprensenting attributes 
       from a standard string based attributes representation */   
    short Dos9_MakeFileAttributes(const char* lpToken);
    /* Check that a file has specified attribute */
    int Dos9_CheckFileAttributes(short wAttr, const FILELIST* lpflList);
    /* Split a FILELIST in two list based on attributes */
    int Dos9_AttributesSplitFileList(short wAttr, FILELIST* pIn, FILELIST** pSelected, FILELIST** pRemaining);

The Dos9\_MakeFileAttributes\(\) and Dos9\_CheckFileAttributes\(\) are quite 
straightforward to understand. However, the Dos9\_AttributesSplitFileList\(\) 
is a bit less easy to understand. Given a FILELIST \*pIn and an attribute 
wAttr, the function returns to lists of files. Uppon successful return \(0\), 
\*pSelected points to a file list that match wAttr. On the other hand, 
\*pRemaining points to a file list that do not match wAttr. Note that 
\*\*pSelected and \*\*pRemaining are made from \*pIn elements, that is \*pIn 
is somehow destroyed in the process. Both \*\*pSelected and \*\*pRemaining 
must be freed when they are no longer useful using Dos9\_FreeFileList\(\).

## Output streams ##

As said earlier, Dos9 is multi-threaded, various thread may run 
simultaneously, and have different output files. For these reasons, standard 
output, error and input must be accessed using Dos9 internal variables listed 
below, which are thread-local:

    __thread FILE* fInput; /* stdin */
    __thread FILE* fOutput; /* stdout */
    __thread FILE* fError; /* stderr */

Thus, code must use the **f** prefixed versions of C standard library 
functions \(eg. **fprintf** and so on\).

In addition, both **fInput**, **fOutput** and **fError** are binary files, 
thus, If you want a message containing newlines to be printed portably across 
platforms, use the **DOS9\_NL** macro instead of **\n**.

## Includes ##

Almost everything you may need is in:

     #include "Dos9_Core.h" /* file in dos9/core/ */
     #include <libDos9.h>

## Command semantics ##

Dos9 follows some rules for the design of command-lines for the command it 
includes. As far as possible, the following rules must be met:

* No switches should be mandatory. Switch are designed to toogle in optional 
  features, not to provide you with easy-to-use delimiters. It is possible to 
  pass parameters following switches using either **/switch parameter** or 
  **/switch\[:\]parameter** at your option. However, the second syntax should 
  not be used if **/switch** requires several parameters.

* No choices should be mandatory. We consider that there is very few occasions 
  where a choice cannot be ignored in a well designed command. Please try to 
  avoid syntaxes like **{param1 | param2}**.

* Commands dealing with files should definitely handle regular expressions and 
  be able to process as many file name as can be provided. This is sadly not 
  the case in every Dos9 command but providing robust commands is highly 
  considered. 

