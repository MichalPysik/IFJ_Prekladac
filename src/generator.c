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
	previousToken.type = TOKEN_EMPTY;
	previousToken.pos_line = 0;
	previousToken.pos_number = 0;
	

	static int bracketCnt = 0;
	static bool inFunction = false;
	static char *inFunctionName = NULL;
	static bool inExpression = false;
	static bool inPrint = false;
	
	
	// get current and previous Token
	scannerTokenListGetPrev(tokenList, &previousToken, errorHandle);
	errorHandleInit(errorHandle);
	scannerTokenListGetActive(tokenList, &currentToken, errorHandle);
	
	
	
	//printf("\n\nVolam generatorGenerateCode:\n\n");
	
	// GENEROVÁNÍ PODLE PRAVIDEL
	while(STACK_DATA_TO_INT(parserStackPeek(leftAndRightAnalysisStack)) >= 0){
		int grammarRule = STACK_DATA_TO_INT(parserStackPop(leftAndRightAnalysisStack));
		printf("  rule: %d\n", grammarRule);
		
		// ZAČÁTEK SOUBORU (PROLOG)
		if(grammarRule == 1){
			printf(".IFJcode20\n\n\nJUMP main\n\n");
		}
		
		
		// DEFINICE FUNKCE
		if(grammarRule == 2){
			printf("# -----func %s--------------------------------\n", currentToken.attribute.string);
			printf("LABEL %s\n\n", currentToken.attribute.string);
			printf("CREATEFRAME\nPUSHFRAME\n\n");
			inFunction = true;
		}

		if(grammarRule == 16){
			//printf("Atributa: %s\n", currentToken.attribute.string);
			//printf("Token Type: %s\n", tokenTypes[currentToken.type]);
			//add(10, 5)
			if(!strcmp(currentToken.attribute.string,"print")) inPrint = true;
			

			// Jestli je to print
			/*
			if(!strcmp(currentToken.attribute.string,"print") && currentToken.type == TOKEN_ID)
			{
				int shift_count = 0; //čítač posuvu
				scannerTokenListMoveNext(tokenList, errorHandle); //ted jsme na "('
				scannerTokenListMoveNext(tokenList, errorHandle); // hodnota do currentTokenu
				shift_count += 2;

				// Prochazeni parametru uvnitr funkce print
				while(scannerTokenListGetActive(tokenList, &currentToken, errorHandle) == ALL_OK && currentToken.type != TOKEN_RROUNDBRACKET)
				{ 
					if(currentToken.type == TOKEN_STRINGVALUE){
						printf("WRITE string@");
						for (int i = 0; currentToken.attribute.string[i] != '\0'; i++){
							if(currentToken.attribute.string[i] <= 32) //Pokud je white character
							{
								if(currentToken.attribute.string[i] < 10){
									printf("\\00%d", currentToken.attribute.string[i]);
								}
								else{
									printf("\\0%d", currentToken.attribute.string[i]);
								}
							}
							else if (currentToken.attribute.string[i] == 35) printf("\\035");
							else if (currentToken.attribute.string[i] == 92) printf("\\092");
							else {
								printf("%c", currentToken.attribute.string[i]);
							}
						}
						printf("\n");
					}
					else if(currentToken.type == TOKEN_INTVALUE){
						printf("WRITE int@%ld\n", currentToken.attribute.integer);
					}
					else if(currentToken.type == TOKEN_FLOATVALUE){
						printf("WRITE float@%a\n", currentToken.attribute.real);
					}
					//BOOLTHEN exp
					//else if(currentToken.type == TOKEN_BOOLVALUE){
					//		printf("WRITE bool@%s, currentToken.attribute.bool ? "true" : "false")
					//}
					else if(currentToken.type == TOKEN_ID){
						printf("WRITE LF@%s\n", currentToken.attribute.string);
					}
					scannerTokenListMoveNext(tokenList, errorHandle);// jsme s v carce
					shift_count += 1;
					scannerTokenListGetActive(tokenList, &currentToken, errorHandle);

					if(currentToken.type != TOKEN_RROUNDBRACKET && currentToken.type == TOKEN_COMMA)
					{
						scannerTokenListMoveNext(tokenList, errorHandle);// jsme v dalsim parametru
						shift_count += 1;
					}
					
				}

				for(int i = 0; i < shift_count; i++) //Navrat ukazatele na puvodni token
				{
					scannerTokenListMovePrev(tokenList, errorHandle);
				}
				scannerTokenListGetActive(tokenList, &currentToken, errorHandle);
			}*/
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
	
	
	
	printf("%s", tokenTypes[currentToken.type]);
	
	// GENEROVANÍ PODLE TOKENŮ
	switch (currentToken.type)
	{
		case TOKEN_INIT:
	
			scannerTokenListGetPrev(tokenList, &currentToken, errorHandle); // BERU TOKEN VLEVO od := 
			//printf("%s\n", tmp);
			printf("DEFVAR LF@%s\n", currentToken.attribute.string);
			
			scannerTokenListGetNext(tokenList, &currentToken, errorHandle); // BERU TOKEN VPRAVO od :=
			if(currentToken.type == TOKEN_INTVALUE) printf("MOVE LF@\n");
			else if(currentToken.type == TOKEN_STRINGVALUE) printf("MOVE string@\n");
			else if(currentToken.type == TOKEN_FLOATVALUE) printf("MOVE float@\n");

			break;

		case TOKEN_RROUNDBRACKET:
			if(inPrint == true) inPrint = false;
			break;

		case TOKEN_STRINGVALUE:

			if(inPrint == true){
				printf("WRITE string@");
				for (int i = 0; currentToken.attribute.string[i] != '\0'; i++){
					if(currentToken.attribute.string[i] <= 32) //Pokud je white character
					{
						if(currentToken.attribute.string[i] < 10){
							printf("\\00%d", currentToken.attribute.string[i]);
						}
						else{
							printf("\\0%d", currentToken.attribute.string[i]);
						}
					}
					else if (currentToken.attribute.string[i] == 35) printf("\\035");
					else if (currentToken.attribute.string[i] == 92) printf("\\092");
					else {
						printf("%c", currentToken.attribute.string[i]);
					}
				}
				printf("\n");
			}
			break;

		case TOKEN_INTVALUE:
			if(inPrint == true){
				printf("WRITE int@%ld\n", currentToken.attribute.integer);
			}
			break;

		case TOKEN_FLOATVALUE:
			if(inPrint == true){
				printf("WRITE int@%a\n", currentToken.attribute.real);
			}
			break;

		case TOKEN_ID:
		
			// TOKEN SE JMÉNEM FUNKCE
			if(inFunction == true && inFunctionName == NULL){
				inFunctionName = currentToken.attribute.string;
				printf("# --- func %s ------------------------------\nLABEL %s\nCREATEFRAME\nPUSHFRAME\n\n\n", inFunctionName, inFunctionName);
			}
			
			if(inPrint == true && strcmp(currentToken.attribute.string,"print")){
				printf("WRITE LF@%s\n", currentToken.attribute.string);
			}
			
			
			
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
			//break;
		
		
		
		/*
		case TOKEN_LCURLYBRACKET:
			bracketCnt++;
			break;
			
		
		case TOKEN_RCURLYBRACKET:
			
			// UKONČENÍ DEFINICE FUNKCE
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

		case TOKEN_LROUNDBRACKET: return errorHandle->errorID;
			break;

		case TOKEN_STRINGVALUE: 

		default:
			break;
			*/
	}	
	
	
	
	
	
	// GENEROVÁNÍ PODLE ULOŽENÝCH DAT
	
	

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
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
