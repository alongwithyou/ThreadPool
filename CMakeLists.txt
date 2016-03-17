project(ThreadPool)
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)
add_definitions(-Wall -Wextra -pedantic)

add_library(ThreadPool SHARED src/ThreadPool.cc)
install(TARGETS ThreadPool LIBRARY DESTINATION lib)
install(FILES include/ThreadPool.hh DESTINATION include)

add_executable(test_ThreadPool test/test_ThreadPool.cc)
target_link_libraries(test_ThreadPool ThreadPool)
