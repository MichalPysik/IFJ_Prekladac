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
typedef struct SymTableDataStruct{
	IDtype idType;											// typ identifikátoru
	bool idDefined;											// je identifikátor definovaný
	IDdataType idDataType;									// typ proměnné nebo návratový typ funkce
	struct SymTableBinTreeNode *functionLocalSymTable;		// ukazatel na lokální tabulku symbolů funkce
	int functionParamCount;									// počet parametrů funkce
} SymTableData;

// Uzel stromu
typedef struct SymTableBinTreeNode{
	char *key;								// identifikátor
	SymTableData data;						// data uzlu
	struct SymTableBinTreeNode *leftPtr;	// levá větev stromu (menší klíč)
	struct SymTableBinTreeNode *rightPtr;	// pravá větev stromu (větší klíč)
} *SymTableBinTreePtr;


// Pomocný zásobník na procházení stromu
typedef struct BinTreeStack{
	int size;							// velikost zásobníku
	int top;							// index nejvyššího prvku
	SymTableBinTreePtr *binTreeStack;	// zásobník ukazatelů na položky
} *BinTreeStackPtr;



void symTableInit(SymTableBinTreePtr *RootPtr);

void symTableInitData(SymTableData *data, IDtype idType, bool idDefined, IDdataType idDataType, SymTableBinTreePtr functionLocalSymTable, int functionParamCount);

int symTableInsert(SymTableBinTreePtr *RootPtr, char *key, SymTableData data);

int symTableSearch(SymTableBinTreePtr RootPtr, char *key, SymTableData *data);

void symTableDispose(SymTableBinTreePtr *RootPtr);


#endif //defined _SYMTABLE_H