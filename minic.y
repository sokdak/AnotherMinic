%error-verbose
%code requires {
#include "include/node.h"
#include "include/symbol.h"
#include "include/function.h"
#include "include/program.h"
}
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "include/minic.h"
#include "include/symbol.h"
#include "include/log.h"
#include "include/eval.h"
#include "include/opt.h"
%}
%locations
%union {
  double dval;                // for double
  int ival;                   // for int
  char* idval;                // for id

  ast_tree_t* treeptr;        // for ast
  symbol_t* symbolptr;        // for local variable
  function_t* fptr;           // for function
  program_t* pptr;            // for program
}

%token <idval> T_ID 
%token <ival> T_INTEGER
%token <dval> T_FLOAT

%token T_PLUS T_MINUS T_MULTI T_DIVIDE T_ASSIGN
%token T_LGT T_LEGT T_RGT T_REGT T_EQ T_NEQ
%token T_LPAREN T_RPAREN T_LCURLY T_RCURLY T_COMMA T_COLON

%token T_LOCAL
%token T_DEF T_IF T_ELSE T_WHILE
%token T_PRINT T_RETURN

%left T_LGT T_LEGT T_RGT T_REGT T_EQ T_NEQ
%left T_MINUS T_PLUS
%left T_MULTI T_DIVIDE
%right UMINUS
%nonassoc THEN
%nonassoc T_ELSE

%type <treeptr> arg_list stmt stmt_list expr expr_list
                print_stmt control_stmt while_stmt return_stmt if_stmt block value

%type <symbolptr> var_list local_decl
%type <fptr> fun_def fun_list
%type <pptr> program
%%
program       : fun_list stmt_list {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to program: fun stmt\n");

                  program_t* program;

                  program = (program_t*)malloc(sizeof(program_t));
                  program->function = $1;
                  program->statement = $2;

                  if (YACC_DEBUG)
                    fprintf(stderr, "\n");
                  
                  eval(program);

                  if (YACC_DEBUG)
                    full_log(program);

                  $$ = program;
                } // function + main context
              | stmt_list {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to program: stmt\n");

                  program_t* program;

                  program = (program_t*)malloc(sizeof(program_t));
                  program->function = NULL;
                  program->statement = $1;

                  if (YACC_DEBUG) 
                    fprintf(stderr, "\n");
                  
                  eval(program);

                  if (YACC_DEBUG)
                    full_log(program);
 
                  $$ = program;
                } // main context만 있는 경우
              ;

fun_list      : fun_def fun_list {
                  $$ = append_func($1, $2);
                }
              | fun_def {
                  $$ = $1;
                }
              ;

// ast구축해서 symbol_table에 type function으로 두고 구축하고 eval함수 통해서 evaluation하기
fun_def       : T_DEF T_ID T_LPAREN arg_list T_RPAREN T_LCURLY local_decl stmt_list T_RCURLY {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to fun_def, def id laren var_list parent ... rcurly\n");
                  
                  function_t *newfunc;
                  newfunc = (function_t*)malloc(sizeof(function_t)); // 0번 function은 main context

                  newfunc->name = strdup($2);
                  newfunc->defined = true;
                  newfunc->arguments = $4;     // function arguments (args_list:: ast_tree_t*)
                  newfunc->ast = $8;           // 구축된 ast를 function list에 넣기 (stmt_list:: ast_tree_t*)
                  newfunc->ast->localvar = $7; // local symtab을 ast에 할당 (ldecl_list:: symbol_t*)

                  // ast localvar에 arguments를 추가
                  ast_tree_t* newvar = newfunc->arguments;

                  if (YACC_DEBUG)
                    if (newvar)
                      fprintf(stderr, "newvar at %p\n", newvar);

                  while (newvar != NULL) {
                    insert_symbol(newvar->token.value.argval, &(newfunc->ast->localvar));
                    if (YACC_DEBUG)
                      fprintf(stderr, "[yacc] copying function arg(%s) into localsym(%p)\n",
                        newvar->token.value.argval, newfunc->ast->localvar);
                    newvar = newvar->arglist;
                  }

                  if (YACC_DEBUG)
                    fprintf(stderr, "localsym current: %p\n", newvar);

                  $$ = newfunc;
                }
              ; // def abcd(a, b, c) { local a, b; a = 3; b = 2; }

// 함수 input (argument) 정의 -> function구조체의 arguments에 넣기
arg_list        : T_ID T_COMMA arg_list {
                    token_data_t newtoken = { .type = TT_ARGS, .value.argval = strdup($1) };
                    $$ = append_args(mkleaf(newtoken), $3);
                  }
                | T_ID {
                    token_data_t newtoken = { .type = TT_ARGS, .value.argval = strdup($1) };
                    $$ = append_args(NULL, mkleaf(newtoken));
                  }
                ;

local_decl    : T_LOCAL var_list T_COLON { $$ = $2; }
              | /* epsilon */ { $$ = NULL; }
              ;

// 함수 내 local variable 선언
var_list      : T_ID T_COMMA var_list {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to var_list, var comma var_list\n");
                  
                  int idx = -1;
                  idx = insert_symbol($1, &$3);
                  
                  if (YACC_DEBUG)
                    fprintf(stderr, "  symbol created at index %d\n", idx);

                  $$ = $3; // create_symbol_if_not_exists가 var_list에 이미 추가
                }
              | T_ID {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to var_list, var\n");
                  
                  symbol_t* localvar = NULL;
                  int idx = -1;

                  idx = insert_symbol($1, &localvar);
                  
                  if (YACC_DEBUG)
                    fprintf(stderr, "  symbol created at index %d\n", idx);

                  $$ = localvar;
                }
              ;

stmt_list     : stmt stmt_list { // stmt_list는 ast에 stmtlist에 계속 매달림
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to stmt_list, stmt stmt_list\n");

                  $$ = append_stmts($2, $1);
                }
              | stmt { // 한줄이면 그냥 올려버리기
                 if (YACC_DEBUG)
                    fprintf(stderr, "reduce to stmt_list, stmt\n");

                 $$ = $1; // append_stmts(NULL, $1);과 같음
                }
              ;

stmt          : expr T_COLON {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to stmt, expr colon\n");

                  $$ = $1;
                }
              | print_stmt T_COLON {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to stmt, print stmt\n");

                  $$ = $1;
                }
              | return_stmt T_COLON {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to stmt, return stmt\n");

                  $$ = $1;
                }
              | control_stmt {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to stmt, control stmt\n");

                  $$ = $1;
                }
              | block {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to stmt, block\n");

                  $$ = $1;
                }
              | /* epsilon */ {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to stmt, epsilon, notimpl\n");
                }
              ;

print_stmt    : T_PRINT expr {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to print_stmt, print expr\n");

                  token_data_t newtoken = { .type = TT_PRINT };
                  $$ = mknode(newtoken, $2, NULL);
                }
              ;

return_stmt   : T_RETURN expr {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to return_stmt, return expr\n");

                  token_data_t newtoken = { .type = TT_RETURN };
                  $$ = mknode(newtoken, $2, NULL);
                }
              ;

control_stmt  : if_stmt {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to control_stmt, if_stmt\n");

                  $$ = $1;
                }
              | while_stmt {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to control_stmt, while_stmt\n");

                  $$ = $1;
                }
              ;

if_stmt       : T_IF T_LPAREN expr T_RPAREN stmt T_ELSE stmt { 
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to if_stmt, if and else\n");

                  token_data_t newtoken = { .type = TT_IFELSE };
                  $$ = mknode_control(newtoken, $5, $7, $3);
                }
              | T_IF T_LPAREN expr T_RPAREN stmt %prec THEN { 
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to if_stmt, if without else\n");

                  token_data_t newtoken = { .type = TT_IF };
                  $$ = mknode_control(newtoken, $5, NULL, $3);
                }
              ;

while_stmt    : T_WHILE T_LPAREN expr T_RPAREN stmt {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to while_stmt, while\n");

                  token_data_t newtoken = { .type = TT_WHILE };
                  $$ = mknode_control(newtoken, $5, NULL, $3);
                }
              ;

block         : T_LCURLY stmt_list T_RCURLY {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to block, lcurly stmt_list rcurly\n");

                  $$ = $2;
                }
              ;

expr          : value {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to expr, value\n");
                  
                  $$ = $1;
                }
              | T_ID { // 변수: 정의되지 않고 사용하는 경우 값을 예측할 수 없어야 한다
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to expr, T_ID\n");

                  // 노드 만들고 symid올리기
                  token_data_t newtoken = { .type = TT_ID, .value.symval = strdup($1) };
                  $$ = mkleaf(newtoken);
                }
              | T_ID T_ASSIGN expr { // 전역변수: 할당
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to expr, T_ID assign expr\n");

                  token_data_t newtoken_l = { .type = TT_ID, .value.symval = strdup($1) };
                  token_data_t newtoken = { .type = TT_ASSIGN };
                  $$ = mknode(newtoken, mkleaf(newtoken_l), $3);
                }
              | expr T_PLUS expr {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to expr, expr + expr\n");

                  token_data_t newtoken = { .type = TT_ARITH_PLUS };
                  $$ = mknode(newtoken, $1, $3);
                }
              | expr T_MINUS expr {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to expr, expr - expr\n");

                  token_data_t newtoken = { .type = TT_ARITH_MINUS };
                  $$ = mknode(newtoken, $1, $3);
                }
              | expr T_MULTI expr { 
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to expr, expr * expr\n");

                  token_data_t newtoken = { .type = TT_ARITH_MULTI };
                  $$ = mknode(newtoken, $1, $3);
                }
              | expr T_DIVIDE expr {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to expr, expr / expr\n");

                  token_data_t newtoken = { .type = TT_ARITH_DIVIDE };
                  $$ = mknode(newtoken, $1, $3);
                }
              | expr T_LGT expr { 
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to expr, expr > expr\n");

                  token_data_t newtoken = { .type = TT_COMP_LGT };
                  $$ = mknode(newtoken, $1, $3);
                }
              | expr T_LEGT expr { 
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to expr, expr >= expr\n");

                  token_data_t newtoken = { .type = TT_COMP_LEGT };
                  $$ = mknode(newtoken, $1, $3);
                }
              | expr T_RGT expr { 
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to expr, expr < expr\n");

                  token_data_t newtoken = { .type = TT_COMP_RGT };
                  $$ = mknode(newtoken, $1, $3);
                }
              | expr T_REGT expr { 
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to expr, expr <= expr\n");

                  token_data_t newtoken = { .type = TT_COMP_REGT };
                  $$ = mknode(newtoken, $1, $3);
                }
              | expr T_EQ expr { 
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to expr, expr == expr\n");

                  token_data_t newtoken = { .type = TT_COMP_EQUALS };
                  $$ = mknode(newtoken, $1, $3);
                }
              | expr T_NEQ expr { 
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to expr, expr != expr\n");

                  token_data_t newtoken = { .type = TT_COMP_NOTEQUALS };
                  $$ = mknode(newtoken, $1, $3);
                }
              | T_MINUS expr %prec UMINUS { 
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to expr, -expr\n");

                  token_data_t newtoken = { .type = TT_ARITH_UMINUS };
                  $$ = mknode(newtoken, $2, NULL);
                }
              | T_LPAREN expr T_RPAREN { 
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to expr, (expr)\n");

                  $$ = $2;
                }
              | T_ID T_LPAREN expr_list T_RPAREN { // function call
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to expr, ID(expr_list)\n");

                  token_data_t newtoken = { .type = TT_PROCEDURE_CALL, .value.funstr = strdup($1) };
                  $$ = mknode_pcall(newtoken, $3);
                }
              ;

expr_list     : expr T_COMMA expr_list {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to expr_list, expr, expr_list\n");

                  $$ = append_args($1, $3);
                }
              | expr {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to expr_list, expr\n");

                  $$ = $1;
                }
              ;

value         : T_INTEGER {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to value, integer\n");

                  token_data_t newtoken = { .type = TT_INT, .value.intval = $1 };
                  $$ = mkleaf(newtoken);
                }
              | T_FLOAT {
                  if (YACC_DEBUG)
                    fprintf(stderr, "reduce to value, float\n");

                  token_data_t newtoken = { .type = TT_FLOAT, .value.doubleval = $1 };
                  $$ = mkleaf(newtoken);
                }
              ;
%%
