//
// Created by farid on 05.11.22.
//

#ifndef LLP_DATABASE_NODE_H
#define LLP_DATABASE_NODE_H

#include <cstdint>
#include <string>

enum data_type {
    BOOL = 0,
    INT,
    FLOAT,
    STRING
};

struct data { // данные, записанные в node
    struct data_type; // кажется это тут не должно быть
    std::string data;
};

struct node {
    int64_t offset;
    int64_t parent;
    int64_t prev;
    int64_t next;
    int64_t first_child;
    uint32_t size; // количество памяти, которое реально занимает структура
    // (т.к. она может лежать в свободном пространстве, большем чем ему нужно)
    struct data;
};

struct free_space {
    int64_t next;
    uint32_t size; // = sizeof(free_space) + space
};

#endif //LLP_DATABASE_NODE_H
