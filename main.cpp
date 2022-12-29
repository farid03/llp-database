#include <unordered_map>
#include "dbstruct/tree_header.h"
#include "commands/commands.h"
#include "dbstruct/node.h"
#include "file_workers/file_utils.h"
// TODO есть места где можно оптимизировать read|write node, тк не всегда нужна сериализованная часть узла

int main() {

    std::unordered_map<std::string, data_type> schema = {
            {"name", STRING},
            {"age", INT}
    }; // TODO возможно не помешают методs создания новых нод на основе схемы
    std::unordered_map<std::string, std::string> node1 = {
            {"name", "aboba1"},
            {"age", "1"}
    };

    auto fd = initialize_db(".data", schema, node1);
//    auto fd = initialize_db(".data");

    std::unordered_map<std::string, std::string> node2 = {
            {"name", "aboba22"},
            {"age", "22"}
    };
    auto id2 = add_node(fd, 0, node2); // FIXME бесконечный цикл при добавлении ноды в индекс
    // FIXME ошибка появления несуществубщего id в индексе
    std::unordered_map<std::string, std::string> node3 = {
            {"name", "aboba333"},
            {"age", "333"}
    };
    auto id3 = add_node(fd, 0, node3);

    std::unordered_map<std::string, std::string> node4 = {
            {"name", "aboba4444"},
            {"age", "4444"}
    };
    auto id4 = add_node(fd, 0, node4);

    // во всех методах применяется эвристика, что бд уже валидна, поэтому нужен валидный хедер файла изначально
    auto all = find_all(fd);
    auto node_by_id = find_node_by_id(fd, 0); // почему выводятся не все поля
    auto node_by_parent_id = find_node_by_parent(fd, 0);
    auto node_by_condition = find_node_by_condition(fd, {"age", "2222"});

    std::unordered_map<std::string, std::string> node_to_update = {
            {"name", "aboba2updated"},
            {"age", "121212"}
    };
    // FIXME проверить наличие ошибок, когда сначала записывается что-то в базу данных, а потом пытаются использовать старый хедер

    auto upd = update_node(fd, 2, node_to_update);
    auto dlt = delete_node(fd, 2);
    return 0;
}
