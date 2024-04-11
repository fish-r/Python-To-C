#include "../include/c_writer.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Define the maximum number of nodes in the tree */
#define MAX_NODES 100
/* Define the states of the FSM */

/* Function to traverse the tree using FSM */
void traverse_tree(TreeNode *root, State *prev_state, TreeNode *temp_node, Token **token_array, int *token_count)
{
  /* Create a stack to store the nodes */
  TreeNode *current_node = root;
  State current_state = *prev_state;
  int i = 0;

  /* Process the current node */
  set_state(&current_state, current_node);
  printf("Current Node: %s \n", current_node->label);
  process_node(current_node, &current_state, temp_node, token_array, token_count);

  if (root->num_children == 0)
  {
    /*printf("Done with node %s \n", current_node->label);*/
    return;
  }

  for (i = 0; i < current_node->num_children; i++)
  {
    /* implement lookahead */
    if (i > 0 &&
        strcmp(current_node->children[i - 1]->label, "Parameter") == 0 &&
        strcmp(current_node->children[i]->label, "Parameter") == 0)
    {
      write_to_file(", ");
    }
    traverse_tree(current_node->children[i], &current_state, temp_node, token_array, token_count);
  }
  /* On recursion exit write closing brackets */
  if (strcmp(current_node->label, "Block") == 0)
  {
    write_to_file("}\n");
  }
  /*printf("Exiting node %s \n", current_node->label);*/
}

void process_node(TreeNode *current_node, State *current_state,
                  TreeNode *temp_node, Token **token_array, int *token_count)
{
  /* printf("Current State: %d, Current Label: %s\n", *current_state,
          current_node->label);*/
  /*printf("Current node address %p\n", (void *)current_node);*/
  if (current_node->token != NULL)
  {
    write_indent(current_node->token->num_indentation);
  }

  /* Store identifier token in array */
  if (strcmp(current_node->label, "Identifier") == 0)
  {
    Token *current_token = current_node->token;
    if (!is_token_present(token_array, *token_count, current_token))
    {
      if (current_token->c_type != NULL)
      {
        token_array[*token_count] = current_token;
        (*token_count)++;
      }
      else
      {
        current_token->c_type = "int";
        token_array[*token_count] = current_token;
        (*token_count)++;
      }
      /*
      printf("Token count after adding: %d\n", *token_count);
      printf("Added token:\n");
      printf("  Lexeme: %s\n", current_token->lexeme);
      printf("  Type: %d\n", current_token->type);
      printf("  Line Number: %d\n", current_token->line_number);
      printf("  Number of Indentation: %d\n", current_token->num_indentation);
      printf("  C Type: %s\n", current_token->c_type);
      printf("  String Length: %d\n", current_token->str_length);
      */
    }
  }
  if (*current_state == WRITE_INCLUDES)
  {
    if (strcmp(current_node->label, "Program") == 0)
    {
      write_to_file("#include <stdio.h>\n#include <stdlib.h>\n\n");
    }
  }

  if (strcmp(current_node->label, "Block") == 0)
  {
    write_to_file("{\n");
  }

  if (*current_state == WRITE_FN_DEF)
  {
    if (strcmp(current_node->label, "Identifier") == 0)
    {
      printf("return type %s\n", current_node->token->c_type);
      write_to_file(current_node->token->c_type);
      write_to_file(" ");
      write_to_file(current_node->token->lexeme);
      write_to_file("(");
    }
    else if (strcmp(current_node->label, "Parameter") == 0)
    {
      write_to_file("int");
      write_to_file(" ");
      write_to_file(current_node->token->lexeme);
    }
    else if (strcmp(current_node->label, "EOL") == 0)
    {
      write_to_file(")\n");
    }
    return;
  }

  else if (*current_state == WRITE_IF_STMT)
  {
    if (strcmp(current_node->label, "IfStatement") == 0)
    {
      write_to_file("if (");
    }
    else if (strcmp(current_node->label, "EOL") == 0)
    {
      write_to_file(")");
    }
    return;
  }

  else if (*current_state == WRITE_CONDITION)
  {
    if (strcmp(current_node->label, "Term") == 0)
    {
      write_to_file(current_node->token->lexeme);
    }
    else if (strcmp(current_node->label, "Operator") == 0)
    {
      write_to_file(" ");
      write_to_file(current_node->token->lexeme);
      write_to_file(" ");
    }
    return;
  }

  else if (*current_state == WRITE_PRINT_STMT)
  {
    if (strcmp(current_node->label, "PrintStatement") == 0)
    {
      write_to_file("printf(");
    }
    if (strcmp(current_node->label, "Literal") == 0)
    {
      if (strcmp(current_node->token->c_type, "int") == 0)
      {
        write_to_file("\"%d\\n\", ");
      }
      else if (strcmp(current_node->token->c_type, "float") == 0)
      {
        write_to_file("\"%f\\n\", ");
      }
      else if (strcmp(current_node->token->c_type, "str") == 0)
      {
        write_to_file("\"%f\\n\", ");
      }
      write_to_file(current_node->token->lexeme);
      write_to_file(")");
    }
    else if (strcmp(current_node->label, "Identifier") == 0)
    {
      char *c_type;
      int i;
      for (i = 0; i < *token_count; i++)
      {
        if (strcmp(token_array[i]->lexeme, current_node->token->lexeme) == 0)
        {
          c_type = token_array[i]->c_type;
          break;
        }
      }
      if (strcmp(c_type, "int") == 0)
      {
        write_to_file("\"%d\", ");
      }
      else if (strcmp(c_type, "float") == 0)
      {
        write_to_file("\"%f\", ");
      }
      else if (strcmp(c_type, "str") == 0)
      {
        write_to_file("\"%s\", ");
      }
      write_to_file(current_node->token->lexeme);
      write_to_file(")");
    }
    else if (strcmp(current_node->label, "EOL") == 0)
    {
      write_to_file("; \n");
    }
    return;
  }

  else if (*current_state == WRITE_ELSE_STMT)
  {
    if (strcmp(current_node->label, "ElseStatement") == 0)
    {
      write_to_file("else");
    }
    else if (strcmp(current_node->label, "EOL") == 0)
    {
      write_to_file("\n");
    }
    return;
  }

  else if (*current_state == WRITE_LOOP)
  {
    if (strcmp(current_node->label, "ForStatement") == 0)
    {
      write_to_file("for (");
    }
    else if (strcmp(current_node->label, "Identifier") == 0)
    {
      /*      write_to_file(current_node->token->c_type);
            write_to_file(" ");
            write_to_file(current_node->token->lexeme);*/
      *temp_node = *current_node;
    }
    else if (strcmp(current_node->label, "Start") == 0)
    {
      write_to_file(current_node->token->c_type);
      write_to_file(" ");
      write_to_file(temp_node->token->lexeme);
      write_to_file(" = ");
      write_to_file(current_node->token->lexeme);
      write_to_file("; ");
    }
    else if (strcmp(current_node->label, "Stop") == 0)
    {
      write_to_file(temp_node->token->lexeme);
      write_to_file(" < ");
      write_to_file(current_node->token->lexeme);
      write_to_file("; ");
    }
    else if (strcmp(current_node->label, "Step") == 0)
    {
      write_to_file(temp_node->token->lexeme);
      write_to_file(" += ");
      write_to_file(current_node->token->lexeme);
      write_to_file(")");
    }
    else if (strcmp(current_node->label, "EOL") == 0)
    {
      write_to_file("\n");
    }
    return;
  }

  else if (*current_state == WRITE_RETURN)
  {
    if (strcmp(current_node->label, "ReturnStatement") == 0)
    {
      write_to_file("return ");
    }
    else if (strcmp(current_node->label, "Literal") == 0)
    {
      write_to_file(current_node->token->lexeme);
    }
    else if (strcmp(current_node->label, "EOL") == 0)
    {
      write_to_file(";\n");
    }

    return;
  }

  else if (*current_state == WRITE_EXPRESSION)
  {
    if (strcmp(current_node->label, "Identifier") == 0)
    {
      write_to_file(current_node->token->c_type);
      write_to_file(" ");
      write_to_file(current_node->token->lexeme);
      write_to_file(" ");
    }
    else if (strcmp(current_node->label, "Operator") == 0)
    {
      write_to_file(current_node->token->lexeme);
      write_to_file(" ");
    }
    else if (strcmp(current_node->label, "Literal") == 0)
    {
      write_to_file(current_node->token->lexeme);
    }
    else if (strcmp(current_node->label, "EOL") == 0)
    {
      write_to_file(";\n");
    }

    return;
  }

  return;
}

void set_state(State *current_state, TreeNode *current_node)
{
  /*printf("Current State: %d\n", *current_state);*/
  if (strcmp(current_node->label, "Program") == 0)
  {
    *current_state = WRITE_INCLUDES;
  }
  else if (strcmp(current_node->label, "FunctionDefinition") == 0)
  {
    *current_state = WRITE_FN_DEF;
  }
  else if (strcmp(current_node->label, "IfStatement") == 0)
  {
    *current_state = WRITE_IF_STMT;
  }
  else if (strcmp(current_node->label, "Condition") == 0)
  {
    *current_state = WRITE_CONDITION;
  }
  else if (strcmp(current_node->label, "PrintStatement") == 0)
  {
    *current_state = WRITE_PRINT_STMT;
  }
  else if (strcmp(current_node->label, "ElseStatement") == 0)
  {
    *current_state = WRITE_ELSE_STMT;
  }
  else if (strcmp(current_node->label, "ReturnStatement") == 0)
  {
    *current_state = WRITE_RETURN;
  }
  else if (strcmp(current_node->label, "ForStatement") == 0)
  {
    *current_state = WRITE_LOOP;
  }
  else if (strcmp(current_node->label, "Expression") == 0)
  {
    *current_state = WRITE_EXPRESSION;
  }
  else
  {
    return;
  }
}

void write_c_file(TreeNode *root)
{
  State initial_state = STATE_INIT;
  TreeNode *temp_node = (TreeNode *)malloc(sizeof(TreeNode));
  Token **token_array = (Token **)malloc(sizeof(Token));
  int token_count = 0;

  clear_file("output.c");

  traverse_tree(root, &initial_state, temp_node, token_array, &token_count);
}

/* helper function to clear output.c */
void clear_file(char *filename)
{
  FILE *file = fopen(filename, "w");
  if (file == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }
  fclose(file);
}

/* helper function to write at start of file*/
void write_at_start(char *content)
{
  long i, content_size, file_size;
  char c;
  FILE *file = fopen("output.c", "r+");
  if (file == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }

  content_size = strlen(content);

  fseek(file, 0, SEEK_END);
  file_size = ftell(file);

  for (i = file_size - 1; i >= 0; i--)
  {
    fseek(file, i, SEEK_SET);
    c = fgetc(file);
    fseek(file, i + content_size, SEEK_SET);
    fputc(c, file);
  }

  fseek(file, 0, SEEK_SET);
  fprintf(file, "%s", content);

  fclose(file);
}

/* helper function to write to "output.c" */
void write_to_file(char *content)
{
  FILE *file = fopen("output.c", "a");
  if (file == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }
  fprintf(file, "%s", content);
  fclose(file);
}

void write_indent(int num_indent)
{

  int num_spaces = num_indent;
  int i;
  FILE *file = fopen("output.c", "a");
  if (file == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }
  for (i = 0; i < num_spaces; ++i)
  {
    fprintf(file, " ");
  }
  fclose(file);
}

char *get_c_type(Token *token_array, int token_count, const char *lexeme_to_find)
{
  int i;
  for (i = 0; i < token_count; i++)
  {

    if (strcmp(token_array[i].lexeme, lexeme_to_find) == 0)
    {
      return token_array[i].c_type;
    }
  }
  return NULL;
}

int is_token_present(Token **token_array, int token_count, Token *current_token)
{
  int i;
  for (i = 0; i < token_count; i++)
  {
    if (strcmp(token_array[i]->lexeme, current_token->lexeme) == 0)
    {
      return 1;
    }
  }
  return 0;
}