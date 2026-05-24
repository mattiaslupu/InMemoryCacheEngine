cmake_minimum_required(VERSION 3.14)
project(InMemoryCacheEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(include)

add_executable(InMemoryCacheEngine
        src/main.cpp
        src/Statistics.cpp
        src/Benchmark.cpp
        src/BenchmarkResult.cpp
        src/Operation.cpp
        src/CacheManager.cpp
        src/Menu.cpp)

add_custom_target(copy_data ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        ${CMAKE_BINARY_DIR}/data
)

add_dependencies(InMemoryCacheEngine copy_data)
