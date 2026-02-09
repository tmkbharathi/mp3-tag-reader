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
    printf("Time: %02d:%02d  %s  %s  %d kb/s  %d Hz  %s\n", min, sec,
           mpeg_info.version, mpeg_info.layer, mpeg_info.bitrate,
           mpeg_info.sample_rate, mpeg_info.mode);
  }

  printf("------------------------------------------------------------\n");

  // Line 4: id3 version
  ID3v2_Content v2_content;
  memset(&v2_content, 0, sizeof(ID3v2_Content));
  Status v2_status = read_id3v2_tag(filepath, &v2_content);

  ID3v1_Tag v1_tag;
  memset(&v1_tag, 0, sizeof(ID3v1_Tag));
  Status v1_status = read_id3v1_tag(filepath, &v1_tag);

  if (v2_status == SUCCESS) {
    printf("ID3 v2.%d:\n", v2_content.major_version);
    // Line 5: title, artist
    printf("title: %s       artist: %s\n",
           v2_content.title ? v2_content.title : "",
           v2_content.artist ? v2_content.artist : "");

    // Line 6: album, year (conditionally show year)
    printf("album: %s", v2_content.album ? v2_content.album : "");
    if (v2_content.year)
      printf("  year: %s", v2_content.year);
    printf("\n");

    // Line 7: track, genre (conditionally show track)
    if (v2_content.track)
      printf("track: %s   ", v2_content.track);
    printf("genre: %s\n", v2_content.genre ? v2_content.genre : "");

    // Image details if present
    if (v2_content.image.size > 0) {
      printf("Image: [Type: %d] [Mime: %s] [Size: %u bytes]\n",
             v2_content.image.type,
             v2_content.image.mime_type ? v2_content.image.mime_type
                                        : "unknown",
             v2_content.image.size);
      if (v2_content.image.description &&
          strlen(v2_content.image.description) > 0)
        printf("       Description: %s\n", v2_content.image.description);
    }

    // Line 8: comment
    if (v2_content.comment) {
      if (v2_content.comment_desc || v2_content.lang) {
        printf("Comment: [Description: %s] [Lang: %s]\n",
               v2_content.comment_desc ? v2_content.comment_desc : "",
               v2_content.lang ? v2_content.lang : "");
      } else {
        printf("Comment: \n");
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
  return SUCCESS;
}

Status update_id3_tags(const char *filepath, const TagUpdate *update) {
  printf("Updating tags for file: %s\n", filepath);
  printf("----------------------------------------\n");

  // Update ID3v1
  ID3v1_Tag tag;
  memset(&tag, 0, sizeof(ID3v1_Tag));
  Status v1_read = read_id3v1_tag(filepath, &tag);

  if (v1_read == SUCCESS || v1_read == ERROR_TAG_NOT_FOUND) {
    if (v1_read == ERROR_TAG_NOT_FOUND) {
      tag.genre = 12; // Other
    }
    if (update->title)
      strncpy(tag.title, update->title, 30);
    if (update->artist)
      strncpy(tag.artist, update->artist, 30);
    if (update->album)
      strncpy(tag.album, update->album, 30);
    if (update->year)
      strncpy(tag.year, update->year, 4);
    if (update->comment)
      strncpy(tag.comment, update->comment, 30);
    if (update->genre)
      tag.genre = (uint8_t)atoi(update->genre);

    write_id3v1_tag(filepath, &tag);
    printf("  ID3v1 updated.\n");
  }

  // Update ID3v2
  Status v2_write = write_id3v2_tag(filepath, update);
  if (v2_write == SUCCESS) {
    printf("  ID3v2 updated successfully.\n");
  } else {
    printf("  Error updating ID3v2: %d\n", v2_write);
  }

  printf("----------------------------------------\n");
  return v2_write;
}

Status delete_id3_tags(const char *filepath) {
  printf("Deleting tags from: %s\n", filepath);
  remove_id3v1_tag(filepath);
  remove_id3v2_tag(filepath);
  printf("Tags deleted.\n");
  return SUCCESS;
}
