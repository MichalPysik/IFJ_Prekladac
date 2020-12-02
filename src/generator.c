// generator.c

#include "generator.h"



void printString(char* string){
	for (int i = 0; string[i] != '\0'; i++){
		if(string[i] <= 32)
		{
			if(string[i] < 10){
				printf("\\00%d", string[i]);
			}
			else{
				printf("\\0%d", string[i]);
			}
		}
		else if (string[i] == 35) printf("\\035");
		else if (string[i] == 92) printf("\\092");
		else {
			printf("%c", string[i]);
		}
	}
	printf("\n");
}


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
	static bool inMain = false;
	static bool singleExpression = false;
	static int expressionCounter = 0;
	static char* currentVariableID;
	static Token_type expressionType = TOKEN_EMPTY;
	
	// get current and previous Token
	scannerTokenListGetPrev(tokenList, &previousToken, errorHandle);
	errorHandleInit(errorHandle);
	scannerTokenListGetActive(tokenList, &currentToken, errorHandle);
	
	
	
	//printf("\n\nVolam generatorGenerateCode:\n\n");
	static int grammarRule  =0;
	// GENEROVÁNÍ PODLE PRAVIDEL
	while(STACK_DATA_TO_INT(parserStackPeek(leftAndRightAnalysisStack)) >= 0){
		grammarRule = STACK_DATA_TO_INT(parserStackPop(leftAndRightAnalysisStack));
		//printf("  rule: %d\n", grammarRule);
		
		// ZAČÁTEK SOUBORU (PROLOG)
		if(grammarRule == 1){
			printf(".IFJcode20\n\n\nJUMP main\n\n");
		}
		
		
		// DEFINICE FUNKCE
		if(grammarRule == 2){
			//printf("\n# -----func %s--------------------------------\n", currentToken.attribute.string);
			//printf("LABEL %s\n\n", currentToken.attribute.string);
			//printf("CREATEFRAME\nPUSHFRAME\n\n");
			inFunction = true;
		}

		if(grammarRule == 16){
			
			if(!strcmp(currentToken.attribute.string,"print")) inPrint = true;
		
		}
		
		
		// VÝRAZY
		if(52 <= grammarRule && grammarRule <= 61){
			if(inExpression == false){
				expressionType = TOKEN_EMPTY;
				printf("CREATEFRAME\n");
				printf("DEFVAR TF@$operand1\n");
				printf("DEFVAR TF@$operand2\n");
				printf("DEFVAR TF@$result\n");
			}
			inExpression = true;
			
			if(semanticRuleStack != NULL){
				Token operator; // OPERATOR
				operator.type = TOKEN_EMPTY;
				Token operand2; // OPERAND 2
				operand2.type = TOKEN_EMPTY;
				Token operand1; // OPERAND 1
				operand1.type = TOKEN_EMPTY;
				int tokenCount = 0;
				
				ParserStackPtr top = (*semanticRuleStack);
				while(top != NULL && tokenCount < 3){ // načte max první tři terminály
					//printf("\n\n    EXPRESSION: %s", tokenTypes[STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type]);
					if(STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type != TOKEN_LROUNDBRACKET && STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type != TOKEN_RROUNDBRACKET){
						if(tokenCount == 0){
							operator = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
						} else if(tokenCount == 1){
							operand2 = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
							expressionType = operand2.type;
						} else if(tokenCount == 2){
							operand1 = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
							expressionType = operand1.type;
						}
					}
					
					top = top->next;
					parserStackPop(semanticRuleStack);
					tokenCount++;
				}
				//printf("\n %s %s \n", tokenTypes[operand1.type],tokenTypes[operand2.type]);
				if(operand2.type == TOKEN_EMPTY){
					printf("POPS TF@$operand2\n");
				}
				if(operand1.type == TOKEN_EMPTY){
					printf("POPS TF@$operand1\n");
				}

				// TISK INSTRUKCI VYRAZU
				if(operator.type == TOKEN_MUL) printf("MUL TF@$result");
				else if (operator.type == TOKEN_DIV && expressionType == TOKEN_FLOATVALUE){printf("DIV TF@$result");}
				else if (operator.type == TOKEN_DIV && expressionType == TOKEN_INTVALUE){printf("IDIV TF@$result");}
				else if(operator.type == TOKEN_ADD) printf("ADD TF@$result");
				else if(operator.type == TOKEN_SUB) printf("SUB TF@$result");

				if(operand1.type == TOKEN_INTVALUE) printf(" int@%ld", operand1.attribute.integer);
				else if(operand1.type == TOKEN_FLOATVALUE) printf("float@%a", operand1.attribute.real);
				else if(operand1.type == TOKEN_STRINGVALUE) {
					printf(" string@");
					printString(operand1.attribute.string);
				}
				else if(operand1.type == TOKEN_ID){
					printf(" LF@%s", operand1.attribute.string);
				}
				else if(operand1.type == TOKEN_EMPTY){
					printf(" TF@$operand1");
				}

				if(operand2.type == TOKEN_INTVALUE) printf(" int@%ld\n", operand2.attribute.integer);
				else if(operand2.type == TOKEN_FLOATVALUE) printf(" float@%a\n", operand2.attribute.real);
				else if(operand2.type == TOKEN_STRINGVALUE) {
					printf(" string@\n");
					printString(operand2.attribute.string);
				}
				else if(operand2.type == TOKEN_ID){
					printf(" LF@%s\n", operand2.attribute.string);
				}
				else if(operand2.type == TOKEN_EMPTY){
					printf(" TF@$operand2\n");
				}
					
				printf("PUSHS TF@$result\n\n");
				/*
				if(data.tokenType == TOKEN_MUL)printf("MUL %s", pomoc);
				else if(data.tokenType == TOKEN_DIV) printf("DIV %s", pomoc);
				else if(data.tokenType == TOKEN_ADD) printf("ADD %s", pomoc);
				else if(data.tokenType == TOKEN_SUB) printf("SUB %s", pomoc);
				else if(data.tokenType == TOKEN_INTVALUE) printf("int@%ld ", data.token.attribute.integer);
				else if(data.tokenType == TOKEN_FLOATVALUE) printf("float@%a ", data.token.attribute.real);
				else if(data.tokenType == TOKEN_STRINGVALUE) {
					printf("string@");
					printString(data.token.attribute.string);
				}
				*/
				//TODO
				// provedení operace podle čísla pravidla
				// + něco s výsledným výrazem
			}
			
		} else if(grammarRule > 61 && currentToken.type == TOKEN_EOL){
			
			
			if(semanticRuleStack != NULL){
				ParserStackPtr top = (*semanticRuleStack);
				if(top != NULL && top->next == NULL && inExpression == false){
					//printf("CREATEFRAME\n");
					//printf("");
					top = top->next;
					parserStackPop(semanticRuleStack);
				}
			}
		}
		
	}
	
	if(previousToken.type == TOKEN_EOL && inExpression == true && grammarRule < 51){
		inExpression = false;
		singleExpression = false;
		printf("MOVE LF@%s TF@$result\n", currentVariableID);
		//currentVariableID = NULL;
	}
	
	
	//printf(" %s \n", tokenTypes[currentToken.type]);
	
	// GENEROVANÍ PODLE TOKENŮ
	switch (currentToken.type)
	{
		case TOKEN_INIT:
	
			scannerTokenListGetPrev(tokenList, &currentToken, errorHandle); // BERU TOKEN VLEVO od := 
			//printf("%s\n", tmp);
			printf("DEFVAR LF@%s\n", currentToken.attribute.string);
			currentVariableID = currentToken.attribute.string;
			
			scannerTokenListGetNext(tokenList, &currentToken, errorHandle); // BERU TOKEN VPRAVO od :=

			//TODO - Tisk definice temporalni promenne (tiskne i pro single expression, coz nechceme)
			if(currentToken.type != TOKEN_ID){
				//currentVariableID = currentToken.attribute.string; //ulozeni promenne pro prizazeni po vyhodnoceni vyrazu
			}
		
			break;

		case TOKEN_RROUNDBRACKET:

			if(inPrint == true) inPrint = false;
			
			break;

		case TOKEN_STRINGVALUE:

			if(inPrint == true){
				printf("WRITE string@");
				printString(currentToken.attribute.string);
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
				printf("\n# --- func %s ------------------------------\nLABEL %s\nCREATEFRAME\nPUSHFRAME\n\n\n", inFunctionName, inFunctionName);
			}

			
			if(inPrint == true && strcmp(currentToken.attribute.string,"print")){
				printf("WRITE LF@%s\n", currentToken.attribute.string);
			}
			
			// používat: strcmp() == 0 -> jinak se nemusí rovnat
			/*
			if(strcmp(currentToken.attribute.string,"main") == 0 && previousToken.type == TOKEN_KEYWORD_FUNC){
				inMain = true;
				//printf("\n# --- func %s ------------------------------\nLABEL %s\nCREATEFRAME\nPUSHFRAME\n\n\n", currentToken.attribute.string, currentToken.attribute.string);
				//printf("CREATEFRAME\nPUSHFRAME\n");
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

		/*
		case TOKEN_LROUNDBRACKET: return errorHandle->errorID;
			break;

		case TOKEN_STRINGVALUE:

		case TOKEN_KEYWORD_IF:
			

			break; 

		case TOKEN_KEYWORD_FOR:

			break;
		
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