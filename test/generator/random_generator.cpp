#include "random_generator.h"
#include <cinttypes>
#include <string>

char symbols[36] = {
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 'a', 's', 'd', 'f', 'g',
        'h', 'j', 'k', 'l', 'z', 'x', 'c', 'v', 'b', 'n', 'm', '1', '2', '3', '4',
        '5', '6', '7', '8', '9', '0'};

std::string strings_dict[50] = {
        "Vestibulum", "malesuada", "amet", "morbi", "aenean", "luctus", "amet", "dapibus", "velit", "molestie",
        "faucibus", "dui", "malesuada", "augue", "accumsan", "lorem", "mattis", "ultricies", "Sit", "mattis", "quis",
        "odio", "Molestieet", "eleifend", "nunc", "justo", "ornare", "quis", "lorem", "interdum", "augue", "amet",
        "velit", "tempus", "mattis", "nulla", "molestie", "non", "vulputate", "lacinia", "in", "ultricies", "Ornare",
        "nunc", "eleifend", "non", "sit", "faucibus", "Nulla", "quamquam"
};

int32_t get_random_int(int32_t from, int32_t to) {
    return from + rand() % (to - from + 1);
}

bool get_random_bool() {
    return get_random_int(0, 1) == 1;
}

float get_random_float(float from, float to) {
    return (float) rand() / (float) RAND_MAX * (to - from) + from;
}

std::string get_random_string_from_array() {
    return strings_dict[get_random_int(0, 50)];
}

std::string get_random_string(int32_t len) {
    auto res = std::string(len, 'a');
    for (int i = 0; i < len; ++i) {
        res[i] = symbols[get_random_int(0, 35)];
    }
    return res;
}


std::unordered_map<std::string, std::string> generate_random_node_by_schema(
        const std::unordered_map<std::string, data_type> &schema,
        int32_t min, int32_t max, int32_t str_len
) {
    std::unordered_map<std::string, std::string> node{};
    for (const auto &name_to_type: schema) {
        switch (name_to_type.second) {
            case BOOL:
                node[name_to_type.first] = get_random_bool() ? "true" : "false";
                break;
            case INT:
                node[name_to_type.first] = std::to_string(get_random_int(min, max));
                break;
            case FLOAT:
                node[name_to_type.first] = std::to_string(get_random_float(min, max));
                break;
            case STRING:
                node[name_to_type.first] = get_random_string_from_array();
                break;
        }
    }

    return node;
}
