// hlavickovy soubor scanneru

#ifndef _SCANNER_H
#define _SCANNER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "errors.h"

typedef enum
{
	TOKEN_EMPTY, //prazdny defaultni typ

	TOKEN_KEYWORD_ELSE,
	TOKEN_KEYWORD_FLOAT64,
	TOKEN_KEYWORD_FOR,
	TOKEN_KEYWORD_FUNC,
	TOKEN_KEYWORD_IF,
	TOKEN_KEYWORD_INT,
	TOKEN_KEYWORD_RETURN,
	TOKEN_KEYWORD_STRING,
	TOKEN_KEYWORD_PACKAGE,
	TOKEN_KEYWORD_MAIN,

	TOKEN_ID, //identifikator
	TOKEN_ASSIGN, //prirazeni =
	TOKEN_INIT, //inicializace promenne :=
	TOKEN_COMMA, //klasicka oddelovaci carka ,
	TOKEN_LROUNDBRACKET, //leva zavorka (
	TOKEN_RROUNDBRACKET, //prava zavorka )
	TOKEN_LCURLYBRACKET, //leva spicata zavorka {
	TOKEN_RCURLYBRACKET, //prava spicata zavorka }
	TOKEN_EOL, //end of line - konec radku
	TOKEN_EOF, //end of file - konec souboru
	TOKEN_INTVALUE, //celociselna hodnota
	TOKEN_FLOATVALUE, //desetinna hodnota
	TOKEN_STRINGVALUE, //hodnota string neboli retezec znaku

	TOKEN_ADD, //scitani +
	TOKEN_SUB, //odcitani -
	TOKEN_MUL, //nasobeni *
	TOKEN_DIV, //deleni /

	TOKEN_EQ, //relacni operator ==
	TOKEN_NEQ, //relacni operator !=
	TOKEN_GT, //relacni operator >
	TOKEN_LT, //relacni operator <
	TOKEN_GTE, //relacni operator >=
	TOKEN_LTE, //relacni operator <=

} Token_type;



typedef union {
	char string[30];
	int integer;
	double real;
} Token_attribute;



typedef struct {
	Token_type type;
	Token_attribute attribute;
} Token;


typedef enum {

	SSTATE_START,
	SSTATE_ID,
	SSTATE_ID_FINAL,
	SSTATE_NUM,
	SSTATE_NUM_REAL,
	SSTATE_NUM_DOT,
	SSTATE_NUM_EXPONENT,
	SSTATE_NUM_EXPONENT_SIGN,
	SSTATE_LESS,
	SSTATE_MORE,
	SSTATE_EQUAL,
	SSTATE_EOL,
	SSTATE_SLASH,
	SSTATE_COMMENT,
	SSTATE_BLOCK_COMMENT,


} SFSM_STATE; //State of scanner FSM

//funkce co nacte a ulozi novy token na urcitou adresu tokenu
int getToken (Token *currentToken);



#endif //defined _SCANNER_H
