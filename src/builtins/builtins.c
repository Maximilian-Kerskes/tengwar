#include "tengwar/builtins/builtins.h"
#include "builtins_internal.h"
#include <string.h>

static const Builtin builtins[] = {
    {"cd", builtin_cd},       {"exit", builtin_exit},   {"export", builtin_export},
    {"unset", builtin_unset}, {"pwd", builtin_pwd},     {"echo", builtin_echo},
    {"true", builtin_true},   {"false", builtin_false},
};

static const Builtin *find_builtin(const char *name) {
    for (size_t i = 0; i < sizeof(builtins) / sizeof(*builtins); i++) {
        if (strcmp(builtins[i].name, name) == 0) {
            return &builtins[i];
        }
    }

    return NULL;
}

int builtin_is_builtin(const char *name) { return find_builtin(name) != NULL; }

int builtin_execute(const Command *command) {
    const Builtin *builtin = find_builtin(command->argv[0]);
    return builtin->fn(command);
}
