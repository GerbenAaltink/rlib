#ifndef RTERM_H
#define RTERM_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <string.h>
#include "rio.h"
#include "rtime.h"
typedef struct winsize winsize_t;

typedef struct rshell_keypress_t {
    bool pressed;
    bool ctrl;
    bool shift;
    bool escape;
    char c;
    int ms;
    int fd;
} rshell_keypress_t;

typedef struct rterm_t {
    bool show_cursor;
    bool show_footer;
    int ms_tick;
    rshell_keypress_t key;
    void (*before_cursor_move)(struct rterm_t *);
    void (*after_cursor_move)(struct rterm_t *);
    void (*after_key_press)(struct rterm_t *);
    void (*before_key_press)(struct rterm_t *);
    void (*before_draw)(struct rterm_t *);
    void (*after_draw)(struct rterm_t *);
    void *session;
    unsigned long iterations;
    void (*tick)(struct rterm_t *);
    char *status_text;
    char *_status_text_previous;
    winsize_t size;
    struct {
        int x;
        int y;
        int pos;
        int available;
    } cursor;
} rterm_t;

typedef void (*rterm_event)(rterm_t *);

void rterm_init(rterm_t *rterm) {
    memset(rterm, 0, sizeof(rterm_t));
    rterm->show_cursor = true;
    rterm->cursor.x = 0;
    rterm->cursor.y = 0;
    rterm->ms_tick = 100;
    rterm->_status_text_previous = NULL;
}

void rterm_getwinsize(winsize_t *w) {
    // Get the terminal size
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, w) == -1) {
        perror("ioctl");
        exit(EXIT_FAILURE);
    }
}

void rrawfd(int fd) {
    struct termios orig_termios;
    tcgetattr(fd, &orig_termios); // Get current terminal attributes

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ISIG | ECHO); // ECHO // Disable canonical mode and echoing
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 240; // Set timeout for read input

    tcsetattr(fd, TCSAFLUSH, &raw);
}

// Terminal setup functions
void enableRawMode(struct termios *orig_termios) {

    struct termios raw = *orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echoing
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 240; // Set timeout for read input

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode(struct termios *orig_termios) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH,
              orig_termios); // Restore original terminal settings
}

void rterm_clear_screen() {
    printf("\x1b[2J"); // Clear the entire screen
    printf("\x1b[H");  // Move cursor to the home position (0,0)
}

void setBackgroundColor() {
    printf("\x1b[34m"); // Set background color to blue
}

void rterm_move_cursor(int x, int y) {

    printf("\x1b[%d;%dH", y + 1, x + 1); // Move cursor to (x, y)
}

void cursor_set(rterm_t *rt, int x, int y) {
    rt->cursor.x = x;
    rt->cursor.y = y;
    rt->cursor.pos = y * rt->size.ws_col + x;
    rterm_move_cursor(rt->cursor.x, rt->cursor.y);
}
void cursor_restore(rterm_t *rt) { rterm_move_cursor(rt->cursor.x, rt->cursor.y); }

void rterm_print_status_bar(rterm_t *rt, char c, unsigned long i) {
    if (rt->_status_text_previous && !strcmp(rt->_status_text_previous, rt->status_text)) {
        return;
    }
    if (rt->_status_text_previous) {
        free(rt->_status_text_previous);
    }
    rt->_status_text_previous = strdup(rt->status_text);
    winsize_t ws = rt->size;
    cursor_set(rt, rt->cursor.x, rt->cursor.y);
    rterm_move_cursor(0, ws.ws_row - 1);

    char output_str[1024];
    output_str[0] = 0;

    // strcat(output_str, "\x1b[48;5;240m");

    for (int i = 0; i < ws.ws_col; i++) {
        strcat(output_str, " ");
    }
    char content[500];
    content[0] = 0;
    if (!rt->status_text) {
        sprintf(content, "\rp:%d:%d | k:%c:%d | i:%ld ", rt->cursor.x + 1, rt->cursor.y + 1, c == 0 ? '0' : c, c, i);
    } else {
        sprintf(content, "\r%s", rt->status_text);
    }
    strcat(output_str, content);
    // strcat(output_str, "\x1b[0m");
    printf("%s", output_str);
    cursor_restore(rt);
}

void rterm_show_cursor() {
    printf("\x1b[?25h"); // Show the cursor
}

void rterm_hide_cursor() {
    printf("\x1b[?25l"); // Hide the cursor
}

rshell_keypress_t rshell_getkey(rterm_t *rt) {
    static rshell_keypress_t press;
    press.c = 0;
    press.ctrl = false;
    press.shift = false;
    press.escape = false;
    press.pressed = rfd_wait(0, rt->ms_tick);
    if (!press.pressed) {
        return press;
    }
    press.c = getchar();
    char ch = press.c;
    if (ch == '\x1b') {
        // Get detail
        ch = getchar();

        if (ch == '[') {
            // non char key:
            press.escape = true;

            ch = getchar(); // is a number. 1 if shift + arrow
            press.c = ch;
            if (ch >= '0' && ch <= '9')
                ch = getchar();
            press.c = ch;
            if (ch == ';') {
                ch = getchar();
                press.c = ch;
                if (ch == '5') {
                    press.ctrl = true;
                    press.c = getchar(); // De arrow
                }
            }
        } else if (ch == 27) {
            press.escape = true;
            press.c = ch;
        } else {
            press.c = ch;
        }
    }
    return press;
}

// Main function
void rterm_loop(rterm_t *rt) {
    struct termios orig_termios;
    tcgetattr(STDIN_FILENO, &orig_termios); // Get current terminal attributes
    enableRawMode(&orig_termios);

    int x = 0, y = 0; // Initial cursor position
    char ch = 0;

    ;
    while (1) {
        rterm_getwinsize(&rt->size);
        rt->cursor.available = rt->size.ws_col * rt->size.ws_row;
        if (rt->tick) {
            rt->tick(rt);
        }

        rterm_hide_cursor();
        setBackgroundColor();
        rterm_clear_screen();
        if (rt->before_draw) {
            rt->before_draw(rt);
        }
        rterm_print_status_bar(rt, ch, rt->iterations);
        if (rt->after_draw) {
            rt->after_draw(rt);
        }
        if (!rt->iterations || (x != rt->cursor.x || y != rt->cursor.y)) {
            if (rt->cursor.y == rt->size.ws_row) {
                rt->cursor.y--;
            }
            if (rt->cursor.y < 0) {
                rt->cursor.y = 0;
            }
            x = rt->cursor.x;
            y = rt->cursor.y;
            if (rt->before_cursor_move)
                rt->before_cursor_move(rt);
            cursor_set(rt, rt->cursor.x, rt->cursor.y);
            if (rt->after_cursor_move)
                rt->after_cursor_move(rt);
            // x = rt->cursor.x;
            // y = rt->cursor.y;
        }
        if (rt->show_cursor)
            rterm_show_cursor();

        fflush(stdout);

        rt->key = rshell_getkey(rt);
        if (rt->key.pressed && rt->before_key_press) {
            rt->before_key_press(rt);
        }
        rshell_keypress_t key = rt->key;
        ch = key.c;
        if (ch == 'q')
            break; // Press 'q' to quit
        if (key.c == -1) {
            nsleep(1000 * 1000);
        }
        // Escape
        if (key.escape) {
            switch (key.c) {
            case 65: // Move up
                if (rt->cursor.y > -1)
                    rt->cursor.y--;
                break;
            case 66: // Move down
                if (rt->cursor.y < rt->size.ws_row)
                    rt->cursor.y++;
                break;
            case 68: // Move left
                if (rt->cursor.x > 0)
                    rt->cursor.x--;
                if (key.ctrl)
                    rt->cursor.x -= 4;
                break;
            case 67: // Move right
                if (rt->cursor.x < rt->size.ws_col) {
                    rt->cursor.x++;
                }
                if (key.ctrl) {
                    rt->cursor.x += 4;
                }
                break;
            }
        }

        if (rt->key.pressed && rt->after_key_press) {
            rt->after_key_press(rt);
        }
        rt->iterations++;

        //  usleep (1000);
    }

    // Cleanup
    printf("\x1b[0m"); // Reset colors
    rterm_clear_screen();
    disableRawMode(&orig_termios);
}
#endif
