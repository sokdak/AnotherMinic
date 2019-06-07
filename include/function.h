#ifndef AS3_FUNCTION_H
#define AS3_FUNCTION_H

#include <stdbool.h>
#include "node.h"

typedef struct function {
  int id;
  char *name;
  bool defined;
  
  ast_tree_t* arguments;
  int num_args;

  ast_tree_t* ast;
  struct function *next;
} function_t;

function_t* append_func(function_t *, function_t *);
int find_function(char *);

#endif