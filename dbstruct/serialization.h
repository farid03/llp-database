#ifndef LLP_DATABASE_SERIALIZATION_H
#define LLP_DATABASE_SERIALIZATION_H

#include <boost/serialization/unordered_map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <fstream>
#include "tree_header.h"
#include "../file_workers/file_utils.h"

std::unordered_map<std::string, data_type> deserialize_file_header_schema(int32_t fd, int64_t header_size) {
    auto buff = malloc(header_size - offsetof(tree_header, schema));
    auto cfd = open_file(".cache");
    read_from_file(fd, offsetof(tree_header, schema), buff, header_size - offsetof(tree_header, schema));
    write_into_file(cfd, 0, buff, header_size - offsetof(tree_header, schema)); // зачем
    std::ifstream ifs(".cache"); // десериализуем
    boost::archive::text_iarchive ia(ifs);
    std::unordered_map<std::string, data_type> map = {};
    ia >> map;
    ifs.close();
    free(buff);

    return map;
}

void serialize_and_cache_file_header(std::unordered_map<std::string, data_type> &name_to_type) {
    std::ofstream ofs(".cache"); // сериализуем
    boost::archive::text_oarchive oa(ofs);
    oa << name_to_type; // записываем сериализованные данные в кэш
    ofs.close();
}

size_t get_cache_size(int32_t cached_fd) {
    struct stat64 cache_file_stat = {0};
    fstat64(cached_fd, &cache_file_stat);

    return cache_file_stat.st_size;
}

#endif //LLP_DATABASE_SERIALIZATION_H
