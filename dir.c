#include "dir.h"

#include <stdio.h>

#if defined(_WIN32)
static void dir_windows() {

}
#else
static void dir_mac_linux() {

}
#endif

void dir() {

#if defined(_WIN32)
  dir_windows();
#else
  dir_mac_linux();
#endif

  putchar('\n');
}

