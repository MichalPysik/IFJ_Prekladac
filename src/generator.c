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
	
	static ParserStackPtr variableStack = NULL;

	
	static int bracketCnt = 0;
	static int variableCount = 0; 

	static bool inFunction = false;
	static char *inFunctionName = NULL;
	static bool inExpression = false;
	static bool inPrint = false;
	static bool inMain = false;
	static bool singleExpression = false;
	static bool leftSide = false;

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
	
	
	//printf("\n\nVolam generatorGenerateCode:\n\n");
	//printf("TOKEN: %s \n", tokenTypes[currentToken.type]);

	static int grammarRule = 0;

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
			
			if(currentToken.type == TOKEN_ID) leftSide = true;
			if(!strcmp(currentToken.attribute.string,"print")) inPrint = true;
			
		}
		
		
		// VÝRAZY
		// VÍCE OPERANDŮ
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
					printf("ADD TF@$result");
				}
				else if(operator.type == TOKEN_SUB){
					printf("SUB TF@$result");
				}

				// OPERAND 1
				if(operand1.type == TOKEN_INTVALUE) {
					printf(" int@%ld", operand1.attribute.integer);
				}
				else if(operand1.type == TOKEN_FLOATVALUE) {
					printf("float@%a", operand1.attribute.real);
				}
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

				// OPERAND 2
				if(operand2.type == TOKEN_INTVALUE){
					printf(" int@%ld\n", operand2.attribute.integer);
				} 
				else if(operand2.type == TOKEN_FLOATVALUE){
					printf(" float@%a\n", operand2.attribute.real);
				}
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
			}
		// POUZE 1 OPERAND
		} else if(grammarRule > 61 && currentToken.type == TOKEN_EOL){
			
			if(semanticRuleStack != NULL){
				ParserStackPtr top = (*semanticRuleStack);
				if(top != NULL && top->next == NULL && inExpression == false){
					
					/*printf("MOVE LF@%s ", currentVariableID);

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
						printString(expressionValue.attribute.string);
					}
					else if(expressionValue.type == TOKEN_ID){
						printf(" LF@%s\n", expressionValue.attribute.string);
					}*/

					top = top->next;
					parserStackPop(semanticRuleStack);
				}
			}
		}
	}
	
	if(previousToken.type == TOKEN_EOL && inExpression == true && grammarRule < 51){
		inExpression = false;
		printf("MOVE LF@%s TF@$result\n", currentVariableID);
		
		//singleExpression = false;
		//currentVariableID = NULL;
	}
	
	
	
	
	// GENEROVANÍ PODLE TOKENŮ
	switch (currentToken.type)
	{
		case TOKEN_ASSIGN:
			leftSide = false;

			break;
		case TOKEN_INIT:
			//scannerTokenListGetPrev(tokenList, &currentToken, errorHandle); // BERU TOKEN VLEVO od := 
			//printf("%s\n", tmp);
			//printf("DEFVAR LF@%s\n", currentToken.attribute.string);
			currentVariableID = currentToken.attribute.string;

			//---------------------------Osetreni Vyrazu s jednou hodnotou-------------------------------------------------//
			//!!!!!!!!!!!!!!!!!!!!jeste nefunguje pro proměnné + není odzkoušeno přiřazení a := -1
			int shiftCount = 0;
			Token singleExpressionToken;

			scannerTokenListMoveNext(tokenList, errorHandle); //presunuti na prvni hodnotu za :=
			shiftCount++;
			scannerTokenListGetActive(tokenList, &currentToken, errorHandle);

			while(1){//dokud nedojdeme na nejakou hodnotu)
				if(currentToken.type != TOKEN_LROUNDBRACKET){
					singleExpressionToken = currentToken;
					break;
				}
				
				scannerTokenListMoveNext(tokenList, errorHandle); //posunuti dal
				shiftCount++;
				scannerTokenListGetActive(tokenList, &currentToken, errorHandle); //peeknuti za ni, abychom vedeli jestli to je jen jedna hodnota

			}
			bool isMultiExpression = false;
			while(currentToken.type != TOKEN_EOL){ //cekujeme jestli je po hodnote znamenko operatoru, je => je to multiExpression
				if(currentToken.type == TOKEN_ADD ||
				   currentToken.type == TOKEN_SUB ||
				   currentToken.type == TOKEN_MUL ||
				   currentToken.type == TOKEN_DIV ){
					isMultiExpression = true;
					break;
				}
				scannerTokenListMoveNext(tokenList, errorHandle); //posunuti dal
				shiftCount++;
				scannerTokenListGetActive(tokenList, &currentToken, errorHandle);
			}


			//navrat na puvodni token
			for (int i = 0; i < shiftCount; i++){scannerTokenListMovePrev(tokenList, errorHandle);}
			scannerTokenListGetActive(tokenList, &currentToken, errorHandle);

			
			if(isMultiExpression == false/*singleValueExpression == true*/){
				//printf("DEFVAR LF@%s\n", currentVariableID);
				if(singleExpressionToken.type == TOKEN_INTVALUE) printf("MOVE LF@%s int@%ld\n", currentVariableID, singleExpressionToken.attribute.integer);
				else if(singleExpressionToken.type == TOKEN_FLOATVALUE) printf("MOVE LF@%s float@%a\n", currentVariableID, singleExpressionToken.attribute.real);
				else if(singleExpressionToken.type == TOKEN_STRINGVALUE) {
					printf("MOVE LF@%s string@", currentVariableID);
					printString(singleExpressionToken.attribute.string);
				}
			}
			isMultiExpression = false;
			//////////// Osetreni Vyrazu s jednou hodnotou ////////////


			scannerTokenListGetNext(tokenList, &currentToken, errorHandle); // BERU TOKEN VPRAVO od :=
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

			// PUSHujeme do variableStacku proměnné na levé straně
			if(leftSide == true){
				ParserStackData data;
				data.token = currentToken;
				variableCount++;
				//printf("\nPUSHING: %s\n", data.token.attribute.string);
				parserStackPush(&variableStack, data);
			}
			
			if(inPrint == true && strcmp(currentToken.attribute.string,"print")){
				printf("WRITE LF@%s\n", currentToken.attribute.string);
			}
			

			if(!strcmp(currentToken.attribute.string, "inputi")){
				printf("\nCALL inputi\n");
				ParserStackData data;
				Token variableToken[variableCount];
				for(int i = variableCount - 1; i >= 0; i--)
				{
					data = parserStackPop(&variableStack);
					variableToken[i] = data.token;
					
				}
				for(int i = 0; i < variableCount; i++)
				{
					printf("POPS LF@%s\n", variableToken[i].attribute.string);
				}
				
				inputi = true;
			}
			if(!strcmp(currentToken.attribute.string, "inputf")){
				printf("\nCALL inputf\n");
				inputf = true;
			}
			if(!strcmp(currentToken.attribute.string, "inputs")){
				printf("\nCALL inputs\n");
				inputs = true;
			}
			if(!strcmp(currentToken.attribute.string, "int2float")){
				printf("\nCALL int2float\n");
				int2float = true;
			}
			if(!strcmp(currentToken.attribute.string, "float2int")){
				printf("\nCALL float2int\n");
				float2int = true;
			}
			if(!strcmp(currentToken.attribute.string, "len")){
				printf("\nCALL len\n");
				len = true;
			}
			if(!strcmp(currentToken.attribute.string, "substr")){
				//TODO
				printf("\nCALL substr\n");
				substr = true;
			}
			if(!strcmp(currentToken.attribute.string, "ord")){
				//TODO 
				printf("\nCALL ord\n");
				ord = true;
			}
			if(!strcmp(currentToken.attribute.string, "chr")){
				printf("\nCALL chr\n");
				chr = true;
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
				
				//!!! CHYBI OSETRENI VOLANI PRO VICE NAVRATOVYCH HODNOT napr a, err := inputi() nefunguje, protoze ma vice navratovych hodnot
				if(inputi == true){//INPUT INT
					printf("\n\n\nLABEL inputi\nCREATEFRAME\nDEFVAR TF@$1\nREAD TF@$1 int\n\nJUMPIFEQ $inputi_err TF@$1 nil@nil\nPUSHS TF@$1\nPUSHS int@0\nRETURN\nLABEL $inputi_err\nPUSHS int@1\nPUSHS int@0\nRETURN\n");
				}
				printf("\n\n\n");
				if(inputf == true){//INPUT FLOAT
					printf("\n\n\nLABEL inputf\nCREATEFRAME\nDEFVAR TF@$1\nREAD TF@$1 float\n\nJUMPIFEQ $inputi_err TF@$1 nil@nil\nPUSHS int@0\nPUSHS TF@$1\nRETURN\nLABEL $inputi_err\nPUSHS int@1\nPUSHS int@0\nRETURN\n");
				}
				printf("\n\n\n");
				if(inputs == true){//INPUT STRING
					printf("\n\n\nLABEL inputs\nCREATEFRAME\nDEFVAR TF@$1\nREAD TF@$1 string\n\nJUMPIFEQ $inputi_err TF@$1 nil@nil\nPUSHS int@0\nPUSHS TF@$1\nRETURN\nLABEL $inputi_err\nPUSHS int@1\nPUSHS int@0\nRETURN\n");
				}

				if(int2float == true){//INPUT STRING
					printf("\n\n\nLABEL int2float\n\nCREATEFRAME\nDEFVAR TF@$1\nDEFVAR TF@$2\nPOPS TF@$1\n\nINT2FLOAT TF@$2 TF@$1\n\nPUSHS TF@$2\nRETURN\n");
				}
				if(float2int == true){//INPUT STRING
					printf("\n\n\nLABEL float2int\n\nCREATEFRAME\nDEFVAR TF@$1\nDEFVAR TF@$2\nPOPS TF@$1\n\nFLOAT2INT TF@$2 TF@$1\n\nPUSHS TF@$2\nRETURN\n");
				}
				if(len == true){//INPUT STRING
					printf("\n\n\nLABEL len\nCREATEFRAME\n\nDEFVAR TF@s\nPOPS TF@s\n\nDEFVAR TF@$1\n\nSTRLEN TF@$1 TF@s\n\nPUSHS TF@$1\n\nRETURN\n");
				}
				if(substr == true){//INPUT STRING
					printf("\n\n\nTODO");
				}
				if(ord == true){//INPUT STRING
					printf("\n\n\nTODO");
				}
				if(chr == true){//INPUT STRING
					printf("\n\n\nLABEL chr\nCREATEFRAME\n\nDEFVAR TF@$0\nDEFVAR TF@$1\nDEFVAR TF@$2\n\nPOPS TF@$1\nLT TF@$compareResult TF@$1 int$0\nJUMPIFEQ $chr_error TF@$compareResult bool@true\nGT TF@$compareResult TF@$1 int$255\nJUMPIFEQ $chr_error TF@$compareResult bool@true\n\nINT2CHAR TF@$2\nPUSHS int@0\nPUSHS string@$2\nRETURN\n\nLABEL $chr_error\nPUSHS int@1\nPUSHS string@/000\nRETURN\n");
				}
			}
			break;

		case TOKEN_EOL:
			leftSide = false;
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
	//Print_tree(globalSymTable);
	
	
	return errorHandle->errorID;
}