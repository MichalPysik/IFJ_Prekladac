// hlavickovy soubor tabulky symbolu

#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#include "common.h"



#define SYMTABLE_STACK_CHUNK_SIZE 100


// Typ identifikátoru
typedef enum{
	VAR,	// Identifikátor je proměnná
	FUNC	// Identifikátor je funkce
} STdataType;

// Datový typ identifikátoru
typedef enum{
	INT,
	FLOAT64,
	STRING,
	BOOL,
	NIL
} STparamType;

// Data v uzlu
typedef struct SYM_TABLE_DATA{
	STdataType idType;										// Typ identifikátoru
	bool idDefined;											// Je identifikátor definovaný
	STparamType idDataType;									// typ proměnné nebo návratový typ funkce
	struct SYM_TABLE_BINARY_TREE *functionLocalSymTable;	// Ukazatel na lokální tabulku symbolů funkce
	int functionParamCount;									// Počet parametrů funkce
} *STdata;

// Uzel stromu
typedef struct SYM_TABLE_BINARY_TREE{
	char *key;							// Identifikátor
	struct SYM_TABLE_DATA *data;		// Data uzlu
	struct SYM_TABLE_BINARY_TREE *lptr;	// Levá větev stromu (menší klíč)
	struct SYM_TABLE_BINARY_TREE *rptr;	// Pravá větev stromu (větší klíč)
} *STtree;

//TODO pomocný zásobník

#endif //defined _SYMTABLE_H