#include "builtins_internal.h"
#include "tengwar/parser.h"
#include <stdio.h>
#include <stdlib.h>

int builtin_unset(const Command *command) {
	for (size_t i = 1; i < command->argc; i++) {
		if (unsetenv(command->argv[i]) == -1) {
			perror("unsetenv...");
			return 1;
		}
	}

	return 0;
}
