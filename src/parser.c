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
	int result;
	
	result = parserSymTableInitBuiltIn(globalSymTable); // vložení předdefinovaných funkcí
	if(result != ALL_OK){return result;}
	
	
	result = scannerTokenListSetActiveFirst(tokenList);
	if(result != ALL_OK){return result;}
	
	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	do{
		result = scannerTokenListGetActive(tokenList, &currentToken);
		if(result != ALL_OK){return result;}
		/*******************/
		if(currentToken.type == TOKEN_ID){ // máme ID...
			Token prevToken;
			prevToken.type = TOKEN_EMPTY;
			if(scannerTokenListGetPrev(tokenList, &prevToken) == 0 && prevToken.type == TOKEN_KEYWORD_FUNC){ // ...a jedná se o definici funkce
				if(symTableSearch(*globalSymTable, currentToken.attribute.string, NULL) == 1){ // pokud je již v tabulce, jedná se o redefinici -> error
					return SYNTAX_ERROR;// TODO error
				} else {
					
					
					//TODO
					
					
					printf("JOOOOP\n");
					
					
					
				}
			}
		}
		/*******************/
		if(currentToken.type != TOKEN_EOF){result = scannerTokenListMoveNext(tokenList);}
	} while(currentToken.type != TOKEN_EOF && result == ALL_OK);
	
	//TODO
	
	
	
	
	
	printf("RESULT = %d\n",result);
	
	
	
	
	
	
	
	
	
	
	
	printf("OK1\n");

	symTableInsert(globalSymTable, "key", symTableInitDataInLine(FUNC, false, 3, ((IDdataType[]){FLOAT64, INT, INT}), 2, ((IDdataType[]){STRING, STRING}), NULL));
	
	printf("OK2\n");
	
	
	SymTableParamList paramList;
	symTableParamListInit(&paramList);
	
	symTableParamListAdd(&paramList, INT);
	symTableParamListAdd(&paramList, FLOAT64);
	symTableParamListAdd(&paramList, FLOAT64);
	symTableParamListAdd(&paramList, STRING);
	symTableParamListAdd(&paramList, STRING);
	
	SymTableParamList paramList2;
	symTableParamListInit(&paramList2);
	
	symTableInsert(globalSymTable, "key", symTableInitData(FUNC, false, paramList, paramList2, NULL));
	
	
	printf("OK3\n");
	printf("SIZE: %d\n", symTableParamListGetSize(&paramList));
	printf("OK4\n");
	symTableParamListFree(&paramList);
	printf("OK5\n");
	printf("SIZE: %d\n", symTableParamListGetSize(&paramList));
	
	
	// TODO
	// BT - STROM - Tabulka symbolů - globální
	//
	// na uložení globálních:
	// PROMĚNNÝCH
	// a
	// FUNKCÍ -> + uložení vstupních a výstupních parametrů do podstromu
	//
	// + vložení předdefinovaných funkcí
	return result;
}


// Naplnění tabulky built-in funkcemi
int parserSymTableInitBuiltIn(SymTableBinTreePtr *globalSymTable)
{
	// TODO zkontrolovat podle zadání
	int result;
	result = symTableInsert(globalSymTable, "inputs", symTableInitDataInLine(FUNC, true, 0, NULL, 2, ((IDdataType[]){STRING, INT}), NULL));
	result = symTableInsert(globalSymTable, "inputi", symTableInitDataInLine(FUNC, true, 0, NULL, 2, ((IDdataType[]){INT, INT}), NULL));
	result = symTableInsert(globalSymTable, "inputf", symTableInitDataInLine(FUNC, true, 0, NULL, 2, ((IDdataType[]){FLOAT64, INT}), NULL));
	result = symTableInsert(globalSymTable, "print", symTableInitDataInLine(FUNC, true, -1, NULL, 0, NULL, NULL));
	
	result = symTableInsert(globalSymTable, "int2float", symTableInitDataInLine(FUNC, true, 1, ((IDdataType[]){INT}), 1, ((IDdataType[]){FLOAT64}), NULL));
	result = symTableInsert(globalSymTable, "float2int", symTableInitDataInLine(FUNC, true, 1, ((IDdataType[]){FLOAT64}), 1, ((IDdataType[]){INT}), NULL));
	
	result = symTableInsert(globalSymTable, "len", symTableInitDataInLine(FUNC, true, 1, ((IDdataType[]){STRING}), 1, ((IDdataType[]){INT}), NULL));
	result = symTableInsert(globalSymTable, "substr", symTableInitDataInLine(FUNC, true, 3, ((IDdataType[]){STRING, STRING, STRING}), 1, ((IDdataType[]){STRING}), NULL));
	
	result = symTableInsert(globalSymTable, "ord", symTableInitDataInLine(FUNC, true, 2, ((IDdataType[]){STRING, STRING}), 1, ((IDdataType[]){STRING}), NULL));
	result = symTableInsert(globalSymTable, "chr", symTableInitDataInLine(FUNC, true, 1, ((IDdataType[]){STRING}), 1, ((IDdataType[]){STRING}), NULL));
	return result;
}


/****************************************************** PARSER RUN ANALYZE ******************************************************************************/

int parserRunAnalyze(TokenList *tokenList, SymTableBinTreePtr *globalSymTable)
{
	// TODO - po další přednášce
	return 0;
}