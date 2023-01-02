#include <unordered_map>
#include "dbstruct/tree_header.h"
#include "commands/commands.h"
#include "commands/iterator/result_iterator.h"
#include "test/generator/random_generator.h"
#include "test/test.h"

// TODO есть места где можно оптимизировать read|write node, тк не всегда нужна сериализованная часть узла

int main() {
    std::unordered_map<std::string, data_type> schema = {
            {"word",  STRING},
            {"count", INT},
            {"flag",  BOOL},
            {"ratio", FLOAT}
    };

    std::vector<std::unordered_map<std::string, std::string>> nodes = {
            {{"word", "w1"}, {"count", "12"},{"flag", "false"},{"ratio", "213.2"}},             // 0
            {{"word", "w1avfa"}, {"count", "13"},{"flag", "true"},{"ratio", "21.2"}},           // 1
            {{"word", "w123sdv"}, {"count", "14"},{"flag", "false"},{"ratio", "213.2123"}},     // 2
            {{"word", "w1sdcsd"}, {"count", "15"},{"flag", "true"},{"ratio", "213.2"}},         // 3
            {{"word", "w"}, {"count", "16"},{"flag", "true"},{"ratio", "0.2"}},                 // 4
            {{"word", "wsssss1"}, {"count", "177"},{"flag", "false"},{"ratio", "23.2"}},        // 5
            {{"word", "w1sdcs"}, {"count", "188"},{"flag", "false"},{"ratio", "1.2"}},          // 6
            {{"word", "w1sdcsdcssssss"}, {"count", "199"},{"flag", "true"},{"ratio", "2123.2"}},// 7
    };

    auto fd = initialize_db(".data", schema, nodes[0]);
    std::vector<bool> res(5);
    res[0] = add_node(fd, 0, nodes[1]);
    res[3] = add_node(fd, 0, nodes[4]);
    res[4] = add_node(fd, 0, nodes[5]);
    res[1] = add_node(fd, 1, nodes[2]);
    res[2] = add_node(fd, 1, nodes[3]);

    std::unordered_map<std::string, std::string> node = {
            {"word",  get_random_string_from_array()},
            {"count", std::to_string(get_random_int(1, 23))},
            {"flag",  (get_random_bool()) ? "true" : "false"},
            {"ratio", std::to_string(get_random_float(0, 23))}
    };

    auto all = find_all(fd);
    all.print();

    auto node_by_id = find_node_by_id(fd, 0);
    printf("--- Find_by_ID ---\n");
    for (const auto &name_to_value: node_by_id) {
        printf("%s: %s, ", name_to_value.first.c_str(), name_to_value.second.c_str());
    }
    printf("\n\n");

    auto node_by_parent_id = find(fd, condition("parent_id", "0", EQUAL));
    node_by_parent_id.print();

    auto node_by_condition = find(fd, condition("count", "2222", EQUAL));
    node_by_condition.print();

    node_by_condition = find(fd, condition("count", "2222", NOT_EQUAL));
    node_by_condition.print();

    auto upd = update_node(fd, 2, nodes[7]);
    all = find_all(fd);
    all.print();

    auto dlt = delete_node(fd, 2);
    all = find_all(fd);
    all.print();

    return 0;
}