//
// Created by farid on 05.11.22.
//

#include "file_utils.h"

enum file_read_status read_from_file(FILE *file, void *buffer, size_t size) {
    size_t length = fread(buffer, size, 1, file);
    enum file_read_status code = READ_OK;
    if (length < 1) {
        code = READ_INVALID;
    } else if (sizeof(buffer) < length) {
        code = READ_END_OF_FILE;
    }
    return code;
}

enum file_write_status write_to_file(FILE *file, void *buffer, size_t size) {
    size_t length = fwrite(buffer, size, 1, file);
    enum file_write_status code = WRITE_OK;
    if (length < 1) {
        code = WRITE_WRONG_INTEGRITY;
    } else if (sizeof(buffer) < length) {
        code = WRITE_INVALID;
    }
    return code;
}

static enum file_open_status open_file(FILE **file, char *filename, char *open_descriptor){
    *file = fopen(filename, open_descriptor);
    enum file_open_status code = OPEN_OK;
    if (*file==NULL){
        code = OPEN_FAILED;
    }
    return code;
}

enum file_open_status open_exist_file(FILE **file, char *filename){
    return open_file(file, filename, "r+");
}

enum file_open_status open_new_file(FILE **file, char *filename){
    return open_file(file, filename, "w+");
}

void close_file(FILE *file){
    fclose(file);
}

enum file_open_status open_empty_file(FILE **file, char *filename){
    open_file(file, filename, "w");
    close_file(*file);
    return open_file(file, filename, "r+");
}
