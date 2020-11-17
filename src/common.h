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
	TERM_EPSILON,
	
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
	NONTERM_expression,
	
} Term_type;


#define GRAMMAR_RULE_LIST__ROW_MAX_SIZE 10

static Term_type GrammmarRuleList[][GRAMMAR_RULE_LIST__ROW_MAX_SIZE] = {
	{TERM_KEYWORD_PACKAGE, TERM_ID, TERM_EOL, NONTERM_program},
	{TERM_KEYWORD_FUNC, TERM_ID, TERM_LROUNDBRACKET, NONTERM_param_in_first, TERM_RROUNDBRACKET, NONTERM_funkce_body, TERM_EOL, NONTERM_program},
	{TERM_EOL, NONTERM_program},
	{TERM_EOF},

	{TERM_ID, NONTERM_type, NONTERM_param_in_next},
	{TERM_EPSILON},
	{TERM_COMMA, TERM_ID, NONTERM_type, },
	{TERM_EPSILON},

	{TERM_LCURLYBRACKET, TERM_EOL, NONTERM_statement_list, TERM_RCURLYBRACKET}, //9
	{TERM_LROUNDBRACKET, NONTERM_type, NONTERM_param_out_next, TERM_RROUNDBRACKET, TERM_RCURLYBRACKET, TERM_EOL, NONTERM_statement_list, TERM_RCURLYBRACKET},

	{TERM_COMMA, NONTERM_type, NONTERM_param_out_next},
	{TERM_EPSILON},
	
	{TERM_ID, NONTERM_state_id_list, NONTERM_statement_list},
	{TERM_KEYWORD_RETURN, NONTERM_expression, NONTERM_expr_next}, //16
	{TERM_KEYWORD_IF, NONTERM_if, NONTERM_statement_list},
	{TERM_KEYWORD_FOR, NONTERM_for, NONTERM_state_id_list},
	{TERM_EPSILON},

	{TERM_LROUNDBRACKET, NONTERM_param_first, TERM_RROUNDBRACKET},
	{TERM_INIT, NONTERM_expression},
	{TERM_ASSIGN, NONTERM_expression},
	{TERM_COMMA, TERM_ID, NONTERM_id_next},

	{TERM_COMMA, TERM_ID, NONTERM_id_next},
	{TERM_INIT, NONTERM_expression, NONTERM_expr_next},
	{TERM_ASSIGN, NONTERM_expression, NONTERM_expr_next},

	{NONTERM_expression, NONTERM_expr_next},
	{TERM_EPSILON},

	{NONTERM_expression, TERM_LCURLYBRACKET, TERM_EOL, NONTERM_statement_list, TERM_LCURLYBRACKET, TERM_KEYWORD_ELSE, TERM_LCURLYBRACKET, TERM_EOL, NONTERM_statement_list, TERM_RCURLYBRACKET},
	{NONTERM_for_definition, TERM_SEMICOLON, NONTERM_expression, TERM_SEMICOLON, NONTERM_for_assignment, TERM_LCURLYBRACKET, TERM_EOL, NONTERM_statement_list, TERM_RCURLYBRACKET},
	{TERM_ID, TERM_INIT, NONTERM_expression},
	{TERM_EPSILON},
	{TERM_ID, TERM_ASSIGN, NONTERM_expression},
	{TERM_EPSILON},

	{TERM_KEYWORD_INT},
	{TERM_KEYWORD_FLOAT64},
	{TERM_KEYWORD_STRING},

	{TERM_COMMA, NONTERM_expression, NONTERM_expr_next},
	{TERM_EPSILON}
	//todo: Pravidla pro výrazy (expressions)
};


#define LL_TABLE__ROW_MAX_SIZE 20

static int LLTable[][LL_TABLE__ROW_MAX_SIZE] = {
	{ 0, 0, 0, 0, 1, 2, 0, 0, 0, 3, 4}, //<program>
	{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6}, 	//<param_in_first>
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 7}, 	//<param_in_next>
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,10, 0, 9}, //<funkce_body>
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,12, 0, 0,11}, 	//<param_out_next>
	{13, 0, 0, 0, 0, 0,13,13,13,13, 0, 0, 0, 0,14}, //<statement_list>
	{15, 0, 0, 0, 0, 0,17,18,16,19}, //<statement>
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,20, 0, 0, 0,23, 0,21,22}, //<state_id_list>
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,24, 0,25,26}, //<id_next>
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,28, 0, 0, 0, 0, 0, 0}, //<param_first>
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //<if>
	{30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30},	//<for>
	{31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32}, //<for_assignment>
	{33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,34}, //<for_definition>
	{ 0,35,36,37},	//<type>
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