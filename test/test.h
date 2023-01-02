#ifndef LLP_DATABASE_TEST_H
#define LLP_DATABASE_TEST_H

#include <cinttypes>
#include "generator/random_generator.h"

// просто вставляем рандомные элементы
void random_insert_test(int32_t fd, int32_t count);

// обновляем рандомные элементы
void random_update_test(int32_t fd, int32_t count);

// выбираем элемент по id
void random_find_by_id_test(int32_t fd, int32_t count);

// выбираем элемент по условию и итерируемся (поиск по условию и итерацию выполнять после каждых 50 добавленных элементов)
void random_find_by_condition_test(int32_t fd, const struct condition &condition, int32_t count);

// выбираем элемент по условию (parent_id) и итерируемся (поиск по условию и итерацию выполнять после каждых 50 добавленных элементов)
void random_find_by_parent_id_test(int32_t fd, int32_t count);

// вставляем рандомные элементы и каждый после вставки удаляем (затем добавляем обратно его же)
void random_delete_without_child_test(int32_t fd, int32_t count);

// вставляем n элементов в один элемент, а затем удаляем его с детьми и так по возрастанию n замеряем время
void random_delete_with_child_test(int32_t fd, int32_t count);

// вставляем и удаляем рандомные элементы (сколько удаляем, столько и вставляем) данные нужно будет отсортировать по количеству элементов
void random_file_size_test(int32_t fd, int32_t count);

// запускаем все тесты
void run_all_test(const char *file_name,
                  std::unordered_map<std::string, data_type> schema,
                  std::unordered_map<std::string, std::string> first_node,
                  int32_t test_count);
#endif //LLP_DATABASE_TEST_H
