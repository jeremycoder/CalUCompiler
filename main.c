#include <stdio.h>
#include <string.h>
#include "file_util.h"
#include "extra.h"

int scanner(char* buffer, FILE* in_file, FILE* out_file, FILE * list_file); //Scans for tokens. Check "tokens.h"

void getCmdParameters(int argc, char** argv, char* inputFilePath, char* outputFilePath);

int main(int argc, char** argv) 
{
    char inputFilePath[MAX_PATH_SIZE] = { '\0' };
    char outputFilePath[MAX_PATH_SIZE] = { '\0' };
    char** reservedNames = (char**)malloc(MAX_PATH_SIZE * sizeof(char) * 2);
    
    reservedNames[0] = "list.out";
    reservedNames[1] = "temp.out";

    int invalid = 0;

    FILE* inputFilePtr = NULL;
    FILE* outputFilePtr = NULL;
    FILE* tempFilePtr = NULL;
    FILE* listFilePtr = NULL;

    displayTitle();
    getCmdParameters(argc, argv, inputFilePath, outputFilePath);
    if (invalid = getFiles(inputFilePath, outputFilePath, 2, (const char**)reservedNames) != 1)
    {
        inputFilePtr = fileOpen(inputFilePath, "r");
        outputFilePtr = fileOpen(outputFilePath, "w");
        char temp[MAX_PATH_SIZE];
        getFileDirectory(temp, outputFilePath);
        changeFileDirectory(reservedNames[0], temp);
        listFilePtr = fileOpen(reservedNames[0], "w");
        changeFileDirectory(reservedNames[1], temp);
        tempFilePtr = fileOpen(reservedNames[1], "w");
        printf("\n");
    }

    if (!invalid && listFilePtr != NULL && tempFilePtr != NULL)
    {
        // Do scanner stuff        
        printf("\nSCANNER STUFF HERE!");
        
        
    }
    
    printf("\nSCANNER STUFF HERE!");
    
    fileClose(inputFilePtr, inputFilePath);
    fileClose(outputFilePtr, outputFilePath);
    fileClose(listFilePtr, reservedNames[0]);
    fileClose(tempFilePtr, reservedNames[1]);

    //if (fileExists("temp.out"))
        //remove("temp.out");

    free(reservedNames);

    return 0;
}

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

