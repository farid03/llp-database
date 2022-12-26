#ifndef LLP_DATABASE_COMMANDS_H
#define LLP_DATABASE_COMMANDS_H
#include <cinttypes>

enum command_type {
    ADD,
    UPDATE,
    FIND_BY_ID,
    FIND_BY_PARENT,
    FIND_ALL,
    DELETE,
};

// TODO подумать над структурой запроса
struct command {

};

int32_t get_next_node_id(int32_t fd);
int64_t add_node(int32_t fd, const std::unordered_map<std::string, std::string> &node_data, bool is_update);

#endif //LLP_DATABASE_COMMANDS_H
