#include "../libpBat.h"
#include "../../config.h"

/*
pBat_Getch : Derived from darkbox getch by Teddy ASTIE

Darkbox - A Fast and Portable Console IO Server
Copyright (c) 2016 Teddy ASTIE (TSnake41)

*/
#if defined(LIBPBAT_NO_CONSOLE)

LIBPBAT int pBat_Getch(FILE *f)
{
  return fgetc(f);
}

LIBPBAT int pBat_Kbhit(FILE *f)
{
  return 0;
}

#elif !defined(WIN32)

#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

LIBPBAT int pBat_Getch(FILE *f)
{
  struct termios oldattr, newattr;
  int ch;
  tcgetattr(fileno(f), &oldattr);
  newattr = oldattr;
  newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr(fileno(f), TCSANOW, &newattr);
    ch = fgetc(f);
  tcsetattr(fileno(f), TCSANOW, &oldattr);

  /* Handle special chracters */
  if (ch == '\033' && pBat_Getch(f) == '[') {
    switch (pBat_Getch(f)) {
      case 'A': /* up arrow */
        return 72;
        break;
      case 'B': /* down arrow */
        return 80;
        break;
      case 'C': /* right arrow */
        return 77;
        break;
      case 'D': /* left arrow */
        return 75;
        break;
      case 'F': /* end */
        return 79;
        break;
      case 'H': /* begin */
        return 71;
        break;

      case '2': /* insert */
        pBat_Getch(f); /* ignore the next character */
        return 82;
        break;
      case '3': /* delete */
        pBat_Getch(f);
        return 83;
        break;
      case '5': /* page up */
        pBat_Getch(f);
        return 73;
        break;
      case '6': /* page down */
        pBat_Getch(f);
        return 81;
        break;

      case 'M':
        /* Mouse input beginning sequence. */
        return -2;
        break;

      default:
        return -1; /* unmanaged/unknown key */
        break;
    }
  } else return ch;
}


/* Morgan McGuire, morgan@cs.brown.edu */
int pBat_Kbhit(FILE *f)
{
  int bytesWaiting;
  struct termios term;

  tcgetattr(fileno(f), &term);
  term.c_lflag &= ~ICANON;
  tcsetattr(fileno(f), TCSANOW, &term);
  setbuf(f, NULL);

  ioctl(fileno(f), FIONREAD, &bytesWaiting);
  return bytesWaiting;
}

#else

#include <stdio.h>

#include <windows.h>
#include <conio.h>

static HANDLE __inline__ _pBat_GetHandle(FILE* f)
{
    HANDLE h = (HANDLE)_get_osfhandle(fileno(f));
    DWORD mode;

    if (GetConsoleMode(h, &mode) == 0 &&
            GetLastError() == ERROR_INVALID_HANDLE)
        return (HANDLE)-1;

    return h;
}

int pBat_Getch(FILE* f)
{
    if (_pBat_GetHandle(f) == (HANDLE)-1)
        return fgetc(f);

    return getch();
}

int pBat_Kbhit(FILE* f)
{
    if (_pBat_GetHandle(f) == (HANDLE)-1)
        return !feof(f);

    return kbhit();
}
#endif
