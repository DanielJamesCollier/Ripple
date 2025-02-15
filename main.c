#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#ifdef _WIN32
#include <windows.h>
#define ENABLE_VT_CODES() do { \
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); \
    DWORD dwMode = 0; \
    GetConsoleMode(hOut, &dwMode); \
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING); \
  } while (0)
#define SLEEP(ms) Sleep(ms)
#define GET_CWD _getcwd
#define CHDIR _chdir
#else
#include <unistd.h>
#include <sys/ioctl.h>
#define ENABLE_VT_CODES() ((void)0)
#define SLEEP(ms) usleep((ms) * 1000)
#define GET_CWD getcwd
#define CHDIR chdir
#endif

#define BLUE_BG "\033[44m"
#define RESET "\033[0m"
#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"
#define CLEAR_SCREEN "\033[H\033[2J\033[3J"

#define ON_DELAY  700  // Blue cursor duration (700ms)
#define OFF_DELAY 400  // No cursor duration (400ms)

void cleanup(int signum) {
    (void)signum;
    printf(CLEAR_SCREEN SHOW_CURSOR "\n");
    fflush(stdout);
    exit(0);
}

// Function to get terminal size
void get_terminal_size(int* rows, int* cols) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
    else {
        *cols = 80; // Default fallback
        *rows = 24;
    }
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *rows = w.ws_row;
    *cols = w.ws_col;
#endif
}

// Function to center text
void center_text(const char* text) {
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
void handle_terminal() {
    char cwd[1024];
    char input[256];

    while (1) {
        if (GET_CWD(cwd, sizeof(cwd)) != NULL) {
            printf("\033[%d;1H%s\n> ", 999, cwd);
            fflush(stdout);
        }
        else {
            perror("getcwd() error");
        }

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            exit(0);
        }

        input[strcspn(input, "\n")] = 0;

        if (strncmp(input, "cd ", 3) == 0) {
            char* path = input + 3;
            if (CHDIR(path) != 0) {
                perror("cd failed");
            }
        }
        else if (strcmp(input, "exit") == 0) {
            break;
        }
        else {
            printf("Command not found: %s\n", input);
        }
    }
}

int main(void) {
    signal(SIGINT, cleanup);
    ENABLE_VT_CODES();

    printf(HIDE_CURSOR);

    // this fixes flashing when we are writing multiple lines
    // and the stoud randomly gets flushed before we finished writing.
    setvbuf(stdout, NULL, _IOFBF, 16384);

    for (int i = 0; i < 3; i++) {
        center_text("Ripple Terminal");
        printf("Ripple Terminal" BLUE_BG " " RESET);
        fflush(stdout);
        SLEEP(ON_DELAY);

        center_text("Ripple Terminal");
        printf("Ripple Terminal ");
        fflush(stdout);
        SLEEP(OFF_DELAY);
    }

    printf(CLEAR_SCREEN);
    printf(SHOW_CURSOR);

    handle_terminal();

    return 0;
}

