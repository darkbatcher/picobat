#include <stdio.h>
#include <stdlib.h>
#include <libpBat.h>
#include <pBat_Module.h>

int my_foo(char* line)
{
    line += 3;

    fprintf(fOutput, "%s CRY NOOOOPS" PBAT_NL, line);
    return 0;
}

void pBat_ModuleAttach(void)
{
    fprintf(fOutput, "Registering FOO command !" PBAT_NL);
    pBat_RegisterCommand("FOO", my_foo);
}
