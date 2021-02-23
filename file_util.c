#include "file_util.h"

// Returns 1 if the file extension in "filename" exists
// Returns NULL if the file extension in "filename" does not exist
int hasFileExtension(const char* filename)
{
    int returnVal = 0;

    if (filename != NULL)
    {
        if (strstr(filename, "."))
        {
            if (strlen(filename) - strcspn(filename, ".") > 1)
            {
                returnVal = 1;
            }
        }
    }

    return returnVal;
}

// Returns 1 if there is a file extension and copies it into "dest"
// Returns 0 if there is no file extension
int getFileExtension(char* dest, const char* filename)
{
    int returnVal = 0;

    if (hasFileExtension(filename))
    {
        size_t extensionSize = strlen(filename) - strcspn(filename, ".");
        // It's ugly, but it works
        memcpy(dest, filename + sizeof(char) * (strlen(filename) - extensionSize), extensionSize);
        dest[extensionSize] = '\0';
        returnVal = 1;
    }

    return returnVal;
}

// Returns 0 if the file does not exist
// Returns 1 if the file does exist
int fileExists(const char* filename)
{
    int returnVal = 0;

    FILE* theFile = fopen(filename, "r");
    if (theFile != NULL)
    {
        fclose(theFile);
        returnVal = 1;
    }

    return returnVal;
}

// Returns a file pointer to a file if it was successfully opened and displays a message
// If it fails to open an error message is displayed
FILE* fileOpen(const char* filename, const char* mode)
{
    FILE* file = fopen(filename, mode);

    if (file == NULL)
    {
        printf("\nFailed to open %s", filename);
        perror("fopen()");
    }
    else
    {
        printf("\nSuccessfully opened %s", filename);
    }

    return file;
}

// Checks if the filePtr is pointing to anything and closes the file while displaying a close message
void fileClose(FILE* filePtr, char* filename)
{
    if (filePtr != NULL)
    {
        printf("\nClosing %s", filename);
        fclose(filePtr);
    }
}

// Copies the data in "inputFile" into "outputFile"
// Returns 1 if either "inputFile" or "outputFile" is NULL
// Otherwise returns 0
int copyFileContents(FILE* inputFile, FILE* outputFile)
{
    int returnVal = 0;

    if (inputFile && outputFile)
    {
        char currentChar = fgetc(inputFile);

        while (currentChar != EOF)
        {
            fputc(currentChar, outputFile);
            currentChar = fgetc(inputFile);
        }
        rewind(inputFile);
    }
    else
        returnVal = 1;

    return returnVal;
}

// Forces stdin to point at either a new line or end of file character
void flushInputs()
{
    char currentChar = getchar();
    while (currentChar != '\n' && currentChar != EOF)
    {
        currentChar = getchar();
    }
}

// Gets up to "maxLength" characters from stdin and copies them into "dest"
// Places a NULL character in "dest" where it stopped reading from stdin
// It is recommended that "maxLength" be equal to the size of the buffer in "dest" minus one
void getLine(char* dest, const int maxLength)
{
    int i;
    for (i = 0; i < maxLength; i++)
    {
        dest[i] = getchar();
        if (dest[i] == '\n' || dest[i] == EOF)
        {
            dest[i] = '\0';
            i = maxLength;
        }
    }
    // Null terminates just in case a newline or EOF character was never reached
    dest[maxLength] = '\0';
}

// Copies the file directory in "filename" into "dest"
// Returns 1 if the filename contains a directory
// Returns 0 if the directory is not in the filename
int getFileDirectory(char* dest, const char* filename)
{
    int returnVal = 0;

    if (filename != NULL)
    {
        if (strchr(filename, '\\'))
        {
            strncpy(dest, filename, strlen(filename) - strlen(strrchr(filename, '\\')) + 1);
            if (dest != NULL)
            {
                dest[strlen(filename) - strlen(strrchr(filename, '\\')) + 1] = '\0';
                returnVal = 1;
            }
        }
    }

    return returnVal;
}

// Copies the filename of "filename" with its extension but without its directory into "dest"
// Returns 1 if the filename contains a directory
// Returns 0 if the directory is not in the filename
int getFileWithoutDirectory(char* dest, const char* filename)
{
    int returnVal = 0;

    if (filename != NULL)
    {
        if (strchr(filename, '\\'))
        {
            getFileDirectory(dest, filename);
            int directoryLength = strlen(dest);
            unsigned int i;
            for (i = directoryLength; i < strlen(filename); i++)
                dest[i - directoryLength] = filename[i];
            dest[strlen(filename) - directoryLength] = '\0';
            if (dest != NULL)
            {
                returnVal = 1;
            }
        }
        else
            strcpy(dest, filename);
    }

    return returnVal;
}

// Copies the file name of "filename" without its extension of into "dest"
void getFileWithoutExtension(char* dest, const char* filename)
{
    if (hasFileExtension(filename))
    {
        memcpy(dest, filename, strcspn(filename, "."));
    }
    else
        strcpy(dest, filename);
}

// Changes the current file extension
// If no extension exists then the extension is simply added
// Returns 0 if the extension is added
// Returns 1 if the filename is empty
// Returns -1 if the extension is invalid
int changeFileExtension(char* filename, const char* extension)
{
    int result = 0;

    if (extension != NULL && extension[0] == '.')
    {
        if (filename != NULL)
        {
            if (strchr(filename, '.'))
            {
                // Places a NULL character where the '.' is
                filename[strlen(filename) - strlen(strchr(filename, '.'))] = '\0';
            }
            strcat(filename, extension);
            result = 0;
        }
        else
            result = 1;
    }
    else
        result = -1;

    return result;
}

// Changes the current file directory in "filename"
// If no extension exists then the extension is simply added
// Returns 0 if the extension is added
// Returns 1 if the filename is empty
// Returns -1 if the extension is invalid
int changeFileDirectory(char* filename, const char* directory)
{
    int result = 0;

    if (directory != NULL && strchr(directory, '\\'))
    {
        if (filename != NULL)
        {
            getFileWithoutDirectory(filename, filename);

            int dirLength = strlen(directory);
            int filenameLength = strlen(filename);

            int i;
            // Copies "filename" to where it will be after the new directory
            for (i = 0; i < filenameLength; i++)
            {
                filename[i + dirLength] = filename[i];
            }
            // Copies the directory into "filename"
            for (i = 0; i < dirLength; i++)
            {
                filename[i] = directory[i];
            }
            result = 0;
        }
        else
            result = 1;
    }
    else
        result = -1;

    return result;
}

// Prompts the user for input and output file paths and opens the files in their respective modes
// Returns 0 if the files were successfully opened, else returns 1
int getFiles(char* inputFilePath, char* outputFilePath, const int resNameCount, const char** reservedNames)
{
    int invalid = 0;

    invalid = getInputFile(inputFilePath);

    if (!invalid)
        invalid = getOutputFile(outputFilePath, inputFilePath, resNameCount, reservedNames);

    return invalid;
}

// Prompts the user for an input file name until either an existing file's path is entered or the user does not enter anything
// Returns 0 when the user enters a valid input file path else returns 1
// Does not allow the user to use the ".out" extension
int getInputFile(char* filePath)
{
    int gettingFileName = 1;
    int invalid = 0;

    char tempBuff[MAX_PATH_SIZE] = { "\0" };

    // Prompts the user for the input file name if it was not included as a parameter
    if (filePath[0] == '\0')
    {
        printf("\nPlease enter the name of the input file: ");
        getLine(filePath, MAX_PATH_SIZE - 1);
    }

    while (gettingFileName)
    {
        if (filePath[0] == '\0')
        {
            gettingFileName = 0;
            invalid = 1;
            printf("\nNo input file was entered.");
        }
        else
        {
            getFileExtension(tempBuff, filePath);
            if (!strcmp(tempBuff, ".out"))
            {
                printf("\nInvalid file extension for input file.");
                filePath[0] = '\0';
            }
            // Opens the input file if it exists
            if (fileExists(filePath))
            {
                gettingFileName = 0;
            }
            // Checks if there was anything entered for the input filename
            else
            {
                printf("\nInput file not found!");
                printf("\nPlease enter the name of the input file: ");
                getLine(filePath, MAX_PATH_SIZE - 1);
            }
        }
    }

    return invalid;
}

// Prompts the user for an output file name until either a valid file path is entered or the user does not enter anything
// Returns 0 when the user enters a valid output file path else returns 1
// Does not allow the user to use an output filename that matches any string in "reservedNames"
int getOutputFile(char* filePath, const char* defaultPath, const int resNameCount, const char** reservedNames)
{
    int gettingFileName = 1;
    int invalid = 0;
    char decision[2] = { "\0" };
    char tempBuff[MAX_PATH_SIZE] = { "\0" };

    //Prompts the user for the ouptut file name if it was not included as a parameter
    if (filePath[0] == '\0')
    {
        printf("\nPlease enter a name for the output file: ");
        getLine(filePath, MAX_PATH_SIZE - 1);
    }

    while (gettingFileName)
    {
        // Checks if there was anything entered for the output filename
        if (filePath[0] == '\0')
        {
            strcpy(filePath, "output.out");
            getFileDirectory(tempBuff, defaultPath);
            changeFileDirectory(filePath, tempBuff);
            printf("\nNo output file name was entered. The name was defaulted to %s", filePath);
        }
        // Checks if there was anything entered and adds an extension if there was a file name entered
        if (!hasFileExtension(filePath))
        {
            printf("\nOutput file is missing an extension.\n");
            changeFileExtension(filePath, ".out");
            printf("The output file is now %s\n", filePath);
        }

        getFileWithoutDirectory(tempBuff, filePath);
        int i;
        // Ensures that "filePath"'s name does not match any of the reserved names
        for (i = 0; i < resNameCount; i++)
        {
            if (!strcmp(tempBuff, reservedNames[i]))
            {
                filePath[0] = '\0';
                printf("\nThe name \"%s\" cannot be used!\n", reservedNames[i]);
                i = resNameCount;
                invalid = 1;
                gettingFileName = 0;
            }
        }
        // Checks if an output file of that name already exists
        if (fileExists(filePath) == 0)
        {
            // A valid name was found
            gettingFileName = 0;
        }
        // A file with that name already exists
        else
        {
            printf("\nA file with the name %s already exists.\n", filePath);
            printf("\nType 'o' to back it up and then overwrite, 'n' to type a new name, or 'q' to quit: ");
            getLine(decision, 1);

            // If nothing was entered then the input file's name is used for the output file
            if (decision[0] == '\0')
            {
                strcpy(filePath, defaultPath);
                changeFileExtension(filePath, ".out");
                printf("\nA decision was not entered. Defaulting the name to %s\n", filePath);
            }
            // Checks if the user wants to enter a new filename
            else if (decision[0] == 'n')
            {
                printf("\nPlease enter a name for the output file: ");
                flushInputs();
                getLine(filePath, MAX_PATH_SIZE - 1);
            }
            // Checks if the user wants to overwrite the output file
            else if (decision[0] == 'o')
            {
                char backFile[MAX_PATH_SIZE];
                strcpy(backFile, filePath);
                changeFileExtension(backFile, ".bak");
                // Checks if a backup file with that name already exists
                if (fileExists(backFile))
                {
                    printf("\nA file with the name %s already exists.\n", backFile);
                    printf("\nType 'o' to overwrite the backup file, 'q' to quit, or enter nothing to type a new output filename: ");
                    decision[0] = '\0';
                    flushInputs();
                    getLine(decision, 1);
                }
                // Checks if the user still wants to copy to to the backup file
                if (decision[0] == 'o')
                {
                    FILE* backupFilePtr = fopen(backFile, "w");
                    FILE* outputFilePtr = fopen(filePath, "r");

                    copyFileContents(outputFilePtr, backupFilePtr);
                    printf("\nBacked up %s to %s\n", filePath, backFile);

                    fclose(backupFilePtr);
                    fclose(outputFilePtr);

                    gettingFileName = 0;
                }
                // Checks if the user wants to enter a new filename when the last file already has a backup
                else if (decision[0] == '\0')
                {
                    printf("\nPlease enter a name for the output file: ");
                    getLine(filePath, MAX_PATH_SIZE - 1);
                }
            }

            // Catches a quit decision
            if (decision[0] == 'q')
            {
                invalid = 1;
                gettingFileName = 0;
            }
        }
    }

    return invalid;
}
