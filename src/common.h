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


// test
#define ROW(x) (x-1)
#define ITEM(x) (x-1)

#define NONTERM_ROW(x) (x-1)
#define TERM_ITEM(x) (x-1)
//test - end





#define IS_TERM(x) (x < NONTERM_program)
#define IS_NONTERM(x) (x > TERM_PSEUDO_EPSILON)

#define TERM_TO_TABLE(x) (x - TERM_KEYWORD_PACKAGE)
#define NONTERM_TO_TABLE(x) (x - NONTERM_program)



typedef enum
{
	TERM_EMPTY, //prazdny defaultni typ



	// SEŘAZENO PODLE LL TABULKY
	TERM_KEYWORD_PACKAGE,
	TERM_ID, //identifikator
	TERM_EOL, //end of line - konec radku
	TERM_KEYWORD_FUNC,
	TERM_LROUNDBRACKET, //leva zavorka (
	TERM_RROUNDBRACKET, //prava zavorka )
	TERM_EOF, //end of file - konec souboru
	TERM_COMMA, //klasicka oddelovaci carka ,
	TERM_LCURLYBRACKET, //leva spicata zavorka {
	TERM_RCURLYBRACKET, //prava spicata zavorka }
	TERM_KEYWORD_RETURN,
	TERM_KEYWORD_IF,
	TERM_KEYWORD_FOR,
	TERM_SEMICOLON, // strednik ;
	TERM_INIT, //inicializace promenne :=
	TERM_ASSIGN, //prirazeni =
	TERM_KEYWORD_INT,
	TERM_KEYWORD_FLOAT64,
	TERM_KEYWORD_STRING,



	// SEŘAZENO PODLE PRECEDENČNÍ TABULKY (TODO - neseřazeno)
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
	
	
	
	// NEVYUŽITÉ TERMINALY
	TERM_KEYWORD_ELSE,
	TERM_KEYWORD_MAIN,
	TERM_PSEUDO_DOLLAR,
	TERM_PSEUDO_HANDLE,
	TERM_PSEUDO_EPSILON,
	
	
	
	// SEŘAZENO PODLE LL TABULKY
	NONTERM_program,
	NONTERM_param_in_first,
	NONTERM_param_in_next,
	NONTERM_funkce_body,
	NONTERM_param_out_next,
	NONTERM_statements,
	NONTERM_state_id_list,
	NONTERM_id_next,
	NONTERM_for_definition,
	NONTERM_for_assignment,
	NONTERM_type,
	NONTERM_expr_next,
	
	
	
	// NEVYUŽITÉ NETERMINALY
	NONTERM_expression
	
} Term_type;

static char termTypes[][STATIC_STRING_LENGHT] = {"TERM_EMPTY", "TERM_KEYWORD_PACKAGE", "TERM_ID", "TERM_EOL", "TERM_KEYWORD_FUNC",
	"TERM_LROUNDBRACKET", "TERM_RROUNDBRACKET", "TERM_EOF", "TERM_COMMA", "TERM_LCURLYBRACKET", "TERM_RCURLYBRACKET", "TERM_KEYWORD_RETURN",
	"TERM_KEYWORD_IF", "TERM_KEYWORD_FOR", "TERM_SEMICOLON", "TERM_INIT", "TERM_ASSIGN", "TERM_KEYWORD_INT", "TERM_KEYWORD_FLOAT64",
	"TERM_KEYWORD_STRING", "TERM_INTVALUE", "TERM_FLOATVALUE", "TERM_STRINGVALUE", "TERM_ADD", "TERM_SUB", "TERM_MUL", "TERM_DIV", "TERM_EQ",
	"TERM_NEQ", "TERM_GT", "TERM_LT", "TERM_GTE", "TERM_LTE", "TERM_KEYWORD_ELSE", "TERM_KEYWORD_MAIN", "TERM_PSEUDO_EPSILON",
	
	"NONTERM_program", "NONTERM_param_in_first", "NONTERM_param_in_next", "NONTERM_funkce_body", "NONTERM_param_out_next",
	"NONTERM_statements", "NONTERM_state_id_list", "NONTERM_id_next", "NONTERM_for_definition",
	"NONTERM_for_assignment", "NONTERM_type", "NONTERM_expr_next", "NONTERM_expression"
};

static Term_type MAP_TOKEN_TO_TERM[] = {
	TERM_EMPTY, //prazdny defaultni typ

	TERM_KEYWORD_ELSE,
	TERM_KEYWORD_FLOAT64,
	TERM_KEYWORD_FOR,
	TERM_KEYWORD_FUNC,
	TERM_KEYWORD_IF,
	TERM_KEYWORD_INT,
	TERM_KEYWORD_RETURN,
	TERM_KEYWORD_STRING,
	TERM_KEYWORD_PACKAGE,
	TERM_ID, // KEYWORD MAIN to ID -> kontrola až zvlášť v tabulce symbolů

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
};


#define GRAMMAR_RULE_LIST__ROW_MAX_SIZE 13

static Term_type GrammmarRuleList[][GRAMMAR_RULE_LIST__ROW_MAX_SIZE] = {
	{TERM_KEYWORD_PACKAGE, TERM_ID, TERM_EOL, NONTERM_program},
	{TERM_KEYWORD_FUNC, TERM_ID, TERM_LROUNDBRACKET, NONTERM_param_in_first, TERM_RROUNDBRACKET, NONTERM_funkce_body, TERM_EOL, NONTERM_program},
	{TERM_EOL, NONTERM_program},
	{TERM_EOF},

	{TERM_ID, NONTERM_type, NONTERM_param_in_next},
	{TERM_PSEUDO_EPSILON},
	{TERM_COMMA, TERM_ID, NONTERM_type, NONTERM_param_in_next},
	{TERM_PSEUDO_EPSILON},

	{TERM_LCURLYBRACKET, TERM_EOL, NONTERM_statements, TERM_RCURLYBRACKET}, //9
	{TERM_LROUNDBRACKET, NONTERM_type, NONTERM_param_out_next, TERM_RROUNDBRACKET, TERM_LCURLYBRACKET, TERM_EOL, NONTERM_statements, TERM_RCURLYBRACKET},

	{TERM_COMMA, NONTERM_type, NONTERM_param_out_next},
	{TERM_PSEUDO_EPSILON},
	
	//<statements>
	{TERM_ID, NONTERM_state_id_list, TERM_EOL, NONTERM_statements},
	{TERM_KEYWORD_RETURN, NONTERM_expression, NONTERM_expr_next, TERM_EOL, NONTERM_statements}, //14
	{TERM_KEYWORD_IF, NONTERM_expression, TERM_LCURLYBRACKET, TERM_EOL, NONTERM_statements, TERM_RCURLYBRACKET, TERM_KEYWORD_ELSE, TERM_LCURLYBRACKET, TERM_EOL, NONTERM_statements, TERM_RCURLYBRACKET, TERM_EOL, NONTERM_statements},
	{TERM_KEYWORD_FOR, NONTERM_for_definition, TERM_SEMICOLON, NONTERM_expression, TERM_SEMICOLON, NONTERM_for_assignment, TERM_LCURLYBRACKET, TERM_EOL, NONTERM_statements, TERM_RCURLYBRACKET, TERM_EOL, NONTERM_statements},
	{TERM_EOL, NONTERM_statements},
	{TERM_PSEUDO_EPSILON},

	{TERM_LROUNDBRACKET, NONTERM_expression, NONTERM_expr_next, TERM_RROUNDBRACKET},
	{TERM_INIT, NONTERM_expression},
	{TERM_ASSIGN, NONTERM_expression},
	{TERM_COMMA, TERM_ID, NONTERM_id_next},

	{TERM_COMMA, TERM_ID, NONTERM_id_next},
	{TERM_INIT, NONTERM_expression, NONTERM_expr_next},//24
	{TERM_ASSIGN, NONTERM_expression, NONTERM_expr_next},

	{TERM_ID, TERM_INIT, NONTERM_expression},
	{TERM_PSEUDO_EPSILON},
	{TERM_ID, TERM_ASSIGN, NONTERM_expression},
	{TERM_PSEUDO_EPSILON},

	{TERM_KEYWORD_INT},//30
	{TERM_KEYWORD_FLOAT64},
	{TERM_KEYWORD_STRING},

	{TERM_COMMA, NONTERM_expression, NONTERM_expr_next},
	{TERM_PSEUDO_EPSILON}
	
	
	
	// <expression> pravidla
	
	// TODO
};


#define LL_TABLE__ROW_MAX_SIZE 20

static int LLTable[][LL_TABLE__ROW_MAX_SIZE] = {
	{ 1, 0, 3, 2, 0, 0, 4}, //<program>
	{ 0, 5, 0, 0, 0, 6}, 	//<param_in_first>
	{ 0, 0, 0, 0, 0, 8, 0, 7}, 	//<param_in_next>
	{ 0, 0, 0, 0,10, 0, 0, 0, 9}, //<funkce_body>
	{ 0, 0, 0, 0, 0,12, 0,11}, 	//<param_out_next>
	{ 0,13,17, 0, 0, 0, 0, 0, 0,18,14,15,16}, //<statements>
	{ 0, 0, 0, 0,19, 0, 0,22, 0, 0, 0, 0, 0, 0,20,21}, //<state_id_list>
	{ 0, 0, 0, 0, 0, 0, 0,23, 0, 0, 0, 0, 0, 0,24,25}, //<id_next>
	{ 0,26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,27}, //<for_definition>
	{ 0,28, 0, 0, 0, 0, 0, 0,29}, //<for_assignment>
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,30,31,32},	//<type>
	{ 0, 0,34, 0, 0,34, 0,33} //<expr_next
	//Pozor, číslování pravidel začíná od 1, ne od 0.
};


#define PRECEDENCE_TABLE__ROW_MAX_SIZE 14

static char PrecedenceTable[][PRECEDENCE_TABLE__ROW_MAX_SIZE] = {
	{ '>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>',}, //+
	
	// TODO
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