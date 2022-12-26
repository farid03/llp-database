cmake_minimum_required(VERSION 3.23)
project(llp_database)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -g")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
#set(ASAN_OPTIONS "${ASAN_OPTIONS} detect_leaks=1")
set(Boost_INCLUDE_DIR /home/lowlimb/Downloads/boost_1_70_0)
set(Boost_LIBRARY_DIR /home/lowlimb/Downloads/boost_1_70_0/stage/lib)
find_package(Boost 1.74.0 COMPONENTS system filesystem serialization REQUIRED)
include_directories(
        include
        src
        ${Boost_INCLUDE_DIR}
)
link_directories(${Boost_LIBRARY_DIR})

add_executable(llp_database main.cpp dbstruct/node.cpp dbstruct/node.h file_workers/file_utils.cpp file_workers/file_utils.h dbstruct/tree_header.cpp dbstruct/tree_header.h commands/commands.cpp commands/commands.h commands/schema_validation.h dbstruct/serialization.h)
target_link_libraries(llp_database PRIVATE ${Boost_LIBRARIES})
