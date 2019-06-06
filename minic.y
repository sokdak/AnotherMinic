%error-verbose
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "include/minic.h"
#include "include/symbol.h"
#include "include/util.h"

symbol_table_t vbltable[100];
symbol_table_t global_vblTable[100];

int vblCount = 0;
int global_vblCount = 0;

symbol_table_t* currentFunction;
%}
%union {
  double dval;
  int ival;
  int vblno;
}

%token T_ID T_INTEGER T_FLOAT
%token T_PLUS T_MINUS T_MULTI T_DIVIDE T_ASSIGN
%token T_LGT T_LEGT T_RGT T_REGT T_EQ T_NEQ
%token T_LPAREN T_RPAREN T_LCURLY T_RCURLY T_COMMA T_COLON

%token T_LOCAL
%token T_DEF T_IF T_ELSE T_WHILE
%token T_PRINT T_RETURN

%left T_MINUS T_PLUS
%left T_MULTI T_DIVIDE
%right UMINUS

%type <dval> T_FLOAT
%type <ival> T_INTEGER
%type <vblno> T_ID
%%
program       : fun_list stmt_list { fprintf(stderr, "reduce to program: fun stmt"); } // function + main context
              | stmt_list { fprintf(stderr, "reduce to program: stmt"); } // main context만 있는 경우
              ;

// fun_def에서는 AST를 구축해야 함
fun_list      : fun_def fun_list        // function declaration시 symbol에 AST 구축
              | /* epsilon */ { currentFunction = NULL; }          // function이 끝나면 parseFunction section 끝
              ;

// ast구축해서 symbol_table에 type function으로 두고 구축하고 eval함수 통해서 evaluation하기
fun_def       : T_DEF T_ID {
                  // currentFunction = &vbltable[$2];
                  // currentFunction->assigned = false;
                  // currentFunction->symbol = hardcopy_str(yytext);
                  // currentFunction->type = TYPE_FUNCTION;
                } // ID를 심볼테이블에 저장하기
                T_LPAREN var_list T_RPAREN T_LCURLY decl_list stmt_list T_RCURLY {
                  // currentFunction->assigned = true;
                  fprintf(stderr, "reduce to fun_def, def id laren var_list parent ... rcurly\n");
                }
              ; // def abcd(a, b, c) { local a, b; a = 3; b = 2; }

decl_list     : local_decl decl_list
              | /* epsilon */
              ;

local_decl    : T_LOCAL var_list T_COLON
              ;

stmt_list     : stmt stmt_list  { fprintf(stderr, "reduce to stmt_list, stmt stmt_list\n"); }
              | stmt { fprintf(stderr, "reduce to stmt_list, stmt\n"); }
              ;

stmt          : expr T_COLON { fprintf(stderr, "reduce to stmt, expr colon\n"); }
              | print_stmt T_COLON  { fprintf(stderr, "reduce to stmt, print stmt\n"); }
              | return_stmt T_COLON { fprintf(stderr, "reduce to stmt, return stmt\n"); }
              | control_stmt { fprintf(stderr, "reduce to stmt, control stmt\n"); }
              | block { fprintf(stderr, "reduce to stmt, block\n"); }
              ;

print_stmt    : T_PRINT expr { fprintf(stderr, "reduce to print_stmt, print expr\n");}
              ;

return_stmt   : T_RETURN expr { fprintf(stderr, "reduce to return_stmt, return expr\n");}
              ;

control_stmt  : if_stmt { fprintf(stderr, "reduce to control_stmt, if_stmt\n");}
              | while_stmt { fprintf(stderr, "reduce to control_stmt, while_stmt\n");}
              ;

if_stmt       : T_IF T_LPAREN expr T_RPAREN stmt T_ELSE stmt { fprintf(stderr, "reduce to if_stmt, if and else\n");}
              | T_IF T_LPAREN expr T_RPAREN stmt { fprintf(stderr, "reduce to if_stmt, if without else\n");}
              ;

while_stmt    : T_WHILE T_LPAREN expr T_RPAREN stmt { fprintf(stderr, "reduce to while_stmt, while\n");}
              | /* epsilon */ { fprintf(stderr, "reduce to while_stmt, epsilon\n");}
              ;

block         : T_LCURLY stmt_list T_RCURLY { fprintf(stderr, "reduce to block, lcurly stmt_list rcurly\n");}
              ;

var_list      : variable T_COMMA var_list  { fprintf(stderr, "reduce to var_list, var comma var_list\n");}
              | variable  { fprintf(stderr, "reduce to var_list, var\n");}
              ;

expr          : value { fprintf(stderr, "reduce to expr, value\n");}
              | variable { fprintf(stderr, "reduce to expr, variable\n");}
              | variable T_ASSIGN expr { fprintf(stderr, "reduce to expr, var assign expr\n");}
              | expr T_PLUS expr { fprintf(stderr, "reduce to expr, expr + expr\n");}
              | expr T_MINUS expr { fprintf(stderr, "reduce to expr, expr - expr\n");}
              | expr T_MULTI expr { fprintf(stderr, "reduce to expr, expr * expr\n");}
              | expr T_DIVIDE expr { fprintf(stderr, "reduce to expr, expr / expr\n");}
              | expr T_LGT expr { fprintf(stderr, "reduce to expr, expr > expr\n");}
              | expr T_LEGT expr { fprintf(stderr, "reduce to expr, expr >= expr\n");}
              | expr T_RGT expr { fprintf(stderr, "reduce to expr, expr < expr\n");}
              | expr T_REGT expr { fprintf(stderr, "reduce to expr, expr <= expr\n");}
              | expr T_EQ expr { fprintf(stderr, "reduce to expr, expr == expr\n");}
              | expr T_NEQ expr { fprintf(stderr, "reduce to expr, expr != expr\n");}
              | T_MINUS expr %prec UMINUS  { fprintf(stderr, "reduce to expr, -expr\n");}
              | T_LCURLY expr T_RCURLY { fprintf(stderr, "reduce to expr, (expr)\n");}
              | T_ID T_LPAREN expr_list T_RPAREN { fprintf(stderr, "reduce to expr, ID(expr_list)\n");}
              ;

expr_list     : expr T_COMMA expr_list { fprintf(stderr, "reduce to expr_list, expr, expr_list\n");}
              | expr { fprintf(stderr, "reduce to expr_list, expr\n");}
              ;

value         : T_INTEGER { fprintf(stderr, "reduce to value, integer\n");}
              | T_FLOAT { fprintf(stderr, "reduce to value, float\n");}
              ;

variable      : T_ID { fprintf(stderr, "reduce to variable, id\n");}
              ;
%%
// https://stackoverflow.com/questions/32472496/methods-in-yacc-to-get-line-no
int yyerror(const char* msg)
{
  fprintf(stderr, "error: %s at line %d\n", msg, yylineno);
  return 0;
}