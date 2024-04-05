#include "../include/parseTree.h"
#include "../include/lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TreeNode *createNode(char *label, char *lexeme)
{
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    node->label = strdup(label);
    node->parent = NULL;
    node->children = NULL;
    node->num_children = 0;
    node->lexeme = strdup(lexeme);
    return node;
}

void addChild(TreeNode *parent, TreeNode *child)
{
    parent->children = (TreeNode **)realloc(parent->children, (parent->num_children + 1) * sizeof(TreeNode *));
    parent->children[parent->num_children] = child;
    parent->num_children++;
    child->parent = parent;
}

void printParseTree(TreeNode *root, int depth)
{
    size_t i;
    for (i = 0; i < depth; i++)
    {
        printf("  ");
    }
    printf("%s\n", root->label);
    for (i = 0; i < root->num_children; i++)
    {
        printParseTree(root->children[i], depth + 1);
    }
}

PythonTokenType peekToken(Token *token)
{
    if (token != NULL)
    {
        return token->type;
    }
    else
    {
        return UNKNOWN;
    }
}

TreeNode *buildParseTreeFromTokens(Token **tokens, size_t num_tokens)
{
    TreeNode *program = createNode("Program", "Program");
    TreeNode *current_node = program;
    TreeNode *base_node = NULL;
    size_t i = 0;
    printf("\nbuildparsetree start\n");
    while (i < num_tokens)
    {
        Token *current_token = tokens[i];
        PythonTokenType nextTokenType = peekToken(tokens[i + 1]);
        printf("%d\n", current_token->type);
        switch (current_token->type)
        {
        case PYTOK_DEF:
            addChild(current_node, createNode("FunctionDefinition", current_token->lexeme));
            base_node = current_node;
            current_node = current_node->children[current_node->num_children - 1];
            break;
        case PYTOK_LEFTPARENTHESIS:
            addChild(current_node, createNode("ParameterList", current_token->lexeme));
            current_node = current_node->children[current_node->num_children - 1];
            break;
        case PYTOK_IDENTIFIER:
            addChild(current_node, createNode("Identifier", current_token->lexeme));
            current_node = current_node->children[current_node->num_children - 1];
            break;
        case PYTOK_RIGHTPARENTHESIS:
            if (nextTokenType == PYTOK_DEF)
            {
                current_node = base_node;
            }
            else
            {
                current_node = current_node->parent;
            }
            break;
        case PYTOK_COLON:
            current_node = current_node->parent;
            break;
        case PYTOK_PRINT:
            addChild(current_node, createNode("PrintStatement", current_token->lexeme));
            current_node = current_node->children[current_node->num_children - 1];
            break;
        case PYTOK_STRING:
            addChild(current_node, createNode("StringLiteral", current_token->lexeme));
            current_node = current_node->children[current_node->num_children - 1];
            break;
        case PYTOK_FOR:
            addChild(current_node, createNode("ForStatement", current_token->lexeme));
            current_node = current_node->children[current_node->num_children - 1];
            break;
        case PYTOK_IN:
            addChild(current_node, createNode("InKeyword", current_token->lexeme));
            current_node = current_node->children[current_node->num_children - 1];
            break;
        default:
            break;
        }
        i++;
    }

    printf("\nbuildparsetree end\n");
    return program;
}
