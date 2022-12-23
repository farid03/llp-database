#ifndef LLP_DATABASE_FILE_UTILS_H
#define LLP_DATABASE_FILE_UTILS_H

#include <iostream>
#include <csignal>
#include <sys/stat.h>
#include <fcntl.h>
#include "file_utils.h"


/** Записывает буфер данныx в файл по заданному смещению.
 * @param fd - файловый дескриптор
 * @param m_stat_buf - информация о файле fd
 * @param offset - заданное смещение в байтах
 * @param data - данные записываемые в файл
 * @param n - счетчик записываемых данных в байтах
 * @return offset, если запись прошла успешно, 1L в случае неудачи
 */
uint64_t
write_into_db(const int32_t fd, struct stat *m_stat_buf, const int64_t offset, const void *data, const size_t n);

/** Считывает данные в буфер из файла по заданному смещению.
 * @param fd - файловый дескриптор
 * @param offset - заданное смещение в байтах
 * @param data - буфер, куда будут считаны данные
 * @param n - счетчик считываемых данных в байтах
 * @return количество действительно считанных байт, 1 в случае ошибки или 0 при попытке чтения в конце файла
 */
ssize_t read_from_db(const int32_t fd, const int64_t offset, void *data, const size_t n);


/** Открывает файл с заданным названием, если файла с этим названием не существует, то он будет создан.
 * @param filename - название файла
 * @return файловый дескриптор открытого файла или -1 в случае ошибки
 */
int32_t open_file(const char *filename);

/** Закрывает файл с заданным файловым дескриптором.
 * @param fd - файловый дескриптор
 */
void close_file(int32_t fd);

#endif //LLP_DATABASE_FILE_UTILS_H
