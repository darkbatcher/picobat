#include <stdio.h>
#include <stdlib.h>
#include <libDos9.h>
#include <Dos9_Module.h>

int my_foo(char* line)
{
    line += 3;

    fprintf(fOutput, "%s CRY NOOOOPS" DOS9_NL, line);
    return 0;
}

void Dos9_ModuleAttach(void)
{
    fprintf(fOutput, "Registering FOO command !" DOS9_NL);
    Dos9_RegisterCommand("FOO", my_foo);
}
