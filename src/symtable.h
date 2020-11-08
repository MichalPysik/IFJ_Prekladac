// hlavickovy soubor tabulky symbolu

#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#include "common.h"



//#define SYMTABLE_STACK_CHUNK_SIZE 100


// Typ identifikátoru
typedef enum{
	VAR,	// proměnná
	FUNC	// funkce
} IDtype;

// Datový typ identifikátoru
typedef enum{
	INT,
	FLOAT64,
	STRING,
	BOOL,
	NIL
} IDdataType;

// Data v uzlu
typedef struct SymTableData{
	IDtype idType;											// typ identifikátoru
	bool idDefined;											// je identifikátor definovaný
	IDdataType idDataType;									// typ proměnné nebo návratový typ funkce
	struct SymTableBinTree *functionLocalSymTable;			// ukazatel na lokální tabulku symbolů funkce
	int functionParamCount;									// počet parametrů funkce
} *SymTableDataPtr;

// Uzel stromu
typedef struct SymTableBinTree{
	char *key;							// identifikátor
	struct SymTableData *data;			// data uzlu
	struct SymTableBinTree *leftPtr;	// levá větev stromu (menší klíč)
	struct SymTableBinTree *rightPtr;	// pravá větev stromu (větší klíč)
} *SymTableBinTreePtr;


// Pomocný zásobník na procházení stromu
typedef struct BinTreeStack{
	int size;				// velikost zásobníku
	int top;				// index nejvyššího prvku
	SymTableBinTreePtr *binTreeStack;	// zásobník ukazatelů na položky
} *BinTreeStackPtr;

#endif //defined _SYMTABLE_H