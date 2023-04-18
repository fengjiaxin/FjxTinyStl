cmake_minimum_required(VERSION 3.19)
project(FjxTinyStl)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/MyTinySTL)
add_executable(list-test test/list-test.cpp)
add_executable(vector-test test/vector-test.cpp)
add_executable(deque-test test/deque-test.cpp)
add_executable(hashtable-test test/hashtable-test.cpp)
add_executable(hashset-test test/hashset-test.cpp)
add_executable(hashmap-test test/hashmap-test.cpp)
add_executable(stack-test test/stack-test.cpp)
add_executable(queue-test test/queue-test.cpp)