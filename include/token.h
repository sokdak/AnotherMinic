#ifndef AS3_TOKEN_H
#define AS3_TOKEN_H

#include "symbol.h"

typedef enum token_type {
  TT_ID = 1,        // ID
  TT_INT,           // INTEGER
  TT_FLOAT,         // FLOAT
  TT_COMP_LGT,      // >
  TT_COMP_LEGT,     // >=
  TT_COMP_RGT,      // <
  TT_COMP_REGT,     // <=
  TT_COMP_EQUALS,   // ==
  TT_COMP_NOTEQUALS,// !=
  TT_ARITH_PLUS,    // + 
  TT_ARITH_MINUS,   // -
  TT_ARITH_MULTI,   // *
  TT_ARITH_DIVIDE,  // /
  TT_ARITH_UMINUS,  // -
  TT_ASSIGN,        // =
  TT_LPAREN,        // (
  TT_RPAREN,        // )
  TT_LCURLY,        // {
  TT_RCURLY,        // }
  TT_COMMA,         // ,
  TT_COLON,         // ;
  TT_IF,            // if
  TT_IFELSE,        // if-else
  TT_WHILE,         // while
  TT_RETURN,        // return
  TT_ELSE,          // else
  TT_PRINT,         // print
  TT_DEF,           // def
  TT_LOCAL,         // local
  TT_PROCEDURE_CALL,// procedure call
  TT_ARGS,          // arguments
  TT_BLOCK          // block
} token_type_t;

typedef struct token_data {
  token_type_t type;
  union {
    int intval;
    double doubleval;
    char* symval;
    char* argval;
    char* funstr;
  } value;
} token_data_t;

#endif