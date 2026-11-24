cmake_minimum_required(VERSION 3.20)

project(ScientificCalculator
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

option(BUILD_FUZZER "Build the LibFuzzer expression parser target" OFF)

function(configure_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow
        )
    endif()
endfunction()

add_library(calculator_core
    src/core/engine.cpp
    src/core/history.cpp
    src/parser/expression_parser.cpp
)

target_include_directories(calculator_core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

configure_warnings(calculator_core)

add_library(calculator_app
    src/app/calculator_app.cpp
)

target_include_directories(calculator_app
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(calculator_app
    PUBLIC calculator_core
)

configure_warnings(calculator_app)

add_executable(calculator
    src/main.cpp
)

target_link_libraries(calculator PRIVATE calculator_app)

if(BUILD_TESTING)
    find_package(GTest REQUIRED)

    add_executable(calculator_tests
        tests/unit/engine_tests.cpp
        tests/unit/history_tests.cpp
        tests/unit/parser_tests.cpp
        tests/unit/app_tests.cpp
    )

    target_link_libraries(calculator_tests
        PRIVATE calculator_app GTest::gtest_main
    )

    configure_warnings(calculator_tests)

    include(GoogleTest)
    gtest_discover_tests(calculator_tests)
endif()

if(BUILD_FUZZER)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(FATAL_ERROR
            "BUILD_FUZZER requires Clang with LibFuzzer support."
        )
    endif()

    add_executable(expression_parser_fuzz
        tests/fuzz/expression_parser_fuzz.cpp
        src/parser/expression_parser.cpp
        src/core/engine.cpp
    )

    target_include_directories(expression_parser_fuzz
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_compile_options(expression_parser_fuzz PRIVATE
        -fsanitize=fuzzer,address,undefined
        -fno-omit-frame-pointer
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow
    )

    target_link_options(expression_parser_fuzz PRIVATE
        -fsanitize=fuzzer,address,undefined
    )
endif()
