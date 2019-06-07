#include <stdlib.h>
#include "include/function.h"

function_t* append_func(function_t* fun, function_t* org) {
  function_t* ret_func = org;
  function_t* list = ret_func->next;
  
  while (list != NULL) // NULL arg가 나올때 까지 반복
    list = list->next;

  list = fun;

  return ret_func;
}

int find_function(char* name) {
  // function을 찾아서 idx 리턴
}