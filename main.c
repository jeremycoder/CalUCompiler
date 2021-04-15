
/*
	Created by: Group 5- Jeremy Mwangelwa, Nathaniel DeHart, Matt Oblock, George Brown
	Class: CSC 460 Language Translation
	Program 2: Scanner
	Contacts: mwa2711@calu.edu, deh5850@calu.edu, obl2109@calu.edu, bro8079@calu.edu
*/

#include <string.h>
#include "file_util.h"
#include "parser.h"

FILE* InputFilePtr;
FILE* OutputFilePtr;
FILE* ListFilePtr;
FILE* TempFilePtr;

// Returns 1 if start was successful
// Returns 0 if there were problems opening files
int start(char* inputFilePath, char* outputFilePath, char* listFilePath, char* tempFilePath);

void end(char* inputFilePath, char* outputFilePath, char* listFilePath, char* tempFilePath);

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
		compile(InputFilePtr, OutputFilePtr, ListFilePtr, TempFilePtr);

		end(inputFilePath, outputFilePath, listFilePath, tempFilePath);
	}

	return 0;
}

// Attempts to open all of the needed files
// Returns 1 if start was successful
// Returns 0 if there were problems opening files
int start(char* inputFilePath, char* outputFilePath, char* listFilePath, char* tempFilePath)
{
	int returnVal;

	char** restrictExtsn = calloc(3, sizeof(char*));
	restrictExtsn[0] = calloc(1, FILENAME_MAX);
	restrictExtsn[1] = calloc(1, FILENAME_MAX);
	restrictExtsn[2] = calloc(1, FILENAME_MAX);

	char temp[FILENAME_MAX];

	strcpy(restrictExtsn[0], ".lis");
	strcpy(restrictExtsn[1], ".tmp");
	strcpy(restrictExtsn[2], ".out");

	// Displays our group title
	printf("\n-----------------------------------\n");
	printf("------GROUP 5: PARSER PROGRAM------\n");
	printf("-----------------------------------\n");
	printf("\n");

	returnVal = getInputFile(inputFilePath, (const char**)restrictExtsn, 2);
	if (returnVal == 0)
	{
		strcpy(restrictExtsn[1], ".in");
		returnVal = getOutputFile(outputFilePath, inputFilePath, (const char**)restrictExtsn, 2);
	}
	// Checks if still valid
	if (returnVal == 0)
	{
		strcpy(listFilePath, outputFilePath);
		changeFileExtension(listFilePath, ".lis");
		strcpy(tempFilePath, outputFilePath);
		changeFileExtension(tempFilePath, ".tmp");

		InputFilePtr = fileOpen(inputFilePath, "rb");
		OutputFilePtr = fileOpen(outputFilePath, "w");
		ListFilePtr = fileOpen(listFilePath, "w");
		TempFilePtr = fileOpen(tempFilePath, "w+");

		if (InputFilePtr == NULL || OutputFilePtr == NULL || ListFilePtr == NULL || TempFilePtr == NULL)
			returnVal = 1;
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
	char tempBuffer[50];
	int lexErrors = getTotalLexErrors();
	int synErrors = getTotalSynErrors();

	sprintf(tempBuffer, "\n\nThere are %d lexical errors.", getTotalLexErrors()); // Resuses the token buffer temporarily because why not
	fputs(tempBuffer, ListFilePtr); // Puts the total number of errors at the end of the list file

	sprintf(tempBuffer, "\nThere are %d syntax errors.", getTotalSynErrors());
	fputs(tempBuffer, ListFilePtr);

	if (getParserErrorState())
		sprintf(tempBuffer, "\nThe program did not finish compilation.");
	else
	{
		if (synErrors != 0 || lexErrors != 0)
			sprintf(tempBuffer, "\nThe program compiled with a total of %d errors.", (lexErrors + synErrors));
	}
	fputs(tempBuffer, ListFilePtr);

	rewind(TempFilePtr);
	copyFileContents(TempFilePtr, OutputFilePtr); // In this case copies "The Temp" into the end of the output file (current file position)

	fileClose(InputFilePtr, inputFilePath);
	fileClose(OutputFilePtr, outputFilePath);
	fileClose(ListFilePtr, listFilePath);
	fileClose(TempFilePtr, tempFilePath);

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
