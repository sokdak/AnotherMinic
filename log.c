#include <stdio.h>
#include "include/log.h"

extern symbol_t *sym_global;

void debug(char* msg, const char* tag) {
  fprintf(stderr, "[debug:%s] %s\n", tag, msg);
}

void full_log(program_t* program) {
  if (program->function)
    print_functions(program->function);

  fprintf(stderr, "global(%p)::", sym_global);
  print_symboltable(sym_global);
}