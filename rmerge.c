#include "rlexer.h"
#include "rmalloc.h"
#include "rprint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool has_error = false;

char *readline(FILE *f) {
    static char data[4096];
    data[0] = 0;
    int index = 0;
    char c;
    while ((c = fgetc(f)) != EOF) {
        if (c != '\0') {
            data[index] = c;
            index++;
            if (c == '\n')
                break;
        }
    }
    data[index] = 0;
    if (data[0] == 0)
        return false;
    return data;
}
void writestring(FILE *f, char *line) {
    char c;
    while ((c = *line) != '\0') {
        fputc(c, f);
        line++;
    }
}
char files_history[8096];
char files_duplicate[8096];
bool is_merging = false;

void merge_file(char *source, FILE *d) {
    if (is_merging == false) {
        is_merging = true;
        files_history[0] = 0;
        files_duplicate[0] = 0;
    }
    if (strstr(files_history, source)) {
        if (strstr(files_duplicate, source)) {
            rprintmf(stderr,
                     "\\l Already included: %s. Already on duplicate list.\n",
                     source);
        } else {
            rprintcf(stderr,
                     "\\l Already included: %s. Adding to duplicate list.\n",
                     source);
            strcat(files_duplicate, source);
            strcat(files_duplicate, "\n");
        }
        return;
    } else {
        rprintgf(stderr, "\\l Merging: %s.\n", source);
        strcat(files_history, source);
        strcat(files_history, "\n");
    }
    FILE *fd = fopen(source, "rb");
    if (!fd) {
        rprintrf(stderr, "\\l File does not exist: %s\n", source);
        has_error = true;
        return;
    }

    char *line;
    char include_path[4096];
    while ((line = readline(fd))) {

        include_path[0] = 0;
        if (!*line)
            break;
        if (!strncmp(line, "#include ", 9)) {
            int index = 0;
            while (line[index] != '"' && line[index] != 0) {
                index++;
            }
            if (line[index] == '"') {
                int pindex = 0;
                index++;
                while (line[index] != '"') {
                    include_path[pindex] = line[index];
                    pindex++;
                    index++;
                }
                if (line[index] != '"') {
                    include_path[0] = 0;
                } else {
                    include_path[pindex] = '\0';
                }
            }
        }
        if (include_path[0]) {
            printf("// Found (local) include: %s\n", include_path);
            merge_file(include_path, d);
            include_path[0] = 0;
        } else {
            writestring(d, line);
        }
    }
    fclose(fd);
    writestring(d, "\n");
}

int main(int argc, char *argv[]) {
    char *file_input = NULL;
    if (argc != 2) {
        printf("Usage: <input-file>\n");
    } else {
        file_input = argv[1];
        // file_output = argv[2];
    }
    FILE *f = tmpfile();
    printf("// RETOOR - %s\n", __DATE__);
    merge_file(file_input, f);
    rewind(f);
    char *data;
    int line_number = 0;
    while ((data = readline(f))) {
        if (line_number) {
            printf("/*%.5d*/    ", line_number);
            line_number++;
        }
        printf("%s", data);
    }
    printf("\n");
    if (has_error) {
        rprintrf(stderr,
                 "\\l Warning: there are errors while merging this file.\n");
    } else {
        rprintgf(stderr, "\\l Merge succesful without error(s).\n");
    }
}