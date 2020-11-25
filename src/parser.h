// hlavickovy soubor parseru

#ifndef _PARSER_H
#define _PARSER_H

#include "common.h"
#include "scanner.h"
#include "symtable.h"


#define STACK_INT_TO_DATA(x) (ParserStackData)((int)x)
#define STACK_TERM_TO_DATA(x) (ParserStackData)((Term_type)x)
#define STACK_SYMTABLE_TO_DATA(x) (ParserStackData)((SymTableBinTreePtr)x)
#define STACK_TOKEN_TO_DATA(x) (ParserStackData)((Token)x)

#define STACK_DATA_TO_INT(x) ((int)x.result)
#define STACK_DATA_TO_TERM(x) ((Term_type)x.TERM_NONTERM)
#define STACK_DATA_TO_SYMTABLE(x) ((SymTableBinTreePtr)x.SYM_TABLE)
#define STACK_DATA_TO_TOKEN(x) ((Token)x.token)



typedef union {
	int result;
	Term_type TERM_NONTERM;
	SymTableBinTreePtr SYM_TABLE;
	Token token;
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

int parserRunPredictiveSyntaxAnalysis(TokenList *tokenList, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);

int parserRunPrecedentSyntaxAnalysis(TokenList *expressionList, ParserStackPtr *semanticStack, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);

int parserSemanticAnalysis(TokenList *tokenList, ParserStackPtr *semanticStack, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);

int parserSemanticExpressionCheckOperatorsAndOperands(ParserStackPtr *semanticStack);

int parserSemanticChangeIDsToTypes(ParserStackPtr *semanticStack);

int parserTokenListFree(TokenList *tokenList);

int parserLeftAnalysis(int ruleNumber);

int parserRightAnalysis(int ruleNumber);


/****************************************************** PRECEDENT SYNTAX STACK ******************************************************************************/

ParserStackData parserStackPrecedentTop(ParserStackPtr *stack);

int parserStackPrecedentTopAddHandle(ParserStackPtr *stack);

int parserStackPrecedentTopHasHandle(ParserStackPtr *stack);

int parserStackPrecedentTopPopAndPushRule(ParserStackPtr *stack);


/****************************************************** SYNTAX STACK ******************************************************************************/

void parserStackInit(ParserStackPtr *stack);

ParserStackData parserStackPush(ParserStackPtr *stack, ParserStackData data);

ParserStackData parserStackPeek(ParserStackPtr *stack);

ParserStackData parserStackPop(ParserStackPtr *stack);

void parserStackFree(ParserStackPtr *stack);


#endif //defined _PARSER_H
