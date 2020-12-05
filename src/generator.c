// generator.c

#include "generator.h"

ParserStackPtr reverseStack(ParserStackPtr *varStack, int varCount){
	ParserStackPtr tempStack = NULL;
	ParserStackData top;
	if (varStack == NULL) return NULL;

	for(int i = 0; i < varCount; i++){
	top = parserStackPop(varStack);
		parserStackPush(&tempStack, top);
	}
	return tempStack;
}

void preorderTreeParamsTraversal(SymTableBinTreePtr TempTree)
{
	if (TempTree == NULL) 
        return; 

	if(strcmp(TempTree->key, "_")){
		printf("\nDEFVAR LF@%s\nPOPS LF@%s\n", TempTree->key, TempTree->key);
	}
    //printf("PARAM: %s \n", TempTree->key);   

    preorderTreeParamsTraversal(TempTree->leftPtr);   
    preorderTreeParamsTraversal(TempTree->rightPtr); 
}


void pushArguments(ParserStackPtr *argStack, int argCount){
	ParserStackData data;
	printf("\n");
	for(int i = 0; i < argCount; i++){
		data = parserStackPop(argStack);
		if(data.token.type == TOKEN_INTVALUE){
            printf("PUSHS int@%ld\n", data.token.attribute.integer);
		}
		else if (data.token.type == TOKEN_FLOATVALUE){
			printf("PUSHS float@%a\n", data.token.attribute.real);
		}
		else if (data.token.type == TOKEN_STRINGVALUE){
			printf("PUSHS string@%s\n", data.token.attribute.string);
		}
		else if(data.token.type == TOKEN_ID){
			printf("PUSHS LF@%s\n", data.token.attribute.string);
		}
	}
}

void printPops(ParserStackPtr *varStack, int varCount){
	ParserStackData data;
	Token variableToken[varCount];
	for(int i = 0; i < varCount; i++)	{
		data = parserStackPop(varStack);
		printf("POPS LF@%s\n", data.token.attribute.string);
	}
}

void printString(char* string, bool printNewLine){
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
	if(printNewLine == true){
		printf("\n");
	}
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
	
	static ParserStackPtr variableStack = NULL;
	static ParserStackPtr argumentStack = NULL;
	
	static int bracketCnt = 0;
	static int variableCount = 0; 
	static int argCount = 0;

	//pomocné proměnné na uchovávání kontextu aktuální pozices
	static bool inFunction = false;
	static char *inFunctionName = NULL;
	static char *inFunctionCallName = NULL;
	static bool inExpression = false;
	static bool inPrint = false;
	static bool inMain = false;
	static bool singleExpression = false;
	static bool leftSide = false;
	static bool inArguments = false;
	static bool inFunctionCall = false;
	static bool isStringExpression = false;
	static bool inMultiAssign = false;
	static bool inReturn = false;

	//Vestavěné funkce
	static bool inputi = false;
	static bool inputf = false;
	static bool inputs = false;
	static bool int2float = false;
	static bool float2int = false;
	static bool len = false;
	static bool substr = false;
	static bool ord = false;
	static bool chr = false;


	static int expressionCounter = 0;
	static char* currentVariableID;
	static Token_type expressionType = TOKEN_EMPTY;
	
	// get current and previous Token
	scannerTokenListGetPrev(tokenList, &previousToken, errorHandle);
	errorHandleInit(errorHandle);
	scannerTokenListGetActive(tokenList, &currentToken, errorHandle);
	
	/*
	printf("\n\nVolam generatorGenerateCode:\n\n");

	printf("TOKEN: %s ", tokenTypes[currentToken.type]);
	if(currentToken.type == TOKEN_ID) {printf(", NAME: %s", currentToken.attribute.string);}
	printf("\n");
	*/
	static int grammarRule = 0;

	// GENEROVÁNÍ PODLE PRAVIDEL
	while(STACK_DATA_TO_INT(parserStackPeek(leftAndRightAnalysisStack)) >= 0){
		grammarRule = STACK_DATA_TO_INT(parserStackPop(leftAndRightAnalysisStack));
		//printf("  rule: %d\n", grammarRule);
	
		// ZAČÁTEK SOUBORU (PROLOG)
		if(grammarRule == 1){
			printf(".IFJcode20\nJUMP main\n\n");
		}
		
		
		// DEFINICE FUNKCE
		if(grammarRule == 2){
			//printf("\n# -----func %s--------------------------------\n", currentToken.attribute.string);
			//printf("LABEL %s\n\n", currentToken.attribute.string);
			//printf("CREATEFRAME\nPUSHFRAME\n\n");
			inFunction = true;
		}

		if(grammarRule == 16){
			
			if(currentToken.type == TOKEN_ID) leftSide = true;
			if(!strcmp(currentToken.attribute.string,"print")) inPrint = true;		
		}

		if(grammarRule == 34 || grammarRule == 36){
			inFunctionCall = true;
		}

		if(grammarRule == 39){
			inReturn = false;
			//TISKNE POSLEDNI PRVEK
		}

		if(inMultiAssign == true && variableCount != 1){
			if(grammarRule == 38 || grammarRule == 39){
				ParserStackData data;
				Token variableToken;
				data = parserStackPop(&variableStack);
				variableToken = data.token;
				printf("POPS LF@%s\n", variableToken.attribute.string);
			}
		}
		
		// VÝRAZY
		// VÍCE OPERANDŮ
		if(52 <= grammarRule && grammarRule <= 61){
			if(inExpression == false){
				expressionType = TOKEN_EMPTY;
				printf("\nCREATEFRAME\n");
				printf("DEFVAR TF@$operand1\n");
				printf("DEFVAR TF@$operand2\n");
				printf("DEFVAR TF@$result\n\n");
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

					if(operand1.type == TOKEN_STRINGVALUE || operand2.type == TOKEN_STRINGVALUE){
						isStringExpression = true;
					}

					top = top->next;
					parserStackPop(semanticRuleStack);
					tokenCount++;
				}
					//printf("OPERAND 2 TOKEN: %s\n", tokenTypes[operand2.type]);
					//printf("OPERAND 1 TOKEN: %s\n", tokenTypes[operand1.type]);
				if(operand2.type == TOKEN_EMPTY){
					printf("POPS TF@$operand2\n");
				}
				if(operand1.type == TOKEN_EMPTY){
					printf("POPS TF@$operand1\n");
				}


				// TISK INSTRUKCI VYRAZU
				if(operator.type == TOKEN_MUL){
					printf("MUL TF@$result");
				}
				else if (operator.type == TOKEN_DIV && expressionType == TOKEN_FLOATVALUE){
					printf("DIV TF@$result");
				}
				else if (operator.type == TOKEN_DIV && expressionType == TOKEN_INTVALUE){
					printf("IDIV TF@$result");
				}
				else if(operator.type == TOKEN_ADD){
					if(isStringExpression){
						printf("CONCAT TF@$result");
					}
					else{
						printf("ADD TF@$result");
					}
				}
				else if(operator.type == TOKEN_SUB){
					printf("SUB TF@$result");
				}

				// OPERAND 1
				if(operand1.type == TOKEN_INTVALUE) {
					printf(" int@%ld", operand1.attribute.integer);
				}
				else if(operand1.type == TOKEN_FLOATVALUE) {
					printf(" float@%a", operand1.attribute.real);
				}
				else if(operand1.type == TOKEN_STRINGVALUE) {
					printf(" string@");
					printString(operand1.attribute.string, false);
				}
				else if(operand1.type == TOKEN_ID){
					printf(" LF@%s", operand1.attribute.string);
				}
				else if(operand1.type == TOKEN_EMPTY){
					printf(" TF@$operand1");
				}

				// OPERAND 2
				if(operand2.type == TOKEN_INTVALUE){
					printf(" int@%ld\n", operand2.attribute.integer);
				} 
				else if(operand2.type == TOKEN_FLOATVALUE){
					printf(" float@%a\n", operand2.attribute.real);
				}
				else if(operand2.type == TOKEN_STRINGVALUE) {
					printf(" string@");
					printString(operand2.attribute.string, true);
				}
				else if(operand2.type == TOKEN_ID){
					printf(" LF@%s\n", operand2.attribute.string);
				}
				else if(operand2.type == TOKEN_EMPTY){
					printf(" TF@$operand2\n");
				}
				
				printf("PUSHS TF@$result\n\n");
			}
		// POUZE 1 OPERAND
		} else if(grammarRule > 61 && currentToken.type == TOKEN_EOL){
			
			if(semanticRuleStack != NULL && inReturn == false){
				ParserStackPtr top = (*semanticRuleStack);
				if(top != NULL && top->next == NULL && inExpression == false){
					//printf("\n\nSINGLE EXPRESSION: %s", tokenTypes[STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type]);
					printf("MOVE LF@%s ", currentVariableID);
	
					Token expressionValue;
					expressionValue.type = TOKEN_EMPTY;
					expressionValue = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
					
					if(expressionValue.type == TOKEN_INTVALUE){
						printf(" int@%ld\n", expressionValue.attribute.integer);
					} 
					else if(expressionValue.type == TOKEN_FLOATVALUE){
						printf(" float@%a\n", expressionValue.attribute.real);
					}
					else if(expressionValue.type == TOKEN_STRINGVALUE) {
						printf(" string@\n");
						//printString(expressionValue.attribute.string);
					}
					else if(expressionValue.type == TOKEN_ID){
						printf(" LF@%s\n", expressionValue.attribute.string);
					}

					top = top->next;
					parserStackPop(semanticRuleStack);
				}
			}
		} 
	}
	
	if(previousToken.type == TOKEN_EOL && inExpression == true && grammarRule < 51){
		inExpression = false;
		//printf("MOVE LF@%s TF@$result\n", currentVariableID);
		//if(inInitOrAssign){
		if(inReturn){
			inReturn = false;
			//push na stack navratovych hodnot;
			//posunuti na dalsi navratovou hodnotu
			//jakmile EOL, tak PUSHnout vsechny hodnoty

		}
		else if(inMultiAssign == false){
			printf("POPS LF@%s\n\n", currentVariableID);
		}
		//}
		
		//singleExpression = false;
		//currentVariableID = NULL;
	}
	
	
	
	
	// GENEROVANÍ PODLE TOKENŮ
	switch (currentToken.type)
	{
		
		case TOKEN_ASSIGN:
			leftSide = false;
			variableStack = reverseStack(&variableStack, variableCount);
			inMultiAssign = true;

			break;
		case TOKEN_INIT: ;//semicolon, protože compiler nadává
			leftSide = false;
			scannerTokenListGetPrev(tokenList, &previousToken, errorHandle);
			printf("DEFVAR LF@%s\n", previousToken.attribute.string);
			currentVariableID = previousToken.attribute.string;
			
			break;

		

		case TOKEN_STRINGVALUE:

			if(inPrint == true){
				printf("\nWRITE string@");
				printString(currentToken.attribute.string, true);
			}
			if(inArguments == true){
				ParserStackData data;
				data.token = currentToken;
				argCount++;
				//printf("\nPUSHING: %s\n", data.token.attribute.string);
				parserStackPush(&argumentStack, data);
			}
			break;

		case TOKEN_INTVALUE:
			if(inPrint == true){
				printf("\nWRITE int@%ld\n", currentToken.attribute.integer);
			}
			if(inArguments == true && inFunctionCall == true){
				ParserStackData data;
				data.token = currentToken;
				argCount++;
				//printf("\nPUSHING: %ld\n", currentToken.attribute.integer);
				parserStackPush(&argumentStack, data);
			}
			break;

		case TOKEN_FLOATVALUE:
			if(inPrint == true){
				printf("\nWRITE int@%a\n", currentToken.attribute.real);
			}
			if(inArguments == true){
				ParserStackData data;
				data.token = currentToken;
				argCount++;
				//printf("\nPUSHING: %f\n", data.token.attribute.real);
				parserStackPush(&argumentStack, data);
			}
			
			break;

		case TOKEN_ID:
			//printf("Token: %s,  Name: %s\n", tokenTypes[currentToken.type], currentToken.attribute.string);
			// TOKEN SE JMÉNEM FUNKCE
			if(inFunction == true && inFunctionName == NULL){
				inFunctionName = currentToken.attribute.string;
				printf("# --- func %s ------------------------------\nLABEL %s\nCREATEFRAME\nPUSHFRAME\n\n", inFunctionName, inFunctionName);
				SymTableData data;
				SymTableData varData;
				int i = 0;
				symTableSearch(*globalSymTable, inFunctionName, &data, errorHandle);
				preorderTreeParamsTraversal(data.functionLocalSymTable);

				//data.functionParamDataTypes.active = data.functionParamDataTypes.first;
				/*
				while(data.functionParamDataTypes.active != NULL){
					if(data.functionParamDataTypes.active->dataType == INT){
						printTreeParams(data.functionLocalSymTable);
						//printf("data.key: %s\n", data.functionLocalSymTable->key);
						//printf("\nDEFVAR LF@$%d\nPOPS LF@$%d\n", i, i);
					}
					else if(data.functionParamDataTypes.active->dataType == FLOAT){
						printf("\nDEFVAR LF@$%d\nPOPS LF@$%d\n", i, i);
					}
					else if(data.functionParamDataTypes.active->dataType == STRING){
						printf("\nDEFVAR LF@$%d\nPOPS LF@$%d\n", i, i);
					}
					
					data.functionParamDataTypes.active = data.functionParamDataTypes.active->rightPtr;
					i++;
				}*/
				
				
			}
			

			if(inArguments == true){
				ParserStackData data;
				data.token = currentToken;
				argCount++;
				//printf("\nPUSHING: %s\n", data.token.attribute.string);
				parserStackPush(&argumentStack, data);
			}

			// PUSHujeme do variableStacku proměnné na levé straně
			if(leftSide == true){
				ParserStackData data;
				data.token = currentToken;
				variableCount++;
				//printf("\nPUSHING: %s\n", data.token.attribute.string);
				parserStackPush(&variableStack, data);
			}

			//BUILT IN FUNKCE
			if(inPrint == true && strcmp(currentToken.attribute.string,"print")){
				printf("\nWRITE LF@%s\n", currentToken.attribute.string);
			}
			if(!strcmp(currentToken.attribute.string, "inputi")){inputi = true;}
			if(!strcmp(currentToken.attribute.string, "inputf")){inputf = true;}
			if(!strcmp(currentToken.attribute.string, "inputs")){inputs = true;}
			if(!strcmp(currentToken.attribute.string, "int2float")){
				/*symTableSearch(*globalSymTable, "int2float", &data, errorHandle);
				printf("int2float pocet params: %d", data.functionParamDataTypes.size);
				pushArguments(tokenList, currentToken);
				printf("\nCALL int2float\n");
				printPops(&variableStack, variableCount);*/
				int2float = true;
			}
			if(!strcmp(currentToken.attribute.string, "float2int")){
				/*printf("\nCALL float2int\n");
				symTableSearch(*globalSymTable, "float2int", &data, errorHandle);
				printf("float2int pocet params: %d, jmeno: ", data.functionParamDataTypes.size, data.functionParamDataTypes.active);
				printf("\nCALL float2float\n");
				printPops(&variableStack, variableCount);*/
				float2int = true;
			}
			if(!strcmp(currentToken.attribute.string, "len")){len = true;}
			if(!strcmp(currentToken.attribute.string, "substr")){substr = true;}
			if(!strcmp(currentToken.attribute.string, "ord")){ord = true;}
			if(!strcmp(currentToken.attribute.string, "chr")){chr = true;}
			
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
					printf("\n\nPOPFRAME\nEXIT int@0\n\n");
					} else {
						printf("\n\nPOPFRAME\nRETURN\n\n");
					}
			
						
				inFunction = false;
				inFunctionName = NULL;

				if(inputi == true){//INPUT INT
					printf("\n%s", FUNC_INPUTI);
				}
				if(inputf == true){//INPUT FLOAT
					printf("\n%s", FUNC_INPUTF);
				}
				if(inputs == true){//INPUT STRING
					printf("\n%s", FUNC_INPUTS);
				}
				if(int2float == true){//INT2FLOAT
					printf("\n%s", FUNC_INT_TO_FLOAT);
				}
				if(float2int == true){//FLOAT2INT
					printf("\n%s", FUNC_FLOAT_TO_INT);
				}
				if(len == true){//STRING LENGTH
					printf("\n%s", FUNC_STRLEN);
				}
				if(substr == true){//SUBSTRING
					printf("\n%s", FUNC_SUBSTRING);
				}
				if(ord == true){//???
					printf("\n%s", FUNC_ORD);
				}
				if(chr == true){//???
					printf("\n%s", FUNC_CHR);
				}
			}
			break;

		case TOKEN_EOL:
			leftSide = false;
			/*
			if(inReturn){	
				inReturn = false;
				//PUSHnout vsechny hodnoty ze stacku navratovych hodnot
			}*/
			variableCount = 0;
			//POPování zbytku proměnných, nemělo by se nikdy stát
			/*
			ParserStackPtr top = variableStack;
			while(top != NULL){
				//printf("POPPING %s", STACK_DATA_TO_TOKEN(parserStackPeek(&top)).attribute.string);
				top = top->next;
				parserStackPop(&variableStack);
			}
			*/
			break;
		
		case TOKEN_LROUNDBRACKET:
			//printf("PREVIOUS: %s", tokenTypes[previousToken.type]);
			//printf("inFunctionCall: %d", inFunctionCall);
			if(inFunctionCall == true && previousToken.type == TOKEN_ID){
				inFunctionCallName = previousToken.attribute.string;
				inArguments = true;
			}
			break;

		case TOKEN_RROUNDBRACKET:
			if(inArguments == true) {
				inArguments = false;
				inFunctionCall = false;
				pushArguments(&argumentStack, argCount);
				argCount = 0;
				
				printf("CALL %s\n", inFunctionCallName);
				printPops(&variableStack, variableCount);
			}
			if(inPrint == true){
				inPrint = false;
			}
			break;

		case TOKEN_KEYWORD_RETURN:
			inReturn = true;
			if(inFunction == true && inFunctionName != "main"){
				/*
				Token tempToken;
				tempToken.type = TOKEN_EMPTY;
				tempToken.pos_line = 0;
				tempToken.pos_number = 0;
				int shiftCounter = 0;

				scannerTokenListGetNext(tokenList, &tempToken, errorHandle);
				while(tempToken.type != TOKEN_EOL){
					scannerTokenListGetActive(tokenList, &tempToken, errorHandle);
					scannerTokenListMoveNext(tokenList, errorHandle);
					shiftCounter++;
				}
				
				printf("\n");
				
				for(int i = 0; i < shiftCounter; i++){
					scannerTokenListMovePrev(tokenList, errorHandle);
					scannerTokenListGetActive(tokenList, &tempToken, errorHandle);
					if(tempToken.type == TOKEN_ID){
						printf("PUSHS LF@%s\n", tempToken.attribute.string);
					}
					else if(tempToken.type == TOKEN_INTVALUE){
						printf("PUSHS int@%ld\n", tempToken.attribute.integer);
					}
					else if(tempToken.type == TOKEN_FLOATVALUE)
					{
						printf("PUSHS float@%a\n", tempToken.attribute.real);
					}
					else if (tempToken.type == TOKEN_STRINGVALUE)
					{
						printf("PUSHS string@%s\n", tempToken.attribute.string);
					}
				}
				*/

			}
			break;

		case TOKEN_KEYWORD_IF:
			

			break; 

		case TOKEN_KEYWORD_FOR:

			break;
		
		default:
			break;
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
	//Print_tree(globalSymTable);
	
	
	return errorHandle->errorID;
}