#include <assert.h>
#include <string.h>

#include "tengwar/lexer.h"

static void lexer_test_basic_command(void) {
    Token tokens[16];

    size_t token_count = lexer_tokenize("echo hello", tokens, 16);

    assert(tokens[0].type == TK_WORD);
    assert(tokens[0].value != NULL);
    assert(strcmp(tokens[0].value, "echo") == 0);
    assert(tokens[1].type == TK_WORD);
    assert(tokens[1].value != NULL);
    assert(strcmp(tokens[1].value, "hello") == 0);
    assert(tokens[2].type == TK_EOF);
    assert(tokens[2].value == NULL);

    lexer_free_tokens(tokens, token_count);
}

static void lexer_test_pipe(void) {
    Token tokens[16];

    size_t token_count = lexer_tokenize("echo hello | ls", tokens, 16);

    assert(tokens[0].type == TK_WORD);
    assert(tokens[0].value != NULL);
    assert(strcmp(tokens[0].value, "echo") == 0);
    assert(tokens[1].type == TK_WORD);
    assert(tokens[1].value != NULL);
    assert(strcmp(tokens[1].value, "hello") == 0);
    assert(tokens[2].type == TK_PIPE);
    assert(tokens[2].value == NULL);
    assert(tokens[3].type == TK_WORD);
    assert(tokens[3].value != NULL);
    assert(strcmp(tokens[3].value, "ls") == 0);
    assert(tokens[4].type == TK_EOF);
    assert(tokens[4].value == NULL);

    lexer_free_tokens(tokens, token_count);
}

int main(void) {
    lexer_test_basic_command();
    lexer_test_pipe();
    return 0;
}
