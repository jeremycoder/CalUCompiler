#include "file_util.h"

// Returns 1 if the file extension in "filename" exists
// Returns 0 if the file extension in "filename" does not exist
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

    return file;
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
// Returns 0 if the directory was changed
// Returns 1 if the filename is empty
// Returns -1 if the directory is invalid
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

// Prompts the user for an input file name until either an existing file's path is entered or the user does not enter anything
// Returns 0 when the user enters a valid input file path else returns 1
int getInputFile(char* filePath, const char** restrictedExtensions, const int resExtCount)
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
            if (hasFileExtension(filePath) == 0)
            {
                changeFileExtension(filePath, ".in");
                printf("\nNo extension was entered for the input file. Searching for \"%s\"\n", filePath);
            }
            getFileExtension(tempBuff, filePath);
            int i;
            // Ensures that "filePath"'s extension does not match any of the restricted extensions
            for (i = 0; i < resExtCount; i++)
            {
                if (!strcmp(tempBuff, restrictedExtensions[i]))
                {
                    filePath[0] = '\0';
                    printf("\nThe extension \"%s\" cannot be used!\n", restrictedExtensions[i]);
                    i = resExtCount; // To break out of the loop
                }
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
    printf("\n");

    return invalid;
}

// Prompts the user for an output file name until either a valid file path is entered or the user does not enter anything
// Returns 0 when the user enters a valid output file path else returns 1
// Does not allow the user to use an output filename that matches any string in "reservedNames"
int getOutputFile(char* filePath, const char* defaultDir, const char** restrictedExtensions, const int resExtCount)
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
            strcpy(filePath, defaultDir);
            changeFileExtension(filePath, ".c");
            printf("\nNo output file name was entered. The name was defaulted to %s\n", filePath);
        }
        // Checks if there was anything entered and adds an extension if there was a file name entered
        if (!hasFileExtension(filePath))
        {
            printf("\nOutput file is missing an extension.\n");
            changeFileExtension(filePath, ".c");
            printf("The output file has been defaulted to %s\n", filePath);
        }

        getFileExtension(tempBuff, filePath);
        int i;

        // Ensures that "filePath"'s extension does not match any of the restricted extensions
        for (i = 0; i < resExtCount; i++)
        {
            if (!strcmp(tempBuff, restrictedExtensions[i]))
            {
                filePath[0] = '\0';
                printf("\nThe extension \"%s\" cannot be used!\n", restrictedExtensions[i]);
                i = resExtCount; // To break out of the loop
            }
        }
        if (filePath[0] == '\0')
        {
            printf("\nPlease enter a name for the output file: ");
            getLine(filePath, MAX_PATH_SIZE - 1);
        }
        else
        {
            // Checks if an output file of that name already exists
            if (!fileExists(filePath))
            {
                // A valid name was found
                gettingFileName = 0;
            }
            // A file with that name already exists
            else
            {
                printf("A file with the name %s already exists.\n", filePath);
                printf("\nType 'o' to back it up first, 'n' to type a new name, or 'q' to quit: ");
                getLine(decision, 1);

                // If nothing was entered then the input file's name is used for the output file
                if (decision[0] == '\0')
                {
                    strcpy(filePath, defaultDir);
                    changeFileExtension(filePath, ".c");
                    printf("\nA decision was not entered. Defaulting the file to %s\n", filePath);
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
                        printf("A file with the name %s already exists.\n", backFile);
                        printf("Type 'o' to overwrite the backup file, 'q' to quit, or enter nothing to type a new output filename: ");
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
                        printf("\nBacked up %s to %s", filePath, backFile);

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
    }

    return invalid;
}
