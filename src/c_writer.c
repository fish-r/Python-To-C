#include "../include/c_writer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Define the maximum number of nodes in the tree */
#define MAX_NODES 100
/* Define the states of the FSM */

/* Function to traverse the tree using FSM */
void traverse_tree(TreeNode *root, State *prev_state, TreeNode *temp_node,
                   Token **token_array, int *token_count) {

  int i = 0;
  /* Process the current node */
  set_state(prev_state, root);

  process_node(root, prev_state, temp_node, token_array, token_count);

  if (root->num_children == 0) {
    return;
  }

  for (i = 0; i < root->num_children; i++) {
    /* implement lookahead */
    if (i > 0 && strcmp(root->children[i - 1]->label, "Parameter") == 0 &&
        strcmp(root->children[i]->label, "Parameter") == 0) {
      write_to_file(", ");
      write_to_header_file(", ");
    }
    traverse_tree(root->children[i], prev_state, temp_node, token_array,
                  token_count);
  }
  /* On recursion exit write closing brackets */
  if (strcmp(root->label, "Block") == 0) {
    write_indent(root->token->num_indentation);

    write_to_file("}\n");
  }
}

void process_node(TreeNode *current_node, State *current_state,
                  TreeNode *temp_node, Token **token_array, int *token_count) {
  char temp_str[100];
  printf("Current State: %d, Current Label: %s\n", *current_state,
         current_node->label);
  /*printf("Current node address %p\n", (void *)current_node);*/
  if (current_node->token != NULL) {
    write_indent(current_node->token->num_indentation);
  }

  /* Store identifier token in array */
  if (strcmp(current_node->label, "Identifier") == 0) {
    Token *current_token = current_node->token;
    if (!is_token_present(token_array, *token_count, current_token)) {
      if (current_token->c_type != NULL) {
        token_array[*token_count] = current_token;
        (*token_count)++;
      } else {
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
      printf("  String Length: %d\n", current_token->str_length); */
    }
  }
  if (*current_state == WRITE_INCLUDES) {
    if (strcmp(current_node->label, "Program") == 0) {
      write_to_file(
          "#include <stdio.h>\n#include <stdlib.h>\n#include \"output.h\"\n\n");
      write_to_header_file("#include <stdio.h>\n#include <stdlib.h>\n\n");
    }
  }

  if (strcmp(current_node->label, "Block") == 0) {
    write_to_file("{\n");
  }

  if (*current_state == WRITE_FN_DEF) {
    if (strcmp(current_node->label, "Identifier") == 0) {

      if (strcmp(current_node->token->c_type, "char []") == 0) {
        write_to_file("char* ");
        write_to_file(" ");
        write_to_file(current_node->token->lexeme);
        write_to_file("(");

        /* write to header file */
        write_to_header_file("char* ");
        write_to_header_file(" ");
        write_to_header_file(current_node->token->lexeme);
        write_to_header_file("(");
      } else {
        write_to_file(current_node->token->c_type);
        write_to_file(" ");
        write_to_file(current_node->token->lexeme);
        write_to_file("(");

        /* write to header file */
        write_to_header_file(current_node->token->c_type);
        write_to_header_file(" ");
        write_to_header_file(current_node->token->lexeme);
        write_to_header_file("(");
      }
    } else if (strcmp(current_node->label, "Parameter") == 0) {
      write_to_file("int");
      write_to_file(" ");
      write_to_file(current_node->token->lexeme);
      /* write to header file */
      write_to_header_file("int");
      write_to_header_file(" ");
      write_to_header_file(current_node->token->lexeme);
    } else if (strcmp(current_node->label, "EOL") == 0) {
      write_to_file(")\n");
      write_to_header_file(");\n");
    }
    return;
  }

  else if (*current_state == WRITE_FN_CALL) {
    if (strcmp(current_node->label, "Identifier") == 0) {
      write_to_file(current_node->token->lexeme);
      write_to_file("(");
    } else if (strcmp(current_node->label, "Parameter") == 0) {
      write_to_file(current_node->token->lexeme);
    } else if (strcmp(current_node->label, "EOL") == 0) {
      write_to_file("); \n");
    }
    return;
  }

  else if (*current_state == WRITE_IF_STMT) {
    if (strcmp(current_node->label, "IfStatement") == 0) {
      write_to_file("if (");
    } else if (strcmp(current_node->label, "ElifStatement") == 0) {
      write_to_file("else if (");
    } else if (strcmp(current_node->label, "EOL") == 0) {
      write_to_file(")");
    }
    return;
  }

  else if (*current_state == WRITE_CONDITION) {
    if (strcmp(current_node->label, "Term") == 0) {
      write_to_file(current_node->token->lexeme);
    } else if (strcmp(current_node->label, "Operator") == 0) {
      write_to_file(" ");
      write_to_file(current_node->token->lexeme);
      write_to_file(" ");
    } else if (strcmp(current_node->label, "EOL") == 0) {
      write_to_file(")\n");
    }
    return;
  }

  else if (*current_state == WRITE_PRINT_STMT) {
    if (strcmp(current_node->label, "PrintStatement") == 0) {
      write_to_file("printf(");
    } else if (strcmp(current_node->label, "Literal") == 0) {
      if (strcmp(current_node->token->c_type, "int") == 0) {
        write_to_file("\"%d\", ");
      } else if (strcmp(current_node->token->c_type, "float") == 0) {
        write_to_file("\"%f\", ");
      } else if ((strcmp(current_node->token->c_type, "char") == 0)) {
        write_to_file("\"%c\", ");
      } else if ((strcmp(current_node->token->c_type, "char[]") == 0)) {
        write_to_file("\"%s\", ");
      }
      write_to_file(current_node->token->lexeme);
      write_to_file(")");
    } else if (strcmp(current_node->label, "Identifier") == 0) {
      char *c_type;
      int i;
      for (i = 0; i < *token_count; i++) {
        if (strcmp(token_array[i]->lexeme, current_node->token->lexeme) == 0) {
          c_type = token_array[i]->c_type;
          break;
        }
      }
      if (strcmp(c_type, "int") == 0) {
        write_to_file("\"%d\", ");
      } else if (strcmp(c_type, "float") == 0) {
        write_to_file("\"%f\", ");
      } else if ((strcmp(c_type, "char") == 0)) {
        write_to_file("\"%c\", ");
      } else if ((strcmp(c_type, "char[]"))) {
        write_to_file("\"%s\", ");
      }
      write_to_file(current_node->token->lexeme);
      write_to_file(")");
    } else if (strcmp(current_node->label, "EOL") == 0) {
      write_to_file("; \n");
    }
    return;
  }

  else if (*current_state == WRITE_ELSE_STMT) {
    if (strcmp(current_node->label, "ElseStatement") == 0) {
      write_to_file("else");
    } else if (strcmp(current_node->label, "EOL") == 0) {
      write_to_file("\n");
    }
    return;
  }

  else if (*current_state == WRITE_WHILE_LOOP) {
    if (strcmp(current_node->label, "WhileStatement") == 0) {
      write_to_file("while (");
    } else if (strcmp(current_node->label, "Identifier") == 0) {
      write_to_file(current_node->token->lexeme);
    } else if (strcmp(current_node->label, "EOL") == 0) {
      write_to_file("\n");
    }
    return;
  }

  else if (*current_state == WRITE_FOR_LOOP) {
    if (strcmp(current_node->label, "ForStatement") == 0) {
      write_to_file("for (");
    } else if (strcmp(current_node->label, "Identifier") == 0) {
      printf("Identifier lexeme, length: %s, %d\n", current_node->token->lexeme,
             current_node->token->str_length);
      *temp_node = *current_node;
    }
  }

  else if (*current_state == WRITE_ITERABLE) {
    if (strcmp(current_node->label, "Identifier") == 0) {
      write_to_file(temp_node->token->c_type);
      write_to_file(" ");
      write_to_file(temp_node->token->lexeme);
      write_to_file("= 0; ");
      write_to_file(temp_node->token->lexeme);
      write_to_file(" < ");
      sprintf(temp_str, "%d", temp_node->token->str_length);
      write_to_file(temp_str);
      write_to_file("; ");
      write_to_file(temp_node->token->lexeme);
      write_to_file("++)");
    }
  }

  else if (*current_state == WRITE_RANGE) {
    if (strcmp(current_node->label, "Start") == 0) {
      write_to_file(current_node->token->c_type);
      write_to_file(" ");
      write_to_file(temp_node->token->lexeme);
      write_to_file(" = ");
      write_to_file(current_node->token->lexeme);
      write_to_file("; ");
    } else if (strcmp(current_node->label, "Stop") == 0) {
      write_to_file(temp_node->token->lexeme);
      write_to_file(" < ");
      write_to_file(current_node->token->lexeme);
      write_to_file("; ");
    } else if (strcmp(current_node->label, "Step") == 0) {
      write_to_file(temp_node->token->lexeme);
      write_to_file(" += ");
      write_to_file(current_node->token->lexeme);
      write_to_file(")");
    } else if (strcmp(current_node->label, "EOL") == 0) {
      write_to_file("\n");
    }
  }

  else if (*current_state == WRITE_RETURN) {
    if (strcmp(current_node->label, "ReturnStatement") == 0) {
      write_to_file("return ");
    } else if ((strcmp(current_node->label, "Literal") == 0) ||
               (strcmp(current_node->label, "Identifier") == 0)) {
      write_to_file(current_node->token->lexeme);
    } else if (strcmp(current_node->label, "EOL") == 0) {
      write_to_file(";\n");
    }

    return;
  }

  else if (*current_state == WRITE_EXPRESSION) {
    if (strcmp(current_node->label, "Identifier") == 0) {
      if (strcmp(current_node->token->c_type, "char []") == 0) {
        write_to_file("char");
        write_to_file(" ");
        write_to_file(current_node->token->lexeme);
        write_to_file("[] ");
      } else if (strcmp(current_node->token->c_type, "int []") == 0) {
        write_to_file("int");
        write_to_file(" ");
        write_to_file(current_node->token->lexeme);
        write_to_file("[] ");
      } else if (strcmp(current_node->token->c_type, "float []") == 0) {
        write_to_file("float");
        write_to_file(" ");
        write_to_file(current_node->token->lexeme);
        write_to_file("[] ");
      } else if (strcmp(current_node->token->c_type, "char* []") == 0) {
        write_to_file("char*");
        write_to_file(" ");
        write_to_file(current_node->token->lexeme);
        write_to_file("[] ");
      }

      else {
        write_to_file(current_node->token->c_type);
        write_to_file(" ");
        write_to_file(current_node->token->lexeme);
        write_to_file(" ");
      }
    } else if (strcmp(current_node->label, "Operator") == 0) {
      write_to_file(current_node->token->lexeme);
      write_to_file(" ");
    } else if (strcmp(current_node->label, "Literal") == 0) {
      write_to_file(current_node->token->lexeme);
    } else if (strcmp(current_node->label, "EOL") == 0) {
      write_to_file(";\n");
    }

    return;
  }

  else if (*current_state == WRITE_COMMENT) {
    if (strcmp(current_node->label, "Comment") == 0) {
      write_to_file("/* ");
      write_to_file(current_node->token->lexeme);
      write_to_file(" */");
    } else if (strcmp(current_node->label, "EOL") == 0) {
      write_to_file("\n");
    }
    return;
  }

  return;
}

void set_state(State *current_state, TreeNode *current_node) {
  if (strcmp(current_node->label, "Program") == 0) {
    *current_state = WRITE_INCLUDES;
  } else if (strcmp(current_node->label, "FunctionDefinition") == 0) {
    printf("Setting state to write function definition\n");
    *current_state = WRITE_FN_DEF;
  } else if (strcmp(current_node->label, "IfStatement") == 0) {
    *current_state = WRITE_IF_STMT;
  } else if (strcmp(current_node->label, "ElifStatement") == 0) {
    *current_state = WRITE_IF_STMT;
  } else if (strcmp(current_node->label, "Condition") == 0) {
    *current_state = WRITE_CONDITION;
  } else if (strcmp(current_node->label, "PrintStatement") == 0) {
    *current_state = WRITE_PRINT_STMT;
  } else if (strcmp(current_node->label, "ElseStatement") == 0) {
    *current_state = WRITE_ELSE_STMT;
  } else if (strcmp(current_node->label, "ReturnStatement") == 0) {
    *current_state = WRITE_RETURN;
  } else if (strcmp(current_node->label, "ForStatement") == 0) {
    *current_state = WRITE_FOR_LOOP;
  } else if (strcmp(current_node->label, "WhileStatement") == 0) {
    *current_state = WRITE_WHILE_LOOP;
  } else if (strcmp(current_node->label, "Expression") == 0) {
    *current_state = WRITE_EXPRESSION;
  } else if (strcmp(current_node->label, "FunctionCall") == 0) {
    *current_state = WRITE_FN_CALL;
  } else if (strcmp(current_node->label, "Comment") == 0) {
    *current_state = WRITE_COMMENT;
  } else if (strcmp(current_node->label, "Iterable") == 0) {
    *current_state = WRITE_ITERABLE;
  } else if ((*current_state == WRITE_ITERABLE) &&
             (strcmp(current_node->label, "Range") == 0)) {
    *current_state = WRITE_RANGE;
  }
  return;
}

void write_c_file(TreeNode *root) {
  State initial_state = STATE_INIT;
  TreeNode *temp_node = (TreeNode *)malloc(sizeof(TreeNode));
  Token **token_array = (Token **)malloc(sizeof(Token));
  int token_count = 0;

  clear_file("output.c");
  clear_file("output.h");

  traverse_tree(root, &initial_state, temp_node, token_array, &token_count);
}

/* helper function to clear output.c */
void clear_file(char *filename) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }
  fclose(file);
}

/* helper function to write at start of file*/
void write_at_start(char *content) {
  long i, content_size, file_size;
  char c;
  FILE *file = fopen("output.c", "r+");
  if (file == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }

  content_size = strlen(content);

  fseek(file, 0, SEEK_END);
  file_size = ftell(file);

  for (i = file_size - 1; i >= 0; i--) {
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
void write_to_file(char *content) {
  FILE *file = fopen("output.c", "a");
  if (file == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }
  fprintf(file, "%s", content);
  fclose(file);
}

/* helper function to write to header file */
void write_to_header_file(char *content) {
  FILE *file = fopen("output.h", "a");
  if (file == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }
  fprintf(file, "%s", content);
  fclose(file);
}

void write_indent(int num_indent) {

  int num_spaces = num_indent;
  int i;
  FILE *file = fopen("output.c", "a");
  if (file == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }
  for (i = 0; i < num_spaces; ++i) {
    fprintf(file, " ");
  }
  fclose(file);
}

char *get_c_type(Token *token_array, int token_count,
                 const char *lexeme_to_find) {
  int i;
  for (i = 0; i < token_count; i++) {

    if (strcmp(token_array[i].lexeme, lexeme_to_find) == 0) {
      return token_array[i].c_type;
    }
  }
  return NULL;
}

int is_token_present(Token **token_array, int token_count,
                     Token *current_token) {
  int i;
  for (i = 0; i < token_count; i++) {
    if (strcmp(token_array[i]->lexeme, current_token->lexeme) == 0) {
      return 1;
    }
  }
  return 0;
}
