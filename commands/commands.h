#ifndef LLP_DATABASE_COMMANDS_H
#define LLP_DATABASE_COMMANDS_H

#include <cinttypes>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>
#include "iterator/result_iterator.h"

enum command_type {
    ADD,
    UPDATE,
    FIND,
    DELETE
};

struct query { // query должен строить пользователь, который пользуется модулем
    command_type command;
    struct condition condition;

    query(command_type command, struct condition condition) : command(command), condition{std::move(condition)} {}
};

/** Добавляет новый документ в дерево.
 * @param fd - файловый дескриптор
 * @param parent_id - id родителя добавляемого узла
 * @param node_data - поля добавляемого узла | node_data = map[field_name] = data
 * @return возвращает id записанного узла, -1 в случае неудачи
 */
int64_t add_node(int32_t fd, int32_t parent_id, const std::unordered_map<std::string, std::string> &node_data);

/**
 * Возвращает документ по заданному id.
 * @param fd - файловый дескриптор
 * @param id - id заданного узла
 * @return поля найденного узла в формате node_data = map[field_name] = data. \n
 * id и parent_id (при наличии) включены в результат
 */
std::unordered_map<std::string, std::string> find_node_by_id(int32_t fd, int32_t id);

/**
 * Осуществляет поиск в дереве документов подходящих заданному условию.
 * @param fd - файловый дескриптор
 * @param condition - условие поиска
 * @return итератор по найденным данным. \n
 * если вернулся невалидный итератор (is_valid = false), то данных удовлетворяющих условию в дереве нет
 */
struct result_iterator find(int32_t fd, const struct condition &condition);

/**
 * Возвращает все дерево документов.
 * @param fd - файловый дескриптор
 * @return итератор по всем элментам дерева документов
 */
struct result_iterator find_all(int32_t fd);

/**
 * Обновляет поля заданного узла дерева.
 * @param fd - файловый дескриптор
 * @param id - id обновляемого узла
 * @param node_data - новые поля обновляемого узла (должны соответствовать схеме данных модуля)
 * @return true в случае успешности операции, иначе false
 */
bool update_node(int32_t fd, int32_t id, const std::unordered_map<std::string, std::string> &node_data);

/**
 * Рекурсивно удаляет заданный узел из дерева.
 * @param fd - файловый дескриптор
 * @param id - id удаляемого узла
 * @return true в случае успешности операции, иначе false
 * @details удаляются сам узел с заданным id и все его потомки
 */
bool delete_node(int32_t fd, int32_t id);

#endif //LLP_DATABASE_COMMANDS_H
