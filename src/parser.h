// hlavickovy soubor parseru

#ifndef _PARSER_H
#define _PARSER_H

#include "common.h"



int parserAnalyze(TokenList tokenList);


int parserPreRun(TokenList tokenList, SymTableBinTreePtr globalSymTable);

int parserRunAnalyze(TokenList tokenList, symTable globalSymTable);


#endif //defined _PARSER_H