#include "result_iterator.h"

bool check(int32_t fd, struct node node, const struct condition &cond) {
    if (node.parent != 0) {
        node.data["parent_id"] = std::to_string(read_node_header_from_db(fd, node.parent).id);
    } else if (cond.name_value == "parent_id") { // у ноды без родителя спрашивать про родителя не толерантно!
        return false;
    }
    node.data["id"] = std::to_string(node.id);

    switch (cond.operation) {
        case (NOP):
            return true;
        case (NOT_EQUAL):
            if (cond.value != node.data[cond.name_value]) {
                return true;
            }
            return false;
        case (EQUAL):
            if (cond.value == node.data[cond.name_value]) {
                return true;
            }
            return false;
    }

    return false;
}

std::unordered_map<std::string, std::string> result_iterator::operator*() {
    node.data["id"] = std::to_string(node.id);
    if (node.parent != 0) {
        node.data["parent_id"] = std::to_string(read_node_header_from_db(fd, node.parent).id);
    }

    return node.data;
}

bool result_iterator::proceed() {
    if (!is_valid) {
        return false;
    }

    do {
        if (node.first_child != 0) {
            node = read_node_from_db(fd, node.first_child);
        } else if (node.next != 0) {
            node = read_node_from_db(fd, node.next);
        } else {
            if (node.parent == get_tree_header_without_schema_from_db(fd).first_node) {
                is_valid = false;
                return false;
            }
            node = read_node_from_db(fd, node.parent);
            while (node.next == 0) {
                if (node.parent == get_tree_header_without_schema_from_db(fd).first_node) {
                    is_valid = false;
                    return false;
                }
                node = read_node_from_db(fd, node.parent);
            }
            if (node.next != 0) {
                node = read_node_from_db(fd, node.next);
            } else {
                is_valid = false;
                return false;
            }
        }
    } while (!check(fd, node, condition));

    return true;
}

// To debug
void result_iterator::print() {
    printf("--- Result Iterator | Condition: %s ---\n", this->condition.to_string().c_str());

    if (!this->is_valid) {
        printf("Iterator is not valid\n");
        printf("\n");
        return;
    }

    do {
        for (const auto &name_to_value: *(*this)) {
            printf("%s: %s, ", name_to_value.first.c_str(), name_to_value.second.c_str());
        }
        printf("\n");
    } while (this->proceed());
    printf("\n");
}