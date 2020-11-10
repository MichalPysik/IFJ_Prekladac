// main

#include "common.h"
#include "scanner.h"
#include "parser.h"
#include "generator.h"

int main()
{
	FILE_INPUT	= stdin;
	FILE_OUTPUT	= stdout;
	FILE_ERROR	= stderr;

	int result;
	TokenList tokenList;
	scannerTokenListInit(&tokenList);
	
	// získání seznamu s tokeny
	result = scannerGetTokenList(&tokenList);
	if(result != ALL_OK){
		scannerTokenListFree(&tokenList);
		return handleError(result);
	}
	

	// Syntaktická analýza + Sémantická analýza
	result = parserAnalyze(&tokenList); 
	if(result != ALL_OK){
		scannerTokenListFree(&tokenList);
		return handleError(result);
	}
	
	
	// generování výsledného kódu
	result = generatorGenerateCode(&tokenList); // zde nebo rovnou v scannerGetTokenList?
	if(result != ALL_OK){
		scannerTokenListFree(&tokenList);
		return handleError(result);
	}
	
	scannerTokenListFree(&tokenList);
	return result;
}


// TODO SCANNER - vytvořit Seznam na tokeny nebo Zásobník na tokeny
// TODO SYMTABLE - vytvořit pomocí binárního stromu tabulku symbolů
// TODO PARSER - semantic pre run (+ vložení do tabulky symbolů i předdefinované funkce)
// TODO GENERATOR - generování kódu z tokenů + zdrojové kódy předdefinovaných funkcí
