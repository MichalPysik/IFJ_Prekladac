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

	Token previousToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;

	// get current Token
	scannerTokenListGetActive(tokenList, &currentToken, errorHandle);
	
	// výpis tokenů/u
	/*
	if(currentToken.type == TOKEN_EOL){
		printf("\n");
	}
	else if (currentToken.type == TOKEN_ID){
		printf("%s", currentToken.attribute.string);
	}

	else{
	printf("%s ",tokenTypes[currentToken.type]);
	}
	*/
	int cnt = 0;
	static bool inMain = false;
	static int bracketCnt = 0;

	switch (currentToken.type)
	{
	case TOKEN_ID:
		scannerTokenListGetPrev(tokenList, &previousToken, errorHandle);
		errorHandleInit(errorHandle);

		if(!strcmp(currentToken.attribute.string,"main") && previousToken.type == TOKEN_KEYWORD_FUNC){
			inMain = true;
			printf(".IFJcode20\n\nCREATEFRAME\nPUSHFRAME\n");
		}
		/*
		if(!strcmp(currentToken.attribute.string,"print")){
			scannerTokenListMoveNext(tokenList, &currentToken, errorHandle);
			scannerTokenListMoveNext(tokenList, &currentToken, errorHandle);
			cnt+=2;

			while (currentToken.type != TOKEN_RROUNDBRACKET)
			{
				if(currentToken.type == TOKEN_INTVALUE){
					printf("WRITE int@%s\n",currentToken.attribute.integer);
				}
				else if(currentToken.type == TOKEN_STRINGVALUE){
					printf("WRITE string@%s\n",currentToken.attribute.string);
				}
				else if(currentToken.type == TOKEN_FLOATVALUE){
					printf("WRITE float@%s\n",currentToken.attribute.real);
				}
				scannerTokenListMoveNext(tokenList, &currentToken, errorHandle);
				cnt++;
			}
		}
		*/
		//scannerTokenListMoveNext(tokenList, &currentToken, errorHandle);
		break;

	case TOKEN_LCURLYBRACKET:
		bracketCnt++;
	break;

	case TOKEN_RCURLYBRACKET:
		bracketCnt--;
		if(inMain == true && bracketCnt == 0){
			printf("POPFRAME\nRETURN\n");
		}
	break;

	default:
		break;
	}

	// TODO
	
	
	
	return errorHandle->errorID;
}
