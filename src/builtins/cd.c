#include "tengwar/parser.h"
#include "builtins_internal.h"
#include <stdio.h>
#include <unistd.h>

int builtin_cd(const Command *command) {
    if (command->argc < 2) {
        return 0;
    }

    if (chdir(command->argv[1]) == -1) {
        perror("cd failed...");
        return 1;
    }

    return 0;
}
