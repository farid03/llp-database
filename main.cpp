#include <unordered_map>
#include "file_workers/file_utils.h"
#include "dbstruct/tree_header.h"
#include "commands/commands.h"

int main() {
    int32_t fd = open_file(".data");
    std::unordered_map<std::string, data_type> schema = {}; // TODO возможно не помешают метод создания новых нод на основе схемы
    schema["name"] = STRING;
    schema["parent_id"] = INT;
    schema["age"] = INT;
    std::unordered_map<std::string, std::string> first_node = {};
    first_node["name"] = "aboba";
    first_node["parent_id"] = "-1";
    first_node["age"] = "13";

    bool res = initialize_db(fd, schema, first_node);
    auto header = get_tree_header_from_db(fd);
    struct node n = {0, get_next_node_id(fd), 0, 0, 0, 0, 0, first_node};
    auto offset = write_node_to_db(fd, n);
    auto nde = read_node_from_db(fd, offset);
    // во всех методах применяется эвристика, что бд уже валидна, поэтому нужен валидный хедер файла изначально

    // TODO проверить работоспособность методов для записи и чтения struct node

    // сделать ключ для запуска -- обновить или продолжить в файле
//    нужно сначала проверить что у базы данных есть хедер, если есть,
//    то читаем и работаем, если нет,
//    то создаем хедер и требуемую схему

    return 0;
}
