#include "../include/utils.h"

#include <stdarg.h>
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

void throwError(const char *format, ...) {
  va_list args;
  va_start(args, format);

  fprintf(stderr, "Error : ");
  vfprintf(stderr, format, args);
  fprintf(stderr, "\n");

  va_end(args);
  exit(EXIT_FAILURE);
}

void checkAndThrowError(const char *matched_lexeme,
                        const char **unImplementedTokenList, int numTokens,
                        const char *tokenType, int current_line_number) {
  int i;
  for (i = 0; i < numTokens; i++) {
    if (strcmp(matched_lexeme, unImplementedTokenList[i]) == 0) {
      throwError("Unimplemented %s '%s' at line %d\n", tokenType,
                 matched_lexeme, current_line_number);
      return;
    }
  }
}

int is_digit(char c) { return c >= '0' && c <= '9'; }

int is_whitespace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

NodeQueue *createNodeQueue(int size) {
  NodeQueue *queue = (NodeQueue *)malloc(sizeof(NodeQueue));
  if (queue == NULL) {
    fprintf(stderr, "Error allocating memory for NodeQueue\n");
    exit(EXIT_FAILURE);
  }
  queue->nodes = (TreeNode **)malloc(size * sizeof(TreeNode *));
  if (queue->nodes == NULL) {
    fprintf(stderr, "Error allocating memory for NodeQueue nodes\n");
    exit(EXIT_FAILURE);
  }
  queue->front = 0;
  queue->rear = -1;
  queue->size = size;
  return queue;
}

void enqueueNode(NodeQueue *queue, TreeNode *node) {
  if (queue == NULL) {
    fprintf(stderr, "Error: NodeQueue is NULL\n");
    exit(EXIT_FAILURE);
  }
  if (queue->rear == queue->size - 1) {
    fprintf(stderr, "Error: NodeQueue is full\n");
    exit(EXIT_FAILURE);
  }
  queue->rear++;
  queue->nodes[queue->rear] = node;
}

TreeNode *dequeueNode(NodeQueue *queue) {
  TreeNode *node;
  if (queue == NULL) {
    fprintf(stderr, "Error: NodeQueue is NULL\n");
    exit(EXIT_FAILURE);
  }
  if (queue->front > queue->rear) {
    fprintf(stderr, "Error: NodeQueue is empty\n");
    exit(EXIT_FAILURE);
  }
  node = queue->nodes[queue->front];
  queue->front++;
  return node;
}

TreeNode *peekNode(NodeQueue *queue) {
  if (queue == NULL) {
    fprintf(stderr, "Error: NodeQueue is NULL\n");
    exit(EXIT_FAILURE);
  }
  if (queue->front > queue->rear) {
    fprintf(stderr, "Error: NodeQueue is empty\n");
    exit(EXIT_FAILURE);
  }
  return queue->nodes[queue->front];
}

int isNodeQueueEmpty(NodeQueue *queue) {
  if (queue == NULL) {
    fprintf(stderr, "Error: NodeQueue is NULL\n");
    exit(EXIT_FAILURE);
  }
  return queue->front > queue->rear;
}

void freeNodeQueue(NodeQueue *queue) {
  if (queue == NULL) {
    fprintf(stderr, "Error: NodeQueue is NULL\n");
    exit(EXIT_FAILURE);
  }
  free(queue->nodes);
  free(queue);
}
