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

#define MAX_ARGS 1024

enum CommandType {
  CMD_CD,
  CMD_DIR,
  CMD_SPLASH,
  CMD_EXIT,
  CMD_CMD,
  CMD_NEW,
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

  size_t input_len = strlen(input);

  if (input_len > args_size) {
    fprintf(stderr, "Error. Input too long.");
    return CMD_ERROR;
  }
  
  if (strncmp(input, "cd", 2) == 0) {
    strcpy(args, input + 2);
    return CMD_CD;
  } else if (strcmp(input, "dir") == 0) {
    return CMD_DIR;
  } else if (strcmp(input, "splash") == 0) {
    return CMD_SPLASH;
  } else if (strcmp(input, "exit") == 0) {
    return CMD_EXIT;
  } else if (strncmp(input, "new", 3) == 0) {
    strcpy(args, input + 4);
    return CMD_NEW;
  }
  return CMD_CMD;
}

// Function to display cwd and handle user input
static void handle_terminal() {
  char cwd[1024];
  char input[256];

  while (1) {
    if (get_cwd(cwd, sizeof(cwd)) == 0) {
      printf("\033[%d;1H%s\n> ", 999, cwd);
      fflush(stdout);
    }

    if (fgets(input, sizeof(input), stdin) == NULL) {
      printf("\n");
      exit(0);
    }

    input[strcspn(input, "\n")] = 0;

    char args[1024];
    enum CommandType cmd = parse_command(input, args, sizeof(args));
    switch(cmd) {
      case CMD_CD:     { cd(args); } break;
      case CMD_DIR:    { dir();    } break;
      case CMD_NEW:    { new(args);} break;
      case CMD_SPLASH: { splash(); } break;
      case CMD_EXIT:   { exit(0);  } break;
      case CMD_CMD:    { printf("todo cmd\n\n"); } break;
      case CMD_ERROR:  { continue; } break;
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
  setvbuf(stdout, NULL, _IOFBF, 16384);

  splash();
  handle_terminal();
  return 0;
}

