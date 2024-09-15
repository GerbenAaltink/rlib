#include "rterm.h"
#include <stdlib.h>
#include <stdbool.h>
#include "rstring.h"
#include "rrex4.h"
#include <limits.h>
#include "rautocomplete.h"

typedef struct ricli_line_t {
    unsigned int index;
    char type[20];
    size_t length;
    char *content;
} ricli_line_t;

ricli_line_t *ricli_line_new() {
    ricli_line_t *line = (ricli_line_t *)malloc(sizeof(ricli_line_t));
    line->index = 0;
    memset(line->type, 0, sizeof(line->type));
    line->length = 0;
    line->content = NULL;
    return line;
}

char *rscli_line_to_json(ricli_line_t *line) {

    char *json =
        (char *)malloc(sizeof(line->type) + strlen(line->content) * 2 + 10);
    json[0] = 0;
    strcpy(json, "{\"type\":\"");
    strcat(json, line->type);
    strcat(json, "\",\"content\":\"");
    char content_safe[strlen(line->content) * 2];
    content_safe[0] = 0;
    rstraddslashes(line->content, content_safe);
    strcat(json, content_safe);
    strcat(json, "\"}");
    return json;
}
typedef struct ricli_t {
    ricli_line_t **lines;
    int line_count;
    bool line_numbers;
    char input[1024 * 5];
    unsigned int history_index;
    unsigned int x;
    bool auto_save;
    rautocomplete_t *autocomplete;
    char history_file[FILENAME_MAX];
    bool reset;
    void (*before_add_line)(struct ricli_t *r);
    void (*after_add_line)(struct ricli_t *r);
    void (*keypress)(struct ricli_t *);
    void (*before_draw)(struct ricli_t *);
    rterm_t *term;
} ricli_t;

void ricli_keypress(rterm_t *rt);
void ricli_before_draw(rterm_t *rt);
void ricli_save(ricli_t *cli, char *path);
void ricli_autocomplete_execute(ricli_t *cli);
void ricli_add_autocomplete(ricli_t *cli, char *str) {
    if (rautocomplete_contains(cli->autocomplete, str))
        return;
    rautocomplete_add(cli->autocomplete, str);
}

ricli_line_t *ricli_get_last_line(ricli_t *r) {
    if (!r->line_count) {
        return NULL;
    }
    return r->lines[r->line_count - 1];
}

void ricli_after_draw(rterm_t *rt) {
    ricli_t *r = (ricli_t *)rt->session;
    ricli_autocomplete_execute(r);
}

ricli_t *ricli_terminal_new() {
    ricli_t *terminal = (ricli_t *)malloc(sizeof(ricli_t));
    terminal->lines = NULL;
    terminal->line_count = 0;
    terminal->line_numbers = false;
    terminal->reset = true;
    terminal->history_index = 0;
    terminal->before_add_line = NULL;
    terminal->term = NULL;
    terminal->history_file[0] = 0;
    terminal->autocomplete = rautocomplete_new();
    terminal->auto_save = true;
    terminal->x = 0;
    memset(terminal->input, 0, sizeof(terminal->input));
    terminal->term = (rterm_t *)malloc(sizeof(rterm_t));

    rterm_init(terminal->term);
    terminal->line_numbers = true;

    terminal->term->after_key_press = ricli_keypress;
    terminal->term->before_draw = ricli_before_draw;
    terminal->term->after_draw = ricli_after_draw;
    terminal->term->session = (void *)terminal;
    return terminal;
}
void ricli_set_input(ricli_t *cli, const char *content);
void ricli_autocomplete_execute(ricli_t *r) {

    char *result = rautocomplete_find(
        r->autocomplete,
        r->input); // rautocomplete_find(r->autocomplete, r->input);
    unsigned int original_x = r->term->cursor.x;
    unsigned int original_y = r->term->cursor.y;
    if (result) {
        original_x = r->x;
        // ricli_set_input(r,result);
        // r->term->status_text = result;
        // ricli_set_input(r,result);
        // r->x = original_x;
        cursor_set(r->term, 0, r->term->size.ws_row - 1);
        printf("%s", result);
        //  r->term->status_text = result;
        cursor_set(r->term, original_x, original_y);
    }
}
void ricli_add_line(ricli_t *r, char *type, char *content) {

    ricli_line_t *line = ricli_line_new();
    strcpy(line->type, type ? type : "");
    line->content = (char *)malloc(strlen(content ? content : "") + 1);
    strcpy(line->content, content ? content : "");
    line->length = strlen(line->content);
    if (line->length && line->content[line->length - 1] == '\n') {
        line->content[line->length - 1] = 0;
        line->length--;
    }
    if (line->length)
        ricli_add_autocomplete(r, line->content);
    strcpy(line->type, type ? type : "");
    line->index = r->line_count;
    r->lines = realloc(r->lines, sizeof(ricli_line_t *) * (r->line_count + 1));
    r->lines[r->line_count] = line;
    r->line_count++;
    r->history_index = r->line_count;
    r->x = 0;

    if (r->history_file[0] && r->auto_save)
        ricli_save(r, r->history_file);
}

ricli_t *rt_get_ricli(rterm_t *rt) { return (ricli_t *)rt->session; }

void ricli_reset(rterm_t *rt) {
    ricli_t *cli = rt_get_ricli(rt);
    cli->reset = false;
    cursor_set(rt, 0, rt->size.ws_row - 1);
}

void ricli_before_draw(rterm_t *rt) {
    ricli_t *cli = rt_get_ricli(rt);
    int offset = 0;
    if (cli->line_count > rt->size.ws_row - 1) {
        offset = cli->line_count - rt->size.ws_row;
    }
    for (int i = offset; i < cli->line_count; i++) {
        printf("%.5d %s\n", i + 1, cli->lines[i]->content);
    }
    rt->status_text = cli->input;
    if (cli->reset) {
        ricli_reset(rt);
    }
}

void ricli_clear_input(ricli_t *cli) {
    char line[cli->term->size.ws_col + 1];
    memset(line, ' ', sizeof(line));
    line[sizeof(line) - 1] = 0;
    cursor_set(cli->term, 0, cli->term->cursor.y);
}
void ricli_set_input(ricli_t *cli, const char *content) {
    if (cli->input != content) {
        memset(cli->input, 0, sizeof(cli->input));
        strcpy(cli->input, content);
    }
    strcpy(cli->term->status_text, cli->input);
    ricli_clear_input(cli);
    rterm_print_status_bar(cli->term, 'c', cli->input);
    cursor_set(cli->term, cli->x, cli->term->size.ws_row);
}
void ricli_put_input(ricli_t *cli, char c) {
    bool was_zero = cli->input[cli->x] == 0;
    if (was_zero) {

        cli->input[cli->x] = c;
        cli->input[cli->x + 1] = 0;
    } else {
        char line_first[strlen(cli->input) + 5];

        memset(line_first, 0, sizeof(line_first));
        line_first[0] = 0;
        strncpy(line_first, cli->input, cli->x);

        char line_end[strlen(cli->input) + 2];
        memset(line_end, 0, sizeof(line_end));

        char *input_ptr = cli->input;
        strcpy(line_end, input_ptr + cli->x);
        char new_char[] = {c, 0};
        strcat(line_first, new_char);
        strcat(line_first, line_end);
        memset(cli->input, 0, sizeof(cli->input));
        strcpy(cli->input, line_first);
    }
    cli->history_index = cli->line_count;
    rterm_print_status_bar(cli->term, 'c', cli->input);

    if (cli->x >= strlen(cli->input))
        cli->x = strlen(cli->input) - 1;
    cli->x++;
    cursor_set(cli->term, cli->x, cli->term->cursor.y);
}

void ricli_load(ricli_t *cli, char *path) {
    strcpy(cli->history_file, path);
    size_t size = rfile_size(path);
    if (size == 0) {
        return;
    }

    char *data = malloc(size + 1);
    memset(data, 0, size + 1);
    rfile_readb(path, data, size);
    r4_t *r = r4(data, "\"type\":\"(.*)\",\"content\":\"(.*)\"");
    while (r->match_count == 2) {
        char stripped_slashes[strlen(r->matches[1]) + 1];
        memset(stripped_slashes, 0, sizeof(stripped_slashes));
        rstrstripslashes(r->matches[1], stripped_slashes);
        ricli_add_line(cli, r->matches[0], stripped_slashes);
        r4_next(r, NULL);
    }
    r4_free(r);
    free(data);
}
void ricli_save(ricli_t *cli, char *path) {
    FILE *f = fopen(path, "w+");
    for (int i = 0; i < cli->line_count; i++) {
        if (!cli->lines[i]->length)
            continue;
        char *json_line = rscli_line_to_json(cli->lines[i]);
        if (i != cli->line_count - 1) {
            strcat(json_line, ",");
        }
        fwrite(json_line, 1, strlen(json_line), f);
        free(json_line);
    }
    fclose(f);
}

ricli_delete_input(ricli_t *cli, unsigned int index) {
    if (cli->input[index + 1] == 0) {
        cli->input[index] = 0;
    } else {
        char new_line[strlen(cli->input) + 5];
        memset(new_line, 0, sizeof(new_line));
        strncpy(new_line, cli->input, index);
        char *input_ptr = cli->input;
        strcat(new_line, input_ptr + index + 1);
        strcpy(cli->input, new_line);
    }
    cursor_set(cli->term, cli->x, cli->term->cursor.y);
}

void ricli_keypress(rterm_t *rt) {
    ricli_t *cli = rt_get_ricli(rt);
    if (rt->key.c == 10) {
        if (cli->input[rt->cursor.x] == 0) {
            cli->input[rt->cursor.x] = '\n';
            cli->input[rt->cursor.x + 1] = 0;
        }
        if (cli->before_add_line) {
            cli->before_add_line(cli);
        }
        ricli_add_line(cli, "user", cli->input);
        cursor_set(rt, 0, rt->cursor.y);
        memset(cli->input, 0, sizeof(cli->input));
        cli->history_index = cli->line_count;
        if (cli->after_add_line) {
            cli->after_add_line(cli);
        }
    } else if (rt->key.escape && rt->key.c == 'A') {
        if (cli->history_index != 0)
            cli->history_index--;

        strcpy(cli->input, cli->lines[cli->history_index]->content);
        cli->x = strlen(cli->input);
        ricli_set_input(cli, cli->lines[cli->history_index]->content);

    } else if (rt->key.c == 127) {

        if (cli->x > 0) {
            cli->x--;
            // cli->input[cli->x] = 0;
            ricli_delete_input(cli, cli->x);
        }
    } else if (rt->key.escape && rt->key.c == 'B') {
        if (cli->history_index < cli->line_count - 1) {
            cli->history_index++;

            strcpy(cli->input, cli->lines[cli->history_index]->content);
            cli->x = strlen(cli->input);
            ricli_set_input(cli, cli->lines[cli->history_index]->content);

        } else {
            cli->x = 0;
            ricli_set_input(cli, "");
            cli->history_index = cli->line_count;
        }

    } else if (rt->key.escape && rt->key.c == 'D') {
        cli->x = rt->cursor.x;
        cursor_set(rt, cli->x, rt->cursor.y);
    } else if (rt->key.escape && rt->key.c == 'C') {
        cli->x = rt->cursor.x;
        if (cli->x > strlen(cli->input))
            cli->x = strlen(cli->input);
        cursor_set(rt, cli->x, rt->cursor.y);
    } else {
        if (rt->cursor.x > strlen(cli->input)) {
            rt->cursor.x = strlen(cli->input);
            cli->x = strlen(cli->input);
        }
        ricli_put_input(cli, rt->key.c);
        ricli_autocomplete_execute(cli);
    }
    // rterm_print_status_bar(rt, 0, 0);
}

void ricli_loop(ricli_t *r) { rterm_loop(r->term); }

int main() {

    ricli_t *cli = ricli_terminal_new();
    ricli_load(cli, "/tmp/.ricli.json");
    ricli_loop(cli);
    return 0;
}