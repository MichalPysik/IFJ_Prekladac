// hlavickovy soubor parseru

#ifndef _PARSER_H
#define _PARSER_H

#include "common.h"
#include "scanner.h"
#include "generator.h"



int parserAnalyze(TokenList *tokenList, ErrorHandle *errorHandle);


/****************************************************** PARSER SYMTABLE PRERUN ******************************************************************************/

int parserPreRun(TokenList *tokenList, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);

int parserSymTableInitBuiltIn(SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);


/****************************************************** PARSER RUN ANALYZE ******************************************************************************/

int parserRunPredictiveSyntaxAnalysis(TokenList *tokenList, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);

int parserRunPrecedentSyntaxAnalysis(TokenList *expressionList, ParserStackPtr *semanticStack, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);

int parserSemanticAnalysis(TokenList *tokenList, ParserStackPtr *semanticStack, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);

int parserSemanticChangeIDsToTypes(ParserStackPtr *semanticStack, ParserStackPtr *symtableStack, ErrorHandle *errorHandle);

Token_type parserSemanticExpressionCheckOperatorsAndOperands(ParserStackPtr *semanticStack, ErrorHandle *errorHandle);

IDdataType parserSemanticTokenTypeToVarType(Token_type tokenType);

Token parserSemanticVarTypeToToken(IDdataType varDataType);

int parserTokenListFree(TokenList *tokenList);

int parserLeftAnalysis(int ruleNumber, TokenList *tokenList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);

int parserRightAnalysis(int ruleNumber, TokenList *expressionList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);


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
