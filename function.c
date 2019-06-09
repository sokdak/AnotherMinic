#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/function.h"

function_t* append_func(function_t* fun, function_t* org) {
  function_t* ret_func = org;
  function_t* list = ret_func->next;
  
  while (list != NULL) // NULL arg가 나올때 까지 반복
    list = list->next;

  list = fun;

  return ret_func;
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
