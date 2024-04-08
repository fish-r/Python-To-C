#ifndef C_WRITER_H
#define C_WRITER_H
#include "parseTree.h"

typedef enum {
  STATE_INIT,
  WRITE_INCLUDES,
  WRITE_FN_DEF,
  WRITE_IF_STMT,
  WRITE_CONDITION,
  WRITE_PRINT_STMT,
  WRITE_ELSE_STMT,
  STATE_END
} State;

void traverse_tree(TreeNode *root, State *prev_state);
void set_state(State * current_state, TreeNode *current_node);
void write_c_file(TreeNode *root);
void process_node(TreeNode *current_node, State *current_state);
/* Utils */
void write_to_file( char *content);
void clear_file(char *filename);
void write_at_start(char *content);
#endif
