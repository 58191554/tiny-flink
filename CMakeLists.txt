cmake_minimum_required(VERSION 3.16)
project(tiny_flink LANGUAGES CXX)

# TypeDescriptor uses std::remove_cvref_t (C++20).
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)

find_package(absl CONFIG REQUIRED)
add_library(logical_graph src/logical/logical_graph.cpp)
target_include_directories(logical_graph PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(logical_graph PUBLIC absl::status)

add_library(graph_builder src/logical/graph_builder.cpp)
target_link_libraries(graph_builder PUBLIC logical_graph absl::statusor)

if(BUILD_TESTING)
    include(FetchContent)
    if(POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
    endif()
    FetchContent_Declare(
        googletest
        # Pin the v1.18.0 release for reproducible builds.
        URL https://github.com/google/googletest/archive/063de7e9578f82b369302001269680b4b1553359.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(id_test src/core/id_test.cpp)
    target_include_directories(id_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(id_test PRIVATE GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(id_test TEST_PREFIX "core.id.")

    add_executable(type_descriptor_test src/core/type_descriptor_test.cpp)
    target_include_directories(type_descriptor_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(type_descriptor_test PRIVATE GTest::gtest_main)
    gtest_discover_tests(type_descriptor_test TEST_PREFIX "core.type_descriptor.")

    add_executable(record_test src/core/record_test.cpp)
    target_include_directories(record_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(record_test PRIVATE GTest::gtest_main)
    gtest_discover_tests(record_test TEST_PREFIX "core.record.")

    add_executable(stream_message_test src/core/stream_message_test.cpp)
    target_include_directories(stream_message_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(stream_message_test PRIVATE GTest::gtest_main)
    gtest_discover_tests(stream_message_test TEST_PREFIX "core.stream_message.")

    add_executable(logical_graph_test src/logical/logical_graph_test.cpp)
    target_link_libraries(logical_graph_test PRIVATE logical_graph GTest::gtest_main)
    gtest_discover_tests(logical_graph_test TEST_PREFIX "logical.graph.")

    add_executable(graph_builder_test src/logical/graph_builder_test.cpp)
    target_link_libraries(graph_builder_test PRIVATE graph_builder GTest::gtest_main)
    gtest_discover_tests(graph_builder_test TEST_PREFIX "logical.builder.")
endif()
