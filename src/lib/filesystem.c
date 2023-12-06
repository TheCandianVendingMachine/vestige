#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <utime.h>

#include "lib/filesystem.h"

time_t get_modify_time(const char* file) {
    struct stat file_data;

    if (stat((const char*)file, &file_data) < 0) {
        perror((const char*)file);
        return 0;
    }
    return file_data.st_mtime;
}

void update_file_modify_time(FileMetaData* file) {
    file->last_modified = time_from_seconds(get_modify_time((const char*)file->file_path.buffer));
}

bool has_file_been_modified(FileMetaData file) {
    return time_as_seconds(file.last_modified) == get_modify_time((const char*)file.file_path.buffer);
}
