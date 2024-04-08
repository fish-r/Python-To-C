#ifndef C_WRITER_H
#define C_WRITER_H
#include "parseTree.h"

void traverse_tree(TreeNode *root);
void write_c_file(TreeNode *root);
void write_to_file(char *filename, char *content);

#endif
