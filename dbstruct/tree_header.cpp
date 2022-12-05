//
// Created by farid on 04.12.22.
//

#include "tree_header.h"
#include "../file_workers/file_utils.h"

struct tree_header get_tree_header_from_db(const int32_t fd) {
    tree_header header = { };
    read_from_db(fd, 0, &header, sizeof(header));

    return header;
}

struct stat set_tree_header_to_db(const int32_t fd, tree_header header) {
    struct stat m_stat_buf = {};
    write_into_db(fd, &m_stat_buf, 0, &header, sizeof(header));

    return m_stat_buf;
}
