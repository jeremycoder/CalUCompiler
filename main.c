
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
#include "extra.h"
#include "token.h"

//Scans input file for tokens and returns the next token
int scanner(char* buffer, char* errorBuffer, FILE* in_file, FILE* out_file, FILE* list_file, int* lineNum, int* errorNum);

//Given enum value of token, and a string, returns string value of token
char * getTokenType(int token, char * str);

// Checks if the string stored in buffer matches any of the hardcoded reserved names
// If it matches a reserved name, then the corresponding token enumeration is returned, else -1 is returned
int check_reserved(const char* buffer);

// Gets parameters from the command prompt (if they exist) and stores them in "inputFilePath" and "outputFilePath" respectively
void getCmdParameters(int argc, char** argv, char* inputFilePath, char* outputFilePath);

int main(int argc, char** argv)
{
    char inputFilePath[MAX_PATH_SIZE] = { '\0' };
    char outputFilePath[MAX_PATH_SIZE] = { '\0' };

    // Allocates space for 2 pointers
    // This pointer to an array of pointers is used so that any reserved file names can easily be passed into "getFiles(...)"
    char** reservedFileNames = (char**)calloc(2, sizeof(char*));
    // Allocates space for a reserved file path
    reservedFileNames[0] = (char*)calloc(MAX_PATH_SIZE, sizeof(char));
    // Allocates space for a reserved file path
    reservedFileNames[1] = (char*)calloc(MAX_PATH_SIZE, sizeof(char));

    strcpy(reservedFileNames[0], "list.out");
    strcpy(reservedFileNames[1], "temp.out");

    int invalid = 0;

    FILE* inputFilePtr = NULL;
    FILE* outputFilePtr = NULL;
    FILE* tempFilePtr = NULL;
    FILE* listFilePtr = NULL;

    // Displays our group title
    displayTitle();
    // Get command line parameters if they exist
    getCmdParameters(argc, argv, inputFilePath, outputFilePath);

    // Prompt the user for any missing and/or invalid file paths (returns 0 if the user enters valid file paths)
    // "reservedNames" is explicitly cast as "const char**" because DevCPP complains
    if ((invalid = getFiles(inputFilePath, outputFilePath, 2, (const char**)reservedFileNames)) == 0)
    {
        inputFilePtr = fileOpen(inputFilePath, "rb"); // Must be in binary read mode for fseek to properly function
        outputFilePtr = fileOpen(outputFilePath, "w");

        // Defines a temp buffer for holding the outputFilePaths' directory
        char temp[MAX_PATH_SIZE];
        getFileDirectory(temp, outputFilePath);

        // Gives the listing file the same directory as the output file
        changeFileDirectory(reservedFileNames[0], temp);
        listFilePtr = fileOpen(reservedFileNames[0], "w");

        // Gives the temporary file the same directory as the output file
        changeFileDirectory(reservedFileNames[1], temp);
        tempFilePtr = fileOpen(reservedFileNames[1], "w+");
    }

    if (invalid == 0 && listFilePtr != NULL && tempFilePtr != NULL)
    {
		fputs("The Temp", tempFilePtr);

    	/* -- WORKING ON THE SCANNER --	 */
    	    	
		char tokenBuffer[50] = {'\0'}; //To hold tokens from input file
		int rec_token = 0; //Token received from scanner
		int lineNum = 0; // To keep track of the input file's line number
		int errorNum = 0;
		char errorBuffer[ERROR_BUFFER_SIZE] = { '\0' }; //To hold scanning errors
        
    	while (rec_token != SCANEOF)
    	{
			//Token is received as an integer
			rec_token = scanner(tokenBuffer, errorBuffer, inputFilePtr, outputFilePtr, listFilePtr, &lineNum, &errorNum);

			//Note: Output file writing has been moved to the end of the scanner function
    	}

		printf("\n\nFinished Scanning: %s\n", inputFilePath);

		sprintf(tokenBuffer, "\n\nThere are %d lexical errors.", errorNum); // Resuses the token buffer temporarily because why not
		fputs(tokenBuffer, listFilePtr); // Puts the total number of errors at the end of the list file

		rewind(tempFilePtr);
		copyFileContents(tempFilePtr, outputFilePtr); // In this case copies "The Temp" into the end of the output file (current file position)
    }

    fileClose(inputFilePtr, inputFilePath);
    fileClose(outputFilePtr, outputFilePath);
    fileClose(listFilePtr, reservedFileNames[0]);
    fileClose(tempFilePtr, reservedFileNames[1]);

    //if (fileExists("temp.out"))
        //remove("temp.out");

    // What's allocated must be deallocated!
    free(reservedFileNames[1]);
    free(reservedFileNames[0]);
    free(reservedFileNames);

    return 0;
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

//Scans input file for tokens and returns the next token
//Should be called in a loop until the returned token is SCANEOF
//When a token is returned "buffer" will contain the string form of the token
int scanner(char* buffer, char* errorBuffer, FILE* in_file, FILE* out_file, FILE* list_file, int* lineNum, int* errorNum)
{
	buffer[0] = '\0'; //Clear token buffer
	int token = -1;
	char c = 0; //Initialize variable to read chars from inputfile

	if (*lineNum < 1)
	{
		*lineNum = 1;
		fputs("1.\t", list_file); // Places the first line number into the listing file
	}
		
	//Read each character in input file
	while (c != EOF)
	{
		//Read character from file
		c = getc(in_file);

		if (charIsAlpha(c)) //An identifier - starts with a-zA-Z
		{

			long lastPos = 0;

			while (charIsAlpha(c) || charIsInt(c)) //An identifier as in as23 etc
			{
				
				lastPos = ftell(in_file);
				strncat(buffer, &c, 1); //copy c to token buffer
				c = getc(in_file);
				
			}

			token = check_reserved(buffer); //Checks if the token matches a reserved token first
			if (token == -1)
				token = ID;
			
			c = '\0'; // So that the listing file doesn't write before a possible new line character
			fseek(in_file, lastPos, SEEK_SET); // Puts the file pointer back a byte so that it doesn't skip the next character

		} //end if (c is alpha)
		else if (c == '-') //Minus sign
		{

			strncat(buffer, &c, 1);
			c = getc(in_file);

			if (c == '-') //A comment
			{
				while (c != '\n') //Process comment until end of line
				{

					strncat(buffer, &c, 1); // Comment this line out to not copy the comment to the listing file
					c = getc(in_file);

				}
				//buffer[0] = '\0'; // Uncomment this line to not copy the '-' to the listing file
				strncat(buffer, &c, 1);

			}
			else if (charIsInt(c)) //A negative number
			{

				long lastPos = 0;

				while (charIsInt(c)) //Copy the rest of digits
				{
					lastPos = ftell(in_file);
					strncat(buffer, &c, 1); //copy c to token buffer
					c = getc(in_file); //read c
				}

				token = check_reserved(buffer); //Checks if the token mathces a reserved token first
				if (token == -1)
					token = INTLITERAL;

				c = '\0'; // So that the listing file doesn't write before a possible new line character
				fseek(in_file, lastPos, SEEK_SET); // Puts the file pointer back a byte so that it doesn't skip the next character
			}
			else
			{
				token = check_reserved(buffer);	//Checks if the token mathces a reserved token first
				if (token == -1)
					token = MINUSOP;
			}

		}
		else if (charIsInt(c))
		{

			long lastPos = 0;

			while (charIsInt(c)) //A number
			{
				lastPos = ftell(in_file);
				strncat(buffer, &c, 1); //copy c to token buffer
				c = getc(in_file);
			}

			token = check_reserved(buffer); //Checks if the token mathces a reserved token first
			if (token == -1)
				token = INTLITERAL;

			c = '\0'; // So that the listing file doesn't write before a possible new line character
			fseek(in_file, lastPos, SEEK_SET); // Puts the file pointer back a byte so that it doesn't skip the next character

		}
		else
		{
			strncat(buffer, &c, 1); //copy c to token buffer

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
				case '|':
					token = NOTOP;
					break;
				case '=':
					token = EQUALOP;
					break;
				//Operators with two characters
				case ':':
					c = getc(in_file);
					strncat(buffer, &c, 1); //copy c to token buffer
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
					c = getc(in_file);
					strncat(buffer, &c, 1); //copy c to token buffer
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
					c = getc(in_file);
					strncat(buffer, &c, 1); //copy c to token buffer
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

		if (token == SCANEOF)
		{
			buffer = "EOF";
		}
		else
		{
			// --------------------START OF LISTING FILE WRITING----------------------

			//Write "buffer" to listing file ("buffer" now gets every character copied into to and emptied before the next token)
			if (fputs(buffer, list_file) != 0)
			{
				printf("\nERROR writing to listing file...");
			}

			// Write the error into a buffer for later writing to the list file
			if (token == ERROR)
			{

				(*errorNum)++;
				strncat(errorBuffer, "Unexpected token '", ERROR_BUFFER_SIZE - strlen(errorBuffer));
				strncat(errorBuffer, buffer, ERROR_BUFFER_SIZE - strlen(errorBuffer));
				strncat(errorBuffer, "'\n", ERROR_BUFFER_SIZE - strlen(errorBuffer));
				if (strlen(errorBuffer) >= ERROR_BUFFER_SIZE)
					strcpy(errorBuffer, "Many unexpected tokens were found!"); // For cases in which the error buffer is too small

			}

			//If reached end of line, print possible errors and new line number to listing file
			if (c == '\n')
			{

				fputs(errorBuffer, list_file);
				errorBuffer[0] = '\0';

				(*lineNum)++;

				int tempSize = (sizeof(char) * 3 + sizeof(char) * (log(*lineNum) / log(10) + 1));
				// Allocates just enough space for the line number, a period, and a tab (and a NULL terminator) in the form of a character string
				char* temp = malloc(tempSize);

				//Converts the line number to a string and stores it in "listingBuffer"
				itoa(*lineNum, temp, 10);
				//Puts the period and tab after the line number
				strcat(temp, ".\t");

				temp[tempSize / sizeof(char) - 1] = '\0'; // NULL terminates the string

				// Writes the line number to the listing file
				if (fputs(temp, list_file) != 0)
				{
					printf("\nERROR writing to listing file...");
				}

				free(temp);

			}
			// --------------------END OF LISTING FILE WRITING----------------------
		}
		
		if (token != -1)
		{

			// --------------------START OF OUTPUT FILE WRITING----------------------

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
			strcat(outputBuffer, buffer); //actual token will be in token buffer
			strcat(outputBuffer, "\n");

			//Write output buffer to output file
			if (fputs(outputBuffer, out_file) != 0)
			{
				printf("\nERROR writing to output file...");
			}

			// --------------------END OF OUTPUT FILE WRITING----------------------

			return token;
		}
		else
			buffer[0] = '\0'; // "Empties" the string
	}
	return SCANEOF;
}

// Checks if the string stored in buffer matches any of the hardcoded reserved names
// If it matches a reserved name, then the corresponding token enumeration is returned, else -1 is returned
int check_reserved(const char* buffer)
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
