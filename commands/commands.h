#ifndef LLP_DATABASE_COMMANDS_H
#define LLP_DATABASE_COMMANDS_H
#include <cinttypes>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>

enum command_type {
    ADD,
    UPDATE,
    FIND,
    DELETE
};

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

struct condition {
    std::string name_value;
    std::string value;
    comparison_operator operation;

    condition() {}

    condition(std::string name_of_value, std::string value, comparison_operator operation) {
        this->name_value = std::move(name_of_value);
        this->value = std::move(value);
        this->operation = operation;
    }

    std::string to_string() const {
        return name_value + ' ' + comparison_operator_strings[operation] + ' ' + value;
    }
};

// TODO добавить описания
// TODO подумать над структурой запроса
struct query { // TODO query должен строить пользователь который пользуется модулем
    command_type command;
    std::vector<struct condition> conditions;
};

int64_t add_node(int32_t fd, int32_t parent_id, const std::unordered_map<std::string, std::string> &node_data);

std::unordered_map<std::string, std::string> find_node_by_id(int32_t fd, int32_t id);
struct result_iterator find(int32_t fd, const struct condition &condition);
struct result_iterator find_all(int32_t fd);
bool update_node(int32_t fd, int32_t id, const std::unordered_map<std::string, std::string>& node_data);
bool delete_node(int32_t fd, int32_t id);

#endif //LLP_DATABASE_COMMANDS_H
