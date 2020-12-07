				Token operator; // OPERATOR
				operator.type = TOKEN_EMPTY;
				Token operand2; // OPERAND 2
				operand2.type = TOKEN_EMPTY;
				Token operand1; // OPERAND 1
				operand1.type = TOKEN_EMPTY;
				int tokenCount = 0;
				
				int operandFromBrackets = 0;
				
				ParserStackPtr top = (*semanticRuleStack);
				while(top != NULL && tokenCount < 3){ // načte max první tři terminály
					//printf("\n\n    EXPRESSION: %s\n", tokenTypes[STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type]);
					if(STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type != TOKEN_LROUNDBRACKET && STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type != TOKEN_RROUNDBRACKET){
						if(tokenCount == 0){
							operator = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
						} else if(operandFromBrackets == 0){
							if(tokenCount == 1){
								operand2 = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
								expressionType = operand2.type;
							} else if(tokenCount == 2){
								operand1 = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
								expressionType = operand1.type;
							}
						} else if(operandFromBrackets == 1){
							if(tokenCount == 1){
								operand1 = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
								expressionType = operand1.type;
							} else if(tokenCount == 2){
								operand2 = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
								expressionType = operand2.type;
							}
						}
						
						tokenCount++;
					} else if(STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type == TOKEN_LROUNDBRACKET){
						operandFromBrackets = 1;
					}

					if(operand1.type == TOKEN_STRINGVALUE || operand2.type == TOKEN_STRINGVALUE){
						isStringExpression = true;
					}

					top = top->next;
					parserStackPop(semanticRuleStack);
				}
				//printf("OPERAND 2 TOKEN: %s\n", tokenTypes[operand2.type]);
				//printf("OPERAND 1 TOKEN: %s\n", tokenTypes[operand1.type]);
				
				if(operand2.type == TOKEN_EMPTY){
					printf("POPS TF@$operand2\n");
				}
				if(operand1.type == TOKEN_EMPTY){
					printf("POPS TF@$operand1\n");
				}