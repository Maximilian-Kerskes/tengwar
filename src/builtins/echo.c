#include "builtins_internal.h"
#include "tengwar/parser.h"
#include <stdio.h>

int builtin_echo(const Command *command) {
    for (size_t i = 1; i < command->argc; i++) {
        if (i > 1) {
            putchar(' ');
        }
        fputs(command->argv[i], stdout);
    }
    putchar('\n');
    return 0;
}
