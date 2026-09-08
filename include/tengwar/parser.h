#pragma once

#include "tengwar/lexer.h"
#include <stddef.h>
// See doc/parsing.md for details
//

typedef enum RedirectType {
    REDIRECT_IN,
    REDIRECT_OUT,
    REDIRECT_APPEND,
} RedirectType;

typedef struct Redirect {
    RedirectType type;
    char *filename;
} Redirect;

typedef struct Command {
    char **argv;
    size_t argc;
    size_t capacity;

    Redirect *redirects;
    size_t redirect_count;
    size_t redirect_capacity;
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
