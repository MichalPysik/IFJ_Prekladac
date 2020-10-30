// common.c

#include "common.h"


int errorHandle(int errorID)
{
	if(errorID == LEX_ERROR){
		fprintf(FILE_ERROR, "FATAL ERROR: Lexical Error!\n");
	} else if(errorID == SYNTAX_ERROR){
		fprintf(FILE_ERROR, "FATAL ERROR: Syntax Error!\n");
	} else if(errorID == SEM_UNDEFINED_VAR_ERROR){
		fprintf(FILE_ERROR, "FATAL ERROR: Semantic Error - undefined variable!\n");
	} else if(errorID == SEM_DATA_TYPE_ERROR){
		fprintf(FILE_ERROR, "FATAL ERROR: Semantic Error - wrong data type!\n");
	} else if(errorID == SEM_TYPE_COMPATIBILITY_ERROR){
		fprintf(FILE_ERROR, "FATAL ERROR: Semantic Error - type compatibility!\n");
	} else if(errorID == SEM_FUNC_PARAM_RETURN_ERROR){
		fprintf(FILE_ERROR, "FATAL ERROR: Semantic Error - wrong return params!\n");
	} else if(errorID == SEM_OTHER_ERROR){
		fprintf(FILE_ERROR, "FATAL ERROR: Semantic Error - other!\n");
	} else if(errorID == ZERO_DIVISION_ERROR){
		fprintf(FILE_ERROR, "FATAL ERROR: Zero Division!\n");
	} else if(errorID == INTERNAL_ERROR){
		fprintf(FILE_ERROR, "FATAL ERROR: Internal Error!\n");
	}
	return errorID;
}