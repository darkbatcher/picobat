#ifndef DOS9_MODULE_READ_H
#define DOS9_MODULE_READ_H

#define MODULE_READ_INIT 0X00
#define MODULE_READ_GETPOS 0x01
/* retour : position du curseur avant la dernière lecture */
#define MODULE_READ_SETPOS 0x03
/* place le curseur à un endroit du fichier */
#define MODULE_READ_GETLINE 0x04
/* retourne la dernière ligne lue */
#define MODULE_READ_SETFILE 0x06
/* définit le fichier à devoir être lù */
#define MODULE_READ_ISEOF 0x07
/* retourne si la fin du fichier a été atteinte */
#define MODULE_READ_GETFILE 0x08
/* recupère le nom du fichier
    param1 : pointeur sur un buffer qui recupere le nom du fichier
    param2 : longueur du buffer
    */


int MODULE_MAIN Dos9_ReadModule(int Msg, void* param1, void* param2);

#endif
