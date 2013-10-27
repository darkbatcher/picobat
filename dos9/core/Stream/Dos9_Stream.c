#include <errno.h>

#include "Dos9_Stream.h"
#include "../Dos9_Core.h"

#include "../../errors/Dos9_Errors.h"

LPSTREAMSTACK Dos9_InitStreamStack(void)
{
    LPSTREAMSTACK lpssStream=NULL;
    LPSTREAMLVL lpStream;
    if ((lpStream=Dos9_AllocStreamLvl())) {
        Dos9_GetDescriptors(lpStream->iStandardDescriptors);
        /* when the stream stack is instancied we can assume that outpout
        and input descriptors are standard inputs and output */

        lpStream->iPopLock=TRUE;
    }
    lpssStream=Dos9_PushStack(lpssStream, lpStream, 0);
    return lpssStream;
}

void Dos9_FreeStreamStack(LPSTREAMSTACK lpssStream)
{
    LPSTREAMLVL lpLvl;

    if (!lpssStream) return;

    do {

        Dos9_GetStack(lpssStream, (void**)&lpLvl);

        Dos9_CloseDescriptors(lpLvl->iFreeDescriptors);
        free(lpLvl);

    } while ((lpssStream=Dos9_PopStack(lpssStream)));

}

 void  Dos9_SetStreamStackLockState(STREAMSTACK* lppsStack, int iState)
 {
    LPSTREAMLVL lpStream;

    Dos9_GetStack(lppsStack, (void**)&lpStream);
    if (lpStream)
        lpStream->iPopLock=iState;
 }

 int Dos9_GetStreamStackLockState(STREAMSTACK* lppsStack)
 {
    LPSTREAMLVL lpStream;

    Dos9_GetStack(lppsStack, (void**)&lpStream);
    if (lpStream)
        return lpStream->iPopLock;
    return FALSE;
 }

int Dos9_OpenOutput(LPSTREAMSTACK lpssStreamStack, char* lpName, int iDescriptor, int iMode)
{
    LPSTREAMLVL lpStream;
    int* iStd, *iFreeStd;
    int iRedirectBoth=FALSE;

    Dos9_GetStack(lpssStreamStack, (void**)&lpStream);

    iStd=lpStream->iStandardDescriptors;
    iFreeStd=lpStream->iFreeDescriptors;
    DEBUG("Openning a file");
    DEBUG(lpName);

    if (!lpName || iDescriptor>3){
            DEBUG("INVALID ARGUMENT");
            return -1;
    }

    if (iMode!=0) iMode=0xffffffff;

    switch ((iDescriptor)) {

        case DOS9_STDERR+DOS9_STDOUT:
            iDescriptor=DOS9_STDOUT;
            iRedirectBoth=TRUE;
            /*  if we selected both descriptors, then considers that we want
            to redirect stdout */

        case DOS9_STDERR:
        case DOS9_STDOUT:
            iMode=O_WRONLY | O_CREAT | O_APPEND | (O_TRUNC & iMode);
            /* setting the descriptor's right (apend, create, and rd-only) */
            break;

        case DOS9_STDIN:
            iMode=O_RDONLY;
    }

    DEBUG("File is about to be loaded");
    if ((iStd[iDescriptor]=open(lpName, iMode, S_IREAD | S_IWRITE))!= -1) {

        DEBUG("File loaded !");
        Dos9_FlushDescriptor(iStd[iDescriptor], iDescriptor);
        iFreeStd[iDescriptor]=iStd[iDescriptor];

        if (iRedirectBoth) {
            iStd[DOS9_STDERR]=iStd[DOS9_STDOUT];
            Dos9_FlushDescriptor(DOS9_STDERR, iStd[DOS9_STDERR]);
        }
        return 0;
    }
    DEBUG("Error: Cant load file !");
    return -1;
}

int Dos9_OpenPipe(LPSTREAMSTACK lpssStreamStack)
{
    int iPipeDescriptors[2], iOldInputDescriptor;
    LPSTREAMLVL lpLvl;

    if (_Dos9_Pipe(iPipeDescriptors, 1024, O_TEXT) == -1)
        return NULL;

    Dos9_GetStack(lppsStreamStack, (void**)&lpLvl);
    if (!lpLvl->iPipeIndicator) {
        lppsStreamStack=Dos9_PushStreamStack(lppsStreamStack);
    }

    Dos9_GetStack(lppsStreamStack, (void**)&lpLvl);
    if (lpLvl) {
        iOldInputDescriptor=lpLvl->iStandardDescriptors[DOS9_STDIN];
        lpLvl->iFreeDescriptors[DOS9_STDIN]=iPipeDescriptors[0];
        lpLvl->iStandardDescriptors[DOS9_STDIN]=iPipeDescriptors[0];
        lpLvl->iPipeIndicator=TRUE;
    }

    lppsStreamStack=Dos9_PushStreamStack(lppsStreamStack);
    Dos9_GetStack(lppsStreamStack, (void**)&lpLvl);

    if (lpLvl) {
        lpLvl->iStandardDescriptors[DOS9_STDIN]=iOldInputDescriptor;
        lpLvl->iStandardDescriptors[DOS9_STDOUT]=iPipeDescriptors[1];
        lpLvl->iFreeDescriptors[DOS9_STDOUT]=iPipeDescriptors[1];
        lpLvl->iPipeIndicator=TRUE;

        Dos9_FlushDescriptor(iPipeDescriptors[1], DOS9_STDOUT);
        return 0;
    }


    Dos9_ShowErrorMessage(DOS9_STREAM_MODULE_ERROR, strerror(errno), TRUE);
    return -1;
}


LPSTREAMSTACK Dos9_Pipe(LPSTREAMSTACK lppsStreamStack)
{
    LPSTREAMLVL lpLvl;


    Dos9_GetStack(lppsStreamStack, (void**)&lpLvl); // on récupère le contenu du niveau courrant;

    DEBUG("In Pipe callback function");
    DEBUG_(lpLvl->iPipeIndicator);
    if (lpLvl->iPipeIndicator) {
        return Dos9_PopStreamStack(lppsStreamStack);
    } else {
        return lppsStreamStack;
    }
}

LPSTREAMSTACK Dos9_PopStreamStack(LPSTREAMSTACK lppsStack)
{
    LPSTREAMLVL lpStream;

    Dos9_GetStack(lppsStack, (void**)&lpStream);
    DEBUG("Ending a redirection lvl");

    if (lpStream) {

        if (lpStream->iPopLock==TRUE) {
                DEBUG("## ERROR [:] STREAM LEVEL LOCKED ##");
                return lppsStack;
        }

        flushall();
        Dos9_FlushStd();
        Dos9_CloseDescriptors(lpStream->iFreeDescriptors);
        free(lpStream);

    }

    lppsStack=Dos9_PopStack(lppsStack);
    Dos9_GetStack(lppsStack, (void**)&lpStream);

    if (lpStream) {
        DEBUG("Flushing low-level descriptors");
        Dos9_FlushDescriptors(lpStream->iStandardDescriptors);
    } else {
        DEBUG("REACHED BOTTOM OF STACK");
    }
    Dos9_SetStdBuffering();

    return lppsStack;
}

LPSTREAMSTACK Dos9_PushStreamStack(LPSTREAMSTACK lppsStack) {
    LPSTREAMLVL lpStream;
    LPSTREAMLVL lpLastStream;
    int i;

    DEBUG("Push a redirection level");
    Dos9_GetStack(lppsStack, (void**)&lpStream);
    Dos9_FlushStd();
//    if (lpStream) {
//        Dos9_GetDescriptors(lpStream->iCurrentDescriptors);
//    }
    lpLastStream=lpStream;
    lpStream=Dos9_AllocStreamLvl();
    for (i=0;i<3;i++) {
        lpStream->iStandardDescriptors[i]=lpLastStream->iStandardDescriptors[i];
        lpStream->iFreeDescriptors[i]=0;
    }
    lpStream->iPipeIndicator=0;
    lppsStack=Dos9_PushStack(lppsStack, lpStream, 0);
    return lppsStack;
}


int Dos9_GetDescriptors(int* Array)
{
    int i;
    for (i=0;i<3;i++) {
        Array[i]=dup(i);
    }
    return 0;
}

int Dos9_FlushDescriptors(int* Array)
{
    int i;
    for (i=0;i<3;i++) {
        DEBUG("New flush loop...");
        if (Array[i]) {
            dup2(Array[i], i);
            DEBUG("Flushing new descriptor !");
            DEBUG_(Array[i]);
        }
    }
    return TRUE;
}

void Dos9_CloseDescriptors(int* Array)
{
    int i;
    for (i=0; i<3;i++) {
        if (Array[i]) close(Array[i]);
    }
}

int Dos9_FlushDescriptor(int iDescriptor, unsigned int iStd)
{
    if (iStd>2 || !iDescriptor) return FALSE;
    DEBUG("fushing descriptor");
    return dup2(iDescriptor, iStd);
}

void Dos9_FlushStd(void)
{
    fflush(stdin);
    fflush(stdout);
    fflush(stderr);
}

void Dos9_SetStdBuffering(void)
{
    setvbuf( stdout, NULL, _IONBF, 0 );
}

LPSTREAMLVL Dos9_AllocStreamLvl(void)
{
    LPSTREAMLVL lpStreamLvl;
    int i;

    if ((lpStreamLvl=malloc(sizeof(STREAMLVL)))) {
        for (i=0;i<3;i++) lpStreamLvl->iStandardDescriptors[i]=0;
        for (i=0;i<3;i++) lpStreamLvl->iFreeDescriptors[i]=0;
    }
    lpStreamLvl->iPipeIndicator=0;
    lpStreamLvl->iPopLock=FALSE;

    return lpStreamLvl;
}
