#ifndef AS3_EVAL_H
#define AS3_EVAL_H

#include "program.h"
#include "symbol.h"

typedef enum rvalue_type {
  RVAL_INT,
  RVAL_REAL
} rvalue_type_t;

typedef struct rvalue {
  rvalue_type_t type;
  union {
    int ival;
    double dval;
  } value;
} rvalue_t;

void eval(program_t *, symbol_t *);
rvalue_t evaluation(ast_tree_t *);

#endif