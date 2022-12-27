#ifndef LLP_DATABASE_COMMANDS_H
#define LLP_DATABASE_COMMANDS_H
#include <cinttypes>
#include <unordered_map>
#include <string>

enum command_type {
    ADD,
    UPDATE,
    FIND_BY_ID,
    FIND_BY_PARENT,
    FIND_ALL,
    DELETE,
};
// TODO добавить описания
// TODO подумать над структурой запроса
struct command {

};

int64_t add_node(int32_t fd, int64_t parent_id, const std::unordered_map<std::string, std::string> &node_data);

#endif //LLP_DATABASE_COMMANDS_H
