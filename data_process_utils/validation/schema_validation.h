#ifndef LLP_DATABASE_SCHEMA_VALIDATION_H
#define LLP_DATABASE_SCHEMA_VALIDATION_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include "../../dbstruct/tree_header.h"

/// Проверяет валидность целого числа в строке
bool is_valid_int(const std::string &s);

/// Проверяет валидность числа с плавающей точкой в строке
bool is_valid_float(const std::string &s);

/**
 * Валидирует все переданные поля узла документного дерева на соответствие схеме данных.
 * @param fd - файловый дескриптор
 * @param node_data - поля проверяемого узла
 * @return true в случае успешности, иначе false
 */
bool validate_node_by_schema(int32_t fd, const std::unordered_map<std::string, std::string> &node_data);

/**
 * Валидирует переданное поле узла документного дерева на соответствие схеме данных.
 * @param fd - файловый дескриптор
 * @param name_to_value - поле проверяемого узла
 * @return true в случае успешности, иначе false
 */
bool validate_field_by_schema(const std::unordered_map<std::string, data_type> &schema,
                              const std::pair<const std::string, std::string> &name_to_value);

#endif //LLP_DATABASE_SCHEMA_VALIDATION_H