#ifndef UTILS_H
# define UTILS_H

#include "parseTree.h"
typedef struct {
    TreeNode **nodes;
    int front;
    int rear;
    int size;
} NodeQueue;

NodeQueue *createNodeQueue(int size);
void enqueueNode(NodeQueue *queue, TreeNode *node);
TreeNode *dequeueNode(NodeQueue *queue);
TreeNode *peekNode(NodeQueue *queue);
int isNodeQueueEmpty(NodeQueue *queue);
void freeNodeQueue(NodeQueue *queue);

int is_digit(char c);
int is_whitespace(char c);
char *read_file(const char *filename);
void throwError(const char *format, ...);
void checkAndThrowError(const char *matched_lexeme, const char **unImplementedTokenList, 
                        int numTokens, const char *tokenType, int current_line_number);

#endif /* UTILS_H */
