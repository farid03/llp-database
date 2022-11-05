//
// Created by farid on 05.11.22.
//

#ifndef LLP_DATABASE_FILE_DATA_H
#define LLP_DATABASE_FILE_DATA_H

#include <cstdint>
#include <string>

enum data_type {
    BOOL = 0,
    INT,
    FLOAT,
    STRING
};

struct data {
    struct data_type;
    std::string data;
};

struct node {
    uint64_t* parent;
    uint64_t* prev;
    uint64_t* next;
    uint64_t* first_child;
    struct data;
};


#endif //LLP_DATABASE_FILE_DATA_H
