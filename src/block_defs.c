#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/block_defs.h"

uint32_t fourcc_to_uint32(const char *fourcc) {
  return (fourcc[0] << 24) | (fourcc[1] << 16) | (fourcc[2] << 8) | fourcc[3];
}

MP4Box *read_box_header(FILE *file) {
  uint8_t header[8];

  if (fread(header, 1, 8, file) != 8) {
    return NULL;
  }

  // Free this when the tree is freed
  MP4Box *box = (MP4Box *)malloc(sizeof(MP4Box));
  box->size =
      (header[0] << 24) | (header[1] << 16) | (header[2] << 8) | header[3];
  box->type =
      (header[4] << 24) | (header[5] << 16) | (header[6] << 8) | header[7];
  box->start_position = ftell(file) - 8;

  if (box->size == 1) {
    uint8_t large_size[8];
    if (fread(large_size, 1, 8, file) != 8) {
      free(box);
      return NULL;
    }
    box->large_size =
        ((uint64_t)large_size[0] << 56) | ((uint64_t)large_size[1] << 48) |
        ((uint64_t)large_size[2] << 40) | ((uint64_t)large_size[3] << 32) |
        ((uint64_t)large_size[4] << 24) | ((uint64_t)large_size[5] << 16) |
        ((uint64_t)large_size[6] << 8) | (uint64_t)large_size[7];
  }

  return box;
}

uint64_t get_box_size(MP4Box *box) {
  return box->size == 1 ? box->large_size : box->size;
}

void skip_box(FILE *file, MP4Box *box) {
  fseek(file, box->start_position + get_box_size(box), SEEK_SET);
}

long get_box_header_size(MP4Box *box) { return box->size == 1 ? 16 : 8; }

long get_box_payload(MP4Box *box) {
  return box->start_position + get_box_header_size(box);
}
