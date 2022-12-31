#include "schema_validation.h"

bool is_valid_int(const std::string &s) {
    size_t offset = 0;
    if (s[offset] == '-') {
        ++offset;
    }
    return s.find_first_not_of("0123456789", offset) == std::string::npos;
}

bool is_valid_float(const std::string &s) {
    size_t offset = 0;
    if (s[offset] == '-') {
        ++offset;
    }
    return s.find_first_not_of("0123456789.", offset) == std::string::npos;
}

bool validate_field_by_schema(const std::unordered_map<std::string, data_type> &schema,
                              const std::pair<const std::string, std::string> &name_to_value) {
    if (schema.count(name_to_value.first) != 1) { // валидируем название полей ноды по схеме, названия уникальны (map)
        if (name_to_value.first != "id" && name_to_value.first != "parent_id") {
            return false;
        } else { // если это id-поля, которые могут использоваться в condition
            return is_valid_int(name_to_value.second);
        }
    }
    switch (schema.at(name_to_value.first)) {
        case BOOL:
            if (name_to_value.second != "true" && name_to_value.second != "false") {
                return false;
            }
            break;
        case INT:
            if (!is_valid_int(name_to_value.second)) {
                return false;
            }
            break;
        case FLOAT:
            if (!is_valid_float(name_to_value.second)) {
                return false;
            }
            break;
        case STRING:
            break;
        default:
            return false;
    }

    return true;
}

bool validate_node_by_schema(int32_t fd, const std::unordered_map<std::string, std::string> &node_data) {
    struct tree_header header = get_tree_header_from_db(fd);
    if (node_data.size() != header.schema.size()) { // валидиуем количество полей
        return false;
    }

    for (const auto &a: node_data) {
        if (!validate_field_by_schema(header.schema, a)) {
            return false;
        }
    }

    return true;
}
