%{

/* Declarations section */
#include "source.hpp"
#include "parser.tab.hpp"
#include "hw3_output.hpp"
%}

%option yylineno
%option noyywrap
 
%%

void                                              return VOID;
int                                               return INT;
byte                                              return BYTE;
b                                                 return B;
bool                                              return BOOL;
enum                                              return ENUM;
and                                               return AND;
or                                                return OR;
not                                               return NOT;
true                                              return TRUE;
false                                             return FALSE;
return                                            return RETURN;
if                                                return IF;
else                                              return ELSE;
while                                             return WHILE;
break                                             return BREAK;
continue                                          return CONTINUE;
;                                                 return SC;
","                                               return COMMA;
"("                                               return LPAREN;
")"                                               return RPAREN;
"{"                                               return LBRACE;
"}"                                               return RBRACE;
"="                                               return ASSIGN;
"<"                                               return RELOP_LT;
">"                                               return RELOP_GT;
"<="                                              return RELOP_LE;
">="                                              return RELOP_GE;
"=="                                              return RELOP_EQ;
"!="                                              return RELOP_NE;
"*"                                               return BINOP_MUL;
"/"                                               return BINOP_DIV;
"+"                                               return BINOP_ADD;
"-"												  return BINOP_SUB;

[a-zA-Z][a-zA-Z0-9]*                              {	
													yylval.id.set_id(string(yytext));
													return ID;
												  }
0|[1-9][0-9]* 									  {
													yylval.num.set_num(string(yytext));
												    return NUM;
												  }
\"([^\n\r\"\\]|\\[rnt"\\])+\"                     return STRING; 

"//"[^\r\n]*[\r|\n|\r\n]?
[\x0D\x09\x20\x0A]

.                                           	  {
													output::errorLex(yylineno); 
													exit(0);
												  }
%%

