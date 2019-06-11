#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/function.h"
#include "include/symbol.h"

function_t* append_func(function_t* fun, function_t* org) {
  function_t* ret_func = org;
  
  while (ret_func->next != NULL)
    ret_func = ret_func->next; // NULL arg가 나올때 까지 반복

  ret_func->next = fun;

  return org;
}

function_t* get_function(char* name, function_t* list) {
  function_t* tmp = list;

  while (tmp != NULL) {
    if (strcmp(tmp->name, name) == 0)
      return tmp;

    tmp = tmp->next;
  }

  return NULL;
}

void print_functions(function_t* list) {
  function_t* func = list;

  fprintf(stderr, "print functionlist:: start\n");

  while (func != NULL) {
    fprintf(stderr, "  [function:%s] defined: %d, arguments: %p, context: %p, lsym: %p\n",
      func->name, func->defined, func->arguments, func->ast, func->ast->localvar);
    
    print_symboltable(func->ast->localvar);

    func = func->next;
  }

  fprintf(stderr, "print functionlist:: end\n");
}