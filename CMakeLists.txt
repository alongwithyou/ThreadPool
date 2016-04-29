project(ThreadPool)
cmake_minimum_required(VERSION 3.1)

include_directories(include)

set(WARNING_FLAGS -Wall -Wextra -pedantic)

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

add_library(ThreadPool SHARED src/ThreadPool.cc)
target_compile_options(ThreadPool PUBLIC ${WARNING_FLAGS})
set_target_properties(ThreadPool PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED True)

install(TARGETS ThreadPool LIBRARY DESTINATION lib)
install(FILES include/ThreadPool.hh DESTINATION include)

add_executable(test_ThreadPool test/test_ParallelMap.cc test/catch_main.cc)
target_link_libraries(test_ThreadPool ThreadPool)
target_compile_options(test_ThreadPool PUBLIC ${WARNING_FLAGS})
set_target_properties(test_ThreadPool PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED True)
