#ifndef LLP_DATABASE_NODE_H
#define LLP_DATABASE_NODE_H

#include <cstdint>
#include <string>
#include <unordered_map>

struct node {
    /// Задается при записи в файл write_node_to_db
    int64_t offset; // АДРЕС В ФАЙЛЕ
    /// Задается в add_node
    int64_t id;
    /// Задается в add_node
    int64_t parent_id; // id
    /// Задается в add_node
    int64_t prev;   // АДРЕС В ФАЙЛЕ
    /// Задается в add_node (0 для новых нод) // тк новая нода записывается в конец бамбука детей
    int64_t next;   // АДРЕС В ФАЙЛЕ
    /// Задается в add_node (0 для новых нод)
    int64_t first_child;    // АДРЕС В ФАЙЛЕ
    /// Задается при записи в файл write_node_to_db
    uint64_t size;
    /// Задается при записи в файл write_node_to_db
    uint64_t r_size; // количество памяти, которое реально занимает структура (блок памяти)
    // (т.к. она может лежать в свободном пространстве, большем чем ему нужно)
    /// Задается и валидируется в add_node
    std::unordered_map<std::string, std::string> data;
};

struct free_space {
    int64_t prev;
    int64_t next;
    uint32_t size; // = sizeof(free_space) + space
};

/** Добавляет struct free_space в связный список освобожденных пространств (struct free_space) в файле и записывает в
 * место структуры node структуру освобожденного пространства.
 * @param fd - файловый дескриптор
 * @param node_to_free - перезаписываемая (освобождаемая) структура
 * @return offset, если запись прошла успешно, 1L в случае неудачи
 */
int64_t remove_node_from_db(int32_t fd, const struct node& node_to_free);

/** Записывает struct node в первый подходящий по размерам free_space, иначе в конец файла.
 * @param fd - файловый дескриптор
 * @param node - записываемая структура
 * @return offset, если запись прошла успешно, 1L в случае неудачи
 * @details Валидирует поля offset, node.size и r_size при записи в файл.
 * @attention Не забыть указать id структуры!
 * @attention Связи и ссылки других узлов на переданную структуру должны валидироваться вне этого метода!
 */
int64_t write_node_to_db(int32_t fd, struct node node);

/** Записывает struct node в файл по заданному смещению.
 * @param fd - файловый дескриптор
 * @param node - записываемая структура
 * @param offset - заданное смещение в байтах
 * @return true, в случае успеха, иначе false
 * @attention валидирует поле size при записи структуры в файл!
 */
bool write_node_to_db(int32_t fd, struct node node, int64_t offset);
bool write_node_header_to_db(int32_t fd, struct node node, int64_t offset);

/** Записывает struct free_space в файл по заданному смещению.
 * @param fd - файловый дескриптор
 * @param free_space - записываемая структура
 * @param offset - заданное смещение в байтах
 * @return true, в случае успеха, иначе false
 */
bool write_free_space_to_db(int32_t fd, struct free_space space, int64_t offset);

/** Считывает struct node из файлf по заданному смещению.
 * @param fd - файловый дескриптор
 * @param offset - заданное смещение в байтах
 * @return struct node - считанная структура
 */
struct node read_node_header_from_db(int32_t fd, int64_t offset);

/** Считывает struct node из файлf по заданному смещению.
 * @param fd - файловый дескриптор
 * @param offset - заданное смещение в байтах
 * @return struct node - считанная структура
 */
struct node read_node_from_db(int32_t fd, int64_t offset);

struct free_space read_free_space_from_db(int32_t fd, int64_t offset); // to debug TODO delete
#endif //LLP_DATABASE_NODE_H
