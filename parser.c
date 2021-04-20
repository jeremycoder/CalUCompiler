#include "parser.h"

//totals syntax/parser errors
int SyntaxErrNum = 0;

//set to 1 when the current line/statement should be skipped
int ErrStateFlag = 0;

//For recursive calls, if no more statements are left in the production
//is 0 or more
int NoMoreStatements = -1;

char FatalError = 0;

FILE* OutputFilePtr;
FILE* ListFilePtr;

//
void compile(FILE* in, FILE* out, FILE* list, FILE* temp)
{
	OutputFilePtr = out;
	ListFilePtr = list;
	scannerInit(in, out, list);
	generatorInit(out, temp);
	systemGoal();
}

// Safely concatenates the characters in "TokenBuffer" 
void catTokenBuffer(char* out)
{
	strncat(out, getTokenBuffer(), MAX_EXPRESSION_SIZE - strlen(out));
}

//Reports errors
void reportError(char* expectedToken)
{
	//Display error
	//printf("\nError: Line %d, expected %s", lineNum, expectedToken);

	if (ErrStateFlag == 0)
	{
		char temp[10];

		itoa(getCurLineNum(), temp, 10);

		SyntaxErrNum++;
		addToErrorBuffer("Syntax error on line ");
		addToErrorBuffer(temp);
		addToErrorBuffer(" (Expected ");
		addToErrorBuffer(expectedToken);
		addToErrorBuffer("): Found \"");
		addToErrorBuffer(getTokenBuffer());
		addToErrorBuffer("\"\n");
	}
}

//Returns 1 if there was an error that prevented the parser from finishing the parse process
int getParserErrorState()
{
	return FatalError;
}

//Returns the current total of lexical errors found in the input file
int getTotalSynErrors()
{
	return SyntaxErrNum;
}

//Check for statements
void checkForStatement()
{
	int t = nextToken();

	//if the next token is not a statement
	if (!(t == ID || t == READ || t == WRITE || t == IF || t == WHILE))
	{
		// If the next token is not the beginning of a statement or the end of a statement then it must be a syntax error
		if (!(t == ENDWHILE || t == END || t == ENDIF || t == ELSE || t == SCANEOF))
		{
			reportError("statement");
			ErrStateFlag = 1;
			do
			{
				match(-1);
				t = nextToken();
				// Checks if the next token is potentially the start of a new statement
				if (t == ID || t == READ || t == WRITE || t == IF || t == WHILE || t == END)
				{
					ErrStateFlag = 0;
				}
			} while (ErrStateFlag == 1 && t != SCANEOF);
		}
		else
		{
			NoMoreStatements++; //raise flag meaning no more statements
		}
	}
}

// Gets the next token from the scanner and checks if it is equal to "token"
// Returns 1 if it is equal
// Returns 0 if it is not equal
int match(int token) //DESTRUCTIVE
{
	int returnVal;
	//printf("\nMatching %d with nextToken: %d\n", token, nextToken());
	if (token == scanner(1))
		returnVal = 1;
	else
		returnVal = 0;
	return returnVal;
}

// Looks better than calling "scanner(0)" everywhere
int nextToken()
{
	return scanner(0);
}

//Parser starts here
// 40. <system goal> -> <program> SCANEOF #genFinish
void systemGoal()
{
	program();
	if (match(SCANEOF) == 0) // Expected end of file
	{
		reportError("End of File");
		printf("\n\nA FATAL ERROR OCCURRED\n\n");
		FatalError = 1;
	}

	genFinish();

	char tempBuffer[50];

	sprintf(tempBuffer, "\n\nThere are %d lexical errors.", getTotalLexErrors()); // Resuses the token buffer temporarily because why not
	fputs(tempBuffer, ListFilePtr); // Puts the total number of errors at the end of the list file

	tempBuffer[0] = '\0';

	sprintf(tempBuffer, "\nThere are %d syntax errors.", getTotalSynErrors());
	fputs(tempBuffer, ListFilePtr);

	tempBuffer[0] = '\0';

	if (getParserErrorState() == 1)
	{
		sprintf(tempBuffer, "\nThe program did not finish compilation.");
		fputs(tempBuffer, ListFilePtr);
	}
	else
	{
		if (getTotalLexErrors() != 0 || getTotalSynErrors() != 0)
		{
			sprintf(tempBuffer, "\nThe program compiled with a total of %d errors.", (getTotalLexErrors() + getTotalSynErrors()));
			fputs(tempBuffer, ListFilePtr);
		}
		else
		{
			sprintf(tempBuffer, "\nThe program compiled with no errors.");
			fputs(tempBuffer, ListFilePtr);
		}
	}
}

// 41. <ident> -> ID #processID
struct ExprRecord ident()
{
	struct ExprRecord tempExpr;
	// Check for ID
	if (nextToken() == ID)
	{
		match(ID);
		tempExpr = processID(getTokenBuffer());
	}
	else
	{
		reportError("identifier");
	}
	return tempExpr;
}

// 1. <program> -> #genStart BEGIN <statement list> END
void program()
{
	genStart();

	//Check if token matches 'BEGIN'
	int t = nextToken();

	if (t == BEGIN)
	{
		match(BEGIN);
	}
	else //token did not match
	{
		//Display error
		reportError("\"BEGIN\"");
	}

	//Continue to next production
	statementList();

	//Check if token matches 'END'
	t = nextToken();

	if (t == END)
	{
		match(END);
	}
	else //token did not match
	{
		//Display error
		reportError("\"END\"");
	}
}

// 2. <statement_list> -> <statement> {<statement list>}
void statementList()
{
	do
	{
		statement();

	} while (NoMoreStatements == -1);
}

// 3. <statement> -> ID ASSIGNOP <expression> #processAssign;
// 4. <statement> -> WRITE LPAREN <idlist> RPAREN;
// 5. <statement> -> WRITE LPAREN <exprlist> RPAREN;
// 6. <statement> -> IF LPAREN <condition> RPAREN THEN <statementlist> <iftail>
// 9. <statement> -> WHILE LPAREN <condition> RPAREN {<statementlist>} ENDWHILE
void statement()
{
	NoMoreStatements = -1; //We have statements to process	

	//Get next token
	int t = nextToken();

	switch (t)
	{
	// 3. <statement> -> ID ASSIGNOP <expression> #processAssign;
	case ID:
	{
		struct ExprRecord leftExpr, rightExpr;
		leftExpr = ident();

		t = nextToken();

		//Check for ':='
		if (t == ASSIGNOP) //incorrect token
		{
			match(ASSIGNOP);
		}
		else
		{
			reportError("\":=\"");
		}

		//call expression production function
		expression(&rightExpr);

		processAssign(&leftExpr, &rightExpr);

		t = nextToken();

		//Check for semicolon
		if (t == SEMICOLON) //incorrect token
		{
			match(SEMICOLON);
		}
		else
		{
			reportError("';'");
		}

		break;
	}

	// 4. <statement> -> WRITE LPAREN <idlist> RPAREN;
	case READ:
	{
		match(READ);
		t = nextToken();

		if (t == LPAREN)
		{
			match(LPAREN);
		}
		else
		{
			reportError("'('");
		}

		//call identifier list
		idlist();

		t = nextToken();

		if (t == RPAREN)
		{
			match(RPAREN);
		}
		else
		{
			reportError("')'");
		}

		t = nextToken();

		//Check for semicolon
		if (t == SEMICOLON)
		{
			match(SEMICOLON);
		}
		else
		{
			reportError("';'");
		}

		break;
	}

	// 5. <statement> -> WRITE LPAREN <exprlist> RPAREN;
	case WRITE:
	{
		match(WRITE);
		t = nextToken();

		if (t == LPAREN)
		{
			match(LPAREN);
		}
		else
		{
			reportError("'('");
		}

		//call expression list
		exprlist();

		t = nextToken();

		if (t == RPAREN)
		{
			match(RPAREN);
		}
		else
		{
			reportError("')'");
		}

		t = nextToken();

		//Check for semicolon
		if (t == SEMICOLON)
		{
			match(SEMICOLON);
		}
		else
		{
			reportError("';'");
		}

		break;
	}

	// 6. <statement> -> IF LPAREN <condition> RPAREN THEN <statementlist> <iftail>
	case IF:
	{
		match(IF);
		t = nextToken();

		if (t == LPAREN)
		{
			match(LPAREN);			
		}
		else
		{
			reportError("'('");
		}
				
		struct ExprRecord exp; //temp expression record
		exp = condition();
		processIf(exp);		

		t = nextToken();

		if (t == RPAREN)
		{
			match(RPAREN);
		}
		else
		{
			reportError("')'");
		}

		t = nextToken();

		if (t == THEN)
		{
			match(THEN);
		}
		else
		{
			reportError("\"THEN\"");
		}

		statementList();
		NoMoreStatements = -1;
		iftail();
		break;
	}

	// 9. <statement> -> WHILE LPAREN <condition> RPAREN {<statementlist>} ENDWHILE
	// 9. <statement> -> WHILE LPAREN <condition> RPAREN {<statementlist>} ENDWHILE
	case WHILE:
	{
		match(WHILE);
		t = nextToken();

		if (t == LPAREN)
		{
			match(LPAREN);
		}
		else
		{
			reportError("'('");
		}

		struct ExprRecord expr;

		expr = condition();

		processWhile(&expr);

		t = nextToken();

		if (t == RPAREN)
		{
			match(RPAREN);
		}
		else
		{
			reportError("')'");
		}

		statementList();

		generate("}\n", "", "", "", "", "");

		t = nextToken();

		if (t == ENDWHILE)
		{
			match(ENDWHILE);
		}
		else
		{
			reportError("\"ENDWHILE\"");
		}

		break;
	}

	default:
	{
		t = nextToken();
		// Can allow the parser to recover from a potentially fatal error if a semicolon can be found
		while (ErrStateFlag == 1 && t != SCANEOF && t != END)
		{
			if (match(SEMICOLON) == 1)
				ErrStateFlag = 0;
			else
				t = nextToken();
		}

		//Checks if next token begins a statement. If not, statementlist loop is terminated
		checkForStatement();
		break;
	}

	}
}

// 7. <IFTail> -> ELSE <statementlist> ENDIF
// 8. <IFTail> -> ENDIF
void iftail()
{
	int t = nextToken();

	switch (t)
	{
	// 7. <IFTail> -> ELSE <statementlist> ENDIF
	case ELSE:
		match(ELSE);
		
		//generate else statement
		generate(" } ", " else { ", "", "", "", "");
		
		statementList();
		NoMoreStatements = -1;
		t = nextToken();

		if (t == ENDIF)
		{
			match(ENDIF);
		}
		else
		{
			reportError("\"ENDIF\"");
		}
		break;

	// 8. <IFTail> -> ENDIF
	case ENDIF:
		match(ENDIF);
		break;
	default:
		reportError("\"ENDIF\" or \"ELSE\"");
		ErrStateFlag = 1;
		break;		
		
	}
	generate("}\n", "", "", "", "", "");
}

// 10. <id list> -> <ident> #readID {, <id list>}
void idlist()
{
	struct ExprRecord id;

	int t = nextToken();

	id = ident();
	readID(&id);

	t = nextToken();

	// If there's a comma, expect more identifiers
	if (t == COMMA)
	{
		match(COMMA);
		idlist();
	}
}

// 11. <expr list> -> <expression> #writeExpr {, <expr list>}
void exprlist()
{
	struct ExprRecord expr;

	expression(&expr);

	writeExpr(&expr);

	if (nextToken() == COMMA)
	{
		match(COMMA);
		exprlist();
	}
}


// 12. <expression> -> <term> {<add op> <term> #genInfix}
void expression(struct ExprRecord* result)
{
	struct ExprRecord lOperand, rOperand;
	struct OpRecord operator;

	term(&lOperand);

	int t = nextToken();

	if (t == PLUSOP || t == MINUSOP)
	{
		addop(&operator);
		term(&rOperand);
		lOperand = genInfix(&lOperand, &operator, &rOperand);
	}

	*result = lOperand;
}

//13. <term> -> <factor> {<mult op> <factor> #genInfix}
void term(struct ExprRecord* result)
{
	struct ExprRecord lOperand, rOperand;
	struct OpRecord operator;

	factor(&lOperand);

	int t = nextToken();

	if (t == MULTOP || t == DIVOP)
	{
		multop(&operator);
		factor(&rOperand);
		lOperand = genInfix(&lOperand, &operator, &rOperand);
	}

	*result = lOperand;
}

// 14. <factor> -> LPAREN <expression> RPAREN
// 15. <factor> -> MINUSOP <factor>
// 16. <factor> -> <ident>
// 17. <factor> -> INTLITERAL #processLiteral
void factor(struct ExprRecord* result)
{
	//get next token
	int t = nextToken();

	switch (t)
	{
	// 14. <factor> -> LPAREN <expression> RPAREN
	case LPAREN:
	{
		struct ExprRecord tempExpr;
		match(LPAREN);
		expression(&tempExpr);
		match(RPAREN);
		*result = tempExpr;
		break;
	}

	// 15. <factor> -> MINUSOP <factor>
	case MINUSOP:
	{
		struct ExprRecord tempExpr;
		match(MINUSOP);
		factor(&tempExpr);

		(*result).type = tempExpr.type;

		// operand = -tempExpr.expression
		(*result).expression[0] = '-';
		strcpy(((*result).expression + 1), tempExpr.expression);
		break;
	}

	// 16. <factor> -> <ident>
	case ID:
	{
		*result = ident();
		break;
	}

	// 17. <factor> -> INTLITERAL #processLiteral
	case INTLITERAL:
	{
		match(INTLITERAL);
		*result = processLiteral(getTokenBuffer());
		break;
	}

	// else error
	default:
	{
		reportError("'(', '-', identifier, or number");
		ErrStateFlag = 1;
	}
	}
}

// 18. <add op> -> PLUSOP #processOp
// 19. <add op> -> MINUSOP #processOp
void addop(struct OpRecord* result)
{
	//get next token
	int t = nextToken();

	if (t == PLUSOP || t == MINUSOP)
	{
		match(t);
		*result = processOp(t);
	}
	else
	{
		reportError("'-' or '+'");
		ErrStateFlag = 1;
	}
}

// 20. <mult op> -> MULTOP #processOp
// 21. <add op> -> DIVOP #processOp
void multop(struct OpRecord* result)
{
	//get next token
	int t = nextToken();

	if (t == MULTOP || t == DIVOP)
	{
		match(t);
		*result = processOp(t);
	}
	else
	{
		reportError("'*' or '/'");
		ErrStateFlag = 1;
	}
}

// 22. <condition> -> <addition> {<rel op> <addition> #genInfix}
struct ExprRecord condition()
{
	struct ExprRecord lOperand, rOperand;
	struct OpRecord operator;

	addition(&lOperand);

	int t = nextToken();

	if (
		(t == LESSOP) || (t == LESSEQUALOP) || (t == GREATEROP) ||
		(t == GREATEREQUALOP) || (t == EQUALOP) || (t == NOTEQUALOP)
	   )
	{
		relop(&operator);
		addition(&rOperand);
		lOperand = genInfix(&lOperand, &operator, &rOperand);
	}

	return lOperand;
}

// 23. <addition> -> <multiplication> {<add op> <multiplication> #genInfix}
void addition(struct ExprRecord* result)
{
	struct ExprRecord lOperand, rOperand;
	struct OpRecord operator;

	multiplication(&lOperand);

	int t = nextToken();

	if ((t == PLUSOP) || (t == MINUSOP))
	{
		addop(&operator);
		multiplication(&rOperand);
		lOperand = genInfix(&lOperand, &operator, &rOperand);
	}

	*result = lOperand;
}

// 24. <multiplication> -> <unary> {<mult op> <unary> #genInfix}
void multiplication(struct ExprRecord* result)
{
	struct ExprRecord lOperand, rOperand;
	struct OpRecord operator;

	unary(&lOperand);

	int t = nextToken();

	if ((t == MULTOP) || (t == DIVOP))
	{
		multop(&operator);
		unary(&rOperand);
		lOperand = genInfix(&lOperand, &operator, &rOperand);
	}

	*result = lOperand;
}

// 25. <unary> -> NOTOP <unary>
// 26. <unary> -> MINUSOP <unary>
// 27. <unary> -> <lprimary>
void unary(struct ExprRecord* result)
{
	int t = nextToken();

	switch (t)
	{
	// 25. <unary> -> NOTOP <unary>
	case NOTOP:
	{
		struct ExprRecord tempExpr;
		match(NOTOP);
		unary(&tempExpr);		
		(*result).type = tempExpr.type;
		(*result).expression[0] = '!';		
		strcpy(((*result).expression + 1), tempExpr.expression);
		break;
	}

	// 26. <unary> -> MINUSOP <unary>
	case MINUSOP:
	{
		struct ExprRecord tempExpr;
		match(MINUSOP);
		unary(&tempExpr);
		(*result).type = tempExpr.type;
		(*result).expression[0] = '-';
		strcpy(((*result).expression + 1), tempExpr.expression);
		break;
	}

	// 27. <unary> -> <lprimary>
	default:
		lprimary(result);
	}
}

// 28. <lprimary> -> INTLITERAL #processLiteral
// 29. <lprimary> -> <ident>
// 30. <lprimary> -> LPAREN <condition> RPAREN
// 31. <lprimary> -> FALSEOP #processLiteral
// 32. <lprimary> -> TRUEOP #processLiteral
// 33. <lprimary> -> NULLOP #processLiteral
void lprimary(struct ExprRecord* result)
{
	//Get next token
	int t = nextToken();

	switch (t)
	{
	// 28. <lprimary> -> INTLITERAL #processLiteral
	case INTLITERAL:
		match(INTLITERAL);
		*result = processLiteral(getTokenBuffer());
		break;

	// 29. <lprimary> -> <ident>
	case ID:
		*result = ident();
		break;

	// 30. <lprimary> -> LPAREN <condition> RPAREN
	case LPAREN:
		match(LPAREN);
		*result = condition();
		match(RPAREN);
		break;

	// 31. <lprimary> -> FALSEOP #processLiteral
	case FALSEOP:
		match(FALSEOP);
		char temp[15];
		strcpy(temp, "0\0");		
		*result = processLiteral(temp); 
		break;

	// 32. <lprimary> -> TRUEOP #processLiteral
	case TRUEOP:
		match(TRUEOP);
		char temp1[15];
		strcpy(temp1, "1");		
		*result = processLiteral(temp1); 
		break;

	// 33. <lprimary> -> NULLOP #processLiteral
	case NULLOP:
		match(NULLOP);
		char temp2[15];
		strcpy(temp2, "0\0");
		*result = processLiteral(temp2); 
		break;

	default:
		reportError("integer, identifier, condition, falseop, trueop, or nullop");
		ErrStateFlag = 1;
		break;
	}
}

// 34. <relop> -> LESSOP #processOp
// 35. <relop> -> LESSEQUALOP #processOp
// 36. <relop> -> GREATEROP #processOp
// 37. <relop> -> GREATEREQUALOP #processOp
// 38. <relop> -> EQUALOP #processOp
// 39. <relop> -> NOTEQUALOP #processOp
void relop(struct OpRecord* result)
{
	//Get next token
	int t = nextToken();

	if (t == LESSOP || t == LESSEQUALOP || t == GREATEROP || t == GREATEREQUALOP || t == EQUALOP || t == NOTEQUALOP)
	{
		match(t);
		*result = processOp(getTokenBuffer());
	}
	else
	{
		reportError("relational operator");
		ErrStateFlag = 1;
	}
}
