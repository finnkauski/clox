#include "lexer.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage() {
    fprintf(stderr, "Usage: clox tokenize <filename>\n");
}

int main(int argc, char *argv[]) {
  // TODO: do we need this ?
  // Disable output buffering
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  if (argc < 3) {
    usage();
    return 1;
  }

  ASSERT(argc >= 3, "Less arguments than expected.");

  const char *command = argv[1];

  if (strcmp(command, "tokenize") == 0) {
    // Read file
    char *file_contents = read_file_contents(argv[2]);

    ASSERT(strlen(file_contents) > 0,
           "File read output is less than or equal to.");

    if (strlen(file_contents) > 0) {
      Lexer lexer = init_lexer(argv[2], file_contents);

      scan_tokens(&lexer);
      for (size_t i = 0; (int)i < arrlen(lexer.tokens); i++) {
        display_token(&lexer.tokens[i]);
      }
      if (lexer.had_error) {
        fprintf(stderr, "ERROR: \033[31mFAIL\033[0m\n");
        exit(LEXER_EXIT_FAILURE);
      }
      free_lexer(&lexer);
      exit(EXIT_SUCCESS);
    }
  } else {
    fprintf(stderr, "ERROR: Unknown command: %s\n", command);
    usage();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
