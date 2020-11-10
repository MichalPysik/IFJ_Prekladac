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


/****************************************************** SYMTABLE TREE ******************************************************************************/

void symTableInit(SymTableBinTreePtr *RootPtr)
{
	(*RootPtr) = NULL;
}

SymTableData symTableInitData(IDtype idType, bool idDefined, SymTableParamList functionParamDataTypes, SymTableParamList functionReturnDataTypes, SymTableBinTreePtr functionLocalSymTable)
{
	SymTableData data;
	
	data.idType = idType;
	data.idDefined = idDefined;
	data.functionParamDataTypes = functionParamDataTypes;
	data.functionReturnDataTypes = functionReturnDataTypes;
	data.functionLocalSymTable = functionLocalSymTable;
	
	return data;
}

SymTableData symTableInitDataInLine(IDtype idType, bool idDefined, int paramCount, IDdataType paramDataTypes[], int returnCount, IDdataType returnDataTypes[], SymTableBinTreePtr functionLocalSymTable)
{
	SymTableData data;
	symTableParamListInit(&(data.functionParamDataTypes));
	symTableParamListInit(&(data.functionReturnDataTypes));
	
	data.idType = idType;
	data.idDefined = idDefined;
	
	int i;
	for(i = 0; i < paramCount; i++){
		symTableParamListAdd(&(data.functionParamDataTypes), paramDataTypes[i]);
	}
	for(i = 0; i < returnCount; i++){
		symTableParamListAdd(&(data.functionReturnDataTypes), returnDataTypes[i]);
	}
	
	data.functionLocalSymTable = functionLocalSymTable;
	
	return data;
}

int symTableInsert(SymTableBinTreePtr *RootPtr, char *key, SymTableData data)
{
	SymTableBinTreePtr *ROOT = RootPtr;
	while((*ROOT) != NULL){
		if(strcmp((*ROOT)->key, key) == 0){
			char *newKey = (char*)realloc((*ROOT)->key,(sizeof(char)*strlen(key))+1);
			if(newKey != NULL){
				strcpy(newKey, key);
				(*ROOT)->key = newKey;
			} else {
				return 2;
			}
			return 0; // existující klíč - přepsat nebo chyba?
		} else if(strcmp(key, (*ROOT)->key) < 0){
			ROOT = &((*ROOT)->leftPtr);
		} else {
			ROOT = &((*ROOT)->rightPtr);
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
			if(data != NULL){
				(*data) = RootPtr->data;
			}
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
		symTableParamListFree(&(root->data.functionParamDataTypes));
		symTableParamListFree(&(root->data.functionReturnDataTypes));
		free(root);
		(*RootPtr) = NULL;
	}
}


/****************************************************** PARAMS STACK ******************************************************************************/

int symTableParamListInit(SymTableParamList *paramList)
{
	if(paramList != NULL){
		paramList->size = 0;
		paramList->first = NULL;
		paramList->active = NULL;
		paramList->last = NULL;
		return 0;
	}
	return 1;
}

int symTableParamListAdd(SymTableParamList *paramList, IDdataType dataType)
{
	if(paramList != NULL){
		SymTableParamListElementPtr added = (SymTableParamListElementPtr)malloc(sizeof(struct SymTableParamListElement));
		if(added != NULL){
			added->dataType = dataType;
			
			added->leftPtr = paramList->last;
			added->rightPtr = NULL;
			
			if(paramList->last != NULL){
				paramList->last->rightPtr = added;
			} else {
				paramList->first = added;
			}
			paramList->last = added;
			
			(paramList->size)++;
			return 0;
		}
		return 2;
	}
	return 1;
}

int symTableParamListMoveNext(SymTableParamList *paramList)
{
	if(paramList != NULL){
		if(paramList->active != NULL){
			if(paramList->active != paramList->last){
				paramList->active = paramList->active->rightPtr;
				return 0;
			}
			return 3;
		}
		return 2;
	}
	return 1;
}

int symTableParamListGetActive(SymTableParamList *paramList, IDdataType *dataType)
{
	if(paramList != NULL){
		if(paramList->active != NULL){
			(*dataType) = paramList->active->dataType;
			return 0;
		}
		return 2;
	}
	return 1;
}

int symTableParamListSetActiveFirst(SymTableParamList *paramList)
{
	if(paramList != NULL){
		if(paramList->first != NULL){
			paramList->active = paramList->first;
			return 0;
		}
		return 2;
	}
	return 1;
}

int symTableParamListGetSize(SymTableParamList *paramList)
{
	if(paramList != NULL){
		return paramList->size;
	}
	return -1;
}

int symTableParamListFree(SymTableParamList *paramList)
{
	if(paramList != NULL){
		SymTableParamListElementPtr first = paramList->first;
		SymTableParamListElementPtr destroy;
		while(first != NULL){
			destroy = first;
			first = first->rightPtr;
			free(destroy);
		}
		paramList->size = 0;
		paramList->first = NULL;
		paramList->active = NULL;
		paramList->last = NULL;
		return 0;
	}
	return 1;
}


/****************************************************** TEST PRINT TREE ******************************************************************************/

void Print_tree2(SymTableBinTreePtr TempTree, char* sufix, char fromdir, char* space_size)
{
	if (TempTree != NULL)
	{
		char* suf2 = (char*) malloc(strlen(sufix) + 4);
		strcpy(suf2, sufix);
		suf2 = strcat(suf2, space_size);
		if (fromdir == 'L') {
			suf2 = strcat(suf2, "  |");
			printf("%s\n", suf2);
		} else {
			suf2 = strcat(suf2, "   ");
		}
		Print_tree2(TempTree->rightPtr, suf2, 'R', space_size);
		printf("%s%s  +-[%s]\n", sufix, space_size, TempTree->key);
		strcpy(suf2, sufix);
		suf2 = strcat(suf2, space_size);
		if (fromdir == 'R') {
			suf2 = strcat(suf2, "  |");
		} else {
			suf2 = strcat(suf2, "   ");
		}
		Print_tree2(TempTree->leftPtr, suf2, 'L', space_size);
		if (fromdir == 'R') {
			printf("%s\n", suf2);
		}
		free(suf2);
	}
}

void Print_tree(SymTableBinTreePtr TempTree)
{
	printf("Struktura binarniho stromu:\n");
	printf("\n");
	if (TempTree != NULL) {
		Print_tree2(TempTree, "", 'X', "   ");
	} else {
		printf("strom je prazdny\n");
	}
	printf("\n");
	printf("=================================================\n");
} 
