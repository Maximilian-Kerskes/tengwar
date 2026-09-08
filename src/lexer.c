#include "tengwar/lexer.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static inline int lexer_is_space(char c) {
    return isspace((unsigned char)c) && c != '\n' && c != '\0';
}

static inline int lexer_is_word_char(char c) {
    return c != '\0' && c != '\n' && c != ';' && !lexer_is_space(c) && c != '<' && c != '>' &&
           c != '|';
}

static char *lexer_get_word(const char **c) {
    const char *start = *c;
    while (lexer_is_word_char(**c)) {
        (*c)++;
    }

    size_t len = *c - start;

    return strndup(start, len);
}

/*
 * Returns the number of tokens found
 */
size_t lexer_tokenize(char *LINE, Token *token_buf, size_t token_buf_size) {
    const char *c = LINE;
    Token *token = token_buf;

    while (token < token_buf + token_buf_size) {
        token->value = NULL;

        if (*c == '\0') {
            token->type = TK_EOF;
            token++;
            break;
        }

        if (lexer_is_space(*c)) {
            c++;
            continue;
        }

        if (lexer_is_word_char(*c)) {
            token->type = TK_WORD;
            token->value = lexer_get_word(&c);
            token++;

            continue;
        }

        if (*c == '\n') {
            token->type = TK_NEWLINE;
            token++;
            c++;
            continue;
        }

        switch (*c) {
        case ';':
            token->type = TK_SEMICOLON;
            token++;
            break;
        case '<':
            token->type = TK_REDIRECT_IN;
            token++;
            break;
        case '>':
            if (c[1] == '>') {
                token->type = TK_REDIRECT_APPEND;
                c += 2;
            } else {
                token->type = TK_REDIRECT_OUT;
                c++;
            }
            token++;
            break;
        case '|':
            token->type = TK_PIPE;
            token++;
            break;
        }

        c++;
    }

    return token - token_buf;
}

void lexer_free_tokens(Token *tokens, size_t token_count) {
    for (size_t i = 0; i < token_count; i++) {
        free(tokens[i].value);
    }
}
