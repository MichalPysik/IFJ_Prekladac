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


#define ROW(x) (x-1)
#define ITEM(x) (x-1)

#define NONTERM_ROW(x) (x-1)
#define TERM_ITEM(x) (x-1)

typedef enum
{
	TERM_TYPE_EMPTY, //prazdny defaultni typ

	TERM_KEYWORD_ELSE,
	TERM_KEYWORD_FLOAT64,
	TERM_KEYWORD_FOR,
	TERM_KEYWORD_FUNC,
	TERM_KEYWORD_IF,
	TERM_KEYWORD_INT,
	TERM_KEYWORD_RETURN,
	TERM_KEYWORD_STRING,
	TERM_KEYWORD_PACKAGE,
	TERM_KEYWORD_MAIN,

	TERM_ID, //identifikator
	TERM_ASSIGN, //prirazeni =
	TERM_INIT, //inicializace promenne :=
	TERM_COMMA, //klasicka oddelovaci carka ,
	TERM_SEMICOLON, // strednik ;
	TERM_LROUNDBRACKET, //leva zavorka (
	TERM_RROUNDBRACKET, //prava zavorka )
	TERM_LCURLYBRACKET, //leva spicata zavorka {
	TERM_RCURLYBRACKET, //prava spicata zavorka }
	TERM_EOL, //end of line - konec radku
	TERM_EOF, //end of file - konec souboru
	TERM_INTVALUE, //celociselna hodnota
	TERM_FLOATVALUE, //desetinna hodnota
	TERM_STRINGVALUE, //hodnota string neboli retezec znaku

	TERM_ADD, //scitani +
	TERM_SUB, //odcitani -
	TERM_MUL, //nasobeni *
	TERM_DIV, //deleni /

	TERM_EQ, //relacni operator ==
	TERM_NEQ, //relacni operator !=
	TERM_GT, //relacni operator >
	TERM_LT, //relacni operator <
	TERM_GTE, //relacni operator >=
	TERM_LTE, //relacni operator <=
	
	NONTERM_program,
	NONTERM_param_in_first,
	NONTERM_param_in_next,
	NONTERM_funkce_body,
	NONTERM_param_out_next,
	NONTERM_statement_list,
	NONTERM_statement,
	NONTERM_state_id_list,
	NONTERM_id_next,
	NONTERM_param_first,
	NONTERM_if,
	NONTERM_for,
	NONTERM_for_definition,
	NONTERM_for_assignment,
	NONTERM_type,
	NONTERM_expr_next,
	

} Term_type;


#define GRAMMAR_RULE_LIST__ROW_MAX_SIZE 10

static Term_type GrammmarRuleList[][GRAMMAR_RULE_LIST__ROW_MAX_SIZE] = {
	{TERM_KEYWORD_PACKAGE, TERM_ID, TERM_EOL, NONTERM_program},
	{TERM_KEYWORD_FUNC, TERM_ID, TERM_LROUNDBRACKET, NONTERM_param_in_first, TERM_RROUNDBRACKET, NONTERM_funkce_body, TERM_EOL, NONTERM_program}
	// TODO doplnit ostatni pravidla
};


#define LL_TABLE__ROW_MAX_SIZE 20

static int LLTable[][LL_TABLE__ROW_MAX_SIZE] = {
	{0, 0, 0, 0, 1, 2, 0, 0, 0, 3, 4},
	{5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6}
	// TODO doplnit ll tabulku
};

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