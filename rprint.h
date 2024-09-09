#ifndef RPRINT_H
#define RPRINT_H
#include "rtime.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

long rpline_number = 0;
nsecs_t rprtime = 0;

int8_t _env_rdisable_colors = -1;
bool _rprint_enable_colors = true;

bool rprint_is_color_enabled() {
    if (_env_rdisable_colors == -1) {
        _env_rdisable_colors = getenv("RDISABLE_COLORS") != NULL;
    }
    if (_env_rdisable_colors) {
        _rprint_enable_colors = false;
    }
    return _rprint_enable_colors;
}

void rprint_disable_colors() { _rprint_enable_colors = false; }
void rprint_enable_colors() { _rprint_enable_colors = true; }
void rprint_toggle_colors() { _rprint_enable_colors = !_rprint_enable_colors; }

void rclear() { printf("\033[2J"); }

void rprintpf(FILE *f, const char *prefix, const char *format, va_list args) {
    char *pprefix = (char *)prefix;
    char *pformat = (char *)format;
    bool reset_color = false;
    bool press_any_key = false;
    char new_format[4096];
    bool enable_color = rprint_is_color_enabled();
    memset(new_format, 0, 4096);
    int new_format_length = 0;
    char temp[1000];
    memset(temp, 0, 1000);
    if (enable_color && pprefix[0]) {
        strcat(new_format, pprefix);
        new_format_length += strlen(pprefix);
        reset_color = true;
    }
    while (true) {
        if (pformat[0] == '\\' && pformat[1] == 'i') {
            strcat(new_format, "\e[3m");
            new_format_length += strlen("\e[3m");
            reset_color = true;
            pformat++;
            pformat++;
        } else if (pformat[0] == '\\' && pformat[1] == 'u') {
            strcat(new_format, "\e[4m");
            new_format_length += strlen("\e[4m");
            reset_color = true;
            pformat++;
            pformat++;
        } else if (pformat[0] == '\\' && pformat[1] == 'b') {
            strcat(new_format, "\e[1m");
            new_format_length += strlen("\e[1m");
            reset_color = true;
            pformat++;
            pformat++;
        } else if (pformat[0] == '\\' && pformat[1] == 'C') {
            press_any_key = true;
            rpline_number++;
            pformat++;
            pformat++;
            reset_color = false;
        } else if (pformat[0] == '\\' && pformat[1] == 'k') {
            press_any_key = true;
            rpline_number++;
            pformat++;
            pformat++;
        } else if (pformat[0] == '\\' && pformat[1] == 'c') {
            rpline_number++;
            strcat(new_format, "\e[2J\e[H");
            new_format_length += strlen("\e[2J\e[H");
            pformat++;
            pformat++;
        } else if (pformat[0] == '\\' && pformat[1] == 'L') {
            rpline_number++;
            temp[0] = 0;
            sprintf(temp, "%ld", rpline_number);
            strcat(new_format, temp);
            new_format_length += strlen(temp);
            pformat++;
            pformat++;
        } else if (pformat[0] == '\\' && pformat[1] == 'l') {
            rpline_number++;
            temp[0] = 0;
            sprintf(temp, "%.5ld", rpline_number);
            strcat(new_format, temp);
            new_format_length += strlen(temp);
            pformat++;
            pformat++;
        } else if (pformat[0] == '\\' && pformat[1] == 'T') {
            nsecs_t nsecs_now = nsecs();
            nsecs_t end = rprtime ? nsecs_now - rprtime : 0;
            temp[0] = 0;
            sprintf(temp, "%s", format_time(end));
            strcat(new_format, temp);
            new_format_length += strlen(temp);
            rprtime = nsecs_now;
            pformat++;
            pformat++;
        } else if (pformat[0] == '\\' && pformat[1] == 't') {
            rprtime = nsecs();
            pformat++;
            pformat++;
        } else {
            new_format[new_format_length] = *pformat;
            new_format_length++;
            if (!*pformat)
                break;

            // printf("%c",*pformat);
            pformat++;
        }
    }
    if (reset_color) {
        strcat(new_format, "\e[0m");
        new_format_length += strlen("\e[0m");
    }

    new_format[new_format_length] = 0;
    vfprintf(f, new_format, args);

    fflush(stdout);
    if (press_any_key) {
        nsecs_t s = nsecs();
        fgetc(stdin);
        rprtime += nsecs() - s;
    }
}

void rprintp(const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(stdout, "", format, args);
    va_end(args);
}

void rprintf(FILE *f, const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(f, "", format, args);
    va_end(args);
}
void rprint(const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(stdout, "", format, args);
    va_end(args);
}
#define printf rprint

// Print line
void rprintlf(FILE *f, const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(f, "\\l", format, args);
    va_end(args);
}
void rprintl(const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(stdout, "\\l", format, args);
    va_end(args);
}

// Black
void rprintkf(FILE *f, const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(f, "\e[30m", format, args);
    va_end(args);
}
void rprintk(const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(stdout, "\e[30m", format, args);
    va_end(args);
}

// Red
void rprintrf(FILE *f, const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(f, "\e[31m", format, args);
    va_end(args);
}
void rprintr(const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(stdout, "\e[31m", format, args);
    va_end(args);
}

// Green
void rprintgf(FILE *f, const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(f, "\e[32m", format, args);
    va_end(args);
}
void rprintg(const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(stdout, "\e[32m", format, args);
    va_end(args);
}

// Yellow
void rprintyf(FILE *f, const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(f, "\e[33m", format, args);
    va_end(args);
}
void rprinty(const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(stdout, "\e[33m", format, args);
    va_end(args);
}

// Blue
void rprintbf(FILE *f, const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(f, "\e[34m", format, args);
    va_end(args);
}

void rprintb(const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(stdout, "\e[34m", format, args);
    va_end(args);
}

// Magenta
void rprintmf(FILE *f, const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(f, "\e[35m", format, args);
    va_end(args);
}
void rprintm(const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(stdout, "\e[35m", format, args);
    va_end(args);
}

// Cyan
void rprintcf(FILE *f, const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(f, "\e[36m", format, args);
    va_end(args);
}
void rprintc(const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(stdout, "\e[36m", format, args);
    va_end(args);
}

// White
void rprintwf(FILE *f, const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(f, "\e[37m", format, args);
    va_end(args);
}
void rprintw(const char *format, ...) {
    va_list args;
    va_start(args, format);
    rprintpf(stdout, "\e[37m", format, args);
    va_end(args);
}
#endif