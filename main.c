#include <stdio.h>
#include <string.h>
#include "file_util.h"
#include "extra.h"
#include "token.h"

//Scans input file for tokens and returns a token
int scanner(char* buffer, FILE* in_file, FILE* out_file, FILE* list_file); 

//Given enum value of token, and a string, returns string value of token
char * getTokenType(int token, char * str);

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
        inputFilePtr = fileOpen(inputFilePath, "r");
        outputFilePtr = fileOpen(outputFilePath, "w");

        // Defines a temp buffer for holding the outputFilePaths' directory
        char temp[MAX_PATH_SIZE];
        getFileDirectory(temp, outputFilePath);

        // Gives the listing file the same directory as the output file
        changeFileDirectory(reservedFileNames[0], temp);
        listFilePtr = fileOpen(reservedFileNames[0], "w");

        // Gives the temporary file the same directory as the output file
        changeFileDirectory(reservedFileNames[1], temp);
        tempFilePtr = fileOpen(reservedFileNames[1], "w");
    }

    if (invalid == 0 && listFilePtr != NULL && tempFilePtr != NULL)
    {
    	/* -- WORKING ON THE SCANNER --	 */
    	    	
    	char temp[12];
    	char tokenType [20] = {'\0'};		 
    	char outputBuffer [70] = {'\0'}; //To print to output file
		char tokenBuffer[50] = {'\0'}; //To hold tokens from input file		
		int rec_token = 0; //Token received from scanner
		char * errorBuffer[50][300]; //To hold scanning errors
		int lineCount = 0;           
        
    	while (rec_token != SCANEOF)
    	{
    		//Reset token type string and scan for token
    		tokenType[0] = '\0';
    		
    		//Token is received as an integer
    		rec_token = scanner(tokenBuffer, inputFilePtr, outputFilePtr, listFilePtr);
			
			//Increment line counter
			lineCount++; 
			
			//Get token type as string	
    		getTokenType(rec_token, tokenType);
    					   		
    		//Build output buffer: Add "token number: "
			strcpy(outputBuffer, "token number: ");
			
			//Turn token into string and add to output buffer
			sprintf(temp, "%d", rec_token);
			strcat(outputBuffer, temp);
			
			//Add tab and "token type: " to output buffer
			strcat(outputBuffer, "\t");
			strcat(outputBuffer, "token type: ");
			strcat(outputBuffer, tokenType);		
			
			//Add two tabs, "actual token: ", and actual token to output buffer
			strcat(outputBuffer, "\t\t");
			strcat(outputBuffer, "actual token: \n");
			strcat(outputBuffer, tokenBuffer); //actual token will be in token buffer
			
			//Write output buffer to output file
			if (fputs(outputBuffer, outputFilePtr) != 0)
			{
				printf("\nERROR writing to output file...");
			}		
			    		
    	} 
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

//Scans input file for tokens and returns a token
int scanner(char* buffer, FILE* in_file, FILE* out_file, FILE* list_file)
{
	printf("\n\nScanner here...\n\n");
	buffer[0] = '\0'; //Clear token buffer
	int c = '0'; //Initialize variable to read chars from inputfile
	char listingBuffer[300] = {'\0'};
	char tempStr [10] = {'\0'};
		
	//Read each character in input file
	while (c != EOF)
	{	
		//Read character from file
		c = getc(in_file);
		
		//Add char to temp string		
		sprintf(tempStr, "%c", c);	
		
		//Add temp string to listing buffer		
		strcat(listingBuffer, tempStr);			
		printf("%c", c);
		
		/* while (c is alpha) //An identifier - starts with a-zA-Z
		{
			copy c to token buffer
			copy c to listing line
			filepos++ ?
			
			while (next char is alphanumeric) //An identifier as in as23 etc
			{
				copy c to token buffer
				copy c to listing line
				filepos++ ?
				read c
			}
			
			while (c is NOT alphanumeric) //End of identifier
			{
				int myToken = check_reserved(token buffer); //Dr. P supplied routine. Just returns token
				return myToken; //As ID
			}
						
		} //end while (c is alpha)
		
		while (c == '-') //Minus sign
		{
			if (next char == '-') //A comment
			{
				while (c != '\n') //Process comment until end of line
				{
					copy c to listingLine;
					filepos++ ?
				}				
			}
			
			if (next char is numeric) //A negative number
			{
				copy c i.e. '-' to token buffer
				copy c to listingLine
				
				while (c is numeric) //Copy the rest of digits
				{
					copy c token buffer
					copy c to listingLine
					read c
				}
				
				int myToken = check_reserved(token buffer); //Dr. P supplied routine. Just returns token
				return myToken; //As INTLITERAL 
			}
			
			int myToken = check_reserved(token buffer); //Dr. P supplied routine. Just returns token
			return myToken; //As MINUSOP
		}
		
		while (c == numeric) //A number
		{
			copy c to token buffer
			copy c to listing line
			
			if (c is NOT numeric) //End of number
			{
				int myToken = check_reserved(token buffer); //Dr. P supplied routine. Just returns token
				return myToken; //As INTLITERAL
			}
		}
		
		//Check for other tokens
		switch (c)
		{
			case '(':
				return token AS LPAREN;
				
			case ')':
				return token AS RPAREN;
				
			case ';':
				return token AS SEMICOLON;
			
			case ',':
				return token AS COMMA;
			
			case '+':
				return token AS PLUSOP;
			
			case '*':
				return token AS MULTOP;
			
			case '/':
				return token AS DIVOP
			
			case '|':
				return token AS NOTOP;
			
			case '=':
				return token AS EQUALOP;			
			
			//Operators with two characters
			case ':'
				if (next char is '=')
				{
					return token AS ASSIGNOP;
				} 
				else 
				{
					return as ERROR ?
				};			
			
			case '<':
				if (next char is '=')
				{
					return token AS LESSEQUALOP;
				} 
				else if (next char is '>'
				{
					return token as NOTEQUALOP;
				} 
				else
				{
					return token as LESSOP;
				}
			
			case '>':
				if (next char is '=')
				{
					return token AS GREATEREQUALOP;
				} 
				else
				{
					return token as GREATEROP
				};
			
			case 'EOF':
				return token as SCANEOF;
			
			default:
				return token as ERROR;			
		} */
	
		//If reached end of line, print to listing file
		if (c == '\n')
		{						
			//Write listing buffer to listing file
			if (fputs(listingBuffer, list_file) != 0)
			{
				printf("\nERROR writing to listing file...");				 
			}
			
			//Reset listing buffer
			listingBuffer[0] = '\0';
			
		}
			
		if (c == EOF)
		{
			return SCANEOF;
		}
		
				
	}	
	
}

//Given enum value of token, and a string, returns string value of token
char * getTokenType(int token, char * str)
{
	//Receive string
	
	//To upper identifier
	
	//Then check token
	
	switch(token)
	{	
		case 0:
			strcpy(str, "BEGIN");
			break;
		
		case 1:
			strcpy(str, "END");
			break;
		
		case 2:
			strcpy(str, "READ");
			break;
		
		case 3:
			strcpy(str, "WRITE");
			break;
		
		case 4:
			strcpy(str, "IF");
			break;
		
		case 5:
			strcpy(str, "THEN");
			break;
		
		case 6:
			strcpy(str, "ELSE");
			break;
		
		case 7:
			strcpy(str, "ENDIF");
			break;
		
		case 8:
			strcpy(str, "WHILE");
			break;
		
		case 9:
			strcpy(str, "ENDWHILE");
			break;
		
		case 10:
			strcpy(str, "ID");
			break;
		
		case 11:
			strcpy(str, "INTLITERAL");
			break;
		
		case 12:
			strcpy(str, "FALSEOP");
			break;
		
		case 13:
			strcpy(str, "TRUEOP");
			break;
		
		case 14:
			strcpy(str, "NULLOP");
			break;
		
		case 15:
			strcpy(str, "LPAREN");
			break;
		
		case 16:
			strcpy(str, "RPAREN");
			break;
		
		case 17:
			strcpy(str, "SEMICOLON");
			break;
		
		case 18:
			strcpy(str, "COMMA");
			break;
		
		case 19:
			strcpy(str, "ASSIGNOP");
			break;
		
		case 20:
			strcpy(str, "PLUSOP");
			break;
		
		case 21:
			strcpy(str, "MINUSOP");
			break;
		
		case 22:
			strcpy(str, "MULTOP");
			break;
		
		case 23:
			strcpy(str, "DIVOP");
			break;
		
		case 24:
			strcpy(str, "NOTOP");
			break;
		
		case 25:
			strcpy(str, "LESSOP");
			break;
		
		case 26:
			strcpy(str, "LESSEQUALOP");
			break;
		
		case 27:
			strcpy(str, "GREATEROP");
			break;
		
		case 28:
			strcpy(str, "GREATEREQUALOP");
			break;
		
		case 29:
			strcpy(str, "EQUALOP");
			break;
		
		case 30:
			strcpy(str, "NOTEQUALOP");
			break;
		
		case 31:
			strcpy(str, "SCANEOF");
			break;
		
		case 32:
			strcpy(str, "ERROR");					
		
	}
			
			
}

