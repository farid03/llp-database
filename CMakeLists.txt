cmake_minimum_required(VERSION 3.23)
project(llp_database)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -g")
#set(ASAN_OPTIONS "${ASAN_OPTIONS} detect_leaks=1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
find_package(Boost 1.74.0 REQUIRED COMPONENTS system filesystem serialization)
include_directories(
        include
        src
        ${Boost_INCLUDE_DIR}
)
link_directories(${Boost_LIBRARY_DIR})

add_executable(llp_database main.cpp
        dbstruct/node.cpp dbstruct/node.h
        file_workers/file_utils.cpp file_workers/file_utils.h
        dbstruct/tree_header.cpp dbstruct/tree_header.h
        commands/commands.cpp commands/commands.h
        data_process_utils/validation/schema_validation.h data_process_utils/validation/schema_validation.cpp
        data_process_utils/serialization/serialization.h data_process_utils/serialization/serialization.cpp
        commands/iterator/result_iterator.cpp commands/iterator/result_iterator.h)
target_link_libraries(llp_database PRIVATE ${Boost_LIBRARIES})
