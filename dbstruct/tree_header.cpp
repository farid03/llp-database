#include <stack>
#include <fstream>
#include "tree_header.h"
#include "../data_process_utils/validation/schema_validation.h"
#include "../data_process_utils/serialization/serialization.h"
#include "node.h"
#include "../file_workers/file_utils.h"

struct index idx = {};

int32_t get_next_node_id(int32_t fd) {
    struct tree_header header = get_tree_header_without_schema_from_db(fd);
    header.nodes_count++;
    set_tree_header_to_db(fd, header);

    return header.nodes_count - 1;
}

struct tree_header get_tree_header_without_schema_from_db(const int32_t fd) {
    struct tree_header header = {0, 0, 0, 0, {}};
    auto ret = read_from_file(fd, 0, &header, offsetof(tree_header, schema) - 1);
    if (ret != offsetof(tree_header, schema) - 1) {
        printf("Error in get_tree_header_without_schema! %lu %zd", offsetof(tree_header, schema) - 1, ret);
    }

    return header;
}

struct tree_header get_tree_header_from_db(const int32_t fd) {
    //    printf("header {%ld, %ld, %d, %ld}\n", header.first_node, header.first_free_space, header.nodes_count, header.size);
    auto header = get_tree_header_without_schema_from_db(fd);
    header.schema = deserialize_file_header_schema(fd, header.size);

    return header;
}

bool set_tree_header_to_db(const int32_t fd, tree_header header) {
    return write_into_file(fd, 0, &header, offsetof(tree_header, schema) - 1) == 0;
}

bool initialize_index(int32_t fd);

int32_t initialize_db(const char *file_name) {
    auto fd = open_file(file_name);
    if (!initialize_index(fd)) {
        printf("Error in initialize_db -> initialize_index! (1)\n");
        return -1;
    }

    return fd;
}

int32_t initialize_db(const char *file_name, std::unordered_map<std::string, data_type> &name_to_type,
                      const std::unordered_map<std::string, std::string> &first_node_data) {
    int32_t fd = open_file(file_name);
    std::fstream clear_file(file_name, std::ios::out); // очищаем файл
    clear_file.close();
    struct tree_header header = {0};

    for (const auto &schema_elem: name_to_type) {
        switch (schema_elem.second) {
            case BOOL:
            case INT:
            case FLOAT:
            case STRING:
                continue;
            default:
                printf("Invalid type in schema!\n");
                return -1;
        }
    }
    header.nodes_count = 1;
    // в функции add_node происходит валидация первого добавляемого узла схеме
    struct node first_node = {
            0, // задается в write_node_to_db
            0, // id
            0, // это нулевая нода, родитель отсутствует
            0, // prev
            0, // next
            0, // first_child
            0, // size задается в write_node_to_db
            0, // r_size задается в write_node_to_db
            first_node_data
    };

    header.first_free_space = 0;

    auto cfd = open_file(".cache"); // блок для сериализации
    serialize_and_cache_file_header(name_to_type);
    move_from_cache_to_db(fd, cfd, offsetof(tree_header, schema));

    header.size = offsetof(tree_header, schema) + get_file_size(cfd) - 1; // валидируем размер хедера файла с учетом сериализованной схемы
    set_tree_header_to_db(fd, header); // записываем чтобы записываемая первая нода нашла себе валидный адрес в конце файла
    if (!validate_node_by_schema(fd, first_node_data)) { // валидация данных по схеме перед записью
        return -1;
    }
    header.first_node = write_node_to_db(fd, first_node);
    header.nodes_count = 1;
    set_tree_header_to_db(fd, header);

    if (!initialize_index(fd)) {
        printf("Error in initialize_db -> initialize_index! (2)\n");
        return -1;
    }
    close_file(cfd);

    return fd;
}

void close_db(int32_t fd) {
    idx.id_to_offset.clear();
    close_file(fd);
}

bool add_node_to_index(int32_t id, int64_t offset) {
    if (idx.id_to_offset.count(id) != 0) {
        printf("Element already exists in index!\n");
        return false;
    } else {
        idx.id_to_offset[id] = offset;
    }

    return true;
}

bool remove_node_from_index(int32_t id) {
    if (idx.id_to_offset.count(id) == 0) {
        printf("Element not exists in index!\n");
        return false;
    } else {
        idx.id_to_offset.erase(id);
    }

    return true;
}

// Проходимся по всему дереву документов и инициализируем индексы
bool initialize_index(int32_t fd) {
    auto header = get_tree_header_without_schema_from_db(fd);
    std::stack<int64_t> s = {};
    auto first_node = read_node_from_db(fd, header.first_node);
    idx.id_to_offset[first_node.id] = header.first_node; // стартовая инициализация индексов

    if (first_node.first_child != 0) {
        s.push(first_node.first_child);
    }
    while (!s.empty()) {
        auto current_node_offset = s.top();
        s.pop();
        auto node = read_node_header_from_db(fd, current_node_offset);
        add_node_to_index(node.id, node.offset);
        if (node.first_child != 0) {
            s.push(node.first_child);
        }
        while (node.next != 0) {
            node = read_node_header_from_db(fd, node.next);
            add_node_to_index(node.id, node.offset);
            if (node.first_child != 0) {
                s.push(node.first_child);
            }
        }
    }

    return true;
}
