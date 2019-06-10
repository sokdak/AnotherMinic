#include <stdio.h>
#include "include/log.h"

extern symbol_t *sym_global;

void debug(char* msg, const char* tag) {
  fprintf(stderr, "[debug:%s] %s\n", tag, msg);
}

void full_log(program_t* program) {
  if (program->function) {
    fprintf(stderr, "[i] information of function %s\n", program->function->name);
    fprintf(stderr, "local(%p)::", program->function->ast->localvar);
    print_symboltable(program->function->ast->localvar);
  }

  fprintf(stderr, "global(%p)::", sym_global);
  print_symboltable(sym_global);
}