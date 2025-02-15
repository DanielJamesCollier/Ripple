#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#ifdef _WIN32
#include <Windows.h>
#define GET_CWD _getcwd
#define CHDIR _chdir
#else
#include <unistd.h>
#include <sys/ioctl.h>
#define GET_CWD getcwd
#define CHDIR chdir
#endif

#include "cd.h"

#define BLUE_BG "\033[44m"
#define RESET "\033[0m"
#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"
#define CLEAR_SCREEN "\033[H\033[2J\033[3J"
#define MAX_ARGS 1024

enum CommandType {
  CMD_CD,
  CMD_EXIT,
  CMD_CMD,
};


void cleanup(int signum) {
  (void)signum;
  printf(CLEAR_SCREEN SHOW_CURSOR "\n");
  fflush(stdout);
  exit(0);
}

static enum CommandType parse_command(const char *input, char *args) {
  if (strncmp(input, "cd", 2) == 0) {
    strcpy(args, input + 2);
    return CMD_CD;
  } else if (strcmp(input, "exit") == 0) {
    return CMD_EXIT;
  }
  return CMD_CMD;
}

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

// Function to display cwd and handle user input
static void handle_terminal() {
  char cwd[1024];
  char input[256];

  while (1) {
    if (GET_CWD(cwd, sizeof(cwd)) != NULL) {
      printf("\033[%d;1H%s\n> ", 999, cwd);
      fflush(stdout);
    } else {
      perror("getcwd() error");
    }

    if (fgets(input, sizeof(input), stdin) == NULL) {
      printf("\n");
      exit(0);
    }

    input[strcspn(input, "\n")] = 0;

    char args[1024];
    enum CommandType cmd = parse_command(input, args);
    switch(cmd) {
      case CMD_CD:   { cd(args); } break;
      case CMD_EXIT: { exit(0); } break;
      case CMD_CMD:  { printf("todo cmd\n"); } break;
    }
  }
}

static void enable_vt_codes() {
#if defined(_WIN32)
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD dwMode = 0;
  GetConsoleMode(hOut, &dwMode);
  SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
  // Not on by default on Mac and Linux.
}

int main(void) {
  signal(SIGINT, cleanup);
  enable_vt_codes();

  printf(HIDE_CURSOR);

  // So we can manually flush to reduce flicker.
  setvbuf(stdout, NULL, _IOFBF, 16384);

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
  handle_terminal();
  return 0;
}

