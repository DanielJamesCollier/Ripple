#if defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/ioctl.h>
#endif

#include "defines.h"
#include "common.h"
#include "cd.h"
#include "dir.h"
#include "splash.h"
#include "new.h"
#include "del.h"
#include "mkdir.h"
#include "gui.h"
#include "clear.h"

#define MAX_ARGS 1024

enum CommandType {
  CMD_CD,
  CMD_DIR,
  CMD_SPLASH,
  CMD_EXIT,
  CMD_CMD,
  CMD_NEW,
  CMD_MKDIR,
  CMD_DEL,
  CMD_GUI,
  CMD_CLEAR,
  CMD_EMPTY,
  CMD_ERROR,
};

void cleanup(int signum) {
  (void)signum;
  printf(CLEAR_SCREEN SHOW_CURSOR "\n");
  fflush(stdout);
  exit(0);
}

static enum CommandType parse_command(const char *input, char *args, int args_size) {
  assert(input);
  assert(args);

  while (*input == ' ') {
    ++input;
  }

  if (*input == '\0') {
    return CMD_EMPTY; 
  }

  size_t input_len = strlen(input);

  if (input_len > args_size) {
    fprintf(stderr, "Error. Input too long.");
    return CMD_ERROR;
  }
  
  if (strncmp(input, "cd", 2) == 0) {
    strcpy(args, input + 2);
    return CMD_CD;
  } else if (strncmp(input, "dir", 3) == 0) {
    return CMD_DIR;
  } else if (strncmp(input, "ls", 2) == 0) {
    return CMD_DIR;
  } else if (strcmp(input, "splash") == 0) {
    return CMD_SPLASH;
  } else if (strcmp(input, "exit") == 0) {
    return CMD_EXIT;
  } else if (strncmp(input, "new", 3) == 0) {
    strcpy(args, input + 4);
    return CMD_NEW;
  } else if (strncmp(input, "mkdir", 5) == 0) {
    strcpy(args, input + 6);
    return CMD_MKDIR;
  } else if (strncmp(input, "del", 3) == 0) {
    strcpy(args, input + 4);
    return CMD_DEL;
  } else if (strncmp(input, "gui", 3) == 0) {
    return CMD_GUI;
  } else if (strncmp(input, "clear", 5) == 0) {
    return CMD_CLEAR;
  } else if (strncmp(input, "cls", 3) == 0)  {
    return CMD_CLEAR;
  }
  return CMD_CMD;
}

static void handle_terminal() {
  char cwd[1024];
  char input[256];
  int print_cwd = 1;

  while (1) {
    if (print_cwd == 1) {
      if (get_cwd(cwd, sizeof(cwd)) == 0) {
        printf("\033[%d;1H%s\n", 999, cwd);
        fflush(stdout);
      }
    } else {
      print_cwd = 1;
    }
    printf("> ");
    fflush(stdout);

    if (fgets(input, sizeof(input), stdin) == NULL) {
      printf("\n");
      exit(0);
    }

    input[strcspn(input, "\n")] = 0;

    char args[1024];
    enum CommandType cmd = parse_command(input, args, sizeof(args));
    switch(cmd) {
      case CMD_CD:     { cd(args);               } break;
      case CMD_DIR:    { dir();                  } break;
      case CMD_NEW:    { new(args);              } break;
      case CMD_MKDIR:  { mkdir_cmd(args);        } break;
      case CMD_DEL:    { del(args);              } break;
      case CMD_SPLASH: { splash();               } break;
      case CMD_GUI:    { gui();                  } break;
      case CMD_EMPTY:  { print_cwd = 0;          } break;
      case CMD_CLEAR:  { clear();                } break;
      case CMD_EXIT:   { putchar('\n'); exit(0); } break;
      case CMD_CMD:    { printf("todo cmd\n\n"); } break;
      case CMD_ERROR:  { continue;               } break;
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
}

int main(void) {
  signal(SIGINT, cleanup);
  enable_vt_codes();
  splash();

  printf("\033[%d;1H%s\n", 999, PRODUCT_NAME " " VERSION "\nby Daniel Collier\n");

  handle_terminal();
  return 0;
}

