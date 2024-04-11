#include "../include/c_writer.h"
#include "../include/lexer.h"
#include "../include/parseTree.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  Token **tokens;
  /*size_t i;*/
  size_t num_tokens = 0;
  TreeNode *parse_tree;

  char *source_code = (char *)malloc(100 * sizeof(char));
  if (argc < 2)
  {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  printf("Reading File: %s\n", argv[1]);

  source_code = read_file(argv[1]);
  printf("Source code:\n %s\n", source_code);
  tokens = lex(source_code);

  if (tokens == NULL)
  {
    fprintf(stderr, "Error: Failed to tokenize the source code.\n");
    free(source_code);
    exit(EXIT_FAILURE);
  }

  while (tokens[num_tokens]->type != PYTOK_EOF)
  {
    num_tokens++;
  }
  parse_tree = buildParseTreeFromTokens(tokens, num_tokens);

  printf("Parse Tree:\n");
  printParseTree(parse_tree, 0);
  write_c_file(parse_tree);

  /*
  free(source_code);
  for (i = 0; i < num_tokens; i++) {
    free_token(tokens[i]);
  }
  free(tokens);
  */

  printf("Done parsing...\n");
  return 0;
}
