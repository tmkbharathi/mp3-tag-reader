#include "../inc/id3_reader.h"
#include "../inc/id3_v1.h"
#include "../inc/id3_v2.h"
#include "../inc/mpeg_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Status read_id3_tags(const char *filepath) {
  // Line 1: [filename] [size]
  MpegInfo mpeg_info;
  memset(&mpeg_info, 0, sizeof(MpegInfo));
  read_mpeg_info(filepath, &mpeg_info);

  printf("%s %.2f MB\n", filepath, (double)mpeg_info.filesize / (1024 * 1024));

  // Line 2: horizontal dot line
  printf("------------------------------------------------------------\n");

  // Line 3: time, mpeg1, layer 111, speed freequeny steoreos
  if (mpeg_info.bitrate > 0) {
    int min = (int)mpeg_info.duration / 60;
    int sec = (int)mpeg_info.duration % 60;
    printf("Tine: %02d:%02d, %s, %s, %d kb/s, %d Hz, %s\n", min, sec,
           mpeg_info.version, mpeg_info.layer, mpeg_info.bitrate,
           mpeg_info.sample_rate, mpeg_info.mode);
  }

  // Line 4: id3 version
  ID3v2_Content v2_content;
  memset(&v2_content, 0, sizeof(ID3v2_Content));
  Status v2_status = read_id3v2_tag(filepath, &v2_content);

  ID3v1_Tag v1_tag;
  memset(&v1_tag, 0, sizeof(ID3v1_Tag));
  Status v1_status = read_id3v1_tag(filepath, &v1_tag);

  if (v2_status == SUCCESS) {
    printf("ID3 v2.3:\n");
    // Line 5: title, artist
    printf("title: %s, artist: %s\n", v2_content.title ? v2_content.title : "",
           v2_content.artist ? v2_content.artist : "");

    // Line 6: album, year (conditionally show year)
    printf("albun: %s", v2_content.album ? v2_content.album : "");
    if (v2_content.year)
      printf(", year: %s", v2_content.year);
    printf("\n");

    // Line 7: track, genre (conditionally show track)
    if (v2_content.track)
      printf("track: %s, ", v2_content.track);
    printf("genre: %s\n", v2_content.genre ? v2_content.genre : "");

    // Line 8: comment
    if (v2_content.comment) {
      if (v2_content.comment_desc || v2_content.lang) {
        printf("Connent: [Description: %s] [Lang: %s]\n",
               v2_content.comment_desc ? v2_content.comment_desc : "",
               v2_content.lang ? v2_content.lang : "");
      } else {
        printf("Connent: \n");
      }
      printf("%s\n", v2_content.comment);
    }
    free_id3v2_content(&v2_content);
  } else if (v1_status == SUCCESS) {
    printf("ID3 v1.1:\n");
    printf("title: %s, artist: %s\n", v1_tag.title, v1_tag.artist);
    printf("albun: %s, year: %s\n", v1_tag.album, v1_tag.year);
    printf("track: , genre: %d\n", v1_tag.genre);
    printf("Connent: %s\n", v1_tag.comment);
  } else {
    printf("No ID3 tags found.\n");
  }

  printf("------------------------------------------------------------\n");
  return SUCCESS;
}

Status update_id3_tags(const char *filepath, const TagUpdate *update) {
  printf("Updating tags for file: %s\n", filepath);
  printf("----------------------------------------\n");

  ID3v1_Tag tag;
  memset(&tag, 0, sizeof(ID3v1_Tag));

  // Try to read existing tag
  Status status = read_id3v1_tag(filepath, &tag);
  if (status != SUCCESS && status != ERROR_TAG_NOT_FOUND) {
    printf("Error: Could not access file or file error.\n");
    return status;
  }

  if (status == ERROR_TAG_NOT_FOUND) {
    printf("No existing ID3v1 tag found. Creating new one.\n");
    // tag is already zeroed.
    // Set default genre to 255 (undefined) if new?
    // Or 12 (Other)? Spec says 12 is 'Other'.
    tag.genre = 12;
  }

  // Update fields if provided
  if (update->title) {
    strncpy(tag.title, update->title, 30);
    printf("  Set Title: %s\n", update->title);
  }
  if (update->artist) {
    strncpy(tag.artist, update->artist, 30);
    printf("  Set Artist: %s\n", update->artist);
  }
  if (update->album) {
    strncpy(tag.album, update->album, 30);
    printf("  Set Album: %s\n", update->album);
  }
  if (update->year) {
    strncpy(tag.year, update->year, 4);
    printf("  Set Year: %s\n", update->year);
  }
  if (update->comment) {
    strncpy(tag.comment, update->comment, 30);
    printf("  Set Comment: %s\n", update->comment);
  }
  if (update->genre) {
    // Basic parsing: if number, use it. If string, simple mapping or default.
    // For now, assume integer input for simplicity or default 12.
    // TODO: Implement string-to-genre mapping.
    int g = atoi(update->genre);
    if (g >= 0 && g <= 255) {
      tag.genre = (uint8_t)g;
      printf("  Set Genre ID: %d\n", g);
    } else {
      printf("  Warning: Invalid genre ID '%s'. Ignored.\n", update->genre);
    }
  }

  // Write back
  Status write_status = write_id3v1_tag(filepath, &tag);
  if (write_status == SUCCESS) {
    printf("Tags updated successfully.\n");
  } else {
    printf("Error writing tags.\n");
  }

  printf("----------------------------------------\n");
  return write_status;
}
