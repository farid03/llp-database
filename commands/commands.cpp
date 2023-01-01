#include <stack>
#include "commands.h"
#include "../data_process_utils/validation/schema_validation.h"

int32_t add_node(const int32_t fd, int32_t parent_id, const std::unordered_map<std::string, std::string> &node_data) {
    if (!validate_node_by_schema(fd, node_data)) {
        printf("Invalid node_data. Field must be match the schema!\n");
        return -1;
    }
    struct node node = {0};
    node.id = get_next_node_id(fd);
    auto parent = read_node_header_from_db(fd, idx.id_to_offset[parent_id]);
    node.parent = parent.offset;
    node.data = node_data;
    int64_t node_offset{0};
    //  offset, size, r_size будут заданы дальше, при записи

    if (parent.first_child != 0) { // если у родителя есть уже другие дети
        auto last_parent_child = read_node_header_from_db(fd, parent.first_child); // найдем последнего ребенка родителя
        while (last_parent_child.next != 0) {
            last_parent_child = read_node_header_from_db(fd, last_parent_child.next);
        }
        node.prev = last_parent_child.offset;
        node_offset = write_node_to_db(fd, node);
        last_parent_child.next = node_offset; // укажем предыдущей ноде родителя нового соседа
        if (!write_node_header_to_db(fd, last_parent_child, last_parent_child.offset)) {
            printf("Error in write_node_header_to_db! Rewriting last parent child node in function add not valid!\n");
            return -1;
        }
    } else {
        node_offset = write_node_to_db(fd, node);
        auto parent_node = read_node_header_from_db(fd, parent.offset);
        parent_node.first_child = node_offset;
        if (!write_node_header_to_db(fd, parent_node, parent_node.offset)) {
            printf("Error in write_node_header_to_db! Rewriting parent node in function add not valid!\n");
            return -1;
        }
    }

    add_node_to_index(node.id, node_offset);
    return node.id;
}

std::unordered_map<std::string, std::string> find_node_by_id(const int32_t fd, int32_t id) {
    if (idx.id_to_offset.count(id) == 0) {
        printf("Node with such an id{%d} doesn't exist!\n", id);
        return {};
    }
    auto node = read_node_from_db(fd, idx.id_to_offset[id]);
    node.data["id"] = std::to_string(node.id);
    if (node.parent != 0) {
        node.data["parent_id"] = std::to_string(read_node_header_from_db(fd, node.parent).id);
    }
    return node.data;
}

struct result_iterator find(const int32_t fd, const struct condition &condition) {
    auto header = get_tree_header_from_db(fd);
    if (!validate_field_by_schema(header.schema, {condition.name_value, condition.value})) {
        printf("Invalid condition in function find!\n");
        return {};
    }
    struct result_iterator res = result_iterator(fd, condition);

    return res;
}

struct result_iterator find_all(int32_t fd) {
    return {fd, condition("nop", "nop", NOP)};
}


bool delete_node(int32_t fd, int32_t id) {
    // при удалении не забыть освободить и выделить free_space на предыдущем месте
    if (id == 0) { // корневой узел нельзя удалять
        printf("Cannot delete a node with ID 0.");
        return false;
    }
    if (idx.id_to_offset.count(id) == 0) {
        printf("Node with such an id{%d} doesn't exist!\n", id);
        return false;
    }

    std::stack<int64_t> rm_nodes_offset = {};
    auto node_to_remove = read_node_header_from_db(fd, idx.id_to_offset[id]);
    remove_node_from_index(node_to_remove.id);
    remove_node_from_db(fd, node_to_remove);
    if (node_to_remove.first_child != 0) {
        rm_nodes_offset.push(node_to_remove.first_child);
    }

    // нужно обновить ссылки у родителя и соседних детей на нас
    auto parent = read_node_header_from_db(fd, node_to_remove.parent);
    if (parent.first_child == node_to_remove.offset) { // если мы единственный ребенок (у родителя есть на нас указатель
        parent.first_child = 0;
        write_node_header_to_db(fd, parent, parent.offset);
    } else {
        if (node_to_remove.next != 0) {
            auto next = read_node_header_from_db(fd, node_to_remove.next);
            next.prev = node_to_remove.prev;
            write_node_header_to_db(fd, next, next.offset);
        }
        if (node_to_remove.prev != 0) {
            auto prev = read_node_header_from_db(fd, node_to_remove.prev);
            prev.next = node_to_remove.next;
            write_node_header_to_db(fd, prev, prev.offset);
        }
    }

    // рекурсивно удаляем все дочерние ноды удаляемой ноды
    while (!rm_nodes_offset.empty()) {
        auto node_offset = rm_nodes_offset.top();
        rm_nodes_offset.pop();
        node_to_remove = read_node_header_from_db(fd, node_offset);
        remove_node_from_index(node_to_remove.id);
        remove_node_from_db(fd, node_to_remove);

        if (node_to_remove.first_child != 0) {
            rm_nodes_offset.push(node_to_remove.first_child);
        }
        while (node_to_remove.next != 0) {
            node_to_remove = read_node_header_from_db(fd, node_to_remove.next);
            remove_node_from_index(node_to_remove.id);
            remove_node_from_db(fd, node_to_remove);
            if (node_to_remove.first_child != 0) {
                rm_nodes_offset.push(node_to_remove.first_child);
            }
        }
    }

    return true;
}

bool update_node(int32_t fd, int32_t id, const std::unordered_map<std::string, std::string> &node_data) {
    if (id == 0) { // корневой узел нельзя удалять
        printf("Cannot update a node with ID 0.");
        return false;
    }
    if (idx.id_to_offset.count(id) == 0) {
        printf("Node with such an id{%d} doesn't exist!\n", id);
        return false;
    }
    if (!validate_node_by_schema(fd, node_data)) {
        printf("Invalid node_data. Field must be match the schema!\n");
        return false;
    }

    auto node_to_update = read_node_header_from_db(fd, idx.id_to_offset[id]);
    node_to_update.data = node_data;
    // валидируется при записи: offset, size, r_size
    // оставляем прежними: id, parent_id, prev, next, first_child
    remove_node_from_db(fd, node_to_update);
    auto updated_offset = write_node_to_db(fd, node_to_update);

    // нужно обновить ссылки у родителя и соседних детей на нас
    auto parent = read_node_header_from_db(fd, node_to_update.parent);
    if (parent.first_child == node_to_update.offset) { // если мы единственный ребенок (у родителя есть на нас указатель
        parent.first_child = updated_offset;
        write_node_header_to_db(fd, parent, parent.offset);
    } else {
        if (node_to_update.next != 0) {
            auto next = read_node_header_from_db(fd, node_to_update.next);
            next.prev = updated_offset;
            write_node_header_to_db(fd, next, next.offset);
        }
        if (node_to_update.prev != 0) {
            auto prev = read_node_header_from_db(fd, node_to_update.prev);
            prev.next = updated_offset;
            write_node_header_to_db(fd, prev, prev.offset);
        }
    }
    idx.id_to_offset[id] = updated_offset; // обновляем offset в индексе

    return true;
}