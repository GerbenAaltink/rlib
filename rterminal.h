#ifndef RLIB_TERMINAL_H
#define RLIB_TERMINAL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rtest.h"

char *rfcaptured = NULL;

void rfcapture(FILE *f, char *buff, size_t size) {
    rfcaptured = buff;
    setvbuf(f, rfcaptured, _IOFBF, size);
}
void rfstopcapture(FILE *f) { setvbuf(f, 0, _IOFBF, 0); }

bool _r_disable_stdout_toggle = false;

FILE *_r_original_stdout = NULL;

bool rr_enable_stdout() {
    if (_r_disable_stdout_toggle)
        return false;
    if (!_r_original_stdout) {
        stdout = fopen("/dev/null", "rb");
        return false;
    }
    if (_r_original_stdout && _r_original_stdout != stdout) {
        fclose(stdout);
    }
    stdout = _r_original_stdout;
    return true;
}
bool rr_disable_stdout() {
    if (_r_disable_stdout_toggle) {
        return false;
    }
    if (_r_original_stdout == NULL) {
        _r_original_stdout = stdout;
    }
    if (stdout == _r_original_stdout) {
        stdout = fopen("/dev/null", "rb");
        return true;
    }
    return false;
}
bool rr_toggle_stdout() {
    if (!_r_original_stdout) {
        rr_disable_stdout();
        return true;
    } else if (stdout != _r_original_stdout) {
        rr_enable_stdout();
        return true;
    } else {
        rr_disable_stdout();
        return true;
    }
}

typedef struct rprogressbar_t {
    unsigned long current_value;
    unsigned long min_value;
    unsigned long max_value;
    unsigned int length;
    bool changed;
    double percentage;
    unsigned int width;
    unsigned long draws;
    FILE *fout;
} rprogressbar_t;

rprogressbar_t *rprogressbar_new(long min_value, long max_value,
                                 unsigned int width, FILE *fout) {
    rprogressbar_t *pbar = (rprogressbar_t *)malloc(sizeof(rprogressbar_t));
    pbar->min_value = min_value;
    pbar->max_value = max_value;
    pbar->current_value = min_value;
    pbar->width = width;
    pbar->draws = 0;
    pbar->length = 0;
    pbar->changed = false;
    pbar->fout = fout ? fout : stdout;
    return pbar;
}

void rprogressbar_free(rprogressbar_t *pbar) { free(pbar); }

void rprogressbar_draw(rprogressbar_t *pbar) {
    if (!pbar->changed) {
        return;
    } else {
        pbar->changed = false;
    }
    pbar->draws++;
    char draws_text[22];
    draws_text[0] = 0;
    sprintf(draws_text, "%ld", pbar->draws);
    char *draws_textp = draws_text;
    // bool draws_text_len = strlen(draws_text);
    char bar_begin_char = ' ';
    char bar_progress_char = ' ';
    char bar_empty_char = ' ';
    char bar_end_char = ' ';
    char content[4096] = {0};
    char bar_content[1024];
    char buff[2048] = {0};
    bar_content[0] = '\r';
    bar_content[1] = bar_begin_char;
    unsigned int index = 2;
    for (unsigned long i = 0; i < pbar->length; i++) {
        if (*draws_textp) {
            bar_content[index] = *draws_textp;
            draws_textp++;
        } else {
            bar_content[index] = bar_progress_char;
        }
        index++;
    }
    char infix[] = "\033[0m";
    for (unsigned long i = 0; i < strlen(infix); i++) {
        bar_content[index] = infix[i];
        index++;
    }
    for (unsigned long i = 0; i < pbar->width - pbar->length; i++) {
        bar_content[index] = bar_empty_char;
        index++;
    }
    bar_content[index] = bar_end_char;
    bar_content[index + 1] = '\0';
    sprintf(buff, "\033[43m%s\033[0m \033[33m%.2f%%\033[0m ", bar_content,
            pbar->percentage * 100);
    strcat(content, buff);
    if (pbar->width == pbar->length) {
        strcat(content, "\r");
        for (unsigned long i = 0; i < pbar->width + 10; i++) {
            strcat(content, " ");
        }
        strcat(content, "\r");
    }
    fprintf(pbar->fout, "%s", content);
    fflush(pbar->fout);
}

bool rprogressbar_update(rprogressbar_t *pbar, unsigned long value) {
    if (value == pbar->current_value) {
        return false;
    }
    pbar->current_value = value;
    pbar->percentage = (double)pbar->current_value /
                       (double)(pbar->max_value - pbar->min_value);
    unsigned long new_length = (unsigned long)(pbar->percentage * pbar->width);
    pbar->changed = new_length != pbar->length;
    if (pbar->changed) {
        pbar->length = new_length;
        rprogressbar_draw(pbar);
        return true;
    }
    return false;
}

size_t rreadline(char *data, size_t len, bool strip_ln) {
    __attribute__((unused)) char *unused = fgets(data, len, stdin);
    size_t length = strlen(data);
    if (length && strip_ln)
        data[length - 1] = 0;
    return length;
}

void rlib_test_progressbar() {
    rtest_banner("Progress bar");
    rprogressbar_t *pbar = rprogressbar_new(0, 1000, 10, stderr);
    rprogressbar_draw(pbar);
    // No draws executed, nothing to show
    rassert(pbar->draws == 0);
    rprogressbar_update(pbar, 500);
    rassert(pbar->percentage == 0.5);
    rprogressbar_update(pbar, 500);
    rprogressbar_update(pbar, 501);
    rprogressbar_update(pbar, 502);
    // Should only have drawn one time since value did change, but percentage
    // did not
    rassert(pbar->draws == 1);
    // Changed is false because update function calls draw
    rassert(pbar->changed == false);
    rprogressbar_update(pbar, 777);
    rassert(pbar->percentage == 0.777);
    rprogressbar_update(pbar, 1000);
    rassert(pbar->percentage == 1);
}

#endif