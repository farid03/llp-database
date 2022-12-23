#ifndef LLP_DATABASE_SCHEMA_VALIDATION_H
#define LLP_DATABASE_SCHEMA_VALIDATION_H

#include "../dbstruct/tree_header.h"

bool is_valid_int(const std::string &s) {
    size_t offset = 0;
    if (s[offset] == '-')
        ++offset;
    return s.find_first_not_of("0123456789", offset) == std::string::npos;
}

bool is_valid_float(const std::string &s) {
    size_t offset = 0;
    if (s[offset] == '-')
        ++offset;
    return s.find_first_not_of("0123456789.", offset) == std::string::npos;
}

bool validate_schema(int32_t fd, const std::unordered_map<std::string, std::pair<data_type, std::string>>& node) {
    struct tree_header header = get_tree_header_from_db(fd);
    for (const auto& a: node) {
        if (header.value_name_to_type[a.first] != a.second.first) {
            return false;
        }
        switch (a.second.first) {
            case BOOL:
                if (a.second.second != "true" && a.second.second != "false") {
                    return false;
                }
                break;
            case INT:
                if (!is_valid_int(a.second.second)) {
                    return false;
                }
                break;
            case FLOAT:
                if (!is_valid_float(a.second.second)) {
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