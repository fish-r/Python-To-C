#ifndef C_WRITER_H
#define C_WRITER_H
#include "parseTree.h"

typedef enum {
  STATE_IDLE,
  STATE_TRAVERSING,
  STATE_DONE,
  WRITE_FN_DEF,
  WRITE_IF_STMT
} State;

void traverse_tree(TreeNode *root);
void set_state(State * current_state, TreeNode *current_node);
void write_c_file(TreeNode *root);
void write_to_file(char *filename, char *content);

#endif
