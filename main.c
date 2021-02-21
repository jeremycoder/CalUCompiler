#include <stdio.h>
#include <string.h>
#include "file_util.h"
#include "extra.h"

int scanner(char* buffer, FILE* in_file, FILE* out_file, FILE* list_file); //Scans for tokens.

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
    	// char * tokenBuffer[50]; //Holds tokens received from scanner
        // Do scanner stuff        
        printf("\nStarting scanner...\n");        
        
        //Algorithm for main.c
        /*
        	infilePtr = OpenFile(inputfile); //Open input file and get input file pointer
			outfilePtr = OpenFile(outputfile); //Open output file and get output file pointer
			listfilePtr = OpenFile(listingfile); //Open listing file and get listing file pointer
			//The operations above should/could be done in a routine called start_up	
        	
        	while (rec_token != SCANEOF) //Check token received from scanner
        	{
        		rec_token = scanner(); //Scanner returns token
        		strcpy(tokenBuffer, rec_token); //Copy received token to tokenBuffer
        		Write tokenBuffer to outputfile        		
        	}
        	
        	close all open files; //Well files are closed below        	
        
        */


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

