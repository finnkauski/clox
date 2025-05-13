#include "lexer.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  // TODO: do we need this ?
  // Disable output buffering
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  if (argc < 3) {
    fprintf(stderr, "Usage: clox tokenize <filename>\n");
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
      printf("Initialising lexer...");
      Lexer lexer = init_lexer(file_contents);
      printf("\033[32mOK\033[0m\n");

      printf("Scanning tokens...");
      LexerStatus status = scan_tokens(&lexer);
      if (status == LEXER_FAILURE) {
        printf("\n\033[31mFAIL\033[0m\n");
        exit(LEXER_EXIT_STATUS);
      }
      printf("\033[32mOK\033[0m\n");
      printf("Tokens: \n");
      for (size_t i = 0; (int)i < arrlen(lexer.tokens); i++) {
        display_token(&lexer.tokens[i]);
        // debug_token(&lexer.tokens[i]);
      }


      exit(0);
    }

    free(file_contents);
  } else {
    fprintf(stderr, "Unknown command: %s\n", command);
    return 1;
  }

  return 0;
}
