#include <unordered_map>
#include "dbstruct/tree_header.h"
#include "commands/commands.h"
#include "dbstruct/node.h"

int main() {

    std::unordered_map<std::string, data_type> schema = {}; // TODO возможно не помешают метод создания новых нод на основе схемы
    schema["name"] = STRING;
    schema["age"] = INT;
    std::unordered_map<std::string, std::string> first_node = {};
    first_node["name"] = "aboba";
    first_node["age"] = "13";

    auto fd = initialize_db(".data", schema, first_node);  // FIXME запретить удаление и изменения нулевой ноды
    auto header = get_tree_header_from_db(fd);

    std::unordered_map<std::string, std::string> first_node2 = {};
    first_node2["name"] = "aboba2";
    first_node2["age"] = "2222";
    // TODO возможно стоит поменять у add_node возвращаемое значение на id добавленной ноды
    auto id = add_node(fd, 0, first_node2); // FIXME неправильны id_to_offset
    auto nde = read_node_from_db(fd, header.first_node);
    // TODO написать функции для работы с индексами
    nde = read_node_from_db(fd, nde.first_child);
    // во всех методах применяется эвристика, что бд уже валидна, поэтому нужен валидный хедер файла изначально
    auto all = find_all(fd);
    auto node_by_id = find_node_by_id(fd, 0);
    auto node_by_parent_id = find_node_by_parent(fd, 0);
    auto node_by_condition = find_node_by_condition(fd, {"age", "13"});
    // TODO проверить работоспособность методов для записи и чтения struct node

    // сделать ключ для запуска -- обновить или продолжить в файле
//    нужно сначала проверить что у базы данных есть хедер, если есть,
//    то читаем и работаем, если нет,
//    то создаем хедер и требуемую схему
    return 0;
}
