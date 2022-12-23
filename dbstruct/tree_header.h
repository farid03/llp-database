#ifndef LLP_DATABASE_TREE_HEADER_H
#define LLP_DATABASE_TREE_HEADER_H
#include "node.h"
#include <unordered_map>

enum data_type {
    BOOL = 0,
    INT,
    FLOAT,
    STRING
};

struct tree_header {
    node first_node;
    free_space first_free_space;
    std::unordered_map<std::string, data_type> value_name_to_type; // schema
};

/** Возвращает tree_header файла
 * @param fd - файловый дескриптор
 */
struct tree_header get_tree_header_from_db(const int32_t fd);

/** Записывает переданный tree_header в файл
 * @param fd - файловый дескриптор
 * @param header - tree_header файла
 * @return struct stat - информация о файле fd
 */
struct stat set_tree_header_to_db(const int32_t fd, tree_header header);

#endif //LLP_DATABASE_TREE_HEADER_H
