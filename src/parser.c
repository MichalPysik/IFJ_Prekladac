// parser.c

#include "parser.h"



int parserAnalyze(TokenList *tokenList, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	SymTableBinTreePtr globalSymTable;
	symTableInit(&globalSymTable);
	
	
	// Sémantický pre-run, naplnění tabulky definicemi funkcí a proměnnými
	parserPreRun(tokenList, &globalSymTable, errorHandle); 
	
	
	// Syntaktická analýza + Sémantická analýza
	parserRunAnalyze(tokenList, &globalSymTable, errorHandle); 
	
	
	// tree test print
//	Print_tree(globalSymTable);
	
	
	handleFreeError(symTableDispose(&globalSymTable), __LINE__, __FILE__);
	
	return errorHandle->errorID;
}


/****************************************************** PARSER SYMTABLE PRERUN ******************************************************************************/

int parserPreRun(TokenList *tokenList, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	scannerTokenListSetActiveFirst(tokenList, errorHandle); // nastavení seznamu tokenů na začátek
	
	parserSymTableInitBuiltIn(globalSymTable, errorHandle); // vložení předdefinovaných funkcí

	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;
	Token tempToken;
	tempToken.type = TOKEN_EMPTY;
	tempToken.pos_line = 0;
	tempToken.pos_number = 0;
	int left_brackets_count = 0;
	do{
		scannerTokenListGetActive(tokenList, &currentToken, errorHandle);
		//******************************************************************************
		if(currentToken.type == TOKEN_ID){ // máme ID...
			char *tokenIdName = tokenList->active->token.attribute.string;
			if(scannerTokenListGetPrev(tokenList, &tempToken, errorHandle) == ALL_OK && tempToken.type == TOKEN_KEYWORD_FUNC){ // ...a jedná se o definici funkce
				if(symTableSearch(*globalSymTable, tokenIdName, NULL, errorHandle) == 0){ // pokud je již v tabulce, jedná se o redefinici -> error
					if(scannerTokenListGetNext(tokenList, &tempToken, errorHandle) == ALL_OK && tempToken.type == TOKEN_LROUNDBRACKET){
						//******************************************************************************
						scannerTokenListMoveNext(tokenList, errorHandle);
						scannerTokenListMoveNext(tokenList, errorHandle);
						
						
						// vytvoření functionLocalSymTable
						SymTableBinTreePtr localSymTable;
						symTableInit(&localSymTable);
						// vytvoření functionParamDataTypes
						SymTableParamList paramList;
						symTableParamListInit(&paramList, errorHandle);
						// vytvoření functionReturnDataTypes
						SymTableParamList returnList;
						symTableParamListInit(&returnList, errorHandle);
						
						
						// uložení vstupních parametrů
						while(errorHandle->errorID == ALL_OK && scannerTokenListGetActive(tokenList, &currentToken, errorHandle) == ALL_OK && currentToken.type != TOKEN_RROUNDBRACKET){
							scannerTokenListGetPrev(tokenList, &tempToken, errorHandle);
							
							if(currentToken.type == TOKEN_ID){
								if(symTableSearch(localSymTable, currentToken.attribute.string, NULL, errorHandle) == 0){
									scannerTokenListGetNext(tokenList, &tempToken, errorHandle);
									if(tempToken.type == TOKEN_KEYWORD_INT){
										//add id to local table
										symTableInsert(&localSymTable, currentToken.attribute.string, symTableInitDataInLine(VAR, false, INT, 0, NULL, 0, NULL, NULL, errorHandle), errorHandle);
										//add type to param list
										symTableParamListAdd(&paramList, INT, errorHandle);
									} else if(tempToken.type == TOKEN_KEYWORD_FLOAT64){
										//add id to local table
										symTableInsert(&localSymTable, currentToken.attribute.string, symTableInitDataInLine(VAR, false, FLOAT, 0, NULL, 0, NULL, NULL, errorHandle), errorHandle);
										//add type to param list
										symTableParamListAdd(&paramList, FLOAT, errorHandle);
									} else if(tempToken.type == TOKEN_KEYWORD_STRING){
										//add id to local table
										symTableInsert(&localSymTable, currentToken.attribute.string, symTableInitDataInLine(VAR, false, STRING, 0, NULL, 0, NULL, NULL, errorHandle), errorHandle);
										//add type to param list
										symTableParamListAdd(&paramList, STRING, errorHandle);
									} else {
										errorSet(SYNTAX_ERROR, "parserPreRun: spatny token v hlavicce funkce", __FILE__, __LINE__, errorHandle);
									}
								} else {
									errorSet(SEM_OTHER_ERROR, "parserPreRun: redefinice promenne", __FILE__, __LINE__, errorHandle);
								}
								scannerTokenListMoveNext(tokenList, errorHandle);
							} else if(currentToken.type != TOKEN_COMMA){// jinak se cyklus neukončí
								errorSet(SYNTAX_ERROR, "parserPreRun: spatny token v hlavicce funkce", __FILE__, __LINE__, errorHandle);
							}
							scannerTokenListMoveNext(tokenList, errorHandle);
						}
						
						
						scannerTokenListMoveNext(tokenList, errorHandle);
						scannerTokenListGetActive(tokenList, &currentToken, errorHandle);
						scannerTokenListGetNext(tokenList, &tempToken, errorHandle);
						// uložení výstupních parametrů (není povinné)
						if(errorHandle->errorID == ALL_OK && currentToken.type == TOKEN_LROUNDBRACKET){
							scannerTokenListMoveNext(tokenList, errorHandle);
							
							while(errorHandle->errorID == ALL_OK && scannerTokenListGetActive(tokenList, &currentToken, errorHandle) == ALL_OK && currentToken.type != TOKEN_RROUNDBRACKET){
								scannerTokenListGetPrev(tokenList, &tempToken, errorHandle);
								
								if(currentToken.type == TOKEN_KEYWORD_INT){
									//add type to return list
									symTableParamListAdd(&returnList, INT, errorHandle);
								} else if(currentToken.type == TOKEN_KEYWORD_FLOAT64){
									//add type to return list
									symTableParamListAdd(&returnList, FLOAT, errorHandle);
								} else if(currentToken.type == TOKEN_KEYWORD_STRING){
									//add type to return list
									symTableParamListAdd(&returnList, STRING, errorHandle);
								} else if(currentToken.type != TOKEN_COMMA){// jinak se cyklus neukončí
									errorSet(SYNTAX_ERROR, "parserPreRun: spatny token v hlavicce funkce", __FILE__, __LINE__, errorHandle);
								}
								scannerTokenListMoveNext(tokenList, errorHandle);
							}
						}
						
						
						scannerTokenListMoveNext(tokenList, errorHandle);
						scannerTokenListGetActive(tokenList, &currentToken, errorHandle);
						scannerTokenListGetNext(tokenList, &tempToken, errorHandle);
						
						// kontrola špičaté závorky a konce řádku
						if(!(errorHandle->errorID == ALL_OK && currentToken.type == TOKEN_LCURLYBRACKET && tempToken.type == TOKEN_EOL)){
							errorSet(SYNTAX_ERROR, "parserPreRun: spatny token v hlavicce funkce", __FILE__, __LINE__, errorHandle);
						}
						left_brackets_count++;
						
						
						// přidání funkce do tabulky symbolů (musí být přidáno i při chybě pro uvolnění paměti)
						if(errorHandle->errorID != ALL_OK){
							ErrorHandle internalErrorHandle;
							errorHandleInit(&internalErrorHandle);
							symTableInsert(globalSymTable, tokenIdName, symTableInitData(FUNC, true, NIL, paramList, returnList, localSymTable), &internalErrorHandle);
						} else {
							symTableInsert(globalSymTable, tokenIdName, symTableInitData(FUNC, true, NIL, paramList, returnList, localSymTable), errorHandle);
						}
						//******************************************************************************
					} else {
						errorSet(SYNTAX_ERROR, "parserPreRun: spatny token v hlavicce funkce", __FILE__, __LINE__, errorHandle);// chybí levá závorka u vstupních parametrů
					}
				} else {
					errorSet(SEM_OTHER_ERROR, "parserPreRun: redefinice funkce", __FILE__, __LINE__, errorHandle);// redefinice funkce
				}
			}/* else if(left_brackets_count == 0 && tempToken.type == TOKEN_EOL && scannerTokenListGetNext(tokenList, &tempToken, errorHandle) == ALL_OK && tempToken.type == TOKEN_INIT){// .. jedná se o globální proměnnou
				scannerTokenListMoveNext(tokenList, errorHandle);
				scannerTokenListGetNext(tokenList, &currentToken, errorHandle);
				if(currentToken.type == TOKEN_INTVALUE){
					symTableInsert(globalSymTable, tokenIdName, symTableInitDataInLine(VAR, false, INT, 0, NULL, 0, NULL, NULL, errorHandle), errorHandle);
				} else if(currentToken.type == TOKEN_FLOATVALUE){
					symTableInsert(globalSymTable, tokenIdName, symTableInitDataInLine(VAR, false, FLOAT, 0, NULL, 0, NULL, NULL, errorHandle), errorHandle);
				} else if(currentToken.type == TOKEN_STRINGVALUE){
					symTableInsert(globalSymTable, tokenIdName, symTableInitDataInLine(VAR, false, STRING, 0, NULL, 0, NULL, NULL, errorHandle), errorHandle);
				}
			}*/
		} else if(currentToken.type == TOKEN_LCURLYBRACKET){
			left_brackets_count++;
		} else if(currentToken.type == TOKEN_RCURLYBRACKET){
			left_brackets_count--;
		}
		//******************************************************************************
		if(currentToken.type != TOKEN_EOF && errorHandle->errorID == ALL_OK){scannerTokenListMoveNext(tokenList, errorHandle);}
	} while(currentToken.type != TOKEN_EOF && errorHandle->errorID == ALL_OK);

	return errorHandle->errorID;
}


// Naplnění tabulky built-in funkcemi
int parserSymTableInitBuiltIn(SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	symTableInsert(globalSymTable, "inputs", symTableInitDataInLine(FUNC, true, NIL, 0, NULL, 2, ((IDdataType[]){STRING, INT}), NULL, errorHandle), errorHandle);
	symTableInsert(globalSymTable, "inputi", symTableInitDataInLine(FUNC, true, NIL, 0, NULL, 2, ((IDdataType[]){INT, INT}), NULL, errorHandle), errorHandle);
	symTableInsert(globalSymTable, "inputf", symTableInitDataInLine(FUNC, true, NIL, 0, NULL, 2, ((IDdataType[]){FLOAT, INT}), NULL, errorHandle), errorHandle);
	symTableInsert(globalSymTable, "print", symTableInitDataInLine(FUNC, true, NIL, -1, NULL, 0, NULL, NULL, errorHandle), errorHandle);

	symTableInsert(globalSymTable, "int2float", symTableInitDataInLine(FUNC, true, NIL, 1, ((IDdataType[]){INT}), 1, ((IDdataType[]){FLOAT}), NULL, errorHandle), errorHandle);
	symTableInsert(globalSymTable, "float2int", symTableInitDataInLine(FUNC, true, NIL, 1, ((IDdataType[]){FLOAT}), 1, ((IDdataType[]){INT}), NULL, errorHandle), errorHandle);

	symTableInsert(globalSymTable, "len", symTableInitDataInLine(FUNC, true, NIL, 1, ((IDdataType[]){STRING}), 1, ((IDdataType[]){INT}), NULL, errorHandle), errorHandle);
	symTableInsert(globalSymTable, "substr", symTableInitDataInLine(FUNC, true, NIL, 3, ((IDdataType[]){STRING, INT, INT}), 2, ((IDdataType[]){STRING, INT}), NULL, errorHandle), errorHandle);

	symTableInsert(globalSymTable, "ord", symTableInitDataInLine(FUNC, true, NIL, 2, ((IDdataType[]){STRING, INT}), 2, ((IDdataType[]){INT, INT}), NULL, errorHandle), errorHandle);
	symTableInsert(globalSymTable, "chr", symTableInitDataInLine(FUNC, true, NIL, 1, ((IDdataType[]){INT}), 2, ((IDdataType[]){STRING, INT}), NULL, errorHandle), errorHandle);

	return errorHandle->errorID;
}


/****************************************************** PARSER RUN ANALYZE ******************************************************************************/

int parserRunAnalyze(TokenList *tokenList, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	scannerTokenListSetActiveFirst(tokenList, errorHandle);
	
	
	
	ParserStackPtr symtableStack;
	parserStackInit(&symtableStack);
	
	parserStackPush(&symtableStack, STACK_SYMTABLE_TO_DATA(globalSymTable)); // globalSymTable
	
	
	ParserStackPtr syntaxStack;
	parserStackInit(&syntaxStack);
	
	parserStackPush(&syntaxStack, STACK_TERM_TO_DATA(TERM_EOF)); // $ -> ukončovací symbol
	parserStackPush(&syntaxStack, STACK_TERM_TO_DATA(NONTERM_program)); // S -> počáteční symbol
	
	
	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;



	// TODO -> ověřit, že se funkce main nachází v tabulce -> odstranění TOKEN_KEYWORD_MAIN ze scanneru


	parserStackExpand(&syntaxStack, tokenList, &symtableStack, errorHandle);

	
/*
	while(scannerTokenListGetActive(tokenList, &currentToken, errorHandle) == ALL_OK && currentToken.type != TOKEN_EOF){
		
		//if(STACK_DATA_TO_INT(parserStackPeek(&syntaxStack)) > TERM_EPSILON){
			
			// stackExpand
			
		//} else {
			
			// stackCompare
			
		//}
		printf("%s ", tokenTypes[currentToken.type]);
		if(currentToken.type == TOKEN_EOL){
			printf("\n");
		}
		
		scannerTokenListMoveNext(tokenList, errorHandle);
	}*/
	
	parserStackFree(&syntaxStack);
	
	parserStackFree(&symtableStack);


	printf("----------------------------------------------------------\n");











	// TESTS
	//----------------------------------------------------------

	/*printf("<%d>\n", GrammmarRuleList[ROW(1)][ITEM(4)]); // přístup do seznamu pravidel [řádek-1][pravidlo-1] -> GRAMMAR_RULE_LIST__ROW_MAX_SIZE
	printf("<%d>\n", LLTable[NONTERM_ROW(2)][TERM_ITEM(1)]); // přístup do seznamu pravidel [řádek-1][pravidlo-1] -> GRAMMAR_RULE_LIST__ROW_MAX_SIZE
	// TODO - po další přednášce
	
	ParserStackPtr syntaxStackTemp;
	parserStackInit(&syntaxStackTemp);
	
	
	parserStackPush(&syntaxStackTemp, STACK_TERM_TO_DATA(GrammmarRuleList[ROW(1)][ITEM(4)]));
	parserStackPush(&syntaxStackTemp, STACK_TERM_TO_DATA(GrammmarRuleList[ROW(1)][ITEM(4)]));
	parserStackPush(&syntaxStackTemp, STACK_TERM_TO_DATA(GrammmarRuleList[ROW(1)][ITEM(4)]));
	
	
	printf("<%d>\n", STACK_DATA_TO_INT(parserStackPeek(&syntaxStackTemp)));
	printf("<%d>\n", STACK_DATA_TO_INT(parserStackPop(&syntaxStackTemp)));
	
	
	parserStackFree(&syntaxStackTemp);*/
	
	
	return errorHandle->errorID;
}


void parserStackCompare(ParserStackPtr *stack) // syntaxStack, token, error
{
	
}

int parserStackExpand(ParserStackPtr *syntaxStack, TokenList *tokenList, ParserStackPtr *symtableStack, ErrorHandle *errorHandle) // stackSyntax, token, stackSymtable, errorHandle
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	
	int result = 1; // 0 = ok; 1 = fail
	int inExpr = 0;
	
	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;
	
	scannerTokenListSetActiveFirst(tokenList, errorHandle);
	
	while(result != ALL_OK && scannerTokenListGetActive(tokenList, &currentToken, errorHandle) == ALL_OK)
	{
		// na stacku je $
		if(STACK_DATA_TO_TERM(parserStackPeek(syntaxStack)) == TERM_EOF){printf("STACK: END\n");
			if(currentToken.type == TERM_EOF){
				result = ALL_OK;
			} else {
				//result = SYNTAX_ERROR;
				errorSet(SYNTAX_ERROR, "PARSER_ANALYZE: SYNTAX_ERROR - NO EOF", __FILE__, __LINE__, errorHandle);
			}
		// na stacku je terminal
		} else if(IS_TERM(STACK_DATA_TO_TERM(parserStackPeek(syntaxStack)))){printf("STACK: TERM\n");
			if(inExpr == 1){
				inExpr = 0;
				
				printf("IN EXPRESSION:\n");
				while(scannerTokenListGetActive(tokenList, &currentToken, errorHandle) == ALL_OK && currentToken.type != STACK_DATA_TO_TERM(parserStackPeek(syntaxStack))){
					printf("%s ", tokenTypes[currentToken.type]);
					scannerTokenListMoveNext(tokenList, errorHandle);
				}
				printf("EXPRESSION END\n");
				
			} else {
				printf("token: %s\n", tokenTypes[currentToken.type]);
				printf("expected term: %s\n", termTypes[STACK_DATA_TO_TERM(parserStackPeek(syntaxStack))]);
				// na vrcholu zásobníku je stejný terminál jako aktuální token
				if(STACK_DATA_TO_TERM(parserStackPeek(syntaxStack)) == TERM_EPSILON){
					parserStackPop(syntaxStack);
				} else if(STACK_DATA_TO_TERM(parserStackPeek(syntaxStack)) == MAP_TOKEN_TO_TERM[currentToken.type]){
					parserStackPop(syntaxStack);
					scannerTokenListMoveNext(tokenList, errorHandle);
				} else  {
					errorSet(SYNTAX_ERROR, "PARSER_ANALYZE: SYNTAX_ERROR - NOT SAME TERM", __FILE__, __LINE__, errorHandle);
				}
			}
		// na stacku je neterminal
		} else if(IS_NONTERM(STACK_DATA_TO_TERM(parserStackPeek(syntaxStack)))){printf("STACK: NONTERM\n");
			if(STACK_DATA_TO_TERM(parserStackPeek(syntaxStack)) != NONTERM_expression){
				printf("nonterm: %s; term: %s (%d)\n", termTypes[STACK_DATA_TO_TERM(parserStackPeek(syntaxStack))], tokenTypes[currentToken.type], currentToken.type);
				printf("LL table: %d; %d\n", NONTERM_TO_TABLE(STACK_DATA_TO_TERM(parserStackPeek(syntaxStack))), TERM_TO_TABLE(MAP_TOKEN_TO_TERM[currentToken.type]));
				int LLtableResult = LLTable[NONTERM_TO_TABLE(STACK_DATA_TO_TERM(parserStackPeek(syntaxStack)))][TERM_TO_TABLE(MAP_TOKEN_TO_TERM[currentToken.type])];
				// pravidlo nalezeno
				if(LLtableResult != 0){
					printf("r: %d\n", LLtableResult);
					
					LLtableResult--; // indexování v poli
					
					parserStackPop(syntaxStack);
					
					int i;
					for(i = GRAMMAR_RULE_LIST__ROW_MAX_SIZE-1; i >= 0; i--) // stack push reversal
					{
						Term_type ruleItem = GrammmarRuleList[LLtableResult][i];
						if(ruleItem != 0){
							parserStackPush(syntaxStack, STACK_TERM_TO_DATA(ruleItem));
							printf("term: %s; ", termTypes[STACK_DATA_TO_TERM(parserStackPeek(syntaxStack))]);
						}
					}
					printf("\n");
				} else {
					// pravidlo nenalezeno
					errorSet(SYNTAX_ERROR, "PARSER_ANALYZE: SYNTAX_ERROR - LL TABLE -> NO RULE", __FILE__, __LINE__, errorHandle);
				}
			} else if(inExpr == 0){
				parserStackPop(syntaxStack);
				inExpr = 1;
			}
		}
	}
	
	// Prediktivní SA - obr. 110.1
	
	return errorHandle->errorID;
}


/****************************************************** SYNTAX STACK ******************************************************************************/

void parserStackInit(ParserStackPtr *stack)
{
	(*stack) = NULL;
}

ParserStackData parserStackPush(ParserStackPtr *stack, ParserStackData data)
{
    ParserStackPtr item = malloc(sizeof(struct parserStackNode));
	if(item != NULL){
		item->data = data;
		item->next = (*stack);
		(*stack) = item;
		return STACK_INT_TO_DATA(0);
	}
	return STACK_INT_TO_DATA(-1);
}

ParserStackData parserStackPeek(ParserStackPtr *stack)
{
	if((*stack) != NULL){
		return (*stack)->data;
	}
	return STACK_INT_TO_DATA(-1);
}

ParserStackData parserStackPop(ParserStackPtr *stack)
{
	ParserStackPtr top = (*stack);
	if(top != NULL){
		(*stack) = top->next;
		ParserStackData data = top->data;
		free(top);
		return data;
	}
	return STACK_INT_TO_DATA(-1);
}

void parserStackFree(ParserStackPtr *stack)
{
	while(STACK_DATA_TO_INT(parserStackPop(stack)) >= 0){}
	parserStackInit(stack);
}
