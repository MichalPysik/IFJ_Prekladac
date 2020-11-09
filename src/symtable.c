// tabulka symbolu implementovana pomoci stromu

#include "symtable.h"

/*
**	symTableInit ...... inicializace vyhledávacího stromu
**	symTableInsert .... vkládání nové hodnoty
**	symTableSearch .... vyhledávání hodnoty uzlu zadaného klíčem
**	symTableDispose ... zrušení celého stromu
*
**	BTPreorder ...... nerekurzivní průchod typu pre-order
**	BTInorder ....... nerekurzivní průchod typu in-order
**	BTPostorder ..... nerekurzivní průchod typu post-order
*/



void symTableInit(SymTableBinTreePtr *RootPtr)
{
	(*RootPtr) = NULL;
}

void symTableInitData(SymTableData *data, IDtype idType, bool idDefined, IDdataType idDataType, SymTableBinTreePtr functionLocalSymTable, int functionParamCount)
{
	data->idType = idType;
	data->idDefined = idDefined;
	data->idDataType = idDataType;
	data->functionLocalSymTable = functionLocalSymTable;
	data->functionParamCount = functionParamCount;
}

int symTableInsert(SymTableBinTreePtr *RootPtr, char *key, SymTableData data)
{
	SymTableBinTreePtr *ROOT = RootPtr;
	while((*ROOT) != NULL){
		if(strcmp((*ROOT)->key, key) == 0){
			return 2; // existující klíč - přepsat nebo chyba?
		} else if(Content < (*ROOT)->Cont){
			ROOT = &((*ROOT)->LPtr);
		} else {
			ROOT = &((*ROOT)->RPtr);
		}
	}
	
	(*ROOT) = (SymTableBinTreePtr)malloc(sizeof(struct SymTableBinTreeNode));
	if((*ROOT) != NULL){
		(*ROOT)->key = (char*)malloc((sizeof(char)*strlen(key))+1);
		(*ROOT)->data = data;
		(*ROOT)->leftPtr = NULL;
		(*ROOT)->rightPtr = NULL;
		if((*ROOT)->key != NULL){
			strcpy((*ROOT)->key, key);
		} else {
			free((*ROOT));
			(*ROOT) = NULL;
			return 3;
		}
		return 0;
	}
	return 1;
}

int symTableSearch(SymTableBinTreePtr RootPtr, char *key, SymTableData *data)
{
	if(RootPtr != NULL){
		if(strcmp(RootPtr->key, key) == 0){
			(*data) = RootPtr->data;
		} else if(strcmp(key, RootPtr->key) < 0){
			return symTableSearch(RootPtr->leftPtr, key, data);
		} else {
			return symTableSearch(RootPtr->rightPtr, key, data);
		}
		return 1;
	}
	return 0;
}

void symTableDispose(SymTableBinTreePtr *RootPtr)
{
	SymTableBinTreePtr root = (*RootPtr);
	if(root != NULL){
		symTableDispose(&root->leftPtr);
		symTableDispose(&root->rightPtr);
		free(root->key);
		free(root);
		(*RootPtr) = NULL;
	}
}
