#ifndef LLP_DATABASE_SERIALIZATION_H
#define LLP_DATABASE_SERIALIZATION_H

#include <boost/serialization/unordered_map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <fstream>
#include "tree_header.h"
#include "../file_workers/file_utils.h"

std::unordered_map<std::string, data_type> deserialize_file_header_schema(int32_t fd, int64_t header_size) {
    auto size = header_size - offsetof(tree_header, schema);
    auto buff = malloc(size);
    auto cfd = open_file(".cache");

    read_from_file(fd, offsetof(tree_header, schema), buff, size);
    write_into_file(cfd, 0, buff, size); // записываем десериалиуемый контейнер из бд в кэш
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

size_t get_cache_size(const int32_t cached_fd) {
    struct stat64 cache_file_stat = {0};
    fstat64(cached_fd, &cache_file_stat);

    return cache_file_stat.st_size;
}

void move_from_cache_to_db(int32_t fd, int32_t cached_fd, int64_t file_offset) {
    auto cfd_size = get_cache_size(cached_fd);
    void *buff = malloc(cfd_size);
    read_from_file(cached_fd, 0, buff, cfd_size);
    write_into_file(fd, file_offset, buff, cfd_size); // записываем сериализованную схему в бд
    free(buff);
}

std::unordered_map<std::string, std::string> deserialize_node_data(int32_t fd, int64_t node_offset, int64_t node_size) {
    auto offset = node_offset + offsetof(node, data);
    auto size = node_size - offsetof(node, data);
    auto buff = malloc(size);
    auto cfd = open_file(".cache");

    read_from_file(fd, offset, buff, size);
    write_into_file(cfd, offset, buff, size); // записываем десериалиуемый контейнер из бд в кэш
    std::ifstream ifs(".cache"); // десериализуем
    boost::archive::text_iarchive ia(ifs);
    std::unordered_map<std::string, std::string> map = {};
    ia >> map;
    ifs.close();
    free(buff);

    return map;
}

void serialize_and_cache_node_data(std::unordered_map<std::string, std::string> &name_to_type) {
    std::ofstream ofs(".cache"); // сериализуем
    boost::archive::text_oarchive oa(ofs);
    oa << name_to_type; // записываем сериализованные данные в кэш
    ofs.close();
}
#endif //LLP_DATABASE_SERIALIZATION_H
