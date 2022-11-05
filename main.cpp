#include <iostream>
#include "file_workers/file_utils.h"
#include "dbstruct/file_data.h"

int main() {
    FILE **file;
    file_open_status status = open_new_file(file, "sdsd.txt");
    FILE *file1 = *file;
    char as[3];
    as[0] = 'a';
    as[1] = 'a';
    as[2] = 'a';
    node node1;



    file_write_status s = write_to_file(file1, as, sizeof(as));

    return 0;
}
