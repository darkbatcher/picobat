/*
 *
 *   Dos9 DUMP, a free file binary dumper, The Dos9 Project
 *   Copyright (C) 2010-2014 DarkBatcher
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
#include <ctype.h>
#include <string.h>

#define PAGE_MODE_ON 0x01
#define ADDRESSES_ON 0x02
#define CHARS_ON 0x04
#define TITLE_ON 0x08
#define HEADERS_ON 0x10

#define LITTLE_ENDIAN 0
#define BIG_ENDIAN 1

#define ENDIANESS LITTLE_ENDIAN
/* the default endianness mode */

/* Ce progrmamme est un essais pour lire une un fichier binaire issu d'un
   d'un capteur CCD */

char lpLicense[]="\n\n    Dos9 DUMP, a free file binary dumper, The Dos9 Project                 \n\
    Copyright (C) 2013 Darkbatcher (Romain Garbi)\n\
\n\
    This program is free software: you can redistribute it and/or modify\n\
    it under the terms of the GNU General Public License as published by\n\
    the Free Software Foundation, either version 3 of the License, or\n\
    (at your option) any later version.\n\
\n\
    This program is distributed in the hope that it will be useful,\n\
    but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
    GNU General Public License for more details.\n\
\n\
    You should have received a copy of the GNU General Public License\n\
    along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n";

char lpHlpFr[]="Dump.exe [version 1.0] - copyright (c) 2013 Darkbatcher (Romain Garbi)\n\
Cette commande fait partie du set de commandes Dos9\n\
Ce programme est un logiciel Libre, pour plus d'information, tapez ``dump /l''\n\
\n\
\tDUMP fichier [sortie] [/P] [/H] [/T[:]type] [/B] [/E:endianess] [/L]\n\
\n\
\tAffiche le contenu d'un fichier sous forme binaire.\n\n\
\t   - fichier : Le chemin du fichier dont le contenu doit etre affiche\n\n\
\t   - sortie : Le chemin du fichier de sortie. Si aucun fichier n'est\n\
\t     specifie, alors la sortie est l'ecran de la console\n\n\
\t   - /P : Active le mode page par page. Attend une action de\n\
\t     l'utilisateur avant d'afficherr la page suivante\n\n\
\t   - /H : Active le mode hexadecimal. Ne marche que pour les types\n\
\t     entiers (``int'', ``short'', ``char'')\n\n\
\t   - /T[:]type : Choisi le type de données a afficher, parmi les\n\
\t      suivants :\n\n\
\t      C : octet                   S : mot (2 octets)\n\
\t      I : double-mot (4 octets)   F : nombre flotant simple precision\n\
\t      D : nombre flotant  double precision\n\n\
\t   - /B : Desactive le titre, les en-tetes de tableau, les addresses et\n\
\t     le char dump. Utile pour créer des fichier pour en generer des\n\
\t     copies portables via un script\n\n\
\t   - /E:endianess : Corrige les problemes d'endianess, marche pour les\n\
\t     types d'endianness suivants :\n\n\
\t     B : gros-boutant (Big Endian)\n\
\t     L : petit-boutant (Little Endian)\n\n\
\t   - /L : Affiche un extrait de licence\n\n\
\n\
\tPar defaut, la presentation activee est equivalente a :\n\n\
\tDUMP fichier /T:C /H\n\n\
Pour plus d'information ou d'autres commandes, visitez le site internet du\n\
projet Dos9, a <http://www.dos9.org/>\n\
Page d'aide de DUMP : <http://www.dos9.org/man/dump>\n\n";

union InputData {
	float fCoord;
	double dCoord;
	int iCoord;
	short sCoord;
	char cCoord;
};

void Dump_ReverseBytes(union InputData* lpData, size_t iSize)
{
	char* lpBuffer=(char*)lpData;
	int i;
	size_t iMid;

	/* simply don't do it if there are no need to do that
	   that is the length is even, except for char which
	   length is 1, and thus don't even need to be reversed
	*/

	if ((iSize % 2) != 0) return;

	iMid=iSize/2-1;
	iSize--;

	for (i=0; i<=iMid; i++) {

		/* swap byte (i) and (iSize-i) */
		lpBuffer[i]^=lpBuffer[iSize-i];
		lpBuffer[iSize-i]^=lpBuffer[i];
		lpBuffer[i]^=lpBuffer[iSize-i];

	}

}

void Dump_ShowHelp(void)
{
	puts(lpHlpFr);
}

void Dump_ShowLicence(void)
{
	puts(lpLicense);
}

void Dump_ProduceNumber(char* lpBuffer, size_t tSize, char* lpFormat, union InputData* lpData)
{
	int i=0;
	snprintf(lpBuffer, tSize, lpFormat, *(lpData));
	while (i<tSize && lpBuffer[i]!='\0') {
		i++;
	}

	if (i>=tSize) return;

	while (i<tSize) {
		lpBuffer[i]=' ';
		i++;
	}
}

void Dump_DumpChar(FILE* pData, char* lpBuf, size_t bufSize)
{
	int i=0;
	for (i=0; i<bufSize; i++) {
		if (iscntrl(lpBuf[i])) fputc('.',pData);
		else fputc(lpBuf[i],pData);
	}
}

void Dump_PrintHeader(FILE* pData, size_t iNumberSize, int iLineTokenNb, size_t iDatasize, int iAddr)
{
	char lpLine[80]=" Adress     ";
	char* lpToken=lpLine;
	union InputData uData;

	int i=0;
	if (iAddr) lpToken+=11;
	uData.iCoord=0;

	for (; i<iLineTokenNb; i++) {

		if ( (unsigned)(lpToken-lpLine+iNumberSize) >= 80)
			break;

		Dump_ProduceNumber(lpToken, iNumberSize, "%X", &uData);
		lpToken+=iNumberSize;;
		uData.iCoord+=iDatasize;

	}

	if ((unsigned)(lpToken-lpLine+iNumberSize) <= 80) {
		strcpy(lpToken, "Char dump");
		lpToken+=sizeof("Char dump");
	}

	*lpToken='\0';
	fputs(lpLine, pData); /* on affiche la ligne */
	fputs("\n\n", pData); /* on ajoute un retour à la ligne */
}

void Dump_DumpFile(FILE* pFile, FILE* pData, size_t iDataSize, size_t iNumberSize, int iLineTokenNb,
                   int iLineNb, char* lpFormat, char iEndianess, int iFlag)
{
	int i=0;
	int j=0;

	int iAddress=0;

	union InputData uData;

	uData.dCoord=0.0;
	uData.iCoord=0;
	char  lpLine[80];
	char* lpToken=lpLine;
	char  lpCharDump[16];
	char* lpCharToken=lpCharDump;

	if (iFlag & ADDRESSES_ON) {

		/* print addreesses */

		Dump_PrintHeader(pData,iNumberSize , iLineTokenNb, iDataSize ,iFlag & ADDRESSES_ON);
		sprintf(lpLine, "%010X   ", iAddress);
		lpToken+=11;

	}


	while (1) {

		iAddress+=fread(&uData, 1, iDataSize, pFile);

		if (iEndianess != ENDIANESS)
			Dump_ReverseBytes(&uData, iDataSize);

		if (feof(pFile))
			break;

		if (iFlag & CHARS_ON) {


			memcpy(lpCharToken, &uData, iDataSize);
			lpCharToken+=iDataSize;

		}

		if ( (unsigned)((lpToken-lpLine+iNumberSize)-1) >= 80) {

			fprintf(stderr, "Erreur ! Line is too long ! (%d,%d)\n", lpToken, lpLine);
			j=0;
			i++;
			lpToken=lpLine;
			continue;

		}

		Dump_ProduceNumber(lpToken, iNumberSize+1, lpFormat, &uData);
		lpToken+=iNumberSize;
		j++;

		if (i==21) { /* screen pause */

			if (iFlag & PAGE_MODE_ON) {

				printf("\nPress ``ENTER'' to continue dumping");

				while (getchar()!='\n');

				Dump_PrintHeader(pData, iNumberSize , iLineTokenNb, iDataSize ,iFlag & ADDRESSES_ON);

			}
			i=0;

		}

		if (j==iLineTokenNb) {

			i++;
			j=0;
			*lpToken='\0';

			fputs(lpLine, pData); /* on affiche la ligne */
			if (iFlag & CHARS_ON) {
				Dump_DumpChar(pData, lpCharDump, sizeof(lpCharDump));
			}

			fputc('\n', pData); /* on ajoute un retour à la ligne */

			lpToken=lpLine;
			lpCharToken=lpCharDump;

			if (iFlag & ADDRESSES_ON) {

				sprintf(lpLine, "%010X   ", iAddress);
				lpToken+=11;

			}

		}

	}

	/* on finit la dernière ligne si elle a déjà été entammée */

	if (j>0) {
		/* on complète la dernière ligne */
		while (j < iLineTokenNb) {
			if ( (unsigned)(lpToken-lpLine+iNumberSize) >= 80) {
				break;
			}

			Dump_ProduceNumber(lpToken, iNumberSize, "", &uData);
			lpToken+=iNumberSize;
			j++;

		}

		/* on complète le char dump */
		while ((unsigned)(lpCharToken - lpCharDump) < sizeof(lpCharDump)) {
			*lpCharToken=' ';
			lpCharToken++;
		}

		/* on affiche la ligne */
		*lpToken='\0';
		fputs(lpLine, pData); /* on affiche la ligne */

		if (iFlag & CHARS_ON)
			Dump_DumpChar(pData, lpCharDump, sizeof(lpCharDump));

		fputc('\n', pData); /* on ajoute un retour à la ligne */

	}
}


int main(int argc, char* argv[])
{
	FILE *pFile, *pData;

	size_t iDataSize=sizeof(char);

	size_t iNumberSize=3;

	int i,j,
	    iHex=1;

	int iFlag=ADDRESSES_ON | CHARS_ON | TITLE_ON;
	int iLineTokenNb=16;

	char iEndianness=ENDIANESS;

	char lpFormat[]="%02X";
	char *lpOutput=NULL, *lpInput=NULL;
	char *lpToken;
	char lpLine[80]=""; /* a buffer that contains text to be printed to screen */

	char* lpDataTypes[]= {"","char","short","","int/float", "", "", "", "double"};

	for (i=1; argv[i]; i++) {

		if (*argv[i]=='/') {

			switch(toupper(*(argv[i]+1))) {

			case 'T':

				argv[i]+=2;
				if (*argv[i]==':') argv[i]++;
				switch (toupper(*(argv[i]))) {
				case 'F':
					/* lire des floats */
					strcpy(lpFormat, "%E");
					iDataSize=sizeof(float);
					iNumberSize=13;
					iLineTokenNb=4;
					break;

				case 'D':
					iDataSize=sizeof(double);
					strcpy(lpFormat, "%E");
					iNumberSize=25;
					iLineTokenNb=2;
					break;

				case 'I':
					strcpy(lpFormat, "%d");
					iDataSize=sizeof(int);
					iNumberSize=13;
					iLineTokenNb=4;
					break;

				case 'S':
					strcpy(lpFormat, "%d");
					iDataSize=sizeof(short);
					iNumberSize=6;
					iLineTokenNb=8;
					break;

				case 'C':
					strcpy(lpFormat, "%d");
					iDataSize=sizeof(char);
					iNumberSize=4;
					iLineTokenNb=16;
					break;

				default:
					printf("Error ! ``%c'' is not a valid data type\n", argv[i]);
				}
				break;

			case 'P': // attend confirmation avant d'afficher une nouvelle page
				iFlag|=PAGE_MODE_ON;
				break;

			case 'B':
				iFlag = 0;
				if (!iHex) break;

			case 'H': // utilise le mode héxadécimal
				iHex=1;
				switch(iDataSize) {
				case 1:

					if (iFlag) {
						iNumberSize=3;
						iLineTokenNb=16;
					} else {
						iNumberSize=2;
						iLineTokenNb=32;
					}
					strcpy(lpFormat, "%02X");
					break;

				case 2:
					if (iFlag) {
						iNumberSize=6;
						iLineTokenNb=8;
					} else {
						iNumberSize=4;
						iLineTokenNb=16;
					}
					strcpy(lpFormat, "%04X");
					break;

				case 4:
					if (iFlag) {
						iNumberSize=13;
						iLineTokenNb=4;
					} else {
						iLineTokenNb=8;
						iNumberSize=8;
					}
					strcpy(lpFormat, "%08X");
					break;

				default:
					strcpy(lpFormat, "%f");
				}
				break;

			case '?':
				Dump_ShowHelp();
				return EXIT_SUCCESS;

			case 'L':
				Dump_ShowLicence();
				return EXIT_SUCCESS;

			case 'E':
				argv[i]+=2;
				if (*argv[i]==':') argv[i]++;

				switch (toupper(*argv[i])) {

				case 'L':
					iEndianness=LITTLE_ENDIAN;
					break;

				case 'B':
					iEndianness=BIG_ENDIAN;
					break;

				default:
					printf("Error : ``%c'' is not a valid endianness flavour.",
					       *argv[i]);
				}
				break;

			default:
				printf("Error : Invalid switch (``%s'')\n", argv[i]);
				return EXIT_FAILURE;
			}
		} else {

			if (!lpInput) {
				lpInput=argv[i];
			} else if (!lpOutput) {
				lpOutput=argv[i];
			} else {
				printf("Error, invalid argument ``%s''\n", argv[i]);
				return EXIT_FAILURE;
			}

		}
	}



	if (!lpOutput) {
		pData=stdout;
	} else {
		if (!(pData=fopen(lpOutput, "w+"))) {
			printf("Error : Can't find file ``%s''\n", lpOutput);
			return EXIT_FAILURE;
		}
	}

	if (!(pFile=fopen(lpInput, "rb"))) {
		printf("Error : Can't find file ``%s''", lpInput);
		return EXIT_FAILURE;
	}

	if (iFlag & TITLE_ON) {
		fprintf(pData, "Dumping content of ``%s'' using data type ``%s'' and format ``%s''\nGenerated by Dos9 Dump.exe - copyleft (c) darkbatcher\n\n", lpInput, lpDataTypes[iDataSize], lpFormat);
	}

	Dump_DumpFile(pFile, pData, iDataSize, iNumberSize, iLineTokenNb, 22, lpFormat, iEndianness, iFlag);

	fclose(pFile);
	fclose(pData);

	return EXIT_SUCCESS;

}
