#ifndef RBENCH_H
#define RBENCH_H

#include "rprint.h"
#include "rtime.h"
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "rstring.h"
#include "rterminal.h"

#define RBENCH(times, action)                                                  \
    {                                                                          \
        unsigned long utimes = (unsigned long)times;                           \
        nsecs_t start = nsecs();                                               \
        for (unsigned long i = 0; i < utimes; i++) {                           \
            { action; }                                                        \
        }                                                                      \
        nsecs_t end = nsecs();                                                 \
        printf("%s\n", format_time(end - start));                              \
    }

#define RBENCHP(times, action)                                                 \
    {                                                                          \
        printf("\n");                                                          \
        nsecs_t start = nsecs();                                               \
        unsigned int prev_percentage = 0;                                      \
        unsigned long utimes = (unsigned long)times;                           \
        for (unsigned long i = 0; i < utimes; i++) {                           \
            unsigned int percentage =                                          \
                ((long double)i / (long double)times) * 100;                   \
            int percentage_changed = percentage != prev_percentage;            \
            __attribute__((unused)) int first = i == 0;                        \
            __attribute__((unused)) int last = i == utimes - 1;                \
            { action; };                                                       \
            if (percentage_changed) {                                          \
                printf("\r%d%%", percentage);                                  \
                fflush(stdout);                                                \
                                                                               \
                prev_percentage = percentage;                                  \
            }                                                                  \
        }                                                                      \
        nsecs_t end = nsecs();                                                 \
        printf("\r%s\n", format_time(end - start));                            \
    }

struct rbench_t;

typedef struct rbench_function_t {
#ifdef __cplusplus
    void (*call)();
#else
    void(*call);
#endif
    char name[256];
    char group[256];
    void *arg;
    void *data;
    bool first;
    bool last;
    int argc;
    unsigned long times_executed;

    nsecs_t average_execution_time;
    nsecs_t total_execution_time;
} rbench_function_t;

typedef struct rbench_t {
    unsigned int function_count;
    rbench_function_t functions[100];
    rbench_function_t *current;
    rprogressbar_t *progress_bar;
    bool show_progress;
    int winner;
    bool stdout;
    unsigned long times;
    bool silent;
    nsecs_t execution_time;
#ifdef __cplusplus
    void (*add_function)(struct rbench_t *r, const char *name,
                         const char *group, void (*)());
#else
    void (*add_function)(struct rbench_t *r, const char *name,
                         const char *group, void *);
#endif
    void (*rbench_reset)(struct rbench_t *r);
    struct rbench_t *(*execute)(struct rbench_t *r, long times);
    struct rbench_t *(*execute1)(struct rbench_t *r, long times, void *arg1);
    struct rbench_t *(*execute2)(struct rbench_t *r, long times, void *arg1,
                                 void *arg2);
    struct rbench_t *(*execute3)(struct rbench_t *r, long times, void *arg1,
                                 void *arg2, void *arg3);

} rbench_t;

FILE *_rbench_stdout = NULL;
FILE *_rbench_stdnull = NULL;

void rbench_toggle_stdout(rbench_t *r) {
    if (!r->stdout) {
        if (_rbench_stdout == NULL) {
            _rbench_stdout = stdout;
        }
        if (_rbench_stdnull == NULL) {
            _rbench_stdnull = fopen("/dev/null", "wb");
        }
        if (stdout == _rbench_stdout) {
            stdout = _rbench_stdnull;
        } else {
            stdout = _rbench_stdout;
        }
    }
}
void rbench_restore_stdout(rbench_t *r) {
    if (r->stdout)
        return;
    if (_rbench_stdout) {
        stdout = _rbench_stdout;
    }
    if (_rbench_stdnull) {
        fclose(_rbench_stdnull);
        _rbench_stdnull = NULL;
    }
}

rbench_t *rbench_new();

rbench_t *_rbench = NULL;
rbench_function_t *rbf;
rbench_t *rbench() {
    if (_rbench == NULL) {
        _rbench = rbench_new();
    }
    return _rbench;
}

typedef void *(*rbench_call)();
typedef void *(*rbench_call1)(void *);
typedef void *(*rbench_call2)(void *, void *);
typedef void *(*rbench_call3)(void *, void *, void *);

#ifdef __cplusplus
void rbench_add_function(rbench_t *rp, const char *name, const char *group,
                         void (*call)()) {
#else
void rbench_add_function(rbench_t *rp, const char *name, const char *group,
                         void *call) {
#endif
    rbench_function_t *f = &rp->functions[rp->function_count];
    rp->function_count++;
    f->average_execution_time = 0;
    f->total_execution_time = 0;
    f->times_executed = 0;
    f->call = call;
    strcpy(f->name, name);
    strcpy(f->group, group);
}

void rbench_reset_function(rbench_function_t *f) {
    f->average_execution_time = 0;
    f->times_executed = 0;
    f->total_execution_time = 0;
}

void rbench_reset(rbench_t *rp) {
    for (unsigned int i = 0; i < rp->function_count; i++) {
        rbench_reset_function(&rp->functions[i]);
    }
}
int rbench_get_winner_index(rbench_t *r) {
    int winner = 0;
    nsecs_t time = 0;
    for (unsigned int i = 0; i < r->function_count; i++) {
        if (time == 0 || r->functions[i].total_execution_time < time) {
            winner = i;
            time = r->functions[i].total_execution_time;
        }
    }
    return winner;
}
bool rbench_was_last_function(rbench_t *r) {
    for (unsigned int i = 0; i < r->function_count; i++) {
        if (i == r->function_count - 1 && r->current == &r->functions[i])
            return true;
    }
    return false;
}

rbench_function_t *rbench_execute_prepare(rbench_t *r, int findex, long times,
                                          int argc) {
    rbench_toggle_stdout(r);
    if (findex == 0) {
        r->execution_time = 0;
    }
    rbench_function_t *rf = &r->functions[findex];
    rf->argc = argc;
    rbf = rf;
    r->current = rf;
    if (r->show_progress)
        r->progress_bar = rprogressbar_new(0, times, 20, stderr);
    r->times = times;
    // printf("   %s:%s gets executed for %ld times with %d
    // arguments.\n",rf->group, rf->name, times,argc);
    rbench_reset_function(rf);

    return rf;
}
void rbench_execute_finish(rbench_t *r) {
    rbench_toggle_stdout(r);
    if (r->progress_bar) {
        free(r->progress_bar);
        r->progress_bar = NULL;
    }
    r->current->average_execution_time =
        r->current->total_execution_time / r->current->times_executed;
    ;
    // printf("   %s:%s finished executing in
    // %s\n",r->current->group,r->current->name,
    // format_time(r->current->total_execution_time));
    // rbench_show_results_function(r->current);
    if (rbench_was_last_function(r)) {
        rbench_restore_stdout(r);
        unsigned int winner_index = rbench_get_winner_index(r);
        r->winner = winner_index + 1;
        if (!r->silent)
            rprintgf(stderr, "Benchmark results:\n");
        nsecs_t total_time = 0;

        for (unsigned int i = 0; i < r->function_count; i++) {
            rbf = &r->functions[i];
            total_time += rbf->total_execution_time;
            bool is_winner = winner_index == i;
            if (is_winner) {
                if (!r->silent)
                    rprintyf(stderr, " > %s:%s:%s\n",
                             format_time(rbf->total_execution_time), rbf->group,
                             rbf->name);
            } else {
                if (!r->silent)
                    rprintbf(stderr, "   %s:%s:%s\n",
                             format_time(rbf->total_execution_time), rbf->group,
                             rbf->name);
            }
        }
        if (!r->silent)
            rprintgf(stderr, "Total execution time: %s\n",
                     format_time(total_time));
    }
    rbench_restore_stdout(r);
    rbf = NULL;
    r->current = NULL;
}
struct rbench_t *rbench_execute(rbench_t *r, long times) {

    for (unsigned int i = 0; i < r->function_count; i++) {

        rbench_function_t *f = rbench_execute_prepare(r, i, times, 0);
        rbench_call c = (rbench_call)f->call;
        nsecs_t start = nsecs();
        f->first = true;
        c();
        f->first = false;
        f->last = false;
        f->times_executed++;
        for (int j = 1; j < times; j++) {
            c();
            f->times_executed++;
            f->last = f->times_executed == r->times - 1;
            if (r->progress_bar) {
                rprogressbar_update(r->progress_bar, f->times_executed);
            }
        }
        f->total_execution_time = nsecs() - start;
        r->execution_time += f->total_execution_time;
        rbench_execute_finish(r);
    }
    return r;
}

struct rbench_t *rbench_execute1(rbench_t *r, long times, void *arg1) {

    for (unsigned int i = 0; i < r->function_count; i++) {
        rbench_function_t *f = rbench_execute_prepare(r, i, times, 1);
        rbench_call1 c = (rbench_call1)f->call;
        nsecs_t start = nsecs();
        f->first = true;
        c(arg1);
        f->first = false;
        f->last = false;
        f->times_executed++;
        for (int j = 1; j < times; j++) {
            c(arg1);
            f->times_executed++;
            f->last = f->times_executed == r->times - 1;
            if (r->progress_bar) {
                rprogressbar_update(r->progress_bar, f->times_executed);
            }
        }
        f->total_execution_time = nsecs() - start;
        r->execution_time += f->total_execution_time;
        rbench_execute_finish(r);
    }
    return r;
}

struct rbench_t *rbench_execute2(rbench_t *r, long times, void *arg1,
                                 void *arg2) {

    for (unsigned int i = 0; i < r->function_count; i++) {
        rbench_function_t *f = rbench_execute_prepare(r, i, times, 2);
        rbench_call2 c = (rbench_call2)f->call;
        nsecs_t start = nsecs();
        f->first = true;
        c(arg1, arg2);
        f->first = false;
        f->last = false;
        f->times_executed++;
        for (int j = 1; j < times; j++) {
            c(arg1, arg2);
            f->times_executed++;
            f->last = f->times_executed == r->times - 1;
            if (r->progress_bar) {
                rprogressbar_update(r->progress_bar, f->times_executed);
            }
        }
        f->total_execution_time = nsecs() - start;
        r->execution_time += f->total_execution_time;
        rbench_execute_finish(r);
    }
    return r;
}

struct rbench_t *rbench_execute3(rbench_t *r, long times, void *arg1,
                                 void *arg2, void *arg3) {

    for (unsigned int i = 0; i < r->function_count; i++) {
        rbench_function_t *f = rbench_execute_prepare(r, i, times, 3);

        rbench_call3 c = (rbench_call3)f->call;
        nsecs_t start = nsecs();
        f->first = true;
        c(arg1, arg2, arg3);
        f->first = false;
        f->last = false;
        f->times_executed++;
        for (int j = 1; j < times; j++) {
            c(arg1, arg2, arg3);
            f->times_executed++;
            f->last = f->times_executed == r->times - 1;
            if (r->progress_bar) {
                rprogressbar_update(r->progress_bar, f->times_executed);
            }
        }
        f->total_execution_time = nsecs() - start;
        rbench_execute_finish(r);
    }
    return r;
}

rbench_t *rbench_new() {

    rbench_t *r = (rbench_t *)malloc(sizeof(rbench_t));
    memset(r, 0, sizeof(rbench_t));
    r->add_function = rbench_add_function;
    r->rbench_reset = rbench_reset;
    r->execute1 = rbench_execute1;
    r->execute2 = rbench_execute2;
    r->execute3 = rbench_execute3;
    r->execute = rbench_execute;
    r->stdout = true;
    r->silent = false;
    r->winner = 0;
    r->show_progress = true;
    return r;
}
void rbench_free(rbench_t *r) { free(r); }

#endif