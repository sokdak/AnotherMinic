#include <stdio.h>
#include <stdlib.h>
#include "include/minic.h"

int main(int argc, char *argv[])
{
   int i, flag;

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
   fclose(yyin);

   return flag;
}