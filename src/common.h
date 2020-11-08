// common.h

#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


const int FILE_INPUT =	STDIN;
const int FILE_OUTPUT =	STDOUT;
const int FILE_ERROR =	STDERR;

#define STATIC_STRING_LENGHT 50


//onumerovani navratovych hodnot pro chyby
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


// vytiskne chybovou hlášku podle čísla chyby
int errorHandle(int errorID);



#endif //defined _COMMON_H