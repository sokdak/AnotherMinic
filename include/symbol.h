#ifndef AS3_SYMBOL_H
#define AS3_SYMBOL_H

#include <stdbool.h>
#include "ast.h"

#define SYMBOL_TABLE_MAX 100

typedef enum symbol_type {
  TYPE_VARIABLE,
  TYPE_FUNCTION
} symbol_type_t;

typedef struct argument {
  char* arg_name;
} argument_t;

// 함수 구현: yyparse 재호출
// function declaration에서는 AST를 구축
// ast를 yyparse에 넘기고 return 에서 yyparse의 output을 가지고 나오기 (pass by reference)
// func_flag = yyparse(ptree, *)
// https://github.com/swmaestro06-apus/apus/wiki/yyparse()-%ED%95%A8%EC%88%98%EC%9D%98-%EC%9D%B8%EC%9E%90-%EC%A0%84%EB%8B%AC
typedef struct {
  char *symbol;                           // 심볼 이름
  bool assigned;                          // 할당 여부
  symbol_type_t type;                     // 심볼의 타입
  argument_t* arguments;                  // 함수일 경우 args들
  union {
    int integer_constant;                 // 정수
    double real_constant;                 // 실수
    ast_btree_t* ast;                         // 심볼이 펑션일 경우 앞서 파싱한 ast를 저장
  } value;
} symbol_table_t;

#endif