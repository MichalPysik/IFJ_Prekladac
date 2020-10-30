// hlavni soubor scanneru

#include "scanner.h"

// Stavy konecneho automatu scanneru
typedef enum
{
	SSTATE_START,
	SSTATE_ID,
	SSTATE_NUM,
	SSTATE_NUM_REAL,
	SSTATE_NUM_DOT,
	SSTATE_NUM_EXPONENT,
	SSTATE_NUM_EXPONENT_SIGN,
	SSTATE_NUM_EXPONENT_AFTER,
	SSTATE_NUM_ZERO,
	SSTATE_STRING,
	SSTATE_STRING_BACKSLASH,
	SSTATE_STRING_HEX1,
	SSTATE_STRING_HEX2,
	SSTATE_LESS,
	SSTATE_MORE,
	SSTATE_EQUAL,
	SSTATE_INITVAR,
	SSTATE_SLASH,
	SSTATE_COMMENT,
	SSTATE_BLOCK_COMMENT,
	SSTATE_BLOCK_COMMENT_END,

} SFSM_STATE; //Vsechny stavy scanner FSM


// test zda je token keyword, kdyz ano zmeni jeho typ na konkretni keyword
bool keywordCheck (Token *currentToken)
{
	if (currentToken->type != TOKEN_ID) return false;

	if (!strcmp(currentToken->attribute.string, "else"))
	{
		currentToken->type = TOKEN_KEYWORD_ELSE;
	}
	else if (!strcmp(currentToken->attribute.string, "float64"))
	{
		currentToken->type = TOKEN_KEYWORD_FLOAT64;
	}
	else if (!strcmp(currentToken->attribute.string, "for"))
	{
		currentToken->type = TOKEN_KEYWORD_FOR;
	}
	else if (!strcmp(currentToken->attribute.string, "func"))
	{
		currentToken->type = TOKEN_KEYWORD_FUNC;
	}
	else if (!strcmp(currentToken->attribute.string, "if"))
	{
		currentToken->type = TOKEN_KEYWORD_IF;
	}
	else if (!strcmp(currentToken->attribute.string, "int"))
	{
		currentToken->type = TOKEN_KEYWORD_INT;
	}
	else if (!strcmp(currentToken->attribute.string, "return"))
	{
		currentToken->type = TOKEN_KEYWORD_RETURN;
	}
	else if (!strcmp(currentToken->attribute.string, "string"))
	{
		currentToken->type = TOKEN_KEYWORD_STRING;
	}
	else if (!strcmp(currentToken->attribute.string, "package"))
	{
		currentToken->type = TOKEN_KEYWORD_PACKAGE;
	}
	else if (!strcmp(currentToken->attribute.string, "main"))
	{
		currentToken->type = TOKEN_KEYWORD_MAIN;
	}
	else return false;

	return true; // vypada to neintuitivne ale true se vrati kdyz je to keyword
} // konec funkce keyword check


// funkce co nacte novy Token do adresy currentToken
// vraci 0 pri kdyz nacte platny token, 1 kdyz narazi na syntaktickou chybu
int scannerGetToken (Token *currentToken)
{
	currentToken->type = TOKEN_EMPTY;
	SFSM_STATE state = SSTATE_START;

	char currChar;

	int i = 0; //pocitadlo string attributu
	char hexString[2] = {'0', '0'}; //hex cislo uvnitr stringu (napriklad "\x2F")

	while(1)
	{
		currChar = getc(FILE_INPUT);

		switch(state)
		{
			case(SSTATE_START):
				if (currChar == '\n')
				{
					currentToken->type = TOKEN_EOL;
					return ALL_OK;
				}
				else if (isspace(currChar))
				{
					state = SSTATE_START;
				}
				else if (isalpha(currChar) || currChar == '_')
				{
					currentToken->attribute.string[i] = currChar;
					i++;
					state = SSTATE_ID;
				}
				else if (isdigit(currChar) && currChar != '0')
				{
					currentToken->attribute.string[i] = currChar;
					i++;
					state = SSTATE_NUM;
				}
				else if (currChar == '0')
				{
					currentToken->attribute.string[i] = currChar;
					i++;
					state = SSTATE_NUM_ZERO;
				}
				else if (currChar == '\"')
				{
					state = SSTATE_STRING;
				}
				else if (currChar == ',')
				{
					currentToken->type = TOKEN_COMMA;
					return ALL_OK;
				}
				else if (currChar == ';')
				{
					currentToken->type = TOKEN_SEMICOLON;
					return ALL_OK;
				}
				else if (currChar == '/')
				{
					state = SSTATE_SLASH;
				}
				else if (currChar == '(')
				{
					currentToken->type = TOKEN_LROUNDBRACKET;
					return ALL_OK;
				}
				else if (currChar == ')')
				{
					currentToken->type = TOKEN_RROUNDBRACKET;
					return ALL_OK;
				}
				else if (currChar == '{')
				{
					currentToken->type = TOKEN_LCURLYBRACKET;
					return ALL_OK;
				}
				else if (currChar == '}')
				{
					currentToken->type = TOKEN_RCURLYBRACKET;
					return ALL_OK;
				}
				else if (currChar == '+')
				{
					currentToken->type = TOKEN_ADD;
					return ALL_OK;
				}
				else if (currChar == '-')
				{
					currentToken->type = TOKEN_SUB;
					return ALL_OK;
				}
				else if (currChar == '*')
				{
					currentToken->type = TOKEN_MUL;
					return ALL_OK;
				}
				else if (currChar == '=')
				{
					state = SSTATE_EQUAL;
				}
				else if (currChar == '<')
				{
					state = SSTATE_LESS;
				}
				else if (currChar == '>')
				{
					state = SSTATE_MORE;
				}
				else if (currChar == ':')
				{
					state = SSTATE_INITVAR;
				}
				else if (currChar == EOF)
				{
					currentToken->type = TOKEN_EOF;
					return ALL_OK;
				}
				else
				{
					fprintf(FILE_ERROR, "Lexical analysis error: Invalid token\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_ID):
				if (isalnum(currChar) || currChar == '_')
				{
					currentToken->attribute.string[i] = currChar;
					i++;
				}
				else
				{
					ungetc(currChar, inputFile);
					currentToken->attribute.string[i] = '\0';
					currentToken->type = TOKEN_ID;
					keywordCheck(currentToken);
					return ALL_OK;
				}
				break;


			case(SSTATE_NUM):
				if (isdigit(currChar))
				{
					currentToken->attribute.string[i] = currChar;
					i++;
				}
				else if (currChar == '.')
				{
					currentToken->attribute.string[i] = currChar;
					i++;
					state = SSTATE_NUM_DOT;
				}
				else if (currChar == 'e' || currChar == 'E')
				{
					currentToken->attribute.string[i] = currChar;
					i++;
					state = SSTATE_NUM_EXPONENT;
				}
        			else
        			{
        				ungetc(currChar, inputFile);
        				currentToken->attribute.string[i] = '\0';
        				currentToken->attribute.integer = (atoll(currentToken->attribute.string));
        				currentToken->type = TOKEN_INTVALUE;
        				return ALL_OK;
        			}
        			break;


			case(SSTATE_NUM_DOT):
				if (isdigit(currChar))
				{
        				currentToken->attribute.string[i] = currChar;
					i++;
					state = SSTATE_NUM_REAL;
				}
				else
				{
					ungetc(currChar, inputFile);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: The dot must be followed by atlest one digit for a valid float value\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_NUM_REAL):
				if (isdigit(currChar))
				{
					currentToken->attribute.string[i] = currChar;
					i++;
				}
				else if (currChar == 'e' || currChar == 'E')
				{
					currentToken->attribute.string[i] = 'e';
					i++;
					state = SSTATE_NUM_EXPONENT;
				}
				else
				{
					ungetc(currChar, inputFile);
					currentToken->attribute.string[i] = '\0';
					currentToken->attribute.real = (atof(currentToken->attribute.string));
					currentToken->type = TOKEN_FLOATVALUE;
					return ALL_OK;
				}
				break;


			case(SSTATE_NUM_ZERO):
				if (currChar == '.')
				{
					currentToken->attribute.string[i] = currChar;
					i++;
					state = SSTATE_NUM_DOT;
				}
				else if (isdigit(currChar))
				{
					ungetc(currChar, inputFile);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Whole decimal number cannot start with a zero\n");
					return LEX_ERROR;
				}
				else
				{
					ungetc(currChar, inputFile);
        				currentToken->attribute.integer = 0;
        				currentToken->type = TOKEN_INTVALUE;
        				return ALL_OK;
				}
        			break;


			case(SSTATE_NUM_EXPONENT):
				if (currChar == '+' || currChar == '-')
				{
					currentToken->attribute.string[i] = currChar;
					i++;
					state = SSTATE_NUM_EXPONENT_SIGN;
				}
				else if (isdigit(currChar))
				{
					currentToken->attribute.string[i] = currChar;
					i++;
					state = SSTATE_NUM_EXPONENT_AFTER;
				}
				else
				{
					ungetc(currChar, inputFile);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Exponent must be followed by at least one digit\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_NUM_EXPONENT_SIGN):
				if (isdigit(currChar))
				{
					currentToken->attribute.string[i] = currChar;
					i++;
					state = SSTATE_NUM_EXPONENT_AFTER;
				}
				else
				{
					ungetc(currChar, inputFile);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Exponent sign must be followed by at least one digit\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_NUM_EXPONENT_AFTER):
				if (isdigit(currChar))
				{
					currentToken->attribute.string[i] = currChar;
					i++;
				}
				else
				{
					ungetc(currChar, inputFile);
					currentToken->attribute.string[i] = '\0';
					currentToken->attribute.real = (atof(currentToken->attribute.string));
					currentToken->type = TOKEN_FLOATVALUE;
					return ALL_OK;
				}
				break;


			case(SSTATE_STRING):
				if (currChar == '\"')
				{
					currentToken->attribute.string[i] = '\0';
					currentToken->type = TOKEN_STRINGVALUE;
					return ALL_OK;
				}
				else if (currChar == '\\')
				{
					state = SSTATE_STRING_BACKSLASH;
				}
				else if (currChar == '\n' || currChar == EOF)
				{
					ungetc(currChar, inputFile);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: String must be written on a single line and cannot reach End Of File\n");
					return LEX_ERROR;
				}
				else if (currChar > 31 && currChar != 34)
				{
					currentToken->attribute.string[i] = currChar;
					i++;
				}
				else
				{
					ungetc(currChar, inputFile);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Invalid character or character sequence inside string\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_STRING_BACKSLASH):
				if (currChar == '\"')
				{
					currentToken->attribute.string[i] = '\"';
					i++;
					state = SSTATE_STRING;
				}
				else if (currChar == 'n')
				{
					currentToken->attribute.string[i] = '\n';
					i++;
					state = SSTATE_STRING;
				}
				else if (currChar == 't')
				{
					currentToken->attribute.string[i] = '\t';
					i++;
					state = SSTATE_STRING;
				}
				else if (currChar == '\\')
				{
					currentToken->attribute.string[i] = '\\';
					i++;
					state = SSTATE_STRING;
				}
				else if (currChar == 'x')
				{
					state = SSTATE_STRING_HEX1;
				}
				else
				{
					ungetc(currChar, inputFile);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Invalid escape sequence inside string\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_STRING_HEX1):
			{
				if (isxdigit(currChar))
				{
					hexString[0] = tolower(currChar);
					state = SSTATE_STRING_HEX2;
				}
				else
				{
					ungetc(currChar, inputFile);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Invalid hexadecimal sequence inside string\n");
					return LEX_ERROR;
				}
			}
			break;


			case(SSTATE_STRING_HEX2):
			{
				if (isxdigit(currChar))
				{
					hexString[1] = tolower(currChar);
					unsigned int x;
					sscanf(hexString, "%x", &x);
					currChar = x & 0xFF;
					currentToken->attribute.string[i] = currChar;
					i++;
					state = SSTATE_STRING;
				}
				else
				{
					ungetc(currChar, inputFile);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Invalid hexadecimal sequence inside string\n");
					return LEX_ERROR;
				}
			}
			break;


			case(SSTATE_SLASH):
				if (currChar == '/')
				{
					state = SSTATE_COMMENT;
				}
				else if (currChar == '*')
				{
					state = SSTATE_BLOCK_COMMENT;
				}
				else
				{
					ungetc(currChar, inputFile);
					currentToken->type = TOKEN_DIV;
					return ALL_OK;
				}
				break;


			case(SSTATE_EQUAL):
				if (currChar == '=')
				{
					currentToken->type = TOKEN_EQ;
					return ALL_OK;
				}
				else
				{
					currentToken->type = TOKEN_ASSIGN;
					return ALL_OK;
				}
				break;


			case(SSTATE_INITVAR):
				if (currChar == '=')
				{
					currentToken->type = TOKEN_INIT;
					return ALL_OK;
				}
				else
				{
					ungetc(currChar, inputFile);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Character \":\" can only be used as variable initialization \":=\"\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_LESS):
				if (currChar == '=')
				{
					currentToken->type = TOKEN_LTE;
					return ALL_OK;
				}
				else
				{
					ungetc(currChar, inputFile);
					currentToken->type = TOKEN_LT;
					return ALL_OK;
				}
				break;


			case(SSTATE_MORE):
				if (currChar == '=')
				{
					currentToken->type = TOKEN_GTE;
					return ALL_OK;
				}
				else
				{
					ungetc(currChar, inputFile);
					currentToken->type = TOKEN_GT;
					return ALL_OK;
				}
				break;


			case(SSTATE_COMMENT):
				if (currChar == '\n')
				{
					currentToken->type = TOKEN_EOL;
					return ALL_OK;
				}
				else if (currChar == EOF)
				{
					currentToken->type = TOKEN_EOF;
					return ALL_OK;
				}
				break;


			case(SSTATE_BLOCK_COMMENT):
				if (currChar == '*')
				{
					state = SSTATE_BLOCK_COMMENT_END;
				}
				else if (currChar == EOF)
				{
					currentToken->type = TOKEN_EOF;
					return ALL_OK;
				}
				break;


			case(SSTATE_BLOCK_COMMENT_END):
				if(currChar == '/')
				{
					state = SSTATE_START;
				}
				else if (currChar == EOF)
				{
					currentToken->type = TOKEN_EOF;
					return ALL_OK;
				}
				else if (currChar != '*')
				{
					state = SSTATE_BLOCK_COMMENT;
				}
				break;


			default:
				fprintf(FILE_ERROR, "Internal error: Scanner reached undefined state\n");
				return INTERNAL_ERROR;

		} //konec switche stavu

	} //konec while(1)


	return ALL_OK;
} //konec funkce getToken
