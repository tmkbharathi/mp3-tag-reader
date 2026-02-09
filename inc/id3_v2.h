#ifndef ID3_V2_H
#define ID3_V2_H

#include "types.h"

// Image metadata
typedef struct {
  char *mime_type;
  uint8_t type;
  char *description;
  uint32_t size;
  unsigned char *data;
} ImageMetadata;

// Struct for ID3v2 tag data (simplified for display)
typedef struct {
  char *title;
  char *artist;
  char *album;
  char *year;
  char *comment;
  char *comment_desc;
  char *lang;
  char *genre;
  char *track;
  int major_version;
  ImageMetadata image;
} ID3v2_Content;

// Function to read ID3v2 tag
Status read_id3v2_tag(const char *filepath, ID3v2_Content *content);
Status write_id3v2_tag(const char *filepath, const TagUpdate *update);
Status remove_id3v2_tag(const char *filepath);
void free_id3v2_content(ID3v2_Content *content);

#endif // ID3_V2_H
