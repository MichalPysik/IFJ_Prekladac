// generator.c

#include "generator.h"



int generatorGenerateCode(TokenList *tokenList, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	scannerTokenListSetActiveFirst(tokenList, errorHandle);

	printf("test");

	// TODO
	
	
	
	return errorHandle->errorID;
}
