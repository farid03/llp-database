#include "commands.h"
#include "../data_process_utils/validation/schema_validation.h"
#include "../dbstruct/tree_header.h"
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
    struct node node = { 0 };
    node.id = get_next_node_id(fd);
    node.parent_id = parent_id;
    node.data = node_data;
    int64_t node_offset = 0;
    //  offset, size, r_size будут заданы дальше, при записи

    if (!idx.parent_to_childs[parent_id].empty()) { // если у родителя есть уже другие дети
         auto last_parent_child = idx.parent_to_childs[parent_id].end(); // добавим в лист родителя нового ребенка
         node.prev = *(--last_parent_child);
        node_offset = write_node_to_db(fd, node);
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

std::unordered_map<std::string, std::string> find_node_by_id(const int32_t fd, int32_t id) {
    if (idx.id_to_offset.count(id) == 0) {
        printf("Node with such an id doesn't exist!\n");
        return {};
    }
    auto node = read_node_from_db(fd, idx.id_to_offset[id]);
    node.data["id"] = std::to_string(node.id);
    node.data["parent_id"] = std::to_string(node.parent_id);

    return read_node_from_db(fd, idx.id_to_offset[id]).data;
}

std::list<std::unordered_map<std::string, std::string>> find_node_by_parent(const int32_t fd, int32_t parent_id) {
    std::list<std::unordered_map<std::string, std::string>> result{};
    if (idx.parent_to_childs.count(parent_id) == 0) {
        printf("Node with such a parent_id doesn't exist!\n");
        return {};
    }

    for (auto id: idx.parent_to_childs[parent_id]) {
        if (idx.id_to_offset.count(id) == 0) { // невозможно, конечно, но пусть будет
            printf("Node with such an id doesn't exist!\n");
        }
        auto node = read_node_from_db(fd, idx.id_to_offset[id]);
        node.data["id"] = std::to_string(node.id);
        node.data["parent_id"] = std::to_string(node.parent_id);

        result.emplace_back(node.data);
    }

    return result;
}

// TODO узнать насколько разумен такой формат возврата результатов
std::list<std::unordered_map<std::string, std::string>> find_all(const int32_t fd) {
    std::list<std::unordered_map<std::string, std::string>> result{};

    for (auto node: idx.id_to_offset) {
        auto current_node = read_node_from_db(fd, node.second);
        current_node.data["id"] = std::to_string(current_node.id);
        current_node.data["parent_id"] = std::to_string(current_node.parent_id);
        result.emplace_back(current_node.data);
    }

    return result;
}

// условие можно задавать только на поля содержащиеся в заданной схеме
std::list<std::unordered_map<std::string, std::string>>
find_node_by_condition(const int32_t fd, const std::pair<std::string, std::string>& condition) {
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

bool delete_node(int32_t fd, int32_t id) {
    // при удалении не забыть освободить и выделить free_space на предыдущем месте
    return false;
}

bool update_node(int32_t fd, std::unordered_map<std::string, std::string> node_data) {
    return delete_node(fd, std::stoi(node_data["id"]));
}