#ifndef LLP_DATABASE_COMMANDS_H
#define LLP_DATABASE_COMMANDS_H
#include "../dbstruct/node.h"

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


#endif //LLP_DATABASE_COMMANDS_H
