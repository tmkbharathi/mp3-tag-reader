#include "../inc/id3_v1.h"
#include <stdio.h>
#include <string.h>

// Genres list could be added here or in utils

Status read_id3v1_tag(const char *filepath, ID3v1_Tag *tag) {
  FILE *fp = fopen(filepath, "rb");
  if (!fp) {
    return ERROR_FILE_OPEN;
  }

  // Seek to end - 128 bytes
  if (fseek(fp, -128, SEEK_END) != 0) {
    fclose(fp);
    return ERROR_INVALID_FORMAT; // File likely too small
  }

  char buffer[128];
  if (fread(buffer, 1, 128, fp) != 128) {
    fclose(fp);
    return ERROR_INVALID_FORMAT;
  }
  fclose(fp);

  // Check for "TAG"
  if (strncmp(buffer, "TAG", 3) != 0) {
    return ERROR_TAG_NOT_FOUND;
  }

// Copy data to struct
// We use strncpy but verify null-termination manually or format carefully
// The SRS says empty space is 0-filled, but safe to clamp.

// Helper macro to copy and zero-terminate
#define COPY_FIELD(dest, src, size)                                            \
  memcpy(dest, src, size);                                                     \
  dest[size] = '\0';

  COPY_FIELD(tag->title, buffer + 3, 30);
  COPY_FIELD(tag->artist, buffer + 33, 30);
  COPY_FIELD(tag->album, buffer + 63, 30);
  COPY_FIELD(tag->year, buffer + 93, 4);
  COPY_FIELD(tag->comment, buffer + 97, 30);
  tag->genre = buffer[127];

  // Check for ID3v1.1 (Track number)
  // In v1.1, Comment[28] is NULL and Comment[29] is Track
  // Since we copied 30 bytes to comment, check index 28 (29th byte)
  // buffer[97 + 28] = buffer[125]
  if (buffer[125] == 0 && buffer[126] != 0) {
    // ID3v1.1 detected
    // Modify comment to be just 28 chars (already null terminated if 0)
    // Store track? Struct doesn't have track yet.
    // SRS doesn't explicitly ask for track in "Mandatory" list, but "Display
    // all metadata" suggests yes. For now, leave it as is or add track to
    // struct later.
  }

  return SUCCESS;
}

Status write_id3v1_tag(const char *filepath, const ID3v1_Tag *tag) {
  if (!filepath || !tag)
    return ERROR_INVALID_FORMAT; // Invalid args

  FILE *fp = fopen(filepath, "r+b"); // Read/Update binary
  if (!fp)
    return ERROR_FILE_OPEN;

  // Check if file has existing tag
  fseek(fp, 0, SEEK_END);
  long filesize = ftell(fp);
  int has_tag = 0;

  if (filesize >= 128) {
    fseek(fp, -128, SEEK_END);
    char header[3];
    if (fread(header, 1, 3, fp) == 3 && strncmp(header, "TAG", 3) == 0) {
      has_tag = 1;
    }
  }

  // Prepare buffer
  char buffer[128];
  memset(buffer, 0, 128);
  strncpy(buffer, "TAG", 3);

  // Helper to copy and pad with nulls/spaces (spec says 0-filled usually, but
  // space is common too. we use 0) strncpy pads with 0 if src is shorter than
  // n.
  strncpy(buffer + 3, tag->title, 30);
  strncpy(buffer + 33, tag->artist, 30);
  strncpy(buffer + 63, tag->album, 30);
  strncpy(buffer + 93, tag->year, 4);
  strncpy(buffer + 97, tag->comment, 30);
  buffer[127] = tag->genre;

  // Write
  if (has_tag) {
    fseek(fp, -128, SEEK_END);
  } else {
    fseek(fp, 0, SEEK_END); // Append
  }

  if (fwrite(buffer, 1, 128, fp) != 128) {
    fclose(fp);
    return ERROR_FILE_OPEN; // Write error
  }

  fclose(fp);
  return SUCCESS;
}
