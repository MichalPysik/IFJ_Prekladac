// main

#include "common.h"
#include "scanner.h"
#include "parser.h"
#include "generator.h"

int getResult(ErrorHandle errorHandle, TokenList *tokenList);



int main()
{
	FILE_INPUT	= stdin;
	FILE_OUTPUT	= stdout;
	FILE_ERROR	= stderr;
	
	ErrorHandle errorHandle;
	errorHandleInit(&errorHandle);


	TokenList tokenList;
	scannerTokenListInit(&tokenList, &errorHandle);


	// získání seznamu s tokeny
	scannerGetTokenList(&tokenList, &errorHandle);
	
	
	// Syntaktická analýza + Sémantická analýza
	parserAnalyze(&tokenList, &errorHandle); 
	
	
	// generování výsledného kódu
	generatorGenerateCode(&tokenList, &errorHandle);


	int result = getResult(errorHandle, &tokenList);
	handleFreeError(scannerTokenListFree(&tokenList), __LINE__, __FILE__);
	return result;
}


int getResult(ErrorHandle errorHandle, TokenList *tokenList)
{
	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.attribute.string[0] = '\0';
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;
	ErrorHandle internalErrorHandle;
	errorHandleInit(&internalErrorHandle);
	handleFreeError(scannerTokenListGetActive(tokenList, &currentToken, &internalErrorHandle), __LINE__, __FILE__);
	return handleError(errorHandle, tokenTypes[currentToken.type], currentToken.pos_line, currentToken.pos_number, currentToken.attribute.string);
}



// TODO SCANNER - vytvořit Seznam na tokeny nebo Zásobník na tokeny
// TODO SYMTABLE - vytvořit pomocí binárního stromu tabulku symbolů
// TODO PARSER - semantic pre run (+ vložení do tabulky symbolů i předdefinované funkce)
// TODO GENERATOR - generování kódu z tokenů + zdrojové kódy předdefinovaných funkcí
