// hlavni soubor scanneru

#include "scanner.h"


//funkce co nacte novy Token do adresy currentToken
int getToken (Token *currentToken)
{
  currentToken->type = TOKEN_EMPTY;
  int state = SSTATE_START;

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
        else if (isdigit(currChar))
        {
          currentToken->attribute.string[i] = currChar;
          i++;
          state = SSTATE_NUM;
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
    }

  }


  return ALL_OK;
}



// !!MAIN TU MAM JEN DOCASNE NA TESTOVANI!!
int main() {
  Token *testToken = malloc(sizeof(Token));

  testToken->type = TOKEN_EMPTY;

  int retval = 0;

while(testToken->type != TOKEN_EOF)
{
    retval = getToken(testToken);
    printf("Token Type: %d\n", testToken->type);
}



  free(testToken);

  return retval;
}
