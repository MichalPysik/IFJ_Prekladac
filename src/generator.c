// generator.c

#include "generator.h"



int generatorGenerateCode(TokenList *tokenList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ParserStackPtr *leftAndRightAnalysisStack, ParserStackPtr *semanticRuleStack, ErrorHandle *errorHandle)
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
	
	
	static int bracketCnt = 0;
	static bool inFunction = false;
	static char *inFunctionName = NULL;
	static bool inExpression = false;
	
	
	// get current and previous Token
	scannerTokenListGetPrev(tokenList, &previousToken, errorHandle);
	errorHandleInit(errorHandle);
	scannerTokenListGetActive(tokenList, &currentToken, errorHandle);
	
	
	
	
	
	// GENEROVÁNÍ PODLE PRAVIDEL
	while(STACK_DATA_TO_INT(parserStackPeek(leftAndRightAnalysisStack)) >= 0){
		int grammarRule = STACK_DATA_TO_INT(parserStackPop(leftAndRightAnalysisStack));
		//printf("  rule: %d\n", grammarRule);
		
		// ZAČÁTEK SOUBORU (PROLOG)
		if(grammarRule == 1){
			printf(".IFJcode20\n\n\n");
		}
		
		
		// DEFINICE FUNKCE
		if(grammarRule == 2){
			inFunction = true;
		}
		
		
		// VÝRAZY
		if(51 <= grammarRule && grammarRule <= 61){
			inExpression = true;
			
			if(semanticRuleStack != NULL){
				Token token1operator; // OPERATOR
				token1operator.type = TOKEN_EMPTY;
				Token token2operand2; // OPERAND 2
				token2operand2.type = TOKEN_EMPTY;
				Token token3operand1; // OPERAND 1
				token3operand1.type = TOKEN_EMPTY;
				int tokenCount = 0;
				
				ParserStackPtr top = (*semanticRuleStack);
				while(top != NULL && tokenCount < 3){ // načte max první tři terminály
					//printf("    EXPRESSION: %s\n", tokenTypes[STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type]);
					
					if(tokenCount == 0){
						token1operator = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
					} else if(tokenCount == 1){
						token2operand2 = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
					} else if(tokenCount == 2){
						token3operand1 = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
					}
					
					
					top = top->next;
					parserStackPop(semanticRuleStack);
					tokenCount++;
				}
				
				//TODO
				// provedení operace podle čísla pravidla
				// + něco s výsledným výrazem
			}
			
		} else if(61 < grammarRule && currentToken.type == TOKEN_EOL){
			
			if(semanticRuleStack != NULL){
				ParserStackPtr top = (*semanticRuleStack);
				if(top != NULL && top->next == NULL && inExpression == false){
					//printf("    ALONE EXPRESSION: %s\n", tokenTypes[STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type]);
					
					// TODO něco s výsledným výrazem
					
					top = top->next;
					parserStackPop(semanticRuleStack);
				}
			}
		}
		
		if(currentToken.type == TOKEN_EOL){
			inExpression = false;
		}
	}
	
	
	
	
	
	// GENEROVANÍ PODLE TOKENŮ
	switch (currentToken.type)
	{
		case TOKEN_ID:
			

			// používat: strcmp() == 0 -> jinak se nemusí rovnat
			/*if(strcmp(currentToken.attribute.string,"main") == 0 && previousToken.type == TOKEN_KEYWORD_FUNC){
				inMain = true;
				printf("CREATEFRAME\nPUSHFRAME\n");
			}*/
			/*
			if(strcmp(currentToken.attribute.string,"print") == 0){
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
			
		// UKONČENÍ DEFINICE FUNKCE
		case TOKEN_RCURLYBRACKET:
			bracketCnt--;
			if(inFunction == true && bracketCnt == 0){
				if(strcmp(inFunctionName, "main") == 0){
					printf("POPFRAME\nEXIT int@0\n\n");
				} else {
					printf("POPFRAME\nRETURN\n\n");
				}
				
				
				inFunction = false;
				inFunctionName = NULL;
				
			}
			break;
			
		default:
			break;
	}
	
	
	
	
	
	// GENEROVÁNÍ PODLE ULOŽENÝCH DAT
	
	// TOKEN SE JMÉNEM FUNKCE
	if(inFunction == true && inFunctionName == NULL){
		inFunctionName = currentToken.attribute.string;
		printf("# --- func %s ------------------------------\nLABEL %s\nCREATEFRAME\nPUSHFRAME\n\n", inFunctionName, inFunctionName);
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	// TODO
	
	
	
	
	// výpis tokenů/u
	/*
	if(currentToken.type == TOKEN_EOL){
		printf("\n");
	}
	else if (currentToken.type == TOKEN_ID){
		printf("%s ", currentToken.attribute.string);
	}
	else{
		printf("%s ",tokenTypes[currentToken.type]);
	}
	*/
	
	
	
	return errorHandle->errorID;
}
