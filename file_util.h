#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// The max path size that Windows supports by default
#define MAX_PATH_SIZE 260

// Returns 1 if the file extension in "filename" exists
// Returns NULL if the file extension in "filename" does not exist
int hasFileExtension(const char* filename);

// Returns 1 if there is a file extension and copies it into "dest"
// Returns 0 if there is no file extension
int getFileExtension(char* dest, const char* filename);

// Returns 0 if the file does not exist
// Returns 1 if the file does exist
int fileExists(const char* filename);

// Returns a file pointer to a file if it was successfully opened and displays a message
// If it fails to open an error message is displayed
FILE* fileOpen(const char* filename, const char* mode);

// Checks if the filePtr is pointing to anything and closes the file while displaying a close message
void fileClose(FILE* filePtr, char* filename);

// Copies the data in "inputFile" into "outputFile"
// Returns 1 if either "inputFile" or "outputFile" is NULL
// Otherwise returns 0
int copyFileContents(FILE* inputFile, FILE* outputFile);

// Forces stdin to point at either a new line or end of file character
void flushInputs();

// Gets up to "maxLength" characters from stdin and copies them into "dest"
// Places a NULL character in "dest" where it stopped reading from stdin
// It is recommended that "maxLength" be equal to the size of the buffer in "dest" minus one
void getLine(char* dest, const int maxLength);

// Copies the file directory in "filename" into "dest"
// Returns 1 if the filename contains a directory
// Returns 0 if the directory is not in the filename
int getFileDirectory(char* dest, const char* filename);

// Copies the filename of "filename" with its extension but without its directory into "dest"
// Returns 1 if the filename contains a directory
// Returns 0 if the directory is not in the filename
int getFileWithoutDirectory(char* dest, const char* filename);

// Copies the file name of "filename" without its extension of into "dest"
void getFileWithoutExtension(char* dest, const char* filename);

// Changes the current file extension
// If no extension exists then the extension is simply added
// Returns 0 if the extension is added
// Returns 1 if the filename is empty
// Returns -1 if the extension is invalid
int changeFileExtension(char* filename, const char* extension);

// Changes the current file directory in "filename"
// If no extension exists then the extension is simply added
// Returns 0 if the directory was changed
// Returns 1 if the filename is empty
// Returns -1 if the directory is invalid
int changeFileDirectory(char* filename, const char* directory);

// Prompts the user for an input file name until either an existing file's path is entered or the user does not enter anything
// Returns 0 when the user enters a valid input file path else returns 1
int getInputFile(char* filePath, const char restrictedExtensions[][FILENAME_MAX], const int resExtCount);

// Prompts the user for an output file name until either a valid file path is entered or the user does not enter anything
// Returns 0 when the user enters a valid output file path else returns 1
// Does not allow the user to use an output filename that matches any string in "reservedNames"
int getOutputFile(char* filePath, const char* defaultDir, const char restrictedExtensions[][FILENAME_MAX], const int resExtCount);

#include "file_util.c"

#endif
