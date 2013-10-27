#include "../LibDos9.h"

LIBDOS9 int Dos9_LibInit(void)
{
    /* instanciation routine */

    if (_Dos9_Estr_Init() != NULL)
        return -1;

    atexit(_Dos9_LibClose);
        /* sets the procedure to
           close the Lib to be executed at end of program */

    return 0;

}

LIBDOS9 void _Dos9_LibClose(void)
{

    Dos9_EsFree(NULL);
        /* free all remaining string buffers */

    _Dos9_Estr_Close();

}
