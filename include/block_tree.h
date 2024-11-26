#ifndef BLOCK_TREE_H
#define BLOCK_TREE_H

#include "block_defs.h"

/* BlockNode represents a node in the MP4 box hierarchy tree structure.
 * Each node contains an MP4 box and can have multiple child nodes. */
typedef struct BlockNode {
  MP4Box* box;            // Pointer to the MP4 box data for this node
  uint32_t capacity;      // Current allocated capacity for children array
  uint32_t num_children;  // Number of child nodes currently stored
  struct BlockNode**
      children;  // Dynamic array of pointers to child nodes (Unbounded array)
} BlockNode;

BlockNode* create_block_node(MP4Box* box);
void add_child_block_node(BlockNode* parent, BlockNode* child);
void free_block_tree(BlockNode* root);

void print_block_tree(BlockNode* root, int indent);

#endif  // BLOCK_TREE_H