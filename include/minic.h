#ifndef AS3_MINIC_H
#define AS3_MINIC_H

extern FILE *yyin;
extern FILE *yyout;
extern char *yytext;
extern int yylineno;
extern int yyparse();

int yylex();
void yyerror(const char *msg);

#endif