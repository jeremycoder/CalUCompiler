#include <time.h>
#include "generator.h"

#define SYMBOL_SIZE 15
#define INITIAL_TABLE_SIZE 10

// Pointer to array of pointers to arrays of characters
char** Symbols = NULL;
char* CompilationMsg = NULL;
int TableSize = 0;
int TabNum = 0;

FILE* OutputFile;
FILE* TempFile;

int TempNum = 0;
// A flag that is set to 1 when the last "generate" call had a newline
int NewLineStarted = 0;

// Passes the Generator pointers to an output file and a temporary file for compilation
void generatorInit(FILE* out, FILE* temp)
{
	OutputFile = out;
	TempFile = temp;
}

// Returns the current temporary code generation variable number
int getTempNum()
{
	return TempNum;
}

// Sets the current temporary code generation variable number
void setTempNum(int tempNum)
{
	TempNum = tempNum;
}

// #start
void genStart()
{
	time_t myTime;
	myTime = time(NULL);

	fputs("//Current Date and Time:\n//", OutputFile);
	fputs(ctime(&myTime), OutputFile);
	fputs("\n#include <stdio.h>\n\nint main()\n{\n", OutputFile);

	NewLineStarted = 1;
	TabNum = 0;
	//Initialize symbol table
	clearTable();
	//Initialize temp counter
	TempNum = 0;
	//Initialize line counter - done in parser

	addTab();

}

// Increments the tab counter for how many tabs are put at the newlines
void addTab()
{
	TabNum++;
}

// Decrements the tab counter for how many tabs are put at the newlines
// Restricts the tab counter from going below 0
void removeTab()
{
	if (TabNum > 0)
		TabNum--;
}

// Copies "msg" into a buffer that will be printed as a C-style comment to the temp file when "genFinish" is called
void setCompilationMsg(char* msg)
{
	if (CompilationMsg != NULL)
		free(CompilationMsg);
	CompilationMsg = calloc(strlen(msg) + 1, sizeof(char));
	strcpy(CompilationMsg, msg);
}

// Returns a pointer to a new temporary variable name for code generation
char* getTemp()
{
	TempNum++;
	char temp[SYMBOL_SIZE] = { "Temp\0" };
	itoa(TempNum, temp + 4, 10);
	checkID(temp);
	return Symbols[hashTo(temp)];
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

	if (i < TableSize)
	{
		// Handles collisions
		while (i < TableSize && Symbols[i] != 0)
		{
			// Checks if "symbol[i]" is the one that is trying to be hashed to
			if (strcmp(Symbols[i], symbol) == 0)
				break;
			i++;
		}
	}
	return index;
}

// Frees any memory that was allocated for the symbol table
void clearTable()
{
	if (Symbols != 0)
	{
		int i;
		for (i = TableSize - 1; i > -1; i--)
		{
			if (Symbols[i] != 0)
				free(Symbols[i]);
		}
		TableSize = 0;
		free(Symbols);
	}
}

// Checks if "var" exists yet in the symbol table
// If it exists, then 1 is returned else 0
int lookup(char* var)
{
	int index = hashTo(var);
	//printf("\nLooking up: %s, with index: %d\n", var, index);
	int returnVal = 0;
	if (index < TableSize)
	{
		if (Symbols[index] != NULL)
			returnVal = 1;
	}
	return returnVal;
}

// Enters "var" into the symbol table using hashing
void enter(char* var)
{
	int index = hashTo(var);

	// Checks if the table should be resized
	if (index >= TableSize)
	{
		int i;
		int newTableSize = index + 1;
		char** newTable = calloc(newTableSize, sizeof(char*));
		// Allocates for added length to the symbol table
		if (Symbols != 0)
		{
			for (i = 0; i < TableSize; i++)
			{
				// Copies each pointer to an array of characters into the new table
				newTable[i] = Symbols[i];
			}
			free(Symbols);
		}
		TableSize = newTableSize;
		Symbols = newTable;
	}
	Symbols[index] = calloc(SYMBOL_SIZE, sizeof(char));
	strcpy(Symbols[index], var);
}

// Checks if "var" exists in the symbol table or not
// If it does not exist, then it is entered into the symbol table and appropriate C code is generated
void checkID(char* var)
{
	if (lookup(var) == 0)
	{
		enter(var);
		fputc('\t', OutputFile);
		fputs("int ", OutputFile);
		fputs(var, OutputFile);
		fputs(";\n", OutputFile);
	}
}

// Prints "s1", "s2", "s3", "s4", and "s5" to the "TempFile"
// Prints a new line character if "newLine" is set to 1
// A call to "generate" following a call that performed a new line will add a number of tabs equal to "TabNum"
void generate(char* s1, char* s2, char* s3, char* s4, char* s5, int newLine)
{
	if (NewLineStarted == 1)
	{
		NewLineStarted = 0;
		int i;
		for (i = 0; i < TabNum; i++)
			fputc('\t', TempFile);
	}
	fputs(s1, TempFile);
	fputs(s2, TempFile);
	fputs(s3, TempFile);
	fputs(s4, TempFile);
	fputs(s5, TempFile);
	if (newLine == 1)
	{
		fputs("\n", TempFile);
		NewLineStarted = 1;
	}
}

// #assign
void processAssign(struct ExprRecord* leftSide, struct ExprRecord* rightSide)
{	
	//Is target leftSide or is source rightSide
	generate(leftSide->expression, " = ", rightSide->expression, ";", "", 1);	

}

// #processIf
void processIf(struct ExprRecord* expr)
{
	generate("if (", expr->expression, ")", "", "", 1);
	generate("{", "", "", "", "", 1);
	addTab();
}

// #processElse
void processElse()
{
	removeTab();
	generate("}", "", "", "", "", 1);
	generate("else", "", "", "", "", 1);
	generate("{", "", "", "", "", 1);
	addTab();
}

// #processEndif
void processEndif()
{
	generate("}", "", "", "", "", 1);
}

// #processWhile
void processWhile(struct ExprRecord* expr)
{
	generate("while (", expr->expression, ")", "", "", 1);
	generate("{", "", "", "", "", 1);
	addTab();
}

// #processEndwhile
void processEndwhile()
{
	removeTab();
	generate("}", "", "", "", "", 1);
}

// #processLiteral
struct ExprRecord processLiteral(char* literal)
{
	struct ExprRecord tempRec;
	tempRec.type = INTLITERAL;

	strcpy(tempRec.expression, literal);

	return tempRec;
}

// #processOp
struct OpRecord processOp(int operator)
{
	struct OpRecord op;

	switch (operator)
	{
	case EQUALOP:
		strcpy(op.record, "==");
		break;
	case NOTEQUALOP:
		strcpy(op.record, "!=");
		break;
	case GREATEROP:
		strcpy(op.record, ">");
		break;
	case GREATEREQUALOP:
		strcpy(op.record, ">=");
		break;
	case LESSOP:
		strcpy(op.record, "<");
		break;
	case LESSEQUALOP:
		strcpy(op.record, "<=");
		break;
	case ASSIGNOP:
		strcpy(op.record, "=");
		break;
	case PLUSOP:
		strcpy(op.record, "+");
		break;
	case MINUSOP:
		strcpy(op.record, "-");
		break;
	case DIVOP:
		strcpy(op.record, "/");
		break;
	case MULTOP:
		strcpy(op.record, "*");
		break;
	case NOTOP:
		strcpy(op.record, "!");
		break;
	default:
		strcpy(op.record, " ");
		break;
	}

	return op;

}

// #processID 
//Generates code for ID semantic record
struct ExprRecord processID(char* var)
{
	struct ExprRecord tempRec; //create temp str

	checkID(var); //checks if var is in symbol table	

	tempRec.type = IDEXPR;
	strcpy(tempRec.expression, var);

	return tempRec;
}

// #readID Generates code for read statement
void readID(struct ExprRecord* id)
{
	//write scanf to temp file
	generate("scanf(\"%d\", &", id->expression, ");", "", "", 1);
}

// #writeExpr
void writeExpr(struct ExprRecord* expr)
{
	//write printf to temp file
	generate("printf(\"%d\", ", expr->expression, ");", "", "", 1);
}

// #genInfix 
//Generates the code for the infox semantic record
struct ExprRecord genInfix(struct ExprRecord* leftSide, struct OpRecord* operator, struct ExprRecord* rightSide)
{	
	//Creates a temp expression record and sets its kind to "TEMPEXPR"
	struct ExprRecord expr;
	expr.type = TEMPEXPR;	
	
	//Sets it's string to a new temp ID using getTemp()
	strcpy(expr.expression, getTemp());
	
	//Uses generate to assign the left, right, and operand expressions to the temp EX: Temp5 = X + 7
	generate(expr.expression, " = ", leftSide->expression, " ", operator->record, 0);
	generate(" ", rightSide->expression, ";", "", "", 1);
	
	//Returns the temp expression record
	return expr;
}

// #genFinish
void genFinish()
{
	clearTable();

	removeTab();

	fputs("\n\treturn 0;\n}", TempFile);

	generate("\n/* ", CompilationMsg, " */", "", "", 0);

	if (CompilationMsg != NULL)
		free(CompilationMsg);

	rewind(TempFile);
	copyFileContents(TempFile, OutputFile);

	//if (fileExists("temp.out"))
		//remove("temp.out");
}
