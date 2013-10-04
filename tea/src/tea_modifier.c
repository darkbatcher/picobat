#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


char*   tea_TextModifierLink(char* lpContent, char* lpTarget)
{

    size_t iSizeAllocate=sizeof(" (-> `")+sizeof("')")-1;
    char* lpModifier;


       /* on calcule la taille qu'il faut allouer */

    iSizeAllocate+=strlen(lpContent)+strlen(lpTarget);

    /* on alloue l'espace nécessaire */

    if (!(lpModifier=malloc(iSizeAllocate))) {
        fprintf(stderr, "Erreur : tea_TextModifierLink() : %s", strerror((errno)));
        exit(-1);
    }

    sprintf(lpModifier, "%s (-> `%s')", lpContent, lpTarget);

    return lpModifier;
}
