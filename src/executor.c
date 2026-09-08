#include "tengwar/executor.h"
#include "tengwar/builtins/builtins.h"
#include "tengwar/parser.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

static int apply_redirections(const Command *command) {
    for (size_t i = 0; i < command->redirect_count; i++) {
        const Redirect *redirect = &command->redirects[i];

        int fd;

        switch (redirect->type) {
        case REDIRECT_IN:
            fd = open(redirect->filename, O_RDONLY);
            break;
        case REDIRECT_OUT:
            fd = open(redirect->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            break;
        case REDIRECT_APPEND:
            fd = open(redirect->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
            break;
        }

        if (fd == -1) {
            perror(redirect->filename);
            return -1;
        }

        int target = redirect->type == REDIRECT_IN ? STDIN_FILENO : STDOUT_FILENO;

        if (dup2(fd, target) == -1) {
            perror("dup2");
            close(fd);
            return -1;
        }
        close(fd);
    }
    return 0;
}

static int execute_parent_builtin(const Command *command) {
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO);

    if (apply_redirections(command) == -1) {
        perror("apply_redirections");
        dup2(saved_stdin, STDIN_FILENO);
        dup2(saved_stdout, STDOUT_FILENO);

        close(saved_stdin);
        close(saved_stdout);
        return -1;
    }

    int status = builtin_execute(command);
    dup2(saved_stdin, STDIN_FILENO);
    dup2(saved_stdout, STDOUT_FILENO);

    close(saved_stdin);
    close(saved_stdout);

    return status;
}

static int execute_child_builtin(const Command *command) { return builtin_execute(command); }

static void execute_external_command(const Command *command) {
    execvp(command->argv[0], command->argv);

    perror(command->argv[0]);
    _exit(127);
}

static void execute_child_command(const Command *command) {
    if (builtin_is_builtin(command->argv[0])) {
        _exit(execute_child_builtin(command));
    } else {
        execute_external_command(command);
    }
}

static void execute_pipeline(const Pipeline *pipeline) {
    int prev_in_fd = -1;
    pid_t pids[pipeline->command_count];

    Command *command = pipeline->commands;

    if (pipeline->command_count == 1 && builtin_is_builtin(command->argv[0])) {
        execute_parent_builtin(command);
        return;
    }

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

            if (apply_redirections(command) == -1) {
                _exit(1);
            }

            execute_child_command(command);
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
