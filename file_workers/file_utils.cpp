#include "file_utils.h"

uint64_t
write_into_file(const int32_t fd, const int64_t offset, const void *data, const size_t n) {
    auto offset1 = lseek(fd, offset, SEEK_SET);
    write(fd, data, n);
    struct stat64 _{};
    fstat64(fd, &_);

    return offset1;
}

ssize_t read_from_file(const int32_t fd, const int64_t offset, void *data, const size_t n) {
    lseek(fd, offset, SEEK_SET);

    return read(fd, data, n);
}

int32_t open_file(const char *filename) {
    int32_t fd = open(filename, O_RDWR | O_CREAT, S_IWRITE | S_IREAD);
    if (fd == -1) {
        printf("Cannot open file. Errno: %d\n ", errno);
    }

    return fd;
}

size_t get_file_size(const int32_t fd) {
    struct stat64 file_stat = {0}; // TODO возможно, придется переделывать под windows
    if (fstat64(fd, &file_stat) != 0) {
        printf("Cannot get file stat. Errno: %d\n ", errno);
    }

    return file_stat.st_size;
}

void close_file(int32_t fd) {
    close(fd);
}