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
	parserRunPredictiveSyntaxAnalysis(tokenList, &globalSymTable, errorHandle); 
	
	
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
						
						// kontrola špičaté závorky a konce řádku -> kontroluje syntaktická analýza
						/*if(!(errorHandle->errorID == ALL_OK && currentToken.type == TOKEN_LCURLYBRACKET && tempToken.type == TOKEN_EOL)){
							errorSet(SYNTAX_ERROR, "parserPreRun: spatny token v hlavicce funkce", __FILE__, __LINE__, errorHandle);
						}
						left_brackets_count++;*/
						
						
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
			}/* else if(left_brackets_count == 0 && tempToken.type == TOKEN_EOL && scannerTokenListGetNext(tokenList, &tempToken, errorHandle) == ALL_OK && tempToken.type == TOKEN_INIT){// .. jedná se o globální proměnnou (Není v zadání a ani v GRAMATICE! -> nebude fungovat po odkomentování)
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

int parserRunPredictiveSyntaxAnalysis(TokenList *tokenList, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	
	int result = 1; // 0 = ok; 1 = fail
	int inExpr = 0;
	
	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;
	
	scannerTokenListSetActiveFirst(tokenList, errorHandle);
	
	
	// TODO - kontrola zda symtable obsahuje funkci main s 0 parametry
	
	
	// TODO - vyřešit symbolické tabulky (lokální rámce)
	// symtable stack
	ParserStackPtr symtableStack;
	parserStackInit(&symtableStack);
	
	//parserStackPush(&symtableStack, STACK_SYMTABLE_TO_DATA(globalSymTable)); // globalSymTable - nebude v zásobníku
	
	
	// syntax stack
	ParserStackPtr syntaxStack;
	parserStackInit(&syntaxStack);
	
	//parserStackPush(&syntaxStack, STACK_TERM_TO_DATA(TERM_EOF)); // $ -> ukončovací symbol -> už je součástí neterminálu <program>
	parserStackPush(&syntaxStack, STACK_TERM_TO_DATA(NONTERM_program)); // S -> počáteční symbol
	
	
	// expression token list
	TokenList expressionList;
	scannerTokenListInit(&expressionList, errorHandle);
	
	
	while(result != ALL_OK && scannerTokenListGetActive(tokenList, &currentToken, errorHandle) == ALL_OK)
	{
		// na stacku je ukončovací symbol - $
		if(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)) == TERM_EOF){

			// i aktuální token je ukončovací symbol
			if(MAP_TOKEN_TO_TERM[currentToken.type] == TERM_EOF){
				result = ALL_OK;
			// jinak chyba
			} else {
				errorSet(SYNTAX_ERROR, "PARSER_ANALYZE: SYNTAX_ERROR - NO EOF TOKEN", __FILE__, __LINE__, errorHandle);
			}
		
		// na stacku je terminal
		} else if(IS_TERM(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)))){

			// epsilon jen přeskočíme
			if(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)) == TERM_EPSILON){
				parserStackPop(&syntaxStack);
			// jsme v EXPRESSION
			} else if(inExpr == 1){
				// pokud tokeny EXPRESSION jsou již načteny, protože na stacku nebyl TERM, který by ukončil EXPRESSION
				if(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)) == MAP_TOKEN_TO_TERM[currentToken.type]){
				
				// pokud na stacku se ihned nacházel TERM, který by ukončil expression -> nutno načíst expression tokeny až nyní
				} else {
					while(scannerTokenListGetActive(tokenList, &currentToken, errorHandle) == ALL_OK && STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)) != MAP_TOKEN_TO_TERM[currentToken.type]){
						// EXPRESSION - ADD TOKEN
						scannerTokenListAdd(&expressionList, currentToken, errorHandle);
						scannerTokenListMoveNext(tokenList, errorHandle);
					}
				}
				inExpr = 0;
				
				parserRunPrecedentSyntaxAnalysis(&expressionList, &symtableStack, globalSymTable, errorHandle);
				
				// EXPRESSION - END
				
			// porovnání zásobníku - na vrcholu zásobníku je stejný terminál jako aktuální token
			} else if(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)) == MAP_TOKEN_TO_TERM[currentToken.type]){
				parserStackPop(&syntaxStack);
				scannerTokenListMoveNext(tokenList, errorHandle);
				
				parserSemanticAnalysis(tokenList, &symtableStack, globalSymTable, errorHandle);
				
			// jinak chyba
			} else  {
				errorSet(SYNTAX_ERROR, "PARSER_ANALYZE: SYNTAX_ERROR - NOT SAME TERM", __FILE__, __LINE__, errorHandle);
			}
		
		// na stacku je neterminal
		} else if(IS_NONTERM(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)))){
			
			// nonterminal není expression
			if(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)) != NONTERM_expression){
				
				int LLtableResult = LLTable[NONTERM_TO_TABLE(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)))][TERM_TO_TABLE(MAP_TOKEN_TO_TERM[currentToken.type])];
				// pravidlo nalezeno
				if(LLtableResult != 0){
					
					// levý rozbor
					if(inExpr == 0){// pokud nejsme v expression
						parserLeftAnalysis(LLtableResult);
					}
					
					LLtableResult--; // indexování v poli
					
					parserStackPop(&syntaxStack); // stack pop neterminal
					
					int i;
					for(i = GRAMMAR_RULE_LIST__ROW_MAX_SIZE-1; i >= 0; i--) // stack push reversal
					{
						Term_type ruleItem = GrammmarRuleList[LLtableResult][i];
						if(ruleItem != 0){
							parserStackPush(&syntaxStack, STACK_TERM_TO_DATA(ruleItem));
						}
					}
				
				// pokud ještě není na stacku TERM, který by ukončil EXPRESSION -> pokračujeme v převodu neterminálů
				} else if(inExpr == 1){
					// EXPRESSION - ADD TOKEN
					scannerTokenListAdd(&expressionList, currentToken, errorHandle);
					scannerTokenListMoveNext(tokenList, errorHandle);
					
				// pravidlo nenalezeno
				} else {
					errorSet(SYNTAX_ERROR, "PARSER_ANALYZE: SYNTAX_ERROR - LL TABLE -> NO RULE", __FILE__, __LINE__, errorHandle);
				}
			} else if(inExpr == 0){
				// EXPRESSION - START
				inExpr = 1;
				parserStackPop(&syntaxStack); // pop <expression> neterminal
				
				scannerTokenListAdd(&expressionList, currentToken, errorHandle);
				scannerTokenListMoveNext(tokenList, errorHandle);
			}
		}
	}
	
	handleFreeError(scannerTokenListFree(&expressionList), __LINE__, __FILE__);
	
	parserStackFree(&syntaxStack);
	
	parserStackFree(&symtableStack);
	
	if(result == ALL_OK) {
		printf("\nSYNTAX OK!\n\n");
	} else {
		printf("\nSYNTAX ERROR!\n\n");
	}
	
	return errorHandle->errorID;
}


int parserRunPrecedentSyntaxAnalysis(TokenList *expressionList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	
	ParserStackPtr statementStack;
	parserStackInit(&statementStack);
	
	parserStackPush(&statementStack, STACK_TERM_TO_DATA(TERM_PSEUDO_DOLLAR)); // koncový symbol a počáteční symbol

	
	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;
	
	scannerTokenListSetActiveFirst(expressionList, errorHandle);
	
	// počáteční symbol je dollar
	//scannerTokenListMoveNext(expressionList, errorHandle);
	//scannerTokenListGetActive(expressionList, &currentToken, errorHandle);
	//parserStackPush(&statementStack, STACK_TERM_TO_DATA(MAP_TOKEN_TO_TERM[currentToken.type])); // počáteční symbol
	
	while(scannerTokenListGetActive(expressionList, &currentToken, errorHandle) == ALL_OK){
		printf(" %s;", tokenTypes[currentToken.type]);
		scannerTokenListMoveNext(expressionList, errorHandle);
		
		// TODO - precedencni analyza
		
		// TODO - semanticka nalyza operatoru
		
	}
	errorHandleInit(errorHandle); // reset GetActive Error
	printf("\n\n");
	
	// clear expressionList
	handleFreeError(scannerTokenListFree(expressionList), __LINE__, __FILE__);
	scannerTokenListInit(expressionList, errorHandle);
	
	parserStackFree(&statementStack);
	
	
	
	// TODO
	// vypsaní expr tokenů
	/*Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;
	
	scannerTokenListSetActiveFirst(expressionList, errorHandle);
	
	while(scannerTokenListGetActive(expressionList, &currentToken, errorHandle) == ALL_OK){
		printf(" %s;", tokenTypes[currentToken.type]);
		scannerTokenListMoveNext(expressionList, errorHandle);
	}
	errorHandleInit(errorHandle); // reset GetActive Error
	printf("\n\n");
	
	// clear expressionList
	handleFreeError(scannerTokenListFree(expressionList), __LINE__, __FILE__);
	scannerTokenListInit(expressionList, errorHandle);*/
	
	return errorHandle->errorID;
}

int parserSemanticAnalysis(TokenList *tokenList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	
	// TODO
	
	return errorHandle->errorID;
}

int parserLeftAnalysis(int ruleNumber)
{
	// TODO
	//printf("r: %d\n", ruleNumber);
	
	return 0;
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
