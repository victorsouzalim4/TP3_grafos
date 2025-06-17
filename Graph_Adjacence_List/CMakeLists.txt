cmake_minimum_required(VERSION 3.14)
project(Graph_Adjacence_List CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Inclui headers
include_directories(include)

# Lib principal
file(GLOB SRC_FILES src/*.cpp)
list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/Main.cpp)
add_library(graphlib ${SRC_FILES})

# GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Executável de testes
add_executable(tests
    tests/test_directed_graph.cpp
    tests/test_graph.cpp
    tests/test_undirected_graph.cpp
    #tests/test_graph_utils.cpp
)

# Linkagem
target_link_libraries(tests graphlib gtest_main)

# >>> IMPORTANTE: estes dois devem vir **depois**
enable_testing()
include(GoogleTest)
gtest_discover_tests(tests)
