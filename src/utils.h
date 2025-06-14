#ifndef UTILS_H
#define UTILS_H
#include "lexer.h"
#include <stdio.h>

#define DEBUG 0
#if DEBUG

#define ASSERT(cond, ...)                                                      \
  do {                                                                         \
    if (!(cond)) {                                                             \
      fprintf(stderr, "[ASSERT FAILED] %s:%d: %s\n", __FILE__, __LINE__,       \
              #cond);                                                          \
      if (*#__VA_ARGS__) {                                                     \
        fprintf(stderr, "  Message: " __VA_ARGS__);                            \
        fprintf(stderr, "\n");                                                 \
      }                                                                        \
      abort();                                                                 \
    }                                                                          \
  } while (0)

#define debug(FMT, ...) fprintf(stderr, "[DEBUG] " FMT "\n", ##__VA_ARGS__)

#define debug_block(...)                                                       \
  do {                                                                         \
    __VA_ARGS__                                                                \
  } while (0)

#define preview_string(STRING)                                                   \
    do {                                                                         \
      fprintf(stderr, "[DEBUG] <|");                                             \
      for (int debug_i      =  0; debug_i < 10; debug_i++) {                     \
        if ((STRING)[debug_i] == '\0')                                           \
          break;                                                                 \
        if (isprint((STRING)[debug_i])) {                                        \
          fprintf(stderr, "%c", (STRING)[debug_i]);                              \
        } else {                                                                 \
          fprintf(stderr, "\\?");                                                \
        }                                                                        \
      };                                                                         \
      fprintf(stderr, "|>\n");                                                   \
    } while(0)                                                 
#else

#define ASSERT(cond, ...) ((void)0)

#define debug(fmt, ...)                                                        \
  do {                                                                         \
  } while (0)

#define debug_block(...)                                                       \
  do {                                                                         \
  } while (0)

#define preview_string(STRING)                                                 \
  do {                                                                         \
  } while (0)

#endif

#define defer_with(RESULT) \
    result = (RESULT); goto defer;\

// Read the file contents
char *read_file_contents(const char *filename);

void debug_token_value(const Value *value);
void debug_token(const Token *token);
void display_token(const Token *token);
void debug_lexer(const Lexer *lexer);
#endif // UTILS_H
