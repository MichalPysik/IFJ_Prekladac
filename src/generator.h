// hlavickovy soubor generatoru

#ifndef _GENERATOR_H
#define _GENERATOR_H

#include "common.h"
#include "scanner.h"
#include "parser.h"



//int generatorGenerateCode(int ruleNumber, TokenList *tokenList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);
int generatorGenerateCode(TokenList *tokenList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);


#endif //defined _GENERATOR_H