#include "rlexer.c";

typedef enum rip_ast_type_t {
    RIP_NONE = 0,
    RIP_BLOCK,
    RIP_CALL,
    RIP_LITERAL
} rip_ast_type_t;

typedef struct rip_ast_t {
    struct rip_ast_t *children;
    struct rip_ast_t *next;
    struct rip_ast_t *previous;
    rip_ast_type_t type;
} rip_ast_t;

rip_ast_t *rip_ast_new() {
    rip_ast_t *ast = (rip_ast_t *)malloc(sizeof(rip_ast_t));
    ast->children = NULL;
    ast->next = NULL;
    ast->previous = NULL;
    ast->type = RIP_NONE;
    return ast;
}

rip_ast_t *rip_parse() {
    rtoken_t token = rlex_next();
    if (token.type == RT_CURLY_BRACE_OPEN) {
        rip_ast_t *ast = rip_ast_new();
        while ()
            rip_ast_t *statement = rip_parse();
    }
}

int main() {

    char *script = "{print(\"test\")}";
    rlex(script);
    while (true) {
        rtoken_t token = rlex_next();
        if (token.type = RT_CURLY_BRACE_OPEN) {
            rclos
        }
    }
}