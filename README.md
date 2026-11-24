
# Scientific Calculator

A modular scientific calculator implemented in C++20. It provides an interactive command-line interface, mathematical expression parsing, calculation history, automated unit tests, and an optional LibFuzzer target.

## Features

- Arithmetic: addition, subtraction, multiplication, and division
- Unary operators and exponentiation
- Parenthesized expressions
- Implicit multiplication, such as `2pi`, `2(3+4)`, and `3sqrt(16)`
- Constants: `pi` and `e`
- Scientific functions:
  - `sqrt`
  - `sin`, `cos`, `tan`
  - `asin`, `acos`, `atan`
  - `ln`, `log`
  - `abs`, `exp`
- Trigonometric functions use degrees
- Calculation history and the `ans` previous-result variable
- Input and mathematical error handling
- GoogleTest unit tests
- Optional LibFuzzer expression-parser testing
- CMake installation and package-export support

## Technology

- C++20
- CMake 3.20 or newer
- Ninja
- GoogleTest
- Clang and LibFuzzer (optional)

## Project Structure

```text
ScientificCalculator/
├── .github/
│   └── workflows/
│       └── ci.yml
├── cmake/
│   └── ScientificCalculatorConfig.cmake.in
├── include/
│   └── calculator/
│       ├── app/
│       ├── core/
│       └── parser/
├── src/
│   ├── app/
│   ├── core/
│   ├── parser/
│   └── main.cpp
├── tests/
│   ├── fuzz/
│   │   ├── corpus/
│   │   └── expression_parser_fuzz.cpp
│   └── unit/
├── CMakeLists.txt
└── README.md
```

## Requirements

On Ubuntu or WSL Ubuntu:

```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build libgtest-dev
```

For optional fuzz testing:

```bash
sudo apt install -y clang
```

## Build

Configure a Debug build:

```bash
cmake -S . -B build-linux -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=ON
```

Compile:

```bash
cmake --build build-linux
```

Run the calculator:

```bash
./build-linux/calculator
```

## Run Tests

```bash
ctest --test-dir build-linux --output-on-failure
```

## Release Build

```bash
cmake -S . -B build-release -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON

cmake --build build-release

ctest --test-dir build-release --output-on-failure
```

## Fuzz Testing

The fuzz target requires Clang and LibFuzzer.

Configure and build:

```bash
CC=clang CXX=clang++ cmake -S . -B build-fuzz -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=OFF \
  -DBUILD_FUZZER=ON

cmake --build build-fuzz
```

Run a bounded fuzzing campaign using a temporary corpus copy:

```bash
mkdir -p /tmp/calculator-fuzz-corpus
cp -a tests/fuzz/corpus/. /tmp/calculator-fuzz-corpus/

./build-fuzz/expression_parser_fuzz \
  -max_len=4096 \
  -timeout=10 \
  -runs=100000 \
  /tmp/calculator-fuzz-corpus
```

Using a temporary corpus avoids adding generated fuzz inputs to the project's tracked corpus.

## Installation

Install to a local prefix:

```bash
cmake --install build-release --prefix "$HOME/.local"
```

Run the installed calculator:

```bash
"$HOME/.local/bin/calculator"
```

The installation includes the executable, static libraries, public headers, and CMake package configuration.

## Using the Installed CMake Package

A separate CMake project can locate the installed package with:

```cmake
find_package(ScientificCalculator CONFIG REQUIRED)

target_link_libraries(your_target
    PRIVATE ScientificCalculator::calculator_app
)
```

Exported targets:

- `ScientificCalculator::calculator_core`
- `ScientificCalculator::calculator_app`

Set `CMAKE_PREFIX_PATH` to the installation prefix if CMake does not discover the package automatically.

## Continuous Integration

The GitHub Actions workflow builds and tests Debug and Release configurations and runs an AddressSanitizer/UndefinedBehaviorSanitizer test configuration.
