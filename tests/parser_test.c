#include "tengwar/lexer.h"
#include "tengwar/parser.h"

#include <assert.h>
#include <string.h>

static void test_single_command(void) {
    Token tokens[16];
    List list;

    size_t token_count = lexer_tokenize("echo hello", tokens, 16);

    Parser parser = {.p_current = tokens};

    parse_list(&parser, &list);

    assert(list.pipeline_count == 1);

    Pipeline *pipeline = &list.pipelines[0];

    assert(pipeline->command_count == 1);

    Command *command = &pipeline->commands[0];

    assert(command->argc == 2);
    assert(strcmp(command->argv[0], "echo") == 0);
    assert(strcmp(command->argv[1], "hello") == 0);
    assert(command->argv[2] == NULL);

    list_free(&list);
    lexer_free_tokens(tokens, token_count);
}

static void test_pipeline(void) {
    Token tokens[16];
    List list;

    size_t token_count = lexer_tokenize("echo hello | grep hello", tokens, 16);

    Parser parser = {.p_current = tokens};

    parse_list(&parser, &list);

    assert(list.pipeline_count == 1);

    Pipeline *pipeline = &list.pipelines[0];

    assert(pipeline->command_count == 2);

    Command *echo = &pipeline->commands[0];
    Command *grep = &pipeline->commands[1];

    assert(echo->argc == 2);
    assert(strcmp(echo->argv[0], "echo") == 0);
    assert(strcmp(echo->argv[1], "hello") == 0);

    assert(grep->argc == 2);
    assert(strcmp(grep->argv[0], "grep") == 0);
    assert(strcmp(grep->argv[1], "hello") == 0);

    assert(echo->argv[2] == NULL);
    assert(grep->argv[2] == NULL);

    list_free(&list);
    lexer_free_tokens(tokens, token_count);
}

static void test_list(void) {
    Token tokens[16];
    List list;

    size_t token_count = lexer_tokenize("echo hello; echo world", tokens, 16);

    Parser parser = {.p_current = tokens};

    parse_list(&parser, &list);

    assert(list.pipeline_count == 2);

    Command *first = &list.pipelines[0].commands[0];
    Command *second = &list.pipelines[1].commands[0];

    assert(strcmp(first->argv[0], "echo") == 0);
    assert(strcmp(first->argv[1], "hello") == 0);

    assert(strcmp(second->argv[0], "echo") == 0);
    assert(strcmp(second->argv[1], "world") == 0);

    list_free(&list);
    lexer_free_tokens(tokens, token_count);
}

int main(void) {
    test_single_command();
    test_pipeline();
    test_list();

    return 0;
}
