#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Tea_Lib.h"
#include "tea_modes.h"
#include "tea_modifier.h"
#include "tea_strupr.h"
/* mode texte */

TEAMODIFIERS teaTextMods = {
    tea_TextModifierLink,
    tea_Strupr,
    tea_Strupr,
    free,
    NULL,
    NULL,
};

TEAOUTPUT teaTextOutput = {
    {
        NULL, /* modifiers */
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    },
        /* code styles */
    {
        79,
        8,
        10 /* for code blocks, indent is a little bit particular becauses
        it causes indentation of wrapped lines */
    },
        /* paragraph styles */
    {
        79, /* the width of text */
        5, /* the width of left margin */
        0 /* the width of indentation */
    },
        /* block spacing */
    "\n\n"
};

    /* mode HTML */

TEAMODIFIERS teaHTMLMods = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

TEAOUTPUT teaHTMLOutput = {
    {
        tea_HtmlHeadingStart, /* modifiers */
        tea_HtmlHeadingEnd,
        tea_HtmlCodeStart,
        tea_HtmlCodeEnd,
        tea_HtmlParagraphStart,
        tea_HtmlParagraphEnd,
        tea_HtmlEmphasisStart,
        tea_HtmlEmphasisEnd,
        tea_HtmlLinkStart,
        tea_HtmlLinkEnd
    },
        /* code styles */
    {
        0x0fffffff,
        0,
        0 /* for code blocks, indent is a little bit particular becauses
        it causes indentation of wrapped lines */
    },
        /* paragraph styles */
    {
        80, /* the width of text */
        0, /* the width of left margin */
        0 /* the width of indentation */
    },
        /* block spacing */
    "\n\n"
};

char* lpTeaHTMLFrontEnd="tea_html";

    /* mode HLP */

TEAMODIFIERS teaHlpMods = {
    tea_HlpModifierLink,
    NULL,
    NULL,
    free,
    NULL,
    NULL,
};

TEAOUTPUT teaHlpOutput = {
    {
        tea_HlpHeadingStart, /* modifiers */
        tea_HlpEnd,
        tea_HlpCodeStart,
        tea_HlpEnd,
        NULL,
        NULL,
        tea_HlpEmphasisStart,
        tea_HlpEnd,
        tea_HlpLinkStart,
        tea_HlpEnd
    },
        /* code styles */
    {
        79,
        8,
        10 /* for code blocks, indent is a little bit particular becauses
        it causes indentation of wrapped lines */
    },
        /* paragraph styles */
    {
        79, /* the width of text */
        5, /* the width of left margin */
        0 /* the width of indentation */
    },
        /* block spacing */
    "\n\n"
};

char* lpTeaHlpFrontEnd="hlp_make ";
