#include "../inc/id3_reader.h"
#include "../inc/id3_v2.h"
#include "../inc/types.h"
#include <stdio.h>
#include <string.h>

void print_help(const char *program_name) {
  printf("usage: %s -[tTaAycg] \"value\" file1\n", program_name);
  printf("usage: %s -v\n", program_name);
  printf("-t\tModifies a Title tag\n");
  printf("-T\tModifies a Track tag\n");
  printf("-a\tModifies an Artist tag\n");
  printf("-A\tModifies an Album tag\n");
  printf("-y\tModifies a Year tag\n");
  printf("-c\tModifies a Comment tag\n");
  printf("-g\tModifies a Genre tag\n");
  printf("-h\tDisplays this help info\n");
  printf("-v\tPrints version info\n");
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    print_help(argv[0]);
    return 1;
  }

  // Simple argument parsing
  // Check for singular flags first
  if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
    print_help(argv[0]);
    return 0;
  }
  if (strcmp(argv[1], "-v") == 0 && argc == 2) {
    printf("mp3tag v1.0.0\n");
    return 0;
  }

  // Parse arguments
  char *filepath = NULL;
  char *title = NULL;
  char *artist = NULL;
  char *album = NULL;
  char *year = NULL;
  char *comment = NULL;
  char *genre = NULL;
  char *track = NULL;

  int extract_image = 0;
  int delete_tags = 0;
  int i;
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--delete-tag") == 0) {
      delete_tags = 1;
      continue;
    }
    if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--extract-image") == 0) {
      extract_image = 1;
      continue;
    }
    if (argv[i][0] == '-') {
      // It's a flag
      char flag = argv[i][1];
      if (flag == '-')
        continue; // Skip -- flags if any remaining

      if (flag == 'v') {
        printf("mp3tag v1.0.0\n");
        continue;
      }

      if (i + 1 >= argc) {
        printf("Error: Missing value for option -%c\n", flag);
        return 1;
      }

      char *value = argv[i + 1];
      i++; // Skip value

      switch (flag) {
      case 't':
        title = value;
        break;
      case 'a':
        artist = value;
        break;
      case 'A':
        album = value;
        break;
      case 'y':
        year = value;
        break;
      case 'c':
        comment = value;
        break;
      case 'g':
        genre = value;
        break;
      case 'T':
        track = value;
        break;
      default:
        printf("Unknown option: -%c\n", flag);
        print_help(argv[0]);
        return 1;
      }
    } else {
      // It's likely the filename
      filepath = argv[i];
    }
  }

  if (filepath == NULL) {
    printf("Error: No file specified.\n");
    print_help(argv[0]);
    return 1;
  }

  // Dispatch
  if (title || artist || album || year || comment || genre) {
    TagUpdate update;
    update.title = title;
    update.artist = artist;
    update.album = album;
    update.year = year;
    update.comment = comment;
    update.genre = genre;
    update.track = track;

    update_id3_tags(filepath, &update);
  } else if (delete_tags) {
    delete_id3_tags(filepath);
  } else if (extract_image) {
    ID3v2_Content content;
    memset(&content, 0, sizeof(ID3v2_Content));
    if (read_id3v2_tag(filepath, &content) == SUCCESS &&
        content.image.size > 0) {
      const char *ext = ".bin";
      if (content.image.mime_type) {
        if (strstr(content.image.mime_type, "jpeg") ||
            strstr(content.image.mime_type, "jpg"))
          ext = ".jpg";
        else if (strstr(content.image.mime_type, "png"))
          ext = ".png";
      }

      char out_name[64];
      sprintf(out_name, "album_art%s", ext);

      FILE *img = fopen(out_name, "wb");
      if (img) {
        fwrite(content.image.data, 1, content.image.size, img);
        fclose(img);
        printf("Album art extracted to '%s' (%u bytes)\n", out_name,
               content.image.size);
      } else {
        printf("Error: Could not create output image file.\n");
      }
    } else {
      printf("No embedded image found to extract.\n");
    }
    free_id3v2_content(&content);
  } else {
    // View mode
    read_id3_tags(filepath);
  }

  return 0;
}
