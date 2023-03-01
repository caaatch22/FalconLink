#include "../include/util.hpp"
#include <stdio.h>
#include <stdlib.h>

void errif(bool condition, const char *errmsg) {
  if(condition) {
    perror(errmsg);
    exit(EXIT_FAILURE);
  }
}