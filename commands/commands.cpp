#include <unordered_map>
#include <list>
#include <string>
#include "../dbstruct/node.h"
#include "commands.h"
#include "../data_process_utils/validation/schema_validation.h"

/// абстракция над add_node, будет публичным интерфейсом для работы с бд
bool add(const int32_t fd, const std::unordered_map<std::string, std::string> &node_data) {
    return false;
}
/// сделать аналогичные публичные интерфейсы для update find и delete и вынести их в отдельный файл




/** Добавляет новый документ в дерево.
 * @param fd - файловый дескриптор
 * @param node - новый узел, который нужно добавить | node = map[field_name]: data
 * @return смещение в документе, по которому записана новый узел
 */
int64_t add_node(const int32_t fd, const std::unordered_map<std::string, std::string> &node_data, bool is_update) {
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
std::list<node> *find_node_by_id(const int32_t fd, int32_t id) { return NULL; }

std::list<node> *find_node_by_parent(const int32_t fd, int32_t parent_id) { return NULL; }

// Реализовать первым
std::list<node> *find_all(const int32_t fd) { return NULL; }

std::list<node> *
find_node_by_condition(const int32_t fd, std::pair<std::string, std::string> condition) { return NULL; }

bool delete_node(int32_t fd, int32_t id) {
    return false;
}

bool update_node(int32_t fd, std::unordered_map<std::string, std::string> node_data) {
    return delete_node(fd, std::stoi(node_data["id"])) && add_node(fd, node_data, true);
}