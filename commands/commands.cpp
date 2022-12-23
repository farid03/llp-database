#include "commands.h"
#include "../dbstruct/tree_header.h"
#include <unordered_map>
#include <list>
#include <utility>
#include <sys/stat.h>
#include "./schema_validation.h"

int32_t get_next_id(int32_t fd) {
    struct tree_header header = get_tree_header_from_db(fd);
    header.nodes_count++;
    set_tree_header_to_db(fd, header);

    return header.nodes_count - 1;
}

/// абстракция над add_node, будет публичным интерфейсом для работы с бд
bool add(const int32_t fd, const std::unordered_map<std::string, std::pair<data_type, std::string>> &node) {

}

/** Добавляет новый документ в дерево.
 * @param fd - файловый дескриптор
 * @param node - новый узел, который нужно добавить | node = map[field_name]: {data_type, data}
 * @return true, если запись прошла успешно, false в случае неудачи
 */
bool
add_node(const int32_t fd, const std::unordered_map<std::string, std::pair<data_type, std::string>> &node,
         bool is_update) {
    // TODO не забыть проинициализировать поля node
    if (!is_update) {

    } else {

    }
    return false;
}

std::list<node> *find_node_by_id(const int32_t fd, int32_t id) { return NULL; }

std::list<node> *find_node_by_parent(const int32_t fd, int32_t parent_id) { return NULL; }

std::list<node> *find_all(const int32_t fd) { return NULL; }

std::list<node> *
find_node_by_condition(const int32_t fd, std::pair<std::string, std::string> condition) { return NULL; }

bool delete_node(int32_t fd, int32_t id) {
    return false;
}

bool update_node(int32_t fd, std::unordered_map<std::string, std::pair<data_type, std::string>> node) {
    return delete_node(fd, std::stoi(node["id"].second)) && add_node(fd, node, true);
}