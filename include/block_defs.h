#ifndef BLOCK_DEFS_H
#define BLOCK_DEFS_H

#include <stdint.h>
#include <stdio.h>

// MP4 box types
#define FTYP 0x66747970
#define MOOV 0x6D6F6F76
#define MDAT 0x6D646174
#define TRAK 0x7472616B
#define MDIA 0x6D646961
#define MINF 0x6D696E66
#define STBL 0x7374626C
#define FREE 0x66726565
#define SKIP 0x736B6970
#define MVHD 0x6D766864
#define UDTA 0x75647461
#define TKHD 0x746B6864
#define EDTS 0x65647473
#define ELST 0x656C7374
#define MDHD 0x6D646864
#define HDLR 0x68646C72
#define META 0x6D657461
#define VMHD 0x766D6864
#define SMHD 0x736D6864
#define DINF 0x64696E66
#define STSD 0x73747364
#define STTS 0x73747473
#define STSC 0x73747363
#define STSZ 0x7374737A
#define STCO 0x7374636F
#define SGPD 0x73677064
#define SBGP 0x73626770
#define STSS 0x73747373
#define CTTS 0x63747473

/* MP4Box represents the basic building block of an MP4 file.
 * Each box contains size and type information along with its position in the
 * file. */
typedef struct {
  uint32_t size;       // Total box size
  uint32_t type;       // Box type (4-character code)
  uint64_t large_size; // For boxes larger than 2^32 bytes
  long start_position; // Starting position in file
} MP4Box;

/* MP4FileTypeBox represents an MP4 File Type box (ftyp).
 * This box specifies the major brand, minor version and compatible brands for
 * the file. */
typedef struct {
  uint32_t major_brand;            // Major brand identifier
  uint32_t minor_version;          // Minor version of the major brand
  uint32_t compatible_brands[256]; // List of compatible brand identifiers
} MP4FileTypeBox;

uint32_t fourcc_to_uint32(const char *fourcc);
MP4Box *read_box_header(FILE *file);
void skip_box(FILE *file, MP4Box *box);
uint64_t get_box_size(MP4Box *box);
long get_box_header_size(MP4Box *box);
long get_box_payload(MP4Box *box);

#endif // BLOCK_DEFS_H