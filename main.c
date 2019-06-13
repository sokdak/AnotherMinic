#include <stdio.h>
#include <stdlib.h>
#include "include/opt.h"
#include "include/minic.h"
#include "include/function.h"
#include "include/eval.h"
#include "include/log.h"

program_t* program;

// 글로벌 vbltable 초기화해주기
int main(int argc, char *argv[])
{
  int i, flag;

  program = NULL;

  if (argc < 2) {
    printf("Usage: ./minic source.mc\n");
    exit(0);
  }

  FILE *fp = fopen(argv[1], "r");

  if (!fp) {
    printf("Error occured while opening file to parse\n");
    exit(0);
  }

  yyin = fp;
  flag = yyparse();

  if (program != NULL)
    eval(program);
  else fprintf(stderr, "Error(s) are occured while parse minic file.\nPlease fix these problems above and try again.\n");

  if (program != NULL && MAIN_DEBUG)
    full_log(program);

  fclose(yyin);

 return flag;
}