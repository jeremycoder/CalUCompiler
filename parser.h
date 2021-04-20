#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "generator.h"
#include <string.h>

//
void compile(FILE* in, FILE* out, FILE* list, FILE* temp);

// Safely concatenates the characters in "TokenBuffer" 
void catTokenBuffer(char* out);

//Reports errors
void reportError(char* expectedToken);

//Returns 1 if there was an error that prevented the parser from finishing the parse process
int getParserErrorState();

//Returns the current total of lexical errors found in the input file
int getTotalSynErrors();

//Check for statements
void checkForStatement();

// Gets the next token from the scanner and checks if it is equal to "token"
// Returns 1 if it is equal
// Returns 0 if it is not equal
int match(int token);

// Looks better than calling "scanner(0)" everywhere
int nextToken();

//Parser starts here
// 40. <system goal> -> <program> SCANEOF;
void systemGoal();

// 41. <ident> -> ID #processID 
struct ExprRecord ident();

// 1. <program> -> #genStart BEGIN <statement list> END
void program();

// 2. <statement_list> -> <statement> {<statement list>}
void statementList();

// 3. <statement> -> ID ASSIGNOP <expression> #processAssign;
// 4. <statement> -> WRITE LPAREN <idlist> RPAREN;
// 5. <statement> -> WRITE LPAREN <exprlist> RPAREN;
// 6. <statement> -> IF LPAREN <condition> RPAREN THEN <statementlist> <iftail>
// 9. <statement> -> WHILE LPAREN <condition> RPAREN {<statementlist>} ENDWHILE
void statement();

// 7. <IFTail> -> ELSE <statementlist> ENDIF
// 8. <IFTail> -> ENDIF
void iftail();

// 10. <id list> -> <ident> #readID {, <id list>}
void idlist();

// 11. <expr list> -> <expression> #writeExpr {, <expr list>}
void exprlist();

// 12. <expression> -> <term> {<add op> <term> #genInfix}
void expression(struct ExprRecord* result);

//13. <term> -> <factor> {<mult op> <factor> #genInfix}
void term(struct ExprRecord* result);

// 14. <factor> -> LPAREN <expression> RPAREN
// 15. <factor> -> MINUSOP <factor>
// 16. <factor> -> <ident>
// 17. <factor> -> INTLITERAL #processLiteral
void factor(struct ExprRecord* result);

// 18. <add op> -> PLUSOP #processOp
// 19. <add op> -> MINUSOP #processOp
void addop(struct OpRecord* result);

// 20. <mult op> -> MULTOP #processOp
// 21. <add op> -> DIVOP #processOp
void multop(struct OpRecord* result);

// 22. <condition> -> <addition> {<rel op> <addition> #genInfix}
struct ExprRecord condition();

// 23. <addition> -> <multiplication> {<add op> <multiplication> #genInfix}
void addition(struct ExprRecord* result);

// 24. <multiplication> -> <unary> {<mult op> <unary> #genInfix}
void multiplication(struct ExprRecord* result);

// 25. <unary> -> NOTOP <unary>
// 26. <unary> -> MINUSOP <unary>
// 27. <unary> -> <lprimary>
void unary(struct ExprRecord* result);

// 28. <lprimary> -> INTLITERAL #processLiteral
// 29. <lprimary> -> <ident>
// 30. <lprimary> -> LPAREN <condition> RPAREN
// 31. <lprimary> -> FALSEOP #processLiteral
// 32. <lprimary> -> TRUEOP #processLiteral
// 33. <lprimary> -> NULLOP #processLiteral
void lprimary(struct ExprRecord* result);

// 34. <relop> -> LESSOP #processOp
// 35. <relop> -> LESSEQUALOP #processOp
// 36. <relop> -> GREATEROP #processOp
// 37. <relop> -> GREATEREQUALOP #processOp
// 38. <relop> -> EQUALOP #processOp
// 39. <relop> -> NOTEQUALOP #processOp
void relop(struct OpRecord* result);

#endif
