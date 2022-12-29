#include "node.h"
#include "../file_workers/file_utils.h"
#include "../data_process_utils/serialization/serialization.h"
#include "tree_header.h"

bool write_free_space_to_db(int32_t fd, struct free_space space, int64_t offset) {
    return write_into_file(fd, offset, &space, sizeof(space)) == offset;
}

/** Считывает struct free_space из файл по заданному смещению.
 * @param fd - файловый дескриптор
 * @param offset - заданное смещение в байтах
 * @return struct free_space - считанная структура
 */
struct free_space read_free_space_from_db(int32_t fd, int64_t offset) {
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
    int64_t space_offset = header.first_free_space;
    struct free_space space = read_free_space_from_db(fd, header.first_free_space);
    while (space.next != 0) {
        space_offset = space.next;
        space = read_free_space_from_db(fd, space.next);
    }

    return space_offset;
}

int64_t remove_node_from_db(int32_t fd, const struct node &node_to_free) {
    auto last_free_space_offset = get_last_free_space_offset(fd);
    struct free_space space = {0};
    if (last_free_space_offset != -1) { // если в листе есть освобожденные пространства, то изменяем указатель на след
        auto last_free_space = read_free_space_from_db(fd, last_free_space_offset);
        last_free_space.next = node_to_free.offset;
        write_free_space_to_db(fd, last_free_space, last_free_space_offset);
        space.prev = last_free_space_offset;
    } else { // иначе записываем в хедер файла первое освобожденное пространство
        auto header = get_tree_header_without_schema_from_db(fd);
        header.first_free_space = node_to_free.offset;
        set_tree_header_to_db(fd, header);
    }
    space.size = node_to_free.r_size;
    write_free_space_to_db(fd, space, node_to_free.offset);

    return node_to_free.offset;
}

bool write_node_header_to_db(int32_t fd, struct node node, int64_t offset) {
    return write_into_file(fd, offset, &node, offsetof(struct node, data) - 1) == offset;
}

bool write_node_to_db(int32_t fd, struct node node, int64_t offset) {
    auto cfd = open_file(".cache");
    serialize_and_cache_node_data(node.data);
    node.size = offsetof(struct node, data) - 1 + get_file_size(cfd);
    if (node.r_size == 0 || node.r_size < node.size) {
        node.r_size = node.size;
    }
    auto data_offset = move_from_cache_to_db(fd, cfd, offset + offsetof(struct node, data));
    auto node_header_offset = write_into_file(fd, offset, &node, offsetof(struct node, data) - 1);
    close_file(cfd);

    return node_header_offset == offset && data_offset == offset + offsetof(struct node, data);
}

inline int64_t count_node_size(struct node &node) {
    auto cfd = open_file(".cache");
    serialize_and_cache_node_data(node.data);
    auto size = offsetof(struct node, data) - 1 + get_file_size(cfd);
    close_file(cfd);

    return size;
}

int64_t write_node_to_db(int32_t fd, struct node node) {
    struct tree_header header = get_tree_header_without_schema_from_db(fd);
    struct free_space space = {0};
    int64_t space_offset = 0;

    if (header.first_free_space == 0) {
        goto write_node_to_eof;
    }
    node.size = count_node_size(node);// надо валидировать сайз перед апдейтом/записью для поиска верного free_space
    space_offset = header.first_free_space;
    space = read_free_space_from_db(fd, header.first_free_space);
    while (space.next != 0 && space.size < node.size) { // поиск первого подходящего пространства
        space_offset = space.next;
        space = read_free_space_from_db(fd, space.next);
    }

    if (space_offset != 0 && space.size > node.size) { // если есть подходящее освобожденное пространство
        node.r_size = space.size;
        node.offset = space_offset;
        if (!write_node_to_db(fd, node, space_offset)) {
            printf("Error in write_node_to_db! (1)\n");
            return 1;
        }
        if (space.prev == 0) { // если использованный спейс был указан в хедере (первым)
            header = get_tree_header_without_schema_from_db(fd);
            header.first_free_space = space.next;
            set_tree_header_to_db(fd, header);
            if (space.next != 0) { // если следующий после первого спейса существует, то указываем ему, что он первый
                auto next = read_free_space_from_db(fd, space.next);
                next.prev = 0;
                write_free_space_to_db(fd, next, space.next);
            }
        } else {
            auto prev = read_free_space_from_db(fd, space.prev);
            if (space.next != 0) { // если использованный спейс не последний, то указываем соседям друг друга
                auto next = read_free_space_from_db(fd, space.next);
                prev.next = space.next;
                next.prev = space.prev;
                write_free_space_to_db(fd, next, space.next);
                write_free_space_to_db(fd, prev, space.prev);
            } else { // иначе указываем предыдущему соседу, что он последний
                prev.next = 0;
                write_free_space_to_db(fd, prev, space.prev);
            }
        }

        return space_offset;
    }

    write_node_to_eof: // если нет подходящих пространств и нужно записать ноду в конец файла
    int64_t offset = get_file_size(fd);
    node.offset = offset;
    if (!write_node_to_db(fd, node, offset)) {
        printf("Error in write_node_to_db! (3)\n");
        return 1;
    }

    return node.offset;
}

struct node read_node_header_from_db(int32_t fd, int64_t offset) {
    struct node result = {0};
    read_from_file(fd, offset, &result, offsetof(node, data) - 1);

    return result;
}

struct node read_node_from_db(int32_t fd, int64_t offset) {
    struct node result = read_node_header_from_db(fd, offset);
    result.data = deserialize_node_data(fd, offset, result.size);

    return result;
}