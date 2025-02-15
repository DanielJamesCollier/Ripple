#include "splash.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include "defines.h"

static void ripple_sleep(int ms) {
#if defined(_WIN32)
  Sleep(ms);
#else
  usleep(ms * 1000);
#endif
}


static void get_terminal_size(int* rows, int* cols) {
#ifdef _WIN32
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
    *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
  } else {
    // Default fallback
    *cols = 80; 
    *rows = 24;
  }
#else
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  *rows = w.ws_row;
  *cols = w.ws_col;
#endif
}


static void center_text(const char* text) {
  int rows, cols;
  get_terminal_size(&rows, &cols);

  int text_length = strlen(text) + 1; // +1 for the blinking space
  int padding_left = (cols - text_length) / 2;
  int padding_top = rows / 2;

  printf(CLEAR_SCREEN);
  printf("\033[%d;1H", padding_top);

  for (int i = 0; i < padding_left; i++) {
    printf(" ");
  }
}


void splash() {
  printf(HIDE_CURSOR);

  for (int i = 0; i < 3; i++) {
    center_text("Ripple Terminal");
    printf("Ripple Terminal" BLUE_BG " " RESET);
    fflush(stdout);
    ripple_sleep(700);

    center_text("Ripple Terminal");
    printf("Ripple Terminal ");
    fflush(stdout);
    ripple_sleep(400);
  }

  printf(CLEAR_SCREEN);
  printf(SHOW_CURSOR);
}
