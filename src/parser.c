#include "tengwar/parser.h"
#include "tengwar/lexer.h"
#include <stdlib.h>

#define ARRAY_PUSH(array, count, capacity, value)                                                  \
    do {                                                                                           \
        if ((count) == (capacity)) {                                                               \
            (capacity) = (capacity) ? (capacity) * 2 : 8;                                          \
            (array) = realloc((array), (capacity) * sizeof(*(array)));                             \
        }                                                                                          \
        (array)[(count)++] = (value);                                                              \
    } while (0)

static void command_free(Command *command) {
    for (size_t i = 0; i < command->argc; i++) {
        free(command->argv[i]);
    }
    free(command->argv);
}

static void pipeline_free(Pipeline *pipeline) {
    for (size_t i = 0; i < pipeline->command_count; i++) {
        command_free(&pipeline->commands[i]);
    }
    free(pipeline->commands);
}

void list_free(List *list) {
    for (size_t i = 0; i < list->pipeline_count; i++) {
        pipeline_free(&list->pipelines[i]);
    }
    free(list->pipelines);
}

static void parse_command(Parser *parser, Command *command) {
    *command = (Command){0};

    while (parser->p_current->type == TK_WORD) {
        ARRAY_PUSH(command->argv, command->argc, command->capacity, parser->p_current->value);
        parser->p_current->value = NULL;
        parser->p_current++;
    }

    /*
     * Execvc requires NULL termination in **argv
     */
    ARRAY_PUSH(command->argv, command->argc, command->capacity, NULL);
    command->argc--;
}

static void parse_pipeline(Parser *parser, Pipeline *pipeline) {
    *pipeline = (Pipeline){0};

    Command command;

    parse_command(parser, &command);
    ARRAY_PUSH(pipeline->commands, pipeline->command_count, pipeline->capacity, command);

    while (parser->p_current->type == TK_PIPE) {
        parser->p_current++;

        parse_command(parser, &command);
        ARRAY_PUSH(pipeline->commands, pipeline->command_count, pipeline->capacity, command);
    }
}

void parse_list(Parser *parser, List *list) {
    *list = (List){0};

    Pipeline pipeline;

    parse_pipeline(parser, &pipeline);
    ARRAY_PUSH(list->pipelines, list->pipeline_count, list->capacity, pipeline);

    while (parser->p_current->type == TK_SEMICOLON) {
        parser->p_current++;

        parse_pipeline(parser, &pipeline);
        ARRAY_PUSH(list->pipelines, list->pipeline_count, list->capacity, pipeline);
    }
}
