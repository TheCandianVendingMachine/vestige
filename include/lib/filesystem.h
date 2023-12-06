#ifndef VESTIGE_FILE_SYSTEM_H
#define VESTIGE_FILE_SYSTEM_H

#include <stdbool.h>

#include "lib/string.h"
#include "lib/time.h"

typedef struct FileMetaData {
    String file_path;
    Time last_modified;
} FileMetaData;

void update_file_modify_time(FileMetaData* file);
bool has_file_been_modified(FileMetaData file);

#endif
