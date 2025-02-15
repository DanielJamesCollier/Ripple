#include "cd.h"

#include <stdio.h>
#include <assert.h>

#if defined(_WIN32)
#include <direct.h>
#else
#include <unistd.h>
#endif

#include "common.h"

void cd(const char *str) {
  assert(str);

  while (*str == ' ') ++str;

  if (change_directory(str) != 0) {
    printf("cd failed. Directory not found.\n");
  }

  putchar('\n');
}
