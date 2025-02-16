#include "new.h"

#include <stdio.h>
#include <assert.h>

// When no filename is provided to the 
// new command, we will use the default name.
static const char *default_name = "file.txt";

static void new_internal(const char *filename) {
  assert(filename);

  FILE *file = fopen(filename, "r");
  if (file) {
    fclose(file);
    fprintf(stderr, "Error: File %s already exists.\n\n", filename);
    return;
  }

  file = fopen(filename, "w");
  if (!file) {
    fprintf(stderr, "Error: Could not create %s.\n\n", filename);
    return;
  }

  printf("File %s was created.\n\n", filename);
  fclose(file);
  return;
}

void new(const char *filename) {
  assert(filename);

  while (*filename == ' ') {
    ++filename; 
  }

  if (*filename == '\0') {
    new_internal(default_name);
  } else {
    new_internal(filename);
  }
}
