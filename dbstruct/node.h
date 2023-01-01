#ifndef LLP_DATABASE_NODE_H
#define LLP_DATABASE_NODE_H

#include <cstdint>
#include <string>
#include <unordered_map>

/**
 * Структура представляющая из себя узлы записываемы в файл документного дерева.
 */
struct node {
    /**
     * Смещение от начала документа (адрес) записанного узла.
     * @details Задается при записи в файл write_node_to_db
     */
     int64_t offset;
    /**
     * Идентификатор записанного узла.
     * @details Задается в add_node
     */
    int32_t id;
    /**
     * Смещение от начала документа (адрес) родителя узла. \n
     * Равняется 0 в случае отсутствия.
     * @details Задается в add_node
     */
    int64_t parent;
    /**
     * Смещение от начала документа (адрес) предыдущего ребенка родителя текущего узла. \n
     * Равняется 0 в случае отсутствия.
     * @details Задается в add_node
     */
    int64_t prev;
    /**
     * Смещение от начала документа (адрес) следующего ребенка родителя текущего узла. \n
     * Равняется 0 в случае отсутствия.
     * @details Задается в add_node
     */
    int64_t next;
    /**
     * Смещение от начала документа (адрес) первого ребенка текущего узла. \n
     * Равняется 0 в случае отсутствия.
     * @details Задается в add_node
     */
    int64_t first_child;
    /**
     * Реальный размер структуры записанного узла в файле (с учетом сериализованной части).
     * @details Задается при записи в файл write_node_to_db
     */
    uint64_t size;
    /**
     * Размер занимаемой памяти записанного узла в файле (с учетом сериализованной части). \n
     * Требуется для устранения утечек памяти в файле, т.к. узел может лежать
     * в свободном пространстве, большем чем ему нужно.
     * @details Задается при записи в файл write_node_to_db
     */
    uint64_t r_size;
    /**
     * Все поля узла дерева в соответствии с заданной схемой данных.
     * @details Задается и валидируется в add_node
     */
    std::unordered_map<std::string, std::string> data;
};

/**
 * Структура представляющая из себя блоки освобожденной памяти (после удаления или изменения узлов) в файле.
 */
struct free_space {
    /// Адрес в файле предыдущего блока, 0 в случае отсутствия.
    int64_t prev;
    /// Адрес в файле предыдущего блока, 0 в случае отсутствия.
    int64_t next;
    /// Размер блока (= sizeof(free_space) + space)
    uint32_t size;
};

/** Добавляет struct free_space в связный список освобожденных пространств (struct free_space) в файле и записывает в
 * место структуры node структуру освобожденного пространства.
 * @param fd - файловый дескриптор
 * @param node_to_free - перезаписываемая (освобождаемая) структура
 * @return offset, если запись прошла успешно, 1L в случае неудачи
 * @attention Нужно обновлять ссылки у родителя и соседних детей на node_to_free отдельно!
 */
int64_t remove_node_from_db(int32_t fd, const struct node &node_to_free);

/** Записывает struct node в первый подходящий по размерам free_space, иначе в конец файла.
 * @param fd - файловый дескриптор
 * @param node - записываемая структура
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

/** Записывает struct node в файл по заданному смещению без node.data. \n
 * Используется для оптимизации в случае отсутствия необходимости записи полей структуры.
 * @param fd - файловый дескриптор
 * @param node - записываемая структура
 * @param offset - заданное смещение в байтах
 * @return true, в случае успеха, иначе false
 */
bool write_node_header_to_db(int32_t fd, struct node node, int64_t offset);

/** Записывает struct free_space в файл по заданному смещению.
 * @param fd - файловый дескриптор
 * @param free_space - записываемая структура
 * @param offset - заданное смещение в байтах
 * @return true, в случае успеха, иначе false
 */
bool write_free_space_to_db(int32_t fd, struct free_space space, int64_t offset);

/** Считывает struct node из файла по заданному смещению без node.data.
 * Используется для оптимизации в случае отсутствия необходимости чтения полей структуры.
 * @param fd - файловый дескриптор
 * @param offset - заданное смещение в байтах
 * @return struct node - считанная структура
 */
struct node read_node_header_from_db(int32_t fd, int64_t offset);

/** Считывает struct node из файла по заданному смещению.
 * @param fd - файловый дескриптор
 * @param offset - заданное смещение в байтах
 * @return struct node - считанная структура
 */
struct node read_node_from_db(int32_t fd, int64_t offset);

#endif //LLP_DATABASE_NODE_H
