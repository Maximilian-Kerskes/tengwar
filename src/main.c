#include "tengwar/executor.h"
#include "tengwar/lexer.h"
#include "tengwar/parser.h"
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

        Token tokens[128];
        size_t token_count = lexer_tokenize(line, tokens, 128);

        Parser parser = {
            .p_current = tokens,
        };

        List list;

        parse_list(&parser, &list);

        execute_list(&list);

        list_free(&list);
        lexer_free_tokens(tokens, token_count);
    }
    free(line);

    return 0;
}
