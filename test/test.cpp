#include <unordered_set>
#include "test.h"
#include "../commands/commands.h"
#include "generator/random_generator.h"
#include "../file_workers/file_utils.h"

#define MIN 0
#define MAX 100
#define LEN 16



void random_insert_test(int32_t fd, int32_t count) {
    auto header = get_tree_header_from_db(fd);
    auto nodes_counter = 1;

    printf("elem_count,time\n");
    for (int i = 0; i < count; ++i) {
        auto node = generate_random_node_by_schema(header.schema, MIN, MAX, LEN);
        auto parent_id = get_random_int(0, nodes_counter - 1);

        unsigned int start_time =  clock(); // начальное время
        auto id = add_node(fd, parent_id, node);
        unsigned int end_time = clock(); // конечное время
        unsigned int search_time = end_time - start_time;
        printf("%d,%f\n", i, (double) search_time / CLOCKS_PER_SEC);
        ++nodes_counter;

    }
}

void random_update_test(int32_t fd, int32_t count) {
    auto header = get_tree_header_from_db(fd);
    auto nodes_counter = 1;

    printf("elem_count,time\n");
    for (int i = 0; i < count; ++i) {
        auto node = generate_random_node_by_schema(header.schema, MIN, MAX, LEN);
        auto parent_id = get_random_int(0, nodes_counter - 1);
        auto id = add_node(fd, parent_id, node);
        ++nodes_counter;
        auto node_to_update_id = get_random_int(1, nodes_counter - 1);
        auto updated_node = generate_random_node_by_schema(header.schema, MIN, MAX, LEN);

        unsigned int start_time =  clock(); // начальное время
        auto updated = update_node(fd, node_to_update_id, updated_node);
        unsigned int end_time = clock(); // конечное время
        unsigned int search_time = end_time - start_time;
        printf("%d,%f\n", i, (double) search_time / CLOCKS_PER_SEC);
    }
}

void random_find_by_id_test(int32_t fd, int32_t count) {
    auto header = get_tree_header_from_db(fd);
    auto nodes_counter = 1;

    printf("elem_count,time\n");
    for (int i = 0; i < count; ++i) {
        auto node = generate_random_node_by_schema(header.schema, MIN, MAX, LEN);
        auto parent_id = get_random_int(0, nodes_counter - 1);
        auto id = add_node(fd, parent_id, node);
        ++nodes_counter;

        unsigned int start_time =  clock(); // начальное время
        auto elem = find_node_by_id(fd, get_random_int(0, nodes_counter - 1));
        unsigned int end_time = clock(); // конечное время
        unsigned int search_time = end_time - start_time;
        printf("%d,%f\n", i, (double) search_time / CLOCKS_PER_SEC);

    }
}

void random_find_by_condition_test(int32_t fd, const struct condition& condition, int32_t count) {
    auto header = get_tree_header_from_db(fd);
    auto nodes_counter = 1;

    printf("elem_count,time\n");
    for (int i = 0; i < count; ++i) {
        auto node = generate_random_node_by_schema(header.schema, MIN, MAX, LEN);
        auto parent_id = get_random_int(0, nodes_counter - 1);
        auto id = add_node(fd, parent_id, node);
        ++nodes_counter;

        unsigned int start_time =  clock(); // начальное время
        auto elem = find(fd, condition);
        while (elem.proceed()); // итерирумся по всем элементам
        unsigned int end_time = clock(); // конечное время
        unsigned int search_time = end_time - start_time;
        printf("%d,%f\n", i, (double) search_time / CLOCKS_PER_SEC);

    }
}

void random_find_by_parent_id_test(int32_t fd, int32_t count) {
    auto header = get_tree_header_from_db(fd);
    auto nodes_counter = 1;
    struct condition cond = condition("parent_id", "0", EQUAL);

    printf("elem_count,time\n");
    for (int i = 0; i < count; ++i) {
        auto node = generate_random_node_by_schema(header.schema, MIN, MAX, LEN);
        auto parent_id = get_random_int(0, nodes_counter - 1);
        auto id = add_node(fd, parent_id, node);
        ++nodes_counter;
        cond.value = std::to_string(get_random_int(0, nodes_counter - 1)); // выбираем рандомный parent_id
        unsigned int start_time =  clock(); // начальное время
        auto elem = find(fd, cond);
        while (elem.proceed()); // итерирумся по всем элементам
        unsigned int end_time = clock(); // конечное время
        unsigned int search_time = end_time - start_time;
        printf("%d,%f\n", i, (double) search_time / CLOCKS_PER_SEC);
    }
}

void random_delete_without_child_test(int32_t fd, int32_t count) {
    auto header = get_tree_header_from_db(fd);
    auto nodes_id = std::unordered_set<int32_t>{0};

    printf("elem_count,time\n");
    for (int i = 0; i < count; ++i) {
        auto node = generate_random_node_by_schema(header.schema, MIN, MAX, LEN);
        auto parent_id = *nodes_id.begin();
        auto id = add_node(fd, parent_id, node);
        nodes_id.insert(id);
        if (parent_id == 0) {
            continue;
        }
        unsigned int start_time =  clock(); // начальное время
        auto del = delete_node(fd, id);
        unsigned int end_time = clock(); // конечное время
        unsigned int search_time = end_time - start_time;
        printf("%d,%f\n", i, (double) search_time / CLOCKS_PER_SEC);
        nodes_id.erase(id);
        id = add_node(fd, parent_id, node);
    }
}

void random_delete_with_child_test(int32_t fd, int32_t count) {
    auto header = get_tree_header_from_db(fd);
    auto nodes_id = std::unordered_set<int32_t>{0};

    auto node = generate_random_node_by_schema(header.schema, MIN, MAX, LEN);
    auto parid = add_node(fd, 0, node);

    printf("elem_count,time\n");
    for (int i = 1; i < count; ++i) {
        auto parent_id = parid;
        int32_t id;
        for (int j = 0; j < i; ++j) {
            node = generate_random_node_by_schema(header.schema, MIN, MAX, LEN);
            id = add_node(fd, parent_id, node);
            nodes_id.insert(id);
        }

        unsigned int start_time =  clock(); // начальное время
        auto del = delete_node(fd, parent_id);
        unsigned int end_time = clock(); // конечное время
        unsigned int search_time = end_time - start_time;
        printf("%d,%f\n", i, (double) search_time / CLOCKS_PER_SEC);

        nodes_id.clear();
        nodes_id.insert(0);
        parid = add_node(fd, 0, node);
        nodes_id.insert(parid);
    }
}

void random_file_size_test(int32_t fd, int32_t count) {
    auto header = get_tree_header_from_db(fd);
    auto nodes_id = std::unordered_set<int32_t>{0};

    printf("elem_count,file_size\n");
    for (int i = 0; i < count; ++i) {
        auto node = generate_random_node_by_schema(header.schema, MIN, MAX, LEN);
        auto parent_id = *nodes_id.begin();
        auto id = add_node(fd, parent_id, node);
        nodes_id.insert(id);
        if (parent_id == 0) {
            continue;
        }
        auto del = delete_node(fd, id);
        nodes_id.erase(id);
        id = add_node(fd, parent_id, node);
        printf("%d,%zu\n", i, get_file_size(fd));
    }
}

void run_all_test(const char *file_name,
                  std::unordered_map<std::string, data_type> schema,
                  std::unordered_map<std::string, std::string> first_node,
                  int32_t test_count) {
    srand(time(0)); // рандомизация генератора случайных чисел

    auto fd = initialize_db(".data", schema, first_node);
    random_insert_test(fd, test_count);
    printf("$,$\n");
    close_db(fd);

    fd = initialize_db(".data", schema, first_node);
    random_update_test(fd, test_count);
    printf("$,$\n");
    close_db(fd);

    fd = initialize_db(".data", schema, first_node);
    random_find_by_id_test(fd, test_count);
    printf("$,$\n");
    close_db(fd);

    fd = initialize_db(".data", schema, first_node);
    random_find_by_condition_test(fd, condition("word", "eleifend", EQUAL), test_count);
    printf("$,$\n");
    close_db(fd);

    fd = initialize_db(".data", schema, first_node);
    random_find_by_parent_id_test(fd, test_count);
    printf("$,$\n");
    close_db(fd);

    fd = initialize_db(".data", schema, first_node);
    random_delete_without_child_test(fd, test_count);
    printf("$,$\n");
    close_db(fd);

    fd = initialize_db(".data", schema, first_node);
    random_file_size_test(fd, test_count);
    printf("$,$\n");
    close_db(fd);
}