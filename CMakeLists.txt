
cmake_minimum_required(VERSION 3.20)

project(ScientificCalculator
    VERSION 1.0.0
    DESCRIPTION "A modular C++20 scientific calculator"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CTest)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_FUZZER "Build the LibFuzzer expression parser target" OFF)

function(configure_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4
            /permissive-
        )
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wshadow
        )
    endif()
endfunction()

add_library(calculator_core
    src/core/engine.cpp
    src/core/history.cpp
    src/parser/expression_parser.cpp
)

target_include_directories(calculator_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

configure_warnings(calculator_core)

add_library(calculator_app
    src/app/calculator_app.cpp
)

target_include_directories(calculator_app
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(calculator_app
    PUBLIC calculator_core
)

configure_warnings(calculator_app)

add_executable(calculator
    src/main.cpp
)

target_link_libraries(calculator
    PRIVATE calculator_app
)

configure_warnings(calculator)

if(BUILD_TESTING)
    find_package(GTest REQUIRED)

    add_executable(calculator_tests
        tests/unit/engine_tests.cpp
        tests/unit/history_tests.cpp
        tests/unit/parser_tests.cpp
        tests/unit/app_tests.cpp
    )

    target_link_libraries(calculator_tests
        PRIVATE
            calculator_app
            GTest::gtest_main
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

    target_compile_options(expression_parser_fuzz
        PRIVATE
            -fsanitize=fuzzer,address,undefined
            -fno-omit-frame-pointer
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wshadow
    )

    target_link_options(expression_parser_fuzz
        PRIVATE
            -fsanitize=fuzzer,address,undefined
    )
endif()

# Install reusable libraries
install(TARGETS
    calculator_core
    calculator_app
    EXPORT ScientificCalculatorTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install command-line executable
install(TARGETS calculator
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install only actual public C++ headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
        PATTERN "*.hpp"
)

# Export reusable CMake targets
install(EXPORT ScientificCalculatorTargets
    FILE ScientificCalculatorTargets.cmake
    NAMESPACE ScientificCalculator::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ScientificCalculator
)

# Generate package configuration
configure_package_config_file(
    cmake/ScientificCalculatorConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ScientificCalculatorConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ScientificCalculator
)

# Generate package version configuration
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ScientificCalculatorConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install package configuration files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ScientificCalculatorConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ScientificCalculatorConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ScientificCalculator
)

# Browser-compatible WebAssembly build
if(EMSCRIPTEN)
    add_executable(calculator_wasm
        web/wasm/calculator_bindings.cpp
    )

    target_link_libraries(calculator_wasm
        PRIVATE calculator_core
    )

    configure_warnings(calculator_wasm)

    target_link_options(calculator_wasm
        PRIVATE
            --bind
            "-sMODULARIZE=1"
            "-sEXPORT_ES6=1"
            "-sALLOW_MEMORY_GROWTH=1"
            "-sENVIRONMENT=web"
    )

    set_target_properties(calculator_wasm PROPERTIES
        OUTPUT_NAME "calculator"
        SUFFIX ".js"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/web/wasm"
    )
endif()
