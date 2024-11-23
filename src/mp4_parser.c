#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/block_defs.h"
#include "../include/block_tree.h"

void parse_mp4_helper(FILE *file, long pos, long end, BlockNode *root) {
  if (pos >= end) {
    return;
  }

  MP4Box *current_box;
  while ((current_box = read_box_header(file)) != NULL) {
    BlockNode *child = create_block_node(current_box);
    add_child_block_node(root, child);

    switch (current_box->type) {
    case FTYP:
      // printf("Found File Type (ftyp) box with size %d\n", current_box.size);
      break;
    case MOOV:
      // printf("Found Movie (moov) box\n");
      parse_mp4_helper(file, pos + get_box_header_size(current_box),
                       pos + get_box_size(current_box), child);
      break;
    case MDAT:
      // printf("Found Media Data (mdat) box\n");
      break;
    case TRAK:
    parse_mp4_helper(file, pos + get_box_header_size(current_box),
                       pos + get_box_size(current_box), child);
      // printf("Found Track (trak) box\n");
      break;
    case MDIA:
      // printf("Found Media (mdia) box\n");
      break;
    case MINF:
      // printf("Found Media Info (minf) box\n");
      break;
    case STBL:
      // printf("Found Sample Table (stbl) box\n");
      break;
    case FREE:
      // printf("Found Free (free) box\n");
      break;
    case SKIP:
      // printf("Found Skip (skip) box\n");
      break;
    case MVHD:
      // printf("Found Movie Header (mvhd) box\n");
      break;
    case UDTA:
      // printf("Found User Data (udta) box\n");
      break;
    case TKHD:
      // printf("Found Track Header (tkhd) box\n");
      break;
    case EDTS:
      // printf("Found Edit List (edts) box\n");
      break;
    default: {
      char type_str[5];
      memcpy(type_str, (char*)&current_box->type, 4);
      type_str[4] = '\0';
      printf("Found unknown box: %s\n", type_str);
      break;
    }
    }

    // Move to next box
    skip_box(file, current_box);

    // Check for EOF
    if (feof(file) || ftell(file) >= end) {
      break;
    }
  }
}

int parse_mp4(const char *filename, BlockNode *root) {
  FILE *file = fopen(filename, "rb");
  if (!file) {
    fprintf(stderr, "Error opening file: %s\n", filename);
    return -1;
  }

  // Get file size
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  parse_mp4_helper(file, 0, size, root);

  fclose(file);
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <mp4_file>\n", argv[0]);
    return 1;
  }

  BlockNode *root = create_block_node(NULL);
  int status = parse_mp4(argv[1], root);
  print_block_tree(root, 0);
  free_block_tree(root);

  return status;
}
