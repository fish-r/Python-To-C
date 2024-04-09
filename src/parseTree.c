#include "../include/parseTree.h"
#include "../include/lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TreeNode *createNode(char *label, Token *token)
{
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    node->label = strdup(label);
    node->parent = NULL;
    node->children = NULL;
    node->num_children = 0;
    node->token = token;
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
    if (root->token != NULL)
    {
        printf("%s -> %s\n", root->label, root->token->lexeme);
    }
    else
    {
        printf("%s -> %s\n", root->label, root->label);
    }
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

TreeNode *initMain(TreeNode *program)
{
    TreeNode *mainNode;
    /* add child to program node */
    addChild(program, createNode("FunctionDefinition", create_token(PYTOK_DEF, "main", 0, 0, NULL)));
    mainNode = program->children[program->num_children - 1];

    /* add identifier */
    addChild(mainNode, createNode("Identifier", create_token(PYTOK_IDENTIFIER, "main", 0, 0, NULL)));

    /* add return type */
    addChild(mainNode, createNode("Return", create_token(PYTOK_RETURN, "int", 0, 0, "int")));

    /* add block */
    addChild(mainNode, createNode("Block", NULL));
    mainNode = mainNode->children[mainNode->num_children - 1];

    return mainNode;
}

TreeNode *buildParseTreeFromTokens(Token **tokens, size_t num_tokens)
{
    TreeNode *program = createNode("Program", NULL);
    TreeNode *currentNode = program;
    TreeNode *mainNode = initMain(program);
    size_t index = 0;
    Token *currentToken = tokens[index];

    printf("\nBuild parse tree start\n");

    while (index < num_tokens)
    {
        switch (currentToken->type)
        {
        case PYTOK_FOR:
            index = parseForStatement(tokens, mainNode, index);
            currentToken = tokens[index];
            currentNode = currentNode->children[currentNode->num_children - 1];
            break;
        case PYTOK_DEF:
            index = parseFuncDef(tokens, currentNode, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            currentNode = program;
            break;

        case PYTOK_PRINT:
            index = parsePrint(tokens, mainNode, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            currentNode = program;
            break;
        case PYTOK_ELSE:
            index = parseElseStatement(tokens, mainNode, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            currentNode = currentNode->children[currentNode->num_children - 1];
            break;
        case PYTOK_IF:
            index = parseIfStatement(tokens, mainNode, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            currentNode = currentNode->children[currentNode->num_children - 1];
            break;
        case PYTOK_IDENTIFIER:
            index = parseExpression(tokens, mainNode, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            currentNode = currentNode->children[currentNode->num_children - 1];
            break;
        case PYTOK_WHILE:
            index = parseWhileStatement(tokens, mainNode, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            currentNode = currentNode->children[currentNode->num_children - 1];
            break;
        case PYTOK_EOF:
            index++;
            break;
        default:
            index++;
            currentToken = tokens[index];
            break;
        }
    }
    printf("Build parse tree end\n");
    return program;
}

size_t parseForStatement(Token **tokens, TreeNode *currentNode, size_t index)
{
    /* add child */
    addChild(currentNode, createNode("ForStatement", tokens[index]));
    currentNode = currentNode->children[currentNode->num_children - 1];
    index++;
    /* peek next token */
    if (peekToken(tokens[index]) == PYTOK_IDENTIFIER)
    {
        /* add identifier to child */
        addChild(currentNode, createNode("Identifier", tokens[index]));
        index++;

        /* skip in keyword */
        index++;

        /* add string literal to child */
        addChild(currentNode, createNode("StringLiteral", tokens[index]));
        index++;

        /* parse block */
        index = parseBlock(tokens, currentNode, index);
    }
    return index;
}

size_t parseFuncDef(Token **tokens, TreeNode *currentNode, size_t index)
{
    /* add child */
    addChild(currentNode, createNode("FunctionDefinition", tokens[index]));
    currentNode = currentNode->children[currentNode->num_children - 1];
    index++;

    /* add identifier as child */
    addChild(currentNode, createNode("Identifier", tokens[index]));
    index++;

    /* skip left paranthesis */
    index++;

    /* peek at next token -> if no params */
    if (peekToken(tokens[index]) == PYTOK_RIGHTPARENTHESIS)
    {
        /* skip right paran and parse block*/
        index = parseBlock(tokens, currentNode, index + 1);
    }
    /* have parameters */
    else
    {
        index = parseParamList(tokens, currentNode, index);
        /* skip right paran and parse block */
        index = parseBlock(tokens, currentNode, index + 1);
    }
    return index;
}

size_t parseParamList(Token **tokens, TreeNode *currentNode, size_t index)
{
    do
    {
        /* check if next token is comma */
        if (peekToken(tokens[index]) == PYTOK_COMMA)
        {
            /* skip comma token */
            index++;
        }
        /* add child */
        addChild(currentNode, createNode("Parameter", tokens[index]));
        index++;
    } while (peekToken(tokens[index]) == PYTOK_COMMA);
    return index;
}

size_t parsePrint(Token **tokens, TreeNode *currentNode, size_t index)
{
    /* add child */
    addChild(currentNode, createNode("PrintStatement", tokens[index]));
    currentNode = currentNode->children[currentNode->num_children - 1];
    index++;

    /* skip left paran */
    index++;

    /* add print value as child */
    switch (tokens[index]->type)
    {
    case PYTOK_STRING:
        addChild(currentNode, createNode("StringLiteral", tokens[index]));
        index++;
        break;
    case PYTOK_INT:
        addChild(currentNode, createNode("IntLiteral", tokens[index]));
        index++;
        break;
    case PYTOK_FLOAT:
        addChild(currentNode, createNode("FloatLiteral", tokens[index]));
        index++;
        break;
    case PYTOK_IDENTIFIER:
        addChild(currentNode, createNode("Identifier", tokens[index]));
        index++;
        break;

    default:
        break;
    }

    /* skip right paran */
    index++;

    return index;
}

size_t parseIfStatement(Token **tokens, TreeNode *currentNode, size_t index)
{
    /* add if statement as child */
    addChild(currentNode, createNode("IfStatement", tokens[index]));
    index++;
    currentNode = currentNode->children[currentNode->num_children - 1];

    /* parse condition */
    index = parseCondition(tokens, currentNode, index);

    /* parse block */
    index = parseBlock(tokens, currentNode, index);

    return index;
}

size_t parseCondition(Token **tokens, TreeNode *currentNode, size_t index)
{
    /* <term> <op> <term> */

    /* add condition as child */
    addChild(currentNode, createNode("Condition", NULL));
    currentNode = currentNode->children[currentNode->num_children - 1];

    /* add first term as child */
    addChild(currentNode, createNode("Term", tokens[index]));
    index++;

    /* add op as child */
    addChild(currentNode, createNode("Operator", tokens[index]));
    index++;

    /* add second term as child */
    addChild(currentNode, createNode("Term", tokens[index]));
    index++;

    return index;
}

size_t parseElseStatement(Token **tokens, TreeNode *currentNode, size_t index)
{
    /* add else statement as child */
    addChild(currentNode, createNode("ElseStatement", tokens[index]));
    currentNode = currentNode->children[currentNode->num_children - 1];
    index++;

    /* parse block */
    index = parseBlock(tokens, currentNode, index);

    return index;
}

size_t parseElifStatement(Token **tokens, TreeNode *currentNode, size_t index)
{
    /* add elif statement as child */
    addChild(currentNode, createNode("ElifStatement", tokens[index]));
    currentNode = currentNode->children[currentNode->num_children - 1];
    index++;

    /* parse condition */
    index = parseCondition(tokens, currentNode, index);

    /* parse block */
    index = parseBlock(tokens, currentNode, index);

    return index;
}

size_t parseReturnStatement(Token **tokens, TreeNode *currentNode, size_t index)
{
    /* add return statement as child */
    addChild(currentNode, createNode("ReturnStatement", tokens[index]));
    currentNode = currentNode->children[currentNode->num_children - 1];
    index++;

    addChild(currentNode, createNode("Identifier", tokens[index]));
    index++;

    return index;
}

size_t parseExpression(Token **tokens, TreeNode *currentNode, size_t index)
{
    /* identifier = <term> <operator> <term> */

    /* take initial line number */
    int lineNumber = tokens[index]->line_number;
    /* add expression as child */
    addChild(currentNode, createNode("Expression", NULL));
    currentNode = currentNode->children[currentNode->num_children - 1];

    /* add identifier as child */
    addChild(currentNode, createNode("Identifier", tokens[index]));
    index++;

    /* add operator as child */
    addChild(currentNode, createNode("Operator", tokens[index]));
    index++;

    /* check if still the same line number */
    while (lineNumber == tokens[index]->line_number)
    {
        switch (tokens[index]->type)
        {
        case PYTOK_IDENTIFIER:
            /* check if next token is () */
            /*if (peekToken(tokens[index]) == PYTOK_LEFTPARENTHESIS)
            {
                index = parseFunctionCall();
            }*/
            addChild(currentNode, createNode("Identifier", tokens[index]));
            index++;
            break;
        case PYTOK_STRING:
            addChild(currentNode, createNode("StringLiteral", tokens[index]));
            index++;
            break;
        case PYTOK_INT:
            addChild(currentNode, createNode("IntLiteral", tokens[index]));
            index++;
            break;
        case PYTOK_FLOAT:
            addChild(currentNode, createNode("FloatLiteral", tokens[index]));
            index++;
            break;
        default:
            index++;
            break;
        }
        /* add operator */
        if ((peekToken(tokens[index]) >= PYTOK_PLUS) && (peekToken(tokens[index]) <= PYTOK_UNEQUALITY))
        {
            addChild(currentNode, createNode("Operator", tokens[index]));
            index++;
        }
    }
    return index;
}

size_t parseWhileStatement(Token **tokens, TreeNode *currentNode, size_t index)
{
    /* add while as child */
    addChild(currentNode, createNode("WhileStatement", tokens[index]));
    currentNode = currentNode->children[currentNode->num_children - 1];
    index++;

    /* parse condition */
    index = parseCondition(tokens, currentNode, index);

    /* parse block */
    index = parseBlock(tokens, currentNode, index);

    return index;
}

size_t parseBlock(Token **tokens, TreeNode *currentNode, size_t index)
{
    /* Block -> : <statements> */
    int indent;

    /* skip colon keyword */
    index++;

    /* set current indent level */
    indent = tokens[index]->num_indentation;

    /* add block as child */
    addChild(currentNode, createNode("Block", NULL));
    currentNode = currentNode->children[currentNode->num_children - 1];
    while (tokens[index]->num_indentation == indent)
    /*while ((tokens[index]->type) != PYTOK_COLON)*/
    {
        /* peek next token */
        switch (peekToken(tokens[index]))
        {
        case PYTOK_FOR:
            index = parseForStatement(tokens, currentNode, index);
            break;
        case PYTOK_IF:
            index = parseIfStatement(tokens, currentNode, index);
            break;
        case PYTOK_WHILE:
            index = parseWhileStatement(tokens, currentNode, index);
            break;
        case PYTOK_ELSE:
            index = parseElseStatement(tokens, currentNode, index);
            break;
        case PYTOK_ELIF:
            index = parseElifStatement(tokens, currentNode, index);
            break;
        case PYTOK_RETURN:
            index = parseReturnStatement(tokens, currentNode, index);
            break;
        case PYTOK_PRINT:
            index = parsePrint(tokens, currentNode, index);
            break;
        case PYTOK_IDENTIFIER:
            index = parseExpression(tokens, currentNode, index);
            break;
        default:
            break;
        }
    }
    return index;
}
