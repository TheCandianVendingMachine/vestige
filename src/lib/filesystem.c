#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <utime.h>
#include <errno.h>

#include "lib/filesystem.h"
#include "logger.h"

time_t get_modify_time(const char* file) {
    struct stat file_data;

    if (stat((const char*)file, &file_data) < 0) {
        log_warning("%s: %s", file, strerror(errno));
        return 0;
    }
    return file_data.st_mtime;
}

FileMetaData file_meta_data(const char* file) {
    return (FileMetaData) {
        .file_path = string_from_cstr((char*)file),
        .last_modified = time_from_milliseconds(1000 * get_modify_time(file))
    };
}

void destroy_file_meta_data(FileMetaData* file) {
    del_string(file->file_path);
}

void update_file_modify_time(FileMetaData* file) {
    const char* file_str = cstr_from_string(file->file_path);
    file->last_modified = time_from_milliseconds(1000 * get_modify_time(file_str));
    free((char*)file_str);
}

bool has_file_been_modified(FileMetaData file) {
    const char* file_str = cstr_from_string(file.file_path);
    time_t modify_time = get_modify_time(file_str);
    free((char*)file_str);
    Time t = time_from_milliseconds(modify_time);
    log_debug("%d %d %d %d", modify_time, time_as_seconds(file.last_modified), t._seconds, t._nanoseconds);
    return time_as_seconds(file.last_modified) != modify_time;
}
