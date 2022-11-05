#include <iostream>
#include "file_workers/file_utils.h"
#include "dbstruct/file_data.h"

int main() {
    int32_t fd = open_file("text.txt");

    char as[3];
    as[0] = 'a';
    as[1] = 'a';
    as[2] = 'a';

    void* data[5];
//    auto s = read_from_db(fd, 0, data, 5);
    struct stat* m_stat;
    uint64_t s = write_into_db(fd, m_stat, 0, as, 3);
    as[0] = 'b';
    as[1] = 'b';
    as[2] = 'b';
    s = write_into_db(fd, m_stat, 3, as, 3);
    printf("%d", m_stat->st_size);
    s = write_into_db(fd, m_stat, m_stat->st_size, as, 3);
    printf("%d", m_stat->st_size);

    close_file(fd);

    return 0;
}
