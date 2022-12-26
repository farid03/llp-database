#include "tree_header.h"
#include "../data_process_utils/serialization/serialization.h"
#include "../commands/commands.h"

int32_t get_next_node_id(int32_t fd) {
    struct tree_header header = get_tree_header_without_schema_from_db(fd);
    header.nodes_count++;
    set_tree_header_to_db(fd, header);

    return header.nodes_count - 1;
}

struct tree_header get_tree_header_without_schema_from_db(const int32_t fd) {
    struct tree_header header = {0, 0, 0, 0, {}};
    read_from_file(fd, 0, &header, offsetof(tree_header, schema) - 1);

    return header;
}

struct tree_header get_tree_header_from_db(const int32_t fd) {
    auto header = get_tree_header_without_schema_from_db(fd);
    header.schema = deserialize_file_header_schema(fd, header.size);

    return header;
}

bool set_tree_header_to_db(const int32_t fd, tree_header header) {
    return write_into_file(fd, 0, &header, offsetof(tree_header, schema) - 1) == 0;
}

bool initialize_db(int32_t fd, std::unordered_map<std::string, data_type> &name_to_type,
                   const std::unordered_map<std::string, std::string> &first_node_data) {
    struct tree_header header = {0};
    struct free_space space = {0};
    space.size = 256;

    if (name_to_type["parent_id"] != INT) {
        printf("Cannot find parent_id in schema!\n");
        return false;
    }

    for (const auto &schema_elem: name_to_type) {
        switch (schema_elem.second) {
            case BOOL:
            case INT:
            case FLOAT:
            case STRING:
                continue;
            default:
                printf("Invalid schema type!\n");
                return false;
        }
    }
    header.nodes_count = 1;
    // в функции add_node происходит валидация первого добавляемого узла схеме
    header.first_node = add_node(fd, first_node_data, false); // TODO не забыть в add_node сгенерировать id для узла
//    write_node_to_db()

    header.first_free_space = 0;

    auto cfd = open_file(".cache"); // блок для сериализации
    serialize_and_cache_file_header(name_to_type);
    move_from_cache_to_db(fd, cfd, offsetof(tree_header, schema));

    header.size = offsetof(tree_header, schema) + get_file_size(cfd) - 1; // валидируем размер хедера файла с учетом сериализованной схемы
    set_tree_header_to_db(fd, header);

    return true;
}

