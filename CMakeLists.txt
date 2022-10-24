cmake_minimum_required(VERSION 3.23)
project(llp_database)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -g -O1")
set(ASAN_OPTIONS "${ASAN_OPTIONS} detect_leaks=1")

add_executable(llp_database main.cpp)
