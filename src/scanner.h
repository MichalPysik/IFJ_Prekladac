// hlavickovy soubor scanneru

#ifndef _SCANNER_H
#define _SCANNER_H

#include "common.h"


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
	TOKEN_SEMICOLON, // strednik ;
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


static char tokenTypes[][STATIC_STRING_LENGHT] = {"TOKEN_EMPTY", "TOKEN_KEYWORD_ELSE", "TOKEN_KEYWORD_FLOAT64", "TOKEN_KEYWORD_FOR", "TOKEN_KEYWORD_FUNC",
	"TOKEN_KEYWORD_IF", "TOKEN_KEYWORD_INT", "TOKEN_KEYWORD_RETURN", "TOKEN_KEYWORD_STRING", "TOKEN_KEYWORD_PACKAGE", "TOKEN_KEYWORD_MAIN", "TOKEN_ID",
	"TOKEN_ASSIGN", "TOKEN_INIT", "TOKEN_COMMA", "TOKEN_SEMICOLON", "TOKEN_LROUNDBRACKET", "TOKEN_RROUNDBRACKET", "TOKEN_LCURLYBRACKET",
	"TOKEN_RCURLYBRACKET", "TOKEN_EOL", "TOKEN_EOF", "TOKEN_INTVALUE", "TOKEN_FLOATVALUE", "TOKEN_STRINGVALUE", "TOKEN_ADD", "TOKEN_SUB", "TOKEN_MUL",
	"TOKEN_DIV", "TOKEN_EQ", "TOKEN_NEQ", "TOKEN_GT", "TOKEN_LT", "TOKEN_GTE", "TOKEN_LTE"
};


typedef union {
	char *string;
	int64_t integer;
	double real;
} Token_attribute;



typedef struct {
	Token_type type;
	Token_attribute attribute;
	int pos_line;
	int pos_number;
} Token;


//funkce ktera kontroluje zda je string v tokenu keyword, kdyz ano tak zmeni jeho typ, uvolni pamet stringu a vraci true
bool keywordCheck (Token *currentToken);


//funkce co nacte a ulozi novy token na urcitou adresu tokenu
int scannerGetToken (Token *currentToken);


/****************************************************** SCANNER TOKEN LIST ******************************************************************************/

typedef struct TokenListElement {
	Token token;
	struct TokenListElement *leftPtr;
	struct TokenListElement *rightPtr;
} *TokenListElementPtr;



typedef struct {
	TokenListElementPtr active;
	TokenListElementPtr first;
	TokenListElementPtr last;
} TokenList;



int scannerTokenListInit(TokenList *tokenList, ErrorHandle *errorHandle);

int scannerTokenListAdd(TokenList *tokenList, Token token, ErrorHandle *errorHandle);

int scannerTokenListDeleteActive(TokenList *tokenList, ErrorHandle *errorHandle);

int scannerTokenListMovePrev(TokenList *tokenList, ErrorHandle *errorHandle);

int scannerTokenListMoveNext(TokenList *tokenList, ErrorHandle *errorHandle);

int scannerTokenListGetPrev(TokenList *tokenList, Token *token, ErrorHandle *errorHandle);

int scannerTokenListGetActive(TokenList *tokenList, Token *token, ErrorHandle *errorHandle);

int scannerTokenListGetNext(TokenList *tokenList, Token *token, ErrorHandle *errorHandle);

int scannerTokenListSetActiveFirst(TokenList *tokenList, ErrorHandle *errorHandle);

int scannerTokenListSetActiveLast(TokenList *tokenList, ErrorHandle *errorHandle);

int scannerTokenListFree(TokenList *tokenList);


/****************************************************** SCANNER GET TOKEN LIST ******************************************************************************/

int scannerGetTokenList(TokenList *tokenList, ErrorHandle *errorHandle);


#endif //defined _SCANNER_H
