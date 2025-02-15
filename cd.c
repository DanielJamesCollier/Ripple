#include "cd.h"

#include <stdio.h>
#include <assert.h>

#if defined(_WIN32)
#include <direct.h>
#else
#include <unistd.h>
#endif

void cd(const char *str) {
  assert(str);
  while (*str == ' ') ++str;

#ifdef _WIN32
  if (_chdir(str) != 0) {
    printf("cd failed. Directory not found.\n");
  }
#else
  if (chdir(str) != 0) {
    printf("cd failed. Directory not found.\n");
  }
  putchar('\n');
#endif
}
