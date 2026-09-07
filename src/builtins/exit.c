#include "builtins_internal.h"
#include "tengwar/parser.h"
#include <stdlib.h>

int builtin_exit(const Command *command) {
    if (command->argc > 1) {
        exit(atoi(command->argv[1]));
    }
    exit(0);
}
