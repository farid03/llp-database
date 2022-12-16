#ifndef LLP_DATABASE_NODE_H
#define LLP_DATABASE_NODE_H

#include <cstdint>
#include <string>

enum data_type {
    BOOL = 0,
    INT,
    FLOAT,
    STRING
};

struct data { // данные, записанные в node
    struct data_type; // кажется это тут не должно быть
    std::string data;
};

struct node {
    int64_t offset;
    int64_t parent;
    int64_t prev;
    int64_t next;
    int64_t first_child;
    uint32_t size; // количество памяти, которое реально занимает структура
    // (т.к. она может лежать в свободном пространстве, большем чем ему нужно)
    // TODO задать минимальный размер для структуры
    struct data;
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
int64_t add_free_space_to_list(int32_t fd, struct node node_to_free);

/** Записывает struct node в первый подходящий по размерам free_space, иначе в конец файла.
 * @param fd - файловый дескриптор
 * @param node - записываемая структура
 * @return struct stat - информация о файле fd
 */
struct stat write_node_to_db(int32_t fd, struct node node);

#endif //LLP_DATABASE_NODE_H
