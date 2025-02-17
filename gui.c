#include "gui.h"

#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
#include <Windows.h>
#include <direct.h>
#pragma comment(lib, "Shell32.lib")
#endif // _WIN32

#if defined(__APPLE__)
#include <stdio.h>
#include <unistd.h>
#endif // __APPLE__

#if defined(_WIN32)
static void gui_windows() {
  char cwd[MAX_PATH];
  
  // Get the current working directory
  if (!_getcwd(cwd, sizeof(cwd))) {
    fprintf(stderr, "Error: Could not get current working directory.\n\n");
    return;
  }

  // Open File Explorer at the current working directory
  ShellExecute(NULL, "open", cwd, NULL, NULL, SW_SHOWNORMAL);
}
#endif

#if defined(__APPLE__)
static void gui_mac() {
  char cwd[1024];

  // Get the current working directory
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    fprintf(stderr, "Error: Could not get current working directory.\n\n");
    return;
  }

  // Open Finder at the current working directory
  char command[1060]; // Extra space for "open " and safety
  snprintf(command, sizeof(command), "open \"%s\"", cwd);
  system(command); 
}
#endif // __APPLE__

void gui() {
#if defined(_WIN32)
  gui_windows();
#endif // _WIN32

#if defined(__APPLE__)
  gui_mac();
#endif // __APPLE__
  putchar('\n');
}
