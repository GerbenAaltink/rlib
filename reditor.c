#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

void rget_terminal_size(int *x, int *y) {
    struct winsize w;

    // Get terminal size
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    // Print terminal size
    // printf("Rows: %d, Columns: %d\n", w.ws_row, w.ws_col);

    // You can use this width in your program logic
    int terminal_width = w.ws_col;
    *x = w.ws_col;
    *y = w.ws_row;

    //  printf("Setting content width to half the terminal width:\n");

    // Example content that fits within half the terminal width
    //    printf("%.*s\n", terminal_width / 2, "This text is formatted to half
    //    the terminal width.");

}

struct termios rorig_termios;

// Restore original terminal settings
void reset_terminal_mode() { tcsetattr(STDIN_FILENO, TCSANOW, &rorig_termios); }

void set_raw_mode() {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &rorig_termios); // Get current terminal settings
    atexit(
        reset_terminal_mode); // Ensure original settings are restored on exit

    new_termios = rorig_termios;
    new_termios.c_lflag &=
        ~(ICANON | ECHO); // Disable canonical mode and echoing
    new_termios.c_cc[VMIN] =
        1; // Minimum number of characters for noncanonical read
    new_termios.c_cc[VTIME] = 0; // Timeout in deciseconds for noncanonical read

    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios); // Apply new settings
}

unsigned int read_key() {
    int nread;
    char c;
    if ((nread = read(STDIN_FILENO, &c, 1)) == -1)
        return -1;
    return c;
}

void rrclear() {
    printf("\033[2J"); // Clear screen
}

void rset_cursor_position(int x, int y) {
    // rrclear();
    printf("\033[%d;%dH", y, x);
}

void get_cursor_position(int *cols, int *rows) {
    char buf[32];
    unsigned int i = 0;

    // Request cursor position
    printf("\033[6n");

    // Read the response: ESC [ rows ; cols R
    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, buf + i, 1) != 1)
            break;
        if (buf[i] == 'R')
            break;
        i++;
    }
    buf[i] = '\0';

    // Parse the response
    if (buf[0] == '\033' && buf[1] == '[') {
        sscanf(buf + 2, "%d;%d", rows, cols);
    }
}

void run() {
    int c;
    int x = 3;
    int y = 3; // Initial y position
    int file_index = 0;
    set_raw_mode();
    printf("\033[2J"); // Clear screen
    rset_cursor_position(x, y);
    char screen_data[1024];

    int width, height;
    rget_terminal_size(&width, &height);

    screen_data[0] = 0;
    for (int i = 0; i < width * height; i++) { // screen_data[i] = '\0';
                                               // screen_data[i] = 0;
    }
    memset(&screen_data, 0, 2048);
    // printf(screen_data);

    while (1) {
        c = read_key();
        if (c == '\033') { // If the first character is ESC

            if (read_key() == '[') { // If the second character is '['
                rrclear();
                c = read_key();
                if (c == 'A') {
                    if (y) {
                        y--;
                    }
                    rset_cursor_position(x, y);
                } else if (c == 'B') {
                    if (y) {
                        y++;
                    }
                    rset_cursor_position(x, y);
                } else if (c == 'C') {

                    x++;

                    rset_cursor_position(x, y);

                } else if (c == 'D') {
                    x--;

                    rset_cursor_position(x, y);
                }
                printf(screen_data);
            }
        } else if (c == 'q') {
            break; // Press 'q' to quit
        } else {
            for (int i = 0; i < file_index; i++) {
                if (screen_data[i] == '\0') {
                    screen_data[i] = ' ';
                }
            }
            screen_data[file_index] = c;
            // file_index++;
            get_cursor_position(&x, &y);
            file_index = x * y;
            x++;
            // putc(c, stdout);
            // rrclear();
            rset_cursor_position(1, 1);
            // ss x++;
            printf(screen_data);
            rset_cursor_position(x, y);

            fflush(stdout);
        }
    }
}

int main() { run(); }
