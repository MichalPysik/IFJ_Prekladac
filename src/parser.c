// parser.c

#include "parser.h"
#include "symtable.h"


int parserAnalyze(TokenList tokenList)
{
	SymTableBinTreePtr symTable;
	symTableInit(&symTable);



	// Sémantický pre-run, naplnění tabulky definicemi funkcí a proměnnými
	result = parserPreRun(&tokenList, symTable); 
	if(result != ALL_OK){
		symTableDispose(&symTable);
		return result;
	}
	
	// Syntaktická analýza + Sémantická analýza
	result = parserAnalyze(&tokenList, symTable); 
	if(result != ALL_OK){
		symTableDispose(&symTable);
		return result;
	}
	
	
	
	symTableDispose(&symTable);
}


int parserPreRun(TokenList tokenList, SymTableBinTreePtr globalSymTable)
{
	SymTableData data;
	symTableInitData(&data, FUNC, false, NIL, NULL, 0);
	
	symTableInsert(&globalSymTable, "key", data);
	
	
	
	// TODO
	// BT - STROM - Tabulka symbolů - globální
	//
	// na uložení globálních:
	// PROMĚNNÝCH
	// a
	// FUNKCÍ -> + uložení vstupních a výstupních parametrů do podstromu
	//
	// + vložení předdefinovaných funkcí
}




int parserRunAnalyze(TokenList tokenList, symTable globalSymTable)
{
	// TODO - po další přednášce
}