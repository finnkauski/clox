#include "lexer.h"
#include "utils.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUMBER_DIGITS 256

TokenType identifier_type(const char *start, uint32_t length) {
  for (size_t i = 0; i < (sizeof(KEYWORDS) / sizeof(KEYWORDS[0])); ++i) {
    const char *keyword = KEYWORDS[i].keyword;
    size_t kw_len = strlen(keyword);
    if (length == kw_len && strncmp(start, keyword, length) == 0) {
      return KEYWORDS[i].type;
    }
  }
  return TOKEN_IDENTIFIER;
}

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
    [TOKEN_IDENTIFIER] = "IDENTIFIER",
    [TOKEN_STRING] = "STRING",
    [TOKEN_NUMBER] = "NUMBER",
    [TOKEN_AND] = "AND",
    [TOKEN_CLASS] = "CLASS",
    [TOKEN_ELSE] = "ELSE",
    [TOKEN_FALSE] = "FALSE",
    [TOKEN_FOR] = "FOR",
    [TOKEN_FUN] = "FUN",
    [TOKEN_IF] = "IF",
    [TOKEN_NIL] = "NIL",
    [TOKEN_OR] = "OR",
    [TOKEN_PRINT] = "PRINT",
    [TOKEN_RETURN] = "RETURN",
    [TOKEN_SUPER] = "SUPER",
    [TOKEN_THIS] = "THIS",
    [TOKEN_TRUE] = "TRUE",
    [TOKEN_VAR] = "VAR",
    [TOKEN_WHILE] = "WHILE",
    [TOKEN_EOF] = "EOF",
};

const char *TOKEN_VALUES[TOKEN_TYPE_LEN] = {
    [TOKEN_LEFT_PAREN] = "(",
    [TOKEN_RIGHT_PAREN] = ")",
    [TOKEN_LEFT_BRACE] = "{",
    [TOKEN_RIGHT_BRACE] = "}",
    [TOKEN_COMMA] = ",",
    [TOKEN_COMMENT] = "// ... \\n",
    [TOKEN_DOT] = ".",
    [TOKEN_MINUS] = "-",
    [TOKEN_PLUS] = "+",
    [TOKEN_SEMICOLON] = ";",
    [TOKEN_SLASH] = "/",
    [TOKEN_STAR] = "*",
    [TOKEN_BANG] = "!",
    [TOKEN_BANG_EQUAL] = "!=",
    [TOKEN_EQUAL] = "=",
    [TOKEN_EQUAL_EQUAL] = "==",
    [TOKEN_GREATER] = ">",
    [TOKEN_GREATER_EQUAL] = ">=",
    [TOKEN_LESS] = "<",
    [TOKEN_LESS_EQUAL] = "<=",
    [TOKEN_IDENTIFIER] = "'IDENTIFIER'",
    [TOKEN_STRING] = "'STRING'",
    [TOKEN_NUMBER] = "NUMBER",
    [TOKEN_AND] = "AND",
    [TOKEN_CLASS] = "CLASS",
    [TOKEN_ELSE] = "ELSE",
    [TOKEN_FALSE] = "FALSE",
    [TOKEN_FOR] = "FOR",
    [TOKEN_FUN] = "FUN",
    [TOKEN_IF] = "IF",
    [TOKEN_NIL] = "NIL",
    [TOKEN_OR] = "OR",
    [TOKEN_PRINT] = "PRINT",
    [TOKEN_RETURN] = "RETURN",
    [TOKEN_SUPER] = "SUPER",
    [TOKEN_THIS] = "THIS",
    [TOKEN_TRUE] = "TRUE",
    [TOKEN_VAR] = "VAR",
    [TOKEN_WHILE] = "WHILE",
    [TOKEN_EOF] = "",
};

Lexer init_lexer(const char *filename, const char *source) {
  size_t source_len = strlen(source);
  Token *tokens = NULL;
  return (Lexer){.current = source,
                 .source = source,
                 .source_filename = filename,
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
void errorf(const char *filename, size_t line, size_t line_offset,
            const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "%s:%zu:%zu ", filename, line + 1, line_offset);
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  va_end(args);
}

// Core
unsigned long get_offset(const Lexer *lexer) {
  ASSERT(lexer->current >= lexer->source,
         "Current pointer less than source pointer for a Lexer");
  return lexer->current - lexer->source;
}

char advance(Lexer *lexer) {
  char c = *lexer->current;

  debug("Advanced: %c", c);

  ASSERT(c != '\0' || !lexer->finished, "Tried advancing a finished parser.");

  lexer->current++;
  // After consuming the whole string the final current pointer
  // will point to the null character.
  if (c == '\0') {
    lexer->finished = true;
    debug("Lexer finished");
  } else {
    lexer->line_offset++;
  }

  return c;
}

// Token constructor for simple tokens
Token token_at(Lexer *lexer, TokenType type, const char *start, Value value) {
  debug("Creating new token: %s", TOKEN_NAMES[type]);
  return (Token){
      .type = type, .line = lexer->line, .start = start, .value = value};
}

// Simply make a new token with no value
Token newtoken(Lexer *lexer, TokenType type) {
  const char *start = lexer->current;
  return token_at(lexer, type, start, (Value){.type = TYPE_NULL});
}

// If current token matches eat it
bool match(Lexer *lexer, const char expected) {
  // NOTE: reason here we don't increment is because in scanning
  // We are already incrementing so we want to check if the current
  // character matches and if it does we consume
  if (*(lexer->current) == expected) {
    debug("Matched: %c", expected);
    advance(lexer);
    return true;
  };
  debug("Not Matched: %c", expected);
  return false;
}

// Get current token
char peek(Lexer *lexer) {
  if (lexer->finished)
    return '\0';
  debug("Peeked");
  return *lexer->current;
}

char peekn(Lexer *lexer, uint32_t n) {
  if (lexer->finished)
    return '\0';
  debug("Peeked n: %d", n);
  return *(lexer->current + n - 1);
}

// Consume a whole comment
bool match_comment(Lexer *lexer) {
  if (match(lexer, '/')) {
    debug("Matched comment");
    while ((peek(lexer) != '\n') && !lexer->finished) {
      advance(lexer);
    }

    return true;
  }
  return false;
}

// Parse identifier
Token parse_identifier(Lexer *lexer) {
  // NOTE: we should have consumed the first digit
  const char *start = lexer->current - 1;
  size_t length = 1;

  ASSERT(isalpha(*start) || *start == '_',
         "Starting character for identifier found to be non-alpha");

  char c = peek(lexer);
  while ((isalnum(c) || c == '_') && !lexer->finished) {
    length++;
    advance(lexer);
    c = peek(lexer);
  }

  TokenType type = identifier_type(start, length);
  Value value = {.type = type == TOKEN_IDENTIFIER ? TYPE_IDENTIFIER : TYPE_KEYWORD,
                 .as.string_value = {.start = start, .length = length}};
  return token_at(lexer, type, start, value);
}

// Parse a string token
Token parse_string(Lexer *lexer) {
  const char *start = lexer->current;
  size_t length = 0;

  debug("Parsing string");
  debug_block({ fprintf(stderr, "STRING \""); });
  while (peek(lexer) != '"' && !lexer->finished) {

#if DEBUG
    char c = advance(lexer);
    debug_block({ fprintf(stderr, "%c", c); });
#else
    advance(lexer);
#endif

    length++;
  }
  debug_block({ fprintf(stderr, "\" (length: %zu)\n", length); });

  // At the end of parsing we will be on the `"` and so we
  // consume that one more time.
  advance(lexer);

  Value value = {.type = TYPE_STRING,
                 .as.string_value = {.start = start, .length = length}};
  return token_at(lexer, TOKEN_STRING, start, value);
}

Token parse_number(Lexer *lexer) {
  // NOTE: we should have consumed the first digit
  const char *start = lexer->current - 1;
  size_t length = 1;

  ASSERT(isdigit(*start;),
         "Starting character found to be not digit when parsing number");

  bool encountered_dot = false;
  char c = peek(lexer);
  while ((isdigit(c) || (c == '.' && !encountered_dot)) && !lexer->finished) {
    if (c == '.' && !encountered_dot) {
      // NOTE: make sure that we have a digit after the dot
      if (!isdigit(peekn(lexer, 2))) {
        break;
      }
      encountered_dot = true;
    }
    length++;
    advance(lexer);
    c = peek(lexer);
  }
  // NOTE: we do this to do the conversion with `strtod`
  char buf[MAX_NUMBER_DIGITS + 1]; // enough space for number + null terminator
  memcpy(buf, start, length);
  buf[length] = '\0';

  Value value = {.type = TYPE_NUMBER, .as.number_value = strtod(buf, NULL)};

  return token_at(lexer, TOKEN_NUMBER, start, value);
}

void consume_whitespace(Lexer *lexer) {
  // Indicates if we consumed any whitespace
  ASSERT(lexer->current != NULL,
         "Lexer current is NULL while consuming whitespace");
  while (isspace(*lexer->current)) {
    if (*lexer->current == '\n') {
      debug("Consuming WHITESPACE ('\\n')");
      lexer->line++;
      advance(lexer);
      lexer->line_offset = 0; // RESET the line_offset
    } else {
      debug("Consuming WHITESPACE");
      advance(lexer);
    }
  }
}

Token next_token(Lexer *lexer) {
  ASSERT(!lexer->finished, "Tried to fetch next token from a finished lexer");

  consume_whitespace(lexer);

  Token token;
  char c = advance(lexer);
  switch (c) {
  case '(':
    token = newtoken(lexer, TOKEN_LEFT_PAREN);
    break;
  case ')':
    token = newtoken(lexer, TOKEN_RIGHT_PAREN);
    break;
  case '{':
    token = newtoken(lexer, TOKEN_LEFT_BRACE);
    break;
  case '}':
    token = newtoken(lexer, TOKEN_RIGHT_BRACE);
    break;
  case ',':
    token = newtoken(lexer, TOKEN_COMMA);
    break;
  case '.':
    token = newtoken(lexer, TOKEN_DOT);
    break;
  case '-':
    token = newtoken(lexer, TOKEN_MINUS);
    break;
  case '+':
    token = newtoken(lexer, TOKEN_PLUS);
    break;
  case ';':
    token = newtoken(lexer, TOKEN_SEMICOLON);
    break;
  case '*':
    token = newtoken(lexer, TOKEN_STAR);
    break;
  case '/':
    token = match_comment(lexer) ? newtoken(lexer, TOKEN_COMMENT)
                                 : newtoken(lexer, TOKEN_SLASH);
    break;
  case '!':
    token = match(lexer, '=') ? newtoken(lexer, TOKEN_BANG_EQUAL)
                              : newtoken(lexer, TOKEN_BANG);
    break;
  case '=':
    token = match(lexer, '=') ? newtoken(lexer, TOKEN_EQUAL_EQUAL)
                              : newtoken(lexer, TOKEN_EQUAL);
    break;
  case '>':
    token = match(lexer, '=') ? newtoken(lexer, TOKEN_GREATER_EQUAL)
                              : newtoken(lexer, TOKEN_GREATER);
    break;
  case '<':
    token = match(lexer, '=') ? newtoken(lexer, TOKEN_LESS_EQUAL)
                              : newtoken(lexer, TOKEN_LESS);
    break;
  case '"':
    token = parse_string(lexer);
    break;

  case '\0':
    token = newtoken(lexer, TOKEN_EOF);
    break;

  default:
    if (isdigit(c)) {
      token = parse_number(lexer);
    } else if (isalpha(c) || c == '_') {
      token = parse_identifier(lexer);
    } else {
      // NOTE: doesn't change lexer state the current in the case of failure;
      errorf(lexer->source_filename, lexer->line, lexer->line_offset,
             "Unexpected character: %c", c);
      lexer->had_error = true;
      token = newtoken(lexer, TOKEN_ERROR);
    }
  }

  return token;
}

void scan_tokens(Lexer *lexer) {
  while ((size_t)(lexer->current - lexer->source) < lexer->source_len) {
    ASSERT(lexer->current != NULL,
           "NULL `current` found for the given lexer inside of scan_token");

    debug_block({ preview_string(lexer->current); });

    Token token = next_token(lexer);

    if (token.type != TOKEN_COMMENT && token.type != TOKEN_ERROR)
      arrput(lexer->tokens, token);

    if (lexer->finished)
      break;
  }
}
