// main

#include "scanner.h"
#include "parser.h"
#include "generator.h"
#include "common.h"


int main()
{
	int result;
	Token currentToken = NULL;
	Token previousToken = NULL;
	do{// Dočasný main - TODO
		previousToken = currentToken;
		
		// Lexikální analýza
		// TODO - vytvořit Seznam na tokeny nebo Zásobník na tokeny
		// scannerListInit();
		// scannerListAdd();
		// scannerListGetPrev();
		// scannerListGetNext();
		// scannerListDelete();
		// scannerListFree();
		result = scannerGetToken(&currentToken);
		if(result != ALL_OK){
			errorHandle(result);
			break;
		}

		// Syntaktická analýza a Sémantická analýza
		result = parserPreRun(&currentToken,&previousToken); // Sémantický pre-run, naplnění tabulky definicemi funkcí a proměnnými
		if(result != ALL_OK){
			errorHandle(result);
			break;
		}
		result = parserAnalyze(&currentToken); // Syntaktická analýza + Sémantická analýza
		if(result != ALL_OK){
			errorHandle(result);
			break;
		}
		
		result = generatorGenerateCode(&currentToken); // tu nebo mimo while?
		if(result != ALL_OK){
			errorHandle(result);
			break;
		}
	}while(currentToken.type != TOKEN_EOF);
	
	return result;
}


// TODO SCANNER - vytvořit Seznam na tokeny nebo Zásobník na tokeny
// TODO SYMTABLE - vytvořit pomocí binárního stromu tabulku symbolů
// TODO PARSER - semantic pre run (+ vložení do tabulky symbolů i předdefinované funkce)
// TODO GENERATOR - generování kódu z tokenů + zdrojové kódy předdefinovaných funkcí
