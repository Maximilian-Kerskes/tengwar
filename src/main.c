#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int main(void) {
    char *line = NULL;
    size_t len = 0;

    while (1) {
        printf("> ");
        fflush(stdout);

        if (getline(&line, &len, stdin) == -1) {
            break;
        }

        if (strcmp(line, "ls\n") == 0) {
            pid_t pid = fork();

            if (pid == 0) {
                execlp("ls", "ls", NULL);
                perror("ls");
                exit(1);
            }

            waitpid(pid, NULL, 0);
        }

        if (strcmp(line, "exit\n") == 0) {
            break;
        }
    }
    free(line);

    return 0;
}
