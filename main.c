
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

FILE* inputFilePtr;
FILE* outputFilePtr;
FILE* listFilePtr;
FILE* tempFilePtr;

int lexicalErrNum;
int curLineNum;

//To hold scanning errors
char errorBuffer[ERROR_BUFFER_SIZE] = { '\0' };
//To hold tokens from input file
char tokenBuffer[50] = { '\0' };

void systemGoal();

// Returns the next token from the scanner
int nextToken();

// Gets the next token from the scanner and checks if it is equal to "token"
// Returns 1 if it is equal
// Returns 0 if it is not equal
int match(int token);

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

	int invalid;

	// Get command line parameters if they exist
	getCmdParameters(argc, argv, inputFilePath, outputFilePath);

    if (start(inputFilePath, outputFilePath, listFilePath, tempFilePath) == 0)
    { 	



		// START OF OLD SCANNER PROGRAM STUFF
		int token = -1; // REMOVE THIS
		while (token != SCANEOF) // REMOVE THIS 
		{ // REMOVE THIS
			token = scanner(1); // REMOVE THIS
			updateOutputFile(token); // REMOVE THIS
		} // REMOVE THIS
		// END OF OLD SCANNER PROGRAM STUFF



		//systemGoal(); // UNCOMMENT THIS

		end(inputFilePath, outputFilePath, listFilePath, tempFilePath);
    }

    return 0;
}

void systemGoal()
{
	if (program())
	{
		if (match(SCANEOF))
		{
			// Everything is good
		}
	}
	else
	{
		// There was an error in the program
	}
}

int program()
{
	int returnVal;

	
}

// Gets the next token from the scanner and checks if it is equal to "token"
// Returns 1 if it is equal
// Returns 0 if it is not equal
int match(int token)
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

	char restrictExtsn[2][FILENAME_MAX];
	char temp[FILENAME_MAX];

	strcpy(restrictExtsn[0], ".lis");
	strcpy(restrictExtsn[1], ".out");

	// Displays our group title
	printf("\n-----------------------------------\n");
	printf("------GROUP 5: PARSER PROGRAM------\n");
	printf("-----------------------------------\n");
	printf("\n");

	returnVal = getInputFile(inputFilePath, restrictExtsn, 2);
	if (returnVal == 0)
	{
		returnVal = getOutputFile(outputFilePath, inputFilePath, restrictExtsn, 1);
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

	return returnVal;
}

// Prints the total number of lexical errors at the end of the listing file
// Closes all of the files
void end(char* inputFilePath, char* outputFilePath, char* listFilePath, char* tempFilePath)
{
	sprintf(tokenBuffer, "\n\nThere are %d lexical errors.", lexicalErrNum); // Resuses the token buffer temporarily because why not
	fputs(tokenBuffer, listFilePtr); // Puts the total number of errors at the end of the list file

	rewind(tempFilePtr);
	copyFileContents(tempFilePtr, outputFilePtr); // In this case copies "The Temp" into the end of the output file (current file position)

	fileClose(inputFilePtr, inputFilePath);
	fileClose(outputFilePtr, outputFilePath);
	fileClose(listFilePtr, listFilePath);
	fileClose(tempFilePtr, tempFilePath);

	//if (fileExists("temp.out"))
		//remove("temp.out");
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
				token = NEWLINE;

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
					token = NEWLINE; // For easier use with knowing when to print to the listing file
					break;
				default:
					token = ERROR;
			}
		}

		// Updates the List file (including spaces)
		if (destructive)
			updateListFile(token);

		if (token != -1)
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
	}
	//Write "buffer" to listing file ("buffer" now gets every character copied into to and emptied before the next token)
	else if (fputs(tokenBuffer, listFilePtr))
	{
		printf("\nERROR writing to listing file...");
	}
	// Write the error into a buffer for later writing to the list file
	if (token == ERROR)
	{
		lexicalErrNum++;
		strncat(errorBuffer, "Unexpected token '", ERROR_BUFFER_SIZE - strlen(errorBuffer));
		strncat(errorBuffer, tokenBuffer, ERROR_BUFFER_SIZE - strlen(errorBuffer));
		strncat(errorBuffer, "'\n", ERROR_BUFFER_SIZE - strlen(errorBuffer));
		if (strlen(errorBuffer) >= ERROR_BUFFER_SIZE)
			strcpy(errorBuffer, "Many unexpected tokens were found!"); // For cases in which the error buffer is too small
	}
	//If reached end of line, print possible errors and new line number to listing file
	else if (token == NEWLINE)
	{
		fputs(errorBuffer, listFilePtr);
		errorBuffer[0] = '\0';

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
	// Ensures the token is valid (NEWLINE isn't actually considered part of the language)
	if (token > -1 && token != NEWLINE)
	{
		char tokenType[20] = { '\0' };
		char temp[12];
		char outputBuffer[70] = { '\0' }; //To print to output file

		//Get token type as string	
		getTokenType(token, tokenType);

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
		
	}
			
	return str;
}
