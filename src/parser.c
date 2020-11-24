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

	printf("\n\n-----------------------------------------------------------------------------------\n\n");
	if(!errorExists(*errorHandle)) {
		printf("\nSYNTAX OK!\n\n");
	} else {
		printf("\nSYNTAX ERROR!\n\n");
	}
	printf("\n\n-----------------------------------------------------------------------------------\n\n");
	
	
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
	int prolog_exists = 0;
	
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
						
						
						/*// kontrola špičaté závorky a konce řádku -> kontroluje syntaktická analýza
						scannerTokenListMoveNext(tokenList, errorHandle);
						scannerTokenListGetActive(tokenList, &currentToken, errorHandle);
						scannerTokenListGetNext(tokenList, &tempToken, errorHandle);
						
						if(!(errorHandle->errorID == ALL_OK && currentToken.type == TOKEN_LCURLYBRACKET && tempToken.type == TOKEN_EOL)){
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
		} else if(currentToken.type == TOKEN_KEYWORD_PACKAGE){
			prolog_exists++;
		}
		//******************************************************************************
		if(currentToken.type != TOKEN_EOF && errorHandle->errorID == ALL_OK){scannerTokenListMoveNext(tokenList, errorHandle);}
	} while(currentToken.type != TOKEN_EOF && errorHandle->errorID == ALL_OK);
	
	
	// kontrola zda symtable obsahuje funkci main s 0 parametry a pouze jeden prolog
	SymTableData data;
	if(!(symTableSearch(*globalSymTable, "main", &data, errorHandle) == 1 && symTableParamListGetSize(&data.functionParamDataTypes, errorHandle) == 0 && symTableParamListGetSize(&data.functionReturnDataTypes, errorHandle) == 0 && prolog_exists != 0)){
		errorSet(SEM_OTHER_ERROR, "parserPreRun: prolog or main function doesn't exist!", __FILE__, __LINE__, errorHandle);// redefinice funkce
	}

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
	int BracketInExpr = 0;
	
	// semantika
	int CodeBlockNumber = 1;
	int returnExists = 0;
	int inFor = 0;
	int VarDECorDEFparamCount = 0;
	
	
	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;
	
	Token tempToken;
	tempToken.type = TOKEN_EMPTY;
	tempToken.pos_line = 0;
	tempToken.pos_number = 0;
	
	scannerTokenListSetActiveFirst(tokenList, errorHandle);


	// symtable stack
	ParserStackPtr symtableStack;
	parserStackInit(&symtableStack);
	
	
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
			if(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)) == TERM_PSEUDO_EPSILON){
				parserStackPop(&syntaxStack);
			
			// začátek EXPRESSION
			}else if(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)) == TERM_EXPRESSION){
				// EXPRESSION - START
				inExpr = 1;
				parserStackPop(&syntaxStack); // pop EXPRESSION neterminal
			
				scannerTokenListAdd(&expressionList, currentToken, errorHandle);
				scannerTokenListMoveNext(tokenList, errorHandle);
			
			// jsme v EXPRESSION
			} else if(inExpr == 1){
				
				// pokud se jedná o terminál z precedeční gramatiky
				if(TERM_PREC_ADD <= MAP_TOKEN_TO_PREC_TERM[currentToken.type] && MAP_TOKEN_TO_PREC_TERM[currentToken.type] <= TERM_PREC_IDVALUE){
					
					while(scannerTokenListGetActive(tokenList, &currentToken, errorHandle) == ALL_OK && STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)) != MAP_TOKEN_TO_TERM[currentToken.type]){
						// EXPRESSION - ADD TOKEN
						scannerTokenListAdd(&expressionList, currentToken, errorHandle);
						scannerTokenListMoveNext(tokenList, errorHandle);
					}
					while(scannerTokenListGetActive(tokenList, &currentToken, errorHandle) == ALL_OK && STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)) == MAP_TOKEN_TO_TERM[currentToken.type] && TERM_PREC_ADD <= MAP_TOKEN_TO_PREC_TERM[currentToken.type] && MAP_TOKEN_TO_PREC_TERM[currentToken.type] <= TERM_PREC_IDVALUE){
						// EXPRESSION - ADD TOKEN
						parserStackPop(&syntaxStack); // pop
						
						scannerTokenListAdd(&expressionList, currentToken, errorHandle);
						scannerTokenListMoveNext(tokenList, errorHandle);
					}
				}
				
				inExpr = 0;
				
				parserRunPrecedentSyntaxAnalysis(&expressionList, &symtableStack, globalSymTable, &CodeBlockNumber, &returnExists, &inFor, &VarDECorDEFparamCount, errorHandle);
				
				// EXPRESSION - END
				
			// porovnání zásobníku - na vrcholu zásobníku je stejný terminál jako aktuální token
			} else if(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)) == MAP_TOKEN_TO_TERM[currentToken.type]){
				parserSemanticAnalysis(tokenList, &symtableStack, globalSymTable, &CodeBlockNumber, &returnExists, &inFor, &VarDECorDEFparamCount, errorHandle);
				
				parserStackPop(&syntaxStack);
				scannerTokenListMoveNext(tokenList, errorHandle);
				
			// jinak chyba
			} else  {
				errorSet(SYNTAX_ERROR, "PARSER_ANALYZE: SYNTAX_ERROR - NOT SAME TERM", __FILE__, __LINE__, errorHandle);
			}
		
		// na stacku je neterminal
		} else if(IS_NONTERM(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)))){
			
			// načtení dalšího tokenu
			tempToken.type = TOKEN_EMPTY;
			if(tokenList->active != tokenList->last){
				scannerTokenListGetNext(tokenList, &tempToken, errorHandle);
			}
			
			// získání pravidla z LL tabulky
			int LLtableResult = LLTable[NONTERM_TO_TABLE(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)))][TERM_TO_TABLE(MAP_TOKEN_TO_TERM[currentToken.type])];
			// kotrola o jeden token dopředu, zda se jedná o volání funkce nebo expression
			if((LLtableResult == 34 || LLtableResult == 36) && MAP_TOKEN_TO_TERM[currentToken.type] == TERM_ID && MAP_TOKEN_TO_TERM[tempToken.type] != TERM_LROUNDBRACKET){LLtableResult++;}
			
			// kotrola, zda začíná EXPRESSION
			int LLtableResultExpression = LLTable[NONTERM_TO_TABLE(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)))][TERM_TO_TABLE(TERM_EXPRESSION)];
			
			
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
				
			// začátek EXPRESSION
			} else if(LLtableResultExpression != 0){
				// EXPRESSION - START
				inExpr = 1;
				
				parserStackPop(&syntaxStack); // pop EXPRESSION neterminal
			
				scannerTokenListAdd(&expressionList, currentToken, errorHandle);
				scannerTokenListMoveNext(tokenList, errorHandle);
			
			// pravidlo nenalezeno
			} else {
				errorSet(SYNTAX_ERROR, "PARSER_ANALYZE: SYNTAX_ERROR - LL TABLE -> NO RULE", __FILE__, __LINE__, errorHandle);
			}
		}
	}
	
	handleFreeError(parserTokenListFree(&expressionList), __LINE__, __FILE__);

	parserStackFree(&syntaxStack);
	
	if(symtableStack != NULL){
		while(symtableStack->next != NULL){
			SymTableBinTreePtr symtableFree = STACK_DATA_TO_SYMTABLE(parserStackPop(&symtableStack));
			handleFreeError(symTableDispose(&symtableFree), __LINE__, __FILE__);
		}
	}
	parserStackFree(&symtableStack);
	
	
	if(result != ALL_OK && !errorExists(*errorHandle)) {
		errorSet(SYNTAX_ERROR, "PARSER_ANALYZE: SYNTAX_ERROR - UNKNOWN", __FILE__, __LINE__, errorHandle);
	}
	
	return errorHandle->errorID;
}


int parserRunPrecedentSyntaxAnalysis(TokenList *expressionList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, int *CodeBlockNumber, int *returnExists, int *inFor, int *VarDECorDEFparamCount, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}


	ParserStackPtr statementStack;
	parserStackInit(&statementStack);
	
	parserStackPush(&statementStack, STACK_TERM_TO_DATA(TERM_PSEUDO_DOLLAR)); // koncový symbol a počáteční symbol

	
	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;
	
	currentToken.type = TOKEN_EOL;
	scannerTokenListAdd(expressionList, currentToken, errorHandle); // koncový symbol (token)
	currentToken.type = TOKEN_EOF;
	scannerTokenListAdd(expressionList, currentToken, errorHandle); // koncový symbol (token)
	
	scannerTokenListSetActiveFirst(expressionList, errorHandle);

	while(scannerTokenListGetActive(expressionList, &currentToken, errorHandle) == ALL_OK){

			Term_type stackTopTerm = STACK_DATA_TO_TERM(parserStackPrecedentTop(&statementStack));
			Term_type tokenTerm = MAP_TOKEN_TO_PREC_TERM[currentToken.type];
			char operation = PrecedenceTable[TERM_TO_PREC_TABLE(stackTopTerm)][TERM_TO_PREC_TABLE(tokenTerm)];
			
			if(stackTopTerm == TERM_PSEUDO_DOLLAR && tokenTerm == TERM_PSEUDO_DOLLAR){
				break;
			} else if(operation == '='){
				parserSemanticAnalysis(expressionList, symtableStack, globalSymTable, CodeBlockNumber, returnExists, inFor, VarDECorDEFparamCount, errorHandle);
				
				parserStackPush(&statementStack, STACK_TERM_TO_DATA(tokenTerm));
				scannerTokenListMoveNext(expressionList, errorHandle);
			} else if(operation == '<'){
				parserSemanticAnalysis(expressionList, symtableStack, globalSymTable, CodeBlockNumber, returnExists, inFor, VarDECorDEFparamCount, errorHandle);
				
				parserStackPrecedentTopAddHandle(&statementStack);
				parserStackPush(&statementStack, STACK_TERM_TO_DATA(tokenTerm));
				scannerTokenListMoveNext(expressionList, errorHandle);
			} else if(operation == '>'){
				int rightAnalysis = parserStackPrecedentTopPopAndPushRule(&statementStack);
				if(rightAnalysis > 0){
					// pravý rozbor
					parserRightAnalysis(rightAnalysis);
				} else {
					errorSet(SYNTAX_ERROR, "PARSER_ANALYZE: SYNTAX_ERROR - PRECEDENT TABLE -> NO RULE", __FILE__, __LINE__, errorHandle);
				}
			} else {
				errorSet(SYNTAX_ERROR, "PARSER_ANALYZE: SYNTAX_ERROR - PRECEDDENT (LAST OK TOKEN = FIRST OK TOKEN AFTER EXPRESSION)", __FILE__, __LINE__, errorHandle);
			}
	}
	// clear expressionList
	handleFreeError(parserTokenListFree(expressionList), __LINE__, __FILE__);
	scannerTokenListInit(expressionList, errorHandle);
	
	parserStackFree(&statementStack);

	return errorHandle->errorID;
}


int parserSemanticAnalysis(TokenList *tokenList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, int *CodeBlockNumber, int *returnExists, int *inFor, int *VarDECorDEFparamCount, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	
	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;
	Token tempToken;
	tempToken.type = TOKEN_EMPTY;
	tempToken.pos_line = 0;
	tempToken.pos_number = 0;
	// get current Token
	scannerTokenListGetActive(tokenList, &currentToken, errorHandle);
	ErrorHandle errorHandleTemp;
	errorHandleInit(&errorHandleTemp);
	scannerTokenListGetPrev(tokenList, &tempToken, &errorHandleTemp);
	
	// výpis tokenů/u
	//if(MAP_TOKEN_TO_TERM[currentToken.type] < TERM_PREC_ADD){
		printf("%s ",tokenTypes[currentToken.type]);if(currentToken.type == TOKEN_EOL){printf("\n");}
	//}
	
	
	/*
	// DEFINICE FUNKCE
	// při definici funkce se přidá do symtableStacku její localSymtable a CodeBlockNumber se nastaví na 1
	if(currentToken.type == TOKEN_ID && tempToken.type == TOKEN_KEYWORD_FUNC){
		char *tokenIdName = tokenList->active->token.attribute.string;
		SymTableData data;
		if(symTableSearch(*globalSymTable, tokenIdName, &data, errorHandle) == 1){ // pokud funkce existuje v globalSymTable
			if(data.functionLocalSymTable != NULL){
				data.functionLocalSymTable->data.functionParamDataTypes.size = data.functionParamDataTypes.size;
				data.functionLocalSymTable->data.functionReturnDataTypes.size = data.functionReturnDataTypes.size;
			}
			parserStackPush(symtableStack, STACK_SYMTABLE_TO_DATA(data.functionLocalSymTable));
			(*CodeBlockNumber) = 1;
		} else {
			// TODO - error kontrola typu chyby
			errorSet(SEM_OTHER_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - MISSING FUNCTION", __FILE__, __LINE__, errorHandle);
		}
	}
	
	
	// VSTUP DO NOVÉHO ROZSAHU PLATNOSTI
	// při levé složené závorce '{' nebo klíčovém slově FOR přičteme CodeBlockNumber++ a přidáme novou symtable do symtableListu
	if(currentToken.type == TOKEN_LCURLYBRACKET || currentToken.type == TOKEN_KEYWORD_FOR){
		SymTableBinTreePtr newSymTable;
		symTableInit(&newSymTable);
		parserStackPush(symtableStack, STACK_SYMTABLE_TO_DATA(newSymTable));
		(*CodeBlockNumber)++;
		
		// ZAČÁTEK CYKLU FOR
		// při klíčovém slově FOR inFor = 1;
		if(currentToken.type == TOKEN_KEYWORD_FOR){
			(*inFor) = 1;
		}
	}
	
	
	// UKONČENÍ ROZSAHU PLATNOSTI
	// při pravé složené závorce '}' jednou nebo dvakrát inFor == 1 pokud CodeBlockNumber-- a odstranění symtables (1 nebo 2)
	if(currentToken.type == TOKEN_RCURLYBRACKET){
		if(STACK_DATA_TO_INT(parserStackPeek(symtableStack)) != -1){
			SymTableBinTreePtr freeSymTable = STACK_DATA_TO_SYMTABLE(parserStackPop(symtableStack));
			handleFreeError(symTableDispose(&freeSymTable), __LINE__, __FILE__);
			(*CodeBlockNumber)--;
		} else {
			// TODO - error kontrola typu chyby
			errorSet(SEM_OTHER_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - MISSING SYMTABLE", __FILE__, __LINE__, errorHandle);
		}
		
		// KONEC CYKLU FOR
		// pokud inFor == 1 a '}' -> inFor = 0; a odstranění ještě jedné tabulky
		if((*inFor) == 1){
			(*inFor) = 0;
			
			if(STACK_DATA_TO_INT(parserStackPeek(symtableStack)) != -1){
				SymTableBinTreePtr freeSymTable = STACK_DATA_TO_SYMTABLE(parserStackPop(symtableStack));
				handleFreeError(symTableDispose(&freeSymTable), __LINE__, __FILE__);
				(*CodeBlockNumber)--;
			} else {
				// TODO - error kontrola typu chyby
				errorSet(SEM_OTHER_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - MISSING SYMTABLE (FOR)", __FILE__, __LINE__, errorHandle);
			}
		}
	}
	
	
	// UKONČENÍ FUNKCE
	// při pravé složené závorce '}' a CodeBlockNumber == 2
	// dojde také ke kontrole returnExists -> pokud má funkce návratové hodnoty a returnExists == 1 -> OK
	if(currentToken.type == TOKEN_RCURLYBRACKET && (*CodeBlockNumber) == 1){// protože při definici == 1 a pak je teprve závorka {++
		(*CodeBlockNumber)--;

		if(STACK_DATA_TO_INT(parserStackPeek(symtableStack)) != -1){
			SymTableBinTreePtr functionLocalSymTable = STACK_DATA_TO_SYMTABLE(parserStackPop(symtableStack));
			if(functionLocalSymTable != NULL){
				if(functionLocalSymTable->data.functionReturnDataTypes.size > 0 && (*returnExists) != 1){
					// TODO - error kontrola typu chyby
					errorSet(SEM_OTHER_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - key word return doesn't EXIST!", __FILE__, __LINE__, errorHandle);
				} else if(functionLocalSymTable->data.functionReturnDataTypes.size == 0 && (*returnExists) == 1){
					// TODO - error kontrola typu chyby
					errorSet(SEM_OTHER_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - key word return EXISTS!", __FILE__, __LINE__, errorHandle);
				}
			} else {
				if((*returnExists) == 1){
					// TODO - error kontrola typu chyby
					errorSet(SEM_OTHER_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - key word return EXISTS!", __FILE__, __LINE__, errorHandle);
				}
			}
		} else {
			// TODO - error kontrola typu chyby
			errorSet(SEM_OTHER_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - WRONG COUNT OF '}' - end of function not found", __FILE__, __LINE__, errorHandle);
		}
	}
	
	
	
	
	
	// RETURN NENALEZEN
	// při levé složené závorce '{' nastavíme returnExists na 0
	if(currentToken.type == TOKEN_LCURLYBRACKET){
		(*returnExists) = 0;
	}
	
	
	// TODO - [možná sémantický zásobník] - v precedeční analýze
	
	// TODO proměnná VarDECorDEFparamCount


	// RETURN NALEZEN
	// při klíčovém slově return nastavíme returnExists na 1
	// také kontrola, zda [return vrací správné typy a počet parametrů]
	if(currentToken.type == TOKEN_KEYWORD_RETURN){
		(*returnExists) = 1;
		//TODO
	}
	

	*/
	
	
	
	
	// VOLÁNÍ FUNKCE
	// kotrola, že funkce existuje v globalSymTable a vrací správný počet hodnot porovnáním s VarDECorDEFparamCount a [vstupní parametry jsou správného typu]
	// [kotroloval bych na konci precedeční analýzy expressionu]
	
	
	
	// DEKLARACE PROMĚNNÉ
	// kontrola, že proměnná není v aktuální symtable a její přidání s typem hodnoty do aktuální symtable
	// do proměnné VarDECorDEFparamCount se přidá počet výstupní proměnných (var1 , var2, var3 :=)
	
	
	
	// DEFINICE PROMĚNNÉ
	// kontrola, že proměnná je v aktuální symtable a změna jejího typu podle hodnoty
	// do proměnné VarDECorDEFparamCount se přidá počet výstupní proměnných (var1 , var2, var3 =)
	
	
	
	// PROMĚNNÁ V EXPRESSION
	// kontrola, že proměnná je v některé symtable na listu, a že [všechny typy v expression jsou stejné]
	// [kotroloval bych v precedeční analýze expressionu]
	
	
	
	return errorHandle->errorID;
}


int parserTokenListFree(TokenList *tokenList)
{
	if(tokenList != NULL){
		TokenListElementPtr first = tokenList->first;
		TokenListElementPtr destroy;
		while(first != NULL){
			destroy = first;
			first = first->rightPtr;
			free(destroy);
		}
		tokenList->first = NULL;
		tokenList->active = NULL;
		tokenList->last = NULL;
		return ALL_OK;
	}
	return INTERNAL_ERROR;
}


int parserLeftAnalysis(int ruleNumber)
{
	// Levý rozbor (číslováno od 1)
	// TODO
	//printf("Lr: %d\n", ruleNumber);
	
	return 0;
}

int parserRightAnalysis(int ruleNumber)
{
	// Pravý rozbor (číslováno od 1)
	// TODO
	//printf("Rr: %d\n", ruleNumber);
	
	return 0;
}


/****************************************************** PRECEDENT SYNTAX STACK ******************************************************************************/

ParserStackData parserStackPrecedentTop(ParserStackPtr *stack)
{
	ParserStackPtr top = (*stack);
	while(top != NULL && (!IS_TERM(STACK_DATA_TO_TERM(top->data)) || (STACK_DATA_TO_TERM(top->data)) == TERM_PSEUDO_HANDLE)){
		top = top->next;
	}
	if(top != NULL){
		return top->data;
	}
	return STACK_INT_TO_DATA(-1);
}

int parserStackPrecedentTopAddHandle(ParserStackPtr *stack)
{
	ParserStackPtr prevTop = (*stack);
	ParserStackPtr top = (*stack);
	while(top != NULL && !IS_TERM(STACK_DATA_TO_TERM(top->data))){
		prevTop = top;
		top = top->next;
	}
	if(top == prevTop){
		return STACK_DATA_TO_INT(parserStackPush(stack, STACK_TERM_TO_DATA(TERM_PSEUDO_HANDLE)));
	} else {
		ParserStackPtr item = malloc(sizeof(struct parserStackNode));
		if(item != NULL){
			item->data = STACK_TERM_TO_DATA(TERM_PSEUDO_HANDLE);
			item->next = top;
			prevTop->next = item;
			return 0;
		}
	}
	return -1;
}

int parserStackPrecedentTopHasHandle(ParserStackPtr *stack)
{
	return (STACK_DATA_TO_INT(parserStackPrecedentTop(stack)) != -1);
}

int parserStackPrecedentTopPopAndPushRule(ParserStackPtr *stack)
{
	ParserStackPtr ruleStack;
	parserStackInit(&ruleStack);
	
	// pop
	ParserStackPtr top = (*stack);
	while(top != NULL && (STACK_DATA_TO_TERM(top->data)) != TERM_PSEUDO_HANDLE){
		parserStackPush(&ruleStack, top->data);
		parserStackPop(stack);
		top = top->next;
	}
	if(top != NULL && (STACK_DATA_TO_TERM(top->data)) == TERM_PSEUDO_HANDLE){
		parserStackPop(stack);
		int found = 0;
		int i, j = 0;
		
		// find
		for(i = PREC_GRAMM_RULES_START-1; i < PREC_GRAMM_RULES_END && found == 0; i++)
		{
			found = 1;
			top = ruleStack;
			j = 0;
			
			while(STACK_DATA_TO_TERM(parserStackPeek(&top)) != -1 && j < GRAMMAR_RULE_LIST__ROW_MAX_SIZE && found == 1){
				
				if(STACK_DATA_TO_TERM(parserStackPeek(&top)) != GrammmarRuleList[i][j]){
					found = 0;
					break;
				}
				
				j++;
				top = top->next;
			}
			if(found == 1){
				break;
			}
		}
		
		parserStackFree(&ruleStack);
		
		
		if(found == 1){
			found = i;
			
			// push
			parserStackPush(stack, STACK_TERM_TO_DATA(GrammmarExprLeftRuleList[PREC_GRAMM_RULES_TO_RULE(found)]));
			
			return found+1;
		}
		return -2;
	}
	parserStackFree(&ruleStack);
	return -1;
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
