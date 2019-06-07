#ifndef AS3_PROGRAM_H
#define AS3_PROGRAM_H

#include "function.h"
#include "node.h"

typedef struct program {
  function_t* function;
  ast_tree_t* statement;
} program_t;

#endif