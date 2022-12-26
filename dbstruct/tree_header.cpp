#include "tree_header.h"
#include "../file_workers/file_utils.h"
#include "../commands/commands.cpp"
#include "serialization.h"

struct tree_header get_tree_header_from_db(const int32_t fd) {
    struct tree_header header = {0, 0, 0, 0, {}};
    read_from_file(fd, 0, &header, offsetof(tree_header, schema) - 1);
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
    header.first_node = add_node(fd, first_node_data, false);

    auto cfd = open_file(".cache"); // блок для сериализации
    serialize_and_cache_file_header(name_to_type);
    auto cfd_size = get_cache_size(cfd);
    void *buff = malloc(cfd_size);
    read_from_file(cfd, 0, buff, cfd_size);
    write_into_file(fd, offsetof(tree_header, schema), buff, cfd_size); // записываем сериализованную схему в бд

    header.size = offsetof(tree_header, schema) + cfd_size - 1; // валидируем размер хедера файла с учетом сериализованной схемы
    auto first_node = read_node_header_from_db(fd, header.first_node);
    header.first_free_space = int64_t(first_node.offset + first_node.size + 1); // создаем искуственно первую пустую ноду
    set_tree_header_to_db(fd, header);
    write_free_space_to_db(fd, space, header.first_free_space); // возможно выйдет обойтись без этого приема, если обработать отсутствие пустой ноды в файле при вставках
    free(buff);

    return true;
}

