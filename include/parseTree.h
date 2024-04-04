#ifndef PARSETREE_H
#define PARSETREE_H
#include <stddef.h>
#include "lexer.h"

typedef struct TreeNode {
    char *label;
    struct TreeNode *parent; 
    struct TreeNode **children;
    size_t num_children;
} TreeNode;


TreeNode *createNode(char *label);
void addChild(TreeNode *parent, TreeNode *child);
void printParseTree(TreeNode *root, int depth);
TreeNode *buildParseTree(Token *tokens, size_t num_tokens);
TreeNode *buildParseTreeFromTokens(Token **tokens, size_t num_tokens);

#endif