#include <unordered_map>
#include "dbstruct/tree_header.h"
#include "commands/commands.h"
#include "dbstruct/node.h"
#include "commands/iterator/result_iterator.h"
// TODO есть места где можно оптимизировать read|write node, тк не всегда нужна сериализованная часть узла

int main() {
    std::unordered_map<std::string, data_type> schema = {
            {"name", STRING},
            {"age",  INT}
    };
    std::unordered_map<std::string, std::string> node1 = {
            {"name", "aboba1"},
            {"age",  "1"}
    };

    auto fd = initialize_db(".data", schema, node1);
//    auto fd = initialize_db(".data");

    std::unordered_map<std::string, std::string> node2 = {
            {"name", "aboba22"},
            {"age",  "22"}
    };
    auto id2 = add_node(fd, 0, node2);

    std::unordered_map<std::string, std::string> node3 = {
            {"name", "aboba333"},
            {"age",  "333"}
    };
    auto id3 = add_node(fd, 0, node3);

    std::unordered_map<std::string, std::string> node4 = {
            {"name", "aboba4444"},
            {"age",  "4444"}
    };
    auto id4 = add_node(fd, 0, node4);

    // во всех методах применяется эвристика, что бд уже валидна, поэтому нужен валидный хедер файла изначально
    auto all = find_all(fd);
    all.print();

    auto node_by_id = find_node_by_id(fd, 0);
    printf("--- Find_by_ID ---\n");
    for (const auto& name_to_value: node_by_id) {
        printf("%s: %s, ", name_to_value.first.c_str(), name_to_value.second.c_str());
    }
    printf("\n");
    printf("\n");

    auto node_by_parent_id = find(fd, condition("parent_id", "0", EQUAL));
    node_by_parent_id.print();

    auto node_by_condition = find(fd, condition("age", "2222", EQUAL));
    node_by_condition.print();

    node_by_condition = find(fd, condition("age", "2222", NOT_EQUAL));
    node_by_condition.print();

    std::unordered_map<std::string, std::string> node_to_update = {
            {"name", "aboba2updated"},
            {"age",  "121212"}
    };

    auto upd = update_node(fd, 2, node_to_update);
    all = find_all(fd);
    all.print();


    auto dlt = delete_node(fd, 2);
    all = find_all(fd);
    all.print();

    return 0;
}
