#pragma once

#include <stdbool.h>
#include <stddef.h>

// see doc/lexing.md for details

typedef enum Token_Type {
    TK_EOF,

    TK_WORD,

    TK_NEWLINE,
    TK_SEMICOLON,

    // <
    TK_REDIRECT_IN,
    // >
    TK_REDIRECT_OUT,
    // >>
    TK_REDIRECT_APPEND,

    // |
    TK_PIPE,

} Token_Type;

typedef struct Token {
    Token_Type type;
    char *value;
} Token;

size_t lexer_tokenize(char *LINE, Token *token_buf, size_t token_buf_size);

void lexer_free_tokens(Token *tokens, size_t token_count);
