#include "../include/parseTree.h"
#include "../include/lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 1000
Token *tokenArray[MAX_TOKENS]; 
size_t tokenCount = 0;

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
    char *returnType = NULL;
    int i;
    while (tokens[index]->type != PYTOK_RETURN && tokens[index]->type != PYTOK_EOF)
    {
        /* check if current token is EOF */
        if (tokens[index]->type == PYTOK_EOF)
        {
            returnType = "void";
            break;
        }
        if (tokens[index]->type == PYTOK_IDENTIFIER && peekToken(tokens[index + 1]) == PYTOK_ASSIGNMENT)
        {
            switch (peekToken(tokens[index + 2]))
            {
                case PYTOK_CHAR:
                    tokens[index]->c_type = "char";
                    break;
                case PYTOK_STRING:
                    tokens[index]->c_type = "char []";
                    break;
                case PYTOK_INT:
                    tokens[index]->c_type = "int";
                    break;
                case PYTOK_FLOAT:
                    tokens[index]->c_type = "float";
                    break;
                case PYTOK_LIST_FLOAT:
                    tokens[index]->c_type = "float []";
                    break;
                case PYTOK_LIST_INT:
                    tokens[index]->c_type = "int []";
                    break;
                case PYTOK_LIST_STR:
                    tokens[index]->c_type = "char* []";
                    break;
                case PYTOK_BOOLEAN:
                    tokens[index]->c_type = "int";
                    break;
                default:
                    break;
            }
            tokenArray[tokenCount] = tokens[index];
            tokenCount++;
        }
        index++;
    }
    for (i = 0; i < tokenCount; i++) {
        printf("[%d] Type: %s, Lexeme: %s\n", i, tokenArray[i]->c_type, tokenArray[i]->lexeme);
    }
    for (i = 0; i < tokenCount; i++)
    {
        if (strcmp(tokens[index + 1]->lexeme, tokenArray[i]->lexeme) == 0)
        {
            return tokenArray[i]->c_type;
        }
    }
    return returnType;
}

int findNumParams(Token **tokens, size_t index)
{
    int count = 0;
    while (tokens[index]->type != PYTOK_RIGHTPARENTHESIS)
    {
        if (tokens[index]->type == PYTOK_INT)
        {
            count++;
        }
        index++;
    }
    return count;
}

TreeNode *buildParseTreeFromTokens(Token **tokens, size_t num_tokens)
{
    TreeNode *program = createNode("Program", NULL);
    TreeNode *currentNode = program;
    size_t index = 0;
    Token *currentToken = tokens[index];
    int mainFuncBool = 0;
    TreeNode *mainFunc;

    printf("\nBuild parse tree start\n");

    /*printf("num token: %ld\n", num_tokens);*/

    while (index < num_tokens)
    {
        switch (currentToken->type)
        {
        case PYTOK_FOR:
            /* check if main function node is created, if not created */
            if (mainFuncBool == 0){
                addChild(currentNode, createNode("FunctionDefinition", create_token(PYTOK_DEF, "main", 0, 0, "main", 0)));
                mainFunc = currentNode->children[currentNode->num_children - 1];
                addChild(mainFunc, createNode("Identifier", create_token(PYTOK_IDENTIFIER, "main", 0, 0, "main", 0)));
                addChild(mainFunc, createNode("EOL", create_token(PYTOK_EOL, "EOL", 0, 0, "EOL", 0)));
                mainFuncBool = 1;
            }
            index = parseForStatement(tokens, mainFunc, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            break;
        case PYTOK_DEF:
            index = parseFuncDef(tokens, currentNode, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            break;

        case PYTOK_PRINT:
            /* check if main function node is created, if not created */
            if (mainFuncBool == 0){
                addChild(currentNode, createNode("FunctionDefinition", create_token(PYTOK_DEF, "main", 0, 0, "main", 0)));
                mainFunc = currentNode->children[currentNode->num_children - 1];
                addChild(mainFunc, createNode("Identifier", create_token(PYTOK_IDENTIFIER, "main", 0, 0, "main", 0)));
                addChild(mainFunc, createNode("EOL", create_token(PYTOK_EOL, "EOL", 0, 0, "EOL", 0)));
                mainFuncBool = 1;
            }
            index = parsePrint(tokens, mainFunc, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            break;
        case PYTOK_ELSE:
            if (mainFuncBool == 0){
                addChild(currentNode, createNode("FunctionDefinition", create_token(PYTOK_DEF, "main", 0, 0, "main", 0)));
                mainFunc = currentNode->children[currentNode->num_children - 1];
                addChild(mainFunc, createNode("Identifier", create_token(PYTOK_IDENTIFIER, "main", 0, 0, "main", 0)));
                addChild(mainFunc, createNode("EOL", create_token(PYTOK_EOL, "EOL", 0, 0, "EOL", 0)));
                mainFuncBool = 1;
            }
            index = parseElseStatement(tokens, mainFunc, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            break;
        case PYTOK_ELIF:
            if (mainFuncBool == 0){
                addChild(currentNode, createNode("FunctionDefinition", create_token(PYTOK_DEF, "main", 0, 0, "main", 0)));
                mainFunc = currentNode->children[currentNode->num_children - 1];
                addChild(mainFunc, createNode("Identifier", create_token(PYTOK_IDENTIFIER, "main", 0, 0, "main", 0)));
                addChild(mainFunc, createNode("EOL", create_token(PYTOK_EOL, "EOL", 0, 0, "EOL", 0)));
                mainFuncBool = 1;
            }
            index = parseElifStatement(tokens, mainFunc, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            break;
        case PYTOK_IF:
            if (mainFuncBool == 0){
                addChild(currentNode, createNode("FunctionDefinition", create_token(PYTOK_DEF, "main", 0, 0, "main", 0)));
                mainFunc = currentNode->children[currentNode->num_children - 1];
                addChild(mainFunc, createNode("Identifier", create_token(PYTOK_IDENTIFIER, "main", 0, 0, "main", 0)));
                addChild(mainFunc, createNode("EOL", create_token(PYTOK_EOL, "EOL", 0, 0, "EOL", 0)));
                mainFuncBool = 1;
            }
            index = parseIfStatement(tokens, mainFunc, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            break;
        case PYTOK_IDENTIFIER:
            if (mainFuncBool == 0){
                addChild(currentNode, createNode("FunctionDefinition", create_token(PYTOK_DEF, "main", 0, 0, "main", 0)));
                mainFunc = currentNode->children[currentNode->num_children - 1];
                addChild(mainFunc, createNode("Identifier", create_token(PYTOK_IDENTIFIER, "main", 0, 0, "main", 0)));
                addChild(mainFunc, createNode("EOL", create_token(PYTOK_EOL, "EOL", 0, 0, "EOL", 0)));
                mainFuncBool = 1;
            }
            /* check if there is () after identifier */
            if (peekToken(tokens[index + 1]) == PYTOK_LEFTPARENTHESIS)
            {
                index = parseFunctionCall(tokens, mainFunc, index);
            }
            else
            {
                index = parseExpression(tokens, mainFunc, index);
            }
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            break;
        case PYTOK_WHILE:
            if (mainFuncBool == 0){
                addChild(currentNode, createNode("FunctionDefinition", create_token(PYTOK_DEF, "main", 0, 0, "main", 0)));
                mainFunc = currentNode->children[currentNode->num_children - 1];
                addChild(mainFunc, createNode("Identifier", create_token(PYTOK_IDENTIFIER, "main", 0, 0, "main", 0)));
                addChild(mainFunc, createNode("EOL", create_token(PYTOK_EOL, "EOL", 0, 0, "EOL", 0)));
                mainFuncBool = 1;
            }
            index = parseWhileStatement(tokens, mainFunc, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            break;
        case PYTOK_EOF:
            index++;
            if (index < num_tokens)
            {
                /*printf("index: %ld\n", index);*/
                currentToken = tokens[index];
            }
            break;
        case PYTOK_COMMENT:
            if (mainFuncBool == 0){
                addChild(currentNode, createNode("FunctionDefinition", create_token(PYTOK_DEF, "main", 0, 0, "main", 0)));
                mainFunc = currentNode->children[currentNode->num_children - 1];
                addChild(mainFunc, createNode("Identifier", create_token(PYTOK_IDENTIFIER, "main", 0, 0, "main", 0)));
                addChild(mainFunc, createNode("EOL", create_token(PYTOK_EOL, "EOL", 0, 0, "EOL", 0)));
                mainFuncBool = 1;
            }
            index = parseComments(tokens, mainFunc, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            break;
        case PYTOK_MULTI_COMMENT:
            if (mainFuncBool == 0){
                addChild(currentNode, createNode("FunctionDefinition", create_token(PYTOK_DEF, "main", 0, 0, "main", 0)));
                mainFunc = currentNode->children[currentNode->num_children - 1];
                addChild(mainFunc, createNode("Identifier", create_token(PYTOK_IDENTIFIER, "main", 0, 0, "main", 0)));
                addChild(mainFunc, createNode("EOL", create_token(PYTOK_EOL, "EOL", 0, 0, "EOL", 0)));
                mainFuncBool = 1;
            }
            index = parseComments(tokens, mainFunc, index);
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            break;
        case PYTOK_EOL:
            index++;
            currentToken = tokens[index];
            break;
        default:
            /* skip current token and move to next token */
            index++;
            if (index < num_tokens)
            {
                currentToken = tokens[index];
            }
            break;
        }
    }
    printf("Build parse tree end\n");
    return program;
}

size_t parseComments(Token **tokens, TreeNode *currentNode, size_t index)
{
    /* add comment as child */
    addChild(currentNode, createNode("Comment", tokens[index]));
    index++;

    /* check for EOL */
    if (tokens[index]->type == PYTOK_EOL)
    {
        addChild(currentNode, createNode("EOL", NULL));
        index++;
        /* remove all subsequent eol */
        while (tokens[index]->type == PYTOK_EOL)
        {
            index++;
        }
    }

    return index;
}

size_t parseForStatement(Token **tokens, TreeNode *currentNode, size_t index)
{
    TreeNode *forNode;
    int numParams = 0;

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

        numParams = findNumParams(tokens, index);
        printf("num param: %d\n", numParams);

        /* check for number of params */
        if (numParams == 1)
        {
            addChild(currentNode, createNode("Start", create_token(PYTOK_INT, "0", 0, 0, "int", 0)));

            addChild(currentNode, createNode("Stop", tokens[index]));
            index++;

            addChild(currentNode, createNode("Step", create_token(PYTOK_INT, "1", 0, 0, "int", 0)));
        }
        else if (numParams == 2)
        {
            addChild(currentNode, createNode("Start", tokens[index]));
            index++;

            /* skip comma */
            index++;

            addChild(currentNode, createNode("Stop", tokens[index]));
            index++;

            addChild(currentNode, createNode("Step", create_token(PYTOK_INT, "1", 0, 0, "int", 0)));
        }
        else
        {
            /* num params = 3 */
            addChild(currentNode, createNode("Start", tokens[index]));
            index++;

            /* skip comma */
            index++;

            addChild(currentNode, createNode("Stop", tokens[index]));
            index++;

            /* skip comma */
            index++;

            addChild(currentNode, createNode("Step", tokens[index]));
            index++;
        }

        index = parseBlock(tokens, forNode, index + 1);
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
        addChild(currentNode, createNode("Char", tokens[index]));
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
        /* add list float as child */
        addChild(currentNode, createNode("List", tokens[index]));
        index++;
        currentNode = currentNode->children[currentNode->num_children - 1];

        /* parse block */
        index = parseBlock(tokens, forNode, index);
        break;
    case PYTOK_LIST_INT:
        /* add list as child */
        addChild(currentNode, createNode("List", tokens[index]));
        index++;
        currentNode = currentNode->children[currentNode->num_children - 1];

        /* parse block */
        index = parseBlock(tokens, forNode, index);
        break;
    case PYTOK_LIST_STR:
        /* add list as child */
        addChild(currentNode, createNode("List", tokens[index]));
        index++;
        currentNode = currentNode->children[currentNode->num_children - 1];

        /* parse block */
        index = parseBlock(tokens, forNode, index);
        break;
    default:
        break;
    }
    return index;
}

size_t parseFuncDef(Token **tokens, TreeNode *currentNode, size_t index)
{
    /* find return type */
    char *returnType = findReturnType(tokens, index);

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

    /* check for comments */
    if (peekToken(tokens[index]) == PYTOK_COMMENT || peekToken(tokens[index]) == PYTOK_MULTI_COMMENT)
    {
        index = parseComments(tokens, currentNode, index);
    }

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


    /* remove left paran */
    if(tokens[index]->type == PYTOK_LEFTPARENTHESIS){
        index++;
    }

    /* add first term as child */
    addChild(currentNode, createNode("Term", tokens[index]));
    index++;

    /* add op as child */
    addChild(currentNode, createNode("Operator", tokens[index]));
    index++;

    /* add second term as child */
    addChild(currentNode, createNode("Term", tokens[index]));
    index++;

    /* remove right paran */
    if(tokens[index]->type == PYTOK_RIGHTPARENTHESIS){
        index++;
    }

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
    /* check for comments */
    if (peekToken(tokens[index]) == PYTOK_COMMENT || peekToken(tokens[index]) == PYTOK_MULTI_COMMENT)
    {
        index = parseComments(tokens, currentNode, index);
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
            tokens[index]->c_type = "char []";
            break;
        case PYTOK_INT:
            tokens[index]->c_type = "int";
            break;
        case PYTOK_FLOAT:
            tokens[index]->c_type = "float";
            break;
        case PYTOK_LIST_FLOAT:
            tokens[index]->c_type = "float []";
            break;
        case PYTOK_LIST_INT:
            tokens[index]->c_type = "int []";
            break;
        case PYTOK_LIST_STR:
            tokens[index]->c_type = "char* []";
            break;
        case PYTOK_BOOLEAN:
            tokens[index]->c_type = "int";
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
        case PYTOK_LIST_FLOAT:
            addChild(currentNode, createNode("Literal", tokens[index]));
            index++;
            break;
        case PYTOK_LIST_INT:
            addChild(currentNode, createNode("Literal", tokens[index]));
            index++;
            break;
        case PYTOK_LIST_STR:
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

        /* check for comments */
        if (peekToken(tokens[index]) == PYTOK_COMMENT || peekToken(tokens[index]) == PYTOK_MULTI_COMMENT)
        {
            index = parseComments(tokens, currentNode, index);
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

    /* parse params if have */
    if (tokens[index]->type != PYTOK_RIGHTPARENTHESIS)
    {

        index = parseParamList(tokens, currentNode, index);
    }
    /* skip right param */
    index++;

    printf("current token: %s\n", tokens[index]->lexeme);

    /* check for comments */
    if (peekToken(tokens[index]) == PYTOK_COMMENT || peekToken(tokens[index]) == PYTOK_MULTI_COMMENT)
    {
        index = parseComments(tokens, currentNode, index);
    }

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
    int indent, parentIndent;

    /* skip colon keyword */
    index++;

    /* check for comments */
    if (tokens[index]->type == PYTOK_COMMENT || tokens[index]->type == PYTOK_MULTI_COMMENT)
    {
        index = parseComments(tokens, currentNode, index);
    }

    /* check for eol, add eol as child */
    if (tokens[index]->type == PYTOK_EOL)
    {
        addChild(currentNode, createNode("EOL", NULL));
        index++;
    }

    /* clear all redundant EOLs */
    while (tokens[index]->type == PYTOK_EOL)
    {
        index++;
    }

    /* set current indent level */
    indent = tokens[index]->num_indentation;

    /* get parent indent */
    parentIndent = currentNode->token->num_indentation;
    /* add block as child */
    addChild(currentNode, createNode("Block", create_token(UNKNOWN, "Block", 0, parentIndent, "Block", 0)));
    currentNode = currentNode->children[currentNode->num_children - 1];

    while (tokens[index]->num_indentation == indent || tokens[index]->type == PYTOK_EOL)
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
        case PYTOK_COMMENT:
            index = parseComments(tokens, currentNode, index);
            break;
        case PYTOK_MULTI_COMMENT:
            index = parseComments(tokens, currentNode, index);
            break;
        case PYTOK_EOL:
            index++;
            break;
        default:
            break;
        }
    }
    return index;
}
