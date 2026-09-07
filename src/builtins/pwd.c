#include "builtins_internal.h"
#include "tengwar/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int builtin_pwd(const Command *command) {
    char *cwd = getcwd(NULL, 0);

    if (cwd == NULL) {
        perror("pwd not working...");
        return 1;
    }

    puts(cwd);
    free(cwd);
    return 0;
}
