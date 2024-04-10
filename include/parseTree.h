#ifndef PARSETREE_H
#define PARSETREE_H
#include <stddef.h>
#include "lexer.h"

typedef struct TreeNode
{
    char *label;
    struct TreeNode *parent;
    struct TreeNode **children;
    size_t num_children;
    Token *token;
} TreeNode;

TreeNode *createNode(char *label, Token *token);
void addChild(TreeNode *parent, TreeNode *child);
void printParseTree(TreeNode *root, int depth);
TreeNode *buildParseTree(Token *tokens, size_t num_tokens);
TreeNode *buildParseTreeFromTokens(Token **tokens, size_t num_tokens);
char *findReturnType(Token **tokens, size_t index);

PythonTokenType peekToken(Token *token);
size_t parseForStatement(Token **tokens, TreeNode *currentNode, size_t index);
size_t parseFuncDef(Token **tokens, TreeNode *currentNode, size_t index);
size_t parsePrint(Token **tokens, TreeNode *currentNode, size_t index);
size_t parseParamList(Token **tokens, TreeNode *currentNode, size_t index);
size_t parseBlock(Token **tokens, TreeNode *currentNode, size_t index);
size_t parseIfStatement(Token **tokens, TreeNode *currentNode, size_t index);
size_t parseCondition(Token **tokens, TreeNode *currentNode, size_t index);
size_t parseElseStatement(Token **tokens, TreeNode *currentNode, size_t index);
size_t parseReturnStatement(Token **tokens, TreeNode *currentNode, size_t index);
size_t parseExpression(Token **tokens, TreeNode *currentNode, size_t index);
size_t parseWhileStatement(Token **tokens, TreeNode *currentNode, size_t index);
size_t parseFunctionCall(Token **tokens, TreeNode *currentNode, size_t index);
size_t parseElifStatement(Token **tokens, TreeNode *currentNode, size_t index);


#endif
