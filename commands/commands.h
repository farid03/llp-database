#ifndef LLP_DATABASE_COMMANDS_H
#define LLP_DATABASE_COMMANDS_H
#include <cinttypes>
#include <unordered_map>
#include <string>
#include <list>

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
std::unordered_map<std::string, std::string> find_node_by_id(int32_t fd, int32_t id);
std::list<std::unordered_map<std::string, std::string>> find_node_by_parent(int32_t fd, int32_t parent_id);
std::list<std::unordered_map<std::string, std::string>> find_all(int32_t fd);
std::list<std::unordered_map<std::string, std::string>>
find_node_by_condition(int32_t fd, const std::pair<std::string, std::string>& condition);

#endif //LLP_DATABASE_COMMANDS_H
