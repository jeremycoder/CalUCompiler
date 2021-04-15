#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>

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
typedef char OpRecord[3];

void generatorInit(FILE* out, FILE* temp);

void generate(char* s1, char* s2, char* s3, char* s4, char* s5);

char* getTemp();

int lookup(char* var);

void enter(char* var);

void checkID(char* var);

// #start
void genStart();

// #assign
void processAssign(struct ExprRecord* leftSide, struct ExprRecord* rightSide);

// #process_id
struct ExprRecord* processID(char* var);

// #read_id
void readID(struct ExprRecord* var);

// #write_expr
void writeExpr(struct ExprRecord* expression);

// #gen_infix
struct ExprRecord* genInfix(struct ExprRecord* leftSide, struct ExprRecord* rightSide, OpRecord* opRec);

// #process_literal
struct ExprRecord* processLiteral(char* literal);

// #process_op
OpRecord* processOp(char* operator);

// #finish
void genFinish();

#endif