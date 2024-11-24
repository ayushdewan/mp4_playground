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
      parse_mp4_helper(
          file,
          current_box->start_position + get_box_header_size(current_box),
          current_box->start_position + get_box_size(current_box),
          child);
      break;
    case MDAT:
      // printf("Found Media Data (mdat) box\n");
      break;
    case TRAK:
      parse_mp4_helper(
          file,
          current_box->start_position + get_box_header_size(current_box),
          current_box->start_position + get_box_size(current_box),
          child);
      // printf("Found Track (trak) box\n");
      break;
    case MDIA:
      parse_mp4_helper(
          file,
          current_box->start_position + get_box_header_size(current_box),
          current_box->start_position + get_box_size(current_box),
          child);
      // printf("Found Media (mdia) box\n");
      break;
    case MINF:
      parse_mp4_helper(
          file,
          current_box->start_position + get_box_header_size(current_box),
          current_box->start_position + get_box_size(current_box),
          child);
      // printf("Found Media Info (minf) box\n");
      break;
    case STBL:
      parse_mp4_helper(
          file,
          current_box->start_position + get_box_header_size(current_box),
          current_box->start_position + get_box_size(current_box),
          child);
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
      parse_mp4_helper(
          file,
          current_box->start_position + get_box_header_size(current_box),
          current_box->start_position + get_box_size(current_box),
          child);
      // printf("Found User Data (udta) box\n");
      break;
    case TKHD:
      // printf("Found Track Header (tkhd) box\n");
      break;
    case EDTS:
      parse_mp4_helper(
          file,
          current_box->start_position + get_box_header_size(current_box),
          current_box->start_position + get_box_size(current_box),
          child);
      // printf("Found Edit List (edts) box\n");
      break;
    case MDHD:
      // printf("Found Media Header (mdhd) box\n");
      break;
    case HDLR:
      // printf("Found Handler Reference (hdlr) box\n");
      break;
    case ELST:
      // printf("Found Edit List (elst) box\n");
      break;
    case META:
      // printf("Found Meta (meta) box\n");
      break;
    case VMHD:
      // printf("Found Video Media Header (vmhd) box\n");
      break;
    case SMHD:
      // printf("Found Sound Media Header (smhd) box\n");
      break;
    case DINF:
      // printf("Found Data Information (dinf) box\n");
      break;
    case STSD:
      // TODO: Fix this, causes a bug
      // parse_mp4_helper(file, current_box->start_position +
      // get_box_header_size(current_box),
      //                  current_box->start_position +
      //                  get_box_size(current_box), child);
      // printf("Found Sample Table (stsd) box\n");
      break;
    case STTS:
      // printf("Found Time to Sample (stts) box\n");
      break;
    case STSC:
      // printf("Found Sample To Chunk (stsc) box\n");
      break;
    case STSZ:
      // printf("Found Sample Size (stsz) box\n");
      break;
    case STCO:
      // printf("Found Sample To Chunk Offset (stco) box\n");
      break;
    case SGPD:
      // printf("Found Sample Group Description (sgpd) box\n");
      break;
    case SBGP:
      // printf("Found Sample To Group (sbpg) box\n");
      break;
    case STSS:
      // printf("Found Sample To Sync Sample (stss) box\n");
      break;
    case CTTS:
      // printf("Found Composition To Sample (ctts) box\n");
      break;
    default: {
      char type_str[5];
      memcpy(type_str, (char *)&current_box->type, 4);
      type_str[4] = '\0';
      printf("Found unknown box: %s\n", type_str);
      if (current_box->size > 0) {
        parse_mp4_helper(
            file,
            current_box->start_position + get_box_header_size(current_box),
            current_box->start_position + get_box_size(current_box),
            child);
      }
      return;
    }
    }

    // Move to sibling box in the next while loop iteration
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
