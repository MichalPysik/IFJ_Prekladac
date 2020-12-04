// hlavickovy soubor generatoru

#ifndef _GENERATOR_H
#define _GENERATOR_H

#include "common.h"
#include "scanner.h"
#include "parser.h"

#define FUNC_INPUTI "# --- func inputi ------------------------------\nLABEL inputi\nCREATEFRAME\nDEFVAR TF@$1\nREAD TF@$1 int\n\nJUMPIFEQ $inputi_err TF@$1 nil@nil\nPUSHS int@0\nPUSHS TF@$1\nRETURN\nLABEL $inputi_err\nPUSHS int@1\nPUSHS int@0\nRETURN\n"
#define FUNC_INPUTF "# --- func inputf ------------------------------\nLABEL inputf\nCREATEFRAME\nDEFVAR TF@$1\nREAD TF@$1 float\n\nJUMPIFEQ $inputf_err TF@$1 nil@nil\nPUSHS int@0\nPUSHS TF@$1\nRETURN\nLABEL $inputf_err\nPUSHS int@1\nPUSHS int@0\nRETURN\n"
#define FUNC_INPUTS "# --- func inputs ------------------------------\nLABEL inputs\nCREATEFRAME\nDEFVAR TF@$1\nREAD TF@$1 string\n\nJUMPIFEQ $inputs_err TF@$1 nil@nil\nPUSHS int@0\nPUSHS TF@$1\nRETURN\nLABEL $inputs_err\nPUSHS int@1\nPUSHS int@0\nRETURN\n"
#define FUNC_INT_TO_FLOAT "# --- func int2float ------------------------------\nLABEL int2float\n\nCREATEFRAME\nDEFVAR TF@$1\nDEFVAR TF@$2\nPOPS TF@$1\n\nINT2FLOAT TF@$2 TF@$1\n\nPUSHS TF@$2\nRETURN\n"
#define FUNC_FLOAT_TO_INT "# --- func float2int ------------------------------\nLABEL float2int\n\nCREATEFRAME\nDEFVAR TF@$1\nDEFVAR TF@$2\nPOPS TF@$1\n\nFLOAT2INT TF@$2 TF@$1\n\nPUSHS TF@$2\nRETURN\n"
#define FUNC_STRLEN "# --- func strlen ------------------------------\nLABEL len\nCREATEFRAME\n\nDEFVAR TF@s\nPOPS TF@s\n\nDEFVAR TF@$1\n\nSTRLEN TF@$1 TF@s\n\nPUSHS TF@$1\n\nRETURN\n"
#define FUNC_SUBSTRING "# --- func substr ------------------------------\n"
#define FUNC_ORD "# --- func ord ------------------------------\n"
#define FUNC_CHR "# --- func chr ------------------------------\nLABEL chr\nCREATEFRAME\n\nDEFVAR TF@$0\nDEFVAR TF@$1\nDEFVAR TF@$2\n\nPOPS TF@$1\nLT TF@$compareResult TF@$1 int$0\nJUMPIFEQ $chr_error TF@$compareResult bool@true\nGT TF@$compareResult TF@$1 int$255\nJUMPIFEQ $chr_error TF@$compareResult bool@true\n\nINT2CHAR TF@$2\nPUSHS int@0\nPUSHS string@$2\nRETURN\n\nLABEL $chr_error\nPUSHS int@1\nPUSHS string@/000\nRETURN\n"


void pushArguments(ParserStackPtr *argStack, int argCount);

void printPushs(ParserStackPtr *varStack, int argCount);

//Tiskne pop návratových hodnot funkcí mimo main
void printPops(ParserStackPtr *varStack, int varCount);

//Tiskne řetězcovou hodnotu podle zadání
void printString(char* string);

//Generování kódu
int generatorGenerateCode(TokenList *tokenList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ParserStackPtr *leftAndRightAnalysisStack, ParserStackPtr *semanticRuleStack, ErrorHandle *errorHandle);


#endif //defined _GENERATOR_H