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

char *findReturnType(Token **tokens, size_t index)
{
    while(tokens[index]->type != PYTOK_RETURN)
    {
        index++;
    }
    return tokens[index+1]->c_type;
}

TreeNode *buildParseTreeFromTokens(Token **tokens, size_t num_tokens)
{
    TreeNode *program = createNode("Program", NULL);
    TreeNode *currentNode = program;
    size_t index = 0;
    Token *currentToken = tokens[index];

    printf("\nBuild parse tree start\n");

    while (index < num_tokens)
    {
        switch (currentToken->type)
        {
        case PYTOK_FOR:
            index = parseForStatement(tokens, currentNode, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            currentNode = program;
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
            index = parsePrint(tokens, currentNode, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            currentNode = program;
            break;
        case PYTOK_ELSE:
            index = parseElseStatement(tokens, currentNode, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            currentNode = currentNode->children[currentNode->num_children - 1];
            break;
        case PYTOK_IF:
            index = parseIfStatement(tokens, currentNode, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            currentNode = currentNode->children[currentNode->num_children - 1];
            break;
        case PYTOK_IDENTIFIER:
            /* check if there is () after identifier */
            if (peekToken(tokens[index + 1]) == PYTOK_LEFTPARENTHESIS)
            {
                index = parseFunctionCall(tokens, currentNode, index);
            }
            else
            {
                index = parseExpression(tokens, currentNode, index);
            }
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            break;
        case PYTOK_WHILE:
            index = parseWhileStatement(tokens, currentNode, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            currentNode = currentNode->children[currentNode->num_children - 1];
            break;
        case PYTOK_EOF:
            index++;
            break;
        case PYTOK_EOL:
            index++;
            currentToken = tokens[index];
            break;
        default:
            /* skip current token and move to next token */
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
    TreeNode *forNode;

    /* add for as child */
    addChild(currentNode, createNode("ForStatement", tokens[index]));
    currentNode = currentNode->children[currentNode->num_children - 1];
    index++;

    /* set for node as current node */
    forNode = currentNode;

    /* add identifier as child */
    addChild(currentNode, createNode("Identifier", tokens[index]));
    index++;

    /* add iterable as child */
    addChild(currentNode, createNode("Iterable", NULL));
    currentNode = currentNode->children[currentNode->num_children - 1];

    /* skip in keyword */
    index++;

    /* peek next token */
    switch (peekToken(tokens[index]))
    {
    case PYTOK_RANGE:
        /* add range as child */
        addChild(currentNode, createNode("Range", NULL));
        currentNode = currentNode->children[currentNode->num_children - 1];
        index++;
        /* skip left paran */
        index++;

        do
        {
            /* check if next token is comma */
            if (peekToken(tokens[index]) == PYTOK_COMMA)
            {
                /* skip comma token */
                index++;
            }
            /* add term as child */
            addChild(currentNode, createNode("Term", tokens[index]));
            index++;
        } while (peekToken(tokens[index]) == PYTOK_COMMA);

        /* skip right paran and parse block */
        index = parseBlock(tokens, forNode, index+1);
        break;
    case PYTOK_STRING:
        /* add string as child */
        addChild(currentNode, createNode("String", NULL));
        index++;
        currentNode = currentNode->children[currentNode->num_children - 1];

        /* parse block */
        index = parseBlock(tokens, forNode, index);
        break;
    case PYTOK_CHAR:
        /* add string as child */
        addChild(currentNode, createNode("Char", NULL));
        index++;
        currentNode = currentNode->children[currentNode->num_children - 1];

        /* parse block */
        index = parseBlock(tokens, forNode, index);
        break;
    case PYTOK_IDENTIFIER:
        /* add string as child */
        addChild(currentNode, createNode("Identifier", tokens[index]));
        index++;
        currentNode = currentNode->children[currentNode->num_children - 1];

        /* parse block */
        index = parseBlock(tokens, forNode, index);
        break;
    case PYTOK_LIST_FLOAT:
        break;
    case PYTOK_LIST_INT:
        break;
    case PYTOK_LIST_STR:
        break;
    default:
        break;
    }
    return index;
}

size_t parseFuncDef(Token **tokens, TreeNode *currentNode, size_t index)
{
    /* find return type */
    char* returnType = findReturnType(tokens, index);

    /* add child */
    addChild(currentNode, createNode("FunctionDefinition", tokens[index]));
    currentNode = currentNode->children[currentNode->num_children - 1];
    index++;

    /* set identifier c_type to return type */
    tokens[index]->c_type = strdup(returnType);
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
    case PYTOK_CHAR:
        addChild(currentNode, createNode("Literal", tokens[index]));
        index++;
        break;
    case PYTOK_STRING:
        addChild(currentNode, createNode("Literal", tokens[index]));
        index++;
        break;
    case PYTOK_INT:
        addChild(currentNode, createNode("Literal", tokens[index]));
        index++;
        break;
    case PYTOK_FLOAT:
        addChild(currentNode, createNode("Literal", tokens[index]));
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

    /* check if next token is eol, add child and remove if yes */
    if (peekToken(tokens[index]) == PYTOK_EOL)
    {
        addChild(currentNode, createNode("EOL", NULL));
        index++;
    }

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

    /* check next token */
    switch (peekToken(tokens[index]))
    {
    case PYTOK_IDENTIFIER:
        addChild(currentNode, createNode("Identifier", tokens[index]));
        index++;
        break;
    case PYTOK_INT:
        addChild(currentNode, createNode("Literal", tokens[index]));
        index++;
        break;
    case PYTOK_FLOAT:
        addChild(currentNode, createNode("Literal", tokens[index]));
        index++;
        break;
    case PYTOK_STRING:
        addChild(currentNode, createNode("Literal", tokens[index]));
        index++;
        break;
    case PYTOK_CHAR:
        addChild(currentNode, createNode("Literal", tokens[index]));
        index++;
        break;
    default:
        break;
    }
    /* check if next token is eol, add child and remove if yes */
    if (peekToken(tokens[index]) == PYTOK_EOL)
    {
        addChild(currentNode, createNode("EOL", NULL));
        index++;
    }

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

    /* check and assign c_type for identifier */
    if (peekToken(tokens[index + 1]) == PYTOK_ASSIGNMENT)
    {
        switch (peekToken(tokens[index + 2]))
        {
        case PYTOK_CHAR:
            tokens[index]->c_type = "char";
            break;
        case PYTOK_STRING:
            tokens[index]->c_type = "str";
            break;
        case PYTOK_INT:
            tokens[index]->c_type = "int";
            break;
        case PYTOK_FLOAT:
            tokens[index]->c_type = "float";
            break;
        case PYTOK_LIST_FLOAT:
            tokens[index]->c_type = "arr_float";
            break;
        case PYTOK_LIST_INT:
            tokens[index]->c_type = "arr_int";
            break;
        case PYTOK_LIST_STR:
            tokens[index]->c_type = "arr_str";
            break;
        case PYTOK_BOOLEAN:
            tokens[index]->c_type = "bool";
            break;
        default:
            break;
        }
    }

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
            if (peekToken(tokens[index + 1]) == PYTOK_LEFTPARENTHESIS)
            {
                index = parseFunctionCall(tokens, currentNode, index);
            }
            else
            {
                addChild(currentNode, createNode("Identifier", tokens[index]));
                index++;
            }
            break;
        case PYTOK_STRING:
            addChild(currentNode, createNode("Literal", tokens[index]));
            index++;
            break;
        case PYTOK_INT:
            addChild(currentNode, createNode("Literal", tokens[index]));
            index++;
            break;
        case PYTOK_FLOAT:
            addChild(currentNode, createNode("Literal", tokens[index]));
            index++;
            break;
        case PYTOK_CHAR:
            addChild(currentNode, createNode("Literal", tokens[index]));
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
        /* check if next token is eol, add child and remove if yes */
        if (peekToken(tokens[index]) == PYTOK_EOL)
        {
            addChild(currentNode, createNode("EOL", NULL));
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

size_t parseFunctionCall(Token **tokens, TreeNode *currentNode, size_t index)
{
    /* add function call as child */
    addChild(currentNode, createNode("FunctionCall", NULL));
    currentNode = currentNode->children[currentNode->num_children - 1];

    /* add identifier as child */
    addChild(currentNode, createNode("Identifier", tokens[index]));
    index++;

    /* skip left param */
    index++;

    /* parse params */
    index = parseParamList(tokens, currentNode, index);

    /* skip right param */
    index++;

    /* check if next token is eol, add child and remove if yes */
    if (peekToken(tokens[index]) == PYTOK_EOL)
    {
        addChild(currentNode, createNode("EOL", NULL));
        index++;
    }

    return index;
}

size_t parseBlock(Token **tokens, TreeNode *currentNode, size_t index)
{
    /* Block -> : <statements> */
    int indent;

    /* skip colon keyword */
    index++;
    /* add eol as child */
    addChild(currentNode, createNode("EOL", NULL));
    index++;

    /* set current indent level */
    indent = tokens[index]->num_indentation;

    /* add block as child */
    addChild(currentNode, createNode("Block", NULL));
    currentNode = currentNode->children[currentNode->num_children - 1];
    while (tokens[index]->num_indentation == indent)
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
            /* check if there is () after identifier */
            if (peekToken(tokens[index + 1]) == PYTOK_LEFTPARENTHESIS)
            {
                index = parseFunctionCall(tokens, currentNode, index);
            }
            else
            {
                index = parseExpression(tokens, currentNode, index);
            }
            break;
        default:
            break;
        }
    }
    return index;
}
