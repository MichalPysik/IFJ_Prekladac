// hlavickovy soubor parseru

#ifndef _PARSER_H
#define _PARSER_H

#include "common.h"
#include "scanner.h"
#include "symtable.h"



int parserAnalyze(TokenList *tokenList, ErrorHandle *errorHandle);


/****************************************************** PARSER SYMTABLE PRERUN ******************************************************************************/

int parserPreRun(TokenList *tokenList, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);

int parserSymTableInitBuiltIn(SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);


/****************************************************** PARSER RUN ANALYZE ******************************************************************************/

int parserRunAnalyze(TokenList *tokenList, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);


#endif //defined _PARSER_H
