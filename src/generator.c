// generator.c

#include "generator.h"



//int generatorGenerateCode(int ruleNumber, TokenList *tokenList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle)
int generatorGenerateCode(TokenList *tokenList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	
	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;
	// get current Token
	scannerTokenListGetActive(tokenList, &currentToken, errorHandle);
	
	// výpis tokenů/u
	printf("%s ",tokenTypes[currentToken.type]);if(currentToken.type == TOKEN_EOL){printf("\n");}


	// TODO
	
	
	
	return errorHandle->errorID;
}
