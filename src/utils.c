#include "../include/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_file(const char *filename) {
  char *path_prefix = "input/";
  char *content;
  long file_size;
  size_t read_size;
  FILE *file;
  char *pathname = (char *)malloc(sizeof(char) *
                                  (strlen(path_prefix) + strlen(filename) + 1));
  if (pathname == NULL) {
    fprintf(stderr, "Error allocating memory for pathname\n");
    exit(EXIT_FAILURE);
  }
  strcpy(pathname, path_prefix);

  strcat(pathname, filename);
  printf("Pathname: %s\n", pathname);

  file = fopen(pathname, "r");
  if (file == NULL) {
    fprintf(stderr, "Error opening file: %s\n", filename);
    exit(EXIT_FAILURE);
  }
  fseek(file, 0, SEEK_END);
  file_size = ftell(file);
  rewind(file);
  content = (char *)malloc((file_size + 1) * sizeof(char));
  if (content == NULL) {
    fprintf(stderr, "Error allocating memory for source code\n");
    exit(EXIT_FAILURE);
  }
  read_size = fread(content, sizeof(char), file_size, file);
  content[read_size] = '\0';
  fclose(file);
  return content;
}
