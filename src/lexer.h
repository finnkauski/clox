#pragma once
#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <stdbool.h>
#define TOKEN_TYPE_LEN 40
#define LEXER_EXIT_STATUS 9

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

extern const char* TOKEN_VALUES[TOKEN_TYPE_LEN];
extern const char* TOKEN_NAMES[TOKEN_TYPE_LEN];


typedef struct {
  TokenType type;
  // string location in the source
  size_t line;
  const char* start;

  // maybe parsed value
  void* value;
} Token;


typedef struct {
  const char *current;
  const char *source;
  size_t source_len;

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

Lexer init_lexer(const char* source);
void free_lexer(Lexer* lexer);
void debug_lexer(const Lexer* lexer);
LexerStatus scan_tokens(Lexer* lexer);

void display_token(const Token* token);
void debug_token(const Token* token);

#endif // LEXER_H
