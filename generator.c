#include <time.h>
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
	char temp[100];
	char temp2[100];
	strcpy(temp, (*leftSide).expression);
	strcpy(temp2, (*rightSide).expression);	
	
	//Is target leftSide or is source rightSide
	generate(temp, " = ", temp2, ";", " ", " ");	

}

// #read_id Generates code for read statement
void readID(struct ExprRecord* id)
{
	char temp[100]; //create temp string
	strcpy(temp, "scanf(\"%d\", &"); //add scanf code to temp string
	strcat(temp, (*id).expression); //add id to string
	strcat(temp, ");\n"); //add last part of scanf code to string	
	
	//write scanf to output file
	fputs(temp, OutputFile);	
}

// #write_expr
void writeExpr(struct ExprRecord* expr)
{
	char temp[100]; //create temp string
	strcpy(temp, "printf(\"%d\", "); //add scanf code to temp string
	strcat(temp, (*expr).expression); //add expr to string
	strcat(temp, ");\n"); //add last part of scanf code to string	
	
	//write scanf to output file
	fputs(temp, OutputFile);	
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
		tableSize = 0;
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
	time_t myTime;
	myTime = time(NULL);
	
	fputs("//Current Date and Time:\n//", OutputFile);
	fputs(ctime(&myTime), OutputFile);
	fputs("\n#include <stdio.h>\n\nint main()\n{\n", OutputFile);
	
	//Initialize symbol table
	clearTable();
	//Initialize temp counter
	TempNum = 0;
	//Initialize line counter - done in parser
	
}


// #gen_infix //Generates the code for the infox semantic record
struct ExprRecord genInfix(struct ExprRecord* leftSide, struct OpRecord* operator, struct ExprRecord* rightSide)
{	
	//Creates a temp expression record and sets its kind to "TEMPEXPR"
	struct ExprRecord expr;
	expr.type = TEMPEXPR;	
	
	//Sets it's string to a new temp ID using getTemp()
	strcpy(expr.expression, getTemp());
	
	//Uses generate to assign the left, right, and operand expressions to the temp EX: Temp5 = X + 7
	generate(expr.expression, " = ", leftSide->expression, operator->record, rightSide->expression, ";\n");
	
	//Returns the temp expression record
	return expr;
}


//-----END FOR GROUP MEMBER 3-----

//-----START FOR GROUP MEMBER 4-----

// #process_literal
struct ExprRecord processLiteral(char* literal)
{
	struct ExprRecord tempRec;
	tempRec.type = INTLITERAL;	
	
	
	strcpy(tempRec.expression, literal);
	
	return tempRec;	
}

// #process_op
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
	}
	
	return op;	
	
}

// #process_id //Generates code for ID semantic record
struct ExprRecord processID(char* var)
{
	struct ExprRecord tempRec; //create temp str
	
	checkID(var); //checks if var is in symbol table	
	
	tempRec.type = IDEXPR;
	strcpy(tempRec.expression, var);
	
	return tempRec;	
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
