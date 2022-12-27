#ifndef LLP_DATABASE_TREE_HEADER_H
#define LLP_DATABASE_TREE_HEADER_H

#include <unordered_map>
#include <set>
#include <cstdint>
#include <string>

extern struct index idx;

struct index {
    std::unordered_map<int64_t, int64_t> id_to_offset = {};
    std::unordered_map<int64_t, std::set<int64_t>> parent_to_childs = {};
};

enum data_type {
    BOOL = 0,
    INT,
    FLOAT,
    STRING
};

struct tree_header {
    /// Адрес смещения в файле первого документа.
    int64_t first_node;
    /// Адрес смещения в файле первого свободного пространства.
    int64_t first_free_space;
    /// Количество документов в базе данных, используется для генерации id новых узлов.
    int32_t nodes_count;
    /// Реальный занимаемый в памяти размер структуры (с учетом сериализованной схемы).
    int64_t size;
    /// map[value_name] = data_type | parrent_id -- обязательная часть схемы.
    std::unordered_map<std::string, data_type> schema;
};

/** Возвращает tree_header файла вместе со схемой данных.
 * @param fd - файловый дескриптор
 */
struct tree_header get_tree_header_from_db(int32_t fd);

/** Возвращает tree_header файла без схемы данных.
 * @param fd - файловый дескриптор
 */
struct tree_header get_tree_header_without_schema_from_db(int32_t fd);

/** Записывает переданный tree_header в файл (без схемы данных).
 * @param fd - файловый дескриптор
 * @param header - tree_header файла
 * @return true, в случае успеха, иначе false
 * @attention схема данных задается один раз при инициализации модуля хранения данных
 */
bool set_tree_header_to_db(int32_t fd, struct tree_header header);

/** Инициализирует внутреннюю структуру файла, в котором хранятся данные.
 * @param fd - файловый дескриптор
 * @param name_to_type - схема базы данных | map[value_name] = data_type
 * @return файловый дескриптор в котором будет инициализирована структура данных, -1 в случае ошибки
 * @attention схема данных задается один раз при инициализации модуля хранения данных и не подлежит изменению
 */
int32_t initialize_db(const char *file_name, std::unordered_map<std::string, data_type> &name_to_type,
                   const std::unordered_map<std::string, std::string> &first_node_data);
// TODO add descriptions
int32_t get_next_node_id(int32_t fd);
bool add_node_to_index(int64_t id, int64_t parent_id,int64_t offset);
bool remove_node_from_index(int64_t id, int64_t parent_id);
bool initialize_index(int32_t fd);
struct index get_idx();
#endif //LLP_DATABASE_TREE_HEADER_H
