
cmake_minimum_required(VERSION 3.20)

project(ScientificCalculator
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

add_library(calculator_core
    src/core/engine.cpp
    src/parser/expression_parser.cpp
)

target_include_directories(calculator_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(calculator_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(calculator_core PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
    )
endif()

add_executable(calculator
    src/main.cpp
)

target_link_libraries(calculator
    PRIVATE calculator_core
)

if(BUILD_TESTING)
    find_package(GTest REQUIRED)

    add_executable(calculator_tests
        tests/unit/engine_tests.cpp
        tests/unit/parser_tests.cpp
    )

    target_link_libraries(calculator_tests
        PRIVATE
            calculator_core
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(calculator_tests)
endif()
