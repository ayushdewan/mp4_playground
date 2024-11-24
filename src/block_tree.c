#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/block_tree.h"

BlockNode *create_block_node(MP4Box *box) {
  BlockNode *root = (BlockNode *)malloc(sizeof(BlockNode));
  root->box = box; // NULL value for root node, represents whole file
  root->num_children = 0;
  root->capacity = 0;
  root->children = NULL;
  return root;
}

void add_child_block_node(BlockNode *parent, BlockNode *child) {
  assert(parent != NULL);
  assert(child != NULL);

  if (parent->num_children == parent->capacity) {
    if (parent->capacity == 0) {
      parent->capacity = 1;
      parent->children =
          (BlockNode **)malloc(parent->capacity * sizeof(BlockNode *));
    } else {
      parent->capacity *= 2;
      parent->children = (BlockNode **)realloc(
          parent->children,
          parent->capacity * sizeof(BlockNode));
    }
  }

  parent->children[parent->num_children] = child;
  parent->num_children++;
}

void free_block_tree(BlockNode *root) {
  assert(root != NULL);

  for (uint32_t i = 0; i < root->num_children; i++) {
    free_block_tree(root->children[i]);
  }

  // sometimes these variables are NULL, and that's ok with stdlib free
  free(root->box);
  free(root->children);
  free(root);
}

void print_block_tree(BlockNode *root, int indent) {
  assert(root != NULL);

  // Print the current node
  if (root->box == NULL) {
    printf("%*s%s\n", indent, "", "ROOT");
  } else {
    // needs reverse byte order for display, kinda hacky
    uint32_t type = ntohl(root->box->type);
    char type_str[5];
    snprintf(
        type_str,
        5,
        "%c%c%c%c",
        type & 0xFF,
        (type >> 8) & 0xFF,
        (type >> 16) & 0xFF,
        (type >> 24) & 0xFF);
    printf(
        "%*s%s (%d bytes, starts at %ld)\n",
        indent,
        "",
        type_str,
        root->box->size,
        root->box->start_position);
  }

  // Recursively print all children
  for (uint32_t i = 0; i < root->num_children; i++) {
    print_block_tree(root->children[i], indent + 2);
  }
}