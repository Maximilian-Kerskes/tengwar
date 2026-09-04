#pragma once

#include "tengwar/lexer.h"
#include <stddef.h>
// See doc/parsing.md for details

typedef struct Command {
    char **argv;
    size_t argc;
    size_t capacity;
} Command;

typedef struct Pipeline {
    struct Command *commands;
    size_t command_count;
    size_t capacity;
} Pipeline;

typedef struct List {
    struct Pipeline *pipelines;
    size_t pipeline_count;
    size_t capacity;
} List;

typedef struct Parser {
    Token *p_current;
} Parser;

void list_free(List *list);

void parse_list(Parser *parser, List *list);
