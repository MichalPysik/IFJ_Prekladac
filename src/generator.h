// hlavickovy soubor generatoru

#ifndef _GENERATOR_H
#define _GENERATOR_H

#include "common.h"
#include "scanner.h"
#include "parser.h"

#define FUNC_INPUTI "# --- func inputi ------------------------------\n\
LABEL inputi\n\n\
CREATEFRAME\n\
DEFVAR TF@$1\n\
READ TF@$1 int\n\n\
JUMPIFEQ $inputi_err TF@$1 nil@nil\n\
PUSHS TF@$1\n\
PUSHS int@0\n\
RETURN\n\
LABEL $inputi_err\n\
PUSHS int@0\n\
PUSHS int@1\n\
RETURN\n"

#define FUNC_INPUTF "# --- func inputf ------------------------------\n\
LABEL inputf\n\n\
CREATEFRAME\n\
DEFVAR TF@$1\n\
READ TF@$1 float\n\n\
JUMPIFEQ $inputf_err TF@$1 nil@nil\n\
PUSHS TF@$1\n\
PUSHS int@0\n\
RETURN\n\
LABEL $inputf_err\n\
PUSHS int@0\n\
PUSHS int@1\n\
RETURN\n"

#define FUNC_INPUTS "# --- func inputs ------------------------------\n\
LABEL inputs\n\n\
CREATEFRAME\n\
DEFVAR TF@$1\n\
READ TF@$1 string\n\n\
JUMPIFEQ $inputs_err TF@$1 nil@nil\n\
PUSHS TF@$1\n\
PUSHS int@0\n\
RETURN\n\
LABEL $inputs_err\n\
PUSHS int@0\n\
PUSHS int@1\n\
RETURN\n"

#define FUNC_INT_TO_FLOAT "# --- func int2float ------------------------------\n\
LABEL int2float\n\n\
CREATEFRAME\n\
DEFVAR TF@$1\n\
DEFVAR TF@$2\n\
POPS TF@$1\n\n\
INT2FLOAT TF@$2 TF@$1\n\n\
PUSHS TF@$2\n\
RETURN\n"

#define FUNC_FLOAT_TO_INT "# --- func float2int ------------------------------\n\
LABEL float2int\n\n\
CREATEFRAME\n\
DEFVAR TF@$1\n\
DEFVAR TF@$2\n\
POPS TF@$1\n\n\
FLOAT2INT TF@$2 TF@$1\n\n\
PUSHS TF@$2\n\
RETURN\n"

#define FUNC_LEN "# --- func len ------------------------------\n\
LABEL len\n\n\
CREATEFRAME\n\n\
DEFVAR TF@s\n\
POPS TF@s\n\n\
DEFVAR TF@$1\n\n\
STRLEN TF@$1 TF@s\n\n\
PUSHS TF@$1\n\n\
RETURN\n"

#define FUNC_SUBSTRING "# --- func substr ------------------------------\n\
LABEL substr\n\n\
CREATEFRAME\n\
DEFVAR TF@$s\n\
DEFVAR TF@$i\n\
DEFVAR TF@$n\n\n\
DEFVAR TF@$substring\n\
MOVE TF@$substring string@\n\
POPS TF@$s\n\
POPS TF@$i\n\
POPS TF@$n\n\
RETURN\n"

#define FUNC_ORD "# --- func ord ------------------------------\n\
LABEL ord\n\n\
CREATEFRAM\n\
DEFVAR LF@$return\n\
TYPE GF@$tmp LF@%%2\n\
JUMPIFEQ ord$cont GF@$tmp string@int\n\
EXIT int@4\n\
LABEL ord$cont\n\
DEFVAR LF@len\n\
MOVE LF@$return nil@nil\n\
CREATEFRAME\n\
DEFVAR TF@%%1\n\
MOVE TF@%%1 LF@%%1\n\
CALL length\n\
MOVE LF@len TF@$return\n\
CLEARS\n\
PUSHS LF@%%2\n\
PUSHS int@0\n\
LTS\n\
PUSHS LF@%%2\n\
PUSHS LF@len\n\
LTS\n\
NOTS\n\
ORS\n\
POPS TF@$return\n\
JUMPIFEQ ord$nil TF@$return bool@true\n\
STRI2INT LF@$return LF@%%1 LF@%%2\n\
LABEL ord$nil\n\
POPFRAME\n\
RETURN\n"

#define FUNC_CHR "# --- func chr ------------------------------\n\
LABEL chr\n\n\
CREATEFRAME\n\n\
DEFVAR TF@$0\n\
DEFVAR TF@$1\n\
DEFVAR TF@$2\n\
DEFVAR TF@$compareResult\n\n\
POPS TF@$1\n\
LT TF@$compareResult TF@$1 int@0\n\
JUMPIFEQ $chr_error TF@$compareResult bool@true\n\
GT TF@$compareResult TF@$1 int@255\n\
JUMPIFEQ $chr_error TF@$compareResult bool@true\n\n\
INT2CHAR TF@$2 TF@$1\n\
PUSHS TF@$2\n\
PUSHS int@0\n\
RETURN\n\n\
LABEL $chr_error\n\
PUSHS string@/000\n\
PUSHS int@1\n\
RETURN\n"

#define FUNC_GTE "# --- func GTE ------------------------------\n\
LABEL $greaterOrEqual\n\n\
DEFVAR TF@$op1\n\
DEFVAR TF@$op2\n\
DEFVAR TF@$res\n\
DEFVAR TF@$res1\n\
DEFVAR TF@$res2\n\n\
POPS TF@$op2\n\
POPS TF@$op1\n\n\n\
GT TF@$res1 TF@$op1 TF@$op2\n\
EQ TF@$res2 TF@$op1 TF@$op2\n\
OR TF@$res TF@$res1 TF@$res2\n\
PUSHS TF@$res\n\
RETURN\n"

#define FUNC_LTE "# --- func LTE ------------------------------\n\
LABEL $lessOrEqual\n\n\
DEFVAR TF@$op1\n\
DEFVAR TF@$op2\n\
DEFVAR TF@$res\n\
DEFVAR TF@$res1\n\
DEFVAR TF@$res2\n\n\
POPS TF@$op2\n\
POPS TF@$op1\n\n\n\
LT TF@$res1 TF@$op1 TF@$op2\n\
EQ TF@$res2 TF@$op1 TF@$op2\n\
OR TF@$res TF@$res1 TF@$res2\n\
PUSHS TF@$res\n\
RETURN\n"

void pushArguments(ParserStackPtr *argStack, int argCount);

void printPushs(ParserStackPtr *varStack, int argCount);

//Tiskne pop návratových hodnot funkcí mimo main
void printPops(ParserStackPtr *varStack, int varCount);

//Tiskne řetězcovou hodnotu podle zadání
void printString(char* string, bool printNewLine);

//Generování kódu
int generatorGenerateCode(TokenList *tokenList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ParserStackPtr *leftAndRightAnalysisStack, ParserStackPtr *semanticRuleStack, ErrorHandle *errorHandle);


typedef struct integerStack {
	int ID;
	int bracketCount;
	int keywordType;
	struct integerStack *next;
} *integerStack;

#define IF 0
#define IF_EXPRESSION 1
#define ELSE 2
#define FOR 3
#define FOR_INIT 4
#define FOR_EXPRESSION 5
#define FOR_ASSIGNMENT 6

#endif //defined _GENERATOR_H