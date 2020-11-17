// hlavickovy soubor parseru

#ifndef _PARSER_H
#define _PARSER_H

#include "common.h"
#include "scanner.h"
#include "symtable.h"


#define STACK_INT_TO_DATA(x) (ParserStackData)((char)x)
#define STACK_TERM_TO_DATA(x) (ParserStackData)((Term_type)x)
#define STACK_SYMTABLE_TO_DATA(x) (ParserStackData)((SymTableBinTreePtr *)x)

#define STACK_DATA_TO_INT(x) ((char)x.result)
#define STACK_DATA_TO_TERM(x) ((Term_type)x.TERM_NONTERM)
#define STACK_DATA_TO_SYMTABLE(x) ((SymTableBinTreePtr *)x.SYM_TABLE)

typedef union {
	char result;
	Term_type TERM_NONTERM;
	SymTableBinTreePtr *SYM_TABLE;
} ParserStackData;

typedef struct parserStackNode {
	ParserStackData data;
	struct parserStackNode *next;
} *ParserStackPtr;



int parserAnalyze(TokenList *tokenList, ErrorHandle *errorHandle);


/****************************************************** PARSER SYMTABLE PRERUN ******************************************************************************/

int parserPreRun(TokenList *tokenList, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);

int parserSymTableInitBuiltIn(SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);


/****************************************************** PARSER RUN ANALYZE ******************************************************************************/

int parserRunAnalyze(TokenList *tokenList, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);


/****************************************************** SYNTAX STACK ******************************************************************************/

void parserStackInit(ParserStackPtr *stack);

ParserStackData parserStackPush(ParserStackPtr *stack, ParserStackData data);

ParserStackData parserStackPeek(ParserStackPtr *stack);

ParserStackData parserStackPop(ParserStackPtr *stack);

void parserStackFree(ParserStackPtr *stack);


#endif //defined _PARSER_H
