#include "ast.h"
#include "lexer.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage() { fprintf(stderr, "Usage: clox tokenize <filename>\n"); }

Lexer lex(const char *filepath) {
  char *file_contents = read_file_contents(filepath);
  if (file_contents == NULL) {
    fprintf(stderr, ERROR "couldn't read input file [%s]", filepath);
    exit(LEXER_EXIT_FAILURE);
  }

  ASSERT(strlen(file_contents) > 0,
         "File read output is less than or equal to.");

  Lexer lexer;
  if (strlen(file_contents) > 0) {
    lexer = init_lexer(filepath, file_contents);

    scan_tokens(&lexer);
  } else {
    fprintf(stderr, ERROR "file [%s] is empty", filepath);
    exit(LEXER_EXIT_FAILURE);
  }

  return lexer;
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
    Lexer lexer = lex(argv[2]);
    for (size_t i = 0; (int)i < arrlen(lexer.tokens); i++) {
      display_token(&lexer.tokens[i]);
    }
    if (lexer.had_error) {
      fprintf(stderr, ERROR ": lexer had errors [%s].\n", argv[2]);
      exit(LEXER_EXIT_FAILURE);
    }
    free_lexer(&lexer);
    exit(EXIT_SUCCESS);
  } else if (strcmp(command, "parse") == 0) {
    Lexer lexer = lex(argv[2]);
    Parser parser = parse(&lexer);
    expr_accept(parser.root, (ExprVisitor *)&AstPrinter);
    free_parser(&parser);
    printf("\n");
  } else {
    fprintf(stderr, ERROR ": Unknown command: %s\n", command);
    usage();
    return 64;
  }

  return EXIT_SUCCESS;
}
