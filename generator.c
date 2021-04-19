#include "generator.h"
#include "file_util.h"

FILE* OutputFile;
FILE* TempFile;

// 
void generatorInit(FILE* out, FILE* temp)
{
	OutputFile = out;
	TempFile = temp;
}

//-----START FOR GROUP MEMBER 1-----

// 
void generate(char* s1, char* s2, char* s3, char* s4, char* s5)
{

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

}

// 
int lookup(char* var)
{

}

// 
void enter(char* var)
{

}

// 
void checkID(char* var)
{

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
struct OpRecord processOp(char* operator)
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
	fputs("\n}", TempFile);
	rewind(TempFile);
	copyFileContents(TempFile, OutputFile);

	//if (fileExists("temp.out"))
		//remove("temp.out");
}