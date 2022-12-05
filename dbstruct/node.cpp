//
// Created by farid on 05.11.22.
//

#include "node.h"
#include "tree_header.h"
#include "../file_workers/file_utils.h"

/** Записывает struct free_space в файл по заданному смещению.
 * @param fd - файловый дескриптор
 * @param free_space - записываемая структура
 * @param offset - заданное смещение в байтах
 * @return struct stat - информация о файле fd
 */
struct stat write_free_space_struct_to_db(int32_t fd, struct free_space space, int64_t offset) {
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

/** Возвращает заданное смещение последнего осовобожденного пространства (struct free_space).
 * @param fd - файловый дескриптор
 * @return смещение последнего освобожденного пространства
 */
int64_t get_last_free_space_offset(int32_t fd) {
    struct tree_header header = get_tree_header_from_db(fd);
    struct free_space space = header.first_free_space;
    struct free_space previous_space = { };
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
    struct free_space space = { };
    last_free_space.next = node_to_free.offset;
    space.size = node_to_free.size;

    return last_free_space.next;
}

// TODO написать добавление node в файл по смещению
// TODO написать добавление node в конец файла
// TODO написать добавление node в в подоходящий free_space иначе в конец файла
