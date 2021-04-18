#ifndef SCANNER_H
#define SCANNER_H

#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//
void scannerInit(FILE* inputFilePtr, FILE* outputFilePtr, FILE* listFilePtr);

//
void addToErrorBuffer(const char* error);

//Returns the current total of lexical errors found in the input file
int getTotalLexErrors();

// Returns a pointer to "TokenBuffer"
// The size of "TokenBuffer" is TOKEN_BUFFER_SIZE characters
char* getTokenBuffer();

//Returns the current line number that the scanner is scanning of the input file
int getCurLineNum();

//Given enum value of token, and a string, returns string value of token
char* getTokenType(int token, char* str);

// Returns 1 if the character is an alpha ASCII character, else returns 0
int charIsAlpha(char c);

// Returns 1 if the character is a numberic ASCII character, else returns 0
int charIsInt(char c);

// Checks if the string stored in buffer matches any of the hardcoded reserved names
// If it matches a reserved name, then the corresponding token enumeration is returned, else -1 is returned
int checkReserved(const char* buffer);

//Scans input file for tokens and returns the next token
int scanner(int destructive);

void updateListFile(int token);

void updateOutputFile(int token);

#endif