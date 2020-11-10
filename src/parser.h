// hlavickovy soubor parseru

#ifndef _PARSER_H
#define _PARSER_H

#include "common.h"
#include "scanner.h"
#include "symtable.h"


int parserAnalyze(TokenList *tokenList);


/****************************************************** PARSER SYMTABLE PRERUN ******************************************************************************/

int parserPreRun(TokenList *tokenList, SymTableBinTreePtr *globalSymTable);

int parserSymTableInitBuiltIn(SymTableBinTreePtr *globalSymTable);


/****************************************************** PARSER RUN ANALYZE ******************************************************************************/

int parserRunAnalyze(TokenList *tokenList, SymTableBinTreePtr *globalSymTable);


#endif //defined _PARSER_H