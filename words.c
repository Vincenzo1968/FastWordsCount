/* gcc -Wall -W -std=c99 -O3 -funroll-loops words.c -o words */
/* gcc -Wall -W -std=c99 -O3 -funroll-loops -static words.c -o words */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdint.h>
#include <time.h>

//#define BLOCK_SIZE 4096
#define BLOCK_SIZE 8192

typedef struct tagFindRec
{
	uint64_t countChars;
	uint64_t countRows;
	uint64_t maxRowLenght;
	uint64_t countWords;
	uint64_t maxWordLenght;	
	char szLongerWord[984];	
} FindRec;

void InitFindRec(FindRec *myFindRec);
void doCount(const char *szFileName, FindRec *myFindRec);
int  isValidSeparatorChar(int c);
int isValidWordChar(int c);
void PrintHelp(const char *szProg);
void ExtractFileName(const char *szPathName, char *szFileName);

/*---------------------------------------------------------------------------------------------------------------------*/

void InitFindRec(FindRec *myFindRec)
{
	myFindRec->countChars = 0;
	myFindRec->countRows = 0;
	myFindRec->maxRowLenght = 0;
	myFindRec->countWords = 0;
	myFindRec->maxWordLenght = 0;		
	myFindRec->szLongerWord[0] = '\0';
}

void doCount(const char *szFileName, FindRec *myFindRec)
{
	uint64_t x;
	int myChar;
	uint64_t index1;
	uint64_t column;
	uint64_t maxWordLenght;	
	uint64_t blockLen;
	FILE *fp;
	char szWord[1024];
	unsigned char *myBlock = NULL;
	
	szWord[0] = '\0';
	
	myBlock = (unsigned char *)malloc(sizeof(unsigned char) * BLOCK_SIZE);
	if ( !myBlock )
	{
		//printf("Memoria insufficiente.\n");
		printf("insufficient memory.\n");		
		return;	
	}
	
	fp = fopen(szFileName, "rb");
	if ( fp == NULL )
	{
		//printf("Errore nell'apertura del file %s. Addio!\n", szFileName);
		printf("Error opening file %s.\n", szFileName);
		return;
	}	
	
	x = 0;
	blockLen = 0;
	column = 0;
	maxWordLenght = 0;
	index1 = 0;
	myFindRec->countChars = 0;
	myFindRec->countWords = 0;
	myFindRec->countRows = 0;
	myFindRec->maxWordLenght = 0;
	myFindRec->maxRowLenght = 0;
	while ( (blockLen = fread(myBlock, 1, BLOCK_SIZE, fp)) )
	{
		myFindRec->countChars += blockLen;
		
		while ( index1 < blockLen )
		{
			if ( myBlock[index1] == '\n' )
			{
				myFindRec->countRows++;
				if ( column > myFindRec->maxRowLenght )
					myFindRec->maxRowLenght = column;
				column = 1;
			}
			else
			{
				column++;
			}
			
			if ( isValidWordChar(myBlock[index1]) )			
			{
				maxWordLenght++;
				szWord[x++] = myBlock[index1];
			}			
			else
			{
				myChar = szWord[0];
				
				if ( (x > 0)
				     && (
				      (
				       (myChar >= 'A' && myChar <= 'Z') || (myChar >= 'a' && myChar <= 'z')
				       || (myChar >= 128 && myChar <= 154) || (myChar >= 224 && myChar <= 237)
				      )  
				     )				     
				   )
				{
					myFindRec->countWords++;
					
					szWord[x] = '\0';					
					
					if ( maxWordLenght > myFindRec->maxWordLenght )
					{
						myFindRec->maxWordLenght = maxWordLenght;				
						strcpy(myFindRec->szLongerWord, szWord);
					}
				}				
				x = 0;				
				maxWordLenght = 0;
			}
			
			index1++;
		}
		index1 = 0;
	}

	fclose(fp);

	free(myBlock);
}

int isValidSeparatorChar(int c)
{
	if (
		(c >= 65 && c <= 90)	||
		(c >= 97 && c <= 122)	|| 
		(c >= 192 && c <= 214)	||
		(c >= 217 && c <= 246)	||
		(c >= 249 && c <= 255) 
	 )
		return 0;
	
	return 1;	
}

int isValidWordChar(int c)
{
	if (
		(c >= 65 && c <= 90)	||
		(c >= 97 && c <= 122)	|| 
		(c >= 192 && c <= 214)	||
		(c >= 217 && c <= 246)	||
		(c >= 249 && c <= 255) 
	 )
		return 1;
	
	return 0;	
}

void PrintHelp(const char *szProg)
{
	//printf("\n\nUso: %s NomeFileInput\n\n", szProg);
	printf("\n\nUsage: %s InputFileName\n\n", szProg);	
}

void ExtractFileName(const char *szPathName, char *szFileName) 
{
	int k, i, x;
	char c;

	k = strlen(szPathName);

/* #ifdef __linux__ || __gnu_linux__ */
#if __linux__ || __gnu_linux__ || __APPLE__
	c = '/';
#else
	c = '\\';
#endif

	i = k - 1;
	while ( i >= 0 )
	{
		if ( szPathName[i] == c )
			break;
		i--;
	}
	i++;
	x = 0;
	for (;;)
	{
		szFileName[x] = szPathName[i];
		if (szFileName[x] == '\0' || szFileName[x] == '.' )
			break;
		x++;
		i++;
	}
	szFileName[x] = '\0';
}

int main(int argc, char* argv[])
{
	FindRec myFindRec;
	clock_t c_start, c_end;
	double TimeTaken;
	
	if ( argc < 2 )
	{

#if __linux__ || __gnu_linux__ || __APPLE__
		PrintHelp(argv[0]);
#else
		ExtractFileName(argv[0], szExeName);
		PrintHelp(szExeName);
#endif

		return -1;
	}
	
	InitFindRec(&myFindRec);
		
	c_start = clock();
	doCount(argv[1], &myFindRec);
	c_end = clock();
	
	TimeTaken = (double)(c_end - c_start) / CLOCKS_PER_SEC;
	//printf("Tempo impiegato -> %5.5f secondi\n\n", TimeTaken);	
	printf("Time taken -> %5.5f seconds\n\n", TimeTaken);	

	printf("\n");
	
	//printf("Il file \"%s\" contiene %llu caratteri.\n", argv[1], myFindRec.countChars);
	//printf("Il file contiene %d righe.\n", myFindRec.countRows);
	//printf("La riga più lunga è lunga %d byte.\n", myFindRec.maxRowLenght - 1);
	//printf("Nel file ci sono %d parole.\n", myFindRec.countWords);	
	//printf("\nLa parola più lunga è lunga %d byte.\n", myFindRec.maxWordLenght);
	//printf("\nEcco una delle parole più lunghe: \"%s\".\n", myFindRec.szLongerWord);	
	
	printf("characters   : %llu\n", (unsigned long long)myFindRec.countChars);		
	printf("rows         : %llu\n", (unsigned long long)myFindRec.countRows);
	printf("longest row  : %llu\n", (unsigned long long)myFindRec.maxRowLenght - 1);
	printf("words        : %llu\n", (unsigned long long)myFindRec.countWords);	
	printf("\nlongest word is long %llu characters\n", (unsigned long long)myFindRec.maxWordLenght);
	printf("\none of the longest words: \"%s\".\n", myFindRec.szLongerWord);
	
	printf("\n");
				
	return 0;
}
