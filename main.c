
/*
	Created by: Group 5- Jeremy Mwangelwa, Nathaniel DeHart, Matt Oblock, George Brown
	Class: CSC 460 Language Translation
	Program 2: Scanner
	Contacts: mwa2711@calu.edu, deh5850@calu.edu, obl2109@calu.edu, bro8079@calu.edu
*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "file_util.h"
#include "token.h"

#define STATEMENT_BUFFER_SIZE 100

FILE* inputFilePtr;
FILE* outputFilePtr;
FILE* listFilePtr;
FILE* tempFilePtr;

int lexicalErrNum;
int syntaxErrNum = 0; //totals syntax/parser errors
int curLineNum;
int lineNum; //for error reporting
long colZeroPos; //variable to hold file position of column 0
//used to calculate file pointer position for error reporting
long temp; //temporary variable
int tempToken = 0; //temporary token variable


//For recursive calls, if no more statements are left in the production
//is 0 or more
int noMore = -1;

char fatalError = 0;

//To hold scanning errors
char lexErrBuffer[ERROR_BUFFER_SIZE] = { '\0' };
//To hold syntactic errors
char synErrBuffer[ERROR_BUFFER_SIZE] = { '\0' };
//To hold tokens from input file
char tokenBuffer[50] = { '\0' };
//
char statementBuffer[STATEMENT_BUFFER_SIZE] = { '\0' };

//Reports errors
void reportError(char* expectedToken);

/* 
	Grammar Production Functions 
	Note: For all production functions, a return of 0 means error-free.
	Anything greater than 0, means syntax erros were found.
*/

//40. <system goal> -> <program> SCANEOF
void systemGoal(); //done

//1. <program> -> BEGIN <statement list> END
int program(); //done

//2. <statement list> -> <statement> {<statement list>}
int statementList(); 

//3 - 6, 9. <statement>
int statement(); 

//7 - 8. <IFTail> ELSE <statementlist> ENDIF
int iftail(); 

//10. <id list>  -> ID {, <id list>}
int idlist(); 

//11. <expr list> -> <expression> {, <expr list>}
int exprlist(); 

//12. <expression> -> <term> {<add op> <term>}
int expression(); 

//13. <term> -> <factor> {<mult op> <factor>}
int term(); 

//14 - 17. <factor>
int factor(); 

//18 - 19. <add op>
int addop(); 

//20 - 21. <mult op>
int multop(); 

//22. <condition> -> <addition> {<rel op> <addition>}
int condition(); 

//23. <addition> -> <multiplication> {<add op> <multiplication>}
int addition(); 

//24. <multiplication> -> <unary> {<mult op> <unary>}
int multiplication(); 

//25 - 27. <unary>
int unary(); 

//28 - 33. <unary>
int lprimary(); 

//34 - 39. <unary>
int relop(); 

/* 
	Grammar Helper Functions 	
*/

// Returns the next token from the scanner
int nextToken(); //NON DESTRUCTIVE

// Gets the next token from the scanner and checks if it is equal to "token"
// Returns 1 if it is equal
// Returns 0 if it is not equal
int match(int token);

//Checks if code has any more of productions 3,4,5,6, or 9
void checkForStatement();

//== end grammar functions ===//

// Print the parsed statement to the output file
void outputStatement();

// Returns 1 if start was successful
// Returns 0 if there were problems opening files
int start(char* inputFilePath, char* outputFilePath, char* listFilePath, char* tempFilePath);

void end(char* inputFilePath, char* outputFilePath, char* listFilePath, char* tempFilePath);

//Scans input file for tokens and returns the next token
int scanner(int destructive);

void updateListFile(int token);

void updateOutputFile(int token);

//Given enum value of token, and a string, returns string value of token
char * getTokenType(int token, char * str);

// Checks if the string stored in buffer matches any of the hardcoded reserved names
// If it matches a reserved name, then the corresponding token enumeration is returned, else -1 is returned
int checkReserved(const char* buffer);

// Gets parameters from the command prompt (if they exist) and stores them in "inputFilePath" and "outputFilePath" respectively
void getCmdParameters(int argc, char** argv, char* inputFilePath, char* outputFilePath);

int main(int argc, char** argv)
{
	char inputFilePath[MAX_PATH_SIZE] = { '\0' };
	char outputFilePath[MAX_PATH_SIZE] = { '\0' };
	char listFilePath[MAX_PATH_SIZE] = { '\0' };
	char tempFilePath[MAX_PATH_SIZE] = { '\0' };

	//int invalid; unused variable

	// Get command line parameters if they exist
	getCmdParameters(argc, argv, inputFilePath, outputFilePath);

    if (start(inputFilePath, outputFilePath, listFilePath, tempFilePath) == 0)
    { 	
		//Begin parser
		systemGoal(); //Comment this out to work on scanner

		end(inputFilePath, outputFilePath, listFilePath, tempFilePath);
    }

    return 0;
}

//Parser starts here
//<system goal> -> <program>SCANEOF;
void systemGoal()
{
	printf("\n\nStarting parser...\n");

	program();
	if (match(SCANEOF) == 0) // Expected end of file
	{
		reportError("End of File");
		printf("\n\nA FATAL ERROR OCCURRED\n\n");
		fatalError = 1;
	}

}

//<program -> BEGIN <statement list> END
int program()
{
	//Check if token matches 'BEGIN'
	tempToken = nextToken();

	if (tempToken == BEGIN)
	{
		match(BEGIN);
	}
	else //token did not match
	{
		//Display error
		reportError("\"BEGIN\"");
	}
	
	//Get line number for error reporting
	lineNum = curLineNum;
	lineNum++;
	colZeroPos = ftell(inputFilePtr); //get position of column zero for error reporting
	
	//Continue to next production
	statementList();

	//Check if token matches 'END'
	tempToken = nextToken();

	if (tempToken == END)
	{
		match(END);
	}
	else //token did not match
	{
		//Display error
		reportError("\"END\"");
	}

	outputStatement();
	
	return 0;
		
}

//2. <statement_list> -> {<statement list>}
int statementList()
{	
	do 
	{
		statement();

	} while (noMore == -1);

	return 0;

}

//3. <statement> -> ID := <expression>;
//4. <statement> -> READ (<id list>);
//5. <statement> -> WRITE (<expr list>);
//6. <statement> -> IF (<condition>) THEN <statementList><IFTail>
//9. <statement> -> WHILE (<condition>) {<statementList>} ENDWHILE
int statement()
{	
	noMore = -1; //We have statements to process	

	outputStatement();

	//Get next token
	tempToken = nextToken();

	switch (tempToken)
	{
		//3. <statement> -> <expression> //done
		case ID:
		{
			match(ID);
			int t = nextToken();

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

			//Means we have reached end of line, add 1 to line count
			lineNum = curLineNum;
			lineNum++;
			
		}
		break;
		
		//4. <statement> -> WRITE (<idlist>); //done
		case READ:
		{
			match(READ);
			int t = nextToken();

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

			//Means we have reached end of line, add 1 to line count
			lineNum = curLineNum;
			lineNum++;
			
		}
		break;
		
		//5. <statement> -> WRITE (<exprlist>); //done
		case WRITE:
		{
			match(WRITE);
			int t = nextToken();

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

			//Means we have reached end of line, add 1 to line count
			lineNum = curLineNum;
			lineNum++;
			
		}	
		break;	

		//6. <statement> -> IF (<condition>) THEN  <statementlist> <iftail> //done
		case IF:
		{
			match(IF);
			int t = nextToken();

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
		}
		break;
		
		//9. <statement> -> WHILE (<condition>) {<statementlist>} ENDWHILE
		case WHILE:
		{
			match(WHILE);
			int t = nextToken();

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

			//Means we have reached end of line, add 1 to line count
			lineNum = curLineNum;
			lineNum++;		

		}
		break;

		default:

			//Checks if next token begins a statement. If not, statementlist loop is terminated
			checkForStatement();

	}

	outputStatement();
	
return 0;

}

//7 - 8.
int iftail() //done
{
	tempToken = nextToken();

	switch(tempToken)
	{
		//<IFTail> ELSE <statementlist> ENDIF
		case ELSE:
		{
			match(ELSE);
			statementList();

			if (match(ENDIF) == 0)
			{
				reportError("\"ENDIF\"");
			}
		}

		//<IFTail> ENDIF
		case ENDIF:
		{
			match(ENDIF);
		}
	}
	statementList();
	tempToken = match(ENDIF);

	return 0;
}

//10. <id list>  -> ID {, <id list>}
int idlist()
{
	//Check for ID
	if(match(ID) == 0) //incorrect token
	{
		reportError("identifier");
	}

	//If there's a comma, expect more identifiers
	while (nextToken() == COMMA)
	{		
		match(COMMA);
		idlist();
	}

	return 0;
}

//11. <expr list> -> <expression> {, <expr list>}
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


//12. <expression> -> <term> {<add op> <term>} //done
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

//13. <term> -> <factor> {<mult op> <factor>}
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

//14 - 17. <factor>
int factor() 
{
	//get next token
	tempToken = nextToken();
	printf("\nTempToken %d", tempToken);

	switch(tempToken) {

		//14. <factor> -> ( <expression> )
		case LPAREN:
		{
			match(LPAREN);
			expression();
			match (RPAREN);
		}
		break;

		//15. <factor> -> - <factor>
		case MINUSOP:
		{
			match(MINUSOP);
			factor();
		}
		break;

		//16. <factor> -> ID
		case ID:
		{				
			match(ID);
		}
		break;

		//17. <factor> -> INTLITERAL
		case INTLITERAL:
		{		
			match(INTLITERAL);
		}
		break;

		//else error
		default:
		{			
			reportError("'(', '-', identifier, or number");
			match(tempToken);
		}
	}

	return 0;
}

//18 - 19. <add op>
int addop() //done
{
	//get next token
	tempToken = nextToken();

	switch(tempToken) 
	{
		//18. <add op> -> +
		case PLUSOP:
		{
			match(PLUSOP);
		}
		break;

		//19. <add op> -> -
		case MINUSOP:
		{
			match(MINUSOP);
		}
		break;

		default:
		{
			reportError("'-' or '+'");
		}
		break;

	}	

	return 0;
}

//20 - 21. <mult op>
int multop() //done
{
	//get next token
	tempToken = nextToken();

	switch(tempToken) 
	{
		//20. <mult op> -> *
		case MULTOP:
		{
			match(MULTOP);
		}
		break;

		//21. <add op> -> /
		case DIVOP:
		{
			match(DIVOP);
		}
		break;

		default:
		{
			reportError("'*' or '/'");
		}
		break;

	}	

	return 0;
}

//22. <condition> -> <addition> {<rel op> <addition>}
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

//23. <addition> -> <multiplication> {<add op> <multiplication>}
int addition() 
{	
	multiplication();
	int t;
	t = nextToken();	

	if ((t == PLUSOP) || (t == MINUSOP))
	{		
		match(t);
		multiplication();
	}

	return 0;
}


//24. <multiplication> -> <unary> {<mult op> <unary>}
int multiplication()
{
	unary();
	int t;
	t = nextToken();

	if ((t == MULTOP) || (t == DIVOP))
	{			
		match(t);
		unary();	
	}

	return 0;
}

//25 - 27. <unary>
int unary() //done
{	
	//get next token
	tempToken = nextToken();

	switch(tempToken) 
	{
		//'!'
		case NOTOP:
		{
			match(NOTOP);
			unary();
		}
		break;

		//'-'
		case MINUSOP:
		{
			match(MINUSOP);
			unary();
		}
		break;

		default:
		{
			lprimary();
		}
	}

	return 0;
}

//28 - 33. <unary>
int lprimary() //done
{
	//Get next token
	tempToken = nextToken();

	switch(tempToken)
	{
		case INTLITERAL:
		{ 
			match(INTLITERAL);
		}
		break;

		case ID:
		{
			match(ID);
		}
		break;

	  //30. <lprimary -> (<condition>)
		case LPAREN:
		{
			match(LPAREN);
			condition();
			match(RPAREN);
		}
		break;

		//31. <lprimary -> FALSEOP
		case FALSEOP:
		{
			match(FALSEOP);			
		}
		break;

		//32. <lprimary -> TRUEOP
		case TRUEOP:
		{
			match(TRUEOP);
		}
		break;

		//33. <lprimary -> NULLOP
		case NULLOP:
		{
			match(NULLOP);			
		}
		break;

		default:
		{
			reportError("integer, identifier, condition, falseop, trueop, or nullop");
		}
		break;
	}	

	return 0;
}

//34 - 39. <unary>
int relop() //done
{	
	//Get next token
	tempToken = nextToken();

	switch(tempToken)
	{
		case LESSOP:
		{ 
			match(LESSOP);
		}
		break;

		case LESSEQUALOP:
		{
			match(LESSEQUALOP);
		}
		break;

		case GREATEROP:
		{
			match(GREATEROP);
		}
		break;

		case GREATEREQUALOP:
		{
			match(GREATEREQUALOP);
		}
		break;

		case EQUALOP:
		{
			match(EQUALOP);
		}
		break;

		case NOTEQUALOP:
		{
			match(NOTEQUALOP);
		}
		break;

		default:
		{
			reportError("relational operator");
		}
		break;
	}

	return 0;
}


// Gets the next token from the scanner and checks if it is equal to "token"
// Returns 1 if it is equal
// Returns 0 if it is not equal
int match(int token) //DESTRUCTIVE
{
	int returnVal;
	if (token == scanner(1))
	{
		returnVal = 1;
	}
	else
	{
		returnVal = 0;
	}
	updateOutputFile(token);
	return returnVal;
}

// Looks better than calling "scanner(0)" everywhere
int nextToken()
{
	return scanner(0);
}

// Attempts to open all of the needed files
// Returns 1 if start was successful
// Returns 0 if there were problems opening files
int start(char* inputFilePath, char* outputFilePath, char* listFilePath, char* tempFilePath)
{
	int returnVal;

	char** restrictExtsn = calloc(2, sizeof(char*));
	restrictExtsn[0] = malloc(FILENAME_MAX);
	restrictExtsn[1] = malloc(FILENAME_MAX);

	char temp[FILENAME_MAX];

	strcpy(restrictExtsn[0], ".lis");
	strcpy(restrictExtsn[1], ".out");

	// Displays our group title
	printf("\n-----------------------------------\n");
	printf("------GROUP 5: PARSER PROGRAM------\n");
	printf("-----------------------------------\n");
	printf("\n");

	returnVal = getInputFile(inputFilePath, (const char**)restrictExtsn, 2);
	if (returnVal == 0)
	{
		returnVal = getOutputFile(outputFilePath, inputFilePath, (const char**)restrictExtsn, 1);
	}
	// Checks if still valid
	if (returnVal == 0)
	{
		strcpy(listFilePath, outputFilePath);
		changeFileExtension(listFilePath, ".lis");
		strcpy(tempFilePath, "temp.lis");
		getFileDirectory(temp, outputFilePath);
		changeFileDirectory(tempFilePath, temp);

		if (strcmp(listFilePath, tempFilePath))
		{
			inputFilePtr = fileOpen(inputFilePath, "rb");
			outputFilePtr = fileOpen(outputFilePath, "w");
			listFilePtr = fileOpen(listFilePath, "w");
			tempFilePtr = fileOpen(tempFilePath, "w+");
		}
		else
		{
			printf("The listing file and temp file have the same name!\n");
			printf("To prevent this do not name the output file \"temp\"\n");
		}

		if (inputFilePtr == NULL || outputFilePtr == NULL || listFilePtr == NULL || tempFilePtr == NULL)
		{
			returnVal = 1;
		}
		else
		{
			fputs("The Temp", tempFilePtr);
		}
	}

	free(restrictExtsn[1]);
	free(restrictExtsn[0]);
	free(restrictExtsn);

	return returnVal;
}

// Prints the total number of lexical errors at the end of the listing file
// Closes all of the files
void end(char* inputFilePath, char* outputFilePath, char* listFilePath, char* tempFilePath)
{
	sprintf(tokenBuffer, "\n\nThere are %d lexical errors.", lexicalErrNum); // Resuses the token buffer temporarily because why not
	fputs(tokenBuffer, listFilePtr); // Puts the total number of errors at the end of the list file

	sprintf(tokenBuffer, "\nThere are %d syntax errors.", syntaxErrNum);
	fputs(tokenBuffer, listFilePtr);

	if (fatalError)
		sprintf(tokenBuffer, "\nThe program did not finish compilation.");
	else
	{
		if (syntaxErrNum == 0 && lexicalErrNum == 0)
			sprintf(tokenBuffer, "\nThe program compiled with no errors.");
		else
			sprintf(tokenBuffer, "\nThe program compiled with a total of %d errors.", (syntaxErrNum + lexicalErrNum));
	}
	fputs(tokenBuffer, listFilePtr);

	rewind(tempFilePtr);
	copyFileContents(tempFilePtr, outputFilePtr); // In this case copies "The Temp" into the end of the output file (current file position)

	fileClose(inputFilePtr, inputFilePath);
	fileClose(outputFilePtr, outputFilePath);
	fileClose(listFilePtr, listFilePath);
	fileClose(tempFilePtr, tempFilePath);

	//if (fileExists("temp.out"))
		//remove("temp.out");
}

// Print the parsed statement to the output file
void outputStatement()
{
	if (statementBuffer[0] != '\0')
	{
		// Note: "statementBuffer" receives its data in "updateOutputFile()"

		fputs(statementBuffer, outputFilePtr);
		fputs("\n\n", outputFilePtr);
		statementBuffer[0] = '\0';
	}
}

// Returns 1 if the character is an alpha ASCII character, else returns 0
int charIsAlpha(char c)
{
	return (c > 64 && c < 91) || (c > 96 && c < 123);
}

// Returns 1 if the character is a numberic ASCII character, else returns 0
int charIsInt(char c)
{
	return c > 47 && c < 58;
}

// Gets parameters from the command prompt (if they exist) and stores them in "inputFilePath" and "outputFilePath" respectively
void getCmdParameters(int argc, char** argv, char* inputFilePath, char* outputFilePath)
{
    // Both input and output files entered
    if (argc >= 3)
    {
        printf("You entered two files:\n");
        printf("1. INPUT: %s \n2. OUTPUT: %s \n", argv[1], argv[2]);
        strcpy(inputFilePath, argv[1]);
        strcpy(outputFilePath, argv[2]);
    }
    else if (argc == 2)
    {

        printf("You entered an input file name: ");
        printf("%s", argv[1]);
        strcpy(inputFilePath, argv[1]);
    }
    else
    {
        printf("You entered no files.");
    }
}

// Scans input file for tokens and returns the next token
// If "destructive" is 0 then the input file pointer will be pointing at the same location as when it started
// If "destructive" is not equal to 0 then it will update the listing file
// When a token is returned "tokenBuffer" will contain the string form of the token
int scanner(int destructive)
{
	tokenBuffer[0] = '\0'; //Clear token buffer
	int token = -1;
	char c = 0; //Initialize variable to read chars from inputfile
	long ogFilePos = ftell(inputFilePtr);

	if (curLineNum < 1)
	{
		curLineNum = 1;
		fputs("1.\t", listFilePtr); // Places the first line number into the listing file
	}
		
	//Read each character in input file
	while (c != EOF)
	{
		//Read character from file
		c = getc(inputFilePtr);

		if (charIsAlpha(c)) //An identifier - starts with a-zA-Z
		{

			long lastPos = 0;

			while (charIsAlpha(c) || charIsInt(c)) //An identifier as in as23 etc
			{
				
				lastPos = ftell(inputFilePtr);
				strncat(tokenBuffer, &c, 1); //copy c to token buffer
				c = getc(inputFilePtr);
				
			}

			token = checkReserved(tokenBuffer); //Checks if the token matches a reserved token first
			if (token == -1)
				token = ID;
			
			c = '\0'; // So that the listing file doesn't write before a possible new line character
			fseek(inputFilePtr, lastPos, SEEK_SET); // Puts the file pointer back a byte so that it doesn't skip the next character

		} //end if (c is alpha)
		else if (c == '-') //Minus sign
		{

			strncat(tokenBuffer, &c, 1);
			c = getc(inputFilePtr);

			if (c == '-') //A comment
			{
				while (c != '\n') //Process comment until end of line
				{

					strncat(tokenBuffer, &c, 1); // Comment this line out to not copy the comment to the listing file
					c = getc(inputFilePtr);

				}
				//tokenBuffer[0] = '\0'; // Uncomment this line to not copy the '-' to the listing file
				strncat(tokenBuffer, &c, 1);

			}
			else if (charIsInt(c)) //A negative number
			{

				long lastPos = 0;

				while (charIsInt(c)) //Copy the rest of digits
				{
					lastPos = ftell(inputFilePtr);
					strncat(tokenBuffer, &c, 1); //copy c to token buffer
					c = getc(inputFilePtr); //read c
				}

				token = checkReserved(tokenBuffer); //Checks if the token mathces a reserved token first
				if (token == -1)
					token = INTLITERAL;

				c = '\0'; // So that the listing file doesn't write before a possible new line character
				fseek(inputFilePtr, lastPos, SEEK_SET); // Puts the file pointer back a byte so that it doesn't skip the next character
			}
			else
			{
				token = checkReserved(tokenBuffer);	//Checks if the token mathces a reserved token first
				if (token == -1)
					token = MINUSOP;
			}

		}
		else if (charIsInt(c))
		{

			long lastPos = 0;

			while (charIsInt(c)) //A number
			{
				lastPos = ftell(inputFilePtr);
				strncat(tokenBuffer, &c, 1); //copy c to token buffer
				c = getc(inputFilePtr);
			}

			token = checkReserved(tokenBuffer); //Checks if the token mathces a reserved token first
			if (token == -1)
				token = INTLITERAL;

			c = '\0'; // So that the listing file doesn't write before a possible new line character
			fseek(inputFilePtr, lastPos, SEEK_SET); // Puts the file pointer back a byte so that it doesn't skip the next character

		}
		else
		{
			strncat(tokenBuffer, &c, 1); //copy c to token buffer

			//Check for other tokens
			switch (c)
			{
			case '(':
				token = LPAREN;
				break;
			case ')':
				token = RPAREN;
				break;
			case ';':
				token = SEMICOLON;
				break;
			case ',':
				token = COMMA;
				break;
			case '+':
				token = PLUSOP;
				break;
			case '*':
				token = MULTOP;
				break;
			case '/':
				token = DIVOP;
				break;
			case '!':
				token = NOTOP;
				break;
			case '=':
				token = EQUALOP;
				break;
				//Operators with two characters
			case ':':
				c = getc(inputFilePtr);
				strncat(tokenBuffer, &c, 1); //copy c to token buffer
				if (c == '=')
				{
					token = ASSIGNOP;
				}
				else
				{
					token = ERROR;
				}
				break;
			case '<':
				c = getc(inputFilePtr);
				strncat(tokenBuffer, &c, 1); //copy c to token buffer
				if (c == '=')
				{
					token = LESSEQUALOP;
				}
				else if (c == '>')
				{
					token = NOTEQUALOP;
				}
				else
				{
					token = LESSOP;
				}
				break;
			case '>':
				c = getc(inputFilePtr);
				strncat(tokenBuffer, &c, 1); //copy c to token buffer
				if (c == '=')
				{
					token = GREATEREQUALOP;
				}
				else
				{
					token = GREATEROP;
				}
				break;
			case EOF:
				token = SCANEOF;
				break;
			case ' ':
				break;
			case '\t':
				break;
			case '\r':
				break;
			case '\n':
				break;
			default:
				token = ERROR;
			}
		}

		// Updates the List file (including spaces)
		if (destructive)
		{
			// Ensures that if the next character is a newline, it gets added to the token buffer
			if (token != ERROR && c != '\n')
			{
				long lastPos = ftell(inputFilePtr);
				c = getc(inputFilePtr);
				if (c != '\n')
					fseek(inputFilePtr, lastPos, SEEK_SET);
				else
					strncat(tokenBuffer, "\n", 1);
			}
			updateListFile(token);
		}

		if (token == ERROR)
			token = -1;

		if (token > -1)
		{
			if (!destructive)
				fseek(inputFilePtr, ogFilePos, SEEK_SET);
			return token;
		}
		else
			tokenBuffer[0] = '\0'; // "Empties" the string
	}
	return SCANEOF;
}

void updateListFile(int token)
{

	if (token == SCANEOF)
	{
		strcpy(tokenBuffer, "EOF");
		fputs("\n", listFilePtr);
		fputs(synErrBuffer, listFilePtr);
		synErrBuffer[0] = '\0';
	}
	//Write "tokenBuffer" to listing file ("tokenBuffer" now gets every character copied into to and emptied before the next token)
	if (fputs(tokenBuffer, listFilePtr))
	{
		printf("\nERROR writing to listing file...");
	}
	// Write the error into a buffer for later writing to the list file
	if (token == ERROR)
	{
		char temp[10];
		itoa(curLineNum, temp, 10);

		lexicalErrNum++;
		strncat(lexErrBuffer, "Lexical error on line ", ERROR_BUFFER_SIZE - strlen(lexErrBuffer));
		strncat(lexErrBuffer, temp, ERROR_BUFFER_SIZE - strlen(lexErrBuffer));
		strncat(lexErrBuffer, ": Unexpected token '", ERROR_BUFFER_SIZE - strlen(lexErrBuffer));
		strncat(lexErrBuffer, tokenBuffer, ERROR_BUFFER_SIZE - strlen(lexErrBuffer));
		strncat(lexErrBuffer, "'\n", ERROR_BUFFER_SIZE - strlen(lexErrBuffer));
		if (strlen(lexErrBuffer) >= ERROR_BUFFER_SIZE)
			strcpy(lexErrBuffer, "Many unexpected tokens were found!"); // For cases in which the error buffer is too small
	}
	//If reached end of line, print possible errors and new line number to listing file
	if (tokenBuffer[strlen(tokenBuffer) - 1] == '\n')
	{
		fputs(lexErrBuffer, listFilePtr);
		lexErrBuffer[0] = '\0';

		fputs(synErrBuffer, listFilePtr);
		synErrBuffer[0] = '\0';

		curLineNum++;

		int tempSize = (sizeof(char) * 3 + sizeof(char) * (log(curLineNum) / log(10) + 1));
		// Allocates just enough space for the line number, a period, and a tab (and a NULL terminator) in the form of a character string
		char* temp = malloc(tempSize);

		//Converts the line number to a string and stores it in "listingBuffer"
		itoa(curLineNum, temp, 10);
		//Puts the period and tab after the line number
		strcat(temp, ".\t");

		temp[tempSize / sizeof(char) - 1] = '\0'; // NULL terminates the string

		// Writes the line number to the listing file
		if (fputs(temp, listFilePtr) != 0)
		{
			printf("\nERROR writing to listing file...");
		}

		free(temp);
	}
}

void updateOutputFile(int token)
{
	// Ensures the token is valid
	if (token > -1)
	{
		// The removes a newline character if there was one
		if (tokenBuffer[strlen(tokenBuffer) - 1] == '\n')
			tokenBuffer[strlen(tokenBuffer) - 1] = '\0';

		char tokenType[20] = { '\0' };
		char temp[12];
		char outputBuffer[70] = { '\0' }; //To print to output file

		//Get token type as string	
		getTokenType(token, tokenType);

		strncat(statementBuffer, tokenType, STATEMENT_BUFFER_SIZE - strlen(statementBuffer));
		strncat(statementBuffer, " ", STATEMENT_BUFFER_SIZE - strlen(statementBuffer));

		//Build output buffer: Add "token number: "
		strcpy(outputBuffer, "token number: ");

		//Turn token into string and add to output buffer
		sprintf(temp, "%d", token);
		strcat(outputBuffer, temp);

		//Add tab and "token type: " to output buffer
		strcat(outputBuffer, "\t\t");
		strcat(outputBuffer, "token type: ");
		strcat(outputBuffer, tokenType);

		//Add two tabs, "actual token: ", and actual token to output buffer
		strcat(outputBuffer, "\t\t\t\t");
		strcat(outputBuffer, "actual token: ");
		strcat(outputBuffer, tokenBuffer); //actual token will be in token buffer
		strcat(outputBuffer, "\n");

		//Write output buffer to output file
		if (fputs(outputBuffer, outputFilePtr) != 0)
		{
			printf("\nERROR writing to output file...");
		}
	}
}

// Checks if the string stored in buffer matches any of the hardcoded reserved names
// If it matches a reserved name, then the corresponding token enumeration is returned, else -1 is returned
int checkReserved(const char* buffer)
{
	int returnVal = -1;

	if (strcmpi(buffer, "begin") == 0) // Compares "buffer" with "begin" while ignoring the case
		returnVal = BEGIN;
	else if (strcmpi(buffer, "end") == 0)
		returnVal = END;
	else if (strcmpi(buffer, "read") == 0)
		returnVal = READ;
	else if (strcmpi(buffer, "write") == 0)
		returnVal = WRITE;
	else if (strcmpi(buffer, "true") == 0)
		returnVal = TRUEOP;
	else if (strcmpi(buffer, "false") == 0)
		returnVal = FALSEOP;
	else if (strcmpi(buffer, "null") == 0)
		returnVal = NULLOP;
	else if (strcmpi(buffer, "if") == 0)
		returnVal = IF;
	else if (strcmpi(buffer, "then") == 0)
		returnVal = THEN;
	else if (strcmpi(buffer, "endif") == 0)
		returnVal = ENDIF;
	else if (strcmpi(buffer, "while") == 0)
		returnVal = WHILE;
	else if (strcmpi(buffer, "endwhile") == 0)
		returnVal = ENDWHILE;

	return returnVal;
}

//Given enum value of token, and a string, returns string value of token
char * getTokenType(int token, char * str)
{
	//Receive string
	
	//To upper identifier
	
	//Then check token
	
	switch(token)
	{	
		case BEGIN:
			strcpy(str, "BEGIN");
			break;
		
		case END:
			strcpy(str, "END");
			break;
		
		case READ:
			strcpy(str, "READ");
			break;
		
		case WRITE:
			strcpy(str, "WRITE");
			break;
		
		case IF:
			strcpy(str, "IF");
			break;
		
		case THEN:
			strcpy(str, "THEN");
			break;
		
		case ELSE:
			strcpy(str, "ELSE");
			break;
		
		case ENDIF:
			strcpy(str, "ENDIF");
			break;
		
		case WHILE:
			strcpy(str, "WHILE");
			break;
		
		case ENDWHILE:
			strcpy(str, "ENDWHILE");
			break;
		
		case ID:
			strcpy(str, "ID");
			break;
		
		case INTLITERAL:
			strcpy(str, "INTLITERAL");
			break;
		
		case FALSEOP:
			strcpy(str, "FALSEOP");
			break;
		
		case TRUEOP:
			strcpy(str, "TRUEOP");
			break;
		
		case NULLOP:
			strcpy(str, "NULLOP");
			break;
		
		case LPAREN:
			strcpy(str, "LPAREN");
			break;
		
		case RPAREN:
			strcpy(str, "RPAREN");
			break;
		
		case SEMICOLON:
			strcpy(str, "SEMICOLON");
			break;
		
		case COMMA:
			strcpy(str, "COMMA");
			break;
		
		case ASSIGNOP:
			strcpy(str, "ASSIGNOP");
			break;
		
		case PLUSOP:
			strcpy(str, "PLUSOP");
			break;
		
		case MINUSOP:
			strcpy(str, "MINUSOP");
			break;
		
		case MULTOP:
			strcpy(str, "MULTOP");
			break;
		
		case DIVOP:
			strcpy(str, "DIVOP");
			break;
		
		case NOTOP:
			strcpy(str, "NOTOP");
			break;
		
		case LESSOP:
			strcpy(str, "LESSOP");
			break;
		
		case LESSEQUALOP:
			strcpy(str, "LESSEQUALOP");
			break;
		
		case GREATEROP:
			strcpy(str, "GREATEROP");
			break;
		
		case GREATEREQUALOP:
			strcpy(str, "GREATEREQUALOP");
			break;
		
		case EQUALOP:
			strcpy(str, "EQUALOP");
			break;
		
		case NOTEQUALOP:
			strcpy(str, "NOTEQUALOP");
			break;
		
		case SCANEOF:
			strcpy(str, "SCANEOF");
			break;
		
		case ERROR:
			strcpy(str, "ERROR");
			break;					
		
	}
			
	return str;
}

//Reports errors
void reportError(char* expectedToken)
{
	//Display error
	//printf("\nError: Line %d, expected %s", lineNum, expectedToken);

	char temp[10];
	itoa(curLineNum, temp, 10);

	syntaxErrNum++;
	strncat(synErrBuffer, "Syntax error on line ", ERROR_BUFFER_SIZE - strlen(synErrBuffer));
	strncat(synErrBuffer, temp, ERROR_BUFFER_SIZE - strlen(synErrBuffer));
	strncat(synErrBuffer, " (Expected ", ERROR_BUFFER_SIZE - strlen(synErrBuffer));
	strncat(synErrBuffer, expectedToken, ERROR_BUFFER_SIZE - strlen(synErrBuffer));
	strncat(synErrBuffer, "): Found \"", ERROR_BUFFER_SIZE - strlen(synErrBuffer));
	strncat(synErrBuffer, tokenBuffer, ERROR_BUFFER_SIZE - strlen(synErrBuffer));
	strncat(synErrBuffer, "\"\n", ERROR_BUFFER_SIZE - strlen(synErrBuffer));
	if (strlen(synErrBuffer) >= ERROR_BUFFER_SIZE)
		strcpy(synErrBuffer, "Many errors tokens were found!"); // For cases in which the error buffer is too small
}

//Check for statements
void checkForStatement()
{
	int myToken;
	myToken = nextToken();

	//if the next token is not a statement
	if (!(myToken == ID) || !(myToken == READ) || !(myToken == WRITE) || !(myToken ==IF) || !(myToken == WHILE))
	{
		noMore++; //raise flag meaning no more statements
	}
}
