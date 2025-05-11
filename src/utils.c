#include <stdio.h>
#include <stdlib.h>


char *read_file_contents(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "Error reading file: %s\n", filename);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  if (file_size < 0) {
    fprintf(stderr, "Error getting file size: %s\n", filename);
    return NULL;
  }
  rewind(file);
  if (file_size < 0) {
    fprintf(stderr, "Error getting file size: %s\n", filename);
    return NULL;
  }

  char *file_contents = malloc(file_size + 1);
  if (file_contents == NULL) {
    fprintf(stderr, "Error memory allocation for file %s failed\n", filename);
    fclose(file);
    return NULL;
  }

  size_t bytes_read = fread(file_contents, 1, file_size, file);
  if (bytes_read < (size_t)file_size) {
    fprintf(stderr, "Error reading file contents\n");
    free(file_contents);
    fclose(file);
    return NULL;
  }

  file_contents[file_size] = '\0';
  fclose(file);

  return file_contents;
}
