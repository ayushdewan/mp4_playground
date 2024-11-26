#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "block_defs.h"

MP4Box* read_box_header(FILE* file) {
  uint8_t header[8];
  if (fread(header, 1, 8, file) != 8) {
    return NULL;
  }

  // free this when the tree is freed
  MP4Box* box = (MP4Box*)malloc(sizeof(MP4Box));

  // all fields are reversed byte order, for endianness
  box->size = ntohl(*(uint32_t*)header);
  box->type = ntohl(*(uint32_t*)(header + sizeof(uint32_t)));
  box->start_position = ftell(file) - 8;

  if (box->size == 1) {
    uint8_t large_size[8];
    if (fread(large_size, 1, 8, file) != 8) {
      free(box);
      return NULL;
    }
    uint32_t upper = ntohl(*(uint32_t*)large_size);
    uint32_t lower = ntohl(*(uint32_t*)(large_size + 4));
    box->large_size = ((uint64_t)upper << 32) | lower;
  }

  return box;
}

uint64_t get_box_size(MP4Box* box) {
  return box->size == 1 ? box->large_size : box->size;
}

void skip_box(FILE* file, MP4Box* box) {
  fseek(file, box->start_position + get_box_size(box), SEEK_SET);
}

long get_box_header_size(MP4Box* box) {
  return box->size == 1 ? 16 : 8;
}

long get_box_payload(MP4Box* box) {
  return box->start_position + get_box_header_size(box);
}

void print_box(MP4Box* box) {
  printf("%c%c%c%c (%d bytes, starts at %ld)\n", (box->type >> 24) & 0xFF,
         (box->type >> 16) & 0xFF, (box->type >> 8) & 0xFF, box->type & 0xFF,
         box->size, box->start_position);
}