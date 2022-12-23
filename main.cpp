#include <iostream>
#include "file_workers/file_utils.h"
#include "dbstruct/node.h"

int main() {
    int32_t fd = open_file("text.txt");

    char as[3];
    as[0] = 'a';
    as[1] = 'a';
    as[2] = 'a';

    auto* m_stat = (struct stat *) malloc(sizeof(struct stat));
    uint64_t s = write_into_db(fd, m_stat, 0, as, 3);
    as[0] = 'b';
    as[1] = 'b';
    as[2] = 'b';
    s = write_into_db(fd, m_stat, 3, as, 3);
    printf("%ld\n", m_stat->st_size);
    s = write_into_db(fd, m_stat, m_stat->st_size, as, 3);
    printf("%ld\n", m_stat->st_size);

    void* data[m_stat->st_size];
    auto b = read_from_db(fd, 0, data, m_stat->st_size);

    printf("%s\n", (char*) data) ;

    close_file(fd);

    struct node n = { };
    printf("%lu", sizeof(n)) ;

//    нужно сначала проверить что у базы данных есть хедер если есть,
//    то читаем и работаем, если нет,
//    то создаем хедер и требуемую схему
//
//
    return 0;
}
