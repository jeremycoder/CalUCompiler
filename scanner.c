#include "scanner.h"

#define ERROR_BUFFER_SIZE 500
#define TOKEN_BUFFER_SIZE 50

int LexicalErrNum;

FILE* InputFilePtr;
FILE* ListFilePtr;
FILE* OutputFilePtr;

//To hold syntactic errors
char ExtraErrBuffer[ERROR_BUFFER_SIZE] = { '\0' };

//To hold tokens from input file
char TokenBuffer[TOKEN_BUFFER_SIZE] = { '\0' };

int CurLineNum;

//To hold scanning errors
char LexErrBuffer[ERROR_BUFFER_SIZE] = { '\0' };

//
void scannerInit(FILE* inputFilePtr, FILE* outputFilePtr, FILE* listFilePtr)
{
	InputFilePtr = inputFilePtr;
	OutputFilePtr = outputFilePtr;
	ListFilePtr = listFilePtr;
}

//
void addToErrorBuffer(const char* error)
{
	strncat(ExtraErrBuffer, error, ERROR_BUFFER_SIZE - strlen(ExtraErrBuffer));
	if (strlen(ExtraErrBuffer) >= ERROR_BUFFER_SIZE)
		strcpy(ExtraErrBuffer, "Many error tokens were found!"); // For cases in which the error buffer is too small
}

//Returns the current total of lexical errors found in the input file
int getTotalLexErrors()
{
	return LexicalErrNum;
}

//Returns the current line number that the scanner is scanning of the input file
int getCurLineNum()
{
	return CurLineNum;
}

// Returns "TokenBuffer"
// The size of "TokenBuffer" is TOKEN_BUFFER_SIZE characters
char* getTokenBuffer()
{
	return TokenBuffer;
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
	else if (strcmpi(buffer, "else") == 0)
		returnVal = ELSE;
	else if (strcmpi(buffer, "endif") == 0)
		returnVal = ENDIF;
	else if (strcmpi(buffer, "while") == 0)
		returnVal = WHILE;
	else if (strcmpi(buffer, "endwhile") == 0)
		returnVal = ENDWHILE;

	return returnVal;
}

//Given enum value of token, and a string, returns string value of token
char* getTokenType(int token, char* str)
{
	switch (token)
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

	}

	return str;
}

// Scans input file for tokens and returns the next token
// If "destructive" is 0 then the input file pointer will be pointing at the same location as when it started
// If "destructive" is not equal to 0 then it will update the listing file
// When a token is returned "tokenBuffer" will contain the string form of the token
int scanner(int destructive)
{
	TokenBuffer[0] = '\0'; //Clear token buffer
	int token = -1;
	char c = 0; //Initialize variable to read chars from inputfile
	long ogFilePos = ftell(InputFilePtr);

	if (CurLineNum < 1)
	{
		CurLineNum = 1;
		fputs("1.\t", ListFilePtr); // Places the first line number into the listing file
	}

	//Read each character in input file
	while (c != EOF)
	{
		//Read character from file
		c = getc(InputFilePtr);

		if (charIsAlpha(c)) //An identifier - starts with a-zA-Z
		{

			long lastPos = 0;

			while (charIsAlpha(c) || charIsInt(c)) //An identifier as in as23 etc
			{

				lastPos = ftell(InputFilePtr);
				strncat(TokenBuffer, &c, 1); //copy c to token buffer
				c = getc(InputFilePtr);

			}

			token = checkReserved(TokenBuffer); //Checks if the token matches a reserved token first
			if (token == -1)
				token = ID;

			c = '\0'; // So that the listing file doesn't write before a possible new line character
			fseek(InputFilePtr, lastPos, SEEK_SET); // Puts the file pointer back a byte so that it doesn't skip the next character

		} //end if (c is alpha)
		else if (c == '-') //Minus sign
		{

			strncat(TokenBuffer, &c, 1);
			c = getc(InputFilePtr);

			if (c == '-') //A comment
			{
				while (c != '\n') //Process comment until end of line
				{

					strncat(TokenBuffer, &c, 1); // Comment this line out to not copy the comment to the listing file
					c = getc(InputFilePtr);

				}
				//tokenBuffer[0] = '\0'; // Uncomment this line to not copy the '-' to the listing file
				strncat(TokenBuffer, &c, 1);

			}
			else if (charIsInt(c)) //A negative number
			{

				long lastPos = 0;

				while (charIsInt(c)) //Copy the rest of digits
				{
					lastPos = ftell(InputFilePtr);
					strncat(TokenBuffer, &c, 1); //copy c to token buffer
					c = getc(InputFilePtr); //read c
				}

				token = checkReserved(TokenBuffer); //Checks if the token mathces a reserved token first
				if (token == -1)
					token = INTLITERAL;

				c = '\0'; // So that the listing file doesn't write before a possible new line character
				fseek(InputFilePtr, lastPos, SEEK_SET); // Puts the file pointer back a byte so that it doesn't skip the next character
			}
			else
			{
				token = checkReserved(TokenBuffer);	//Checks if the token mathces a reserved token first
				if (token == -1)
					token = MINUSOP;
			}

		}
		else if (charIsInt(c))
		{

			long lastPos = 0;

			while (charIsInt(c)) //A number
			{
				lastPos = ftell(InputFilePtr);
				strncat(TokenBuffer, &c, 1); //copy c to token buffer
				c = getc(InputFilePtr);
			}

			token = checkReserved(TokenBuffer); //Checks if the token mathces a reserved token first
			if (token == -1)
				token = INTLITERAL;

			c = '\0'; // So that the listing file doesn't write before a possible new line character
			fseek(InputFilePtr, lastPos, SEEK_SET); // Puts the file pointer back a byte so that it doesn't skip the next character

		}
		else
		{
			strncat(TokenBuffer, &c, 1); //copy c to token buffer

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
				c = getc(InputFilePtr);
				strncat(TokenBuffer, &c, 1); //copy c to token buffer
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
				c = getc(InputFilePtr);
				strncat(TokenBuffer, &c, 1); //copy c to token buffer
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
				c = getc(InputFilePtr);
				strncat(TokenBuffer, &c, 1); //copy c to token buffer
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
				long lastPos = ftell(InputFilePtr);
				c = getc(InputFilePtr);
				if (c != '\n')
					fseek(InputFilePtr, lastPos, SEEK_SET);
				else
					strncat(TokenBuffer, "\n", 1);
			}
			updateListFile(token);
		}

		if (token == ERROR)
			token = -1;

		if (token > -1)
		{
			if (!destructive)
				fseek(InputFilePtr, ogFilePos, SEEK_SET);
			return token;
		}
		else
			TokenBuffer[0] = '\0'; // "Empties" the string
	}
	return SCANEOF;
}


void updateListFile(int token)
{

	if (token == SCANEOF)
	{
		strcpy(TokenBuffer, "EOF");
		fputs("\n", ListFilePtr);
		fputs(ExtraErrBuffer, ListFilePtr);
		ExtraErrBuffer[0] = '\0';
	}
	//Write "tokenBuffer" to listing file ("tokenBuffer" now gets every character copied into to and emptied before the next token)
	if (fputs(TokenBuffer, ListFilePtr))
	{
		printf("\nERROR writing to listing file...");
	}
	// Write the error into a buffer for later writing to the list file
	if (token == ERROR)
	{
		char temp[10];
		itoa(CurLineNum, temp, 10);

		LexicalErrNum++;
		strncat(LexErrBuffer, "Lexical error on line ", ERROR_BUFFER_SIZE - strlen(LexErrBuffer));
		strncat(LexErrBuffer, temp, ERROR_BUFFER_SIZE - strlen(LexErrBuffer));
		strncat(LexErrBuffer, ": Unexpected token '", ERROR_BUFFER_SIZE - strlen(LexErrBuffer));
		strncat(LexErrBuffer, TokenBuffer, ERROR_BUFFER_SIZE - strlen(LexErrBuffer));
		strncat(LexErrBuffer, "'\n", ERROR_BUFFER_SIZE - strlen(LexErrBuffer));
		if (strlen(LexErrBuffer) >= ERROR_BUFFER_SIZE)
			strcpy(LexErrBuffer, "Many unexpected tokens were found!"); // For cases in which the error buffer is too small
	}
	//If reached end of line, print possible errors and new line number to listing file
	if (TokenBuffer[strlen(TokenBuffer) - 1] == '\n')
	{
		fputs(LexErrBuffer, ListFilePtr);
		LexErrBuffer[0] = '\0';

		fputs(ExtraErrBuffer, ListFilePtr);
		ExtraErrBuffer[0] = '\0';

		CurLineNum++;

		int tempSize = (sizeof(char) * 3 + sizeof(char) * (log(CurLineNum) / log(10) + 1));
		// Allocates just enough space for the line number, a period, and a tab (and a NULL terminator) in the form of a character string
		char* temp = malloc(tempSize);

		//Converts the line number to a string and stores it in "listingBuffer"
		itoa(CurLineNum, temp, 10);
		//Puts the period and tab after the line number
		strcat(temp, ".\t");

		temp[tempSize / sizeof(char) - 1] = '\0'; // NULL terminates the string

		// Writes the line number to the listing file
		if (fputs(temp, ListFilePtr) != 0)
		{
			printf("\nERROR writing to listing file...");
		}

		free(temp);
	}
}