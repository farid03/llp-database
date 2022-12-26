#ifndef LLP_DATABASE_NODE_H
#define LLP_DATABASE_NODE_H

#include <cstdint>
#include <string>
#include "tree_header.h"
#include <unordered_map>

struct node {
    int64_t offset;
    int64_t prev;
    int64_t next;
    int64_t first_child;
    uint64_t size;
    uint64_t r_size; // количество памяти, которое реально занимает структура
    // (т.к. она может лежать в свободном пространстве, большем чем ему нужно)
    // TODO задать минимальный размер для структуры
    std::unordered_map<std::string, std::string> data;
};

struct free_space {
    int64_t next;
    uint32_t size; // = sizeof(free_space) + space
};

/** Добавляет struct free_space в связный список освобожденных пространств (struct free_space) в файле и записывает в
 * место структуры node структуру освобожденного пространства.
 * @param fd - файловый дескриптор
 * @param node_to_free - перезаписываемая (освобождаемая) структура
 * @return смещение записанной структуры в файле
 */
int64_t add_free_space_to_list(int32_t fd, const struct node& node_to_free);

/** Записывает struct node в первый подходящий по размерам free_space, иначе в конец файла.
 * @param fd - файловый дескриптор
 * @param node - записываемая структура
 * @return true, в случае успеха, иначе false
 */
bool write_node_to_db(int32_t fd, struct node node);

/** Записывает struct free_space в файл по заданному смещению.
 * @param fd - файловый дескриптор
 * @param free_space - записываемая структура
 * @param offset - заданное смещение в байтах
 * @return true, в случае успеха, иначе false
 */
bool write_free_space_to_db(int32_t fd, struct free_space space, int64_t offset);

struct node read_node_from_db(int32_t fd, int64_t offset);
struct node read_node_header_from_db(int32_t fd, int64_t offset);
struct node read_node_data_from_db(int32_t fd, int64_t offset);
#endif //LLP_DATABASE_NODE_H
