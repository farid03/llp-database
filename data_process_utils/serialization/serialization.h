#ifndef LLP_DATABASE_SERIALIZATION_H
#define LLP_DATABASE_SERIALIZATION_H

#include <boost/serialization/unordered_map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <fstream>
#include "../../dbstruct/tree_header.h"

/** Десериализует схему данных указанную в заголовочной структуре файла.
 * @param fd - файловый дескриптор
 * @param header_size - полный размер header, с учетом сериализованной схемы
 * @return схема данных упакованная в контейнер в формате map[value_name] = data_type
 */
std::unordered_map<std::string, data_type> deserialize_file_header_schema(int32_t fd, int64_t header_size);

/** Сериализует и кэширует схему данных указанную в контейнере.
 * @param name_to_type - схема данных в формате map[value_name] = data_type
 * @param header_size - полный размер header, с учетом сериализованной схемы
 */
void serialize_and_cache_file_header(std::unordered_map<std::string, data_type> &name_to_type);

/** Копирует сериализованные данные из кэша в файл с данными по заданному смещению.
 * @param fd - файловый дескриптор
 * @param cached_fd - дескриптор кэш-файла
 * @param file_move_offset - заданное смещение для копирования
 * @return offset, если запись прошла успешно, 1L в случае неудачи
 */
uint64_t move_from_cache_to_db(int32_t fd, int32_t cached_fd, int64_t file_move_offset);

/** Считывает из файла с данными и десериализует поле data структуры node.
 * @param fd - файловый дескриптор
 * @param node_offset - смещение структуры node в файле
 * @param node_size - полный размер header, с учетом сериализованного поля data
 * @return поле data упакованное в контейнер в формате map[value_name] = data
 */
std::unordered_map<std::string, std::string> deserialize_node_data(int32_t fd, int64_t node_offset, int64_t node_size);

/** Сериализует и кэширует поле data структуры node.
 * @param name_to_value - данные структуры в формате map[value_name] = data
 */
void serialize_and_cache_node_data(std::unordered_map<std::string, std::string> &name_to_value);

#endif //LLP_DATABASE_SERIALIZATION_H
