#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>
#include "file_util.h"
#include "token.h"

#define MAX_EXPRESSION_SIZE 100

enum exprType
{
	IDEXPR,
	LITERALEXPR,
	TEMPEXPR
};

struct ExprRecord
{
	char expression[MAX_EXPRESSION_SIZE];
	int type;
};

// To store 1 or 2 character operators along with a null terminator
struct OpRecord
{
	char record[3];
};

// Passes the Generator pointers to an output file and a temporary file for compilation
void generatorInit(FILE* out, FILE* temp);

// Returns the current temporary code generation variable number
int getTempNum();

// Sets the current temporary code generation variable number
void setTempNum(int tempNum);

// #start
void genStart();

// Increments the tab counter for how many tabs are put at the newlines
void addTab();

// Decrements the tab counter for how many tabs are put at the newlines
// Restricts the tab counter from going below 0
void removeTab();

// Copies "msg" into a buffer that will be printed as a C-style comment to the temp file when "genFinish" is called
void setCompilationMsg(char* msg);

// Returns a pointer to a new temporary variable name for code generation
char* getTemp();

// Returns the index of a symbol in the symbol table that has the same name as "symbol"...
// ...or at least the index of the slot that that symbol should be
int hashTo(char* symbol);

// Frees any memory that was allocated for the symbol table
void clearTable();

// Checks if "var" exists yet in the symbol table
// If it exists, then 1 is returned else 0
int lookup(char* var);

// Enters "var" into the symbol table using hashing
void enter(char* var);

// Checks if "var" exists in the symbol table or not
// If it does not exist, then it is entered into the symbol table and appropriate C code is generated
void checkID(char* var);

// Prints "s1", "s2", "s3", "s4", and "s5" to the "TempFile"
// Prints a new line character if "newLine" is set to 1
// A call to "generate" following a call that performed a new line will add a number of tabs equal to "TabNum"
void generate(char* s1, char* s2, char* s3, char* s4, char* s5, int newLine);

// #assign
void processAssign(struct ExprRecord* leftSide, struct ExprRecord* rightSide);

// #processIf
void processIf(struct ExprRecord* expr);

// #processElse
void processElse();

// #processEndif
void processEndif();

// #processWhile
void processWhile(struct ExprRecord* expr);

// #processEndwhile
void processEndwhile();

// #processLiteral
struct ExprRecord processLiteral(char* literal);

// #processOp
struct OpRecord processOp(int operator);

// #processID 
//Generates code for ID semantic record
struct ExprRecord processID(char* var);

// #readID Generates code for read statement
void readID(struct ExprRecord* id);

// #writeExpr
void writeExpr(struct ExprRecord* expr);

// #genInfix 
//Generates the code for the infox semantic record
struct ExprRecord genInfix(struct ExprRecord* leftSide, struct OpRecord* operator, struct ExprRecord* rightSide);

// #genFinish
void genFinish();

#endif
