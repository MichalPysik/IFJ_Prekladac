// hlavni soubor scanneru

#include "scanner.h"

// Stavy konecneho automatu scanneru
typedef enum {

	SSTATE_START,
	SSTATE_ID,
	SSTATE_NUM,
	SSTATE_NUM_REAL,
	SSTATE_NUM_DOT,
	SSTATE_NUM_EXPONENT,
	SSTATE_NUM_EXPONENT_SIGN,
  SSTATE_NUM_EXPONENT_AFTER,
  SSTATE_NUM_ZERO,
	SSTATE_LESS,
	SSTATE_MORE,
	SSTATE_EQUAL,
	SSTATE_SLASH,
	SSTATE_COMMENT,
	SSTATE_BLOCK_COMMENT,
	SSTATE_BLOCK_COMMENT_END,

} SFSM_STATE; //State of scanner FSM


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
int getToken (Token *currentToken)
{
  currentToken->type = TOKEN_EMPTY;
  SFSM_STATE state = SSTATE_START;

  char currChar;

  int i = 0;

  while(1)
  {
    currChar = getc(stdin);

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
        else if (currChar == ',')
        {
          currentToken->type = TOKEN_COMMA;
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
        else if (currChar == EOF)
        {
          currentToken->type = TOKEN_EOF;
          return ALL_OK;
        }
        else
        {
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
          ungetc(currChar, stdin);
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
        else
        {
          ungetc(currChar, stdin);
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
          ungetc(currChar, stdin);
          currentToken->type = TOKEN_EMPTY;
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
          ungetc(currChar, stdin);
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
        else
        {
          ungetc(currChar, stdin);
          currentToken->type = TOKEN_EMPTY;
          return LEX_ERROR;
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
          ungetc(currChar, stdin);
          currentToken->type = TOKEN_EMPTY;
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
          ungetc(currChar, stdin);
          currentToken->type = TOKEN_EMPTY;
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
        ungetc(currChar, stdin);
        currentToken->attribute.string[i] = '\0';
        currentToken->attribute.real = (atof(currentToken->attribute.string));
        currentToken->type = TOKEN_FLOATVALUE;
        return ALL_OK;
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
          ungetc(currChar, stdin);
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


      case(SSTATE_LESS):
        if (currChar == '=')
        {
          currentToken->type = TOKEN_LTE;
          return ALL_OK;
        }
        else
        {
          ungetc(currChar, stdin);
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
          ungetc(currChar, stdin);
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
        return INTERNAL_ERROR;

    } //konec switche stavu

  } //konec while(1)


  return ALL_OK;
} //konec funkce getToken





/*


// !!MAIN TU MAM JEN DOCASNE NA TESTOVANI!!
int main() {
  Token *testToken = malloc(sizeof(Token));

  testToken->type = TOKEN_EMPTY;

  int retval = 0;

  while(testToken->type != TOKEN_EOF)
  {
      retval = getToken(testToken);

      if (testToken->type == TOKEN_ID) printf("Token Type: %d is string: %s\n", testToken->type, testToken->attribute.string);
      else if (testToken->type == TOKEN_INTVALUE) printf("Token Type: %d is integer: %ld\n", testToken->type, testToken->attribute.integer);
      else if (testToken->type == TOKEN_FLOATVALUE) printf("Token Type: %d is float64: %f\n", testToken->type, testToken->attribute.real);
      else printf("Token Type: %d\n", testToken->type);

      if (retval) printf("nastala nejaka chyba, error code %d\n",retval);
    }


  free(testToken);

  return retval;
} */
