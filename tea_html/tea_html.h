#ifndef TEA_HTML_H
#define TEA_HTML_H

#include <stdio.h>
#include "libDos9.h"

#define TEA_HTML_MODE_HTML 1
#define TEA_HTML_MODE_XML 2
#define TEA_HTML_MODE_CONTENT_ONLY 4

void tea_html_ReadEntireFile(ESTR* lpEsContent, FILE* pFile);
void tea_html_MakeHtmlRootOpen(FILE* pFile);
void tea_html_MakeHtmlMeta(FILE* pFile, char* lpTitle, char* lpEncoding, char* lpFileMeta);
void tea_html_MakeHtmlBody(FILE* pFile, ESTR* lpEsContent, char* lpFileHeader, char* lpFileFooter);
void tea_html_MakeHtmlRootClose(FILE* pFile);
void tea_html_MakeHtmlRootClose(FILE* pFile);

void tea_hmtl_MakeXmlRootOpen(FILE* pFile, char* lpEncoding);


void tea_html_AdjustHtmlCode(ESTR* lpEsContent);

#endif // TEA_HTML_H
