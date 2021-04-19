#ifndef TOKEN_H
#define TOKEN_H

enum token {
	/*0.*/ BEGIN,
	/*1.*/ END,
	/*2.*/ READ,
	/*3.*/ WRITE,
	/*4.*/ IF,
	/*5.*/ THEN,
	/*6.*/ ELSE,
	/*7.*/ ENDIF,
	/*8.*/ WHILE,
	/*9.*/ ENDWHILE,
	/*10.*/ ID,
	/*11.*/ INTLITERAL,
	/*12.*/ FALSEOP,
	/*13.*/ TRUEOP,
	/*14.*/ NULLOP,
	/*15.*/ LPAREN,
	/*16.*/ RPAREN,
	/*17.*/ SEMICOLON,
	/*18.*/ COMMA,
	/*19.*/ ASSIGNOP,
	/*20.*/ PLUSOP,
	/*21.*/ MINUSOP,
	/*22.*/ MULTOP,
	/*23.*/ DIVOP,
	/*24.*/ NOTOP,
	/*25.*/ LESSOP,
	/*26.*/ LESSEQUALOP,
	/*27.*/ GREATEROP,
	/*28.*/ GREATEREQUALOP,
	/*29.*/ EQUALOP,
	/*30.*/ NOTEQUALOP,
	/*31.*/ SCANEOF,
	/*32.*/ ERROR
};

#endif