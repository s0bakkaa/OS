cmake_minimum_required(VERSION 3.17)
project(lab4 C)

set(CMAKE_C_STANDARD 99)

set(THREADS_PREFER_PTHREAD_FLAG ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
find_package(Threads REQUIRED)

add_executable(child0 child0.c memwork.h)
add_executable(child1 child1.c memwork.h)
add_executable(lab4 main.c memwork.h)
target_link_libraries(lab4 PRIVATE Threads::Threads rt)
target_link_libraries(child0 PRIVATE Threads::Threads rt)
target_link_libraries(child1 PRIVATE Threads::Threads rt)

add_dependencies(lab4 child0)
add_dependencies(child0 child1)
