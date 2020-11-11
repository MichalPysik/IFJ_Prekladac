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

// Seznam pro vstupní a výstupní prametry
typedef struct SymTableParamListElement {
	IDdataType dataType;
	struct SymTableParamListElement *leftPtr;
	struct SymTableParamListElement *rightPtr;
} *SymTableParamListElementPtr;

typedef struct {
	int size;
	SymTableParamListElementPtr active;
	SymTableParamListElementPtr first;
	SymTableParamListElementPtr last;
} SymTableParamList;

// Data v uzlu
typedef struct SymTableDataStruct{
	IDtype idType;										// typ identifikátoru
	bool idDefined;										// je identifikátor definovaný
	IDdataType idDataType;								// datový typ proměnné
	SymTableParamList functionParamDataTypes;			// ukazatel na lokální tabulku parametrů funkce (seznam)
	SymTableParamList functionReturnDataTypes;			// ukazatel na lokální tabulku návratových parametrů funkce (seznam)
	struct SymTableBinTreeNode *functionLocalSymTable;	// ukazatel na lokální tabulku symbolů funkce
} SymTableData;

// Uzel stromu
typedef struct SymTableBinTreeNode{
	char *key;								// identifikátor
	SymTableData data;						// data uzlu
	struct SymTableBinTreeNode *leftPtr;	// levá větev stromu (menší klíč)
	struct SymTableBinTreeNode *rightPtr;	// pravá větev stromu (větší klíč)
} *SymTableBinTreePtr;






// Pomocný zásobník na procházení stromu
/*typedef struct BinTreeStack{
	int size;							// velikost zásobníku
	int top;							// index nejvyššího prvku
	SymTableBinTreePtr *binTreeStack;	// zásobník ukazatelů na položky
} *BinTreeStackPtr;
*/

/****************************************************** SYMTABLE TREE ******************************************************************************/

void symTableInit(SymTableBinTreePtr *RootPtr);

SymTableData symTableInitData(IDtype idType, bool idDefined, IDdataType idDataType, SymTableParamList functionParamDataTypes, SymTableParamList functionReturnDataTypes, SymTableBinTreePtr functionLocalSymTable);

SymTableData symTableInitDataInLine(IDtype idType, bool idDefined, IDdataType idDataType, int paramCount, IDdataType paramDataTypes[], int returnCount, IDdataType returnDataTypes[], SymTableBinTreePtr functionLocalSymTable);

int symTableInsert(SymTableBinTreePtr *RootPtr, char *key, SymTableData data);

int symTableSearch(SymTableBinTreePtr RootPtr, char *key, SymTableData *data);

void symTableDispose(SymTableBinTreePtr *RootPtr);


/****************************************************** PARAMS STACK ******************************************************************************/

int symTableParamListInit(SymTableParamList *paramList);

int symTableParamListAdd(SymTableParamList *paramList, IDdataType dataType);

int symTableParamListMoveNext(SymTableParamList *paramList);

int symTableParamListGetActive(SymTableParamList *paramList, IDdataType *dataType);

int symTableParamListSetActiveFirst(SymTableParamList *paramList);

int symTableParamListGetSize(SymTableParamList *paramList);

int symTableParamListFree(SymTableParamList *paramList);


/****************************************************** TEST PRINT TREE ******************************************************************************/
void Print_tree2(SymTableBinTreePtr TempTree, char* sufix, char fromdir, char* space_size);

void Print_tree(SymTableBinTreePtr TempTree);


#endif //defined _SYMTABLE_H