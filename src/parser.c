#include "tengwar/parser.h"
#include "tengwar/lexer.h"
#include <stdbool.h>
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

    for (size_t i = 0; i < command->redirect_count; i++) {
        free(command->redirects[i].filename);
    }
    free(command->redirects);
    free(command->argv);
    *command = (Command){0};
}

static void pipeline_free(Pipeline *pipeline) {
    for (size_t i = 0; i < pipeline->command_count; i++) {
        command_free(&pipeline->commands[i]);
    }
    free(pipeline->commands);
    *pipeline = (Pipeline){0};
}

void list_free(List *list) {
    for (size_t i = 0; i < list->pipeline_count; i++) {
        pipeline_free(&list->pipelines[i]);
    }
    free(list->pipelines);
    *list = (List){0};
}

static int parse_redirect(Parser *parser, Command *command, RedirectType type) {
    parser->p_current++;

    if (parser->p_current->type != TK_WORD) {
        return -1;
    }

    char *filename = parser->p_current->value;
    parser->p_current->value = NULL;
    parser->p_current++;

    ARRAY_PUSH(command->redirects, command->redirect_count, command->redirect_capacity,
               ((Redirect){.type = type, .filename = filename}));
    return 0;
}

static int parse_command(Parser *parser, Command *command) {
    *command = (Command){0};

    bool saw_word = false;

    while (parser->p_current->type == TK_WORD || parser->p_current->type == TK_REDIRECT_IN ||
           parser->p_current->type == TK_REDIRECT_OUT ||
           parser->p_current->type == TK_REDIRECT_APPEND) {
        switch (parser->p_current->type) {
        case TK_WORD:
            saw_word = true;
            ARRAY_PUSH(command->argv, command->argc, command->capacity, parser->p_current->value);
            parser->p_current->value = NULL;
            parser->p_current++;
            break;

        case TK_REDIRECT_IN:
            if (parse_redirect(parser, command, REDIRECT_IN) != 0) {
                command_free(command);
                return -1;
            }
            break;
        case TK_REDIRECT_OUT:
            if (parse_redirect(parser, command, REDIRECT_OUT) != 0) {
                command_free(command);
                return -1;
            }
            break;
        case TK_REDIRECT_APPEND:
            if (parse_redirect(parser, command, REDIRECT_APPEND) != 0) {
                command_free(command);
                return -1;
            }
            break;
        default:
            break;
        }
    }

    if (!saw_word) {
        command_free(command);
        return -1;
    }

    /*
     * Execvc requires NULL termination in **argv
     */
    ARRAY_PUSH(command->argv, command->argc, command->capacity, NULL);
    command->argc--;

    return 0;
}

static int parse_pipeline(Parser *parser, Pipeline *pipeline) {
    *pipeline = (Pipeline){0};

    Command command;

    if (parse_command(parser, &command) != 0) {
        pipeline_free(pipeline);
        return -1;
    };

    ARRAY_PUSH(pipeline->commands, pipeline->command_count, pipeline->capacity, command);

    while (parser->p_current->type == TK_PIPE) {
        parser->p_current++;

        if (parse_command(parser, &command) != 0) {
            pipeline_free(pipeline);
            return -1;
        }
        ARRAY_PUSH(pipeline->commands, pipeline->command_count, pipeline->capacity, command);
    }

    return 0;
}

int parse_list(Parser *parser, List *list) {
    *list = (List){0};

    if (parser->p_current->type == TK_NEWLINE || parser->p_current->type == TK_EOF) {
        return 0;
    }

    Pipeline pipeline;

    if (parse_pipeline(parser, &pipeline) != 0) {
        list_free(list);
        return -1;
    };

    ARRAY_PUSH(list->pipelines, list->pipeline_count, list->capacity, pipeline);

    while (parser->p_current->type == TK_SEMICOLON) {
        parser->p_current++;

        if (parse_pipeline(parser, &pipeline) != 0) {
            list_free(list);
            return -1;
        };
        ARRAY_PUSH(list->pipelines, list->pipeline_count, list->capacity, pipeline);
    }

    return 0;
}
