#ifndef RLEXER_H
#define RLEXER_H
#include "rmalloc.h"
#include "rstring.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define RTOKEN_VALUE_SIZE 1024

typedef enum rtoken_type_t {
    RT_UNKNOWN = 0,
    RT_SYMBOL,
    RT_NUMBER,
    RT_STRING,
    RT_PUNCT,
    RT_OPERATOR,
    RT_EOF = 10,
    RT_BRACE_OPEN,
    RT_CURLY_BRACE_OPEN,
    RT_BRACKET_OPEN,
    RT_BRACE_CLOSE,
    RT_CURLY_BRACE_CLOSE,
    RT_BRACKET_CLOSE
} rtoken_type_t;

typedef struct rtoken_t {
    rtoken_type_t type;
    char value[RTOKEN_VALUE_SIZE];
    unsigned int line;
    unsigned int col;
} rtoken_t;

static char *_content;
static unsigned int _content_ptr;
static unsigned int _content_line;
static unsigned int _content_col;

static int isgroupingchar(char c) {
    return (c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == ']' || c == '"' || c == '\'');
}

static int isoperator(char c) {
    return (c == '+' || c == '-' || c == '/' || c == '*' || c == '=' || c == '>' || c == '<' || c == '|' || c == '&');
}

static rtoken_t rtoken_new() {
    rtoken_t token;
    memset(&token, 0, sizeof(token));
    token.type = RT_UNKNOWN;
    return token;
}

rtoken_t rlex_number() {
    rtoken_t token = rtoken_new();
    token.col = _content_col;
    token.line = _content_line;
    bool first_char = true;
    int dot_count = 0;
    char c;
    while (isdigit(c = _content[_content_ptr]) || (first_char && _content[_content_ptr] == '-') ||
           (dot_count == 0 && _content[_content_ptr] == '.')) {
        if (c == '.')
            dot_count++;
        first_char = false;
        char chars[] = {c, 0};
        strcat(token.value, chars);
        _content_ptr++;
        _content_col++;
    }
    token.type = RT_NUMBER;
    return token;
}

static rtoken_t rlex_symbol() {
    rtoken_t token = rtoken_new();

    token.col = _content_col;
    token.line = _content_line;
    char c;
    while (isalpha(_content[_content_ptr]) || _content[_content_ptr] == '_') {
        c = _content[_content_ptr];
        char chars[] = {c, 0};
        strcat(token.value, chars);
        _content_ptr++;
        _content_col++;
    }
    token.type = RT_SYMBOL;
    return token;
}

static rtoken_t rlex_operator() {

    rtoken_t token = rtoken_new();

    token.col = _content_col;
    token.line = _content_line;
    char c;
    bool is_first = true;
    while (isoperator(_content[_content_ptr])) {
        if (!is_first) {
            if (_content[_content_ptr - 1] == '=' && _content[_content_ptr] == '-') {
                break;
            }
        }
        c = _content[_content_ptr];
        char chars[] = {c, 0};
        strcat(token.value, chars);
        _content_ptr++;
        _content_col++;
        is_first = false;
    }
    token.type = RT_OPERATOR;
    return token;
}

static rtoken_t rlex_punct() {

    rtoken_t token = rtoken_new();

    token.col = _content_col;
    token.line = _content_line;
    char c;
    bool is_first = true;
    while (ispunct(_content[_content_ptr])) {
        if (!is_first) {
            if (_content[_content_ptr] == '"') {
                break;
            }
            if (_content[_content_ptr] == '\'') {
                break;
            }
            if (isgroupingchar(_content[_content_ptr])) {
                break;
            }
            if (isoperator(_content[_content_ptr])) {
                break;
            }
        }
        c = _content[_content_ptr];
        char chars[] = {c, 0};
        strcat(token.value, chars);
        _content_ptr++;
        _content_col++;
        is_first = false;
    }
    token.type = RT_PUNCT;
    return token;
}

static rtoken_t rlex_string() {
    rtoken_t token = rtoken_new();
    char c;
    token.col = _content_col;
    token.line = _content_line;
    char str_chr = _content[_content_ptr];
    _content_ptr++;
    while (_content[_content_ptr] != str_chr) {
        c = _content[_content_ptr];
        if (c == '\\') {
            _content_ptr++;
            c = _content[_content_ptr];
            if (c == 'n') {
                c = '\n';
            } else if (c == 'r') {
                c = '\r';
            } else if (c == 't') {
                c = '\t';
            } else if (c == str_chr) {
                c = str_chr;
            }

            _content_col++;
        }
        char chars[] = {c, 0};
        strcat(token.value, chars);
        _content_ptr++;
        _content_col++;
    }
    _content_ptr++;
    token.type = RT_STRING;
    return token;
}

void rlex(char *content) {
    _content = content;
    _content_ptr = 0;
    _content_col = 1;
    _content_line = 1;
}

static void rlex_repeat_str(char *dest, char *src, unsigned int times) {
    for (size_t i = 0; i < times; i++) {
        strcat(dest, src);
    }
}

rtoken_t rtoken_create(rtoken_type_t type, char *value) {
    rtoken_t token = rtoken_new();
    token.type = type;
    token.col = _content_col;
    token.line = _content_line;
    strcpy(token.value, value);
    return token;
}

rtoken_t rlex_next() {
    while (true) {

        _content_col++;

        if (_content[_content_ptr] == 0) {
            return rtoken_create(RT_EOF, "eof");
        } else if (_content[_content_ptr] == '\n') {
            _content_line++;
            _content_col = 1;
            _content_ptr++;
        } else if (isspace(_content[_content_ptr])) {
            _content_ptr++;
        } else if (isdigit(_content[_content_ptr]) || (_content[_content_ptr] == '-' && isdigit(_content[_content_ptr + 1]))) {
            return rlex_number();
        } else if (isalpha(_content[_content_ptr]) || _content[_content_ptr] == '_') {
            return rlex_symbol();
        } else if (_content[_content_ptr] == '"' || _content[_content_ptr] == '\'') {
            return rlex_string();
        } else if (isoperator(_content[_content_ptr])) {
            return rlex_operator();
        } else if (ispunct(_content[_content_ptr])) {
            if (_content[_content_ptr] == '{') {

                _content_ptr++;
                return rtoken_create(RT_CURLY_BRACE_OPEN, "{");
            }
            if (_content[_content_ptr] == '}') {

                _content_ptr++;
                return rtoken_create(RT_CURLY_BRACE_CLOSE, "}");
            }
            if (_content[_content_ptr] == '(') {

                _content_ptr++;
                return rtoken_create(RT_BRACE_OPEN, "(");
            }
            if (_content[_content_ptr] == ')') {

                _content_ptr++;
                return rtoken_create(RT_BRACE_CLOSE, ")");
            }
            if (_content[_content_ptr] == '[') {

                _content_ptr++;
                return rtoken_create(RT_BRACKET_OPEN, "[");
            }
            if (_content[_content_ptr] == ']') {

                _content_ptr++;
                return rtoken_create(RT_BRACKET_CLOSE, "]");
            }
            return rlex_punct();
        }
    }
}

char *rlex_format(char *content) {
    rlex(content);
    char *result = (char *)malloc(strlen(content) + 4096);
    result[0] = 0;
    unsigned int tab_index = 0;
    char *tab_chars = "    ";
    unsigned int col = 0;
    rtoken_t token_previous;
    token_previous.value[0] = 0;
    token_previous.type = RT_UNKNOWN;
    while (true) {
        rtoken_t token = rlex_next();
        if (token.type == RT_EOF) {
            break;
        }

        // col = strlen(token.value);

        if (col == 0) {
            rlex_repeat_str(result, tab_chars, tab_index);
            // col = strlen(token.value);// strlen(tab_chars) * tab_index;
        }

        if (token.type == RT_STRING) {
            strcat(result, "\"");

            char string_with_slashes[strlen(token.value) * 2 + 1];
            rstraddslashes(token.value, string_with_slashes);
            strcat(result, string_with_slashes);

            strcat(result, "\"");
            // col+= strlen(token.value) + 2;
            // printf("\n");
            // printf("<<<%s>>>\n",token.value);

            memcpy(&token_previous, &token, sizeof(token));
            continue;
        }
        if (!(strcmp(token.value, "{"))) {
            if (col != 0) {
                strcat(result, "\n");
                rlex_repeat_str(result, "    ", tab_index);
            }
            strcat(result, token.value);

            tab_index++;

            strcat(result, "\n");

            col = 0;

            memcpy(&token_previous, &token, sizeof(token));
            continue;
        } else if (!(strcmp(token.value, "}"))) {
            unsigned int tab_indexed = 0;
            if (tab_index)
                tab_index--;
            strcat(result, "\n");

            rlex_repeat_str(result, tab_chars, tab_index);
            tab_indexed++;

            strcat(result, token.value);
            strcat(result, "\n");
            col = 0;

            memcpy(&token_previous, &token, sizeof(token));
            continue;
        }
        if ((token_previous.type == RT_SYMBOL && token.type == RT_NUMBER) ||
            (token_previous.type == RT_NUMBER && token.type == RT_SYMBOL) || (token_previous.type == RT_PUNCT && token.type == RT_SYMBOL) ||
            (token_previous.type == RT_BRACE_CLOSE && token.type == RT_SYMBOL) ||
            (token_previous.type == RT_SYMBOL && token.type == RT_SYMBOL)) {
            if (token_previous.value[0] != ',' && token_previous.value[0] != '.') {
                if (token.type != RT_OPERATOR && token.value[0] != '.') {
                    strcat(result, "\n");
                    rlex_repeat_str(result, tab_chars, tab_index);
                }
            }
        }

        if (token.type == RT_OPERATOR) {
            strcat(result, " ");
        }
        if (token.type == RT_STRING) {
            strcat(result, "\"");
        }
        strcat(result, token.value);
        if (token.type == RT_STRING) {
            strcat(result, "\"");
        }

        if (token.type == RT_OPERATOR) {
            strcat(result, " ");
        }
        if (!strcmp(token.value, ",")) {
            strcat(result, " ");
        }
        col += strlen(token.value);
        memcpy(&token_previous, &token, sizeof(token));
    }
    return result;
}
#endif
