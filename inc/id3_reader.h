#ifndef ID3_READER_H
#define ID3_READER_H

#include "types.h"

Status read_id3_tags(const char *filepath);
Status update_id3_tags(const char *filepath, const TagUpdate *update);

#endif // ID3_READER_H
