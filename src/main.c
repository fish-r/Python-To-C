#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  char *source_code = (char *)malloc(100 * sizeof(char));
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  printf("Reading File: %s\n", argv[1]);

  source_code = read_file(argv[1]);
  printf("Source code:\n %s\n", source_code);

  return 0;
}
