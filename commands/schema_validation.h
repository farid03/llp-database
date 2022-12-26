#ifndef LLP_DATABASE_SCHEMA_VALIDATION_H
#define LLP_DATABASE_SCHEMA_VALIDATION_H

#include "../dbstruct/tree_header.h"

inline bool is_valid_int(const std::string &s) {
    size_t offset = 0;
    if (s[offset] == '-')
        ++offset;
    return s.find_first_not_of("0123456789", offset) == std::string::npos;
}

inline bool is_valid_float(const std::string &s) {
    size_t offset = 0;
    if (s[offset] == '-')
        ++offset;
    return s.find_first_not_of("0123456789.", offset) == std::string::npos;
}
//
// true, в случае успеха, иначе false
static bool validate_schema(int32_t fd, const std::unordered_map<std::string, std::string> &node_data) {
    struct tree_header header = get_tree_header_from_db(fd);
    for (const auto& a: node_data) {
// нет валидации на названия полей
        switch (header.schema[a.second]) {
            case BOOL:
                if (a.second != "true" && a.second != "false") {
                    return false;
                }
                break;
            case INT:
                if (!is_valid_int(a.second)) {
                    return false;
                }
                break;
            case FLOAT:
                if (!is_valid_float(a.second)) {
                    return false;
                }
                break;
            case STRING:
            default:
                return false;
        }
    }

    return true;
}

#endif // LLP_DATABASE_SCHEMA_VALIDATION_H