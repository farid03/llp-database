#include "schema_validation.h"
#include "../../dbstruct/tree_header.h"

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
//
// true, в случае успеха, иначе false
bool validate_node_by_schema(int32_t fd, const std::unordered_map<std::string, std::string> &node_data) {
    struct tree_header header = get_tree_header_from_db(fd);
    if (node_data.size() != header.schema.size()) { // валидиуем количество полей
        return false;
    }

    for (const auto& a: node_data) {
        if (header.schema.count(a.first) != 1) { // валидируем название полей ноды по схеме, названия уникальны (map)
            return false;
        }
        switch (header.schema[a.first]) {
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
                break;
            default:
                return false;
        }
    }

    return true;
}
