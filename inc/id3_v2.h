#ifndef ID3_V2_H
#define ID3_V2_H

#include "types.h"

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
} ID3v2_Content;

// Function to read ID3v2 tag
Status read_id3v2_tag(const char *filepath, ID3v2_Content *content);
void free_id3v2_content(ID3v2_Content *content);

#endif // ID3_V2_H
