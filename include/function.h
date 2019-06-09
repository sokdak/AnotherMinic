#ifndef AS3_FUNCTION_H
#define AS3_FUNCTION_H

#include <stdbool.h>
#include "node.h"

#define EVAL_DEBUG true

typedef struct function {
  int id;
  char *name;
  bool defined;
  
  ast_tree_t* arguments;

  ast_tree_t* ast;
  struct function *next;
} function_t;

function_t* append_func(function_t *, function_t *);
function_t* get_function(char *, function_t *);

#endif