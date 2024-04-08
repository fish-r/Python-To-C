#include "../include/c_writer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Define the maximum number of nodes in the tree */
#define MAX_NODES 100

/* Define the states of the FSM */

/* Function to traverse the tree using FSM */
void traverse_tree(TreeNode *root, State *prev_state) {
  /* Create a stack to store the nodes */
  TreeNode *current_node = root;
  State current_state = *prev_state;
  int i = 0;

  /* Process the current node */
  set_state(&current_state, current_node);
  printf("Current Node: %s \n", current_node->label);
  process_node(current_node, &current_state);

  if (root->num_children == 0) {
    /*printf("Done with node %s \n", current_node->label);*/
    return;
  }

  for (i = 0; i < current_node->num_children; i++) {
    traverse_tree(current_node->children[i], &current_state);
  }
  /* On recursion exit write closing brackets */
  if (strcmp(current_node->label, "Block") == 0) {
    write_to_file("}\n");
  }
  /*printf("Exiting node %s \n", current_node->label);*/
}

void process_node(TreeNode *current_node, State *current_state) {
  /* printf("Current State: %d, Current Label: %s\n", *current_state,
          current_node->label);*/
  if (*current_state == WRITE_FN_DEF) {

    if (strcmp(current_node->label, "FunctionDefinition") == 0) {
      /*TODO: change to return type later*/
      write_to_file("void ");

    } else if (strcmp(current_node->label, "Identifier") == 0) {
      write_to_file(current_node->lexeme->lexeme);
      write_to_file("( ");

    } else if (strcmp(current_node->label, "Parameter") == 0) {
      write_to_file(current_node->lexeme->lexeme);
      /* implement lookahead */
      if (strcmp(current_node++->label, "Parameter") == 0) {
        write_to_file(", ");
      }
    } else if (strcmp(current_node->label, "ReturnStatement") == 0) {
      write_to_file("return ");

    } else if (strcmp(current_node->label, "Block") == 0) {
      write_to_file(") {\n");
    }
  }

  else if (*current_state == WRITE_IF_STMT) {
    if (strcmp(current_node->label, "IfStatement") == 0) {
      write_to_file("if (");
    } else if (strcmp(current_node->label, "Block") == 0) {
      write_to_file(") {\n");
    }
  }

  else if (*current_state == WRITE_CONDITION) {
    printf("Write cond, %d\n", *current_state);
    if (strcmp(current_node->label, "first term") == 0) {
      write_to_file(current_node->lexeme->lexeme);
    } else if (strcmp(current_node->label, "operator") == 0) {
      write_to_file(" ");
      write_to_file(current_node->lexeme->lexeme);
      write_to_file(" ");
    } else if (strcmp(current_node->label, "second term") == 0) {
      write_to_file(current_node->lexeme->lexeme);
    } else if (strcmp(current_node->label, "Block") == 0) {
      write_to_file(") {\n");
    }
  }

  else if (*current_state == WRITE_PRINT_STMT) {
    if (strcmp(current_node->label, "PrintStatement") == 0) {
      write_to_file("printf(");
    } else if (strcmp(current_node->label, "StringLiteral") == 0) {
      write_to_file(current_node->lexeme->lexeme);
      write_to_file(");\n");
    }

  } else if (*current_state == WRITE_ELSE_STMT) {
    if (strcmp(current_node->label, "ElseStatement") == 0) {
      write_to_file("else{\n");
    } else if (strcmp(current_node->label, "StringLiteral") == 0) {
      write_to_file(current_node->lexeme->lexeme);
      write_to_file(");\n");
    }
  }
}

void set_state(State *current_state, TreeNode *current_node) {
  /*printf("Current State: %d\n", *current_state);*/
  if (strcmp(current_node->label, "FunctionDefinition") == 0) {
    *current_state = WRITE_FN_DEF;
  } else if (strcmp(current_node->label, "IfStatement") == 0) {
    *current_state = WRITE_IF_STMT;
  } else if (strcmp(current_node->label, "Condition") == 0) {
    *current_state = WRITE_CONDITION;
  } else if (strcmp(current_node->label, "PrintStatement") == 0) {
    *current_state = WRITE_PRINT_STMT;
  } else if (strcmp(current_node->label, "ElseStatement") == 0) {
    *current_state = WRITE_ELSE_STMT;
  } else {
    return;
  }
}

void write_c_file(TreeNode *root) {
  State initial_state = STATE_INIT;
  /* clear file first */
  clear_file("output.c");
  /* Traverse the tree using FSM */
  traverse_tree(root, &initial_state);
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
