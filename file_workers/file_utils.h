//
// Created by farid on 05.11.22.
//

#ifndef LLP_DATABASE_FILE_UTILS_H
#define LLP_DATABASE_FILE_UTILS_H


#include <cstdio>
#include <cinttypes>

/**
 * Статус чтения файла
 */
enum file_read_status {
    READ_OK = 0,
    READ_END_OF_FILE,
    READ_INVALID
};

/**
 * Статус записи в файл
 */
enum file_write_status {
    WRITE_OK = 0,
    WRITE_WRONG_INTEGRITY,
    WRITE_INVALID
};

/**
 * Статус открытия файла
 */
enum file_open_status {
    OPEN_OK = 0,
    OPEN_FAILED
};

/**
 * Прочитать данные из файла
 * @param file файл для чтения
 * @param buffer контейнер, который надо заполнить из файла
 * @return статус чтения
 */
enum file_read_status read_from_file(FILE *file, void *buffer, size_t size);

/**
 * Записать данные в файл
 * @param file файл для записи
 * @param buffer контейнер, который надо записать в файл
 * @return статус записи
 */
enum file_write_status write_to_file(FILE *file, void *buffer, size_t size);

/**
 * Открытие файла
 * @param file контейнер в который будет записан файловый поток
 * @param filename название файла
 * @return статус открытия
 */
enum file_open_status open_exist_file(FILE **file, char *filename);

/**
 * Создание файла
 * @param file контейнер в который будет записан файловый поток
 * @param filename название файла
 * @return статус открытия
 */
enum file_open_status open_new_file(FILE **file, char *filename);

/**
 * Закрытие файла
 * @param file файловый поток
 */
void close_file(FILE *file);

enum file_open_status open_empty_file(FILE **file, char *filename);

#endif //LLP_DATABASE_FILE_UTILS_H
