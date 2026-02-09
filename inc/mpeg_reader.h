#ifndef MPEG_READER_H
#define MPEG_READER_H

#include "types.h"

typedef struct {
  char version[10]; // e.g., "MPEG 1"
  char layer[10];   // e.g., "Layer III"
  int bitrate;      // kbps
  int sample_rate;  // Hz
  char mode[16];    // e.g., "Joint Stereo"
  double duration;  // Seconds
  long filesize;    // Bytes
} MpegInfo;

// Function to read MPEG header and calculate info
Status read_mpeg_info(const char *filepath, MpegInfo *info);

#endif // MPEG_READER_H
