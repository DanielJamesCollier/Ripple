#include "mkdir.h"

#include <assert.h>
#include <stdio.h>
#include <errno.h>

#if defined(_WIN32)
#include <Windows.h>
#endif // _WIN32

#if defined(__APPLE__)
#include <sys/stat.h>
#include <sys/types.h>
#endif // __APPLE__

#if defined(_WIN32)
static void mkdir_windows(const char *dir) {
  assert(dir);

  BOOL result = CreateDirectory(dir, NULL); 

  if (result != 0) {
    printf("Folder created.\n\n");
  } else {
    DWORD error = GetLastError();   
    if (result == ERROR_ALREADY_EXISTS) {
      fprintf(stderr, "Error: %s already exists.\n\n", dir);
    } else if (error == ERROR_PATH_NOT_FOUND) {
      fprintf(stderr, "Error: Intermediate folders not found.\n\n");
    }  
  } 
}
#endif // _WIN32

#if defined(__APPLE__)
static void mkdir_apple(const char *dir) {
  assert(dir);

  int result = mkdir(dir, 0777);

  if (result == 0) {
    printf("Folder created.\n\n");
  } else {
    if (errno == EEXIST) {
      fprintf(stderr, "Error: %s already exists.\n\n", dir);
    } else {
      fprintf(stderr, "Error creating folder.\n\n");
    }
  }
}
#endif // __APPLE__

void mkdir_cmd(const char *dir) {
  assert(dir);

  while (*dir == ' ') {
    ++dir;
  }

#if defined(_WIN32)
  mkdir_windows(dir);
#endif

#if defined(__APPLE__)
  mkdir_apple(dir);
#endif
}
