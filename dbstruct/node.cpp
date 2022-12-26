#include "node.h"
#include "tree_header.h"
#include "../file_workers/file_utils.h"
#include "serialization.h"

/** Записывает struct free_space в файл по заданному смещению.
 * @param fd - файловый дескриптор
 * @param free_space - записываемая структура
 * @param offset - заданное смещение в байтах
 * @return true, в случае успеха, иначе false
 */
bool write_free_space_to_db(int32_t fd, struct free_space space, int64_t offset) {
    return write_into_file(fd, offset, &space, sizeof(space)) == offset;
}

/** Считывает struct free_space из файл по заданному смещению.
 * @param fd - файловый дескриптор
 * @param offset - заданное смещение в байтах
 * @return struct free_space - считанная структура
 */
struct free_space get_free_space_struct_from_db(int32_t fd, int64_t offset) {
    struct free_space space = { 0 };
    read_from_file(fd, offset, &space, sizeof(space));

    return space;
}

/** Возвращает заданное смещение последнего "осовобожденного пространства" (struct free_space).
 * @param fd - файловый дескриптор
 * @return смещение последнего освобожденного пространства, -1 в случае, если их его нет.
 */
int64_t get_last_free_space_offset(int32_t fd) {
    struct tree_header header = get_tree_header_without_schema_from_db(fd);
    if (header.first_free_space == 0) {
        return -1;
    }
    struct free_space space = get_free_space_struct_from_db(fd, header.first_free_space);
    struct free_space previous_space = {0};
    while (space.next != 0) {
        previous_space = space;
        space = get_free_space_struct_from_db(fd, space.next);
    }

    return previous_space.next;
}

/** Добавляет struct free_space в связный список освобожденных пространств (struct free_space) в файле и записывает в
 * место структуры node структуру освобожденного пространства.
 * @param fd - файловый дескриптор
 * @param node_to_free - перезаписываемая (освобождаемая) структура
 * @return смещение записанной структуры в файле
 */
int64_t add_free_space_to_list(int32_t fd, const struct node &node_to_free) {
    auto last_free_space_offset = get_last_free_space_offset(fd);
    struct free_space space = {0};
    if (last_free_space_offset != -1) { // если в листе есть освобожденные пространства, то изменяем указатель на след
        auto last_free_space = get_free_space_struct_from_db(fd, last_free_space_offset);
        last_free_space.next = node_to_free.offset;
    } else { // иначе записываем в хедер файла первое освобожденное пространство
        auto header = get_tree_header_without_schema_from_db(fd);
        header.first_free_space = node_to_free.offset;
    }
    space.size = node_to_free.r_size;
    write_free_space_to_db(fd, space, node_to_free.offset);
    return node_to_free.next;
}

/** Записывает struct node в файл по заданному смещению.
 * @param fd - файловый дескриптор
 * @param node - записываемая структура
 * @param offset - заданное смещение в байтах
 * @return true, в случае успеха, иначе false
 */
bool write_node_to_db(int32_t fd, struct node node, int64_t offset) {
    auto header_offset = write_into_file(fd, offset, &node, offsetof(struct node, data) - 1);
    auto cfd = open_file(".cache");
    serialize_and_cache_node_data(node.data);
    auto data_offset = move_from_cache_to_db(fd, cfd, offset + offsetof(struct node, data));

    return header_offset == offset && data_offset == offset;
}

/** Записывает struct node в первый подходящий по размерам free_space, иначе в конец файла.
 * @param fd - файловый дескриптор
 * @param node - записываемая структура
 * @return true, в случае успеха, иначе false
 * @attention Не забыть записать в node.size полный размер структуры (вместе с частью сериализ) и указать id структуры!
 */
// TODO требует модификации (сериализация + отсутсвтвие free space)
bool write_node_to_db(int32_t fd, struct node node) {
    struct tree_header header = get_tree_header_without_schema_from_db(fd);
    struct free_space space = { 0 };
    int64_t space_offset = 0;
    if (header.first_free_space == 0) {
        goto write_node_to_eof;
    }

    space = get_free_space_struct_from_db(fd, header.first_free_space);
    while (space.next != 0 && space.size < sizeof(node)) {
        space_offset = space.next;
        space = get_free_space_struct_from_db(fd, space.next);
    }
    if (space_offset != 0 && space.size > sizeof(node)) { // если есть подходящее освобожденное пространство
        node.r_size = space.size;
        node.offset = space_offset;
        return write_node_to_db(fd, node, space_offset) == space_offset;
    } else if (space.size > sizeof(node)) { // если это единственное освобожд пространство и оно подходит нам
        space_offset = header.first_free_space;
        header.first_free_space = 0;
        node.r_size = space.size;
        node.offset = space_offset;
        return write_node_to_db(fd, node, space_offset) == space_offset;
    }

    write_node_to_eof: // если нет подходящих пространств и нужно записать ноду в конец файла
    int64_t offset = get_file_size(fd);
    node.offset = offset;
    return write_node_to_db(fd, node, offset) == node.offset;
}

/** Считывает struct node из файлf по заданному смещению.
 * @param fd - файловый дескриптор
 * @param offset - заданное смещение в байтах
 * @return struct node - считанная структура
 */
struct node read_node_header_from_db(int32_t fd, int64_t offset) {
    struct node result = {0};
    read_from_file(fd, offset, &result, offsetof(node, data) - 1);

    return result;
}

/** Считывает struct node из файлf по заданному смещению.
 * @param fd - файловый дескриптор
 * @param offset - заданное смещение в байтах
 * @return struct node - считанная структура
 */
struct node read_node_from_db(int32_t fd, int64_t offset) {
    struct node result = read_node_header_from_db(fd, offset);
    result.data = deserialize_node_data(fd, offset, result.size);

    return result;
}