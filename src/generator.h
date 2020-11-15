// hlavickovy soubor generatoru

#ifndef _GENERATOR_H
#define _GENERATOR_H

#include "common.h"
#include "scanner.h"



int generatorGenerateCode(TokenList *tokenList, ErrorHandle *errorHandle);


#endif //defined _GENERATOR_H