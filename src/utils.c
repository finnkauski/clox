#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

char *read_file_contents(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, ERROR": reading file: %s\n", filename);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  if (file_size < 0) {
    fprintf(stderr, ERROR": getting file size: %s\n", filename);
    return NULL;
  }
  rewind(file);
  if (file_size < 0) {
    fprintf(stderr, ERROR": getting file size: %s\n", filename);
    return NULL;
  }

  char *file_contents = malloc(file_size + 1);
  if (file_contents == NULL) {
    fprintf(stderr, ERROR": memory allocation for file %s failed\n", filename);
    fclose(file);
    return NULL;
  }

  size_t bytes_read = fread(file_contents, 1, file_size, file);
  if (bytes_read < (size_t)file_size) {
    fprintf(stderr, ERROR": reading file contents\n");
    free(file_contents);
    fclose(file);
    return NULL;
  }

  file_contents[file_size] = '\0';
  fclose(file);

  return file_contents;
}

void debug_token_value(const Value *value) {
  switch (value->type) {
  case TYPE_NULL:
    fprintf(stderr, "None\n");
    break;
  case TYPE_IDENTIFIER:
    fprintf(stderr, "Identifier [");
    for (size_t i = 0; i < value->as.string_value.length; i++) {
      fprintf(stderr, "%c", value->as.string_value.start[i]);
    }
    fprintf(stderr, "]\n");
    break;
  case TYPE_KEYWORD:
    break;
  case TYPE_STRING:
    fprintf(stderr, "String \"");
    for (size_t i = 0; i < value->as.string_value.length; i++) {
      fprintf(stderr, "%c", value->as.string_value.start[i]);
    }
    fprintf(stderr, "\"\n");
    break;
  case TYPE_NUMBER:
    fprintf(stderr, "%f\n", value->as.number_value);
    break;
  default:
    fprintf(stderr, "UNKNOWN");
  }
}

// Printing and displaying
void debug_token(const Token *token) {
  fprintf(stderr, "Token {\n");
  fprintf(stderr, " type: %s\n", TOKEN_NAMES[token->type]);
  fprintf(stderr,
          " line: %zu\n"
          " start: %p\n"
          " value: ",
          token->line, (void *)token->start);
  debug_token_value(&token->value);
  fprintf(stderr, "}\n");
}

void display_token(const Token *token) {
  printf("%s", TOKEN_NAMES[token->type]);
  switch (token->value.type) {
  case TYPE_NULL:
    break;
  case TYPE_IDENTIFIER:
    printf(" [");
    for (size_t i = 0; i < token->value.as.string_value.length; i++) {
      printf("%c", token->value.as.string_value.start[i]);
    }
    printf("]");
    break;
  case TYPE_KEYWORD:
    break;
  case TYPE_STRING:
    printf(" \"");
    for (size_t i = 0; i < token->value.as.string_value.length; i++) {
      printf("%c", token->value.as.string_value.start[i]);
    }
    printf("\"");
    break;
  case TYPE_NUMBER:
    printf(" %f", token->value.as.number_value);
    break;
  }
  printf("\n");
}

void debug_lexer(const Lexer *lexer) {
  ASSERT(lexer->source != NULL, "Lexer source is NULL during display");
  ASSERT(lexer->current != NULL, "Lexer current is NULL during display");

  fprintf(stderr,
          "Lexer {\n"
          " current: `%c`,\n",
          *lexer->current);

  // truncate the source code
  if (lexer->source_len > 10)
    fprintf(stderr, " source: `%.10s...`,\n", lexer->source);
  else
    fprintf(stderr, " source: `%s`,\n", lexer->source);
  fprintf(stderr, " source_len: %zu,\n", lexer->source_len);

  // print token vector
  fprintf(stderr, " tokens: [...]\n");
  fprintf(stderr,
          " line: %zu,\n"
          " line_offset: %zu,\n"
          "}\n",
          lexer->line, lexer->line_offset);
}
