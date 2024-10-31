#ifndef RCOV_H
#define RCOV_H
#include "rtypes.h"
#include "rtemp.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rbench.h"
bool check_lcov() {
    char buffer[1024 * 64];
    FILE *fp;
    fp = popen("lcov --help", "r");
    if (fp == NULL) {
        return false;
    }
    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        return false;
    }
    pclose(fp);
    return strstr(buffer, "lcov: not found") ? false : true;
}

int rcov_main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: [source.c]\n");
        return 1;
    }
    char argstr[4096] = {0};
    for (int i = 2; i < argc; i++) {
        strcat(argstr, argv[i]);
        strcat(argstr, " ");
    }
    if (!check_lcov()) {

        printf("lcov is not installed. Please execute `sudo apt install lcov`.\n");
        return 1;
    }
    char *source_file = argv[1];
    char *commands[] = {"rm -f *.gcda   2>/dev/null",
                        "rm -f *.gcno   2>/dev/null",
                        "rm -f %s.coverage.info   2>/dev/null",
                        "gcc -pg -fprofile-arcs -ftest-coverage -g -o %s_coverage.o %s",
                        "./%s_coverage.o",
                        "lcov --capture --directory . --output-file %s.coverage.info",
                        "genhtml %s.coverage.info --output-directory /tmp/%s.coverage",
                        "rm -f *.gcda   2>/dev/null",
                        "rm -f *.gcno   2>/dev/null",
                        "rm -f %s.coverage.info   2>/dev/null", //"cat gmon.out",

                        "gprof %s_coverage.o gmon.out > output.rcov_analysis",

                        "rm -f gmon.out",
                        "cat output.rcov_analysis",
                        "rm output.rcov_analysis",
                        "rm -f %s_coverage.o",

                        "google-chrome /tmp/%s.coverage/index.html"};
    uint command_count = sizeof(commands) / sizeof(commands[0]);
    RBENCH(1,{
        for (uint i = 0; i < command_count; i++) {
            char *formatted_command = sbuf("");
            sprintf(formatted_command, commands[i], source_file, source_file);
            // printf("%s\n", formatted_command);
            if (formatted_command[0] == '.' && formatted_command[1] == '/') {
                strcat(formatted_command, " ");
                strcat(formatted_command, argstr);
            }

            if (system(formatted_command)) {
                printf("`%s` returned non-zero code.\n", formatted_command);
            }
        });
    }
    return 0;
}
#endif
