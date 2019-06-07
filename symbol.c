#include <stdlib.h>
#include <string.h>
#include "include/symbol.h"
#include "include/token.h"

symbol_t* global_vblTable = NULL;

int find_symbol(char* name, symbol_t* table) {
  return -1;
}

int get_list_size(symbol_t* list) {
  symbol_t* tmp = list;
  int length = 0;

  if (tmp != NULL) {
    length++;
    tmp = tmp->next;
  }

  return length;
}

// 심볼테이블에 자리만들기만 해주면 됨 (값 할당 x)
int insert_symbol(char* name, symbol_t** list) {
  int sindex = -1;
  int list_size = get_list_size(*list);

  if ((sindex = find_symbol(name, *list)) == -1) {
    *list = (symbol_t*)realloc(*list, (list_size + 1) * sizeof(symbol_t)); // 포인터 사이즈 변경

    symbol_t* addr = *list;

    if (list_size > 0) {
      addr = *list + list_size - 1; // 추가하기전 마지막 element
      addr->next = addr + 1;       // next를 할당해주고
      addr = addr->next;           // next로 이동
    }
    
    addr->name = strdup(name);
    addr->assigned = false;
    addr->type = TYPE_VARIABLE_UNDEFINED;

    sindex = list_size;
  }

  return sindex;
}

int insert_symbol_value(int index, token_data_t data, symbol_t** list) {
  if (data.type == TT_INT)
    (*list[index]).value.integer_constant = data.value.intval;
  else if (data.type == TT_FLOAT)
    (*list[index]).value.real_constant = data.value.doubleval;
  else return -1;

  (*list[index]).assigned = true;
  return index;
}