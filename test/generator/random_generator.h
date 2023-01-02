#ifndef LLP_DATABASE_RANDOM_GENERATOR_H
#define LLP_DATABASE_RANDOM_GENERATOR_H

#include <cinttypes>
#include <string>
#include "../../dbstruct/tree_header.h"

// Функция генерирования случайного целочисленного числа в указанных пределах.
// Диапазон чисел: [from, to]
int32_t get_random_int(int32_t from, int32_t to);

// Функция генерирования случайного bool значения.
bool get_random_bool();

// Функция генерирования случайного числа с плавающей точкой в указанных пределах.
// Диапазон чисел: [from, to]
float get_random_float(float from, float to);

// Возвращает случайное слово из заранее объявленного массива строк
std::string get_random_string_from_array();

// Функция генерирования случайной строки указанного размера.
std::string get_random_string(int32_t len);

// Генерирует данные узла на основе заданной схемы
std::unordered_map<std::string, std::string> generate_random_node_by_schema(
        const std::unordered_map<std::string, data_type> &schema,
        int32_t min, int32_t max, int32_t str_len
);

#endif //LLP_DATABASE_RANDOM_GENERATOR_H