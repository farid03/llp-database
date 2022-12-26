#include "commands.h"
#include "../dbstruct/tree_header.h"
#include <unordered_map>
#include <list>
#include <utility>
#include <sys/stat.h>
#include "./schema_validation.h"

static int32_t get_next_id(int32_t fd) {
    struct tree_header header = get_tree_header_from_db(fd);
    header.nodes_count++;
    set_tree_header_to_db(fd, header);

    return header.nodes_count - 1;
}

/// абстракция над add_node, будет публичным интерфейсом для работы с бд
static bool add(const int32_t fd, const std::unordered_map<std::string, std::string> &node_data) {
    return false;
}
/// сделать аналогичные публичные интерфейсы для update find и delete и вынести их в отдельный файл




/** Добавляет новый документ в дерево.
 * @param fd - файловый дескриптор
 * @param node - новый узел, который нужно добавить | node = map[field_name]: data
 * @return смещение в документе, по которому записана новый узел
 */
static int64_t add_node(const int32_t fd, const std::unordered_map<std::string, std::string> &node_data, bool is_update) {
    // TODO не забыть проинициализировать поля node ->
    //  сообщить листу детей родителя, что появился новый ребенок,
    //  ребенку сгенерировать новый id,
    //  offset будет задан дальше, при записи
    if (!is_update) {
        // TODO не забыть сохранить предыдущие данные (id, parrent_id, лист родителей
        //  возможно, стоит разделить логику update и add
    } else {

    }
    return 121212;
}

// Реализовать последними
static std::list<node> *find_node_by_id(const int32_t fd, int32_t id) { return NULL; }

static std::list<node> *find_node_by_parent(const int32_t fd, int32_t parent_id) { return NULL; }

// Реализовать первым
static std::list<node> *find_all(const int32_t fd) { return NULL; }

static std::list<node> *
find_node_by_condition(const int32_t fd, std::pair<std::string, std::string> condition) { return NULL; }

static bool delete_node(int32_t fd, int32_t id) {
    return false;
}

static bool update_node(int32_t fd, std::unordered_map<std::string, std::string> node_data) {
    return delete_node(fd, std::stoi(node_data["id"])) && add_node(fd, node_data, true);
}