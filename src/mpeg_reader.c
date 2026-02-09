#include "../inc/mpeg_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Bitrate tables
static const int bitrate_v1_l1[] = {0,   32,  64,  96,  128, 160, 192, 224,
                                    256, 288, 320, 352, 384, 416, 448};
static const int bitrate_v1_l2[] = {0,   32,  48,  56,  64,  80,  96, 112,
                                    128, 160, 192, 224, 256, 320, 384};
static const int bitrate_v1_l3[] = {0,   32,  40,  48,  56,  64,  80, 96,
                                    112, 128, 160, 192, 224, 256, 320};
static const int bitrate_v2_l1[] = {0,   32,  48,  56,  64,  80,  96, 112,
                                    128, 144, 160, 176, 192, 224, 256};
static const int bitrate_v2_l23[] = {0,  8,  16, 24,  32,  40,  48, 56,
                                     64, 80, 96, 112, 128, 144, 160};

static const int samplerate_v1[] = {44100, 48000, 32000};
static const int samplerate_v2[] = {22050, 24000, 16000};
static const int samplerate_v25[] = {11025, 12000, 8000};

Status read_mpeg_info(const char *filepath, MpegInfo *info) {
  if (!filepath || !info)
    return ERROR_INVALID_FORMAT;
  FILE *fp = fopen(filepath, "rb");
  if (!fp)
    return ERROR_FILE_OPEN;

  // Get file size
  fseek(fp, 0, SEEK_END);
  info->filesize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  int frames_found = 0;

  // Limit search to first 100KB to avoid scanning whole file if not found
  long search_limit = 100 * 1024;
  if (info->filesize < search_limit)
    search_limit = info->filesize;

  // Read up to search_limit into a buffer
  // Just alloc 100KB or file size
  unsigned char *search_buf = (unsigned char *)malloc(search_limit);
  if (!search_buf) {
    fclose(fp);
    return ERROR_MEM_ALLOC;
  }

  size_t bytes_read = fread(search_buf, 1, search_limit, fp);
  // If read less, proceed with what we got (e.g. file smaller than limit)
  // But search_limit was capped by filesize. if read failed, error.
  // unless fread returns short count? Just use ftell to know actual bytes read
  // or assume success if limit correct. Actually fread might return less if
  // EOF.

  fclose(fp); // Close early, we have data.

  for (size_t i = 0; i + 4 < bytes_read; i++) {
    // Sync word
    if (search_buf[i] == 0xFF && (search_buf[i + 1] & 0xE0) == 0xE0) {
      // Found sync at offset i

      // Parse Version
      int ver_bits = (search_buf[i + 1] >> 3) & 0x03;
      int layer_bits = (search_buf[i + 1] >> 1) & 0x03;

      if (ver_bits == 0x01)
        continue;
      if (layer_bits == 0x00)
        continue;

      // Decode Version
      if (ver_bits == 3) {
        strcpy(info->version, "MPEG 1");
      } else if (ver_bits == 2) {
        strcpy(info->version, "MPEG 2");
      } else {
        strcpy(info->version, "MPEG 2.5");
      }

      // Decode Layer
      int layer_idx = 0;
      if (layer_bits == 3) {
        strcpy(info->layer, "Layer I");
        layer_idx = 1;
      } else if (layer_bits == 2) {
        strcpy(info->layer, "Layer II");
        layer_idx = 2;
      } else {
        strcpy(info->layer, "Layer III");
        layer_idx = 3;
      }

      // Bitrate
      int br_idx = (search_buf[i + 2] >> 4) & 0x0F;
      if (br_idx == 0 || br_idx == 15)
        continue;

      int bitrate = 0;
      if (ver_bits == 3) { // V1
        if (layer_idx == 1)
          bitrate = bitrate_v1_l1[br_idx];
        else if (layer_idx == 2)
          bitrate = bitrate_v1_l2[br_idx];
        else
          bitrate = bitrate_v1_l3[br_idx];
      } else { // V2 or 2.5
        if (layer_idx == 1)
          bitrate = bitrate_v2_l1[br_idx];
        else
          bitrate = bitrate_v2_l23[br_idx];
      }
      info->bitrate = bitrate;

      // Sample Rate
      int sr_idx = (search_buf[i + 2] >> 2) & 0x03;
      if (sr_idx == 3)
        continue;

      if (ver_bits == 3)
        info->sample_rate = samplerate_v1[sr_idx];
      else if (ver_bits == 2)
        info->sample_rate = samplerate_v2[sr_idx];
      else
        info->sample_rate = samplerate_v25[sr_idx];

      // Mode
      int mode_bits = (search_buf[i + 3] >> 6) & 0x03;
      switch (mode_bits) {
      case 0:
        strcpy(info->mode, "Stereo");
        break;
      case 1:
        strcpy(info->mode, "Joint Stereo");
        break;
      case 2:
        strcpy(info->mode, "Dual Channel");
        break;
      case 3:
        strcpy(info->mode, "Single Channel");
        break;
      }

      // Calculate Duration (Approx CBR)
      if (bitrate > 0) {
        long audio_size = info->filesize - 128;
        info->duration = (double)audio_size * 8.0 / (bitrate * 1000.0);
      }

      frames_found = 1;
      break;
    }
  }

  free(search_buf);
  if (frames_found)
    return SUCCESS;
  return ERROR_INVALID_FORMAT;
}
