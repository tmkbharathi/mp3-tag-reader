#include "../inc/id3_reader.h"
#include "../inc/types.h"
#include <stdio.h>
#include <string.h>

void print_help(const char *program_name) {
  printf("Usage: %s [OPTIONS] <file.mp3>\n", program_name);
  printf("Options:\n");
  printf("  -t <title>    Modifies a Title tag\n");
  printf("  -a <artist>   Modifies an Artist tag\n");
  printf("  -A <album>    Modifies an Album tag\n");
  printf("  -y <year>     Modifies a Year tag\n");
  printf("  -c <comment>  Modifies a Comment tag\n");
  printf("  -g <genre>    Modifies a Genre tag\n");
  printf("  -h, --help    Displays this help info\n");
  printf("  -v            Prints version info\n");
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

  int i;
  for (i = 1; i < argc; i++) {
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

    update_id3_tags(filepath, &update);
  } else {
    // View mode
    read_id3_tags(filepath);
  }

  return 0;
}
