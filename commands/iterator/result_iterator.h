#ifndef LLP_DATABASE_RESULT_ITERATOR_H
#define LLP_DATABASE_RESULT_ITERATOR_H

#include <cinttypes>
#include <unordered_map>
#include <utility>
#include "../../dbstruct/tree_header.h"
#include "../../dbstruct/node.h"

enum comparison_operator {
    NOP,
    EQUAL,
    NOT_EQUAL
};

const std::string comparison_operator_strings[] = {
        "NOP",
        "EQUAL",
        "NOT_EQUAL"
};

/// Структура для описания условий по которым нужно искать данные
struct condition {
    std::string name_value;
    std::string value;
    comparison_operator operation;

    condition(std::string name_of_value, std::string value, comparison_operator operation)
            : name_value{std::move(name_of_value)}, value{std::move(value)}, operation{operation} {}

    // To debug
    std::string to_string() const {
        return name_value + ' ' + comparison_operator_strings[operation] + ' ' + value;
    }
};

/**
 * Проверяет корректность условия для заданного узла дерева.
 * @param fd - файловый дескриптор
 * @param node - узел, который проверяется на условие
 * @param cond - условие проверки
 * @return true в случае совпадения, иначе false
 */
bool check(int32_t fd, struct node node, const struct condition &cond);

/**
 * Итератор по результату find-функций.
 * @warning Валидность итератора после изменении базы данных не гарантируется.
 */
struct result_iterator {
    int32_t fd;
    /// Текущее состояние итератора, данные валидны только при наличии соответствующего флага
    struct node node;
    /// Условие, по которому возвращаются данные
    struct condition condition;
    /**
     * Флаг валидности данных, на которые указывает итератор.
     * @details false выставляется если в дереве нет узлов подходящих под заданный condition,
     * либо после попытки сделать proceed уже конечной ноды.
     * @warning Невалидный итератор не дает гарантию на валидность возвращаемых данных данных!
     */
    bool is_valid = true;

    result_iterator() : fd{-1}, is_valid{false}, condition{"_nop", "_nop", NOP} {}

    result_iterator(int32_t fd, struct condition cond) : fd{fd}, condition{std::move(cond)} {
        this->node = read_node_from_db(fd, get_tree_header_without_schema_from_db(fd).first_node);
        if (check(fd, node, this->condition)) {
            this->is_valid = true;
        } else {
            this->is_valid = proceed();
        }
    }

    /**
     * Переносит итератор на следующий подходящий элемент в дереве.
     * @attention При вызове proceed() у невалидного итератора,
     * состояние итератора не будет меняться и будет возвращаться всегда false
     * @return true в случае, если удалось успешно перейти к следующему элементу,
     * false в случае, если следующего элемента нет (узел в предыдущем состоянии был последним)
     */
    bool proceed();

    /**
     * Возвращает данные узла в формате map[name_of_value] = value
     * @attention id и parent_id включены в результат
     */
    std::unordered_map<std::string, std::string> operator*();

    // To debug
    void print();
};

#endif //LLP_DATABASE_RESULT_ITERATOR_H
