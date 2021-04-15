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

//
void compile(FILE* in, FILE* out, FILE* list, FILE* temp)
{
	OutputFilePtr = out;
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
		addToErrorBuffer(getActualToken());
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
// 40. <system goal> -> <program> SCANEOF;
void systemGoal()
{
	program();
	if (match(SCANEOF) == 0) // Expected end of file
	{
		reportError("End of File");
		printf("\n\nA FATAL ERROR OCCURRED\n\n");
		FatalError = 1;
	}

}

// 1. <program> -> #genStart BEGIN <statement list> END
int program()
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

	return 0;

}

// 2. <statement_list> -> <statement> {<statement list>}
int statementList()
{
	do
	{
		statement();

	} while (NoMoreStatements == -1);

	return 0;

}

// 3. <statement> -> ID ASSIGNOP <expression> #processAssign;
// 4. <statement> -> WRITE LPAREN <idlist> RPAREN;
// 5. <statement> -> WRITE LPAREN <exprlist> RPAREN;
// 6. <statement> -> IF LPAREN <condition> RPAREN THEN <statementlist> <iftail>
// 9. <statement> -> WHILE LPAREN <condition> RPAREN {<statementlist>} ENDWHILE
int statement()
{
	NoMoreStatements = -1; //We have statements to process	

	//Get next token
	int t = nextToken();

	switch (t)
	{
	// 3. <statement> -> ID ASSIGNOP <expression> #processAssign;
	case ID:
		match(ID);

		struct ExprRecord leftExpr;
		getTokenBuffer();
		leftExpr.type = IDEXPR;

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
		expression();

		

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

	// 4. <statement> -> WRITE LPAREN <idlist> RPAREN;
	case READ:
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

	// 5. <statement> -> WRITE LPAREN <exprlist> RPAREN;
	case WRITE:
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

	// 6. <statement> -> IF LPAREN <condition> RPAREN THEN <statementlist> <iftail>
	case IF:
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

		condition();

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
		iftail();
		break;

	// 9. <statement> -> WHILE LPAREN <condition> RPAREN {<statementlist>} ENDWHILE
	case WHILE:
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

		condition();

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

	default:
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

	return 0;

}

// 7. <IFTail> -> ELSE <statementlist> ENDIF
// 8. <IFTail> -> ENDIF
int iftail()
{
	int t = nextToken();

	switch (t)
	{
	// 7. <IFTail> -> ELSE <statementlist> ENDIF
	case ELSE:
		match(ELSE);
		statementList();
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

	return 0;
}

// 10. <id list> -> ID #readID {, <id list>}
int idlist()
{
	int t = nextToken();

	//Check for ID
	if (t == ID) //incorrect token
	{
		match(ID);
	}
	else
	{
		reportError("identifier");
	}

	t = nextToken();

	//If there's a comma, expect more identifiers
	if (t == COMMA)
	{
		match(COMMA);
		idlist();
	}

	return 0;
}

// 11. <expr list> -> <expression> #writeExpr {, <expr list>}
int exprlist()
{
	expression();

	if (nextToken() == COMMA)
	{
		match(COMMA);
		exprlist();
	}

	return 0;
}


//12. <expression> -> <term> {<add op> <term> #genInfix}
int expression()
{
	term();
	int t = nextToken();

	if (t == PLUSOP || t == MINUSOP)
	{
		match(t);
		term();
	}

	return 0;
}

//13. <term> -> <factor> {<mult op> <factor> #genInfix}
int term()
{
	factor();
	int t = nextToken();

	if (t == MULTOP || t == DIVOP)
	{
		match(t);
		factor();
	}

	return 0;
}

// 14. <factor> -> LPAREN <expression> RPAREN
// 15. <factor> -> MINUSOP <factor>
// 16. <factor> -> ID
// 17. <factor> -> INTLITERAL #processLiteral
int factor()
{
	//get next token
	int t = nextToken();

	switch (t)
	{
	// 14. <factor> -> LPAREN <expression> RPAREN
	case LPAREN:
		match(LPAREN);
		expression();
		match(RPAREN);
		break;

	// 15. <factor> -> MINUSOP <factor>
	case MINUSOP:
		match(MINUSOP);
		factor();
		break;

	// 16. <factor> -> ID
	case ID:
		match(ID);
		break;

	// 17. <factor> -> INTLITERAL
	case INTLITERAL:
		match(INTLITERAL);
		break;

	// else error
	default:
		reportError("'(', '-', identifier, or number");
		ErrStateFlag = 1;
	}

	return 0;
}

// 18. <add op> -> PLUSOP #processOp
// 19. <add op> -> MINUSOP #processOp
int addop() //done
{
	//get next token
	int t = nextToken();

	switch (t)
	{
	// 18. <add op> -> PLUSOP #processOp
	case PLUSOP:
		match(PLUSOP);
		break;

	// 19. <add op> -> MINUSOP #processOp
	case MINUSOP:
		match(MINUSOP);
		break;

	default:
		reportError("'-' or '+'");
		ErrStateFlag = 1;
		break;

	}

	return 0;
}

// 20. <mult op> -> MULTOP #processOp
// 21. <add op> -> DIVOP #processOp
int multop() //done
{
	//get next token
	int t = nextToken();

	switch (t)
	{
	// 20. <mult op> -> MULTOP #processOp
	case MULTOP:
		match(MULTOP);
		break;

	// 21. <add op> -> DIVOP #processOp
	case DIVOP:
		match(DIVOP);
		break;

	default:
		reportError("'*' or '/'");
		ErrStateFlag = 1;
		break;
	}
	

	return 0;
}

// 22. <condition> -> <addition> {<rel op> <addition> #genInfix}
int condition()
{
	addition();
	int t = nextToken();

	if (
		(t == LESSOP) || (t == LESSEQUALOP) || (t == GREATEROP) ||
		(t == GREATEREQUALOP) || (t == EQUALOP) || (t == NOTEQUALOP)
		)
	{
		match(t);
		addition();
	}

	return 0;
}

// 23. <addition> -> <multiplication> {<add op> <multiplication>}
int addition()
{
	multiplication();
	int t = nextToken();

	if ((t == PLUSOP) || (t == MINUSOP))
	{
		match(t);
		multiplication();
	}

	return 0;
}


// 24. <multiplication> -> <unary> {<mult op> <unary>}
int multiplication()
{
	unary();
	int t = nextToken();

	if ((t == MULTOP) || (t == DIVOP))
	{
		match(t);
		unary();
	}

	return 0;
}

// 25. <unary> -> NOTOP <unary>
// 26. <unary> -> MINUSOP <unary>
// 27. <unary> -> <lprimary>
int unary() //done
{
	//get next token
	int t = nextToken();

	switch (t)
	{
	// 25. <unary> -> NOTOP <unary>
	case NOTOP:
		match(NOTOP);
		unary();
		break;

	// 26. <unary> -> MINUSOP <unary>
	case MINUSOP:
		match(MINUSOP);
		unary();
		break;

	// 27. <unary> -> <lprimary>
	default:
		lprimary();
	}

	return 0;
}

// 28. <lprimary> -> INTLITERAL
// 29. <lprimary> -> ID 
// 30. <lprimary> -> LPAREN <condition> RPAREN
// 31. <lprimary> -> FALSEOP
// 32. <lprimary> -> TRUEOP
// 33. <lprimary> -> NULLOP
int lprimary()
{
	//Get next token
	int t = nextToken();

	switch (t)
	{
	// 28. <lprimary> -> INTLITERAL
	case INTLITERAL:
		match(INTLITERAL);
		break;

	// 29. <lprimary> -> ID 
	case ID:
		match(ID);
		break;

	// 30. <lprimary> -> LPAREN <condition> RPAREN
	case LPAREN:
		match(LPAREN);
		condition();
		match(RPAREN);
		break;

	// 31. <lprimary> -> FALSEOP
	case FALSEOP:
		match(FALSEOP);
		break;

	// 32. <lprimary> -> TRUEOP
	case TRUEOP:
		match(TRUEOP);
		break;

	// 33. <lprimary> -> NULLOP
	case NULLOP:
		match(NULLOP);
		break;

	default:
		reportError("integer, identifier, condition, falseop, trueop, or nullop");
		ErrStateFlag = 1;
		break;
	}

	return 0;
}

// 34. <relop> -> LESSOP
// 35. <relop> -> LESSEQUALOP
// 36. <relop> -> GREATEROP
// 37. <relop> -> GREATEREQUALOP
// 38. <relop> -> EQUALOP
// 39. <relop> -> NOTEQUALOP
int relop()
{
	//Get next token
	int t = nextToken();

	switch (t)
	{
	// 34. <relop> -> LESSOP
	case LESSOP:
		match(LESSOP);
		break;
	// 35. <relop> -> LESSEQUALOP
	case LESSEQUALOP:
		match(LESSEQUALOP);
		break;
	// 36. <relop> -> GREATEROP
	case GREATEROP:
		match(GREATEROP);
		break;
	// 37. <relop> -> GREATEREQUALOP
	case GREATEREQUALOP:
		match(GREATEREQUALOP);
		break;
	// 38. <relop> -> EQUALOP
	case EQUALOP:
		match(EQUALOP);
		break;
	// 39. <relop> -> NOTEQUALOP
	case NOTEQUALOP:
		match(NOTEQUALOP);
		break;

	default:
		reportError("relational operator");
		ErrStateFlag = 1;
		break;
	}

	return 0;
}