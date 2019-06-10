#ifndef AS3_SYMBOL_H
#define AS3_SYMBOL_H

#include <stdbool.h>
#include "token.h"

typedef enum symbol_type {
  TYPE_VARIABLE_INT,
  TYPE_VARIABLE_DOUBLE,
  TYPE_VARIABLE_UNDEFINED = -1
} symbol_type_t;

// 함수 구현: yyparse 재호출
// function declaration에서는 AST를 구축
// ast를 yyparse에 넘기고 return 에서 yyparse의 output을 가지고 나오기 (pass by reference)
// func_flag = yyparse(ptree, *)
// https://github.com/swmaestro06-apus/apus/wiki/yyparse()-%ED%95%A8%EC%88%98%EC%9D%98-%EC%9D%B8%EC%9E%90-%EC%A0%84%EB%8B%AC
typedef struct symbol {
  int id;
  char *name;                            // 심볼 이름
  bool assigned;                          // 할당 여부
  symbol_type_t type;                     // 심볼의 타입

  union {
    int integer_constant;                 // 정수
    double real_constant;                 // 실수
  } value;

  struct symbol *next;
} symbol_t;

int find_symbol(char *, symbol_t *);
int get_list_size(symbol_t *);
symbol_t* last_symbol(symbol_t *);
symbol_t* get_symbol(int, symbol_t *);
symbol_t* get_symbol_by_name(char *, symbol_t *);
int insert_symbol(char *, symbol_t **);
int insert_symbol_value(char *, token_data_t, symbol_t **);
bool symbol_exists_on_table(int, symbol_t *);
bool symbol_exists_on_table_by_name(char *, symbol_t *);

void push_symboltable(symbol_t *);
symbol_t* pop_symboltable();
bool stack_isempty();

void print_symboltable(symbol_t *);

#endif