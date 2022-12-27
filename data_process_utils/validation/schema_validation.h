#ifndef LLP_DATABASE_SCHEMA_VALIDATION_H
#define LLP_DATABASE_SCHEMA_VALIDATION_H

#include <cstdint>
#include <string>
#include <unordered_map>
// TODO добавить описания
bool is_valid_int(const std::string &s);

bool is_valid_float(const std::string &s);

bool validate_node_by_schema(int32_t fd, const std::unordered_map<std::string, std::string> &node_data);

#endif //LLP_DATABASE_SCHEMA_VALIDATION_H