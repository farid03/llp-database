#include "node.h"
#include "tree_header.h"
#include "../file_workers/file_utils.h"

/** Записывает struct free_space в файл по заданному смещению.
 * @param fd - файловый дескриптор
 * @param free_space - записываемая структура
 * @param offset - заданное смещение в байтах
 * @return struct stat - информация о файле fd
 */
struct stat write_free_space_to_db(int32_t fd, struct free_space space, int64_t offset) {
    struct stat m_stat_buf = {};
    write_into_db(fd, &m_stat_buf, offset, &space, sizeof(space));

    return m_stat_buf;
}

/** Считывает struct free_space в файл по заданному смещению.
 * @param fd - файловый дескриптор
 * @param offset - заданное смещение в байтах
 * @return struct free_space - считанная структура
 */
struct free_space get_free_space_struct_from_db(int32_t fd, int64_t offset) {
    struct free_space space = { };
    read_from_db(fd, offset, &space, sizeof(space));

    return space;
}

/** Возвращает заданное смещение последнего "осовобожденного пространства" (struct free_space).
 * @param fd - файловый дескриптор
 * @return смещение последнего освобожденного пространства
 */
int64_t get_last_free_space_offset(int32_t fd) {
    struct tree_header header = get_tree_header_from_db(fd);
    struct free_space space = header.first_free_space;
    struct free_space previous_space = { 0 };
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
int64_t add_free_space_to_list(int32_t fd, struct node node_to_free) {
    struct free_space last_free_space = get_free_space_struct_from_db(fd, get_last_free_space_offset(fd));
    struct free_space space = { 0 };
    last_free_space.next = node_to_free.offset;
    space.size = node_to_free.size;
    write_free_space_to_db(fd, space, node_to_free.offset);

    return last_free_space.next;
}

/** Записывает struct node в файл по заданному смещению.
 * @param fd - файловый дескриптор
 * @param node - записываемая структура
 * @param offset - заданное смещение в байтах
 * @return struct stat - информация о файле fd
 */
struct stat write_node_to_db(int32_t fd, struct node node, int64_t offset) {
    struct stat m_stat_buf = {};
    write_into_db(fd, &m_stat_buf, offset, &node, sizeof(node));

    return m_stat_buf;
}

/** Записывает struct node в первый подходящий по размерам free_space, иначе в конец файла.
 * @param fd - файловый дескриптор
 * @param node - записываемая структура
 * @return struct stat - информация о файле fd
 */
struct stat write_node_to_db(int32_t fd, struct node node) {
    struct tree_header header = get_tree_header_from_db(fd);
    struct free_space space = header.first_free_space;
    int64_t space_offset = 0;
    while (space.next != NULL && space.size < sizeof(node)) {
        space_offset = space.next;
        space = get_free_space_struct_from_db(fd, space.next);
    }
    if (space_offset != 0 && space.size > sizeof(node)) { // если есть подходящее освобожденное пространство
        node.size = space.size;
        node.offset = space_offset;
        return write_node_to_db(fd, node, space_offset);
    }

    struct stat64 m_stat_buff = {};
    int64_t offset = fstat64(fd, &m_stat_buff); //TODO переделать под windows
    if (offset == 0) {
        offset = m_stat_buff.st_size;
        node.offset = offset;
        return write_node_to_db(fd, node, offset);
    }

    printf("Error in function write_node_to_db (fstat). Errno: %d\n ", errno);
    return {};
}