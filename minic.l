%{
#include <stdlib.h>
#include <string.h>
#include "minic.tab.h"
#include "include/minic.h"
#include "include/symbol.h"
#include "include/node.h"
%}
%option noyywrap
%option yylineno

INTEGER       ([0]|[1-9][0-9]*)
FLOAT         ([0-9]*\.[0-9]+)
ID            (\_)*[A-Za-z][a-zA-Z0-9_]*
DELIM         [ \t\r]
WS            {DELIM}+
%%
{WS}          ; /* ignoring whitespaces */

"+"           { return T_PLUS; }
"-"           { return T_MINUS; }
"*"           { return T_MULTI; }
"/"           { return T_DIVIDE; }
"="           { return T_ASSIGN; }

"=="          { return T_EQ; }
"!="          { return T_NEQ; }
">"           { return T_LGT; }
">="          { return T_LEGT; }
"<="          { return T_REGT; }
"<"           { return T_RGT; }

"("           { return T_LPAREN; }
")"           { return T_RPAREN; }
"{"           { return T_LCURLY; }
"}"           { return T_RCURLY; }

","           { return T_COMMA; }
";"           { return T_COLON; }

"if"          { return T_IF; }
"while"       { return T_WHILE; }
"return"      { return T_RETURN; }
"else"        { return T_ELSE; }
"print"       { return T_PRINT; }

"def"         { return T_DEF; }
"local"       { return T_LOCAL; }

{INTEGER}     { yylval.ival = atoi(yytext); return T_INTEGER; }
{FLOAT}       { yylval.dval = atof(yytext); return T_FLOAT; }
{ID}          { yylval.idval = strdup(yytext); return T_ID; }

.             { yyerror("lexical error"); }
%%
// https://stackoverflow.com/questions/32472496/methods-in-yacc-to-get-line-no
void yyerror(const char* msg)
{
  fprintf(stderr, "error: %s at line %d\n", msg, yylineno);
}