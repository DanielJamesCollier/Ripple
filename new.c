#include "new.h"

#include <stdio.h>
#include <assert.h>

// When no file name is provided to the 
// "new" command, we will use the default name.
static const char *default_name = "file.txt";

static void new_internal(const char *file_name) {
  assert(file_name);

  FILE *file = fopen(file_name, "r");
  if (file) {
    fclose(file);
    fprintf(stderr, "Error: File %s already exists.\n\n", file_name);
    return;
  }

  file = fopen(file_name, "w");
  if (!file) {
    fprintf(stderr, "Error: Could not create %s.\n\n", file_name);
    return;
  }

  printf("File %s was created.\n\n", file_name);
  fclose(file);
  return;
}

void new(const char *file_name) {
  assert(file_name);

  while (*file_name == ' ') {
    ++file_name; 
  }

  if (*file_name == '\0') {
    new_internal(default_name);
  } else {
    new_internal(file_name);
  }
}
