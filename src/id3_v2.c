#include "../inc/id3_v2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper to decode synchsafe integer (4 bytes, 7 bits each)
static int decode_synchsafe(unsigned char *bytes) {
  return (bytes[0] << 21) | (bytes[1] << 14) | (bytes[2] << 7) | bytes[3];
}

// Helper to read integer (4 bytes, 8 bits each)
static int decode_int(unsigned char *bytes) {
  return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

// Sanitizer to handle UTF-16 (strip nulls) and BOM
static char *sanitize_string(const char *raw, int len, int encoding) {
  if (!raw || len <= 0)
    return NULL;

  char *clean = (char *)malloc(len + 1);
  if (!clean)
    return NULL;

  int j = 0;
  int start = 0;
  if (encoding == 1 && len >= 2) {
    if (((unsigned char)raw[0] == 0xFF && (unsigned char)raw[1] == 0xFE) ||
        ((unsigned char)raw[0] == 0xFE && (unsigned char)raw[1] == 0xFF)) {
      start = 2;
    }
  }

  for (int i = start; i < len; i++) {
    unsigned char c = (unsigned char)raw[i];
    if (c == 0)
      continue;
    if (c < 32 && c != '\n' && c != '\r' && c != '\t')
      continue;
    clean[j++] = c;
  }
  clean[j] = '\0';

  while (j > 0 && clean[j - 1] == ' ') {
    clean[--j] = '\0';
  }

  if (j == 0) {
    free(clean);
    return NULL;
  }
  return clean;
}

void free_id3v2_content(ID3v2_Content *content) {
  if (content->title)
    free(content->title);
  if (content->artist)
    free(content->artist);
  if (content->album)
    free(content->album);
  if (content->year)
    free(content->year);
  if (content->comment)
    free(content->comment);
  if (content->comment_desc)
    free(content->comment_desc);
  if (content->lang)
    free(content->lang);
  if (content->genre)
    free(content->genre);
  if (content->track)
    free(content->track);
  memset(content, 0, sizeof(ID3v2_Content));
}

Status read_id3v2_tag(const char *filepath, ID3v2_Content *content) {
  if (!filepath || !content)
    return ERROR_INVALID_FORMAT;
  FILE *fp = fopen(filepath, "rb");
  if (!fp)
    return ERROR_FILE_OPEN;

  unsigned char header[10];
  if (fread(header, 1, 10, fp) != 10) {
    fclose(fp);
    return ERROR_INVALID_FORMAT;
  }

  // Check ID3 identifier
  if (strncmp((char *)header, "ID3", 3) != 0) {
    fclose(fp);
    return ERROR_TAG_NOT_FOUND;
  }

  // Check version (we support v2.3 mostly, v2.4 frames structure is similar but
  // footer/syncsafe differs slightly)
  int major_version = header[3];
  if (major_version < 3) {
    fclose(fp);
    // TODO: Support v2.2 (3 letter frames) if needed.
    return ERROR_INVALID_FORMAT;
  }

  // Flags
  // int flags = header[5];

  // Size
  int tag_size = decode_synchsafe(&header[6]);

  // Read frames
  long start_pos = 10;
  long end_pos = start_pos + tag_size;

  while (ftell(fp) < end_pos) {
    unsigned char frame_header[10];
    if (fread(frame_header, 1, 10, fp) != 10)
      break;

    // Check for padding (null bytes)
    if (frame_header[0] == 0)
      break;

    char frame_id[5];
    memcpy(frame_id, frame_header, 4);
    frame_id[4] = '\0';

    int frame_size = decode_int(
        &frame_header[4]); // v2.3 uses regular integer for frame size?
    // Spec v2.3: "Frame size: $xx xx xx xx" (32 bit integer). Not synchsafe.
    // v2.4 uses synchsafe. Assume v2.3 for now.

    // Safety check size
    if (frame_size <= 0 || frame_size > tag_size) {
      // Invalid frame size, abort
      break;
    }

    // Read encoding byte
    unsigned char encoding;
    if (fread(&encoding, 1, 1, fp) != 1)
      break;

    int data_len = frame_size - 1;
    char *data = (char *)malloc(data_len + 1);
    if (!data)
      break;

    if (fread(data, 1, data_len, fp) != (size_t)data_len) {
      free(data);
      break;
    }
    data[data_len] = '\0'; // Null terminate

    // Handle Encoding (0 = ISO-8859-1, 1 = UTF-16 with BOM)
    // For simplicity, treat as string. If UTF-16, we might see null bytes.
    // Ideally convert. For this implementation, we assume basic ASCII/ISO or
    // print raw. If encoding == 1, skip BOM (2 bytes) if present.
    char *text_ptr = data;
    if (encoding == 1 && data_len > 2) {
      if ((unsigned char)data[0] == 0xFF && (unsigned char)data[1] == 0xFE)
        text_ptr += 2;
      else if ((unsigned char)data[0] == 0xFE && (unsigned char)data[1] == 0xFF)
        text_ptr += 2;
      // Also UTF-16 uses 2 bytes per char. Naive printing won't work well
      // without conversion. But user output is English "Sunny Sunny". Often
      // encoded as ISO-8859-1.
    }

    // Assign to content struct using sanitizer
    if (strcmp(frame_id, "TIT2") == 0) {
      if (content->title)
        free(content->title);
      content->title = sanitize_string(data, data_len, encoding);
    } else if (strcmp(frame_id, "TPE1") == 0) {
      if (content->artist)
        free(content->artist);
      content->artist = sanitize_string(data, data_len, encoding);
    } else if (strcmp(frame_id, "TALB") == 0) {
      if (content->album)
        free(content->album);
      content->album = sanitize_string(data, data_len, encoding);
    } else if (strcmp(frame_id, "TYER") == 0 || strcmp(frame_id, "TDRC") == 0) {
      if (content->year)
        free(content->year);
      content->year = sanitize_string(data, data_len, encoding);
    } else if (strcmp(frame_id, "TRCK") == 0) {
      if (content->track)
        free(content->track);
      content->track = sanitize_string(data, data_len, encoding);
    } else if (strcmp(frame_id, "COMM") == 0) {
      // COMM frame: Lang(3) + Desc(var) + 00 + Text(var)
      // (Encoding byte was already skipped)
      if (data_len >= 3) {
        char lang[4];
        memcpy(lang, data, 3);
        lang[3] = '\0';
        if (content->lang)
          free(content->lang);
        content->lang = strdup(lang);

        // Scan for null terminator of description (depends on encoding)
        int step = (encoding == 1 || encoding == 2) ? 2 : 1;
        int d_end = 3;
        while (d_end < data_len) {
          if (data[d_end] == 0 &&
              (step == 1 || (d_end + 1 < data_len && data[d_end + 1] == 0)))
            break;
          d_end += step;
        }

        char *desc = sanitize_string(data + 3, d_end - 3, encoding);
        if (desc) {
          if (content->comment_desc)
            free(content->comment_desc);
          content->comment_desc = desc;
        }

        int text_start = d_end + step;
        if (text_start < data_len) {
          char *comment = sanitize_string(data + text_start,
                                          data_len - text_start, encoding);
          if (comment) {
            if (content->comment)
              free(content->comment);
            content->comment = comment;
          }
        }
      }
    } else if (strcmp(frame_id, "TCON") == 0) {
      if (content->genre)
        free(content->genre);
      content->genre = sanitize_string(data, data_len, encoding);
    }

    free(data);
  }

  fclose(fp);
  return SUCCESS;
}
