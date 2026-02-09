#ifndef ID3_V1_H
#define ID3_V1_H

#include "types.h"
#include <stdio.h>

/*
    ID3v1 structure: 128 bytes
    TAG (3)
    Title (30)
    Artist (30)
    Album (30)
    Year (4)
    Comment (30) - Or 28 + 0 + Track for v1.1
    Genre (1)
*/

// Function to check if ID3v1 tag exists and read it
Status read_id3v1_tag(const char *filepath, ID3v1_Tag *tag);
Status write_id3v1_tag(const char *filepath, const ID3v1_Tag *tag);
Status remove_id3v1_tag(const char *filepath);

#endif // ID3_V1_H
