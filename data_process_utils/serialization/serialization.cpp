#include "serialization.h"
#include "../../dbstruct/node.h"
#include "../../file_workers/file_utils.h"

std::unordered_map<std::string, data_type> deserialize_file_header_schema(int32_t fd, int64_t header_size) {
    auto size = header_size - offsetof(tree_header, schema) + 1;
    auto buff = malloc(size);
    std::fstream clear_file(".cache", std::ios::out); // очищаем файл
    clear_file.close(); // todo проверить, возможно, выйдет обойтись без предварительной очистки
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

uint64_t move_from_cache_to_db(int32_t fd, int32_t cached_fd, int64_t file_move_offset) {
    auto cfd_size = get_file_size(cached_fd);
    void *buff = malloc(cfd_size);
    read_from_file(cached_fd, 0, buff, cfd_size);
    auto offset = write_into_file(fd, file_move_offset, buff, cfd_size); // записываем сериализованную схему в бд
    free(buff);

    return offset;
}

std::unordered_map<std::string, std::string> deserialize_node_data(int32_t fd, int64_t node_offset, int64_t node_size) {
    auto serial_offset = node_offset + offsetof(node, data);
    auto size = node_size - offsetof(node, data) + 1;
    auto buff = malloc(size);
    std::fstream clear_file(".cache", std::ios::out); // очищаем файл TODO проверить на необходимость
    clear_file.close();
    auto cfd = open_file(".cache");

    read_from_file(fd, serial_offset, buff, size);
    write_into_file(cfd, 0, buff, size); // записываем десериалиуемый контейнер из бд в кэш
    std::ifstream ifs(".cache"); // десериализуем
    boost::archive::text_iarchive ia(ifs);
    std::unordered_map<std::string, std::string> value_to_data = {};
    ia >> value_to_data;
    ifs.close();
    free(buff);

    return value_to_data;
}

void serialize_and_cache_node_data(std::unordered_map<std::string, std::string> &name_to_value) {
    std::ofstream ofs(".cache"); // сериализуем
    boost::archive::text_oarchive oa(ofs);
    oa << name_to_value; // записываем сериализованные данные в кэш
    ofs.close();
}
