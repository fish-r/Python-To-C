#include "../include/c_writer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Define the maximum number of nodes in the tree */
#define MAX_NODES 100

/* Define the states of the FSM */

/* Function to traverse the tree using FSM */
void traverse_tree(TreeNode *root) {
  /* Create a stack to store the nodes */
  TreeNode *queue[MAX_NODES];
  TreeNode *current_node;
  int front = -1;
  int rear = -1;
  int i = 0;

  /* Initialize the FSM */
  State current_state = STATE_IDLE;

  /* Start traversing the tree */
  while (current_state != STATE_DONE) {
    switch (current_state) {
    case STATE_IDLE:
      /* Enqueue the root node */
      queue[++rear] = root;
      current_state = STATE_TRAVERSING;
      break;

    case STATE_TRAVERSING:
      /* Check if the queue is empty */
      if (front == rear) {
        current_state = STATE_DONE;
        break;
      }
      /* Dequeue the front node */
      current_node = queue[++front];
      /* Process the current node */
      /*printf("Current Node: %s \n", current_node->label);*/
      /* Enqueue the children of the current node */
      for (i = 0; i < current_node->num_children; i++) {
        queue[++rear] = current_node->children[i];
      }
      set_state(&current_state, current_node);
      break;

    case WRITE_FN_DEF:
      printf("Label: %s\n", current_node->label);
      if (strcmp(current_node->label, "FunctionDefinition") == 0) {
        /*TODO: change to return type later*/
        write_to_file("output.c", "void ");
      } else if (strcmp(current_node->label, "Identifier") == 0) {
        write_to_file("output.c", current_node->lexeme);
        write_to_file("output.c", "( ");
      } else if (strcmp(current_node->label, "Parameter") == 0) {
        write_to_file("output.c", current_node->lexeme);
        write_to_file("output.c", ", ");
      } else if (strcmp(current_node->label, "ReturnStatement") == 0) {
        write_to_file("output.c", "return ");
      } else if (strcmp(current_node->label, "Block") == 0) {
        current_state = STATE_TRAVERSING;
        write_to_file("output.c", ") {\n");
      }
      current_node = queue[++front];

      break;

    case WRITE_IF_STMT:
      printf("Writing If Statement\n");
      if (strcmp(current_node->label, "Block") == 0) {
        current_state = STATE_TRAVERSING;
      }
      break;

    case STATE_DONE:
      /* Do nothing */
      printf("Done\n");
      break;
    }
  }
}

void set_state(State *current_state, TreeNode *current_node) {
  if (strcmp(current_node->label, "FunctionDefinition") == 0) {
    *current_state = WRITE_FN_DEF;
  } else if (strcmp(current_node->label, "IfStatement") == 0) {
    *current_state = WRITE_IF_STMT;
  }
}

void write_c_file(TreeNode *root) {
  /* Create a sample tree */
  /* TreeNode *root = createNode("root", "lexeme1"); */

  /* Traverse the tree using FSM */
  traverse_tree(root);
}

/* helper function to write to "output.c" */
void write_to_file(char *filename, char *content) {
  FILE *file = fopen(filename, "a");
  if (file == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }
  fprintf(file, "%s", content);
  fclose(file);
}
