#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "generator.h"
#include <string.h>

//
void compile(FILE* in, FILE* out, FILE* list, FILE* temp);

// Safely concatenates the characters in "TokenBuffer" onto "out"
void catTokenBuffer(char* out, int max);

//Reports errors
void reportError(char* expectedToken);

//
int getParserErrorState();

//Returns the current total of lexical errors found in the input file
int getTotalSynErrors();

//Checks if code has any more of productions 3,4,5,6, or 9
void checkForStatement();

// Print the parsed statement to the output file
void outputStatement();

// Gets the next token from the scanner and checks if it is equal to "token"
// Returns 1 if it is equal
// Returns 0 if it is not equal
int match(int token);

// Returns the next token from the scanner
int nextToken(); //NON DESTRUCTIVE

//=== start grammar functions ===//

//40. <system goal> -> <program> SCANEOF
void systemGoal(); //done

//1. <program> -> BEGIN <statement list> END
int program(); //done

//2. <statement list> -> <statement> {<statement list>}
int statementList();

//3 - 6, 9. <statement>
int statement();

//7 - 8. <IFTail> ELSE <statementlist> ENDIF
int iftail();

//10. <id list>  -> ID {, <id list>}
int idlist();

//11. <expr list> -> <expression> {, <expr list>}
int exprlist();

//12. <expression> -> <term> {<add op> <term>}
int expression();

//13. <term> -> <factor> {<mult op> <factor>}
int term();

//14 - 17. <factor>
int factor();

//18 - 19. <add op>
int addop();

//20 - 21. <mult op>
int multop();

//22. <condition> -> <addition> {<rel op> <addition>}
int condition();

//23. <addition> -> <multiplication> {<add op> <multiplication>}
int addition();

//24. <multiplication> -> <unary> {<mult op> <unary>}
int multiplication();

//25 - 27. <unary>
int unary();

//28 - 33. <unary>
int lprimary();

//34 - 39. <unary>
int relop();

//=== end grammar functions ===//

#endif