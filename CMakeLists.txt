cmake_minimum_required(VERSION 3.23)
project(llp_database)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -g -O3")
#set(ASAN_OPTIONS "${ASAN_OPTIONS} detect_leaks=1")

add_executable(llp_database main.cpp dbstruct/node.cpp dbstruct/node.h file_workers/file_utils.cpp file_workers/file_utils.h dbstruct/tree_header.cpp dbstruct/tree_header.h commands/commands.cpp commands/commands.h commands/schema_validation.h)
