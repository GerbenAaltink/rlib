#include "rlexer.h"
#include "rio.h"
#include "rtest.h"

void test_lexer() {
    rtest_banner("Lexer");
    rlex("123"
         "-123 "
         "123.22.123.33"
         "-123.33"
         "abc "
         "_abc "
         "abc_ "
         "a_a"
         "\"string content 123\""
         "\"!@#$%^& *()-+\""
         "\"ab\\tc\\n\\\"\\r\""
         "--++-+/*<>!@#$%^&*(){}?[]"
         "\n"
         "()");
    rtest_banner("Number");
    rtoken_t token = rlex_next();
    rtest_assert(token.type == RT_NUMBER);
    rtest_assert(!strcmp(token.value, "123"));
    rtest_banner("Negative number");
    token = rlex_next();
    rtest_assert(token.type == RT_NUMBER);
    rtest_assert(!strcmp(token.value, "-123"));
    rtest_banner("Decimal Number");
    token = rlex_next();
    rtest_assert(token.type == RT_NUMBER);
    rtest_assert(!strcmp(token.value, "123.22"));
    token = rlex_next();
    rtest_assert(token.type == RT_PUNCT);
    rtest_assert(!strcmp(token.value, "."));
    token = rlex_next();
    rtest_assert(token.type == RT_NUMBER);
    rtest_assert(!strcmp(token.value, "123.33"));
    rtest_banner("Decimal Negative number");
    token = rlex_next();
    rtest_assert(token.type == RT_NUMBER);
    rtest_assert(!strcmp(token.value, "-123.33"));
    rtest_banner("Symbol");
    token = rlex_next();
    rtest_assert(token.type == RT_SYMBOL);
    rtest_assert(!strcmp(token.value, "abc"));
    token = rlex_next();
    rtest_assert(token.type == RT_SYMBOL);
    rtest_assert(!strcmp(token.value, "_abc"));
    token = rlex_next();

    rtest_assert(token.type == RT_SYMBOL);
    rtest_assert(!strcmp(token.value, "abc_"));

    token = rlex_next();

    rtest_assert(token.type == RT_SYMBOL);
    rtest_assert(!strcmp(token.value, "a_a"));
    rtest_banner("String");
    token = rlex_next();
    rtest_assert(token.type == RT_STRING);
    rtest_assert(!strcmp(token.value, "string content 123"));
    token = rlex_next();
    rtest_assert(token.type == RT_STRING);
    rtest_assert(!strcmp(token.value, "!@#$\%^& *()-+"));
    token = rlex_next();
    rtest_assert(token.type == RT_STRING);
    rtest_assert(!strcmp(token.value, "ab\tc\n\"\r"));

    rtest_banner("Operator");
    token = rlex_next();

    rtest_assert(token.type == RT_OPERATOR);
    rtest_assert(!strcmp(token.value, "--++-+/*<>"));

    rtest_banner("Punct") token = rlex_next();
    rtest_assert(token.type == RT_PUNCT);
    rtest_assert(!strcmp(token.value, "!@#$%^"));
    token = rlex_next();
    rtest_assert(token.type == RT_OPERATOR);
    rtest_assert(!strcmp(token.value, "&*"));

    rtest_banner("Grouping");
    token = rlex_next();
    rtest_assert(token.type == RT_BRACE_OPEN);
    rassert(!strcmp(token.value, "("));

    token = rlex_next();
    rtest_assert(token.type == RT_BRACE_CLOSE);
    rassert(!strcmp(token.value, ")"));

    token = rlex_next();
    rtest_assert(token.type == RT_CURLY_BRACE_OPEN);
    rassert(!strcmp(token.value, "{"));

    token = rlex_next();
    rtest_assert(token.type == RT_CURLY_BRACE_CLOSE);
    rassert(!strcmp(token.value, "}"));

    token = rlex_next();
    rtest_assert(token.type == RT_PUNCT);
    rassert(!strcmp(token.value, "?"));

    token = rlex_next();
    rtest_assert(token.type == RT_BRACKET_OPEN);
    rassert(!strcmp(token.value, "["));

    token = rlex_next();
    rtest_assert(token.type == RT_BRACKET_CLOSE);
    rassert(!strcmp(token.value, "]"));

    rtest_banner("Line number");
    token = rlex_next();
    rtest_assert(token.type == RT_BRACE_OPEN);
    rassert(!strcmp(token.value, "("));
    rassert(token.line == 2);
    token = rlex_next();
    rtest_assert(token.type == RT_BRACE_CLOSE);
    rassert(!strcmp(token.value, ")"));
    rassert(token.line == 2);

    rtest_banner("EOF");
    token = rlex_next();
    rtest_assert(token.type == RT_EOF);
    rtest_assert(!strcmp(token.value, "eof"));
    rtest_assert(token.line == 2);
}

void test_formatter() {
    rtest_banner("Formatter");
    char *formatted = rlex_format("{123{345{678}}}");
    char *expected_curly_braces = "{\n"
                                  "    123\n"
                                  "    {\n"
                                  "        345\n"
                                  "        {\n"
                                  "            678\n"
                                  "        }\n        \n"
                                  "    }\n    \n"
                                  "}\n";
    rtest_assert(!strcmp(formatted, expected_curly_braces));
    free(formatted);
    formatted = rlex_format("\"123\",66,true,(1,2,3)");
    char *expected_comma = "\"123\", 66, true, (1, 2, 3)";

    rtest_assert(!strcmp(formatted, expected_comma));
    free(formatted);

    formatted = rlex_format("lala lolo");
    char *expected_new_lines1 = "lala\nlolo";
    rtest_assert(!strcmp(formatted, expected_new_lines1));
    free(formatted);

    formatted = rlex_format("lala=lolo");
    char *expected_new_lines2 = "lala = lolo";
    rtest_assert(!strcmp(formatted, expected_new_lines2));
    free(formatted);

    formatted = rlex_format("lala+lolo=(1,2,3)");
    char *expected_new_lines3 = "lala + lolo = (1, 2, 3)";
    rtest_assert(!strcmp(formatted, expected_new_lines3));
    free(formatted);

    formatted = rlex_format("lala+lolo=(1,2,3) little.test=(4,5,6)");
    char *expected_new_lines4 =
        "lala + lolo = (1, 2, 3)\nlittle.test = (4, 5, 6)";
    rtest_assert(!strcmp(formatted, expected_new_lines4));
    free(formatted);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        test_formatter();
        test_lexer();
        return rtest_end("");
    } else {
        if (!rfile_exists(argv[1])) {
            rassert(false && "File does not exist.");
        }
        unsigned int length = rfile_size(argv[1]);
        char content[length + 1];

        length = rfile_readb(argv[1], content, length);

        content[length] = 0;
        char *formatted = rlex_format(content);
        printf("%s", formatted);
    }
}