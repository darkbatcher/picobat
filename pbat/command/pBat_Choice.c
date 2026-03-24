/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
 *   Copyright (C) 2010-2018 Romain GARBI
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

#if !defined(WIN32)
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#endif

#include <libpBat.h>

#include "../core/pBat_Core.h"

#include "pBat_Choice.h"

#include "../lang/pBat_ShowHelp.h"

#include "../errors/pBat_Errors.h"

#define PBAT_CHOICE_SLEEP 100
#define PBAT_CHOICE_DELIMS " ;\t\"\n"
#define PBAT_CHOICE_MAX 254

static int pBat_ChoiceHasDuplicates(const char* choices, int case_sensitive)
{
    int i, j;

    for (i = 0; choices[i]; i++) {
        int lhs = (unsigned char)choices[i];

        if (!case_sensitive)
            lhs = toupper(lhs);

        for (j = i + 1; choices[j]; j++) {
            int rhs = (unsigned char)choices[j];

            if (!case_sensitive)
                rhs = toupper(rhs);

            if (lhs == rhs)
                return 1;
        }
    }

    return 0;
}

static void pBat_ChoiceWritePrompt(const char* message, const char* choices)
{
    int i;

    if (message && *message) {
        fputs(message, fOutput);
        fputc(' ', fOutput);
    }

    fputc('[', fOutput);
    for (i = 0; choices[i]; i++) {
        if (i)
            fputc(',', fOutput);
        fputc(choices[i], fOutput);
    }
    fputs("]? ", fOutput);
    fflush(fOutput);
}

static void pBat_ChoiceWriteMessage(const char* message)
{
    if (!message || !*message)
        return;

    fputs(message, fOutput);
    fputc(' ', fOutput);
    fflush(fOutput);
}

static int pBat_ChoiceParseSeconds(const char* arg, int* timeout_ms)
{
    char* endptr;
    long value;

    value = strtol(arg, &endptr, 10);
    if (endptr == NULL || *endptr || value < 0 || value > 9999)
        return 0;

    *timeout_ms = (int)(value * 1000);
    return 1;
}

static int pBat_ChoiceFind(const char* choices, int key, int case_sensitive)
{
    int i;
    int lhs = key;

    if (!case_sensitive)
        lhs = toupper((unsigned char)lhs);

    for (i = 0; choices[i]; i++) {
        int rhs = choices[i];
        if (!case_sensitive)
            rhs = toupper((unsigned char)rhs);
        if (lhs == rhs)
            return i + 1;
    }

    return 0;
}

static int pBat_ChoiceParseDefault(const char* choices, char def, int case_sensitive)
{
    return pBat_ChoiceFind(choices, def, case_sensitive);
}

static int pBat_ChoiceReadWithTimeout(int timeout_ms)
{
#if !defined(WIN32)
    struct termios oldattr, rawattr;
    int fd = fileno(fInput);
    int restore = 0;

    if (isatty(fd) && tcgetattr(fd, &oldattr) == 0) {
        rawattr = oldattr;
        rawattr.c_lflag &= ~(ICANON | ECHO);
        rawattr.c_cc[VMIN] = 0;
        rawattr.c_cc[VTIME] = 0;
        if (tcsetattr(fd, TCSANOW, &rawattr) == 0)
            restore = 1;
    }

    if (timeout_ms < 0) {
        for (;;) {
            fd_set rfds;

            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);

            if (select(fd + 1, &rfds, NULL, NULL, NULL) > 0 && FD_ISSET(fd, &rfds)) {
                int ch = pBat_Getch(fInput);

                if (restore)
                    tcsetattr(fd, TCSANOW, &oldattr);
                return ch;
            }
        }
    }

    while (timeout_ms > 0) {
        fd_set rfds;
        struct timeval tv;
        int wait_ms = timeout_ms;

        if (wait_ms > PBAT_CHOICE_SLEEP)
            wait_ms = PBAT_CHOICE_SLEEP;

        tv.tv_sec = wait_ms / 1000;
        tv.tv_usec = (suseconds_t)((wait_ms % 1000) * 1000);

        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        if (select(fd + 1, &rfds, NULL, NULL, &tv) > 0 && FD_ISSET(fd, &rfds)) {
            int ch = pBat_Getch(fInput);

            if (restore)
                tcsetattr(fd, TCSANOW, &oldattr);
            return ch;
        }

        timeout_ms -= wait_ms;
    }

    if (restore)
        tcsetattr(fd, TCSANOW, &oldattr);
    return -1;
#else
    if (timeout_ms < 0) {
        while (!pBat_Kbhit(fInput))
            pBat_Sleep(PBAT_CHOICE_SLEEP);
        return pBat_Getch(fInput);
    }

    while (timeout_ms > 0) {
        if (pBat_Kbhit(fInput))
            return pBat_Getch(fInput);

        if (timeout_ms > PBAT_CHOICE_SLEEP) {
            pBat_Sleep(PBAT_CHOICE_SLEEP);
            timeout_ms -= PBAT_CHOICE_SLEEP;
        } else {
            pBat_Sleep((unsigned int)timeout_ms);
            timeout_ms = 0;
        }
    }

    return -1;
#endif
}

int pBat_CmdChoice(char* lpLine)
{
    int status = PBAT_NO_ERROR;
    int hide_prompt = 0;
    int case_sensitive = 0;
    int choice_index = 0;
    int have_timeout = 0;
    int have_default = 0;
    int explicit_default = 0;
    int default_index = 0;
    int timeout_ms = -1;
    char default_choice = '\0';
    const char* choices = "YN";
    const char* prompt = NULL;
    ESTR* param = pBat_EsInit_Cached();
    ESTR* choices_buf = pBat_EsInit_Cached();
    ESTR* prompt_buf = pBat_EsInit_Cached();
    char* next;

    lpLine += 6;

    while ((next = pBat_GetNextParameterEsD(lpLine, param, PBAT_CHOICE_DELIMS))) {
        char* payload = NULL;

        lpLine = next;

        if (!stricmp(param->str, "/?")) {
            pBat_ShowInternalHelp(PBAT_HELP_CHOICE);
            goto end;
        }

        if (param->str[0] != '/') {
            pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, param->str, 0);
            status = PBAT_UNEXPECTED_ELEMENT;
            goto end;
        }

        if (!stricmp(param->str, "/CS")) {
            case_sensitive = 1;
            continue;
        }

        if (!strnicmp(param->str, "/C", 2)) {
            payload = param->str + 2;
            if (*payload == ':')
                payload++;

            if (!*payload) {
                if (!(lpLine = pBat_GetNextParameterEsD(lpLine, param, PBAT_CHOICE_DELIMS))) {
                    pBat_ShowErrorMessage(PBAT_EXPECTED_MORE, "CHOICE", FALSE);
                    status = PBAT_EXPECTED_MORE;
                    goto end;
                }
                payload = param->str;
            }

            if (!*payload) {
                pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, "/C", 0);
                status = PBAT_UNEXPECTED_ELEMENT;
                goto end;
            }

            pBat_EsCpy(choices_buf, payload);
            choices = choices_buf->str;
            continue;
        }

        if (!stricmp(param->str, "/N")) {
            hide_prompt = 1;
            continue;
        }

        if (!strnicmp(param->str, "/M", 2)) {
            payload = param->str + 2;
            if (*payload == ':')
                payload++;

            if (!*payload) {
                if (!(lpLine = pBat_GetNextParameterEsD(lpLine, param, PBAT_CHOICE_DELIMS))) {
                    pBat_ShowErrorMessage(PBAT_EXPECTED_MORE, "CHOICE", FALSE);
                    status = PBAT_EXPECTED_MORE;
                    goto end;
                }
                payload = param->str;
            }

            if (!*payload) {
                pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, "/M", 0);
                status = PBAT_UNEXPECTED_ELEMENT;
                goto end;
            }

            pBat_EsCpy(prompt_buf, payload);
            prompt = prompt_buf->str;
            continue;
        }

        if (!strnicmp(param->str, "/D", 2)) {
            payload = param->str + 2;
            if (*payload == ':')
                payload++;

            if (!*payload) {
                if (!(lpLine = pBat_GetNextParameterEsD(lpLine, param, PBAT_CHOICE_DELIMS))) {
                    pBat_ShowErrorMessage(PBAT_EXPECTED_MORE, "CHOICE", FALSE);
                    status = PBAT_EXPECTED_MORE;
                    goto end;
                }
                payload = param->str;
            }

            if (!payload[0] || payload[1]) {
                pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, payload, 0);
                status = PBAT_UNEXPECTED_ELEMENT;
                goto end;
            }

            default_choice = payload[0];
            have_default = 1;
            explicit_default = 1;
            continue;
        }

        if (!strnicmp(param->str, "/T", 2)) {
            payload = param->str + 2;
            if (*payload == ':')
                payload++;

            if (!*payload) {
                if (!(lpLine = pBat_GetNextParameterEsD(lpLine, param, PBAT_CHOICE_DELIMS))) {
                    pBat_ShowErrorMessage(PBAT_EXPECTED_MORE, "CHOICE", FALSE);
                    status = PBAT_EXPECTED_MORE;
                    goto end;
                }
                payload = param->str;
            }

            if (payload[0] && payload[1] == ',' && payload[2]) {
                default_choice = payload[0];
                have_default = 1;
                explicit_default = 1;
                payload += 2;
            }

            if (!pBat_ChoiceParseSeconds(payload, &timeout_ms)) {
                pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, payload, 0);
                status = PBAT_UNEXPECTED_ELEMENT;
                goto end;
            }

            have_timeout = 1;
            continue;
        }

        pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, param->str, 0);
        status = PBAT_UNEXPECTED_ELEMENT;
        goto end;
    }

    if (!choices[0]) {
        pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, "/C", 0);
        status = PBAT_UNEXPECTED_ELEMENT;
        goto end;
    }

    if ((int)strlen(choices) > PBAT_CHOICE_MAX) {
        pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, "/C", 0);
        status = PBAT_UNEXPECTED_ELEMENT;
        goto end;
    }

    if (pBat_ChoiceHasDuplicates(choices, case_sensitive)) {
        pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, "/C", 0);
        status = PBAT_UNEXPECTED_ELEMENT;
        goto end;
    }

    if (!have_default) {
        default_choice = choices[0];
        have_default = 1;
    }

    if (explicit_default && !have_timeout) {
        pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, "/D", 0);
        status = PBAT_UNEXPECTED_ELEMENT;
        goto end;
    }

    default_index = pBat_ChoiceParseDefault(choices, default_choice, case_sensitive);
    if (!default_index) {
        pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, "default choice", 0);
        status = PBAT_UNEXPECTED_ELEMENT;
        goto end;
    }

    if (!hide_prompt)
        pBat_ChoiceWritePrompt(prompt, choices);
    else
        pBat_ChoiceWriteMessage(prompt);

    for (;;) {
        int key = pBat_ChoiceReadWithTimeout(have_timeout ? timeout_ms : -1);

        if (key < 0) {
            choice_index = default_index;
            break;
        }

        if (key == 3) {
#if !defined(WIN32)
            raise(SIGINT);
#endif
            bAbortCommand = PBAT_ABORT_EXECUTION_LEVEL;
            status = PBAT_BREAK_ERROR;
            goto end;
        }

        if (key == '\r' || key == '\n')
            continue;

        choice_index = pBat_ChoiceFind(choices, key, case_sensitive);
        if (choice_index)
            break;

        fputc('\a', fOutput);
        fflush(fOutput);
    }

    if (!hide_prompt)
        fputs(PBAT_NL, fOutput);

    status = choice_index;

end:
    pBat_EsFree_Cached(prompt_buf);
    pBat_EsFree_Cached(choices_buf);
    pBat_EsFree_Cached(param);
    return status;
}
