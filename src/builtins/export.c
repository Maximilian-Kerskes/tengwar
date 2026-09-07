#include "builtins_internal.h"
#include "tengwar/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int builtin_export(const Command *command) {
    for (size_t i = 1; i < command->argc; i++) {
        char *eq = strchr(command->argv[i], '=');
        if (eq == NULL) {
            continue;
        }

        size_t len = eq - command->argv[i];

        char *name = malloc(len + 1);
        if (name == NULL) {
            perror("malloc...");
            return 1;
        }

        memcpy(name, command->argv[i], len);

        name[len] = '\0';
        if (setenv(name, eq + 1, 1) == -1) {
            perror("setenv...");
            free(name);
            return 1;
        }
        free(name);
    }

    return 0;
}
