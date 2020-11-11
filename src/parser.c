// parser.c

#include "parser.h"



int parserAnalyze(TokenList *tokenList)
{
	int result;
	SymTableBinTreePtr globalSymTable;
	symTableInit(&globalSymTable);


	// Sémantický pre-run, naplnění tabulky definicemi funkcí a proměnnými
	result = parserPreRun(tokenList, &globalSymTable); 
	if(result != ALL_OK){
		symTableDispose(&globalSymTable);
		return result;
	}
	
	// Syntaktická analýza + Sémantická analýza
	result = parserRunAnalyze(tokenList, &globalSymTable); 
	if(result != ALL_OK){
		symTableDispose(&globalSymTable);
		return result;
	}
	
	
	Print_tree(globalSymTable);
	
	symTableDispose(&globalSymTable);
	return result;
}


/****************************************************** PARSER SYMTABLE PRERUN ******************************************************************************/

int parserPreRun(TokenList *tokenList, SymTableBinTreePtr *globalSymTable)
{
	int result = ALL_OK;
	
	result = parserSymTableInitBuiltIn(globalSymTable); // vložení předdefinovaných funkcí
	if(result != ALL_OK){return result;}
	
	
	result = scannerTokenListSetActiveFirst(tokenList);
	if(result != ALL_OK){return result;}
	
	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	do{
		result = scannerTokenListGetActive(tokenList, &currentToken);
		if(result != ALL_OK){return result;}
		/******************************************************************************/
		if(currentToken.type == TOKEN_ID){ // máme ID...
			Token tempToken;
			tempToken.type = TOKEN_EMPTY;
			if(scannerTokenListGetPrev(tokenList, &tempToken) == 0 && tempToken.type == TOKEN_KEYWORD_FUNC){ // ...a jedná se o definici funkce
				if(symTableSearch(*globalSymTable, currentToken.attribute.string, NULL) == 0){ // pokud je již v tabulce, jedná se o redefinici -> error
					if(scannerTokenListGetNext(tokenList, &tempToken) == 0 && tempToken.type == TOKEN_LROUNDBRACKET){
						/******************************************************************************/
						char functionName[STATIC_STRING_LENGHT];
						strcpy(functionName, currentToken.attribute.string);
						
						result = scannerTokenListMoveNext(tokenList);
						result = scannerTokenListMoveNext(tokenList);
						
						
						// vytvoření functionLocalSymTable
						SymTableBinTreePtr localSymTable;
						symTableInit(&localSymTable);
						// vytvoření functionParamDataTypes
						SymTableParamList paramList;
						symTableParamListInit(&paramList);
						// vytvoření functionReturnDataTypes
						SymTableParamList returnList;
						symTableParamListInit(&returnList);
						
						
						// uložení vstupních parametrů
						while(scannerTokenListGetActive(tokenList, &currentToken) == 0 && currentToken.type != TOKEN_RROUNDBRACKET && result == ALL_OK){
							result = scannerTokenListGetPrev(tokenList, &tempToken);
							
							if(currentToken.type == TOKEN_ID){
								if(symTableSearch(localSymTable, currentToken.attribute.string, NULL) == 0){
									result = scannerTokenListGetNext(tokenList, &tempToken);
									if(tempToken.type == TOKEN_KEYWORD_INT){
										//add id to local table
										result = symTableInsert(&localSymTable, currentToken.attribute.string, symTableInitDataInLine(VAR, false, INT, 0, NULL, 0, NULL, NULL));
										//add type to param list
										result = symTableParamListAdd(&paramList, INT);
									} else if(tempToken.type == TOKEN_KEYWORD_FLOAT64){
										//add id to local table
										result = symTableInsert(&localSymTable, currentToken.attribute.string, symTableInitDataInLine(VAR, false, FLOAT64, 0, NULL, 0, NULL, NULL));
										//add type to param list
										result = symTableParamListAdd(&paramList, FLOAT64);
									} else if(tempToken.type == TOKEN_KEYWORD_STRING){
										//add id to local table
										result = symTableInsert(&localSymTable, currentToken.attribute.string, symTableInitDataInLine(VAR, false, STRING, 0, NULL, 0, NULL, NULL));
										//add type to param list
										result = symTableParamListAdd(&paramList, STRING);
									} else {
										result = SYNTAX_ERROR;// TODO error - špatný token v hlavičce funkce
									}
								} else {
									result = SYNTAX_ERROR;// TODO error - redefinice proměnné
								}
								if(result == ALL_OK){
									result = scannerTokenListMoveNext(tokenList);
								}
							} else if(currentToken.type != TOKEN_COMMA){
								result = SYNTAX_ERROR;// TODO error - špatný token v hlavičce funkce
							}
							if(result == ALL_OK){
								result = scannerTokenListMoveNext(tokenList);
							}
						}
						
						
						if(result == ALL_OK){
							result = scannerTokenListMoveNext(tokenList);
							result = scannerTokenListGetActive(tokenList, &currentToken);
							result = scannerTokenListGetNext(tokenList, &tempToken);
						}
						// uložení výstupních parametrů (není povinné)
						if(result == ALL_OK && currentToken.type == TOKEN_LROUNDBRACKET){
							result = scannerTokenListMoveNext(tokenList);
							
							while(scannerTokenListGetActive(tokenList, &currentToken) == 0 && currentToken.type != TOKEN_RROUNDBRACKET && result == ALL_OK){
								result = scannerTokenListGetPrev(tokenList, &tempToken);
								
								if(currentToken.type == TOKEN_KEYWORD_INT){
									//add type to return list
									result = symTableParamListAdd(&returnList, INT);
								} else if(currentToken.type == TOKEN_KEYWORD_FLOAT64){
									//add type to return list
									result = symTableParamListAdd(&returnList, FLOAT64);
								} else if(currentToken.type == TOKEN_KEYWORD_STRING){
									//add type to return list
									result = symTableParamListAdd(&returnList, STRING);
								} else if(currentToken.type != TOKEN_COMMA){
									result = SYNTAX_ERROR;// TODO error - špatný token v hlavičce funkce
								}
								if(result == ALL_OK){
									result = scannerTokenListMoveNext(tokenList);
								}
							}						
						}
						
						
						if(result == ALL_OK){
							result = scannerTokenListMoveNext(tokenList);
							result = scannerTokenListGetActive(tokenList, &currentToken);
							result = scannerTokenListGetNext(tokenList, &tempToken);
						}
						// kontrola špičaté závorky a konce řádku
						if(!(result == ALL_OK && currentToken.type == TOKEN_LCURLYBRACKET && tempToken.type == TOKEN_EOL)){
							result = SYNTAX_ERROR;// TODO error - špatné ukončení hlavičky funkce
						}
						
						
						// přidání funkce do tabulky symbolů
						if(result != ALL_OK){
							symTableInsert(globalSymTable, functionName, symTableInitData(FUNC, true, NIL, paramList, returnList, localSymTable));
						} else {
							result = symTableInsert(globalSymTable, functionName, symTableInitData(FUNC, true, NIL, paramList, returnList, localSymTable));
						}
						/******************************************************************************/
					} else {
						return SYNTAX_ERROR;// TODO error - chybí levá závorka u vstupních parametrů
					}
				} else {
					return SYNTAX_ERROR;// TODO error - redefinice funkce
				}
			}
		}
		/******************************************************************************/
		if(currentToken.type != TOKEN_EOF && result == ALL_OK){result = scannerTokenListMoveNext(tokenList);}
	} while(currentToken.type != TOKEN_EOF && result == ALL_OK);
	
	return result;
}


// Naplnění tabulky built-in funkcemi
int parserSymTableInitBuiltIn(SymTableBinTreePtr *globalSymTable)
{
	int result;
	result = symTableInsert(globalSymTable, "inputs", symTableInitDataInLine(FUNC, true, NIL, 0, NULL, 2, ((IDdataType[]){STRING, INT}), NULL));
	result = symTableInsert(globalSymTable, "inputi", symTableInitDataInLine(FUNC, true, NIL, 0, NULL, 2, ((IDdataType[]){INT, INT}), NULL));
	result = symTableInsert(globalSymTable, "inputf", symTableInitDataInLine(FUNC, true, NIL, 0, NULL, 2, ((IDdataType[]){FLOAT64, INT}), NULL));
	result = symTableInsert(globalSymTable, "print", symTableInitDataInLine(FUNC, true, NIL, -1, NULL, 0, NULL, NULL));
	
	result = symTableInsert(globalSymTable, "int2float", symTableInitDataInLine(FUNC, true, NIL, 1, ((IDdataType[]){INT}), 1, ((IDdataType[]){FLOAT64}), NULL));
	result = symTableInsert(globalSymTable, "float2int", symTableInitDataInLine(FUNC, true, NIL, 1, ((IDdataType[]){FLOAT64}), 1, ((IDdataType[]){INT}), NULL));
	
	result = symTableInsert(globalSymTable, "len", symTableInitDataInLine(FUNC, true, NIL, 1, ((IDdataType[]){STRING}), 1, ((IDdataType[]){INT}), NULL));
	result = symTableInsert(globalSymTable, "substr", symTableInitDataInLine(FUNC, true, NIL, 3, ((IDdataType[]){STRING, INT, INT}), 2, ((IDdataType[]){STRING, INT}), NULL));
	
	result = symTableInsert(globalSymTable, "ord", symTableInitDataInLine(FUNC, true, NIL, 2, ((IDdataType[]){STRING, INT}), 2, ((IDdataType[]){INT, INT}), NULL));
	result = symTableInsert(globalSymTable, "chr", symTableInitDataInLine(FUNC, true, NIL, 1, ((IDdataType[]){INT}), 2, ((IDdataType[]){STRING, INT}), NULL));
	return result;
}


/****************************************************** PARSER RUN ANALYZE ******************************************************************************/

int parserRunAnalyze(TokenList *tokenList, SymTableBinTreePtr *globalSymTable)
{
	// TODO - po další přednášce
	return 0;
}