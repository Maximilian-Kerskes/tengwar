#pragma once

#include "tengwar/parser.h"

typedef int (*Builtin_Fn)(const Command *);

typedef struct {
    const char *name;
    Builtin_Fn fn;
} Builtin;

int builtin_is_builtin(const char *name);
int builtin_execute(const Command *command);
