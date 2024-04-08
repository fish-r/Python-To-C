#include "../include/c_writer.h"
#include <stdio.h>
#include <stdlib.h>

/* Define the maximum number of nodes in the tree */
#define MAX_NODES 100

/* Define the states of the FSM */
typedef enum { STATE_IDLE, STATE_TRAVERSING, STATE_DONE } State;

/* Function to traverse the tree using FSM */
void traverse_tree(TreeNode *root) {
  /* Create a stack to store the nodes */
  TreeNode *stack[MAX_NODES];
  TreeNode *current_node;
  int top = -1;
  int i = 0;

  /* Initialize the FSM */
  State current_state = STATE_IDLE;

  /* Start traversing the tree */
  while (current_state != STATE_DONE) {
    switch (current_state) {
    case STATE_IDLE:
      /* Push the root node onto the stack */
      stack[++top] = root;
      current_state = STATE_TRAVERSING;
      break;

    case STATE_TRAVERSING:
      /* Check if the stack is empty */
      if (top == -1) {
        current_state = STATE_DONE;
        break;
      }

      /* Pop the top node from the stack */
      current_node = stack[top--];

      /* Process the current node */
      printf("Current Node: %s \n", current_node->lexeme);

      /* Push the children of the current node onto the stack */
      for (i = 0; i < current_node->num_children; i++) {
        stack[++top] = current_node->children[i];
      }

      break;

    case STATE_DONE:
      /* Do nothing */
      printf("Done\n");
      break;
    }
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
