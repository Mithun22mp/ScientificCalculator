
# Scientific Calculator

A scientific calculator inspired by the
Casio fx-991ES PLUS 2nd Edition.

## Technology

- C++20
- CMake
- Ninja
- GoogleTest
- HTML, CSS and JavaScript
- WebAssembly

## Project Status

Day 1 - Development environment setup

## Architecture

The project is organised into independent modules.

- `src/core`: Core calculator engine
- `src/parser`: Mathematical expression parsing
- `src/math`: Mathematical operations
- `src/modes`: Calculator operating modes
- `src/wasm`: WebAssembly integration
- `include/calculator`: Public C++ headers
- `tests/unit`: Unit tests
- `tests/integration`: Integration tests
- `web`: Browser interface
- `assets`: Static assets
- `docs`: Technical documentation

## Build

Configure:

    cmake -S . -B build -G Ninja

Compile:

    cmake --build build

Run:

    ./build/calculator.exe
