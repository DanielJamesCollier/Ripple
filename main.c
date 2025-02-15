#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <unistd.h>  // For usleep, chdir, getcwd
#include <sys/ioctl.h> // For terminal size
#include <string.h>   // For strlen
#include <stdlib.h>   // For exit()
#include <signal.h>   // For signal handling

#define BLUE_BG "\033[44m"
#define RESET   "\033[0m"
#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"
#define CLEAR_SCREEN "\033[H\033[2J\033[3J"

#define ON_DELAY  700000  // Blue cursor duration (700ms)
#define OFF_DELAY 400000  // No cursor duration (400ms)

void cleanup(int signum) {
  (void)signum;
  printf(CLEAR_SCREEN SHOW_CURSOR "\n");
  fflush(stdout);
  exit(0);
}

// Function to get terminal size
void get_terminal_size(int *rows, int *cols) {
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  *rows = w.ws_row;
  *cols = w.ws_col;
}

// Function to center text
void center_text(const char *text) {
  int rows, cols;
  get_terminal_size(&rows, &cols);

  int text_length = strlen(text) + 1; // +1 for the blinking space
  int padding_left = (cols - text_length) / 2;
  int padding_top = rows / 2;

  // Clear screen and move cursor to top-left
  printf(CLEAR_SCREEN);

  // Move cursor to the center row
  printf("\033[%d;1H", padding_top);

  // Print leading spaces to center horizontally
  for (int i = 0; i < padding_left; i++) {
    printf(" ");
  }
}

// Function to display cwd and handle user input
void handle_terminal() {
  char cwd[1024];
  char input[256];

  while (1) {
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
      printf("\033[%d;1H%s\n> ", 999, cwd);  // Move cursor to bottom-left, print cwd
      fflush(stdout);
    } else {
      perror("getcwd() error");
    }

    if (fgets(input, sizeof(input), stdin) == NULL) {
      printf("\n");
      exit(0);  // Exit on EOF (Ctrl+D)
    }

    // Remove trailing newline
    input[strcspn(input, "\n")] = 0;

    // Handle 'cd' command
    if (strncmp(input, "cd ", 3) == 0) {
      char *path = input + 3;
      if (chdir(path) != 0) {
        perror("cd failed");
      }
    } else if (strcmp(input, "exit") == 0) {
      break;  // Exit terminal
    } else {
      printf("Command not found: %s\n", input);
    }
  }
}

int main(void) {
  signal(SIGINT, cleanup); // Handle Ctrl+C

  printf(HIDE_CURSOR);  // Hide the cursor

  // Perform splash animation
  for (int i = 0; i < 5; i++) { // Run for 5 cycles, then stop
    center_text("Ripple Terminal");

    printf("Ripple Terminal" BLUE_BG " " RESET);  // Print text with blue background space
    fflush(stdout);
    usleep(ON_DELAY);  // Wait 700ms

    center_text("Ripple Terminal");  // Reset cursor to center
    printf("Ripple Terminal ");  // Print text without blue space
    fflush(stdout);
    usleep(OFF_DELAY);  // Wait 400ms
  }

  // Move to top-left before clearing
  printf(CLEAR_SCREEN);
  printf(SHOW_CURSOR);  // Restore cursor

  handle_terminal();  // Start shell loop

  return 0;
}
