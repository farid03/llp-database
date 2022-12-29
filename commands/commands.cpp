#include <stack>
#include "commands.h"
#include "../data_process_utils/validation/schema_validation.h"
#include "../dbstruct/node.h"

/** Добавляет новый документ в дерево.
 * @param fd - файловый дескриптор
 * @param node - новый узел, который нужно добавить | node = map[field_name]: data
 * @return возвращает id записанного узла, -1 в случае неудачи
 */
int64_t add_node(const int32_t fd, int64_t parent_id, const std::unordered_map<std::string, std::string> &node_data) {
    if (!validate_node_by_schema(fd, node_data)) {
        printf("Invalid node_data. Field must be match the schema!\n");
        return -1;
    }
    struct node node = {0};
    node.id = get_next_node_id(fd);
    node.parent_id = parent_id;
    node.data = node_data;
    int64_t node_offset = 0;
    //  offset, size, r_size будут заданы дальше, при записи

    if (!idx.parent_to_childs[parent_id].empty()) { // если у родителя есть уже другие дети
        auto last_parent_child = idx.parent_to_childs[parent_id].end(); // добавим в лист родителя нового ребенка
        node.prev = idx.id_to_offset[*(--last_parent_child)];
        node_offset = write_node_to_db(fd, node);
        auto prev_node = read_node_from_db(fd, node.prev); // укажем предыдущей ноде родителя новую
        prev_node.next = node_offset;
        write_node_to_db(fd, prev_node, prev_node.offset);
    } else {
        node_offset = write_node_to_db(fd, node);
        auto parent_node = read_node_from_db(fd, idx.id_to_offset[parent_id]);
        parent_node.first_child = node_offset;
        if (!write_node_to_db(fd, parent_node, parent_node.offset)) {
            printf("Error in write_node_to_db! Rewriting parent node in function add not valid!\n");
            return -1;
        }
    }

    add_node_to_index(node.id, parent_id, node_offset);
    return node.id;
}

std::unordered_map<std::string, std::string> find_node_by_id(const int32_t fd, int64_t id) {
    if (idx.id_to_offset.count(id) == 0) {
        printf("Node with such an id{%ld} doesn't exist!\n", id);
        return {};
    }
    auto node = read_node_from_db(fd, idx.id_to_offset[id]);
    node.data["id"] = std::to_string(node.id);
    node.data["parent_id"] = std::to_string(node.parent_id);

    return node.data;
}

std::list<std::unordered_map<std::string, std::string>> find_node_by_parent(const int32_t fd, int64_t parent_id) {
    std::list<std::unordered_map<std::string, std::string>> result{};
    if (idx.parent_to_childs.count(parent_id) == 0) {
        printf("Node with such a parent_id doesn't exist!\n");
        return {};
    }

    for (auto id: idx.parent_to_childs[parent_id]) {
        result.emplace_back(find_node_by_id(fd, id));
    }

    return result;
}

// TODO узнать насколько разумен такой формат возврата результатов, не разумен
std::list<std::unordered_map<std::string, std::string>> find_all(const int32_t fd) {
    std::list<std::unordered_map<std::string, std::string>> result{};

    for (auto node: idx.id_to_offset) {
        result.emplace_back(find_node_by_id(fd, node.first));
    }

    return result;
}

// условие можно задавать только на поля содержащиеся в заданной схеме
std::list<std::unordered_map<std::string, std::string>>
find_node_by_condition(const int32_t fd, const std::pair<std::string, std::string> &condition) {
    auto header = get_tree_header_from_db(fd);
    if (!validate_field_by_schema(header.schema, condition)) {
        printf("Invalid condition in function find_node_by_condition!\n");
        return {};
    }
    std::list<std::unordered_map<std::string, std::string>> result{};
    for (auto node: idx.id_to_offset) {
        auto current_node = read_node_from_db(fd, node.second);
        if (current_node.data[condition.first] == condition.second) {
            current_node.data["id"] = std::to_string(current_node.id);
            current_node.data["parent_id"] = std::to_string(current_node.parent_id);
            result.emplace_back(current_node.data);
        }
    }

    return result;
}

bool delete_node(int32_t fd, int64_t id) {
    // при удалении не забыть освободить и выделить free_space на предыдущем месте
    if (id == 0) { // корневой узел нельзя удалять
        printf("Cannot delete a node with ID 0.");
        return false;
    }
    if (idx.id_to_offset.count(id) == 0) {
        printf("Node with such an id{%ld} doesn't exist!\n", id);
        return false;
    }

    std::stack<int64_t> rm_nodes_offset = {};
    auto node_to_remove = read_node_header_from_db(fd, idx.id_to_offset[id]);
    remove_node_from_index(node_to_remove.id, node_to_remove.parent_id);
    remove_node_from_db(fd, node_to_remove);
    if (node_to_remove.first_child != 0) {
        rm_nodes_offset.push(node_to_remove.first_child);
    }

    while (!rm_nodes_offset.empty()) {
        auto node_offset = rm_nodes_offset.top();
        rm_nodes_offset.pop();
        node_to_remove = read_node_header_from_db(fd, node_offset);
        remove_node_from_index(node_to_remove.id, node_to_remove.parent_id);
        remove_node_from_db(fd, node_to_remove);

        if (node_to_remove.first_child != 0) {
            rm_nodes_offset.push(node_to_remove.first_child);
        }
        while (node_to_remove.next != 0) {
            node_to_remove = read_node_header_from_db(fd, node_to_remove.next);
            remove_node_from_index(node_to_remove.id, node_to_remove.parent_id);
            remove_node_from_db(fd, node_to_remove);
            if (node_to_remove.first_child != 0) {
                rm_nodes_offset.push(node_to_remove.first_child);
            }
        }
    }

    return true;
}

bool update_node(int32_t fd, int64_t id, const std::unordered_map<std::string, std::string>& node_data) {
    if (id == 0) { // корневой узел нельзя удалять
        printf("Cannot update a node with ID 0.");
        return false;
    }
    if (idx.id_to_offset.count(id) == 0) {
        printf("Node with such an id{%ld} doesn't exist!\n", id);
        return false;
    }
    if (!validate_node_by_schema(fd, node_data)) {
        printf("Invalid node_data. Field must be match the schema!\n");
        return false;
    }

    auto node_to_update = read_node_header_from_db(fd, idx.id_to_offset[id]);
    node_to_update.data = node_data;
    // валидируется при записи: offset, size, r_size
    // оставляем тем же: id, parent_id, prev, next, first_child
    remove_node_from_db(fd, node_to_update);
    auto updated_offset = write_node_to_db(fd, node_to_update);

    // нужно обновить ссылки у родителя и соседних детей на нас
    auto parent = read_node_header_from_db(fd, idx.id_to_offset[node_to_update.parent_id]);
    if (idx.parent_to_childs[parent.id].size() == 1) { // если мы единственный ребенок (у родителя есть на нас указатель
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