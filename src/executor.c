#include "tengwar/executor.h"
#include "tengwar/parser.h"
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

static int is_builtin(const Command *command) {
    // mock return value
    return 0;
}

static void execute_internal_command(const Command *command) {
    // noop
}

static void execute_external_command(const Command *command) {
    execvp(command->argv[0], command->argv);

    perror(command->argv[0]);
    _exit(127);
}

static void execute_command(const Command *command) {
    if (is_builtin(command)) {
        execute_internal_command(command);
    } else {
        execute_external_command(command);
    }
}

static void execute_pipeline(const Pipeline *pipeline) {
    int prev_in_fd = -1;
    pid_t pids[pipeline->command_count];

    Command *command = pipeline->commands;

    while (command < pipeline->commands + pipeline->command_count) {
        int fd[2];
        int is_last = command + 1 == pipeline->commands + pipeline->command_count;

        if (!is_last && pipe(fd) == -1) {
            perror("pipe() failed...");
            return;
        }

        pid_t pid = fork();

        if (pid == -1) {
            perror("fork() failed...");
            return;
        }

        if (pid == 0) {
            if (prev_in_fd != -1) {
                dup2(prev_in_fd, STDIN_FILENO);
            }

            if (!is_last) {
                dup2(fd[1], STDOUT_FILENO);
            }

            if (prev_in_fd != -1) {
                close(prev_in_fd);
            }

            if (!is_last) {
                close(fd[0]);
                close(fd[1]);
            }

            execute_command(command);
            _exit(127);
        }

        pids[command - pipeline->commands] = pid;

        if (prev_in_fd != -1) {
            close(prev_in_fd);
        }

        if (!is_last) {
            close(fd[1]);
            prev_in_fd = fd[0];
        }

        command++;
    }

    for (size_t i = 0; i < pipeline->command_count; i++) {
        waitpid(pids[i], NULL, 0);
    }
}

void execute_list(const List *list) {
    const Pipeline *pipeline = list->pipelines;

    while (pipeline < list->pipelines + list->pipeline_count) {
        execute_pipeline(pipeline);
        pipeline++;
    }
}
