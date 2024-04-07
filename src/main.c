#include "../include/lexer.h"
#include "../include/mapping.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/_types/_null.h>

int main(int argc, char *argv[]) {
  char *source_code = (char *)malloc(100 * sizeof(char));
  int parsedTokenCount = 0;
  int i = 0;
  CToken *cTokens;

  PythonToken **parsedTokens;
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  printf("Reading File: %s\n", argv[1]);

  source_code = read_file(argv[1]);
  printf("Source code:\n %s\n", source_code);

  parsedTokens = lex(source_code, &parsedTokenCount);
  cTokens = convert_to_c_token(*parsedTokens);

  for (i = 0; i < parsedTokenCount; i++) {
    printf("C Token: %s\n", cTokens[i].lexeme);
  }
  return 0;
}
