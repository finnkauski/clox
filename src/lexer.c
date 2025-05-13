#include "lexer.h"
#include "utils.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define DEBUG 0
#if DEBUG
#define debug(fmt, ...) fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#define debug_block(...)                                                       \
  do {                                                                         \
    __VA_ARGS__                                                                \
  } while (0)
#else
#define debug(fmt, ...)                                                        \
  do {                                                                         \
  } while (0)
#define debug_block(...)                                                       \
  do {                                                                         \
  } while (0)
#endif

// Convenience mappings.
const char *TOKEN_NAMES[TOKEN_TYPE_LEN] = {
    [TOKEN_LEFT_PAREN] = "LEFT_PAREN",
    [TOKEN_RIGHT_PAREN] = "RIGHT_PAREN",
    [TOKEN_LEFT_BRACE] = "LEFT_BRACE",
    [TOKEN_RIGHT_BRACE] = "RIGHT_BRACE",
    [TOKEN_COMMA] = "COMMA",
    [TOKEN_COMMENT] = "COMMENT",
    [TOKEN_DOT] = "DOT",
    [TOKEN_MINUS] = "MINUS",
    [TOKEN_PLUS] = "PLUS",
    [TOKEN_SEMICOLON] = "SEMICOLON",
    [TOKEN_SLASH] = "SLASH",
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
    [TOKEN_COMMA] = ",",      [TOKEN_COMMENT] = "// ... \\n",
    [TOKEN_DOT] = ".",        [TOKEN_MINUS] = "-",
    [TOKEN_PLUS] = "+",       [TOKEN_SEMICOLON] = ";",
    [TOKEN_SLASH] = "/",      [TOKEN_STAR] = "*",
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
  debug("Creating new token: %s", TOKEN_NAMES[type]);
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

// If current token matches eat it
bool match(Lexer *lexer, const char expected) {
  // NOTE: reason here we don't increment is because in scanning
  // We are already incrementing so we want to check if the current
  // character matches and if it does we consume
  if (*(lexer->current) == expected) {
    debug("Consumed after match: `%c`", *lexer->current);
    advance(lexer);
    return true;
  };
  return false;
}

// Get current token
char peek(Lexer *lexer) {
  if (lexer->finished)
    return '\0';
  return *lexer->current;
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
  case '/':
    if (match(lexer, '/')) {
      debug("Starting comment line...");
      // Comments go to the end of the line
      while ((peek(lexer) != '\n') && !lexer->finished) {
        debug("Comment `%c`", *lexer->current);
        advance(lexer);
      }
      *token = newtoken(lexer, TOKEN_COMMENT);
    } else {
      *token = newtoken(lexer, TOKEN_SLASH);
    }
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

  // debug_token(token);

  return status;
}

LexerStatus consume_whitespace(Lexer *lexer) {
  // Indicates if we consumed any whitespace
  LexerStatus status = LEXER_FAILURE;
  ASSERT(lexer->current != NULL,
         "Lexer current is NULL while consuming whitespace");
  while (isspace(*lexer->current)) {
    if (*lexer->current == '\n') {
      debug("Consuming '\\n' and incrementing line");
      lexer->line++;
      advance(lexer);
      lexer->line_offset = 0; // RESET the line_offset
    } else {
      debug("Consuming WHITESPACE");
      advance(lexer);
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

    debug_block({
      fprintf(stderr, "<|");
      for (int debug_i = 0; debug_i < 10; debug_i++) {
        if (isprint(lexer->current[debug_i])) {
          fprintf(stderr, "%c", lexer->current[debug_i]);
        } else {
          // printf("\\x%02X", lexer->current[debug_i]);
          fprintf(stderr, "\\?");
        }
      };
      fprintf(stderr, "|>\n");
    });

    // Consume whitespace advances the lexer, we might be out of text
    // so loop back to check while loop condition;
    if (consume_whitespace(lexer) == LEXER_SUCCESS)
      continue;

    Token token;
    status = next_token(lexer, &token);

    if (status != LEXER_SUCCESS) {
      debug("Lexer failed");
      break; // scaning loop
    }

    ASSERT(status == LEXER_SUCCESS, "Lexer token parsing failed");

    if (token.type != TOKEN_COMMENT)
      arrput(lexer->tokens, token);
  }
  return status;
}

// Printing and displaying
void debug_token(const Token *token) {
  printf("Token {\n");
  printf(" type: %s\n", TOKEN_NAMES[token->type]);
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
         " current: `%c`,\n",
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
