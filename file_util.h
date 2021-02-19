#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PATH_SIZE 260

int hasFileExtension(const char* filename);

int getFileExtension(char* dest, const char* filename);

int fileExists(const char* filename);

FILE* fileOpen(const char* filename, const char* mode);

int fileClose(FILE* filePtr, char* filename);

int copyFileContents(FILE* inputFile, FILE* outputFile);

// Forces stdin to point at either a new line or end of file character
int flushInputs();

// Gets up to "maxLength" characters from stdin and copies them into "dest"
// Places a NULL character in "dest" where it stopped reading from stdin
// It is recommended that "maxLength" be equal to the size of the buffer in "dest" minus one
void getLine(char* dest, const int maxLength);

// Returns 1 if the filename contains a directory
// Returns 0 if the directory is not in the filename
int getFileDirectory(char* dest, const char* filename);

int getFileNameWithoutDirectory(char* dest, const char* filename);

void getFileNameWithoutExtension(char* dest, const char* filename);

// Changes the current file extension
// If no extension exists then the extension is simply added
// Returns 0 if the extension is added
// Returns 1 if the filename is empty
// Returns -1 if the extension is invalid
int changeFileExtension(char* filename, const char* extension);

// Changes the current file directory
// If no extension exists then the extension is simply added
// Returns 0 if the extension is added
// Returns 1 if the filename is empty
// Returns -1 if the extension is invalid
int changeFileDirectory(char* filename, const char* directory);

// Prompts the user for input and output file paths and opens the files in their respective modes
// Returns 0 if the files were successfully opened, else returns 1
int getFiles(char* inputFilePath, char* outputFilePath, const int resNameCount, const char** reservedNames);

// Prompts the user for an input file name until either an existing file's path is entered or the user does not enter anything
// Returns 0 when the user enters a valid input file path else returns 1
// Does not allow the user to use the ".out" extension
int getInputFile(char* filePath);

// Prompts the user for an output file name until either a valid file path is entered or the user does not enter anything
// Returns 0 when the user enters a valid output file path else returns 1
// Does not allow the user to use an output filename that matches any string in "reservedNames"
int getOutputFile(char* filePath, const char* defaultPath, const int resNameCount, const char** reservedNames);

#endif
