// common.h

#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>



FILE *FILE_INPUT;
FILE *FILE_OUTPUT;
FILE *FILE_ERROR;


#define ARRAY_SIZE(x)  (int)(sizeof(x) / sizeof((x)[0]))

#define STATIC_STRING_LENGHT 255



/****************************************************** ERROR HANDLE ******************************************************************************/

//onumerovani navratovych hodnot pro chyby = errorID
#define ALL_OK 0
#define LEX_ERROR 1
#define SYNTAX_ERROR 2
#define SEM_UNDEFINED_VAR_ERROR 3
#define SEM_DATA_TYPE_ERROR 4
#define SEM_TYPE_COMPATIBILITY_ERROR 5
#define SEM_FUNC_PARAM_RETURN_ERROR 6
#define SEM_OTHER_ERROR 7
#define ZERO_DIVISION_ERROR 9
#define INTERNAL_ERROR 99
#define ERROR_ALREADY_EXISTS 100


typedef struct {
	int errorID;
	char errorString[STATIC_STRING_LENGHT];
	char errorPosFile[STATIC_STRING_LENGHT];
	int errorPosLine;
} ErrorHandle;

// vytiskne chybovou hlášku podle čísla chyby
int handleError(ErrorHandle errorHandle, char *lastOKTokenType, int lastOKTokenPosLine, int lastOKTokenPosNumber, char *lastOKTokenString);

void handleFreeError(int errorID, int errorLine, char *errorFile);

int errorHandleInit(ErrorHandle *errorHandle);

int errorExists(ErrorHandle errorHandle);

int errorSet(int errorID, char *errorString, char *errorPosFile, int errorPosLine, ErrorHandle *errorHandle);


#endif //defined _COMMON_H