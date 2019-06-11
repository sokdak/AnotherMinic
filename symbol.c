#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "include/symbol.h"
#include "include/token.h"
#include "include/opt.h"

symbol_t** vblTable_stack = NULL;
int vblTable_stack_depth = 0;

int find_symbol(char* name, symbol_t* table) {
  symbol_t* tmp = table;
  int idx = -1;

  while (tmp != NULL) {
    if (strcmp(tmp->name, name) == 0)
      return idx + 1;

    tmp = tmp->next;
    idx++;
  }

  return -1;
}

int get_list_size(symbol_t* list) {
  symbol_t* tmp = list;
  int length = 0;

  while (tmp != NULL) {
    length++;
    tmp = tmp->next;
  }

  return length;
}

symbol_t* get_symbol(int idx, symbol_t* list) {
  symbol_t* tmp = list;
  int iidx = 0;

  while (tmp != NULL) {
    if (iidx == idx)
      return tmp;

    tmp = tmp->next;
    iidx++;
  }

  return NULL;
}

symbol_t* get_symbol_by_name(char* name, symbol_t* list) {
  symbol_t* tmp = list;

  while (tmp != NULL) {
    if (strcmp(name, tmp->name) == 0)
      return tmp;

    tmp = tmp->next;
  }

  return NULL;
}

symbol_t* last_symbol(symbol_t* list) {
  symbol_t* tmp = list;

  while (tmp->next != NULL)
    tmp = tmp->next;
  
  return tmp;
}

// 심볼테이블에 자리만들기만 해주면 됨 (값 할당 x)
int insert_symbol(char* name, symbol_t** list) {
  int sindex = -1;
  int list_size = get_list_size(*list);

  if (SYMBOL_DEBUG)
    fprintf(stderr, "[symbol] insert, list_size %d\n", list_size);

  if ((sindex = find_symbol(name, *list)) == -1) {
    *list = (symbol_t*)realloc(*list, (list_size + 1) * sizeof(symbol_t)); // 포인터 사이즈 변경

    if (SYMBOL_DEBUG)
      fprintf(stderr, "   !!! resize table to %d\n", list_size + 1);

    symbol_t* lsym;

    if (list_size > 0) {
      lsym = last_symbol(*list);
      lsym->next = (symbol_t*)malloc(sizeof(symbol_t));

      lsym->next->name = strdup(name);
      lsym->next->assigned = false;
      lsym->next->type = TYPE_VARIABLE_UNDEFINED;
      lsym->next->next = NULL;
    }
    else {
      lsym = *list;

      lsym->name = strdup(name);
      lsym->assigned = false;
      lsym->type = TYPE_VARIABLE_UNDEFINED;
      lsym->next = NULL;
    }

    sindex = list_size;

    if (SYMBOL_DEBUG)
      fprintf(stderr, "save symbol at idx %d\n", sindex);
  }
  else {
    if (SYMBOL_DEBUG)
      fprintf(stderr, "symbol definition at %d\n", sindex);
  }

  return sindex;
}

int insert_symbol_value(char* name, token_data_t data, symbol_t** list) {
  symbol_t* tmp = *list;
  
  while (tmp != NULL) {
    if (strcmp(name, tmp->name) == 0) {
      if (data.type == TT_INT)
        tmp->value.integer_constant = data.value.intval;
      else if (data.type == TT_FLOAT)
        tmp->value.real_constant = data.value.doubleval;
      else return -1;

      tmp->assigned = true;
    }

    tmp = tmp->next;
  }

  return -1;
}

bool symbol_exists_on_table(int idx, symbol_t* sym) {
  symbol_t* iter = sym;
  int fidx = -1; // real index
  int tidx = 0;

  while (iter != NULL) {
    if (fidx == tidx) {
      fidx = tidx;
      break;
    }

    tidx++;
    iter = iter->next;
  }

  return fidx != -1 ? true : false;
}

bool symbol_exists_on_table_by_name(char* name, symbol_t* sym) {
  if (SYMBOL_DEBUG)
    fprintf(stderr, "[symbol] name %s, sym %p\n", name, sym);
  
  symbol_t* iter = sym;

  while (iter != NULL) {
    if (strcmp(name, iter->name) == 0)
      return true;

    iter = iter->next;
  }

  return false;
}

void push_symboltable(symbol_t* sym) {
  vblTable_stack = (symbol_t**)realloc(vblTable_stack, sizeof(symbol_t) * (vblTable_stack_depth + 1));
  vblTable_stack[vblTable_stack_depth] = sym;

  if (SYMBOL_DEBUG)
    fprintf(stderr, "[symbol::push_table] pushed symboltable %p, idx %d\n", vblTable_stack[vblTable_stack_depth], vblTable_stack_depth);

  vblTable_stack_depth++;
}

symbol_t* pop_symboltable() {
  if (!stack_isempty()) {
    vblTable_stack_depth--;

    if (SYMBOL_DEBUG)
      fprintf(stderr, "[symbol::pop_table] popped symboltable %p, idx %d\n", vblTable_stack[vblTable_stack_depth], vblTable_stack_depth);

    return vblTable_stack[vblTable_stack_depth];
  }
  else {
    if (SYMBOL_DEBUG)
      fprintf(stderr, "[!] tried to pop symbol from empty stack\n");

    return NULL;
  }
}

bool stack_isempty() {
  if (vblTable_stack_depth > 0) return false;
  else return true;
}

symbol_t* hardcopy_symbollist(symbol_t* orglist) {
  if (SYMBOL_DEBUG)
    fprintf(stderr, "[symbol] hardcopy init\n");
  
  symbol_t *node, *cur;
  symbol_t *tmp;

  tmp = orglist;

  node = (symbol_t*)malloc(sizeof(symbol_t));
  node->name = strdup(orglist->name);
  node->type = orglist->type;
  node->value = orglist->value;
  node->next = NULL;

  cur = node;

  tmp = tmp->next;

  while (tmp != NULL) {
    cur->next = (symbol_t*)malloc(sizeof(symbol_t));
    
    cur = cur->next;
    cur->name = strdup(tmp->name);
    cur->type = tmp->type;
    cur->value = tmp->value;
    cur->next = NULL;

    tmp = tmp->next;
  }

  return node;
}

void print_symboltable(symbol_t* list) {
  symbol_t* sym = list;

  fprintf(stderr, "print symboltable:: start\n");

  while (sym != NULL) {
    if (sym->type != TYPE_VARIABLE_INT) {
      fprintf(stderr, "  [symbol:%s] assigned: %d, type: %d, value: %f\n", 
        sym->name, sym->assigned, sym->type, sym->value.real_constant);
    }
    else {
      fprintf(stderr, "  [symbol:%s] assigned: %d, type: %d, value: %d\n", 
        sym->name, sym->assigned, sym->type, sym->value.integer_constant);
    }
    
    sym = sym->next;
  }

  fprintf(stderr, "print symboltable:: end\n");
}