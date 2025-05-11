#ifndef UTILS_H
#define UTILS_H

#ifndef NDEBUG
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

// Read the file contents
char *read_file_contents(const char *filename);

#endif // UTILS_H
