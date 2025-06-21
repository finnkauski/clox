#pragma once
#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <stdbool.h>
#define LEXER_EXIT_FAILURE 1
#define MAX_NUMBER_DIGITS 256

#include "stb_ds.h"

typedef enum {
    LEXER_SUCCESS,
    LEXER_FAILURE
} LexerStatus;

typedef enum {
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE,
  TOKEN_RIGHT_BRACE,
  TOKEN_COMMA,
  TOKEN_COMMENT,
  TOKEN_DOT,
  TOKEN_MINUS,
  TOKEN_PLUS,
  TOKEN_SEMICOLON,
  TOKEN_SLASH,
  TOKEN_STAR,
  TOKEN_BANG,
  TOKEN_BANG_EQUAL,
  TOKEN_EQUAL,
  TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER,
  TOKEN_GREATER_EQUAL,
  TOKEN_LESS,
  TOKEN_LESS_EQUAL,
  TOKEN_IDENTIFIER,
  TOKEN_STRING,
  TOKEN_NUMBER,
  TOKEN_AND,
  TOKEN_CLASS,
  TOKEN_ELSE,
  TOKEN_FALSE,
  TOKEN_FOR,
  TOKEN_FUN,
  TOKEN_IF,
  TOKEN_NIL,
  TOKEN_OR,
  TOKEN_PRINT,
  TOKEN_RETURN,
  TOKEN_SUPER,
  TOKEN_THIS,
  TOKEN_TRUE,
  TOKEN_VAR,
  TOKEN_WHILE,
  TOKEN_ERROR,
  TOKEN_EOF
} TokenType;
#define TOKEN_TYPE_LEN 41

typedef struct {
    const char* name;
    const char * symbol;
} TokenRepr;

extern const TokenRepr TOKEN_REPRESENTATIONS[TOKEN_TYPE_LEN];

typedef struct {
    const char* keyword;
    TokenType type;
} Keyword;

// TODO: token TOKEN_REPRESENTATIONS has this data
static const Keyword KEYWORDS[] = {
    {"and",    TOKEN_AND},
    {"class",  TOKEN_CLASS},
    {"else",   TOKEN_ELSE},
    {"false",  TOKEN_FALSE},
    {"for",    TOKEN_FOR},
    {"fun",    TOKEN_FUN},
    {"if",     TOKEN_IF},
    {"nil",    TOKEN_NIL},
    {"or",     TOKEN_OR},
    {"print",  TOKEN_PRINT},
    {"return", TOKEN_RETURN},
    {"super",  TOKEN_SUPER},
    {"this",   TOKEN_THIS},
    {"true",   TOKEN_TRUE},
    {"var",    TOKEN_VAR},
    {"while",  TOKEN_WHILE}
};


typedef struct {
    const char* start;
    size_t length;
} String;

typedef struct {
    enum {
        TYPE_NULL,
        TYPE_IDENTIFIER,
        TYPE_BOOL,
        TYPE_NUMBER,
        TYPE_STRING
    } type;
    union {
        String identifier_value;
        String string_value;
        double number_value;
        bool bool_value;
    } as;
} Value;

typedef struct {
  TokenType type;
  // string location in the source
  size_t line;
  const char* start;

  // maybe parsed value
  Value value;
} Token;

#define PRINT_AS_STRING(KIND, TOKEN)                                   \
  for (size_t i = 0; i < (TOKEN)->value.as.KIND##_value.length; i++) { \
    printf("%c", (TOKEN)->value.as.KIND##_value.start[i]);             \
  }                                                                    \


typedef struct {
  const char *current;
  const char *source;
  size_t source_len;
  const char* source_filename;

  Token* tokens;  // Vec<Token>

  size_t line;
  size_t line_offset;

  // Runtime helpful flags
  bool finished;
  bool had_error;
} Lexer;

typedef struct {
    Lexer lexer;
} Lox ;


Lexer init_lexer(const char* filename, const char* source);
void free_lexer(Lexer* lexer);
void scan_tokens(Lexer* lexer);
Token next_token(Lexer* lexer);
#endif // LEXER_H
