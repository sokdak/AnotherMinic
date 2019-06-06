#include <string.h>
#include <stdlib.h>
#include "include/util.h"

char* hardcopy_str(char* str) {
  char* buf;

  buf = (char *)malloc(strlen(str) * sizeof(char) + 1);
  strcpy(buf, str);

  return buf;
}