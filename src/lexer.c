#include "lexer.h"
#include "utils.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

// Convenience mappings.
const char *TOKEN_NAMES[TOKEN_TYPE_LEN] = {
    [TOKEN_LEFT_PAREN] = "LEFT_PAREN",
    [TOKEN_RIGHT_PAREN] = "RIGHT_PAREN",
    [TOKEN_LEFT_BRACE] = "LEFT_BRACE",
    [TOKEN_RIGHT_BRACE] = "RIGHT_BRACE",
    [TOKEN_COMMA] = "COMMA",
    [TOKEN_DOT] = "DOT",
    [TOKEN_MINUS] = "MINUS",
    [TOKEN_PLUS] = "PLUS",
    [TOKEN_SEMICOLON] = "SEMICOLON",
    [TOKEN_STAR] = "STAR",
    [TOKEN_BANG] = "BANG",
    [TOKEN_BANG_EQUAL] = "BANG_EQUAL",
    [TOKEN_EQUAL] = "EQUAL",
    [TOKEN_EQUAL_EQUAL] = "EQUAL_EQUAL",
    [TOKEN_GREATER] = "GREATER",
    [TOKEN_GREATER_EQUAL] = "GREATER_EQUAL",
    [TOKEN_LESS] = "LESS",
    [TOKEN_LESS_EQUAL] = "LESS_EQUAL",
    [TOKEN_EOF] = "EOF",
};

const char *TOKEN_VALUES[TOKEN_TYPE_LEN] = {
    [TOKEN_LEFT_PAREN] = "(", [TOKEN_RIGHT_PAREN] = ")",
    [TOKEN_LEFT_BRACE] = "{", [TOKEN_RIGHT_BRACE] = "}",
    [TOKEN_COMMA] = ",",      [TOKEN_DOT] = ".",
    [TOKEN_MINUS] = "-",      [TOKEN_PLUS] = "+",
    [TOKEN_SEMICOLON] = ";",  [TOKEN_STAR] = "*",
    [TOKEN_BANG] = "!",       [TOKEN_BANG_EQUAL] = "!=",
    [TOKEN_EQUAL] = "=",      [TOKEN_EQUAL_EQUAL] = "==",
    [TOKEN_GREATER] = ">",    [TOKEN_GREATER_EQUAL] = ">=",
    [TOKEN_LESS] = "<",       [TOKEN_LESS_EQUAL] = "<=",
    [TOKEN_EOF] = "",
};

Lexer init_lexer(const char *source) {
  size_t source_len = strlen(source);
  Token *tokens = NULL;
  return (Lexer){.current = source,
                 .source = source,
                 .source_len = source_len,
                 .tokens = tokens,
                 .line = 0,
                 .line_offset = 0};
}

// Free the lexer **AND THE TOKENS** array.
void free_lexer(Lexer *lexer) {
  lexer->current = NULL;
  free((void *)lexer->source);
  lexer->source = NULL;
  lexer->source_len = 0;
  lexer->line = 0;
  lexer->line_offset = 0;

  arrfree(lexer->tokens);
  lexer->tokens = NULL;

  ASSERT(lexer->current == NULL,
         "Lexer `current` is not a null pointer after free.");
  ASSERT(lexer->source == NULL,
         "Lexer `source` is not a null pointer after free.");
  ASSERT(lexer->tokens == NULL,
         "Lexer `tokens` is not a null pointer after free.");
}

// Error
void error(size_t line, size_t line_offset, const char *message) {
  fprintf(stderr, "\n[line %zu, col: %zu] Error: %s", line, line_offset,
          message);
}

// Core
unsigned long get_offset(const Lexer *lexer) {
  ASSERT(lexer->current >= lexer->source,
         "Current pointer less than source pointer for a Lexer");
  return lexer->current - lexer->source;
}

char advance(Lexer *lexer) {
  char c = *lexer->current;

  ASSERT((c != '\0') || lexer->finished, "Tried advancing a finished parser.");

  lexer->current++;
  // After consuming the whole string the final current pointer
  // will point to the null character.
  if (*lexer->current == '\0') {
    lexer->finished = true;
  } else {
    lexer->line_offset++;
  }

  return c;
}

Token token_at(Lexer *lexer, TokenType type, const char *start) {
  return (Token){
      .type = type,
      .line = lexer->line,
      .start = start,
      .value = 0,
  };
}

Token newtoken(Lexer *lexer, TokenType type) {
  const char *start = lexer->current;
  return token_at(lexer, type, start);
}

bool match(Lexer *lexer, const char expected) {
  if (*(lexer->current + 1) == expected) {
    lexer->current++;
    printf("MATCHED offset: %zu\n", get_offset(lexer));
    return true;
  };
  return false;
}

LexerStatus next_token(Lexer *lexer, Token *token) {
  LexerStatus status = LEXER_SUCCESS;

  switch (advance(lexer)) {
  case '(':
    *token = newtoken(lexer, TOKEN_LEFT_PAREN);
    break;
  case ')':
    *token = newtoken(lexer, TOKEN_RIGHT_PAREN);
    break;
  case '{':
    *token = newtoken(lexer, TOKEN_LEFT_BRACE);
    break;
  case '}':
    *token = newtoken(lexer, TOKEN_RIGHT_BRACE);
    break;
  case ',':
    *token = newtoken(lexer, TOKEN_COMMA);
    break;
  case '.':
    *token = newtoken(lexer, TOKEN_DOT);
    break;
  case '-':
    *token = newtoken(lexer, TOKEN_MINUS);
    break;
  case '+':
    *token = newtoken(lexer, TOKEN_PLUS);
    break;
  case ';':
    *token = newtoken(lexer, TOKEN_SEMICOLON);
    break;
  case '*':
    *token = newtoken(lexer, TOKEN_STAR);
    break;
  case '!':
    *token = match(lexer, '=') ? newtoken(lexer, TOKEN_BANG_EQUAL)
                               : newtoken(lexer, TOKEN_BANG);
    break;
  case '=':
    *token = match(lexer, '=') ? newtoken(lexer, TOKEN_EQUAL_EQUAL)
                               : newtoken(lexer, TOKEN_EQUAL);
    break;
  case '>':
    *token = match(lexer, '=') ? newtoken(lexer, TOKEN_GREATER_EQUAL)
                               : newtoken(lexer, TOKEN_GREATER);
    break;
  case '<':
    *token = match(lexer, '=') ? newtoken(lexer, TOKEN_LESS_EQUAL)
                               : newtoken(lexer, TOKEN_LESS);
    break;
  default:
    // NOTE: doesn't change lexer state the current in the case of failure;
    error(lexer->line, lexer->line_offset, "Unexpected character.");
    status = LEXER_FAILURE;
  }

  ASSERT(token->type != TOKEN_ERROR,
         "Did not handle a given token, found TOKEN_ERROR during scanning");

  debug_token(token);

  return status;
}

LexerStatus consume_whitespace(Lexer *lexer) {
  // Indicates if we consumed any whitespace
  LexerStatus status = LEXER_FAILURE;
  ASSERT(lexer->current != NULL,
         "Lexer current is NULL while consuming whitespace");
  while (isspace(*lexer->current)) {
    if (*lexer->current == '\n') {
      lexer->line++;
      // printf("BEFORE `%c` ", *lexer->current);
      // printf("NOM `%c` ", advance(lexer));
      // printf("NOW `%c`\n", *lexer->current);
      advance(lexer);
      lexer->line_offset = 0; // RESET the line_offset
    } else {
      advance(lexer);
      // printf("BEFORE `%c` ", *lexer->current);
      // printf("NOM `%c` ", advance(lexer));
      // printf("NOW `%c`\n", *lexer->current);
    }
    status = LEXER_SUCCESS;
  }
  return status;
}

LexerStatus scan_tokens(Lexer *lexer) {
  LexerStatus status = LEXER_SUCCESS;
  while ((size_t)(lexer->current - lexer->source) < lexer->source_len) {

    ASSERT(lexer->current != NULL,
           "NULL `current` found for the given lexer inside of scan_token");

    // Consume whitespace advances the lexer, we might be out of text
    // so loop back to check while loop condition;
    if (consume_whitespace(lexer) == LEXER_SUCCESS)
      continue;

    Token token;
    status = next_token(lexer, &token);

    if (status != LEXER_SUCCESS) {
      break; // scaning loop
    }

    ASSERT(status == LEXER_SUCCESS, "Lexer token parsing failed");

    // printf("line: %zu, offset: %zu, character `%c`, type: "
    //        "%s\n",
    //        lexer->line, lexer->line_offset, *lexer->current,
    //        TOKEN_NAMES[token.type]);

    // display_token(&token);
    arrput(lexer->tokens, token);
  }
  return status;
}

// Printing and displaying
void debug_token(const Token *token) {
  printf("Token {\n");
  printf(" type: %s", TOKEN_NAMES[token->type]);
  printf(" line: %zu\n"
         " start: %p\n"
         " has_value: %d\n",
         token->line, (void *)token->start, token->value != NULL);
  printf("}\n");
}

void display_token(const Token *token) {
  printf("%s\n", TOKEN_NAMES[token->type]);
}

void debug_lexer(const Lexer *lexer) {
  ASSERT(lexer->source != NULL, "Lexer source is NULL during display");
  ASSERT(lexer->current != NULL, "Lexer current is NULL during display");

  printf("Lexer {\n"
         " current: %c,\n",
         *lexer->current);

  // truncate the source code
  if (lexer->source_len > 10)
    printf(" source: `%.10s...`,\n", lexer->source);
  else
    printf(" source: `%s`,\n", lexer->source);
  printf(" source_len: %zu,\n", lexer->source_len);

  // print token vector
  printf(" tokens: [...]\n");
  printf(" line: %zu,\n"
         " line_offset: %zu,\n"
         "}\n",
         lexer->line, lexer->line_offset);
}
