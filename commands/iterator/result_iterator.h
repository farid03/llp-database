#ifndef LLP_DATABASE_RESULT_ITERATOR_H
#define LLP_DATABASE_RESULT_ITERATOR_H

#include "../../dbstruct/tree_header.h"
#include "../commands.h"
#include "../../dbstruct/node.h"
#include <cinttypes>
#include <unordered_map>
#include <utility>

bool check(int32_t fd, struct node node, const struct condition &cond);

/**
 *  @warning Валидность итератора после изменении базы данных не гарантируется.
 */
struct result_iterator {
    int32_t fd;
    struct node node;
    struct condition cond;
    // false выставляется только когда мы пытаемся сделать proceed уже конечной ноды
    // невалидный итератор дает невалидные данные
    bool is_valid = true;

    result_iterator() {
        this->is_valid = false;
    }

    result_iterator(int32_t fd, struct condition cond) {
        this->fd = fd;
        this->cond = std::move(cond);
        this->node = read_node_from_db(fd, get_tree_header_without_schema_from_db(fd).first_node);
        if (check(fd, node, this->cond)) {
            this->is_valid = true;
        } else {
            this->is_valid = proceed();
        }
    }

    bool proceed();

    std::unordered_map<std::string, std::string> operator*();

    // To debug
    void print() {
        printf("--- Result Iterator | Condition: %s ---\n", this->cond.to_string().c_str());

        if (!this->is_valid) {
            printf("Iterator is not valid\n");
            printf("\n");
            return;
        }

        do {
            for (const auto &name_to_value: *(*this)) {
                printf("%s: %s, ", name_to_value.first.c_str(), name_to_value.second.c_str());
            }
            printf("\n");
        } while (this->proceed());
        printf("\n");
    }
};

#endif //LLP_DATABASE_RESULT_ITERATOR_H
