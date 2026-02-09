#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef enum {
  SUCCESS,
  ERROR_FILE_OPEN,
  ERROR_INVALID_FORMAT,
  ERROR_MEM_ALLOC,
  ERROR_TAG_NOT_FOUND
} Status;

typedef struct {
  char title[31];
  char artist[31];
  char album[31];
  char year[5];
  char comment[31];
  uint8_t genre;
} ID3v1_Tag;

typedef struct {
  char *title;
  char *artist;
  char *album;
  char *year;
  char *comment;
  char *genre;
} TagUpdate;

typedef struct {
  // To be defined
  int version_major;
  int version_minor;
  int size;
} ID3v2_Tag;

#endif // TYPES_H
