#ifndef LLP_DATABASE_SERIALIZATION_H
#define LLP_DATABASE_SERIALIZATION_H

#include <boost/serialization/unordered_map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <fstream>
#include "tree_header.h"
#include "../file_workers/file_utils.h"

/** Десериализует схему данных указанную в заголовочной структуре файла.
 * @param fd - файловый дескриптор
 * @param header_size - полный размер header, с учетом сериализованной схемы
 * @return схема данных упакованная в контейнер в формате map[value_name] = data_type
 */
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

/** Сериализует и кэширует схему данных указанную в контейнере.
 * @param name_to_type - схема данных в формате map[value_name] = data_type
 * @param header_size - полный размер header, с учетом сериализованной схемы
 */
void serialize_and_cache_file_header(std::unordered_map<std::string, data_type> &name_to_type) {
    std::ofstream ofs(".cache"); // сериализуем
    boost::archive::text_oarchive oa(ofs);
    oa << name_to_type; // записываем сериализованные данные в кэш
    ofs.close();
}

/** Копирует сериализованные данные из кэша в файл с данными по заданному смещению.
 * @param fd - файловый дескриптор
 * @param cached_fd - дескриптор кэш-файла
 * @param file_move_offset - заданное смещение для копирования
 * @return offset, если запись прошла успешно, 1L в случае неудачи
 */
uint64_t move_from_cache_to_db(int32_t fd, int32_t cached_fd, int64_t file_move_offset) {
    auto cfd_size = get_file_size(cached_fd);
    void *buff = malloc(cfd_size);
    read_from_file(cached_fd, 0, buff, cfd_size);
    auto offset = write_into_file(fd, file_move_offset, buff, cfd_size); // записываем сериализованную схему в бд
    free(buff);

    return offset;
}

/** Считывает из файла с данными и десериализует поле data структуры node.
 * @param fd - файловый дескриптор
 * @param node_offset - смещение структуры node в файле
 * @param node_size - полный размер header, с учетом сериализованного поля data
 * @return поле data упакованное в контейнер в формате map[value_name] = data
 */
std::unordered_map<std::string, std::string> deserialize_node_data(int32_t fd, int64_t node_offset, int64_t node_size) {
    auto offset = node_offset + offsetof(node, data);
    auto size = node_size - offsetof(node, data);
    auto buff = malloc(size);
    auto cfd = open_file(".cache");

    read_from_file(fd, offset, buff, size);
    write_into_file(cfd, offset, buff, size); // записываем десериалиуемый контейнер из бд в кэш
    std::ifstream ifs(".cache"); // десериализуем
    boost::archive::text_iarchive ia(ifs);
    std::unordered_map<std::string, std::string> value_to_data = {};
    ia >> value_to_data;
    ifs.close();
    free(buff);

    return value_to_data;
}

/** Сериализует и кэширует поле data структуры node.
 * @param name_to_value - данные структуры в формате map[value_name] = data
 */
void serialize_and_cache_node_data(std::unordered_map<std::string, std::string> &name_to_value) {
    std::ofstream ofs(".cache"); // сериализуем
    boost::archive::text_oarchive oa(ofs);
    oa << name_to_value; // записываем сериализованные данные в кэш
    ofs.close();
}
#endif //LLP_DATABASE_SERIALIZATION_H
