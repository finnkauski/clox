#ifndef UTILS_H
#define UTILS_H
#include "lexer.h"

#define DEBUG 0
#ifndef DEBUG
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
#else
#define ASSERT(cond, ...) ((void)0)
#endif

#define defer_with(RESULT) \
    result = (RESULT); goto defer;\

// Read the file contents
char *read_file_contents(const char *filename);

// Printing and displaying debug prints
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

void debug_token_value(const Value *value);
void debug_token(const Token *token);
void display_token(const Token *token);
void debug_lexer(const Lexer *lexer);
#endif // UTILS_H
