#ifndef LLP_DATABASE_TREE_HEADER_H
#define LLP_DATABASE_TREE_HEADER_H

#include <unordered_map>
#include <set>
#include <cstdint>
#include <string>

/** Индекс для быстрой адресации к узлам дерева по id.
 * @details Потребление памяти пренебрежимо мало ~ 1.1Мб на 100 000 элементов
*/
extern struct index idx;

struct index {
    std::unordered_map<int32_t, int64_t> id_to_offset = {};
};

enum data_type {
    BOOL = 0,
    INT,
    FLOAT,
    STRING
};

struct tree_header {
    /**
     * Адрес смещения в файле первого узла дерева.
     */
    int64_t first_node;
    /**
     * Адрес смещения в файле первого свободного пространства. \n
     * Равняется 0 в случае отсутствия.
     */
    int64_t first_free_space;
    /**
     * Количество документов в базе данных, используется для генерации id новых узлов.
     * @bug не уменьшается при удалении узлов из дерева.
     */
    int32_t nodes_count;
    /**
     * Занимаемый в памяти размер структуры (с учетом сериализованной схемы).
     */
    int64_t size;
    /**
     * Схема данных по которой валидируются узлы документного дерева.
     * @details map[value_name] = data_type | id и parent_id -- не являются частью схемы.
     */
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

/** Инициализирует внутреннюю структуру нового файла для хранения данных.
 * @param file_name - название файла
 * @param name_to_type - схема базы данных | map[value_name] = data_type
 * @param first_node_data - поля первого узла документного дерева | map[value_name] = data
 * @return файловый дескриптор в котором будет инициализирована структура данных, -1 в случае ошибки
 * @attention схема данных задается один раз при инициализации модуля хранения данных и не подлежит изменению
 */
int32_t initialize_db(const char *file_name, std::unordered_map<std::string, data_type> &name_to_type,
                      const std::unordered_map<std::string, std::string> &first_node_data);

/** Инициализирует внутреннюю структуру старого файла, в котором хранятся данные.
 * @param file_name - название файла
 * @return файловый дескриптор в котором будет инициализирована структура данных, -1 в случае ошибки
 */
int32_t initialize_db(const char *file_name);

/** Закрывает файл, в котором хранятся данные.
 * @param fd - файловый дескриптор
 */
void close_db(int32_t fd);

/**
 * Возвращает id для следующего узла дерева.
 * @param fd - файловый дескриптор
 * @return идентификатор нового узла документного дерева
 * @attention при вызове изменяет поле tree_header.nodes_count текущего дерева
 */
int32_t get_next_node_id(int32_t fd);

/**
 * Добавляет идентификатор узла в индекс модуля хранения данных.
 * @param id - идентификатор узла
 * @param offset - адрес в файле
 * @return true, в случае успеха, иначе false
 */
bool add_node_to_index(int32_t id, int64_t offset);

/**
 * Удаляет идентификатор узла из индекса модуля хранения данных.
 * @param id - идентификатор узла
 * @return true, в случае успеха, иначе false
 */
bool remove_node_from_index(int32_t id);

#endif //LLP_DATABASE_TREE_HEADER_H
