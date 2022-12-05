//
// Created by farid on 05.11.22.
//
#include "file_utils.h"

uint64_t
write_into_db(const int32_t fd, struct stat *m_stat_buf, const int64_t offset, const void *data, const size_t n) {
    auto offset1 = lseek(fd, offset, SEEK_SET);
    write(fd, data, n);
    fstat(fd, m_stat_buf);

    return offset1;
}

ssize_t read_from_db(const int32_t fd, const int64_t offset, void *data, const size_t n) {
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

void close_file(int32_t fd) {
    close(fd);
}