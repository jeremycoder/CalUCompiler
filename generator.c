#include "generator.h"

#define SYMBOL_SIZE 15
#define INITIAL_TABLE_SIZE 10

// Pointer to array of pointers to arrays of characters
char** symbols = NULL;
int tableSize = 0;

FILE* OutputFile;
FILE* TempFile;

int TempNum = 0;

// 
void generatorInit(FILE* out, FILE* temp)
{
	OutputFile = out;
	TempFile = temp;
}

//-----START FOR GROUP MEMBER 1-----

// 
void generate(char* s1, char* s2, char* s3, char* s4, char* s5, char* s6)
{
	fputs(s1, TempFile);
	fputs(s2, TempFile);
	fputs(s3, TempFile);
	fputs(s4, TempFile);
	fputs(s5, TempFile);
	fputs(s6, TempFile);
}

// #assign
void processAssign(struct ExprRecord* leftSide, struct ExprRecord* rightSide)
{
	
}

// #read_id
void readID(struct ExprRecord* id)
{

}

// #write_expr
void writeExpr(struct ExprRecord* expr)
{

}

//-----END FOR GROUP MEMBER 1-----

//-----START FOR GROUP MEMBER 2-----

// 
char* getTemp()
{
	TempNum++;
	char temp[SYMBOL_SIZE] = { "Temp\0" };
	itoa(TempNum, temp + 4, 10);
	checkID(temp);
	return temp;
}

// Returns the index of a symbol in the symbol table that has the same name as "symbol"...
// ...or at least the index of the slot that that symbol should be
int hashTo(char* symbol)
{
	int index = -1;
	int i;
	int length = strlen(symbol);
	for (i = 0; i < length; i++)
	{
		index += ((symbol[i] - 65) - i); // There's probably a better way of hashing
	}

	if (index < 0)
		index = -index;

	if (i < tableSize)
	{
		// Handles collisions
		while (i < tableSize && symbols[i] != 0)
		{
			// Checks if "symbol[i]" is the one that is trying to be hashed to
			if (strcmp(symbols[i], symbol) == 0)
				break;
			i++;
		}

		return index;
	}
}

void clearTable()
{
	if (symbols != 0)
	{
		int i;
		for (i = tableSize - 1; i > -1; i--)
		{
			if (symbols[i] != 0)
				free(symbols[i]);
		}
		free(symbols);
	}
}

// 
int lookup(char* var)
{
	int index = hashTo(var);
	int returnVal = 0;
	if (index < tableSize)
	{
		if (symbols[index] != NULL)
			returnVal = 1;
	}
	return returnVal;
}

// 
void enter(char* var)
{
	int index = hashTo(var);

	// Checks if the table should be resized
	if (index >= tableSize)
	{
		int i;
		int newTableSize = index + 1;
		char** newTable = calloc(newTableSize, sizeof(char*));
		// Allocates for added length to the symbol table
		if (symbols != 0)
		{
			for (i = 0; i < tableSize; i++)
			{
				// Copies each pointer to an array of characters into the new table
				newTable[i] = symbols[i];
			}
			free(symbols);
		}
		tableSize = newTableSize;
		symbols = newTable;
	}
	symbols[index] = calloc(SYMBOL_SIZE, sizeof(char));
	strcpy(symbols[index], var);
}

// 
void checkID(char* var)
{
	printf("\n%s\n", var);
	if (lookup(var) == 0)
	{
		enter(var);
		fputs("int ", OutputFile);
		fputs(var, OutputFile);
		fputs(";\n", OutputFile);
	}
}

//-----END FOR GROUP MEMBER 2-----

//-----START FOR GROUP MEMBER 3-----

// #start
void genStart()
{
	// Write current date and time to output file here

	fputs("#include <stdio.h>\n\nint main()\n{\n", OutputFile);
}

// #gen_infix (THIS IS PROBABLY A BIG ONE)
struct ExprRecord genInfix(struct ExprRecord* leftSide, struct OpRecord* operator, struct ExprRecord* rightSide)
{
	
}

//-----END FOR GROUP MEMBER 3-----

//-----START FOR GROUP MEMBER 4-----

// #process_literal
struct ExprRecord processLiteral(char* literal)
{
	
}

// #process_op
struct OpRecord processOp(int operator)
{
	
}

// #process_id
struct ExprRecord processID(char* var)
{
	
}

//-----END FOR GROUP MEMBER 4-----

// #finish
void genFinish()
{
	clearTable();

	fputs("\nreturn 0;\n}", TempFile);
	rewind(TempFile);
	copyFileContents(TempFile, OutputFile);

	//if (fileExists("temp.out"))
		//remove("temp.out");
}